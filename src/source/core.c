#include <core.h>

#ifdef TARGET_A2

  //int time_temp_int;
  //struct timespec time_temp;
	unsigned char global_kb;
	
	unsigned int screen_row_offset[] =
	{
		1024U,  // 0x0400
		1152U,
		1280U,
		1408U,
		1536U,
		1664U,
		1792U,
		1920U,
		1064U,
		1192U,
		1320U,
		1448U,
		1576U,
		1704U,
		1832U,
		1960U,
		1104U,
		1232U,
		1360U,
		1488U,
		1616U,
		1744U,
		1872U,
		2000U
	};

	unsigned char PKEY_RETURN      = 0x8D;  //  D
	//unsigned char PKEY_BACKSPACE   = 0x14;   // decimal 20
	unsigned char PKEY_SPACE       = 0xA0;  // 20
	//unsigned char PKEY_UP          = 7;   //                SHIFT is $653 or $654
	//unsigned char PKEY_LEFT        = 2;   // 
	//unsigned char PKEY_RIGHT       = 2;   // 
	//unsigned char PKEY_DOWN        = 7;   // 
	
	unsigned char PKEY_A           = 0xC1;  // 41
	unsigned char PKEY_B           = 0xC2;  // 42
	//unsigned char PKEY_C           = 20; 
	unsigned char PKEY_D           = 0xC4;  // 44
	//unsigned char PKEY_E           = 14;   
	unsigned char PKEY_F           = 0xC6;  // 46
	//unsigned char PKEY_G           = 26;
	//unsigned char PKEY_H           = 29;
	unsigned char PKEY_I           = 0xC9;  // 49
	unsigned char PKEY_J           = 0xCA;  // 4A
	unsigned char PKEY_K           = 0xCB;  // 4B
	unsigned char PKEY_L           = 0xCC;  // 4C
	// M 36
	// 39
	unsigned char PKEY_O           = 0xCF;  // 4F
	unsigned char PKEY_P           = 0xD0;  // 50
	//unsigned char PKEY_R           = 17;   // 'R'
	unsigned char PKEY_S           = 0xD3;  // 53
	// T 22
	//unsigned char PKEY_U           = 30;
	// V 31
	unsigned char PKEY_W           = 0xD7;  // 57
	unsigned char PKEY_X           = 0xD8;  // 58
	//unsigned char PKEY_Z           = 12;   // 'Z'
	unsigned char PKEY_0           = 0x00;

  unsigned char PKEY_1           = 0xB1;
	unsigned char PKEY_2           = 0xB2;
	unsigned char PKEY_3           = 0xB3;
	unsigned char PKEY_4           = 0xB4;
	unsigned char PKEY_5           = 0xB5;
	unsigned char PKEY_6           = 0xB6;
	unsigned char PKEY_7           = 0xB7;
	unsigned char PKEY_8           = 0xB8;
	/*
	unsigned char PKEY_9           = 32;
	*/
	
	unsigned char PKEY_NO_KEY      = 0x7F;   // Placeholder to indicate that NO key has been pressed

