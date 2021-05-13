#include <stdarg.h>          //< va_list, va_start, va_arg
#include <string.h>          //< memcpy, memset, strlen
#include <stdlib.h>          //< Used for srand

// TOOLS SUPPORT LIBRARY (could be useful across many applications)
#include <utility.h>
#include <divmod8_table.h>    //< Used in monitoring blocker tables
#include <divmod4_table.h>    //< Used in monitoring animation bits

// PROJECT SPECIFIC LIBRARY (things generally only applicable to this particular program)
#include <destiny_structs.h>
#include <game_strings.h>
#ifndef QUICK_GAME
  #include <01_destiny.h>
  #include <03_intro.h>
#endif
#include <02_init_persona.h>
#include <stage_rle_values.inc.h>
#include <stage_blocker_values.inc.h>

// SYSTEM INCLUDES COME AFTER THE ABOVE - since core.h is used to establish the target configuration
#ifdef TARGET_C64
  #include <conio.h>	
	unsigned char j_fire_prepare = FALSE;
#endif

static unsigned char rest_mode = FALSE;
static unsigned char (*ptr_blockers)[5];  // of the current map
static unsigned char or_equal_modifier[] = {0xC0,0x30,0x0C,0x03};
static unsigned char weapon_fire_symbols[] = {66, 78, 67, 77, 66, 78, 67, 77};
static unsigned char check_for_flipskill_learned;
static unsigned char stage_event_state;		
static unsigned char which_stats_modified;  //< Flag used to indicate which player-stats/states/status have been modified (only update/redraw those modified states)

#define MAX_LOCATIONS_TO_DRAW 34  // each challenge is up to 3x5, plus weapon and player (so this size allows for 2 large icons (2x15 = 30) or 10 small icons (10x3 = 30)

// WEAPON STATES
#define WS_IDLE          0
#define WS_FIRING        1
#define WS_HIT_GROUND    2
#define WS_HIT_CHALLENGE 3

#define ADJ_NONE   0x00
#define ADJ_LEFT   0x01
#define ADJ_RIGHT  0x02
#define ADJ_UP     0x04
#define ADJ_DOWN   0x08

#define FIREBALL_SYMBOL 42
#define FIREBALL_SYMBOL_REVERSE 170

#define MAX_LOITER_DURATION 14

#define MINIMUM_STAMINA_TO_ACTION 100

#define MAX_MAP_ROWS 23  // 0 to 22, HEIGHT_OF_SCREEN-2
static unsigned char cell_state[MAX_MAP_ROWS][10];  // Y/X, 10*(4 half-nibbles per cell) = 40 columns

#ifdef TARGET_C64
  static unsigned char g_pvec_map_color[MAX_MAP_ROWS][40];  // HEIGHT_OF_SCREEN-2 and WIDTH_OF_SCREEN
#endif
static char g_pvec_map[MAX_MAP_ROWS][40];  // HEIGHT_OF_SCREEN-2 and WIDTH_OF_SCREEN
/*
  g_pvec_map[0] and g_pvec_map[1] are "unused" by the map as projected onto the display.  This is to avoid constantly adjusting 
	for a "virtual screen coordinate" (adding or subtracting 2), but waste 80 bytes of RAM.  Internally this program takes advantage
	of this by using these first two map rows as "scratch space" for certain information.  A summary of that usage is to be
	maintained here:
	
	g_pvec_map[0][0] = used to store the player health BEFORE entering stage 4 (the flashback stage), so that health can be restored if they are defeated during stage 4
	g_pvec_map[0][1] = used to store how many challenges_remaining is necessary to FINISH a map (typically 0, stage 8 needs 4)
	g_pvec_map[1][0-4] = overlayed with a 4-byte Time_counter struct, that corresponds to what JIFFY (time) the stages were started (to keep track of the play-time duration at the end of the game)	
*/	

static unsigned char weapon_carried_symbols[] = {113, 73, 107, 75, 114, 74, 115, 85};

// STAGE4 custom icon animations
static unsigned char wing_symbolsLEFT[] = {85, 45, 74, 45};
static unsigned char wing_symbolsRIGHT[] = {73, 45, 75, 45};

static unsigned char tail_symbolsLEFT[] = {73, 45, 75, 45};
static unsigned char tail_symbolsRIGHT[] = {74, 45, 85, 45};

static unsigned char feet_symbolsLEFT[] = {74, 75};  //108, 123, 126, 124};
//static unsigned char feet_symbolsRIGHT[] = {85, 73, 75, 74};											

static unsigned char scorp_symbolsTOP[2]     = { 95, 105};
static unsigned char scorp_symbolsBOTTOM[2]  = {233, 223};

static unsigned char scorpR_symbolsTOP[2]    = {105,  95};
static unsigned char scorpR_symbolsBOTTOM[2] = {223, 233};

// These constants are used to initialize the starting position of the player per each stage.
// MIN_X/Y is the minium x/y offset distance, MOD_X/Y is a 0 to N-1 random offset.
//                                STAGE	  1   2   3   4   5   6   7   8
static unsigned char stage_mod_x[] = {0, 12,  4,  2,  3,  3,  3,  2,  2};
static unsigned char stage_min_x[] = {0,  3,  2,  2, 18,  7,  3,  2,  3};
static unsigned char stage_mod_y[] = {0, 10, 11,  8,  4,  4,  6,  8, 12};
static unsigned char stage_min_y[] = {0,  4,  6,  5, 10,  5,  6, 10,  6};   

/*
Retained for historical reference:

static char* stage_names[9] = {
	"matins  ",  //< placeholder name, not used (stages are indexed 1-8       (was)
	"rats    ",  // RATS                                                      WOLVES              1
	"komodo  ",  // DRAKE1     Sphynx                                         BOAR                2
	"crocs   ",  // GATORS                                                    GATORS              3
	"strixes ",  // VULTURES                                                  OWL or CROW         4
	"drakes  ",  // DRAKE2     Drake                                          BEAR                5
	"sluaghs ",  // KOBOLS                                                    SKELETONS           6
	"drakes  ",  // DRAKE3     Wyvnern  (poison, large serpent - no feet?)    LIONS               7
	"hydra   ",  // DRAGON                                                    DRAGON              8
};
*/
static char stage_names[][] = {
	{ 13,   1,  20,   9,  14,  19, 160, 160},  // not-used 0
	{146, 129, 148, 147, 160, 160, 160, 160},  // rats     1 
	{139, 143, 141, 143, 132, 143, 160, 160},  // komodo   2
	{131, 146, 143, 131, 147, 160, 160, 160},  // crocs    3  
	{147, 148, 146, 154, 153, 135, 129, 160},  // strzyga  4
	{147, 131, 143, 146, 144, 137, 143, 142},  // scorpion 5
	{147, 140, 149, 129, 135, 136, 147, 160},  // sluaghs  6
	{132, 146, 129, 139, 133, 147, 160, 160},  // drakes   7
	{136, 153, 132, 146, 129, 160, 160, 160},  // hydra    8
};	

// v-- These specify the random+offset x/y initial location of the "free-arrows" per stage
static unsigned char arrow_locations_x_mod[] = { 0, 13,10, 3, 6, 6,10,10,10};
static unsigned char arrow_locations_x_ofs[] = { 0, 12, 3,19,17, 5, 3, 3, 3};
static unsigned char arrow_locations_y_mod[] = { 0,  5,12, 3, 6,16,12,12,12};
static unsigned char arrow_locations_y_ofs[] = { 0, 10, 3,11, 9, 4, 5, 5, 5};	
																						 //  0   1  2  3  4  5  6  7  8   STAGE

//static const char step_symbol[4] = {126, 124, 108, 123};  // 190 188  172  187

													 //  YONI  LANGI
//static char vertical_barL[] = {  72, 93  };
//static char curve_upLEFT[]  = {  75, 113 };  //< Used to be for NAME corner on the TOP LEFT

static char vertical_barR[] = {  66, 93  };
static char curve_upRIGHT[] = {  74, 113 };

static signed char x_delta;
static signed char y_delta;
static signed char x2_delta;
static signed char y2_delta;
static unsigned char x_threshold;
static unsigned char y_threshold;

static Location_to_draw locations_to_draw[MAX_LOCATIONS_TO_DRAW];
static unsigned char num_locations_to_draw = 0;	

typedef void (*ptr_animate_icon_func_type)(Challenge *);

unsigned char IS_BLOCKED(unsigned char check_x, unsigned char check_y)  
{  
  if (ptr_blockers == 0)
		return FALSE;
	return (IS_BIT_ON(ptr_blockers[check_y-2][div8_table[check_x]], mod8_table[check_x]) == TRUE);
}

void draw_stage_overlay(char* stage_name)
{	
	// TOP
/*
	          1         2         3         4
	01234567890123456789012345678901234567890
1	NAMEaaaaaaa|                   |STAGNAME
2 -----------+--=----------------+--------
3								  
4								  
*/	

	for (g_i = 0; g_i < 39; ++g_i)  // WIDTH_OF_SCREEN-1
	{
		WRITE_CHAR(g_i, 1, 64);  // bar
		WRITE_CHAR(g_i,23, 64);  // bar
	}
			
	WRITE_CHAR(30, 0, vertical_barR[global_destiny_status.direction]);
	WRITE_STRING(31, 0, stage_name, STR_STAGENAME_LEN);	
	WRITE_CHAR(30, 1, curve_upRIGHT[global_destiny_status.direction]);				
	
	// BOTTOM
	WRITE_STRING(0, 24, str_stage_instruction, STR_STAGE_INSTRUCTION_LEN);

	WRITE_CHAR(14, 24, 30);  // UP-ARROW
	// two-digit value
	WRITE_CHAR(17, 24, 78);  // SLASH
	WRITE_PU_DIGIT(18, 24, g_ptr_persona_status->arrows_max, 2);
	
	WRITE_CHAR(22, 24, 83);  // HEART
	// 1-digit value
	WRITE_CHAR(24, 24, 78);  // SLASH
	WRITE_1U_DIGIT(25, 24, g_ptr_persona_status->hp_max);
		
/*
22
23
24  -----------------------------------------
25	BLESS [__]    ^[___|_____]v  PERSIST [__]
    1234567890123456789012345678901234567890
	NAME will become PERSONA when playing those stages	
	
	*/
	
  // reverse the persona name
	y_delta = strlen(g_ptr_persona_status->name);
	for (g_i = 0; g_i < y_delta; ++g_i)
	{
		SET_MASK(g_ptr_persona_status->name[g_i], MASK_HIGH_BIT);
	}
	x_delta = ((39 - y_delta) / 2)-1;   // CENTER the name
	WRITE_CHAR(x_delta, 0, 160);  // flank the name with an extra space
	++x_delta;
	WRITE_STRING(x_delta, 0, g_ptr_persona_status->name, y_delta);
	x_delta += y_delta;
	WRITE_CHAR(x_delta, 0, 160);
}

#define STATS_NONE        0x00
#define STATS_HP          0x01  // 0000 0001
#define STATS_ARROWS      0x02  // 0000 0010
#define STATS_BLESS       0x04  // 0000 0100
#define STATS_PERSISTENCY 0x08  // 0000 1000
#define STATS_STEPS       0x10  // 0001 0000
#define STATS_STAMINA     0x20  // 0010 0000
#define STATS_INVENTORY   0x40  // 0100 0000
#define STATS_ALL         0xFF

void update_stats(unsigned char which_ones)
{		
  if (IS_MASK_ON(which_ones, STATS_HP))
	{
		WRITE_1U_DIGIT(23, 24, global_destiny_status.hp_current);
	}
	
	if (IS_MASK_ON(which_ones, STATS_ARROWS))
	{
		if (global_destiny_status.arrows_current > g_ptr_persona_status->arrows_max)
		{
			global_destiny_status.arrows_current = g_ptr_persona_status->arrows_max;
		}
		// -----------------------------------------------
				
		WRITE_PU_DIGIT(15, 24, global_destiny_status.arrows_current, 2);
	}
	
	if (IS_MASK_ON(which_ones, STATS_BLESS))
	{		
		// Just in case...
		if (global_destiny_status.blessing_count > MAX_BLESSING_COUNT)  // shouldn't be possible...
		{
			global_destiny_status.blessing_count = MAX_BLESSING_COUNT;
		}
		// ----------------------------------
		
		WRITE_1U_DIGIT(2, 24, global_destiny_status.blessing_count);
	}
	
	if (IS_MASK_ON(which_ones, STATS_PERSISTENCY))
	{
		if (global_destiny_status.persistency_count > MAX_PERSISTENCY_COUNT)  // if each CHIME gets 9x agains.... it's unlikely but just in case
		{
			global_destiny_status.persistency_count = MAX_PERSISTENCY_COUNT;
		}
		// ----------------------------------
		
		WRITE_PU_DIGIT(36, 24, global_destiny_status.persistency_count, 2);
	}
	
	/*
	if (IS_MASK_ON(which_ones, STATS_STEPS))
	{
		WRITE_PU_DIGIT(25, 0, global_destiny_status.steps_performed, 4);
				
		WRITE_CHAR(29, 0, step_symbol[(global_destiny_status.steps_performed % 4)]);
	}
	*/
	
	if (IS_MASK_ON(which_ones, STATS_INVENTORY))
	{
		/*
		
		  ) G B W O
		
		*/
		WRITE_STRING(2, 0, "          ", 10);  // just clear them all off the screen, less code then having an ELSE and clearing each one individually
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOW))
		{
			WRITE_CHAR(2, 0, SYMBOL_INV_BOW);
		}
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_GEM))
		{
			WRITE_CHAR(4, 0, SYMBOL_INV_GEM);
		}
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOOK))
		{
			WRITE_CHAR(6, 0, SYMBOL_INV_BOOK);
		}
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_FLIPSKILL))
		{
			WRITE_CHAR(8, 0, SYMBOL_INV_FLIP);
		}
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_ORB))
		{
			WRITE_CHAR(10, 0, SYMBOL_INV_ORB1);
		}
		else if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_ORB2))
		{
			WRITE_CHAR(10, 0, SYMBOL_INV_ORB2);
		}
	}
	
	if (
	  (IS_MASK_ON(which_ones, STATS_ARROWS))       // firing an arrow reduced stamina
		|| (IS_MASK_ON(which_ones, STATS_STEPS))     // moving reduced stamina
		|| (IS_MASK_ON(which_ones, STATS_STAMINA))   // some stamina was regained
		|| (rest_mode == TRUE)                       // explicit rest mode was declared
	)
	{
		if (rest_mode == TRUE)
		{
			WRITE_CHAR( 9, 24, 146);  // R
			WRITE_CHAR(10, 24, 133);  // E
			WRITE_CHAR(11, 24, 147);  // S
			WRITE_CHAR(12, 24, 148);  // T
			
			if (global_destiny_status.energy_current > MINIMUM_STAMINA_TO_ACTION)
			{
				rest_mode = FALSE;
			}
		}
		else
		{
			if (global_destiny_status.energy_current > 750)
			{
				WRITE_CHAR(12, 24, 247);  // high				
			}
			else
			{
				WRITE_CHAR(12, 24, 32);  // clear				
			}
			if (global_destiny_status.energy_current > 500)
			{
				WRITE_CHAR(11, 24, 248);  // medium				
			}
			else
			{
				WRITE_CHAR(11, 24, 32);  // clear				
			}
			if (global_destiny_status.energy_current > 250)
			{
				WRITE_CHAR(10, 24, 98);  // low				
			}
			else
			{
				WRITE_CHAR(10, 24, 32);  // clear
			}
			WRITE_CHAR(9, 24, 121);  // very low			
		}	
  }
}

