#ifndef DH_CORE_H
#define DH_CORE_H

/*

core.h/.c are CORE support functions that essentially any program running on
a Commodore system would need:  screen I/O, keyboard I/O, timer support.

All PEEK POKE addresses throughout were derived from the following resources:

www.commodore.ca/wp-content/uploads/2018/11/commodore_pet_memory_map.pdf 
www.zimmers.net/cbmpics/cbm/PETx/petmem.txt

Interesting highlights of interest:

ADDRESS  
DECIMAL  DESCRIPTION
141-143  Jiffy clock
151      which key down
152      shift press indicator
153-154  corrector clock
166      key image (seems identical to 151 in testing I did on my 4016 hardware)

  TIME	  008D-008F   141-143	    Real-Time Jiffy Clock (approx) 1/60 Sec		

          0099-009A   153-154     Jiffy clock correction: 623rd 1/60 sec
	                                does not increment time		


*/

// ================================================================ BEGIN ==
// BUILD CONFIGURATION
// -------------------------------------------------------------------------
// Since every program is basically expected to include core.h fairly immediately,
// we go ahead and use core.h as also the place to specify specific target build settings.

//#define QUICK_GAME           //< Used to pre-initialize main character and start on a particular stage
#define FINAL_BUILD
#define TARGET_C64
//#define TARGET_PET

// The following are optimizations intended for the cc65 compiler environment
//#pragma inline-stdfuncs (on)
#pragma static-locals (on)
#pragma register-vars (on)
// --------------------------------------------------------------------------
#define NDEBUG                  //< NO DEBUG - do a release/fast build (this should also disable any asserts)

#include <peekpoke.h>  //< Make standard PEEK and POKE macros available, doesn't consume any resources if not used
// ================================================================= END ===

// ================================================================ BEGIN ==
// ************* KEYBOARD CONSTANTS AND RELATED FEATURES
// -------------------------------------------------------------------------

#define C64_JOYSTICK_ADDRESS_1 56321U  // Control Port 1
#define C64_JOYSTICK_ADDRESS_2 56320U  // Control Port 2
#define C64_JOYSTICK_NONE     0x7F  //     0111 1111
#define C64_JOYSTICK_UP       0x01  // 254 1111 1110
#define C64_JOYSTICK_DOWN     0x02  // 253 1111 1101
#define C64_JOYSTICK_LEFT     0x04  // 251 1111 1011
#define C64_JOYSTICK_RIGHT    0x08  // 247 1111 0111
#define C64_JOYSTICK_BUTTON   0x10  // 239 1110 1111
#ifdef TARGET_C64
  extern unsigned char g_joy;
#endif

// COMMODORE PET 4016 graphicUS KEYCODES (these are codes when using kbhit, cgetc)
// SL 2021_04_18: 
//   kbhit and cgetc were too slow.  Replaced with GET_PKEY_VIEW
/*
#define KEY_UP_ARROW     0x91
#define KEY_LEFT_ARROW   0x9D
#define KEY_DOWN_ARROW   0x11
#define KEY_RIGHT_ARROW  0x1D
#define KEY_BACKSPACE    0x14
#define KEY_SPACE        0x20
#define KEY_SHIFT_SPACE  0xA0
#define KEY_ESCAPE       0x03     // RUN/STOP key (original PET doesn't have a key marked as ESC)
#define KEY_RETURN       0x0D
*/

