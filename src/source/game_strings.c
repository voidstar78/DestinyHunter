#include <game_strings.h>

#include <core.h>

char str_intro_notice[] = {
	132, 133, 147, 148, 137, 142, 153, 160,   // destiny
	136, 149, 142, 148, 133, 146, 160,  // hunter 
	168, 131, 169, 160,  // (c)
	178, 176, 178, 177,  160, // 2021  
	150, 143, 137, 132, 147, 148, 129, 146, 160, 160, 160, 160, 160
/* normal, NOT reversed
	4, 5, 19, 20, 9, 14, 25, 32,  // destiny  7
	8, 21, 14, 20, 5, 18, 32,  // hunter 7
	40, 3, 41, 32,  // (c)  4
	50, 48, 50, 49, 32,  // 2021  5
	22, 15, 9, 4, 19, 20, 1, 18, 32,  // voidstar   7
*/
	// 20, 5, 3, 8, 46,  // tech.  5
};

char str_press_a_key_reverse[] = {
	144, 146, 133, 147, 147, 160,  // press 6
	129, 160, // a  2
	139, 133, 153  // key  3
};

char str_YONI_reverse[] = {
	153, 143, 142, 137,  // YONI
	160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160 // filler-space	
};

char str_LINGA_reverse[] = {
	140, 137, 142, 135, 129,  // LINGA
	160, 160, 160, 160, 160, 160, 160, 160, 160  // filler-space
};

/*
char str_CHIME_reverse[] =
{
	131, 136, 137, 141, 133  // chime (in reverse)	
};
*/

char str_chime[] =
{
	3, 8, 9, 13, 5, 32, 32, 32, 32, 32, 32 // chime	
};

char str_bless[] =
{
	2, 12, 5, 19, 19, 32, 32, 32, 32, 32, 32  // bless	
};

/*
char str_again[] =
{
	1, 7, 1, 9, 14,  // again
	0,
};
*/

char str_fates_have_forged_reverse[] =
{
	// "the fates have forged your destiny    "
	148, 136, 133, 160,  // the  4
	134, 129, 148, 133, 147, 160,  // fates 6
	136, 129, 150, 133, 160,  // have 5
	134, 143, 146, 135, 133, 132, 160,  // forged 7
	153, 143, 149, 146, 160,  // your 5
	132, 133, 147, 148, 137, 142, 153, 160,  // destiny 7
	160, 160, 160,  // filler  3	
};

/*
char str_blessing[] =
{
	2, 12, 5, 19, 19, 9, 14, 7,  // blessing
	0,
};

char str_persistency[] =
{
	16, 5, 18, 19, 9, 19, 20, 5, 14, 3, 25,  // persistency
	0,
};
*/

char str_press_return_to_proceed[] =
{
	 16,  18,   5,  19,  19,  32,      // press
	146, 133, 148, 149, 146, 142, 32,  // RETURN
	 20,  15,  32,                     // to
	 16,  18,  15,   3,   5,   5,  4,   // proceed
	 0
};

/*
static char str_return_to_begin[] =
{
	16, 18, 5, 19, 19, 160,  // press
	18, 5, 20, 21, 18, 14, 160, // return
	148, 143, 160,  // to
	130, 133, 135, 137, 142,  // begin
	0
};
*/

char str_persona_alcyone[] =
{
	1, 12, 3, 25, 15, 14, 5,  // alcyone
	0
};

char str_persona_skadi[] =
{
	19, 11, 1, 4, 9,  // skadi
	0
};

char str_persona_cyrene[] =
{
	3, 25, 18, 5, 14, 5,  // cyrene
	0
};

char str_persona_rudra[] =
{
	18, 21, 4, 18, 1,  // rudra
	0
};

char str_persona_orion[] =
{
	15, 18, 9, 15, 14,  // orion
	0
};

char str_persona_sidon[] =
{
	19, 9, 4, 15, 14,  // sidon
	0
};

char str_choose_your_persona[] =
{
	3, 8, 15, 15, 19, 5, 32,  // choose
	25, 15, 21, 18, 32,   // your
	16, 5, 18, 19, 15, 14, 1,  // persona
	0
};

char str_select_persona[] =
{
	16, 18, 5, 19, 19, 32,   // press  
	177, 32,   // 1
	20, 15, 32,   // to 
	182, 32,  // 6
	20, 15, 32,   // to
	19, 5, 12, 5, 3, 20, 32,  // select
	16, 5, 18, 19, 15, 14, 1  // persona
};

char str_stat_land_move[] =
{
	12, 1, 14, 4, 32,
	13, 15, 22, 5,
	0
};

char str_stat_water_move[] =
{
	23, 1, 20, 5, 18, 32,
	13, 15, 22, 5,	
	0
};

char str_stat_stealth[] =
{
	19, 20, 5, 1, 12, 20, 8, 
	0
};

char str_stat_max_arrows[] =
{
	13, 1, 24, 46, // max.
	1, 18, 18, 15, 23, 19,  // arrows
	0
};

char str_stat_range[] =
{
	18, 1, 14, 7, 5,	
	0
};

char str_stat_attack[] =
{
	1, 20, 20, 1, 3, 11,
	0
};

char str_stat_defense[] =
{
	4, 5, 6, 5, 14, 19, 5,
	0
};

char str_stat_hp_max[] =
{
	8, 5, 1, 12, 20, 8,  // health
	0
};

char str_stage_instruction[] =
{
	130, 27, 32, 29,  // B[_]
	 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
#ifdef TARGET_C64
  139, 27, 32, 32, 29  // K[__]
#else	 
	176, 27, 32, 32, 29  // 0[__]
#endif
};