void draw_stage_map()
{
	char* ptr_value;
	//unsigned char ch;
	register unsigned char y;
	register unsigned char x;	
#ifdef TARGET_C64
	bordercolor(C64_COLOR_BLACK);
#endif	
	
	// ROW 0 and ROW 1 of the map not used, as those portions are reserved as part of screen-overlay
	for (y = 2; y < 23; ++y)  // HEIGHT_OF_SCREEN-2
	{
		ptr_value = g_pvec_map[y];
		
		for (x = 0; x < 39; ++x)  // WIDTH_OF_SCREEN-1
		{			
			//ch = ptr_value[x];
#ifdef TARGET_C64
			POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*y)+x, g_pvec_map_color[y][x]);
			WRITE_CHAR(x, y, ptr_value[x]);			
#else						
		  WRITE_CHAR(x, y, ptr_value[x]);			
#endif		
		}
	}
	
#ifdef TARGET_C64
	textcolor(C64_COLOR_WHITE);
#endif		
}      

// STAGE STATES
#define STAGE_NOT_STARTED 0
#define STAGE_STARTING    1
#define STAGE_STARTED     2
#define STAGE_COMPLETED   3
#define STAGE_DEATH       4

#define MARK_LOCATION_AS_DRAWN(data_x,data_y) \
		/*ptr_pvec_value0[data_y] = TRUE;*/ \
		cell_state[data_y][div4_table[data_x]] |= or_equal_modifier[mod4_table[data_x]];

void BUFFER_LOCATION_TO_DRAW(unsigned char data_x, unsigned char data_y, unsigned char target_symbol
#ifdef TARGET_C64
, unsigned char a_color
#endif
)
{
	MARK_LOCATION_AS_DRAWN(data_x,data_y);  
	locations_to_draw[num_locations_to_draw].symbol = target_symbol;  
	locations_to_draw[num_locations_to_draw].offset = BASE_SCREEN_ADDRESS+(WIDTH_OF_SCREEN*data_y)+data_x;  
#ifdef TARGET_C64
  locations_to_draw[num_locations_to_draw].color = a_color;
#endif	
	++num_locations_to_draw;
}

void ORIENT_AND_QUEUE_DRAW_WEAPON()
{	
/*if (valid_key == TRUE) .. not sure why this isn't working...*/ 
	switch (global_destiny_status.curr_arrow_direction) 
	{ 
	case DIR_NN: global_destiny_status.weapon_y = global_destiny_status.location_y - 1; global_destiny_status.weapon_x = global_destiny_status.location_x;     break; /* 64 192 */ 
	case DIR_NE: global_destiny_status.weapon_y = global_destiny_status.location_y - 1; global_destiny_status.weapon_x = global_destiny_status.location_x + 1; break; /* 73 201 */ 
	case DIR_EE: global_destiny_status.weapon_y = global_destiny_status.location_y;     global_destiny_status.weapon_x = global_destiny_status.location_x + 1; break; /* 93 221 */ 
	case DIR_SE: global_destiny_status.weapon_y = global_destiny_status.location_y + 1; global_destiny_status.weapon_x = global_destiny_status.location_x + 1; break; /* 75 203 */ 
	case DIR_SS: global_destiny_status.weapon_y = global_destiny_status.location_y + 1; global_destiny_status.weapon_x = global_destiny_status.location_x;     break; /* 64 192 */ 
	case DIR_SW: global_destiny_status.weapon_y = global_destiny_status.location_y + 1; global_destiny_status.weapon_x = global_destiny_status.location_x - 1; break; /* 74 202 */ 
	case DIR_WW: global_destiny_status.weapon_y = global_destiny_status.location_y;     global_destiny_status.weapon_x = global_destiny_status.location_x - 1; break; /* 93 221 */ 
	case DIR_NW: global_destiny_status.weapon_y = global_destiny_status.location_y - 1; global_destiny_status.weapon_x = global_destiny_status.location_x - 1; break; /* 85 213 */ 
	} 
	
				
#ifdef TARGET_C64
	// This ends up looking wrong color on the C64, so looking worse.
#else
	// Invert the weapon when it is on BEACH tiles, just makes it look nicer...
	if (g_pvec_map[global_destiny_status.weapon_y][global_destiny_status.weapon_x] == MAP_BEACH) 
		SET_MASK(global_destiny_status.symbol_weapon, MASK_HIGH_BIT); 
	else 
		CLEAR_MASK(global_destiny_status.symbol_weapon, MASK_HIGH_BIT); 
#endif

	BUFFER_LOCATION_TO_DRAW(
		global_destiny_status.weapon_x,
		global_destiny_status.weapon_y,
		global_destiny_status.symbol_weapon
#ifdef TARGET_C64			
		,C64_COLOR_BROWN
#endif
	);
}

#define EVAL_MOVE_COST(data_x,data_y,allow_target) \
				  map_piece = g_pvec_map[data_y][data_x]; \
				  if ((map_piece == MAP_WATER) || (map_piece == (MAP_WATER | MASK_HIGH_BIT)))  /* mask high bit is the "reverse" version of the water tile */ \
					{     \
						global_destiny_status.energy_current -= (90 * g_ptr_persona_status->water_movement);  \
					}  \
					else  \
					{    \
						global_destiny_status.energy_current -= (60 * g_ptr_persona_status->land_movement);  \
					}					  \
					if (global_destiny_status.energy_current > 0) goto allow_target;  \
					rest_mode = TRUE;

void set_icon_char(Challenge* challenge, unsigned char index, unsigned char icon_index, int num, ...)
{
	va_list valist;
	va_start(valist, num);		
	
	challenge->longest_icon_height = index+1;
	
	if (num > challenge->longest_icon_width)
	{
		challenge->longest_icon_width = num;
	}
	
	for (g_i = 0; g_i < num; ++g_i)  // can't use while(num > 0) here, since va_args read in order (can't skip)
	{
		challenge->icon[icon_index][index][g_i] = va_arg(valist, unsigned char);		
	}
	
	va_end(valist);			
}

void initialize_challengeI(Challenge* ptr_challenge, unsigned char num, ...)
{
	va_list valist;
	va_start(valist, num);		
	
	ptr_challenge->x = va_arg(valist, signed char);
	ptr_challenge->y = va_arg(valist, signed char);
	
	ptr_challenge->hp_max = va_arg(valist, unsigned char);
	ptr_challenge->hp_remaining = ptr_challenge->hp_max;
	
	ptr_challenge->move_speed = va_arg(valist, unsigned long);
	
	ptr_challenge->aggressiveness_threshold = va_arg(valist, unsigned char);
	
	ptr_challenge->attack_speed = va_arg(valist, unsigned char);
	
	INIT_TIMER(ptr_challenge->last_move_timer);	
	INIT_TIMER(ptr_challenge->last_attack_time);	
	
	ptr_challenge->behavior = BEHAVIOR_GOING;
	ptr_challenge->targetN = 0;
	ptr_challenge->target_current = 0;	
	ptr_challenge->on_the_board = FALSE;  //< Assume initially all challenges are somewhere off the board
		
	ptr_challenge->longest_icon_width = 1;
	ptr_challenge->longest_icon_height = 1;
	
	++challenges_count;

  va_end(valist);	
}
                              
void initialize_challengeT(Challenge* ptr_challenge, unsigned char num, ...)
{
	unsigned char index;
	
	va_list valist;
	va_start(valist, num);	
	
  index = va_arg(valist, unsigned char);
	
  //ptr_challenge->targets[index].orig_x = va_arg(valist, signed char);
	//ptr_challenge->targets[index].orig_y = va_arg(valist, signed char);	
	//ptr_challenge->targets[index].target_x = ptr_challenge->targets[index].orig_x;	
	//ptr_challenge->targets[index].target_y = ptr_challenge->targets[index].orig_y;
	
	ptr_challenge->targets[index].target_x = va_arg(valist, signed char);
	ptr_challenge->targets[index].target_y = va_arg(valist, signed char);	
	
	ptr_challenge->targets[index].loiter_max = va_arg(valist, unsigned char); 
	ptr_challenge->targets[index].loiter_current = ptr_challenge->targets[index].loiter_max;
	
	ptr_challenge->direction_state = CD_LEFT;  // default all challeges initially facing/going left	
	
	ptr_challenge->targetN = (index+1);
	
	va_end(valist);	
}

void initialize_stage1_challenges()
{	
	memset(challenges[0].icon, 0, sizeof(challenges[0].icon));	

	challenges[0].max_direction_state = 1;
	challenges[0].hit_box_x = 0;
	challenges[0].hit_box_y = 0;
	challenges[0].icon[0][0][0] = 94;  // PI_SYMBOL for RAT
	
	memcpy(&challenges[1], &challenges[0], sizeof(Challenge));

  // JACK the younger
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[0], 5,     43, 12,  4,  8UL,  80,  JIFFIES_TWELTH_SECOND);	
	                                //                  
	                                //   num  N   x   y loiter
	initialize_challengeT(&challenges[0], 4,  0, 30, 10, 3);
	initialize_challengeT(&challenges[0], 4,  1, 25,  9, 2);
	initialize_challengeT(&challenges[0], 4,  2, 17, 12, 3);		
	initialize_challengeT(&challenges[0], 4,  3, 20,  2, 4);		

	
	// JONAS
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[1], 5,     44, 18,  3, 20UL,  40,  JIFFIES_TWELTH_SECOND);
	                                //                  
	                                //   num  N   x   y  loiter
	initialize_challengeT(&challenges[1], 4,  0, 32, 16, 5);
	initialize_challengeT(&challenges[1], 4,  1, 20, 20, 3);
	initialize_challengeT(&challenges[1], 4,  2, 14, 14, 6);		
}

void initialize_stage2_challenges()
{
	//memset(challenges[0].icon, 0, sizeof(challenges[0].icon));
	
	challenges[0].max_direction_state = 2;
	challenges[0].hit_box_x = 3;
	challenges[0].hit_box_y = 0;
	
  // JERRY the komodo
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[0], 5,     42, 10,  6,  1UL,  90, JIFFIES_EIGTH_SECOND);	
	                                //                  
	                                //   num  N   x   y  loiter
	initialize_challengeT(&challenges[0], 4,  0, 22,  5, 5);
	initialize_challengeT(&challenges[0], 4,  1, 18, 10, 5);
	initialize_challengeT(&challenges[0], 4,  2, 15, 20, 5);		
	initialize_challengeT(&challenges[0], 4,  3, 37, 12, 7);
	
	set_icon_char(&challenges[0], 0, CD_LEFT, 5,  233,  172, 227, 98, 121);	
	set_icon_char(&challenges[0], 1, CD_LEFT, 3,    0,  108, 108 );

	set_icon_char(&challenges[0], 0, CD_RIGHT,  5,  121, 98,  227,  172,  223);	
	set_icon_char(&challenges[0], 1, CD_RIGHT,  4,    0,  0,  123,  123);	
	
}

void initialize_stage3_challenges(unsigned char start, unsigned char count)
{
	unsigned char rand_x;
	unsigned char rand_y;
	unsigned long rand_mov;	
	
	while (count > 0)
	{		
    rand_x = rand_mod(19);
		rand_y = rand_mod(22);
		rand_mov = rand_mod(8);

                                  //                                         HP    mov  
                                  //           num         X        Y       max    spd    aggr   attk_speed
    switch (rand_mod(2))
		{
		case 0:  // X- Y-
		  initialize_challengeI(&challenges[start], 5,     rand_x+9,  0-rand_y,   3,  rand_mov, 70,  JIFFIES_HALF_SECOND);	 
			break;
			
		case 1:  // X+ Y-
		  initialize_challengeI(&challenges[start], 5,     rand_x+9,  22+rand_y,  3,  rand_mov, 60,  JIFFIES_HALF_SECOND);	 
			break;			
		}		
																		//       num  N      X                  Y            loiter
		initialize_challengeT(&challenges[start], 4,  0, rand_mod(19)+9,  rand_mod(20)+2, rand_mod(3)+2);
		initialize_challengeT(&challenges[start], 4,  1, rand_mod(19)+9,  rand_mod(20)+2, rand_mod(2)+3);
		initialize_challengeT(&challenges[start], 4,  2, rand_mod(19)+9,  rand_mod(20)+2, rand_mod(4)+1);		

		memset(challenges[start].icon, 0, sizeof(challenges[0].icon));
		
		challenges[start].max_direction_state = 2;
		challenges[start].hit_box_x = 2;
		challenges[start].hit_box_y = 0;			
		
		set_icon_char(&challenges[start], 0, CD_LEFT,   3,  60, 61, 75);
		
		set_icon_char(&challenges[start], 0, CD_RIGHT,  3,  74, 61, 62);
						
		++start;
		--count;
	}	
	
}