// TO CONSERVE SPACE - only make declarations for the keys that are actually used by the program
// COMMODORE PET 4016 US KEYCODES (using GET_PKEY_VIEW, aka PEEK(166))
extern unsigned char PKEY_RETURN;   // same as '\n'
//unsigned char PKEY_BACKSPACE   = 0x14;   // decimal 20
extern unsigned char PKEY_SPACE;   // decimal 32
extern unsigned char PKEY_UP;   // '8'
extern unsigned char PKEY_LEFT;   // '4'
extern unsigned char PKEY_RIGHT;   // '6'
extern unsigned char PKEY_DOWN;   // '2'
//unsigned char PKEY_PLUS        = 0x2B;   // '+'
//unsigned char PKEY_MINUS       = 0x2D;   // '-'
//unsigned char PKEY_INSTDEL     = 0x14;   // marked "INST DEL" on 4016 keyboard (can use as a backspace)
//unsigned char PKEY_LANGBRACKET = 0x3C;   // '<'
//unsigned char PKEY_LEFT_ARROW  = 0x5F;   // marked '<-'
//unsigned char PKEY_LSQBRACKET  = 0x5B;   // '['
//unsigned char PKEY_EQUALS      = 0x3D;   // '='
//unsigned char PKEY_LEFT_BRK    = 0x5B;   // bracket [
//unsigned char PKEY_RIGHT_BRK   = 0x5D;   // bracket ]
extern unsigned char PKEY_A;   // 'A'
extern unsigned char PKEY_B;   // 'B'
//extern unsigned char PKEY_C;   // 'B'
extern unsigned char PKEY_D;   // 'D'
//unsigned char PKEY_E           = 0x45;   
extern unsigned char PKEY_F;
//unsigned char PKEY_G           = 0x47;
//unsigned char PKEY_H           = 0x48;
extern unsigned char PKEY_I;
extern unsigned char PKEY_J;
extern unsigned char PKEY_K;
extern unsigned char PKEY_L;
// M 4D
// N
extern unsigned char PKEY_O;
extern unsigned char PKEY_P;
// Q
//extern unsigned char PKEY_R;   // 'R'
extern unsigned char PKEY_S;   // 'S'
// T 54
//extern unsigned char PKEY_U;
// V 56
extern unsigned char PKEY_W;   // 'W'
extern unsigned char PKEY_X;   // 'X'                 3000: 0x18
//unsigned char PKEY_Z           = 0x5A;   // 'Z'
extern unsigned char PKEY_0;
extern unsigned char PKEY_1;
extern unsigned char PKEY_2;
extern unsigned char PKEY_3;
extern unsigned char PKEY_4;
extern unsigned char PKEY_5;
extern unsigned char PKEY_6;
extern unsigned char PKEY_7;
extern unsigned char PKEY_8;
extern unsigned char PKEY_NO_KEY;   // Placeholder to indicate that NO key has been pressed

// THESE ARE MACRO definitions, because these values are used to overwrite
// the default values that are initialized above in the .c file.  As such, I
// refer to these as the "B-series" keyboard codes, only because they are 
// not the DEFAULT (which would be the "A-series").  A/B-series is independent
// of which PET model is used.  Here, I have a DEFAULT (A) and then one ALTERNATE (B).
// I obtain these keycodes using a test program that executes GET_PKEY_VIEW in a loop,
// then run that test program using the VICE emulator with different Settings|Models.

// 30XX KEYBOARD CODES (seems identical to 20XX series per the PET 2001 online emulator)
#define B_PKEY_RETURN      0x1B
//#define B_PKEY_BACKSPACE   0x41
#define B_PKEY_SPACE       0x06
#define B_PKEY_UP          0x32   // '8'
#define B_PKEY_LEFT        0x2A   // '4'
#define B_PKEY_RIGHT       0x29   // '6'
#define B_PKEY_DOWN        0x12   // '2'
//#define B_PKEY_PLUS        0x11   // '+'
//#define B_PKEY_MINUS       0x09   // '-'
//#define B_PKEY_INSTDEL     0x04   // marked "INST DEL" on 4016 keyboard (can use as a backspace)
//#define B_PKEY_LANGBRACKET 0x05   // '<'
//#define B_PKEY_LEFT_ARROW  0x49   // marked '<-'
//#define B_PKEY_LSQBRACKET  0x07   // '['
//#define B_PKEY_EQUALS      0x01   // '='
//#define B_PKEY_LEFT_BRK    0x07   // bracket [
//#define B_PKEY_RIGHT_BRK   0x0E   // bracket ]
#define B_PKEY_A           0x30   // 'A'
#define B_PKEY_B           0x1E   // 'B'
// C == 1F
#define B_PKEY_D           0x2F   // 'D'
#define B_PKEY_E           0x3F   
#define B_PKEY_F           0x27
#define B_PKEY_G           0x2E
#define B_PKEY_H           0x26
#define B_PKEY_I           0x35
#define B_PKEY_J           0x2D
#define B_PKEY_K           0x25
#define B_PKEY_L           0x2C
// M 1D
// N 16
#define B_PKEY_O           0x3C
#define B_PKEY_P           0x34
// Q 40
//#define B_PKEY_R           0x37   // 'R'
#define B_PKEY_S           0x28   // 'S'
#define B_PKEY_T           0x3E
#define B_PKEY_U           0x3D
#define B_PKEY_V           0x17
#define B_PKEY_W           0x38   // 'W'
#define B_PKEY_X           0x18   // 'X'                 3000: 0x18
#define B_PKEY_0           0x0A
#define B_PKEY_1           0x1A
#define B_PKEY_2           0x12
#define B_PKEY_3           0x19
#define B_PKEY_4           0x2A
#define B_PKEY_5           0x22
#define B_PKEY_6           0x29
#define B_PKEY_7           0x3A
#define B_PKEY_8           0x32

