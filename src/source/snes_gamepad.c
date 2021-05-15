#include <snes_gamepad.h>

#include <peekpoke.h>  //< Uses standard PEEK,POKE macros defined in the cc65 standard library - these use C pointer syntax to read/write directly to addresses

void read_gamepad()
{
	static unsigned char a;
	static unsigned char x;
	static unsigned char y;
	// INIT/SETUP                        // NOTE: use of "reg" below is just notional (it's end up adjusting to however a,x,y are scoped and declared)
	POKE(0xE843, 0x28);                  // $E843 = 0010 1000                          (mask to init I/O directions on user-port)
	POKEW(GPAD_RESULT_A, 0);             // $033A = 0, $033B = 0	                     (assume no buttons pressed)
	POKE(GPAD_POLL_MASK, 0x08);          // $003C = binary 0000 1000	                 (prepare to poll 4x times)
	x = GPAD_NUM_BUTTONS;                // reg X = 12                                 (used for iteration;  NOTE: 12 buttons, 4+4+2+2)
	y = 1;                               // reg Y = 1	                                 (button results will initially be written to $33B)
	// PULSE USER-PORT (GAMEPAD) LATCH
	POKE(0xE841, 0x20);                  // $E841 = 0010 0000                          (trigger user-port to give input)
	POKE(0xE841, 0x00);                  // $E841 = 0000 0000	
target3:
	a = PEEK(0xE841) & 0x40;             // reg A = $E841 & (0100 0000)                (poll user-port)
	if (a == 0x40) goto target1;         // if (reg A == (0100 0000)) goto target1     (any data response on user-port? if not, skip...)
	a = PEEK(GPAD_POLL_MASK) | PEEK(GPAD_RESULT_A + y); // reg A = 0x033C              (turn on bit corresponding to the button pressed)
	POKE(GPAD_RESULT_A + y, a);          // $033A + reg Y = reg A	                     (reg Y will be either 1 or 0, so write into either $33A or $33B)
target1:		
	a = (PEEK(GPAD_POLL_MASK) >> 1);     // reg A = ($033C >> 1)                       (shift right 1-bit)
	if (a != 0) goto target2;	           // if (reg A != 0) goto target2               (initial 0x08 setup has shifted off)
	--y;                                 // decrement reg Y                            (from 1 to 0; migrates to writing results to $33A)
	a = 0x80;                            // reg A = binary 1000 0000                   (prepare to poll 8x more times)
target2:	
	POKE(GPAD_POLL_MASK, a);             // $033C = reg A                              (store which bit we're on back into $33C)
	// PULSE USER-PORT (GAMEPAD) CLOCK
	POKE(0xE841, 0x08);                  // $E841 = binary 0000 1000
	POKE(0xE841, 0x00);	                 // $E841 = binary 0000 0000
	--x;                                 // decrement reg X  (12...11...10...etc)
	if (x != 0) goto target3;            // if (x != 0) goto target3
}