void initialize_stage4_challenges(unsigned char start, unsigned char count)
{	  
	unsigned char rand_x;
	unsigned char rand_y;
	unsigned long rand_mov;	
	unsigned char orig_start = start;
	  
	while (count > 0)
	{		
    rand_x = rand_mod(40);
		rand_y = rand_mod(22);		
		rand_mov = rand_mod(4)+3;
		
                                  //                                         HP    mov  
                                  //           num         X        Y       max    spd    aggr   attk_speed
    switch (rand_mod(4))
		{
		case 0:  // X- Y-   TOP 
		  // The -4 accounts for TOP RIGHT corner blockers when this spawn is used for STAGE3
		  initialize_challengeI(&challenges[start], 5,     rand_x-4,  0-rand_y,   2,  rand_mov, 60,   JIFFIES_HALF_SECOND);	 			
			break;
			
		case 1:  // X+ Y-   RIGHT      
		  initialize_challengeI(&challenges[start], 5,     38+rand_x,  rand_y,  2,  rand_mov, 40,   JIFFIES_HALF_SECOND);	 
			break;
			
		case 2:  // X+ Y+   BOTTOM
		  // The +12 accounts for BOTTOM LEFT corner blockers when this spawn is used for STAGE3
		  initialize_challengeI(&challenges[start], 5,     rand_x+12,  22+rand_y,  2,  rand_mov, 40,   JIFFIES_HALF_SECOND);	 
			break;
			
		case 3:  // X- Y+   LEFT		  
		  initialize_challengeI(&challenges[start], 5,     0-rand_x,  rand_y,   2,  rand_mov, 60,   JIFFIES_HALF_SECOND);	 
			break;
		}

																		//       num  N      X               Y            loiter
		initialize_challengeT(&challenges[start], 4,  0, rand_mod(38),  rand_mod(20)+2, rand_mod(3)+2);
		initialize_challengeT(&challenges[start], 4,  1, rand_mod(38),  rand_mod(20)+2, rand_mod(3)+1);
		initialize_challengeT(&challenges[start], 4,  2, rand_mod(38),  rand_mod(20)+2, rand_mod(3)+1);		
	
		memset(challenges[start].icon, 0, sizeof(challenges[0].icon));
		
		challenges[start].max_direction_state = 1;
		challenges[start].hit_box_x = 2;
		challenges[start].hit_box_y = 0;	
		
	  set_icon_char(&challenges[start], 0, CD_LEFT, 3,   85,   SYMBOL_SPADE,  73);  // 85/73 flap down,   74/75 flap up
		if (orig_start != 0)
		{
			SET_MASK(challenges[start].icon[0][CD_LEFT][1], MASK_HIGH_BIT);  // REVERSE/INVERSE the bird icon for the new spawns
		}
		
		++start;
		--count;
	}	
	
}

void initialize_stage5_challenges(unsigned char index, signed char initial_x, signed char initial_y)
{
	//memset(challenges[index].icon, 0, sizeof(challenges[0].icon));
	
	challenges[index].max_direction_state = 1;
	challenges[index].hit_box_x = 3;
	challenges[index].hit_box_y = 3;
	
  // JAMES the scorpion
                                  //                                      HP   mov  
                                  //       num      X              Y     max   spd   aggr   attk_speed
	initialize_challengeI(&challenges[index], 5,     initial_x,  initial_y, 12,  0UL,  90,  JIFFIES_QUARTER_SECOND);	
	                                //   num  N   x   y    loiter
	initialize_challengeT(&challenges[index], 4,  0, 14,  7, rand_mod(2)+2);
	initialize_challengeT(&challenges[index], 4,  1, 35, 12, rand_mod(3)+3);
	initialize_challengeT(&challenges[index], 4,  2, 18, 18, rand_mod(3)+3);		
	initialize_challengeT(&challenges[index], 4,  3, 25, 16, rand_mod(1)+4);
	
	if (initial_x < 1)  // JUSTIN
	{
	  set_icon_char(&challenges[index], 0, CD_LEFT, 4,   88,   73,  85, 105);
	  set_icon_char(&challenges[index], 1, CD_LEFT, 4,   74,  160, 189,   0);
  	set_icon_char(&challenges[index], 2, CD_LEFT, 3,    0,   75,  74, 223);
	}
	else
	{
	  set_icon_char(&challenges[index], 0, CD_LEFT, 4,   95,   73,  85,  SYMBOL_CLOVER);
	  set_icon_char(&challenges[index], 1, CD_LEFT, 4,    0,  189, 160,  75 );
  	set_icon_char(&challenges[index], 2, CD_LEFT, 3,  233,   75,  74 );
	}
}

void initialize_stage6_challenges(unsigned char start, unsigned char count)
{  
	unsigned char rand_x;
	unsigned char rand_y;
	unsigned char ofs_x;
	unsigned long rand_mov;	
				
	if (start != 0)
	{
		ofs_x = 3;
	}
	else
	{
		ofs_x = 39;
	}

	while (count > 0)
	{	
		rand_x = rand_mod(10);
		rand_y = rand_mod(13);
    rand_mov = rand_mod(2);  // 0 or 1

																	//                                         HP    mov  
																	//           num         X        Y       max    spd    aggr   attk_speed		
		initialize_challengeI(&challenges[start], 5,     ofs_x+rand_x,  rand_y+5,  2,  rand_mov, 100,   JIFFIES_THIRD_SECOND);	 

    if (rand_mod(2) == 0)  // EVENs cluster in the middle
		{
																		//         num  N     x                   y           loiter
			initialize_challengeT(&challenges[start], 4,  0, rand_mod(12)+12,  rand_mod(12)+6, rand_mov);
			initialize_challengeT(&challenges[start], 4,  1, rand_mod(12)+12,  rand_mod(12)+6, rand_mov);
			initialize_challengeT(&challenges[start], 4,  2, rand_mod(12)+12,  rand_mod(12)+6, rand_mov);					
		}
		else  // ODDs explore a little further out
		{
																		//         num  N     x                   y           loiter
			initialize_challengeT(&challenges[start], 4,  0, rand_mod(24)+7,  rand_mod(16)+4, rand_mov);
			initialize_challengeT(&challenges[start], 4,  1, rand_mod(24)+7,  rand_mod(16)+4, rand_mov);
			initialize_challengeT(&challenges[start], 4,  2, rand_mod(24)+7,  rand_mod(16)+4, rand_mov);					
		}
	
		//memset(challenges[start].icon, 0, sizeof(challenges[0].icon));		
		
		challenges[start].max_direction_state = 1;
		challenges[start].hit_box_x = 0;
		challenges[start].hit_box_y = 0;	
		
		//set_icon_char(&challenges[start], 0, CD_LEFT, 1,    121);
		set_icon_char(&challenges[start], 0, CD_LEFT, 1,    162);
		
		//set_icon_char(&challenges[start], 0, CD_RIGHT, 1,   121);
		//set_icon_char(&challenges[start], 0, CD_RIGHT, 1,   162);
		
		++start;
		--count;
	}		
}

void initialize_stage7_challenges()
{	
	memset(challenges[0].icon, 0, sizeof(challenges[0].icon));  //< Needed to clear out tail of SCOPRION
	
	challenges[0].max_direction_state = 2;
	challenges[0].hit_box_x = 3;
	challenges[0].hit_box_y = 1;
		
	memcpy(&challenges[1], &challenges[0], sizeof(Challenge));

  // JOSEPH the drake
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[0], 5,     42, 10,  6,  8UL,  85,  JIFFIES_TWELTH_SECOND);	

	                                //   num  N   x   y  loiter
	initialize_challengeT(&challenges[0], 4,  0, 22,  5,  2);
	initialize_challengeT(&challenges[0], 4,  1, 18, 10,  3);
	initialize_challengeT(&challenges[0], 4,  2, 12,  6,  1);		
	initialize_challengeT(&challenges[0], 4,  3, 37, 12,  4);
		
	set_icon_char(&challenges[0], 0, CD_LEFT, 2,  233,   73);
	set_icon_char(&challenges[0], 1, CD_LEFT, 5,    0,   74,  160, 160, 73);
	set_icon_char(&challenges[0], 2, CD_LEFT, 4,    0,    0,  108, 108 );

	set_icon_char(&challenges[0], 0, CD_RIGHT,  5,    0,    0,    0,  85, 223);
	set_icon_char(&challenges[0], 1, CD_RIGHT,  4,   85,  160,  160,  75);
	set_icon_char(&challenges[0], 2, CD_RIGHT,  3,    0,  123,  123);
	
  // JIMMY the crazy-fast drake		
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[1], 5,     40, 18, 8,  0UL,  85, JIFFIES_TWELTH_SECOND);	

	                                //   num  N   x   y  loiter
	initialize_challengeT(&challenges[1], 4,  0, 22, 12, 0);
	initialize_challengeT(&challenges[1], 4,  1, 20, 15, 0);
	initialize_challengeT(&challenges[1], 4,  2, 15, 20, 0);		
	initialize_challengeT(&challenges[1], 4,  3, 30, 16, 0);	
	
	set_icon_char(&challenges[1], 0, CD_LEFT, 2,  233,   73);
	set_icon_char(&challenges[1], 1, CD_LEFT, 5,    0,   74,  160, 160, 73);
	set_icon_char(&challenges[1], 2, CD_LEFT, 5,    0,    0,  108, 108, 74 );

	set_icon_char(&challenges[1], 0, CD_RIGHT,  5,    0,    0,    0,  85, 223);
	set_icon_char(&challenges[1], 1, CD_RIGHT,  4,   85,  160,  160,  75);
	set_icon_char(&challenges[1], 2, CD_RIGHT,  3,   75,  123,  123);
}

void initialize_stage8_challenges()
{	
	memset(challenges[0].icon, 0, sizeof(challenges[0].icon));
	
	challenges[0].max_direction_state = 1;
	challenges[0].hit_box_x = 2;
	challenges[0].hit_box_y = 1;
	
	memcpy(&challenges[1], &challenges[0], sizeof(Challenge));
	memcpy(&challenges[2], &challenges[0], sizeof(Challenge));
	//memcpy(&challenges[3], &challenges[0], sizeof(Challenge));

  // JASMINE the hydra
	
	// head1 HEART
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[0], 5,     30,  4, 10,  1UL,  100,  JIFFIES_FULL_SECOND);	

	                                //   num  N   x   y loiter
	initialize_challengeT(&challenges[0], 4,  0, 20,  4, 8);
	initialize_challengeT(&challenges[0], 4,  1, 18, 20, 3);
	initialize_challengeT(&challenges[0], 4,  2, 19,  3, 4);

	set_icon_char(&challenges[0], 0, CD_LEFT, 3,  233, 128,  75);
	set_icon_char(&challenges[0], 1, CD_LEFT, 3,    0,  95, 211);
/*
  // head2  DIAMOND	
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[1], 5,     30,  8,  8,  0UL,  100,  JIFFIES_FULL_SECOND);	

	                                //   num  N   x   y loiter
	initialize_challengeT(&challenges[1], 4,  0, 19,  7, 0);
	initialize_challengeT(&challenges[1], 4,  1, 20, 19, 0);	
	initialize_challengeT(&challenges[1], 4,  2, 26,  9, 0);	
	
			
	set_icon_char(&challenges[1], 0, CD_LEFT, 3,  233, 128,  75);
	set_icon_char(&challenges[1], 1, CD_LEFT, 3,    0,  95, 218);
*/

	// head3  CLUB/CLOVER	
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[1], 5,     30, 12,  8,  2UL,  100,  JIFFIES_FULL_SECOND);	
	                                //   num  N   x   y ofs loiter
	initialize_challengeT(&challenges[1], 4,  0, 20, 11, 2);
	initialize_challengeT(&challenges[1], 4,  1, 22, 18, 1);
	initialize_challengeT(&challenges[1], 4,  2, 25,  8, 3);
			
	set_icon_char(&challenges[1], 0, CD_LEFT, 3,  233, 128,  75);
	set_icon_char(&challenges[1], 1, CD_LEFT, 3,    0,  95, 216);

	// head4  SPADE
	
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[2], 5,     30, 16,  8,  0UL,  100,  JIFFIES_FULL_SECOND);	
	                                //   num  N   x   y ofs loiter
	initialize_challengeT(&challenges[2], 4,  0, 20, 15, 7);
	initialize_challengeT(&challenges[2], 4,  2, 18,  8, 3);
	initialize_challengeT(&challenges[2], 4,  3, 26, 18, 6);
			
	set_icon_char(&challenges[2], 0, CD_LEFT, 3,  233, 128,  75);
	set_icon_char(&challenges[2], 1, CD_LEFT, 3,    0,  95, 193);

  // FIREBALLs
                                  //                   HP   mov        atk
                                  //   num      X   Y max   spd   agg  spd
	initialize_challengeI(&challenges[3], 5,     40, 40, 99,  0UL, 100, JIFFIES_EIGTH_SECOND);
	                                //   num  N   x   y loiter
	initialize_challengeT(&challenges[3], 4,  0, 40, 40, 0);
	
	challenges[3].behavior = BEHAVIOR_DEAD;
  challenges[3].max_direction_state = 1;
	challenges[3].hit_box_x = 0;
	challenges[3].hit_box_y = 0;		
	
	set_icon_char(&challenges[3], 0, CD_LEFT, 1,  FIREBALL_SYMBOL);	 // 42 == '*'	

  // Clone three more FIREBALLS
	memcpy(&challenges[4], &challenges[3], sizeof(Challenge));
	memcpy(&challenges[5], &challenges[3], sizeof(Challenge));	
	//memcpy(&challenges[7], &challenges[4], sizeof(Challenge));	
	
	// The "associated_with" pointer is used to help adjust the icon of the main HYDRA
	// when the corresponding fireball has expired.   The HYDRA associated_with is used
	// to prevent that hydra from firing (resetting) its current fireball.
	challenges[0].associated_with = &challenges[3];
	challenges[1].associated_with = &challenges[4];
	challenges[2].associated_with = &challenges[5];
	//challenges[3].associated_with = &challenges[7];
	
	challenges[3].associated_with = &challenges[0];
	challenges[4].associated_with = &challenges[1];
	challenges[5].associated_with = &challenges[2];
	//challenges[7].associated_with = &challenges[3];
	
	challenges_count += 2;
	
#ifdef TARGET_C64
  // For some reason these extra CHALLENGES aren't working in the C64 version
#else
	initialize_stage6_challenges(challenges_count, 2);
#endif
}