#ifdef TARGET_C64
  #define GET_PKEY_VIEW PEEK(197)  
#else
  #define GET_PKEY_VIEW PEEK(166)  // works 4016
  //#define GET_PKEY_VIEW PEEK(151)  // seems to be identical; maybe there is some slight speed/buffer difference?
#endif
// ================================================================= END ===

// ================================================================ BEGIN ==
// ************* MATH/LOGIC RELATED FEATURES
// -------------------------------------------------------------------------

// BOOLEAN VALUES AND BIT-LOGIC SUPPORT
#define TRUE 1
#define FALSE 0

// BIT-MASK SUPPORT (0-7 OFFSET BASED)
#define SET_BIT(n, k) n = (n | (1 << (k)))
#define CLEAR_BIT(n, k) n = (n & (~(1 << (k))))
#define TOGGLE_BIT(n, k) n = (n ^ (1 << (k)))  
#define IS_BIT_ON(n, k) ((n & (1 << (k))) ? TRUE : FALSE)

// BIT-MASK SUPPORT (1-8 OFFSET BASED) (don't use these: why waste opcodes doing the "-1" adjustment computation)
//#define SET_BIT(n, k) n = (n | (1 << (k - 1)))
//#define CLEAR_BIT(n, k) n = (n & (~(1 << (k - 1))))
//#define TOGGLE_BIT(n, k) n = (n ^ (1 << (k - 1)))
//#define IS_BIT_ON(n, k) ((n & (1 << (k-1))) ? TRUE : FALSE)

// Using these MASK macros is even better, no shifting.
#define MASK_HIGH_BIT 0x80  // 1000 0000
#define IS_MASK_ON(mask, bit) \
  ((mask & bit) == bit)
#define IS_MASK_OFF(mask, bit) \
  ((mask & bit) != bit)	
#define SET_MASK(mask, bit) \
  (mask = (mask | bit))
#define CLEAR_MASK(mask, bit) \
  (mask = (mask & ~bit))	

// INTEGER ROUNDING to nearest WHOLE INTEGER (without use of float)
// cc65 at this time apparently doesn't support FLOAT types.  That's understandable.  These early processors didn't natively
// support floating point either (see "coprocessors", the first being Intel 8087 in 1980).  While BASIC did support floating 
// point, it was software emulated (so it's slow).  Historical note: Bill Gates' 
// original BASIC included a floating-point emulation.   When Wozniak cloned his own BASIC for the Apple I, he kept it 
// limited to INTEGERs only, hence calling it Integer BASIC.    Since Commodore was able to use Microsoft BASIC, this is 
// why the Commodore has that capability.  Wozniak found helpers to complete support for floats by the time he started 
// making the Apple ]['s.   For text-based graphics, FLOAT isn't generally not too essential.  But for financial and trig
// computations, FLOAT starts to become important.   Example:  $1.86 + $12.54   or COSINE(78deg)
//
// So for cc65 to support this, they'd have to emulate FLOAT -- and doing so in a "cross-platform" compatible manner would
// be difficult.  If we come across the need for FLOAT-type processing, we'll locate a third-party C library that likely has
// an emulation already implemented.  For example: github.com/oprecomp/flexfloat
//
// In this case, we needed a ROUND_DIVIDE when preparing the seed for the random number generator.  Not so much for the RNG
// itself, but to help align the result back to the correct screen column without extra code checks to adjust.
#define ROUND_DIVIDE(numer, denom) (((numer) + (denom) / 2) / (denom))

