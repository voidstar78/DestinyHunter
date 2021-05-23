#ifndef DESTINY_STRUCTS_H
#define DESTINY_STRUCTS_H

#include <core.h>

#ifdef TARGET_C64
  #define SYMBOL_YONI  81
#else
	#define SYMBOL_YONI  87
#endif
#define SYMBOL_LANGI 81

#ifdef TARGET_C64
  #define SYMBOL_GEM 90
#else
  #define SYMBOL_GEM 218
#endif

#define SYMBOL_BOOK      221
#define SYMBOL_INV_BOW   169
#define SYMBOL_INV_GEM    90
#define SYMBOL_INV_BOOK  221

#ifdef TARGET_C64
  #define SYMBOL_INV_FLIP    9 
#else
	#define SYMBOL_INV_FLIP  184
#endif

#define SYMBOL_INV_ORB1  209
#define SYMBOL_INV_ORB2  215
#define SYMBOL_SPADE      65
#define SYMBOL_SPADE_INV 193   // C1h, 65 with HIGH-BIT set to 1
#define SYMBOL_DIAMOND    90
#define SYMBOL_CLOVER     88
#define SYMBOL_UP_ARROW   30
#define SYMBOL_BOW        41
#define SYMBOL_ORB       215

#define MAX_BLESSING_COUNT 9       //< limit max number of blessings to 1 character
#define MAX_PERSISTENCY_COUNT 99	 //< limit max number of persistencies to 2 characters

#define MAX_RLE_ENTRIES 250        //< This an EDITOR limitation when doing the RLE compression, not a GAME limitation (8-bit limited to 255)
#define MAX_BLOCKER_ENTRIES 250    //< This an EDITOR limitation when doing the RLE compression, not a GAME limitation (8-bit limited to 255)

// WEAPON DIRECTION, NORTH, NORTH-EAST, EAST, etc...
#define DIR_NN 0
#define DIR_NE 1
#define DIR_EE 2
#define DIR_SE 3
#define DIR_SS 4
#define DIR_SW 5
#define DIR_WW 6
#define DIR_NW 7

#define MAP_WATER   102    // was 'W'  // concept was to be animated
#define MAP_BEACH   160    // was 'B'
#define MAP_ROCK    203    // was 'R'
#define MAP_SPECIAL 223    // 'Z'  // similiar to rock, just different style
#define MAP_LAND    86     // GRASS  was 'G'  was 214
#define MAP_SPACE   32  // EMPTY  was 'S'  was ' '
#define MAP_DEAD1   31  // used to represent defeated challenges, can still pick up arrows
#define MAP_DEAD2   24  // used to represent defeated challenges, no arrows to pick up

// NAME LENGTHS WILL BE THIS VALUE-1 (last position for null character)
#define MAX_NAME_LENGTH 8

// "TWO" DIRECTION TYPES
#define FORWARD_YONI_BLACK   0   // FORWARD ==YIN /YONI  (black-half, feminine)      
#define BACKWARD_LINGA_WHITE 1   // BACKWARD==YANG/LINGA (white half, masculine)
// -- "something_else" is used as an extra banner type not related to the above direction-types
#define SOMETHING_ELSE       2

// Bit masked for destiny player INVENTORY
#define INVENTORY_NONE      0x00
#define INVENTORY_BOW       0x01
#define INVENTORY_GEM       0x02
#define INVENTORY_BOOK      0x04
#define INVENTORY_FLIPSKILL 0x08
#define INVENTORY_ORB       0x10
#define INVENTORY_ORB2      0x20  // activated orb

// Bit masked for motion_this_cycle
#define MOTION_NONE   0x00
#define MOTION_PLAYER 0x01
#define MOTION_WEAPON 0x02

typedef struct 
{
	unsigned char symbol;
	unsigned char length;
	
	//unsigned char encoded;  // MAP_EDITOR MODE ONLY?
} RLE;

typedef struct 
{
	unsigned char x;
	unsigned char y;
} Blocker;

// The "main player" has a Destiny, so this struct represents states of the character
typedef struct
{		
	unsigned char direction;    // not a "point towards", sort of a "gender" but more of a bias or "predisposition-towards" attribute
	
	unsigned int seed_value;          // affinity  (seed for RNG)
	unsigned char blessing_count;     // used for free-levels, gift-items, power-ups
	unsigned char persistency_count;  // sort of like potions
	
	unsigned char hp_current;         // HP that player currently has
	unsigned char arrows_current;     // number of arrows the PLAYER has  (0-99)
	
	unsigned char curr_arrow_direction;  // see DIR_XX macros listed earlier
	
	unsigned char location_x;  // absolute screen position of character X col
	unsigned char location_y;  // absolute screen position of character Y row
	unsigned char symbol;      // symbol of what the character currently looks like (decided by direction)
	
	unsigned char weapon_x;    // absolute screen position of the character weapon  X col
	unsigned char weapon_y;    // absolute screen position of the character weapon  Y col
	unsigned char symbol_weapon;  // symbol of what the character weapon currently looks like
	
	unsigned char motion_this_cycle;  // FLAGS indicate what components have moved during this animation cycle, see MOTION_XXX constants
	
	unsigned int steps_performed;  // counter of the number of character movements (overall); will cap to 9999 (4-digit) of screen real-estate
	unsigned int arrows_fired;
	
	int energy_current;  // energy is like endurance                  0-1000
	
	unsigned char inventory;  // see INVENTORY_XXX bits
	
	Time_counter last_priority1_timer;
	Time_counter last_priority2_timer;
	Time_counter last_energy_regen_timer;
} Destiny_status;
extern Destiny_status global_destiny_status;

