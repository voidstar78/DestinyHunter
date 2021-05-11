#include <03_intro.h>

#include <destiny_structs.h>
#include <utility.h>
#include <game_strings.h>

#ifdef TARGET_C64
  #include <conio.h>
	#include <stdio.h>
#endif
#include <string.h>  //< strlen

void conduct_intro()
{
#if 0
	CLRSCR;
		
	INVERT_BANNER_STYLE(g_ptr_persona_status->direction);
	
	text_banner_center(3, str_destiny_hunter, g_ptr_persona_status->direction);			

	WRITE_STRING(3, 7, str_you_are_dreaming, STR_YOU_ARE_DREAMING_LEN);
	
	//print_fancy(3, 9, str_you_are_sailing, JIFFIES_TWELTH_SECOND);
	WRITE_STRING(3, 9, str_you_are_sailing, STR_YOU_ARE_SAILING_LEN);
	
	//print_fancy(3,10, str_towards_an_island, JIFFIES_EIGTH_SECOND);  
	WRITE_STRING(3,10, str_towards_an_island, STR_TOWARDS_AN_ISLAND_LEN);

	WRITE_CHAR(3, 12, 27);  // [
	g_i = strlen(g_ptr_persona_status->name);
	WRITE_STRING(4, 12, g_ptr_persona_status->name, g_i);
	WRITE_CHAR(4+g_i, 12, 29);  // ]
	WRITE_STRING(6+g_i, 12, str_is_with_you, STR_IS_WITH_YOU_LEN);	
	
	g_i = 0;
  if (g_ptr_persona_status->direction == FORWARD_YONI_BLACK)
	{		
    ++g_i;
		WRITE_CHAR(3, 14, 19);  // S
	}		
      // 1234567890123456789012345678901234567890		
	//print_fancy(3+g_i, 14, str_points_to_island, JIFFIES_TWELTH_SECOND);
	WRITE_STRING(3+g_i, 14, str_points_to_island, STR_POINTS_TO_ISLAND_LEN);
	
	//print_fancy(3, 16, str_your_training_begins, JIFFIES_EIGTH_SECOND);  
	WRITE_STRING(3, 15, str_your_training_begins, STR_YOUR_TRAINING_BEGINS_LEN);  
	
	//print_fancy(3, 18, str_disembark_on_coast, JIFFIES_SIXTEENTH_SECOND);
	WRITE_STRING(3, 17, str_disembark_on_coast, STR_DISEMBARK_ON_COAST_LEN);	

	text_banner_center(22, str_press_return_to_proceed, SOMETHING_ELSE);	
		
	g_enter_result = flush_keyboard_and_wait_for_ENTER();    	
#endif
  
	CLRSCR;	
	
  INVERT_BANNER_STYLE(g_ptr_persona_status->direction);
	text_banner_center(3, str_destiny_hunter, g_ptr_persona_status->direction);			

	WRITE_STRING(6, 7, str_instruction_aim, STR_INSTRUCTION_AIM_LEN);
	WRITE_STRING(6, 9, str_instruction_move, STR_INSTRUCTION_MOVE_LEN);
	WRITE_STRING(6,11, str_instruction_fire, STR_INSTRUCTION_FIRE_LEN);
	WRITE_STRING(6,13, str_instruction_blessing, STR_INSTRUCTION_BLESSING_LEN);
	WRITE_STRING(6,15, str_instruction_persistency, STR_INSTRUCTION_PERSISTENCY_LEN);
	
	text_banner_center(20, str_press_return_to_proceed, SOMETHING_ELSE);			

	INVERT_BANNER_STYLE(g_ptr_persona_status->direction);

  // TBD: Monitor for R key to "reverse" keyboard controls
	
	g_enter_result = flush_keyboard_and_wait_for_ENTER();    
}