// Early on I was using vector functions that used malloc/calloc and free.  I abandoned those functions,
// since that did have slight overhead related to managing the vectors RESERVE.  However, I found it
// useful to keep this VEC_GET style of the original function, to convey the intent that I am
// "getting" an entry out of a vector-array.
#define VEC_GET(a,b) &a[b]
// ================================================================= END ===

// ================================================================ BEGIN ==
// ************* TIME RELATED FEATURES
// -------------------------------------------------------------------------

#define JIFFIES_FULL_SECOND       60       // 60/1   aka Jiffies per Second
#define JIFFIES_HALF_SECOND       30       // 60/2 
#define JIFFIES_THIRD_SECOND      20       // 60/3
#define JIFFIES_QUARTER_SECOND    15       // 60/4
#define JIFFIES_EIGTH_SECOND       7       // 60/8   should be 7.5, we'll TRUNC instead of ROUND
#define JIFFIES_TWELTH_SECOND      5       // 60/12  
#define JIFFIES_FIFTEENTH_SECOND   4       // 60/15
#define JIFFIES_SIXTEENTH_SECOND   3       // 60/16  should be 3.75, we'll TRUNC instead of ROUND
#define JIFFIES_THIRTIETH_SECOND   2       // 60/32  should be 1.875, ROUNDING this time just to distinguish from SIXTIETH
#define JIFFIES_SIXTIETH           1       // 60/60

typedef struct
{
	//unsigned char a;          //< PET TIME is 3 byte (not 4), so this high byte isn't needed - but we want to align/union with an unsigned long type
	//unsigned char b;          //< addr 141
	//unsigned char c;          //< addr 142
	//unsigned char d;		      //< addr 143
	// ^---- I tried to work this into a UNION with the "unsigned long total" defined below.
	//       To be brief, it didn't work out -- I think there is some memory alignment deal that I didn't fully understandable
	//       at the time.  I read there are some other non-portable aspects about unions, so for now I'm just avoiding it.
	// 

	unsigned char corrector;    //< addr 154
	
	union 
	{
    unsigned long   totl;        //< addres 141-143 (high byte isn't used)
		
		struct Data
		{
			//            totl
		  unsigned char    a;
		  unsigned char   b;
		  unsigned char  c;
		  unsigned char d;          //< this high byte is not used
		} data;
	};
} Time_counter;

// Going to assume that every program needs at least one timer.  This "global_timer" is intended to be used to store the "now time".
// So programs would then define timers for additional things they need to monitor delta times for, relative to this global "now time".
// This probably should be called "global_now_timer" to be more clear about that.  But there may be situation where it is not used
// for "now", so, keeping it as just global_timer.
extern Time_counter global_timer;  

extern unsigned long delta_time;      //< Assuming the purpose of a timer is to eventually need to compare delta-elased time; this declaration is standardized for that purpose.	
extern unsigned long delta_time_sec;  //< Max jiffies in 3 bytes is 16777215, so we can represent up to 279620 seconds (which needs 2.5 bytes, just going to use 4-bytes to keep it aligned)
extern unsigned long delta_time_ms;   //< Instead of wasting that information when DIV jiffy count by 60, we can get 3-digits of milliseconds.  This is used to hold that.