char str_finish_map[] =
{
	134, 9, 14, 9, 19, 8, 32, 13, 1, 16	
};

char str_destiny_hunter[] =
{
	160, 160, 160, 160, 160, 160, 160, 160, 160, 160,  // filler
	132, 133, 147, 148, 137, 142, 153, 160,  // destiny
	136, 149, 142, 148, 133, 146,  // hunter
	160, 160, 160, 160, 160, 160, 160, 160, 160, 160,  // filler
	0
};

#if 0
char str_you_are_dreaming[] =
{
	25, 15, 21, 32,   // you
	1,  18,  5, 32,   // are 
	4,  18,  5,  1, 13, 9, 14, 7,   // dreaming
	46, 46, 46  // ...	
};

char str_you_are_sailing[] =
{
	9, 14, 32,  // in  3
	20, 8, 9, 19, 32,  // this  5
	4, 18, 5, 1, 13, 44, 32,  // dream, 7
	25, 15, 21, 32,   // you 4
	1, 18, 5, 32,   // are  4
	19, 1, 9, 12, 9, 14, 7,  // sailing  7
	0
};

char str_towards_an_island[] =
{
	20, 15, 23, 1, 18, 4, 19, 32,  // towards  8
	1, 14, 32,   // an  3
	9, 19, 12, 1, 14, 4, 46  // island.  8
};

char str_is_with_you[] =
{
	20,  8,  5, 32,             // the
	 8, 21, 14, 20, 5, 18, 32,  // hunter
	 9, 19, 32,                 // is 
	23,  9, 20, 8, 32,          // with
	25, 15, 21, 46,             // you.	
};

char str_points_to_island[] =
{
	8, 5, 32,  // he   3
	16, 15, 9, 14, 20, 19, 32,  // point   6
	20, 15, 23, 1, 18, 4, 32,  // toward 8
	20, 8, 5, 32,  // the  4
	9, 19, 12, 1, 14, 4, 44  // island,  7	
};

char str_your_training_begins[] =
{
	34, 25, 15, 21, 18, 32,  // "your     6
	20, 18, 1, 9, 14, 9, 14, 7, 32,  // training  9
	2, 5, 7, 9, 14, 19, 32,  // begins  7
	8, 5, 18, 5, 33, 34 // here!"  6
};

char str_disembark_on_coast[] =
{
	25, 15, 21, 32,   // you    4
	4, 9, 19, 5, 13, 2, 1, 18, 11, 32,  // disembark  10
	1, 12, 15, 14, 7, 32,  // along  6
	20, 8, 5, 32,   // the    4
	3, 15, 1, 19, 20, 46, 46, 46  // coast...  9
};

char str_instructions[] =
{
	137, 142, 147, 148, 146, 149, 131, 148, 137, 143, 142, 147,  // instructions
	0
};
#endif

char str_instruction_aim[] =
{
	 1,  9, 13, 32, 32, 32, 32, 32, 32, 32, 32, 32, 180,  32,  47,  32, 182,  32,  32,  32, 23,  1, 20,  5, 18, 32, 102     // WATER
};

char str_instruction_move[] =
{
	13, 15, 22,  5, 32, 32, 32, 32, 32, 32, 32, 32, 151,  44, 129,  44, 147,  44, 132,  32,  2,  5,  1,  3,  8, 32, 160     // BEACH
};

char str_instruction_fire[] =
{
	 6,  9, 18,  5, 32, 32, 32, 32, 32, 32, 32, 32, 147, 144, 129, 131, 133,  32,  32,  32,  7, 18,  1, 19, 19, 32,  86     // GRASS
};

char str_instruction_blessing[] =
{
	 2, 12,  5, 19, 19,  9, 14,  7, 32, 32, 32, 32, 130,  32,  32,  32,  32,  32,  32,  32, 12,  1, 14,  4, 32, 27,  32, 29  // LAND
};

char str_instruction_persistency[] =
{
	16,  5, 18, 19,  9, 19, 20,  5, 14,  3, 25, 32, 176,  32,  32,  32,  32,  32,  32,  32, 18, 15,  3, 11, 19, 32, 201     // ROCKS
};
/*
char str_flashback[] =
{
	115, 140, 143, 142, 135, 160, 129, 135, 143, 161, 107    // ]LONG AGO![
};
*/
char str_game_over[] =
{
	135, 129, 141, 133, 160, 143, 150, 133, 146  // game over
};

char str_steps[] =
{
	19, 20,  5, 16, 19  // steps
};

char str_fired[] =
{
	6, 9, 18, 5, 4  // fired
};

char str_time[] =
{
	20, 9, 13, 5  // time
};

char str_thank_you[] =
{
/*
1234567890123456789012345678901234567890
THANK YOU FOR PLAYING! PAYPAL DONATE TO
*/	
	20, 8, 1, 14, 11, 32,    // thank
	25, 15, 21, 32,  // you	
	6, 15, 18, 32,   // for
	16, 12, 1, 25, 9, 14, 7, 33,   // playing!
	32, 16, 1, 25, 16, 1, 12,
	32, 4, 15, 14, 1, 20, 5,
	32, 20, 15
	//13, 25, 32,   // my
	//7, 1, 13, 5 // game
};

char str_email[] =
{
	3, 15, 14, 20, 1, 3, 20, 46, 19, 20, 5, 22, 5, 46, 21, 19, 1, 0, 7, 13, 1, 9, 12, 46, 3, 15, 13
};