#elif TARGET_C64
  unsigned char g_joy = 0;

	unsigned char PKEY_RETURN      = 1;   // same as '\n'
	//unsigned char PKEY_BACKSPACE   = 0x14;   // decimal 20
	unsigned char PKEY_SPACE       = 60;   // decimal 32
	unsigned char PKEY_UP          = 7;   //                SHIFT is $653 or $654
	unsigned char PKEY_LEFT        = 2;   // 
	unsigned char PKEY_RIGHT       = 2;   // 
	unsigned char PKEY_DOWN        = 7;   // 
	
	//unsigned char PKEY_PLUS        = 0x2B;   // '+'
	//unsigned char PKEY_MINUS       = 0x2D;   // '-'
	//unsigned char PKEY_INSTDEL     = 0x14;   // marked "INST DEL" on 4016 keyboard (can use as a backspace)
	//unsigned char PKEY_LANGBRACKET = 0x3C;   // '<'
	//unsigned char PKEY_LEFT_ARROW  = 0x5F;   // marked '<-'
	//unsigned char PKEY_LSQBRACKET  = 0x5B;   // '['
	//unsigned char PKEY_EQUALS      = 0x3D;   // '='
	//unsigned char PKEY_LEFT_BRK    = 0x5B;   // bracket [
	//unsigned char PKEY_RIGHT_BRK   = 0x5D;   // bracket ]
	unsigned char PKEY_A           = 10;   // 'A'
	unsigned char PKEY_B           = 28;   // 'B'
	//unsigned char PKEY_C           = 20; 
	unsigned char PKEY_D           = 18;   // 'D'
	//unsigned char PKEY_E           = 14;   
	unsigned char PKEY_F           = 21;
	//unsigned char PKEY_G           = 26;
	//unsigned char PKEY_H           = 29;
	unsigned char PKEY_I           = 33;
	unsigned char PKEY_J           = 34;
	unsigned char PKEY_K           = 37;
	unsigned char PKEY_L           = 42;
	// M 36
	// 39
	unsigned char PKEY_O           = 38;
	unsigned char PKEY_P           = 41;   // 'P'
	//unsigned char PKEY_R           = 17;   // 'R'
	unsigned char PKEY_S           = 13;   // 'S'
	// T 22
	//unsigned char PKEY_U           = 30;
	// V 31
	unsigned char PKEY_W           = 9;   // 'W'
	unsigned char PKEY_X           = 23;   // 'X'                 3000: 0x18
	//unsigned char PKEY_Z           = 12;   // 'Z'
	unsigned char PKEY_0           = 35;

  unsigned char PKEY_1           = 56;
	unsigned char PKEY_2           = 59;
	unsigned char PKEY_3           = 8;
	unsigned char PKEY_4           = 11;
	unsigned char PKEY_5           = 16;
	unsigned char PKEY_6           = 19;
	unsigned char PKEY_7           = 24;
	unsigned char PKEY_8           = 27;
	/*
	unsigned char PKEY_9           = 32;
	*/
	
	unsigned char PKEY_NO_KEY      = 64;   // Placeholder to indicate that NO key has been pressed
	
#else
	
	// COMMODORE PET 4016 US KEYCODES (using GET_PKEY_VIEW, aka PEEK(166))
	unsigned char PKEY_RETURN      = 0x0D;   // same as '\n'
	//unsigned char PKEY_BACKSPACE   = 0x14;   // decimal 20
	unsigned char PKEY_SPACE       = 0x20;   // decimal 32
	//unsigned char PKEY_UP          = 0x38;   // '8'
	//unsigned char PKEY_LEFT        = 0x34;   // '4'
	//unsigned char PKEY_RIGHT       = 0x36;   // '6'
	//unsigned char PKEY_DOWN        = 0x32;   // '2'
	//unsigned char PKEY_PLUS        = 0x2B;   // '+'
	//unsigned char PKEY_MINUS       = 0x2D;   // '-'
	//unsigned char PKEY_INSTDEL     = 0x14;   // marked "INST DEL" on 4016 keyboard (can use as a backspace)
	//unsigned char PKEY_LANGBRACKET = 0x3C;   // '<'
	//unsigned char PKEY_LEFT_ARROW  = 0x5F;   // marked '<-'
	//unsigned char PKEY_LSQBRACKET  = 0x5B;   // '['
	//unsigned char PKEY_EQUALS      = 0x3D;   // '='
	//unsigned char PKEY_LEFT_BRK    = 0x5B;   // bracket [
	//unsigned char PKEY_RIGHT_BRK   = 0x5D;   // bracket ]
	unsigned char PKEY_A           = 0x41;   // 'A'
	unsigned char PKEY_B           = 0x42;   // 'B'
	// C 43
	unsigned char PKEY_D           = 0x44;   // 'D'
	//unsigned char PKEY_E           = 0x45;   
	unsigned char PKEY_F           = 0x46;
	//unsigned char PKEY_G           = 0x47;
	//unsigned char PKEY_H           = 0x48;
	unsigned char PKEY_I           = 0x49;
	unsigned char PKEY_J           = 0x4A;
	unsigned char PKEY_K           = 0x4B;
	unsigned char PKEY_L           = 0x4C;
	// M 4D
	// N
	unsigned char PKEY_O           = 0x4F;
	unsigned char PKEY_P           = 0x50;   // 'P'
	//unsigned char PKEY_R           = 0x52;   // 'R'
	unsigned char PKEY_S           = 0x53;   // 'S'
	// T 54
	//unsigned char PKEY_U           = 0x55;
	// V 56
	unsigned char PKEY_W           = 0x57;   // 'W'
	unsigned char PKEY_X           = 0x58;   // 'X'                 3000: 0x18
	//unsigned char PKEY_Z           = 0x5A;   // 'Z'
	unsigned char PKEY_0           = 0x30;
  unsigned char PKEY_1           = 0x31;
	unsigned char PKEY_2           = 0x32;
	unsigned char PKEY_3           = 0x33;
	unsigned char PKEY_4           = 0x34;
	unsigned char PKEY_5           = 0x35;
	unsigned char PKEY_6           = 0x36;
	unsigned char PKEY_7           = 0x37;
	unsigned char PKEY_8           = 0x38;
	
	unsigned char PKEY_NO_KEY      = 0xFF;   // Placeholder to indicate that NO key has been pressed
	