/*
// The following is technically more correct, but only matters after hours and hours of runtime.  If this is a short-runtime
// program, save the bytes and just use the NO_CORRECTOR version.
#define STORE_TIME(target_timer) \
  POKE(&target_timer.data.c, PEEK(141));  \
  POKE(&target_timer.data.b, PEEK(142));  \
  POKE(&target_timer.data.a, PEEK(143));  \
  target_timer.corrector = PEEK(154);	         \
  target_timer.totl += target_timer.corrector;
*/

// WARNING: STORE_TIME_NO_CORRECTOR does not initialize target_timer.data.d to 0.
#ifdef TARGET_C64
	#define STORE_TIME_NO_CORRECTOR(target_timer)  \
		POKE(&target_timer.data.c, PEEK(160));  \
		POKE(&target_timer.data.b, PEEK(161));  \
		POKE(&target_timer.data.a, PEEK(162));
#else
	#define STORE_TIME_NO_CORRECTOR(target_timer)  \
		POKE(&target_timer.data.c, PEEK(141));  \
		POKE(&target_timer.data.b, PEEK(142));  \
		POKE(&target_timer.data.a, PEEK(143));
#endif  
  
/*
Alternative time update (using individual b, c, d unsigned char, not the "unsigned long" union overlay):
  started.b = PEEK(141); 
	started.c = PEEK(142); 
	started.d = PEEK(143);
	started.total = 
	  ((unsigned long)started.b << 16) | ((unsigned long)started.c << 8) | (unsigned long)started.d;
	started.total += started.corrector;
*/

#define INIT_TIMER(target_timer) \
  POKE(&target_timer.data.d, 0);  /* Necessary only if trying to access .totl */

#define UPDATE_DELTA_JIFFY_ONLY(now_time, start_time) \
	delta_time = (now_time.totl - start_time.totl);

#define UPDATE_DELTA_TIME_FULL(now_time, start_time) \
	delta_time = (now_time.totl - start_time.totl);  \
	delta_time_sec = delta_time / 60; \
	delta_time_ms  = ((((delta_time - (delta_time_sec * 60UL)) * 1000UL) / 60UL) );	  // 3-digit precision
	//delta_time_ms  = ((((delta_time - (delta_time_sec * 60UL)) * 1000UL) / 60UL) / 10UL);   // 2-digit precision
	//delta_time_ms  = ((((delta_time - (delta_time_sec * 60UL)) * 1000UL) / 60UL) / 100UL);  // 1-digit precision
// ================================================================= END ===	

// ================================================================ BEGIN ==
// ************* VIDEO RELATED FEATURES
// -------------------------------------------------------------------------

// Technically only available on C64, but macros take no space until they
// are used.  Since the PET version won't be using them, no real reason to
// wrap these macros in a TARGET_C64 ifdef.
#define C64_COLOR_BLACK  0x00U
#define C64_COLOR_WHITE  0x01U
#define C64_COLOR_RED    0x02U
#define C64_COLOR_CYAN   0x03U
#define C64_COLOR_PURPLE 0x04U
#define C64_COLOR_GREEN  0x05U
#define C64_COLOR_BLUE   0x06U
#define C64_COLOR_YELLOW 0x07U
#define C64_COLOR_ORANGE 0x08U
#define C64_COLOR_BROWN  0x09U
#define C64_COLOR_PINK   0x0AU
#define C64_COLOR_DGREY  0x0BU
#define C64_COLOR_GREY   0x0CU
#define C64_COLOR_LGREEN 0x0DU
#define C64_COLOR_LBLUE  0x0EU
#define C64_COLOR_LGREY  0x0FU

// The following is used to clarify which "algorithm" processing is screen size
// related (by searching the code for this name).  Changing the value doesn't
// necessarily mean everything automatically adjusts accordingly, but it is a
// keyword to help start with places to examine that may be impacted.
// This may need to become a variable in the future, to help dynamically adjust
// on the fly to 40 vs 80 column machines.  But for the moment, it is a build-specific
// macro definition.
#define WIDTH_OF_SCREEN 40
#define HEIGHT_OF_SCREEN 25