void decode_stage_to_map(unsigned char* ptr_rle_stage_values, unsigned char stage_values_size) //, char pvec_map[][41])
{	
  RLE temp_rle;
  unsigned char display_symbol;
#ifdef TARGET_C64
  unsigned char display_color;
#endif	
  unsigned char virtual_x = 0; 
	unsigned char virtual_y = 2; 
  
	                       //0123456789012345678901234567890123456789  // 40 chars
  //strcpy(g_pvec_map[0], "****************************************");  //vec_push_back(pvec_map, &val_map); 		
  //strcpy(g_pvec_map[1], "****************************************");  //vec_push_back(pvec_map, &val_map);
  
  for (g_i = 0; g_i < stage_values_size; ++g_i)
  {
		temp_rle.symbol = ((ptr_rle_stage_values[g_i] & 0xE0) >> 5) & 0x07;  // & 0x07 needed to avoid taking the sign bit?
		temp_rle.length = (ptr_rle_stage_values[g_i] & 0x1F) + 1;	
					
		while (temp_rle.length > 0)
		{		  
			if (virtual_x == 39)
			{
 			  g_pvec_map[virtual_y][virtual_x] = '\0';
			  virtual_x = 0;
        ++virtual_y;				
			}
						
			switch (temp_rle.symbol)
			{				
			case 1: 
			  display_symbol = MAP_BEACH; 
#ifdef TARGET_C64
        display_color = C64_COLOR_YELLOW;
#endif				
				break;					
			case 2: 
			  display_symbol = MAP_LAND; 
#ifdef TARGET_C64
        display_color = C64_COLOR_GREEN;
#endif				
				break;
			case 4:	
			  display_symbol = MAP_SPACE; 
#ifdef TARGET_C64
        display_color = C64_COLOR_WHITE;
#endif				
				break;

			case 0: 
				display_symbol = rand_mod(2)+MAP_WATER;				
				if (display_symbol == 103) 
				{ 
			    display_symbol = 230; 
#ifdef TARGET_C64
          display_color = C64_COLOR_LBLUE;
#endif								
				}
#ifdef TARGET_C64
        else 
				{
          display_color = C64_COLOR_BLUE;
				}
#endif													
				break;  
			
			case 3: 
				//display_symbol = MAP_ROCK; 
				display_symbol = rand_mod(4)+201;    // 201 202 203 (204)
				if (display_symbol == 204) { display_symbol = 213; }
#ifdef TARGET_C64
        display_color = C64_COLOR_DGREY;
#endif												
				break;
							
			case 5:   
				//display_symbol = MAP_SPECIAL; 
				display_symbol = rand_mod(22)+233; 		
#ifdef TARGET_C64
        display_color = C64_COLOR_LGREY;
#endif																
				break;
			}
			
			g_pvec_map[virtual_y][virtual_x] = display_symbol;	  
#ifdef TARGET_C64
			g_pvec_map_color[virtual_y][virtual_x] = display_color;	  
#endif			
			
			--temp_rle.length;
			
			++virtual_x;
		}
  }
}
  
void run_stage(
  register unsigned char stage_index, 
	ptr_animate_icon_func_type ptr_animate_icon
)
{		
  Target* ptr_target;
	
#ifdef TARGET_C64	
	unsigned char joy_down_press_count = 0;
	Time_counter finish_timer;
#endif
	
	unsigned char map_piece;  //< Used when evaluating the movement cost, Water will cost more	
		
	register unsigned char i;  //< Since this is used in a lot of logic, we keep the name short.  But it should be called: temp_cell_state
	register unsigned char temp_x;  //< temp_x used for walking the cell_state, also used to move icons, and in drawing the icons (just the portion that would fit on the screen)
	register unsigned char temp_y;  //< temp_y used for walking the cell_state, also used to move icons, and in drawing the icons (just the portion that would fit on the screen)
	
	unsigned char valid_key;  //< Not exactly that a key press was valid, but that a key that caused some movement was invoked (which in doing so, causes the movement timer to get reset-- prevents moving TOO fast); includes bow movements
	register unsigned char temp_char;	//< used to hold what icon pixel/cell to draw, also used in animation-checks
	
	unsigned char challenge_move_adjustment_mask;
	
	unsigned char icon_x;  //< helper variable to iterate the icon width (since temp_x is already in use)
	unsigned char icon_y;  //< helper variable to iterate the icon width (since temp_y is already in use)
	
	unsigned char weapon_state = WS_IDLE;  //< Holds the current state of the player weapon (see WS_XXX constants)
	
	unsigned char weapon_ranging_steps = 0;  //< Keeps track of how far to fire (used as a countdown)
	unsigned char weapon_range_x;  //< Initialized when the player weapon is fired, keeps track of the arrows current x
	unsigned char weapon_range_y;  //< Initialized when the player weapon is fired, keeps track of the arrows current y
	
	unsigned char weapon_fire_symbol;	  //< Weapon does not change direction once fired, so its firing symbol can be set when it is initially fired
	
	unsigned char challenges_remaining;  //< Instead of re-order or scanning the challenge vector, just maintain a count of remaining active MOBILE challenges.
				
	signed char weapon_dx_adj;  //< Weapon DELTA_X movement, based on the time it was fired  
	signed char weapon_dy_adj;  //< Weapon DELTA_Y movement, based on the time it was fired
	
	// OBJECTS ON THIS MAP....STAGE 1
	unsigned char item_bow_x;
	unsigned char item_bow_y;	
	unsigned char item_arrows_x;
	unsigned char item_arrows_y;
	
	// OBJECTS ON THIS MAP....STAGE 3 CROCS
	unsigned char item_gem_x = 20;
	unsigned char item_gem_y = 11;
  unsigned char item_book_x = 35;
	unsigned char item_book_y = 4;	
	
	// OBJECTS ON THIS MAP....STAGE 4 CROCS
	unsigned char item_orb_x = 35;
	unsigned char item_orb_y = 3;
	// ---------------------------------------------		
		
#ifdef USE_TEST_MEM	
	size_t mem_test;
#endif	

	char* ptr_value;
	
	Challenge* ptr_challenge;	
		
	stage_event_state = STAGE_NOT_STARTED;		
	
	which_stats_modified = STATS_ALL;
	
	CLEAR_MASK(global_destiny_status.inventory, INVENTORY_ORB2);
    
	// SET INITIAL RANDOM LOCATION OF STAGE PIECES
	item_bow_x = rand_mod(12)+5;
	item_bow_y = rand_mod(3)+15;	
	
	item_arrows_x = rand_mod(arrow_locations_x_mod[stage_index]) + arrow_locations_x_ofs[stage_index];
	item_arrows_y = rand_mod(arrow_locations_y_mod[stage_index]) + arrow_locations_y_ofs[stage_index];
	// ---------------------------------------------------	
		
	// -- reset animation bits to 0; this is done in-between each stage ---
	memset(cell_state, 0, sizeof(cell_state));
	// --------------------------------------------------------------------
	
	// Assume all the challenges for this stage were already initialized before calling the run_stage function
	challenges_remaining = challenges_count;

	// Initialize the player starting location
	global_destiny_status.location_x = (rand_mod(stage_mod_x[stage_index])) + stage_min_x[stage_index];
	global_destiny_status.location_y = (rand_mod(stage_mod_y[stage_index])) + stage_min_y[stage_index];	
	
	// The following doesn't really apply to STAGE1 since they don't start with a weapon or arrows...
	global_destiny_status.curr_arrow_direction = DIR_EE;
	global_destiny_status.symbol_weapon = weapon_carried_symbols[global_destiny_status.curr_arrow_direction];
	
	if (stage_index == 1)
	{
		global_destiny_status.weapon_x = global_destiny_status.location_x;
		global_destiny_status.weapon_y = global_destiny_status.location_y;				
		global_destiny_status.energy_current = 1000;  // initialize the default set of energy
		//global_destiny_status.steps_performed = 0;
		//global_destiny_status.inventory = INVENTORY_NONE;  // holds nothing	
		// ^ these are initialized in main, before any of the stages begin
		global_destiny_status.motion_this_cycle = MOTION_NONE;
		
		global_destiny_status.hp_current = g_ptr_persona_status->hp_max;			
		
		if (global_destiny_status.direction == FORWARD_YONI_BLACK)	
		{
		  global_destiny_status.symbol = SYMBOL_YONI;
		}
		else
		{	
		  global_destiny_status.symbol = SYMBOL_LANGI;  // WHITE circle	209 
		}	  
	}
	
	STORE_TIME_NO_CORRECTOR(global_destiny_status.last_priority1_timer);	
	STORE_TIME_NO_CORRECTOR(global_destiny_status.last_priority2_timer);	
	STORE_TIME_NO_CORRECTOR(global_destiny_status.last_energy_regen_timer);

#ifdef TARGET_C64
  textcolor(C64_COLOR_WHITE);
	INIT_TIMER(finish_timer);
#endif	
	CLRSCR;		
		
	draw_stage_overlay( stage_names[stage_index] );
	draw_stage_map();

/*	
	if (stage_index == 4)
	{				
		WRITE_STRING(22, 23, str_flashback, 11);
	}
*/	
	
	update_stats(which_stats_modified);		
	
#ifdef USE_TEST_MEM	
	mem_test = available_blocks(1024);
	WRITE_PU_DIGIT(14, 0, mem_test, 5);	
#endif	
  
	while (TRUE)
	{		        
    STORE_TIME_NO_CORRECTOR(global_timer);  //< global_timer will be used as a "NOW" time

		// ** INIT STAGE UNIQUE ITEMS **********************************
		if (global_destiny_status.arrows_current == 0)
		{						
			BUFFER_LOCATION_TO_DRAW(item_arrows_x, item_arrows_y, SYMBOL_UP_ARROW
#ifdef TARGET_C64
				, C64_COLOR_WHITE
#endif				
			);  // 30 is UP ARROW, 31 is LEFT ARROW			  
		}

#ifdef FINAL_BUILD
    if (stage_index == 1)
#endif			
		{
			// PLACE ANY PICK-UP ITEMS...
      if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_BOW))
			{				
				// IDEA: animate to get players attention
  			BUFFER_LOCATION_TO_DRAW(item_bow_x, item_bow_y, SYMBOL_BOW
#ifdef TARGET_C64
          , C64_COLOR_BROWN
#endif
				);  // ")" right bow
			}
		}
#ifdef FINAL_BUILD
    else