// A Persona is sort of like a "spirit", defines the attributes of the player -- but may get swapped during certain aspects of the story
#define MAX_PERSONAS_TO_SELECT 6
typedef struct 
{
	char name[MAX_NAME_LENGTH];
	
	unsigned char direction;
	
	unsigned char land_movement;   // speed when NOT on water                   0-9  (0 is NOT movable)
	unsigned char water_movement;  // speed when in water "W"                   0-9
	
	unsigned char stealth;         // ability to be ignored by monsters         0-9  (aka hidden, unseen)
	
	unsigned char range;           // STR relates to how far can shoot          0-9
	unsigned char arrows_max;      // kind of a carrying capacity               0-99  
	
	unsigned char att;             // ATTACK STAT, applies to player+persona    0-9  (includes precision, power)
	unsigned char def;             // DEFENSE STAT, applies to player+persona   0-9  (includes agility, dodge)
	
	unsigned char hp_max;          // HP MAX of this persona                    0-8  (seeds Player initial HP MAX)	
} Persona_status;
extern Persona_status g_pvec_personas[MAX_PERSONAS_TO_SELECT];
extern unsigned char g_pvec_personas_count;
extern Persona_status* g_ptr_persona_status;	

#ifdef TARGET_C64
typedef struct 
{
	unsigned int offset;  // address
	unsigned char symbol;
	unsigned char color;
} Location_to_draw;
#endif

typedef struct 
{
	//signed char orig_x;  //< Part of early concept to remember the original target location (after loitering, maybe return back to that original location)
	//signed char orig_y;
	
	signed char target_x;
	signed char target_y;
	
	//unsigned char random_offset;  // how much random offset to allow for x/y target location
	
	unsigned char loiter_max;  // how long to loiter when reached the target location before going to next target
	unsigned char loiter_current;  // how long to loiter when reached the target location before going to next target
} Target;

// queue challenges
#define BEHAVIOR_GOING      0
#define BEHAVIOR_LOITERING  1
#define BEHAVIOR_DEAD       2

// CD = Challenge Direction
#define CD_LEFT  0
#define CD_RIGHT 1
//#define CD_UP    2
//#define CD_DOWN  3
#define MAX_MOVE_TARGETS_PER_CHALLENGE 5
typedef struct 
{
	signed char x;  // signed so they can be "off screen" to the left side initially (negative)
	signed char y;  // signed so they can be "off screen" to the right side initially (negative)
		
	unsigned char hp_max;
	unsigned char hp_remaining;  // 0-9.... some run away after 50%
	
	unsigned char animation_count;  //< Used to animate certain icons
	
	unsigned long move_speed;    // number of JIFFIES between movements (smaller value to make the challenge move faster)
	Time_counter last_move_timer;    // timer when the challenge last moved
	
	unsigned char behavior;  // 0 = going, 1 = loitering, 2 = dead  (see BEHAVIOR_XXX constants)
	
	unsigned char targetN;
	unsigned char target_current;  // 0 to targetN-1
	unsigned char previous_target;  // 0 to targetN-1
	Target targets[MAX_MOVE_TARGETS_PER_CHALLENGE];
	
	unsigned char max_direction_state;
	unsigned char direction_state;  // 0=LEFT, 1=RIGHT, 2=UP, 3=DOWN    (see CD_XXX states, CD = challenge direction)
	char icon[2][3][5];  // [direction] [row] [cols]
	
	unsigned char longest_icon_width;
	unsigned char longest_icon_height;
	
	Time_counter last_attack_time;
	unsigned char stuck_count;
	unsigned char aggressiveness_threshold;  // 0 to 100
	
	unsigned char on_the_board;
	
	unsigned char hit_box_x;  // 0-based, reminder: 0 means 1
	unsigned char hit_box_y;  // 0-based, reminder: 0 means 1   (1x1 is smallest hitbox)
	
	unsigned char attack_speed;
	
	void* associated_with;
	
} Challenge;
#define MAX_CHALLENGES_PER_STAGE 10
extern Challenge challenges[MAX_CHALLENGES_PER_STAGE];
extern unsigned char challenges_count;

#endif