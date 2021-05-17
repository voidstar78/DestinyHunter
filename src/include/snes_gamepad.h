#ifndef SNES_GAMEPAD_H
#define SNES_GAMEPAD_H

// STEVE LEWIS, 2021 - converted for use in cc65, adapted from BASIC PRG provided by Joe Davis

/*******************************************************************************
WARNING: This function writes to $033A-$033C.
WARNING: Be sure to compile with optimization on (or else PEEKPOKE may make unnecessary usage of the x-register)
$033A,$033B hold the RESULT (support up to 16 buttons),  $033C is used to help assist in the polling

No real reason to change from using $033A-$033C to store output.  Nearly all of address $274-$3E5 is 
available on most PETs, since they used for cassette buffers.  As long as your program doesn't actively 
save/load to the cassette while running, and no other code is using those addresses for other user-port 
purposes, then you're fine.  

In main-loop, call read_gamepad() to update the button states, then examine them as follows:
    g_pad1 = PEEK(0x033A);   // declare g_pad1 as unsigned char
		g_pad2 = PEEK(0x033B);   // declare g_pad2 as unsigned char
Example:
  while (TRUE) {
    read_gamepad();
    if ((g_pad1 & GPAD_BUTTON_START) == GPAD_BUTTON_START) goto they_pushed_start;
	}
they_pushed_start:
		
$033A              $033B
================================
TOP RIGHT    1     START    1
TOP LEFT     2     SELECT   2
X            4     Y        4
A            8     B        8
RIGHT       16
LEFT        32
DOWN        64
UP         128
***********************************************************************************/
#define GPAD_NUM_BUTTONS 12   // NOTE: max 16, more than that will require using another byte to hold the next set of 8 (and Y would start 2, etc.)
// $033A (GPAD_RESULT_A)
#define GPAD_BUTTON_TRIGHT_MASK   1    // 0000 0001
#define GPAD_BUTTON_TLEFT_MASK    2    // 0000 0010
#define GPAD_BUTTON_X_MASK        4    // 0000 0100
#define GPAD_BUTTON_A_MASK        8    // 0000 1000
#define GPAD_BUTTON_DPAD_RIGHT   16    // 0001 0000
#define GPAD_BUTTON_DPAD_LEFT    32    // 0010 0000
#define GPAD_BUTTON_DPAD_DOWN    64    // 0100 0000
#define GPAD_BUTTON_DPAD_UP     128    // 1000 0000
// $033B (GPAD_RESULT_B)
#define GPAD_BUTTON_START_MASK    1    // 0000 0001
#define GPAD_BUTTON_SELECT_MASK   2    // 0000 0010
#define GPAD_BUTTON_Y_MASK        4    // 0000 0100
#define GPAD_BUTTON_B_MASK        8    // 0000 1000
// -----------
#define GPAD_RESULT_A        0x033A
#define GPAD_RESULT_B        GPAD_RESULT_A + 1  //< consecutively after RESULT_A

void read_gamepad();

#endif