#endif
		if (stage_index == 3)
		{
			if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_GEM))
			{
				BUFFER_LOCATION_TO_DRAW(item_gem_x, item_gem_y, SYMBOL_GEM
#ifdef TARGET_C64
          , C64_COLOR_PURPLE
#endif							
				);  // inverted DIAMONG
			}
			else if (challenges_remaining == 0)  // draw the book!
			{
		    // has the gem...
				if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_BOOK))
			  {
				  BUFFER_LOCATION_TO_DRAW(item_book_x, item_book_y, SYMBOL_BOOK
#ifdef TARGET_C64
          , C64_COLOR_WHITE
#endif												
					);  // BOOK
			  }
			}
		}
		else if (stage_index == 5)
		{
			if (
			  IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_ORB)
				&& IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_ORB2)
			)
			{
				BUFFER_LOCATION_TO_DRAW(item_orb_x, item_orb_y, SYMBOL_ORB
#ifdef TARGET_C64
          , C64_COLOR_PINK  
#endif												
				);
			}
		}
		// *************************************************************
			
    // ** PERFORM STAMINA REGENERATION CYCLE ***********************
    UPDATE_DELTA_JIFFY_ONLY(global_timer, global_destiny_status.last_energy_regen_timer);		
		if (delta_time > JIFFIES_EIGTH_SECOND)  //< If AT LEAST an eigth of a second has passed...
		{		
		  if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_ORB2))
			{
				global_destiny_status.energy_current += 300;  //< Restore 1D200 stamina points
			}
			else
			{						
				global_destiny_status.energy_current += (rand_mod(200));  //< Restore 1D200 stamina points
			}
			if (global_destiny_status.energy_current > 1000)         //< But cannot exceed max of 1000
			{
				global_destiny_status.energy_current = 1000;           //< Enforce 1000 cap limit
			}			
			
			// Technically the stamina bar only needs to be updated when it crosses one
			// of the 250 unit thresholds (since each bar in the stamina graph is 250 units).			
			// But rather than monitor that delta, we'll just update the stamina graph
			// during this EIGHT_SECOND rate.			
			SET_MASK(which_stats_modified, STATS_STAMINA);
			
			STORE_TIME_NO_CORRECTOR(global_destiny_status.last_energy_regen_timer);  //< Update the delta timer for this event.
		}		
		// **************************************************************    

		// *** ITEM COLLECTION MONITOR FOR THIS STAGE *******************
		if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_BOW))  //< If don't have the bow...
		{
			if (  //< And character is at the bow location...
			  (global_destiny_status.location_x == item_bow_x)
			  && (global_destiny_status.location_y == item_bow_y)
			)
			{
				// The following "randomizes" the direction of the BOW when the player picks it up.  But it cost about 15-bytes....
				//global_destiny_status.curr_arrow_direction = rand_mod(8);  //< Place initial direction of the bow in random direction (0-7, must match range of the DIR_XX constants; DIR_NN etc.)
        //global_destiny_status.symbol_weapon = weapon_carried_symbols[global_destiny_status.curr_arrow_direction];																		
				
				SET_MASK(global_destiny_status.inventory, INVENTORY_BOW);  //< Set flag indicating the player now possess the bow.
				SET_MASK(which_stats_modified, STATS_INVENTORY);
			}
		}
				
		if (
		  (global_destiny_status.arrows_current == 0)
		  && (global_destiny_status.location_x == item_arrows_x)
		  && (global_destiny_status.location_y == item_arrows_y)
		)
		{
			if (stage_index == 1)
			{
			  global_destiny_status.arrows_current = g_ptr_persona_status->arrows_max;
			}
			else
			{
				global_destiny_status.arrows_current = rand_mod(20)+1;
			}			  
				
			SET_MASK(which_stats_modified, STATS_ARROWS);	    			
		}
		
		if (stage_index == 3)
		{
			if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_GEM))  //< If don't have the bow...
			{
				if (  //< And character is at the bow location...
					(global_destiny_status.location_x == item_gem_x)
					&& (global_destiny_status.location_y == item_gem_y)
				)
				{							  	
					SET_MASK(global_destiny_status.inventory, INVENTORY_GEM);  //< Set flag indicating the player now possess the bow.
					SET_MASK(which_stats_modified, STATS_INVENTORY);
				}
			}
			
			if (challenges_remaining == 0)
			{
				if (IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_BOOK))  //< If don't have the bow...
				{
					if (  //< And character is at the bow location...
						(global_destiny_status.location_x == item_book_x)
						&& (global_destiny_status.location_y == item_book_y)
					)
					{				
						SET_MASK(global_destiny_status.inventory, INVENTORY_BOOK);  //< Set flag indicating the player now possess the book.
						++global_destiny_status.blessing_count;
						stage_event_state = STAGE_COMPLETED;  // auto-complete the stage (don't have to set modified-stats mask, screen will get redrawn)
					}
				}				
			}
		}		
				
		else if (
		  (stage_index == 5)
		  && IS_MASK_OFF(global_destiny_status.inventory, INVENTORY_ORB)
		)  //< If don't have the orb already...
		{
			if (  //< And character is at the bow location...
			  (global_destiny_status.location_x == item_orb_x)
			  && (global_destiny_status.location_y == item_orb_y)
			)
			{				
				SET_MASK(global_destiny_status.inventory, INVENTORY_ORB);  //< Set flag indicating the player now possess the bow.
				SET_MASK(which_stats_modified, STATS_INVENTORY);
			}
		}		
		// *****************************************************************		
		
		ENABLE_CHARACTER_SET_A;
		
		// ** RESTORE STAGE MAP PORTIONS THAT ARE NO LONGER COVERED ******** worked first try! 4/18/2021 SL
		{			
			for (temp_y = 2; temp_y < MAX_MAP_ROWS; ++temp_y)
			{
				//if (ptr_pvec_value0[temp_y] == TRUE)  // something was drawn on this row during the stage...
				{
					ptr_value = g_pvec_map[temp_y];  
					
					for (temp_x = 0; temp_x < 10; ++temp_x)  //< 10 corresponds to the width of the cell_state array
					{
						// 0x11 (3) was drawn this last animation frame (already on the screen)
						//      (2) could be used to let icons linger one extra frame (not used here)
						// 0x01 (1) was previously drawn on -- animation may stay (increase it back to 2) or it will drop back 0 (background)
						// 0x00 (0) no change
						
						i = cell_state[temp_y][temp_x];
						
						// 1 2 3 4 5 6 7 8
						// [1] [2] [3] [4]
						
						if (i == 0)
						{
							// nothing to do, no animation impacts
						}
						else
						{					
							// 7 6 5 4 3 2 1 0
							// 1 1 0 0 0 0 0 0 						C0
							// 0 0 1 1 1 1 1 1            3F
							// 0 1 0 0 0 0 0 0            40
							if ((i & 0xC0) == 0xC0) { i &= 0x3F; i |= 0x40; }  // reduce cell_1_state from 2 to 1
							else if ((i & 0xC0) == 0x40)
							{
								// draw map at cell_1 location
								temp_char = (4*temp_x);  // + 0; implied
								global_input_ch = ptr_value[ temp_char ];
#ifdef TARGET_C64
								POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+temp_char, g_pvec_map_color[temp_y][temp_char]);
#endif
								WRITE_CHAR(temp_char, temp_y, global_input_ch);								
								i &= 0x3F;  //< reduce cell_1_state from 1 to 0
							}
							
							// 7 6 5 4 3 2 1 0
							// 0 0 1 1 0 0 0 0 						30
							// 1 1 0 0 1 1 1 1            CF
							// 0 0 0 1 0 0 0 0            10						
							if ((i & 0x30) == 0x30) { i &= 0xCF; i |= 0x10; }  // reduce cell_2_state from 2 to 1
							else if ((i & 0X30) == 0x10)
							{
								// draw map at cell_2 location
								temp_char = (4*temp_x) + 1;
								global_input_ch = ptr_value[ temp_char ];				
#ifdef TARGET_C64
								POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+temp_char, g_pvec_map_color[temp_y][temp_char]);
#endif								
								WRITE_CHAR(temp_char, temp_y, global_input_ch);								
								i &= 0xCF;  //< reduce cell_1_state from 1 to 0
							}
							
							// 7 6 5 4 3 2 1 0
							// 0 0 0 0 1 1 0 0 						0C
							// 1 1 1 1 0 0 1 1            F3
							// 0 0 0 0 0 1 0 0            04
							if ((i & 0x0C) == 0x0C) { i &= 0xF3; i |= 0x04; }  // reduce cell_2_state from 2 to 1
							else if ((i & 0X0C) == 0x04)
							{
								// draw map at cell_3 location, 
								temp_char = (4*temp_x) + 2;
								global_input_ch = ptr_value[ temp_char ];												
#ifdef TARGET_C64
								POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+temp_char, g_pvec_map_color[temp_y][temp_char]);
#endif								
								WRITE_CHAR(temp_char, temp_y, global_input_ch);								
								i &= 0xF3;  //< reduce cell_3_state from 1 to 0
							}
							
							// 7 6 5 4 3 2 1 0
							// 0 0 0 0 0 0 1 1 						03
							// 1 1 1 1 1 1 0 0            FC
							// 0 0 0 0 0 0 0 1            01						
							if ((i & 0x03) == 0x03) { i &= 0xFC; i |= 0x01; }  // reduce cell_2_state from 2 to 1
							else if ((i & 0X03) == 0x01)
							{
								// draw map at cell_4 location
								g_i = (4*temp_x) + 3;
								global_input_ch = ptr_value[ g_i ];																				
#ifdef TARGET_C64
                //textcolor(g_pvec_map_color[temp_y][g_i]);
								//cputcxy(g_i, temp_y, global_input_ch);
								POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+g_i, g_pvec_map_color[temp_y][g_i]);
#endif								
								WRITE_CHAR(g_i, temp_y, global_input_ch);								
								i &= 0xFC;  //< reduce cell_4_state from 1 to 0
							}
							
							cell_state[temp_y][temp_x] = i;
						}
					}
				}
			}
		}
		// ***************************************************************	
			
    // ** DRAW ALL CELLS MARKED TO BE DRAWN **************************		
		while (num_locations_to_draw > 0)
		{
			--num_locations_to_draw;
#ifdef TARGET_C64
			POKE(BASE_COLOR_ADDRESS+(locations_to_draw[num_locations_to_draw].offset - BASE_SCREEN_ADDRESS), locations_to_draw[num_locations_to_draw].color);
#endif
			POKE(				
				locations_to_draw[num_locations_to_draw].offset,
				locations_to_draw[num_locations_to_draw].symbol
			);			
		}
		// ***************************************************************
				
		update_stats(which_stats_modified);		
		which_stats_modified = STATS_NONE;
				
		if (
		  (weapon_state == WS_FIRING)		  
		)
		{		
			if (
			  (weapon_ranging_steps > 0)
			)
			{
				--weapon_ranging_steps;  //< Regardless of what happens, count the weapon as having "stepped" forward in its motion
				
				if (  //< If the weapon is currently on the screen (and moving it would keep it on the screen)...
					(weapon_range_x > 1)
					&& (weapon_range_x < 38)  // WIDTH_OF_SCREEN-2
					// -----
					&& (weapon_range_y > 2)
					&& (weapon_range_y < 22)  // HEIGHT_OF_SCREEN-3
				)
				{
					
				  weapon_range_x += weapon_dx_adj;  //< these weapon dx/dy values were decided based on which direction the weapon was facing when it was fired
				  weapon_range_y += weapon_dy_adj;
				  
				  if (IS_BLOCKED(weapon_range_x, weapon_range_y) == TRUE)
				  {
					  // have hit a wall
					  weapon_ranging_steps = 0;
					  weapon_state = WS_IDLE;
				  }
					else
					{				  
						// did hit a challenge?
						for (i = 0; i < challenges_count; ++i)
						{
							ptr_challenge = VEC_GET(challenges, i);	
							if (
							  (ptr_challenge->behavior != BEHAVIOR_DEAD)  //< If it's not already dead...
								&& (weapon_range_x >= ptr_challenge->x)
								&& (weapon_range_x <= ptr_challenge->x+ptr_challenge->hit_box_x)
								&& (weapon_range_y >= ptr_challenge->y)								
								&& (weapon_range_y <= ptr_challenge->y+ptr_challenge->hit_box_y)
							)
							{
								if ((rand_mod(10)) < g_ptr_persona_status->att)
								{
									// hit	
									--ptr_challenge->hp_remaining;
									WRITE_PU_DIGIT(27, 0, ptr_challenge->hp_remaining, 2);  // write HP remaining of the last CHALLENGE that was hit
								
									if (ptr_challenge->hp_remaining == 0)
									{						  
										ptr_challenge->behavior = BEHAVIOR_DEAD;						
											
										if (stage_index == 3)
										{
											if (challenges_count < 4)
											{
										    initialize_stage3_challenges(challenges_count, 1);												
												++challenges_remaining;
											}
											else if (challenges_count < 5)
											{
												initialize_stage4_challenges(challenges_count, 1);  // SAMUAL
												++challenges_remaining;
											}
										}											
										else if (stage_index == 4)
										{
											if (challenges_count < 8)
											{
										    initialize_stage4_challenges(challenges_count, 2);
												++challenges_remaining;
												++challenges_remaining;
											}
										}
										else if (
										  (stage_index == 7)
											&& (challenges_count == 2)  // haven't yet spawned the SCORP
										)
										{
											if (challenges_remaining == 2)
											{
                        initialize_stage5_challenges(challenges_count, -20, 14);
												++challenges_remaining;
											} 
										}
		
										{
											--challenges_remaining;
										}
										
										if (
										  (challenges_remaining == g_pvec_map[0][1])
									  )
										{
											//print_fancy(0, 23, str_finish_map, JIFFIES_EIGTH_SECOND);
											WRITE_STRING(0, 23, str_finish_map, STR_FINISH_MAP_LEN);
											
#ifdef TARGET_C64											
                      // FLUSH JOYSTICK - wait till no activity
											STORE_TIME_NO_CORRECTOR(finish_timer);											
											while (TRUE)
											{												
												STORE_TIME_NO_CORRECTOR(global_timer);												
												UPDATE_DELTA_JIFFY_ONLY(global_timer, finish_timer);												
												
												if (delta_time > 30UL)  // 30 JIFFIES - half a second
												{
													g_i = PEEK(C64_JOYSTICK_ADDRESS_2);
													if (g_i == C64_JOYSTICK_NONE)
													{
														break;
													}													
												}
											}
#endif
										}
									
										// Mark the spot of the defeated challenge as "X" (bake it directly into the game map)
										ptr_value = g_pvec_map[weapon_range_y];
										ptr_value[weapon_range_x] = MAP_DEAD1;
										
										global_destiny_status.persistency_count += rand_mod(2)+1;
										SET_MASK(which_stats_modified, STATS_PERSISTENCY);
										
										MARK_LOCATION_AS_DRAWN(weapon_range_x, weapon_range_y);  //< Ensure that our new "X" death location marker is drawn
									}
								}
								
								weapon_state = WS_HIT_CHALLENGE;  
							}
						}
					}
				}
				else				
				{
				  // have hit edge of stage wall
				  weapon_ranging_steps = 0;
				  weapon_state = WS_IDLE;
				}
			}
			else
			{
			  weapon_state = WS_HIT_GROUND;
			}
		}
		
		if (weapon_state == WS_FIRING)  //< if still firing...
		{
#ifdef TARGET_C64
      // Background ends up WHITE, not applying this in C64 version
#else
	    /* out of memory...
      // ** OPTIONAL -- make the arrow inverted when firing over a beach
  	  if (g_pvec_map[weapon_range_y][weapon_range_x] == MAP_BEACH) 
	  	  SET_MASK(weapon_fire_symbol, MASK_HIGH_BIT); 
	    else 
		    CLEAR_MASK(weapon_fire_symbol, MASK_HIGH_BIT); 
			*/
			// **********************************************************
#endif

			BUFFER_LOCATION_TO_DRAW(weapon_range_x,weapon_range_y,weapon_fire_symbol
#ifdef TARGET_C64
          , C64_COLOR_WHITE
#endif															
			);
		}
		else if (  // hit something...
		  (weapon_state == WS_HIT_CHALLENGE)
			|| (weapon_state == WS_HIT_GROUND)
	  )
		{
			// Could make a different sound effect here...
			weapon_state = WS_IDLE;
		}							

    global_input_ch = GET_PKEY_VIEW;  //< Poll what key is currently being pressed

    // ***** PRIORITY 1 KEYS - respond with no delay ************************************************
		valid_key = TRUE;  //< Assume initially that some valid key was pressed (prevents having to explicitly set this for each valid key case)
		
#ifdef TARGET_C64
    {
			g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
			
			if (
			  IS_MASK_OFF(g_joy, C64_JOYSTICK_BUTTON)
				&& (challenges_remaining == g_pvec_map[0][1])
			)
			{
				global_input_ch = PKEY_F;
				j_fire_prepare = FALSE;
				joy_down_press_count = 0;
			}
			else if (j_fire_prepare == TRUE)
			{
				if (IS_MASK_ON(g_joy, C64_JOYSTICK_BUTTON))  // they have released the button...
				{
					j_fire_prepare = FALSE;
					if (joy_down_press_count > 1)
					{
						global_input_ch = 0;
					}
					else
					{					
					  global_input_ch = PKEY_SPACE;
					}
					joy_down_press_count = 0;
					WRITE_CHAR(0, 0, 32);
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_LEFT))
				{
					global_input_ch = PKEY_J;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_RIGHT))
				{
					global_input_ch = PKEY_L;
				}
				
				if (IS_MASK_OFF(g_joy, C64_JOYSTICK_UP))
				{
					global_input_ch = PKEY_K;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_DOWN))
				{
					global_input_ch = PKEY_I;
					
					++joy_down_press_count;
					if (joy_down_press_count > 1)
					{
						WRITE_CHAR(0, 0, 32);
					}
				}
			}
			else  // in FIRE_PREPARE MODE...
			{			
				if      (IS_MASK_OFF(g_joy, C64_JOYSTICK_UP))
				{
					global_input_ch = PKEY_W;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_DOWN))
				{
					global_input_ch = PKEY_S;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_LEFT))
				{
					global_input_ch = PKEY_A;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_RIGHT))
				{
					global_input_ch = PKEY_D;
				}
				else if (IS_MASK_OFF(g_joy, C64_JOYSTICK_BUTTON))
				{
					j_fire_prepare = TRUE;
					global_input_ch = PKEY_NO_KEY;
					WRITE_CHAR(0, 0, SYMBOL_UP_ARROW);
				}				
			}
		}
