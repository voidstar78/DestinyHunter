#include <utility.h>

#include <core.h>     //< TRUE/FALSE, keyboard key and other essential constants
#include <string.h>   //< Used for strlen
#include <stdlib.h>   //< Used for rand

unsigned char global_input_ch;  

unsigned char g_i;  //< Global loop integer, only use once at a time, not nested
unsigned char g_enter_result;  //< To be used when calling wait_for_ENTER

unsigned char banner_style[][] = {
	{64,  64, 226},  // buffer_ch_T     TOP
  {70,  64,  98},  // buffer_ch_B     BOTTOM
  {85, 112, 236},  // buffer_ch_TL    TOP_LEFT
  {73, 110, 251},  // buffer_ch_TR    TOP_RIGHT
  {74, 109, 252},  // buffer_ch_BL    BOTTOM_LEFT
	{75, 125, 254},  // buffer_ch_BR    BOTTOM_RIGHT
  {66,  93,  97},  // buffer_ch_LEFT  LEFT
	{72,  93, 225},  // buffer_ch_RIGHT RIGHT
	{32,  32,  32}   // buffer_ch_FILLER
// YO   LA  OTHR
};

unsigned char rand_mod(unsigned char n)
{	  
	return (rand() % n);
}

void flush_keyboard_buffer()
{
	while (TRUE)
	{
	  global_input_ch = GET_PKEY_VIEW;
	  if (global_input_ch == PKEY_NO_KEY)
	  {		  
		  break;
	  }	  
	}
}

unsigned char flush_keyboard_and_wait_for_ENTER()
{
	unsigned char result = FALSE;
	
	flush_keyboard_buffer();  // Make sure user doesn't accidently spam past this point
	
#ifdef TARGET_C64
  /*
  while (TRUE)
	{
    g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
		if (g_joy == C64_JOYSTICK_NONE)
		{
			break;
		}
	}
	*/
#endif	
	while (TRUE)
	{
	  global_input_ch = GET_PKEY_VIEW; 
		
#ifdef TARGET_C64
    /*
    if (global_input_ch == PKEY_NO_KEY)
    {
  		g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
			if (g_joy != C64_JOYSTICK_NONE)
			{
				global_input_ch = PKEY_RETURN;
			}
		}
		*/
#endif		
		
	  if (global_input_ch != PKEY_NO_KEY)
	  {		  
			if (
				(global_input_ch == PKEY_X)				
			)
			{
				result = TRUE;
				break;
			}
			
			if (global_input_ch == PKEY_RETURN)
			{
				break;
			}
			
			// Implicitly an ELSE since the above two do BREAKs...
			
			// Maybe they are using a 2000/3000.  Check for those key codes.
			if (
			  (global_input_ch == B_PKEY_RETURN)
				|| (global_input_ch == B_PKEY_X)
		  )
			{				
				if (global_input_ch == B_PKEY_X)  // Preserve the intent that "X" returns TRUE
				{
				  result = TRUE;
				}
				
				// If these ALTERNATE B-series key codes were pressed, 
				// then swap to using the "B-series" (2000/3000) key codes entirely.
				// In doing this re-assignment, these B-series codes will
				// become the "normal" constant values from here on out.
				// Note that each assignment consumes code-space, so only
				// include the essential set of keycodes used by the program.
				PKEY_RETURN      = B_PKEY_RETURN;
				//PKEY_BACKSPACE   = B_PKEY_BACKSPACE;
				PKEY_SPACE       = B_PKEY_SPACE;
				//PKEY_UP          = B_PKEY_UP;
				//PKEY_LEFT        = B_PKEY_LEFT;
				//PKEY_RIGHT       = B_PKEY_RIGHT;
				//PKEY_DOWN        = B_PKEY_DOWN;
				//PKEY_PLUS        = B_PKEY_PLUS;
				//PKEY_MINUS       = B_PKEY_MINUS;
				//PKEY_INSTDEL     = B_PKEY_INSTDEL;
				//PKEY_LANGBRACKET = B_PKEY_LANGBRACKET;
				//PKEY_LEFT_ARROW  = B_PKEY_LEFT_ARROW;
				//PKEY_LSQBRACKET  = B_PKEY_LSQBRACKET;
				//PKEY_EQUALS      = B_PKEY_EQUALS;
				//PKEY_LEFT_BRK    = B_PKEY_LEFT_BRK;
				//PKEY_RIGHT_BRK   = B_PKEY_RIGHT_BRK;
				PKEY_A           = B_PKEY_A;
				PKEY_B           = B_PKEY_B;
				PKEY_D           = B_PKEY_D;
				//PKEY_E           = B_PKEY_E;
				PKEY_F           = B_PKEY_F;
				//PKEY_G           = B_PKEY_G;
				//PKEY_H           = B_PKEY_H;
				PKEY_I           = B_PKEY_I;
				PKEY_J           = B_PKEY_J;
				PKEY_K           = B_PKEY_K;
				PKEY_L           = B_PKEY_L;
				PKEY_P           = B_PKEY_P;
				PKEY_O           = B_PKEY_O;
				//PKEY_R           = B_PKEY_R;
				PKEY_S           = B_PKEY_S;
				//PKEY_U           = B_PKEY_U;
				PKEY_W           = B_PKEY_W;
				PKEY_X           = B_PKEY_X;
				PKEY_0           = B_PKEY_0;
				PKEY_1           = B_PKEY_1;
				PKEY_2           = B_PKEY_2;
				PKEY_3           = B_PKEY_3;
				PKEY_4           = B_PKEY_4;
				PKEY_5           = B_PKEY_5;
				PKEY_6           = B_PKEY_6;
				PKEY_7           = B_PKEY_7;
				PKEY_8           = B_PKEY_8;
				//PKEY_Z           = B_PKEY_Z;								
				break;
			}
	  }
		else
		{
			// Keep looping until any key is pressed.
		}
	}
	
	return result;
}

