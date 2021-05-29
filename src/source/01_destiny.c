#include <01_destiny.h>

#include <limits.h>  //< Called for UINT_MAX

#include <destiny_structs.h>
#include <game_strings.h>
#include <utility.h>

#define MIN_MOD_VALUE 2U    //< Corresponds to COLUMN 2, the range of the scanning "eye"
#define MAX_MOD_VALUE 21U   //< Corresponds to COLUMN 21, the range of the scanning "eye"
                            //  ^ 21-2 = 19, such that 0-19 is an extent of 20 values
#define NUM_SEEDS_TO_USE 8  //< Corresponding to the 8 stages
#define MIN_SEED_DELTA 100
#define BLOCK_PROGRESS_RANGE 22
void determine_destiny()  // Destiny_stats* ptr_destiny_stats)
{
	unsigned char seed_mod = MIN_MOD_VALUE;  // used to influence the intermediate seed_values (will be a value 1 to 10, inclusive)
	// ^ during "selection" the seed_mod will range from 2-21 to correspond to the display/graphics
	//   once space bar is made, it is scaled back down to 1-10, inclusive
	
	signed char seed_dir_delta = 1;  // signed because will be +1 or -1  (which "direction" to move the "seed-ball")
	unsigned char choice_symbol;  // set the symbol that will be used for CHIMES based on the DIRECTION chosen
	
	unsigned char attempts;  // used to retain how many attempts have been made as pulling a "good" intermediate seed (should be same type as seed_mod, since limited to the same range)
	unsigned char temp_y;
		
	unsigned int seed_delta = 0;  // seed_delta is accumulated across each intermediate seed_iteration (user "randomly" picks the deltas, not the actual seed_value)
	unsigned int temp_seed_value = 0;
		
	unsigned int INTS_PER_BLOCK = UINT_MAX / BLOCK_PROGRESS_RANGE;  // how many "unsigned integer" increments per 1-block (65535 / 21 = 31210)
	unsigned char block_progress = 0;  // how far into the available block progress we currently are
	
  unsigned char symbol_index = 0;
	
	//static const char* symbols_spiral = "\xA3\xA4\xAF\xC0\xC3\xC4\xC5\xC6\xD2\xA2";	  //< old way...
	                                      // 0    1   2   3   4    5   6   7  8   9 
  static const char symbols_spiral[] =   {100, 70, 64, 67, 68, 69, 68, 67, 64, 70};
	
#ifdef TARGET_A2
  #define BLANK_SPACE_CHAR  160
#else
	#define BLANK_SPACE_CHAR  32
#endif
	
#ifdef TARGET_C64   
  Time_counter joy_timer;
	INIT_TIMER(joy_timer);
	STORE_TIME_NO_CORRECTOR(joy_timer);
#endif	
		
	global_destiny_status.blessing_count = 0;
	global_destiny_status.persistency_count = 0;
	
#ifdef TARGET_A2
	WRITE_CHAR(0 ,1,155);
	WRITE_CHAR(23,1,157);
#else	
	WRITE_CHAR(0,1,97 | MASK_HIGH_BIT);
	
	/*
	012345678901234567890123456789
	]                      [
	  ^                  ^    limits  (20 count, gives 10 range)
	*/	
	WRITE_CHAR(23,1,97);
#endif
	
	WRITE_STRING(25,1, str_press_a_key_reverse, STR_PRESS_A_KEY_REVERSE_LEN);
		
  flush_keyboard_buffer();		
	
	// STAGE 1: establish a "direction" (and seed the "mod" used in CHIMES)...
	while (TRUE)
	{
		// GRAPHICS: DRAW STATUS UPDATE ON CURRENT VALUE OF SEED_MOD...
#ifdef TARGET_C64
		//POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*1)+seed_mod, C64_COLOR_CYAN);
#endif
		WRITE_CHAR(seed_mod, 1, SYMBOL_DIAMOND);
		// ----------------------------------------------------------
				
		global_input_ch = GET_PKEY_VIEW;  //kbhit();  // was there a KEY hit?

#ifdef TARGET_C64   
    /*
    if (global_input_ch == PKEY_NO_KEY)
    {
  		g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
			if (g_joy != C64_JOYSTICK_NONE)
			{				
				global_input_ch = PKEY_SPACE;
			}
		}
		*/
#endif

		if (global_input_ch != PKEY_NO_KEY)
		{
			// Use the MOD value to determine the "direction"...  50/50 chance
			if (seed_mod % 2 == 0)
			{
				global_destiny_status.direction = FORWARD_YONI_BLACK;
				choice_symbol = SYMBOL_YONI;
			}
			else
			{
				global_destiny_status.direction = BACKWARD_LINGA_WHITE;
				choice_symbol = SYMBOL_LANGI;  // WHITE CIRCLE
			}									
			
			// ADJUST "visual" SEED_MOD back to a numerical 1-10 (where that value range is then used in the CHIME logic)
			seed_mod -= 1;  // align back to 1-20 instead of 2-21
			seed_mod = ROUND_DIVIDE(seed_mod, 2);  // normalize back to 1-10 scale			
			
			break;  // DONE making DIRECTION+SEED_MOD selection...
		}
    // ELSE - no key was pressed...		
		
		// GRAPHICS: CLEAR CURRENT SEED_MOD VALUE (since we're about to draw it in a new position, erase old one to make it "animated")
		WRITE_CHAR(seed_mod, 1, BLANK_SPACE_CHAR);
		// ---------------------------------------------------------------------------------------
		
		seed_mod += seed_dir_delta;		
		if (seed_mod > MAX_MOD_VALUE)
		{
			// reverse the direction
			seed_mod = MAX_MOD_VALUE-1;
			seed_dir_delta = -1;
		}				
		if (seed_mod < MIN_MOD_VALUE)
		{
			// reverse the direction
			seed_mod = MIN_MOD_VALUE+1;
			seed_dir_delta = 1;
		}
	}	
	
	// GRAPHIC: Report the DIRECTION determined by the MOD_VALUE...
	if (global_destiny_status.direction == FORWARD_YONI_BLACK)
	{
		WRITE_STRING(25, 1, str_YONI_reverse, STR_YONI_REVERSE);
	}
	else  // assume BACKWARD is only other option...
	{
		WRITE_STRING(25, 1, str_LINGA_reverse, STR_LINGA_REVERSE);
	}	
		
	// accumulate seed_values, which will construct the "global seed value"
  for (
	  g_i = 1; 
	  g_i <= NUM_SEEDS_TO_USE; 
	  g_i += 1
	) 
	{    		
 	  //  1                    22
		// 0123456789012345678901234567890123456789
		// ]                      [
		
		temp_y = g_i+1;
		
#ifdef TARGET_A2
		WRITE_CHAR(0, temp_y, 155);
		WRITE_CHAR(23,temp_y, 157);
#else		
		WRITE_CHAR(0,temp_y, 97 | MASK_HIGH_BIT);
		WRITE_CHAR(23,temp_y, 97);
#endif

		//WRITE_STRING(25,temp_y, str_CHIME_reverse, STR_CHIME_REVERSE_LEN);
		WRITE_STRING(25,temp_y, str_press_a_key_reverse, STR_PRESS_A_KEY_REVERSE_LEN);

    // Converge back to the average...
		if (seed_mod > 5)
		{
			--seed_mod;
		}
		else if (seed_mod < 5)
		{
			++seed_mod;
		}
		// else let it sit at seed_mod == 5, balanced
		
		temp_seed_value = temp_seed_value / seed_mod * seed_delta;  // "corrupt" the seed_value based on initial MOD and "prior" (initial) SEED_DELTA

    attempts = 0;  // reset the number of "try_again" attempts for this CHIME...
		flush_keyboard_buffer();
		while (TRUE)
		{
try_again:			        	
      if (block_progress < BLOCK_PROGRESS_RANGE)
			{				
		    WRITE_CHAR(block_progress+1, temp_y, BLANK_SPACE_CHAR);
			}

			seed_delta += (seed_mod * 100);
			//++seed_delta;  // <-- old original policy
			
			block_progress = seed_delta / INTS_PER_BLOCK;
			
			if (block_progress < BLOCK_PROGRESS_RANGE)
			{							  
			  ++symbol_index;
			  if (symbol_index > 9)  // 10 is MAX_LENGTH of symbols
			  {
  				symbol_index = 0;
			  }
#ifdef TARGET_C64
				POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+(block_progress+1), C64_COLOR_CYAN);
#endif
				
				WRITE_CHAR(block_progress+1, temp_y, symbols_spiral[symbol_index]);
			}
						
			global_input_ch = GET_PKEY_VIEW;
			
#ifdef TARGET_C64   
      /*
      STORE_TIME_NO_CORRECTOR(global_timer);
			UPDATE_DELTA_JIFFY_ONLY(global_timer, joy_timer);
			if (delta_time > JIFFIES_EIGTH_SECOND)
			{
				if (global_input_ch == PKEY_NO_KEY)
				{
					g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
					if (g_joy != C64_JOYSTICK_NONE)
					{					
						global_input_ch = PKEY_SPACE;
					}
				}
				joy_timer = global_timer;
			}
			*/
#endif
			
			if (global_input_ch != PKEY_NO_KEY)
			{						
				
				if (seed_delta < MIN_SEED_DELTA)
				{				
					// TOO EARLY!  BEEP...  
					//cprintf("early! [%5u]", seed_delta);
					//DO_BEEP;
				}			
				else
				{
					++attempts;		

					// APPLY the "seed_delta" in the appropriate direction...
					if (global_destiny_status.direction == FORWARD_YONI_BLACK)
					{
						temp_seed_value += seed_delta;  // go "up"
					}
					else
					{
						temp_seed_value -= seed_delta;  // go "down"
					}
					
					// EXAMINE if this resulting intermediate/temporary seed_value is "BLESSED" (with possibility of a retry)
					if ((temp_seed_value % seed_mod) == 0)
					{
#ifdef TARGET_A2
            // TBD
#elif TARGET_C64
	          AUDIO_TURN_ON;
					
						AUDIO_SET_OCTAVE(51U); //audio_octv[symbol_index*2]);
						AUDIO_SET_FREQUENCY(audio_frq0[symbol_index+14]);
						
						jiffy_delay(JIFFIES_SIXTEENTH_SECOND);
						AUDIO_TURN_OFF;
#endif
						
						if (attempts > seed_mod)
						{			
#ifdef TARGET_A2
              // TBD
#elif TARGET_C64					
	            // BLESSING!
						  AUDIO_TURN_ON;
							
							AUDIO_SET_OCTAVE(51U);  //audio_octv[20]);
							AUDIO_SET_FREQUENCY(69U);  //audio_frq0[24]);  // A
							
							jiffy_delay(JIFFIES_EIGTH_SECOND);
							
							AUDIO_SET_FREQUENCY(65U);  //audio_frq0[25]);  // A#
							jiffy_delay(JIFFIES_EIGTH_SECOND);
							
							AUDIO_TURN_OFF;
#endif
					
					    ++global_destiny_status.blessing_count;
							
							// Exceed the "MOD" number of re-tries.  Consider this seed_value to be CURSED!
							// NOTE: This also avoids getting stuck in infinite re-tries... (the max will be 9)			    
#ifdef TARGET_C64
              for (symbol_index = 0; symbol_index < STR_BLESS_LEN; ++symbol_index)
							{								
							  POKE(BASE_COLOR_ADDRESS+(WIDTH_OF_SCREEN*temp_y)+25+symbol_index, C64_COLOR_YELLOW);
							}
#endif							
							WRITE_STRING(25, temp_y, str_bless, STR_BLESS_LEN);
							WRITE_1U_DIGIT(36,temp_y, seed_mod);
						}
						else
						{									
              ++global_destiny_status.persistency_count;						
												
							WRITE_1U_DIGIT(36, temp_y, attempts);
							flush_keyboard_buffer();
							goto try_again;
						}
					}
					else
					{
            WRITE_STRING(25, temp_y, str_chime, STR_CHIME_LEN);
					}						

					WRITE_CHAR(block_progress+1, temp_y, choice_symbol);
								        
					break;
				}		
			}			
		}
				
		// Store this temporary value...		
		global_destiny_status.seed_value += temp_seed_value;
	}	
		
	//print_fancy(0, temp_y+1, str_fates_have_forged_reverse, JIFFIES_TWELTH_SECOND);
	WRITE_STRING(0, temp_y+1, str_fates_have_forged_reverse, STR_FATES_HAVE_FORGED_REVERSE_LEN);
	
	temp_y += 2;	
	WRITE_PU_DIGIT(25, temp_y, global_destiny_status.blessing_count, 2);	
	WRITE_PU_DIGIT(35, temp_y, global_destiny_status.persistency_count, 2);	
}