#endif		
			
		if (global_input_ch == PKEY_NO_KEY)
		{
			valid_key = FALSE;  // quicky set this case without checking for the other keys
		}
		else
		{
			UPDATE_DELTA_JIFFY_ONLY(global_timer, global_destiny_status.last_priority1_timer);  // NOW-LastMovementTimer
			if (delta_time < JIFFIES_THIRTIETH_SECOND)
			{
				// Force that no priority 1 action are yet permitted (not enough time has elapsed)
				// If not enough time since priority 1, then certainly not enough time for a priority 2 action...
			}
			else
			{
				// See if a valid PRIORITY 1 action was invoked...
				if (global_input_ch == PKEY_SPACE)				
				{
					if (
						(weapon_state == WS_IDLE)  // not already firing...
						&& (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOW))  // has the bow...
						&& (rest_mode == FALSE)  // not too tired already...
						&& (global_destiny_status.arrows_current > 0)  // and has at least 1 arrow remaining...
					)
					{
						weapon_state = WS_FIRING;
						weapon_ranging_steps = g_ptr_persona_status->range * 2;  // player stats from 0 to 9; for actual firing scale this to 2X the distance
						weapon_range_x = global_destiny_status.weapon_x;
						weapon_range_y = global_destiny_status.weapon_y;					
						
						++global_destiny_status.arrows_fired;
											
		//prepare_to_skip:
						if (
						  check_for_flipskill_learned 
							&& ((global_destiny_status.steps_performed + global_destiny_status.arrows_fired) >= 200U)   // change to BRANCH						
						)
						{                                                                                           //       ^  skip past this logic
							// Player learns the FLIP SKILL                                                           //       |  after got the FLIPSKILL, 
							SET_MASK(global_destiny_status.inventory, INVENTORY_FLIPSKILL);                           //       |  only need to enable
							SET_MASK(which_stats_modified, STATS_INVENTORY);		                                      //       |  it once.
							// ------------------------------------------------------------------------------------------------/  
							check_for_flipskill_learned = FALSE;												
						}
		//skip_target:					
						
						--global_destiny_status.arrows_current;  // Consume/account for the arrow					
						SET_MASK(which_stats_modified, STATS_ARROWS);							
					
						global_destiny_status.energy_current -= (rand_mod(300))+100;
						if (global_destiny_status.energy_current < 0)
						{
							// Used a lot of energy to shoot, enter REST MODE.
							rest_mode = TRUE;
						}
						SET_MASK(which_stats_modified, STATS_STAMINA);		
						
						switch (global_destiny_status.curr_arrow_direction)
						{  
						// Decide this NOW based on when the weapon was fired: based on the current
						// direction of the bow/arrow, decide the DELTA X/Y movement during each "step".					
						case DIR_NN: weapon_dx_adj =  0;  weapon_dy_adj = -1;  break;
						case DIR_NE: weapon_dx_adj = +1;  weapon_dy_adj = -1;  break;
						case DIR_EE: weapon_dx_adj = +1;  weapon_dy_adj =  0;  break;
						case DIR_SE: weapon_dx_adj = +1;  weapon_dy_adj = +1;  break;
						case DIR_SS: weapon_dx_adj =  0;  weapon_dy_adj = +1;  break;
						case DIR_SW: weapon_dx_adj = -1;  weapon_dy_adj = +1;  break;
						case DIR_WW: weapon_dx_adj = -1;  weapon_dy_adj =  0;  break;
						case DIR_NW: weapon_dx_adj = -1;  weapon_dy_adj = -1;  break;																				
						}					
						
						// Also store down the symbol of the weapon at the time it was fired.
						weapon_fire_symbol = weapon_fire_symbols[global_destiny_status.curr_arrow_direction];
					}
					// ELSE remain in the weapon IDLE state
				}   
#ifdef TARGET_C64				
				else if (global_input_ch == PKEY_J)
#else
	      else if (global_input_ch == PKEY_4)
#endif
				{				 
					if (global_destiny_status.curr_arrow_direction == DIR_NN)
					{
						global_destiny_status.curr_arrow_direction = DIR_NW;
					}
					else
					{
						--(global_destiny_status.curr_arrow_direction);
					}
					
					SET_MASK(global_destiny_status.motion_this_cycle, MOTION_WEAPON);
				}
#ifdef TARGET_C64
				else if (global_input_ch == PKEY_L)
#else
	      else if (global_input_ch == PKEY_6)
#endif					
				{
					++(global_destiny_status.curr_arrow_direction);
					if (global_destiny_status.curr_arrow_direction > DIR_NW)
					{
						global_destiny_status.curr_arrow_direction = DIR_NN;
					}
					
					SET_MASK(global_destiny_status.motion_this_cycle, MOTION_WEAPON);
				}
				else if (global_input_ch == PKEY_P)
				{
					flush_keyboard_and_wait_for_ENTER();
				}
#ifdef TARGET_C64
				else if (global_input_ch == PKEY_K)
#else
	      else if (global_input_ch == PKEY_0)
#endif					
				{
					if (
						(global_destiny_status.persistency_count > 0)
						&& (global_destiny_status.hp_current < g_ptr_persona_status->hp_max)
					)
					{
						--global_destiny_status.persistency_count;
						SET_MASK(which_stats_modified, STATS_PERSISTENCY);	    			
						
						++global_destiny_status.hp_current;				
						SET_MASK(which_stats_modified, STATS_HP);	    			
						
					}
				}			
				else if (global_input_ch == PKEY_B)  //< Execute a BLESSING that is appropriate for the current stage.
				{
					if (				
						(global_destiny_status.blessing_count > 0)  //< Has at least one BLESSING remaining
					)
					{
						--global_destiny_status.blessing_count;  //< Consume the blessing
						SET_MASK(which_stats_modified, STATS_BLESS);	    						
						
						// Slow down the challenges.
						for (i = 0; i < challenges_count; ++i)
						{
							ptr_challenge = VEC_GET(challenges, i);
										
							if (ptr_challenge->move_speed > 0)
							{								
								ptr_challenge->move_speed *= 2;
							}
							else
							{
								ptr_challenge->move_speed = 2;
							}
						}								
					}
				}
				else if (global_input_ch == PKEY_O)
				{
					if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_ORB))
					{
						// Use the ORB, escelate it to the next icon...
						CLEAR_MASK(global_destiny_status.inventory, INVENTORY_ORB);
						SET_MASK(global_destiny_status.inventory, INVENTORY_ORB2);
						SET_MASK(which_stats_modified, STATS_INVENTORY);
					}
				}
				else if (global_input_ch == PKEY_F)
				{
#ifdef FINAL_BUILD			
					if (challenges_remaining == g_pvec_map[0][1])  //< debug, re-enable in final!
#endif			
					{
						stage_event_state = STAGE_COMPLETED;				 
					}
				}
				else
				{
					valid_key = FALSE;
				}
				// *** END PRIORITY 1 KEYS ****************************************************				
				
				if (valid_key == TRUE)
				{
					// WAS A PRIORITY 1 action, mark the timer...
					STORE_TIME_NO_CORRECTOR(global_destiny_status.last_priority1_timer);					
				}
				else						
				{
					// **** PRIORITY 2 KEYS - respond with a minimum threshold delay ************************************************							
					UPDATE_DELTA_JIFFY_ONLY(global_timer, global_destiny_status.last_priority2_timer);  // NOW-LastMovementTimer
					if (delta_time < JIFFIES_FIFTEENTH_SECOND)
					{
						//global_input_ch = PKEY_NO_KEY;  //< Force that no movement or action is yet permitted (not enough time has elapsed)
						//valid_key = FALSE;  // must already be FALSE...
					}
					else
					{
						valid_key = TRUE;  //< 2nd priority keys, set this TRUE for same reason as earlier (assume a valid one was pressed....)
					
            if (global_input_ch == PKEY_W)
						{
							if (
								(global_destiny_status.location_y == 3)
								|| (rest_mode == TRUE)
							)
							{
								 // the movement is not allowed
							}
							else
							{															    
								if (IS_BLOCKED(global_destiny_status.location_x, global_destiny_status.location_y-1) == FALSE)
								{
									EVAL_MOVE_COST(global_destiny_status.location_x, global_destiny_status.location_y, allow_up);
									goto disallow_up;
allow_up:
									--global_destiny_status.location_y;							
									SET_MASK(global_destiny_status.motion_this_cycle, MOTION_PLAYER);
									
disallow_up:						  							
									; 
								}
							}
							
						}
            else if (global_input_ch == PKEY_S)
						{
							if (
								(global_destiny_status.location_y == 21)  // HEIGHT_OF_SCREEN-4
								|| (rest_mode == TRUE)
							)
							{
								// the movement is not allowed
							}
							else
							{				    
								if (IS_BLOCKED(global_destiny_status.location_x, global_destiny_status.location_y+1) == FALSE)					
								{
									EVAL_MOVE_COST(global_destiny_status.location_x, global_destiny_status.location_y, allow_down);
									goto disallow_down;
allow_down:							
									++global_destiny_status.location_y;							
									SET_MASK(global_destiny_status.motion_this_cycle, MOTION_PLAYER);
									
disallow_down:  				  							
									;
								}

							}
						}
            else if (global_input_ch == PKEY_A)
						{
							if (
								(global_destiny_status.location_x == 1)
								|| (rest_mode == TRUE)
							)
							{
								// the movement is not allowed
							}
							else
							{
								if (IS_BLOCKED(global_destiny_status.location_x-1, global_destiny_status.location_y) == FALSE)
								{
									EVAL_MOVE_COST(global_destiny_status.location_x, global_destiny_status.location_y, allow_left);
									goto disallow_left;
allow_left:							
									--global_destiny_status.location_x;							
									SET_MASK(global_destiny_status.motion_this_cycle, MOTION_PLAYER);
									
disallow_left:
									;  							
								}			  
							}
						}
            else if (global_input_ch == PKEY_D)
						{				  
							if (
								(global_destiny_status.location_x == 37)  // WIDTH_OF_SCREEN - 3
								|| (rest_mode == TRUE)					
							)
							{
								// the movement is not allowed
							}
							else
							{				    
								if (IS_BLOCKED(global_destiny_status.location_x+1, global_destiny_status.location_y) == FALSE)
								{
									EVAL_MOVE_COST(global_destiny_status.location_x, global_destiny_status.location_y, allow_right);
									goto disallow_right;
allow_right:							
									++global_destiny_status.location_x;
									SET_MASK(global_destiny_status.motion_this_cycle, MOTION_PLAYER);							
								}
disallow_right:
								;  				  
							}
						}		  
#ifdef TARGET_C64
						else if (global_input_ch == PKEY_I)
#else
						else if (global_input_ch == PKEY_8)
#endif					
						{					
							/*
							FLIP to the opposite direction that the bow is currently facing...
							0 -> 4     0
							1 -> 5    7 1
							2 -> 6   6   2
							3 -> 7    5 3
							4 -> 0     4 
							5 -> 1
							6 -> 2
							7 -> 3
							*/			
							if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_FLIPSKILL))
							{				
								if (global_destiny_status.curr_arrow_direction < 4)
								{
									global_destiny_status.curr_arrow_direction += 4;
								}
								else
								{
									global_destiny_status.curr_arrow_direction -= 4;
								}					
								
								global_destiny_status.symbol_weapon = weapon_carried_symbols[global_destiny_status.curr_arrow_direction];										
								//SET_MASK(global_destiny_status.motion_this_cycle, MOTION_WEAPON);  //<-- this should work and the above not required, but... idk, it caused a flicker.  so... I'm keeping it like this
							}
						}			
						else
						{
							valid_key = FALSE;
						}
						
						if (valid_key == TRUE)
						{
							STORE_TIME_NO_CORRECTOR(global_destiny_status.last_priority2_timer);							
						}

					}			
					//********* END PRIORITY 2 KEYS ********************************
				}
			}			
		}

		if (
		  (stage_event_state == STAGE_NOT_STARTED)
		)
		{
		  // see if we meet the START criteria of the current stage
			if (stage_index == 1)  //< Must have both the BOW and some ARROWS
			{
				if (		  					
					   (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOW))
					&& (global_destiny_status.arrows_current > 0)
		    )
				{
					stage_event_state = STAGE_STARTING;		
				}
			}
			else if (stage_index == 2)  //< START when move a little ways into the map
			{
				if (global_destiny_status.location_x > (rand_mod(3)+3))
				{
					stage_event_state = STAGE_STARTING;		
				}
			}
			else
			{
				stage_event_state = STAGE_STARTING;		
			}
		}
		
		else if (stage_event_state == STAGE_STARTING)
		{
			// initiate a timer used to trigger movement of challenges...
			for (i = 0; i < challenges_count; ++i)
			{
				ptr_challenge = VEC_GET(challenges, i);				
				STORE_TIME_NO_CORRECTOR(ptr_challenge->last_move_timer);
			}
			stage_event_state = STAGE_STARTED;
		}
		
		else if (stage_event_state == STAGE_STARTED)
		{			
	    if (
			  (stage_index == 5)
				&& (challenges_count == 1)  // haven't yet spawned a 2nd SCORP
			)
			{
				if (
				  (global_destiny_status.location_y < 5)
				)
				{
					// spawn from TOP
					initialize_stage5_challenges(challenges_count, 20, -8);  // JUSTIN
					++challenges_remaining;
				}
				else if (global_destiny_status.location_x > 30)
				{
					// spawn from RIGHT
					initialize_stage5_challenges(challenges_count, 40, 12);  // JUSTIN
					++challenges_remaining;
				}
				else if (global_destiny_status.location_y > 20)
				{
					// spawn from BOTTOM
					initialize_stage5_challenges(challenges_count, 20, 30);  // JUSTIN
					++challenges_remaining;
				}
			}
			
			// move all the challenges	      			
			for (i = 0; i < challenges_count; ++i)
			{				
				ptr_challenge = VEC_GET(challenges, i);
														
				if (
				  (ptr_challenge->behavior == BEHAVIOR_DEAD)
				)
				{
					// DO NOTHING... (possibility of resurrect?)
				}
				else
				{
					UPDATE_DELTA_JIFFY_ONLY(global_timer, ptr_challenge->last_move_timer);
					
					if (delta_time > ptr_challenge->move_speed)
					{						
						STORE_TIME_NO_CORRECTOR(ptr_challenge->last_move_timer);
						
						++ptr_challenge->animation_count;  // wether moved or not, count as an animation cycle
						
						// the challenge wants to get to the current target_current index
						ptr_target = &(ptr_challenge->targets[ ptr_challenge->target_current ]);
						
						if (ptr_challenge->behavior == BEHAVIOR_LOITERING)  // REMINDER: all challenges start in GOING state...
						{
							if (ptr_target->loiter_max == 0)
							{
								// This challenge does not LOITER
								ptr_challenge->behavior = BEHAVIOR_GOING;
							}
							else
							{								
								--(ptr_target->loiter_current);
								
								if ((ptr_target->loiter_current) == 0)
								{
									ptr_target->loiter_current = ptr_target->loiter_max;
										
									ptr_challenge->behavior = BEHAVIOR_GOING;
								}
								else
								{
									// 50/50 chance moving in either direction, if chosen to move...
									y_threshold = 5;
									x_threshold = 5;
									temp_x = ptr_challenge->x;
									temp_y = ptr_challenge->y;
									
									switch (rand_mod(3))
									{
									case 0:  x_delta = -1;  break;
									case 1:  x_delta = +1;  break;
									default:  break; // do nothing
									}
																		
									switch (rand_mod(3))
									{
									case 0:  y_delta = -1;  break;
									case 1:  y_delta = +1;  break;
									default:  break; // do nothing
									}
									
									goto move_due_to_loiter;
								}

							}
						}
						else   // BEHAVIOR_GOING
						{		
					    // if (ptr_challenge->targetN > 0)  // this is a piece that can move - check decision to move
							{
								// All challenges have "target paths".  Move this challenge along towards the target path.
								
								// FIRST - make sure the target is on the board... they can be "virtually" off the board initially
								if (ptr_challenge->x < 0)
								{
									// want to get to get on the board...
									++(ptr_challenge->x);
								}
								else if (ptr_challenge->y < 2)
								{
									// want to get to get on the board...
									++(ptr_challenge->y);
								}
								else if (ptr_challenge->x > 38)
								{
									// want to get to get on the board...
									--(ptr_challenge->x);
								}
								else if (ptr_challenge->y > 22)
								{
									// want to get on the board...
									--(ptr_challenge->y);
								}
								else
								{									
									// the challenge is now on the board...
									ptr_challenge->on_the_board = TRUE;
									
									++ptr_challenge->stuck_count;
									
									// if RIGHT_NEXT_TO_PLAYER
									//   attack player									

								  // As used here, these are not really DELTA values.  They are absolute x/y screen coordinates, +1 area around the challenge icon
									x_delta = ptr_challenge->x - 1;
									y_delta = ptr_challenge->y - 1;
									
									x2_delta = (ptr_challenge->x + ptr_challenge->longest_icon_width);
									y2_delta = (ptr_challenge->y + ptr_challenge->longest_icon_height);
									
									UPDATE_DELTA_JIFFY_ONLY(global_timer, ptr_challenge->last_attack_time);
									if (delta_time > ptr_challenge->attack_speed)
									{																				
										if (
											(global_destiny_status.location_x >= x_delta)
											&& (global_destiny_status.location_x <= x2_delta)
											&& (global_destiny_status.location_y >= y_delta)
											&& (global_destiny_status.location_y <= y2_delta)										
										)
										{
											if (rand_mod(10) < g_ptr_persona_status->def)
											{
												// NO HIT
											}
											else
											{
												--global_destiny_status.hp_current;
												if (global_destiny_status.hp_current == 0)
												{
													if (stage_index != 4)
													{
														// GAME OVER
														//print_fancy(15,  1, str_game_over, JIFFIES_TWELTH_SECOND);
														WRITE_STRING(15, 1, str_game_over, STR_GAME_OVER_LEN);
														//print_fancy(10, 23, str_press_return_to_proceed, JIFFIES_TWELTH_SECOND);													
												  }
													WRITE_STRING(10, 23, str_press_return_to_proceed, STR_PRESS_RETURN_TO_PROCEED_LEN);
													WRITE_CHAR(23, 24, 24);  // 'X' on the remaining HP to indicate death state

#ifdef FINAL_BUILD													
													flush_keyboard_and_wait_for_ENTER();
													stage_event_state = STAGE_DEATH;
#else													
													global_destiny_status.hp_current = 8;  // not for FINAL game
#endif
												}
												
												ENABLE_CHARACTER_SET_B;  //< Flash map content to indicate health deduction (will get reverted at top of the main loop, so the duration is the natural "rhythm" of the game)

												SET_MASK(which_stats_modified, STATS_HP);
											}
										}
										
										STORE_TIME_NO_CORRECTOR(ptr_challenge->last_attack_time);
									}
									
									if (
									  (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL)
										|| (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL_REVERSE)
									)
									{
										// This is a fireball, it won't "active track" against the player... Don't check the "stealth box"
									}
									else
									{
									
										// else if "NEAR_TO_PLAYER within STEALTH range"...
										//   move towards the player...  (dynamically adjust "target" point to be at the player?)
										
										// As used here, these are not really DELTA values.  They are absolute x/y screen coordinates, adjusted with the personas stealth value.																		
										// (since we checked for HIT first -- now expand the area around the icon to be the "stealth" distance; see if the challenge observes the player)
										x_delta -= g_ptr_persona_status->stealth;
										y_delta -= g_ptr_persona_status->stealth;
										
										x2_delta += g_ptr_persona_status->stealth;
										y2_delta += g_ptr_persona_status->stealth;
										
										if (
											(ptr_challenge->target_current != MAX_MOVE_TARGETS_PER_CHALLENGE-1)  // not already targeting the player...
											&& (ptr_challenge->aggressiveness_threshold > 0)
											&& (global_destiny_status.location_x >= x_delta)
											&& (global_destiny_status.location_x <= x2_delta)
											&& (global_destiny_status.location_x >= y_delta)
											&& (global_destiny_status.location_x <= y2_delta)										
										)
										{
											ptr_challenge->previous_target = ptr_challenge->target_current;
											
											if (rand_mod(100) < ptr_challenge->aggressiveness_threshold)  // keep going after the player?
											{
												// YES... (follow the player while trying to randomly anticipate where the player is going)
												ptr_challenge->targets[ MAX_MOVE_TARGETS_PER_CHALLENGE-1 ].target_x = global_destiny_status.location_x - 1 + rand_mod(3);  // 0 veers -1 (left), 1 veers 0 (current), 2 veers +1 (right)
												ptr_challenge->targets[ MAX_MOVE_TARGETS_PER_CHALLENGE-1 ].target_y = global_destiny_status.location_y -1 + rand_mod(3);
												ptr_challenge->target_current = MAX_MOVE_TARGETS_PER_CHALLENGE-1;										
											}
											else
											{
												// NO - challenge has lost interest in the player
											}
										}
									}
									
									// can move X and/or move Y...
									x_delta = (ptr_challenge->x - ptr_target->target_x);
									y_delta = (ptr_challenge->y - ptr_target->target_y);
									// ^ these corresponds to how far the challenge needs to go to get from current x/y to the current target x/y
									
									if (x_delta > y_delta)  // since it "takes longer" to move in X, than Y, we'll give a preference to moving in X...
									{
										x_threshold = 3;  // choosing to move in this X direction must be ABOVE this threshold
										y_threshold = 7;  // choosing to move in this Y direction must be ABOVE this threshold
									}
									else
									{
										x_threshold = 4;  // choosing to move in this X direction must be ABOVE this threshold
										y_threshold = 5;  // choosing to move in this Y direction must be ABOVE this threshold
									}
									
									temp_x = ptr_challenge->x;
									temp_y = ptr_challenge->y;
									// ^-- buffer down where the challenge is currently at.  Since it got to its current location,
									//     this is presumably a "safe" and valid location.
									
move_due_to_loiter:
									challenge_move_adjustment_mask = ADJ_NONE;
									if (x_delta != 0)
									{
										// challenge needs to move in X to get to target... should we?
										if (rand_mod(10) >= x_threshold)
										{		
											if (
											  (x_delta > 0)
												&& (temp_x > 0)
										  )
											{
												// move -1
												--temp_x;											
												SET_MASK(challenge_move_adjustment_mask, ADJ_LEFT);  // do --x_delta;
												
												ptr_challenge->direction_state = CD_LEFT;
											}
											else if (temp_x < 38)
											{
												// move +1
												++temp_x;
												SET_MASK(challenge_move_adjustment_mask, ADJ_RIGHT);  // do ++x_delta;
												
												if (ptr_challenge->max_direction_state > 1)
												{
												  ptr_challenge->direction_state = CD_RIGHT;
												}
											}
										}
									}							

									if (y_delta != 0)
									{
										// challenge needs to move in Y to get to target... should we?
										if (rand_mod(10) >= y_threshold)
										{
											if (
											  (y_delta > 0)
												&& (temp_y > 2)
										  )
											{
												// move -1
												--temp_y;	
												SET_MASK(challenge_move_adjustment_mask, ADJ_UP);  // do --y_delta;											
												
												/*
												if (ptr_challenge->max_direction_state > 2)
												{
												  ptr_challenge->direction_state = 2;
												}
												*/
											}
											else if (temp_y < 22)  // HEIGHT_OF_SCREEN-3
											{
												// move +1								
												++temp_y;
												SET_MASK(challenge_move_adjustment_mask, ADJ_DOWN);  // do ++y_delta;
												
												/*
												if (ptr_challenge->max_direction_state > 3)
												{
												  ptr_challenge->direction_state = 3;
												}
												*/
											}
										}
									}
									// --- ^^ the above has decided where challenge wants to move into, and the amount of delta reduction caused by that movement
									
									if (
									  (temp_x != ptr_challenge->x)     // tentative move in x
									  || (temp_y != ptr_challenge->y)  // tentative move in y
									)
									{
										// some amount of movement is being attempted... first see if the movement is permited
										
										if (
										  (
											  (IS_BLOCKED(temp_x, temp_y) == TRUE)
											  && (stage_index != 8)  //< Challenges in STAGE8 immune to blockers...
											)											
									  )
										{
											// ABORT.. no movement by this challenge
										}
										else
										{							
											ptr_challenge->x = temp_x;
											ptr_challenge->y = temp_y;										
											
											ptr_challenge->stuck_count = 0;  //< Since we applied a movement to the challenge, it is not stuck
											
											if (ptr_animate_icon != 0)
											{
												(*ptr_animate_icon)(ptr_challenge);
											}
											
											if (IS_MASK_ON(challenge_move_adjustment_mask,ADJ_LEFT)) --x_delta;   // delta would be positive, decrease it
											if (IS_MASK_ON(challenge_move_adjustment_mask,ADJ_RIGHT)) ++x_delta;  // delta would be negative, increase it
											if (IS_MASK_ON(challenge_move_adjustment_mask,ADJ_UP)) --y_delta;     // delta would be positive, decrease it
											if (IS_MASK_ON(challenge_move_adjustment_mask,ADJ_DOWN)) ++y_delta;   // delta would be negative, increase it
										}
									}
									
									if (
									  (  // reached the current target
										  (x_delta == 0)
									    && (y_delta == 0)
										)
										|| (ptr_challenge->stuck_count > MAX_LOITER_DURATION)  // hasn't moved in awhile (got stuck on rocks, etc)
									)
									{
										// reached the target location...
										//ptr_challenge->stuck_count = 0;
										
										if (
										  (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL)  // This is a fireball - it dies when it reaches its target
											|| (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL_REVERSE)
									  )
										{
											ptr_challenge->behavior = BEHAVIOR_DEAD;
											
											// revert the "owning" HYDRA back to non-firing mode
											(*(Challenge*)(ptr_challenge->associated_with)).icon[CD_LEFT][1][0] = 0;																						
										}
										else
										{											
										
											// compute a new target location
											if (ptr_challenge->target_current == MAX_MOVE_TARGETS_PER_CHALLENGE-1)
											{
												ptr_challenge->target_current = ptr_challenge->previous_target;
											}
											else
											{				
/* originally had NEXT/PREVIOUS - but too much memory for PET version - RANDOM works ok, I'll keep it consistent between the builds...
												switch (rand_mod(4))
												{
												case 0:  // cycle to the NEXT target
													{											  
														++(ptr_challenge->target_current);
														if (ptr_challenge->target_current >= ptr_challenge->targetN)
														{
															ptr_challenge->target_current = 0;
														}																	  
													}
													break;
													
												case 1:  // cycle to the PREVIOUS target
													{	
														if (ptr_challenge->target_current > 0)										  
														{
															--(ptr_challenge->target_current);
														}
														else
														{
															ptr_challenge->target_current = ptr_challenge->targetN-1;
														}																	  
													}
													break;
													
												case 2:  // go to a RANDOM target
													{											  
*/
														ptr_challenge->target_current = rand_mod(ptr_challenge->targetN);
/*
													}
													break;																							
												}
*/
											}
											
											// queue to loiter here for awhile before moving to next target
											ptr_challenge->behavior = BEHAVIOR_LOITERING;
										}
									}
								}
						  }
							//else
//.							{
								// NO TARGETS defined, the challenge has no where to go...
								// NOTE: we need to force the (re)draw at least once to get it on the screen
//							  ptr_challenge->on_the_board = TRUE;
//							}
						}
					}
					
					if (ptr_challenge->on_the_board == TRUE)
					{
						// Since the challenge is located on the stage/board -- render the portion of its icon that is on the map
						
						temp_x = 39 - (ptr_challenge->x);   // WIDTH_OF_SCREEN-1
						if (temp_x > ptr_challenge->longest_icon_width)
						{
							temp_x = ptr_challenge->longest_icon_width;
						}
						temp_y = 23 - (ptr_challenge->y);  // HEIGHT_OF_SCREEN-2
						if (temp_y > ptr_challenge->longest_icon_height)
						{
							temp_y = ptr_challenge->longest_icon_height;
						}							
																
						for (icon_y = 0; icon_y < temp_y; ++icon_y)
						{
							for (icon_x = 0; icon_x < temp_x; ++icon_x)
							{										
								temp_char = ptr_challenge->icon[ptr_challenge->direction_state][icon_y][icon_x];
								if (
									(temp_char == 0)													
								)
								{
									// nothing to draw - icon has a transparent "hole" portion
								}
								else 
								{		
#ifdef TARGET_C64
                  // SPECIAL: Want FIREBALLs to be RED, not the standard CHALLENGE COLOR...
							    g_i = ptr_challenge->icon[CD_LEFT][0][0];
									if (
									  (g_i == FIREBALL_SYMBOL)
										|| (g_i == FIREBALL_SYMBOL_REVERSE)
									)
									{
										BUFFER_LOCATION_TO_DRAW(
											((ptr_challenge->x)+icon_x),
											((ptr_challenge->y)+icon_y),
											temp_char,
											C64_COLOR_RED
									  );
									}
									else
#endif
									{							
										BUFFER_LOCATION_TO_DRAW(
											((ptr_challenge->x)+icon_x),
											((ptr_challenge->y)+icon_y),
									 		temp_char
#ifdef TARGET_C64											
											,
                      C64_COLOR_CYAN											
#endif											
										);
								  }								
								}
							}											
						}
					}
					else
					{
						// CHALLENGE is not on the board yet, don't waste any time on visualizing it
						// IDEA: could add a border marker for which direction it is coming from  ("peripheral vision" skill?)
					}
					// ************ END PHASE 2 *****************************************
				}
			}			
		}
		
		else if (
		  (stage_event_state == STAGE_COMPLETED)
			|| (stage_event_state == STAGE_DEATH)
		)
		{
			// Could play a sound... Break out of stage main-loop and proceed to next stage.
			ENABLE_CHARACTER_SET_A;
			break;
		}
		
		// *** PLAYER AND WEAPON MAINTENANCE *******************************    
		BUFFER_LOCATION_TO_DRAW(global_destiny_status.location_x, global_destiny_status.location_y, global_destiny_status.symbol
#ifdef TARGET_C64
          , C64_COLOR_BROWN
#endif																	
		);
		if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOW))
		{			
		  ORIENT_AND_QUEUE_DRAW_WEAPON();  //(global_destiny_status);
		}
			
    if (IS_MASK_ON(global_destiny_status.motion_this_cycle, MOTION_PLAYER))
		{
			// if the player has moved, implicitly the weapon has moved also...	 (if they have it)
			
			// NOTE: The stamina/energy required to perform the step was already accounted for (as well as any necessary block detection)
			++global_destiny_status.steps_performed;
			if (global_destiny_status.steps_performed > 9999)
			{
				global_destiny_status.steps_performed = 0;
			}
			SET_MASK(which_stats_modified, STATS_STEPS);			
			
			if (g_pvec_map[global_destiny_status.location_y][global_destiny_status.location_x] == MAP_DEAD1)
			{
				global_destiny_status.arrows_current += rand_mod(10)+1;
				SET_MASK(which_stats_modified, STATS_ARROWS);
				
				g_pvec_map[global_destiny_status.location_y][global_destiny_status.location_x] = MAP_DEAD2;
				// no need to mark as drawn, player is on top -- will get marked when player moves off
			}
			
			CLEAR_MASK(global_destiny_status.motion_this_cycle, MOTION_PLAYER);			
		}
		
		if (IS_MASK_ON(global_destiny_status.motion_this_cycle, MOTION_WEAPON))
		{      	    	
			global_destiny_status.symbol_weapon = weapon_carried_symbols[global_destiny_status.curr_arrow_direction];			
			
			CLEAR_MASK(global_destiny_status.motion_this_cycle, MOTION_WEAPON);
		}
		// ***********************************************************************		    
	}
	
	// v This is very important, to clear out the challenges before the next stage.
	challenges_count = 0;
}