#endif

#ifdef TARGET_A2  
unsigned long main_loop_counter;  // Because the Apple ][ doesn't have a clock!
#endif

// Going to assume that every program needs at least one timer.  This "global_timer" is intended to be used to store the "now time".
// So programs would then define timers for additional things they need to monitor delta times for, relative to this global "now time".
Time_counter global_timer;  

unsigned long delta_time;

unsigned long delta_time_sec;  //< Use a long (4 bytes) since max delta seconds is 86400 (larger than 65535, so needs more than 16-bits)
unsigned long delta_time_ms;   //< Used to stores millisecond precision XX.9999

#ifdef TARGET_A2
  unsigned char g_pad_char = 176; // 176 == '0' (normal;  48 is 0 inverse for Apple ][)
#else
  unsigned char g_pad_char = 48;  // 48 == '0'
#endif

void WRITE_PU_DIGIT(unsigned char x, unsigned char y, unsigned long val, unsigned char pad)  // PU = pad unsigned
{	
	//VERSION 1:  you can only pad up to the max length in the table (i.e. 8 or 9), uses 90 more bytes - not sure if runs faster
	/*
	unsigned char index = 0;
	static unsigned long pad_value[] = {
		10,          // 0
		100,         // 1
		1000,        // 2
		10000,       // 3     FFFF =        65,535
		100000,      // 4
		1000000,     // 5
		10000000,    // 6   FFFFFF =    16,777,215  (max value of PET time)
		100000000,   // 7
		1000000000   // 8 FFFFFFFF = 4,294,967,295 
	};
	
	for (index = 0; index < sizeof(pad_value)/sizeof(unsigned long); ++index)
	{
		if (val < pad_value[index])
		{
			// index 0 means 1-digit
			// index 1 means 2-digit
			// index 2 means 3-digit
			// index 3 means 4-digit
			// index 4 means 5-digit
			
			// we need pad-index number of prefix 0
			if (g_pad_char != '\0')
			{
				pad = pad-(index+1);
				while (pad > 0)
				{
					WRITE_CHAR(x, y, g_pad_char);
					++x;					
					--pad;
				}
			}
			break;			
		}
	}	
	
	while (index > 0)
	{		    
		--index;
		WRITE_1U_DIGIT(x, y, val / pad_value[index]);
		val %= pad_value[index];
		++x;		
	}
	
	WRITE_1U_DIGIT(x, y, val);
	*/
	
	// VERSION 2: can pad up to any length; seems to uses less code-space
	unsigned char index = 0;
	unsigned long multi = 10;
		
	while (TRUE)
	{
		if (val < multi)
			break;
		++index;
		multi *= 10;
	}
	
	if (g_pad_char != '\0')
	{	
		pad = pad-(index+1);
		while (pad > 0)
		{
			WRITE_CHAR(x, y, g_pad_char);
			++x;					
			--pad;
		}
	}	

	while (index > 0)
	{		    
		--index;
		multi /= 10;
		WRITE_1U_DIGIT(x, y, val / multi);		
		val %= multi;
		++x;		
	}
	
	WRITE_1U_DIGIT(x, y, val);
}
	
void WRITE_STRING(unsigned char x, unsigned char y, const char* str, unsigned char str_len)
{
  while (str_len > 0)
	{		
		--str_len;
		
		WRITE_CHAR(x+str_len, y, str[str_len]);
	}
}	