// to invert the banner
void INVERT_BANNER_STYLE(unsigned char x)
{	
  unsigned char y;
  //for (x = 0; x < 3; ++x)
	{
		for (y = 0; y < buffer_ch_COUNT; ++y)
		{
			if (IS_MASK_ON(banner_style[y][x], MASK_HIGH_BIT))
			{
				CLEAR_MASK(banner_style[y][x], MASK_HIGH_BIT);			  
			}
			else
			{				
				SET_MASK(banner_style[y][x], MASK_HIGH_BIT);			  
			}
		}
	}		
}

#define BANNER_SPACE_ADJUSTMENT 2
void text_banner_len(unsigned char x, unsigned char y, const char* text_to_say, unsigned char str_len, unsigned char style)
{
	unsigned char Xadj2;
	unsigned char YadjM1 = y-1;
	unsigned char YadjP1 = y+1;
	unsigned char len_adjustment = str_len + (BANNER_SPACE_ADJUSTMENT * 2) - 2;  // text_to_say is flanked by one extra space and the border edge		
	
	// TOP
	Xadj2 = x - BANNER_SPACE_ADJUSTMENT;
	WRITE_CHAR(Xadj2, YadjM1, banner_style[buffer_ch_TL][style]); 	
	g_i = len_adjustment;
	while (g_i > 0)
	{
		WRITE_CHAR(Xadj2+g_i, YadjM1, banner_style[buffer_ch_T][style]);
		--g_i;
	}
	WRITE_CHAR(Xadj2+len_adjustment+1, YadjM1, banner_style[buffer_ch_TR][style]);
			
	// CENTER
	WRITE_CHAR(Xadj2, y, banner_style[buffer_ch_LEFT][style]);
	WRITE_CHAR(x-1, y, banner_style[buffer_ch_FILLER][style]);  // filler
	WRITE_STRING(x, y, text_to_say, str_len);	
	WRITE_CHAR(x+str_len, y, banner_style[buffer_ch_FILLER][style]);  // filler
	WRITE_CHAR(x+len_adjustment-1, y, banner_style[buffer_ch_RIGHT][style]);
		
	// BOTTOM
	WRITE_CHAR(Xadj2, YadjP1, banner_style[buffer_ch_BL][style]);
	g_i = len_adjustment;
	while (g_i > 0)  // for (g_i = 0; g_i < len_adjustment; ++g_i)
	{
		WRITE_CHAR(Xadj2+g_i, YadjP1, banner_style[buffer_ch_B][style]);
		--g_i;
	}
	WRITE_CHAR(Xadj2+len_adjustment+1, YadjP1, banner_style[buffer_ch_BR][style]);
}

// Draws a banner such that the text_to_say is center on the given row y,
// likewise using the given style-tyle.
void text_banner_center(unsigned char y, const char* text_to_say, unsigned char style)
{
	text_banner_len(
	  ((WIDTH_OF_SCREEN - strlen(text_to_say)) / 2), y, 
		text_to_say, 
		strlen(text_to_say), 
    style
	);
}

void print_fancy(unsigned char x, unsigned char y, const char* temp_str, unsigned char delay)
{		
	unsigned char i;
	unsigned char n = delay;	
	unsigned char str_len = strlen(temp_str);
	Time_counter delay_timer;
		
	STORE_TIME_NO_CORRECTOR(delay_timer);
	
	for (i = 0; i < str_len; ++i)
	{		
    while (TRUE)
		{
			global_input_ch = GET_PKEY_VIEW;  //< Pressing a key during the "fancy_print" will speed it up
			
#ifdef TARGET_C64
    if (global_input_ch == PKEY_NO_KEY)
    {
 		  g_joy = PEEK(C64_JOYSTICK_ADDRESS_2);
			if (g_joy != C64_JOYSTICK_NONE)
			{
				global_input_ch = PKEY_SPACE;
			}
		}
#endif		
			
			if (global_input_ch != PKEY_NO_KEY)
			{			
				n = 0;  //< Ends up forcing the delay to get adjusted to the MINUMUM_FANCY
			}
				
			STORE_TIME_NO_CORRECTOR(global_timer);  
			UPDATE_DELTA_JIFFY_ONLY(global_timer, delay_timer);
			if (delta_time > n)
			{
				STORE_TIME_NO_CORRECTOR(delay_timer);				
				
				// RNG how long this "fancy" will appear
				if (n > 0)
				{
					n = rand_mod(delay) + 1;
				}
				
				break;				
			}
			else
			{
				// The "fancy" is to "sparkle" a set of symbols before drawing the next character
				// of the sequence, like they are magically being written.	
				WRITE_CHAR(x+i, y, rand_mod(93) + 161);								
			}
		}
		
		// with the magic completed, now draw the actual next character of the sequence.
		WRITE_CHAR(x+i, y, temp_str[i]);
	}
}