void animate_stage2(Challenge* ptr_challenge)
{
	g_i = ptr_challenge->animation_count % 2;

	ptr_challenge->icon[CD_LEFT][1][1] = feet_symbolsLEFT[g_i];    // bottom left
	ptr_challenge->icon[CD_LEFT][1][2] = feet_symbolsLEFT[g_i];    // bottom left
	ptr_challenge->icon[CD_RIGHT][1][2] = feet_symbolsLEFT[g_i];   // bottom right
	ptr_challenge->icon[CD_RIGHT][1][3] = feet_symbolsLEFT[g_i];   // bottom right
}

void animate_stage3(Challenge* ptr_challenge)
{
	g_i = ptr_challenge->animation_count % 4;
	ptr_challenge->icon[CD_LEFT][0][2] = tail_symbolsLEFT[g_i];
	
	if (
	  (ptr_challenge->icon[CD_LEFT][0][1] == SYMBOL_SPADE)  // SPADE symbol
		|| (ptr_challenge->icon[CD_LEFT][0][1] == SYMBOL_SPADE_INV)
  )
	{
		// This is actually a BIRD with only a CD_LEFT icon - update the left wing portion
		ptr_challenge->icon[CD_LEFT][0][0] = wing_symbolsLEFT[g_i];
	}
	else
	{
	  ptr_challenge->icon[CD_RIGHT][0][0] = tail_symbolsRIGHT[g_i];
	}
}