#ifdef TARGET_C64
  #define BASE_SCREEN_ADDRESS 0x0400
	#define BASE_COLOR_ADDRESS  0xD800
#else
	#define BASE_SCREEN_ADDRESS 0x8000
#endif

#ifdef TARGET_C64
  #define ADDR_CHARSET 53272U
	#define ENABLE_CHARACTER_SET_A \
		POKE(ADDR_CHARSET,21);
	#define ENABLE_CHARACTER_SET_B \
		POKE(ADDR_CHARSET,23);
#else
	#define ADDR_CHARSET 59468U
	#define ENABLE_CHARACTER_SET_A \
		POKEW(ADDR_CHARSET,12);
	#define ENABLE_CHARACTER_SET_B \
		POKEW(ADDR_CHARSET,14);
#endif

// VIDEO MODE CHANGES
#define ENABLE_GRAPHIC_MODE  \
  __asm__("lda #$8E");  \
	__asm__("jsr $ffd2");
#define ENABLE_TEXT_MODE  \
  __asm__("lda #$0E");  \
	__asm__("jsr $ffd2");
	
#define CLRSCR \
	__asm__("lda #$93"); \
	__asm__("jsr $ffd2");
	
// Obsolete - kept for reference only (alternate to the above __asm__)
//#define ENABLE_GRAPHIC_MODE printf("\x8E")  // CHR(142)  aka UPPER case, font is closer together to connect symbols
//#define ENABLE_TEXT_MODE    printf("\x0E")  // CHR(14)   aka LOWER case

// Obsolete - kept for reference only (handled instead by direct POKE to screen)
//#define ENABLE_REVERSE_MODE printf("\x12")  // seems to enable the high bit (REVERSE) of any subsequent character output
//#define ENABLE_REGULAR_MODE printf("\x92")  // disables REVERSE and clears the high bit of any subsequent character output

#define WRITE_CHAR(x,y,ch) \
  POKE(BASE_SCREEN_ADDRESS+(WIDTH_OF_SCREEN*(y))+x, ch)
	
#define READ_CHAR(x,y) \
  PEEK(BASE_SCREEN_ADDRESS+(WIDTH_OF_SCREEN*(y))+x)

// "val" must always be <10 (i.e. 0..9, single digit only)
#define WRITE_1U_DIGIT(x,y,val) \
  WRITE_CHAR(x,y, 48 + val);

extern unsigned char g_pad_char;  //<  Set to '\0' or 0 for NO padding, otherwise set to what you want to use as the LEFT-SIDE padding (typically 48 == '0')
// Write the binary encoded value "val" at x,y converted to decimal.  If g_pad_char is NO-ZERO,
// then left-pad by the "pad" number of characers.
void WRITE_PU_DIGIT(unsigned char x, unsigned char y, unsigned long val, unsigned char pad);

void WRITE_STRING(unsigned char x, unsigned char y, const char* str, unsigned char str_len);

// ================================================================= END ===
	
// ================================================================ BEGIN ==	
// ************* AUDIO FEATURES	
// -------------------------------------------------------------------------

// Per Basic 4.0 Programming for the Commodore PET  shift register and control register should be set to 15 and 16 respectively
#define AUDIO_TURN_ON \
  POKEW(59467U,16);

#define AUDIO_TURN_OFF \
  POKEW(59467U,0);
	
#define AUDIO_SET_OCTAVE(octave) \
  POKEW(59466U,octave);
	
#define AUDIO_SET_FREQUENCY(freq) \
  POKEW(59464U,freq);
	
// COMMAND TO MAKE THE PET SPEAKER BEEP.
// May find a more efficient way in the future.  "\a" means audible alert.
// In other conventions this is called BEL or CTRL-G or ASCII 7.  You can still do this
// at the Windows command prompt, press CTRL-G (you will see "^G") and press ENTER.  It's an 
// invalid command, but the console will interpret it as an audible alert and issue the
// standard alert-audio of the system.
#define DO_BEEP printf("\a")
// ================================================================= END ===

#endif