void animate_stage4(Challenge* ptr_challenge)
{
	g_i = ptr_challenge->animation_count % 4;
	ptr_challenge->icon[CD_LEFT][0][0] = wing_symbolsLEFT[g_i];
	ptr_challenge->icon[CD_LEFT][0][2] = wing_symbolsRIGHT[g_i];												
}

void animate_stage5(Challenge* ptr_challenge)
{
	g_i = ptr_challenge->animation_count % 2; 
	ptr_challenge->icon[CD_LEFT][0][0] = scorp_symbolsTOP[g_i];
	ptr_challenge->icon[CD_LEFT][2][0] = scorp_symbolsBOTTOM[g_i];												
}

void animate_stage7(Challenge* ptr_challenge)
{
	g_i = ptr_challenge->animation_count % 2;
	
	if (ptr_challenge->icon[CD_LEFT][0][0] == SYMBOL_CLOVER)
	{
		// animate scorpion, one side
		ptr_challenge->icon[CD_LEFT][0][3] = scorpR_symbolsTOP[g_i];
		ptr_challenge->icon[CD_LEFT][2][3] = scorpR_symbolsBOTTOM[g_i];														
	}
	else		
	{	
	  ptr_challenge->icon[CD_LEFT][2][2] = feet_symbolsLEFT[g_i];    // bottom left
  	ptr_challenge->icon[CD_LEFT][2][3] = feet_symbolsLEFT[g_i];    // bottom left
	  ptr_challenge->icon[CD_RIGHT][2][1] = feet_symbolsLEFT[g_i];    // bottom left
  	ptr_challenge->icon[CD_RIGHT][2][2] = feet_symbolsLEFT[g_i];    // bottom left
	}	
}

#define FIRING_SYMBOL 75
void animate_stage8(Challenge* ptr_challenge)
{
	Challenge* p_corresponding_fireball;
	
	// **** IF this is a "fireball" challenge, flip its reverse bit *****
	if (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL)
	{
		SET_MASK(ptr_challenge->icon[CD_LEFT][0][0], MASK_HIGH_BIT);
	}
	else if (ptr_challenge->icon[CD_LEFT][0][0] == FIREBALL_SYMBOL_REVERSE)
	{
		CLEAR_MASK(ptr_challenge->icon[CD_LEFT][0][0], MASK_HIGH_BIT);
	}
	// ***********************************************************************
	else if (ptr_challenge->icon[CD_LEFT][1][0] == FIRING_SYMBOL)
	{
		// This HYDRA_HEAD is already in FIRING mode, nothing to do... this icon should get reset when the firing finishes hitting its target
	}	
	else  // assume this is a HYDRA challenge...
	{	
		if (rand_mod(100) > 5)
		{	
	    // Nothing to do, not yet charged up to fire...			
		}
		else  // 5% chance...
		{
			// ENGAGE FIRE MODE!
			ptr_challenge->icon[CD_LEFT][1][0] = FIRING_SYMBOL;
			
			/*
			0 --> 4   HEART
			1 --> 5   DIAMOND
			2 --> 6   CLOVER
			3 --> 7   SPADE
			*/			
			p_corresponding_fireball = ((Challenge*)(ptr_challenge->associated_with));
						
			// "resurrect" the fireball!			
			p_corresponding_fireball->behavior = BEHAVIOR_GOING;			
			
			// place the fireball at the HYDRA current location
			p_corresponding_fireball->x = (ptr_challenge->x);  // -2 to start it a little away from the mouth
			p_corresponding_fireball->y = (ptr_challenge->y);  // +1 since "mouth" is on second row of icon
			
			p_corresponding_fireball->hp_remaining = p_corresponding_fireball->hp_max;
			
			// target the fireball to the players current location
			p_corresponding_fireball->targetN = 1;
			p_corresponding_fireball->target_current = MAX_MOVE_TARGETS_PER_CHALLENGE - 1;  // "target the player"
			
			// vvvv manually doing a MIN using g_i as storage
			// p_corresponding_fireball->targets[MAX_MOVE_TARGETS_PER_CHALLENGE - 1].target_x = MIN(3,  global_destiny_status.location_x);
			g_i = global_destiny_status.location_x;
			if (g_i < 2) g_i = 3;  //< Let the BEACH be a safe area, fireballs can't go there (sand doesn't burn)
		  p_corresponding_fireball->targets[MAX_MOVE_TARGETS_PER_CHALLENGE - 1].target_x = g_i;
			// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			
			p_corresponding_fireball->targets[MAX_MOVE_TARGETS_PER_CHALLENGE - 1].target_y = global_destiny_status.location_y;			
		}		
	}                        
}

void main(void)
{	
	Time_counter started_timer;
	unsigned char temp_hp;
	
start_over:

	INIT_TIMER(global_timer);			
	
	INIT_TIMER(started_timer);				
	//INIT_TIMER((*(Time_counter*)(g_pvec_map[1][0])));
	
	g_pad_char = 48;  //< Back to 0 padded, necessary for 2nd runs of the game
	check_for_flipskill_learned = TRUE;
	
#ifdef TARGET_C64
	ENABLE_CHARACTER_SET_A;	
	textcolor(C64_COLOR_WHITE);
	bgcolor(C64_COLOR_BLACK);
	bordercolor(C64_COLOR_BLACK);	
#else	
	ENABLE_GRAPHIC_MODE;	
	
	ENABLE_CHARACTER_SET_A;
#endif
	
#ifndef QUICK_GAME
reroll:
#endif  
	CLRSCR;	
	
#ifdef QUICK_GAME
  goto quick_game;
#else	
	
	WRITE_STRING(0, 0, str_intro_notice, STR_INTRO_NOTICE_LEN);
		
	memset(&global_destiny_status, 0, sizeof(global_destiny_status));
	determine_destiny();
		
	WRITE_STRING(0, 23, str_press_return_to_proceed, STR_PRESS_RETURN_TO_PROCEED_LEN);
	WRITE_CHAR(39, 23, 152);  // X

	g_enter_result = flush_keyboard_and_wait_for_ENTER();	
	if (g_enter_result == TRUE)
	{
		goto reroll;
	}
#endif

#ifdef QUICK_GAME
quick_game:		
#endif

  choose_persona();
	
	//global_destiny_status.steps_performed = 0;
	//global_destiny_status.inventory = INVENTORY_NONE;  // holds nothing	
	//global_destiny_status.arrows_fired = 0;
	//global_destiny_status.arrows_current = 0;	
	// ^^ these are already set to 0 during a memset up above

	STORE_TIME_NO_CORRECTOR(started_timer);
  //STORE_TIME_NO_CORRECTOR((*(Time_counter*)(g_pvec_map[1][0])));
  	
	srand(global_destiny_status.seed_value);  // initiate RNG based on seed_value	    
			
#ifdef QUICK_GAME  
	WRITE_STRING(5, 5, g_ptr_persona_status->name, strlen(g_ptr_persona_status->name));	
	text_banner_center(22, str_press_return_to_proceed, SOMETHING_ELSE);				
	g_enter_result = flush_keyboard_and_wait_for_ENTER();    
#else
	conduct_intro();  
#endif	    

  g_pvec_map[0][1] = 0;

	challenges_count = 0;

  ptr_blockers = blockers_stage1_values;
	initialize_stage1_challenges();						
	decode_stage_to_map(rle_stage1_values, sizeof(rle_stage1_values));
	run_stage(1, 0);  
  if (stage_event_state	== STAGE_DEATH) goto start_over;
	
	ptr_blockers = blockers_stage2_values;		
	initialize_stage2_challenges();				
	decode_stage_to_map(rle_stage2_values, sizeof(rle_stage2_values));	
	run_stage(2, &animate_stage2);  
	if (stage_event_state	== STAGE_DEATH) goto start_over;
	
	ptr_blockers = blockers_stage3_values;
	initialize_stage3_challenges(0, 3);
	decode_stage_to_map(rle_stage3_values, sizeof(rle_stage3_values));	
	run_stage(3, &animate_stage3); 
	if (stage_event_state	== STAGE_DEATH) goto start_over;
	
	if (IS_MASK_ON(global_destiny_status.inventory, INVENTORY_BOOK))
	{
		//g_pvec_map[0][0] = global_destiny_status.hp_current;
		temp_hp = global_destiny_status.hp_current;
		global_destiny_status.hp_current = g_ptr_persona_status->hp_max;  //< give max HP back during this flashback
		
		ptr_blockers = 0;  // blockers_stage4_values;
		initialize_stage4_challenges(0, 4);
		decode_stage_to_map(rle_stage4_values, sizeof(rle_stage4_values));		
		run_stage(4, &animate_stage4); 
		
		// This is a fashback stage, the player can't actually die... Whether "died" or not, recover original HP
		//global_destiny_status.hp_current = g_pvec_map[0][0];				
		global_destiny_status.hp_current = temp_hp;				
	}
	
	ptr_blockers = blockers_stage5_values;
	initialize_stage5_challenges(0, 35, 12);
	decode_stage_to_map(rle_stage5_values, sizeof(rle_stage5_values));	
	run_stage(5, &animate_stage5);
	if (stage_event_state	== STAGE_DEATH) goto start_over;

	ptr_blockers = blockers_stage6_values;
	initialize_stage6_challenges(0,8);
	decode_stage_to_map(rle_stage6_values, sizeof(rle_stage6_values));	
	run_stage(6, 0);  
	if (stage_event_state	== STAGE_DEATH) goto start_over;
	
	ptr_blockers = blockers_stage7_values;
	initialize_stage7_challenges();
	decode_stage_to_map(rle_stage7_values, sizeof(rle_stage7_values));	
	run_stage(7, &animate_stage7); 
	if (stage_event_state	== STAGE_DEATH) goto start_over;
	
	g_pvec_map[0][1] = 3;  //< The fireballs are challenges that can't be defeated, so if drops to 3, we know all the other challenges were defeated
	ptr_blockers = blockers_stage8_values;
	initialize_stage8_challenges();	
	decode_stage_to_map(rle_stage8_values, sizeof(rle_stage8_values));
	run_stage(8, &animate_stage8); 
	if (stage_event_state	== STAGE_DEATH) goto start_over;
         
	STORE_TIME_NO_CORRECTOR(global_timer);
	UPDATE_DELTA_TIME_FULL(global_timer, started_timer);
	//UPDATE_DELTA_TIME_FULL(global_timer, (*(Time_counter*)(g_pvec_map[1][0])));

  // ********* END GAME **********************************************				 
  CLRSCR;

/*
  STEPS       1234
	FIRED       1234
	TIME       12345.123
*/

  WRITE_STRING(10, 1, g_ptr_persona_status->name, MAX_NAME_LENGTH-1);
	
  WRITE_STRING(10, 3, str_steps, STR_STEPS_LEN);
	WRITE_PU_DIGIT(20, 3, global_destiny_status.steps_performed, 4);
	
	WRITE_STRING(10, 4, str_fired, STR_FIRED_LEN);
	WRITE_PU_DIGIT(20, 4, global_destiny_status.arrows_fired, 4); 

  WRITE_STRING(10, 5, str_time, STR_TIME_LEN);	
	WRITE_PU_DIGIT(19, 5, delta_time_sec, 5);
	WRITE_CHAR(24, 5, 46);  // '.' decimal
	g_pad_char = '\0';  //< Required to ensure milliseconds is NOT padded
	WRITE_PU_DIGIT(25, 5, delta_time_ms, 3);
	// *******************************************************************
	
	WRITE_STRING(0, 7, str_thank_you, STR_THANK_YOU_LEN);
	WRITE_STRING(0, 8, str_email, STR_EMAIL_LEN);
	
	WRITE_STRING(0,10, str_press_return_to_proceed, STR_PRESS_RETURN_TO_PROCEED_LEN);
	
  g_i = flush_keyboard_and_wait_for_ENTER();
  goto start_over;	
}
