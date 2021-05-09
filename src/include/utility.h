#ifndef DH_UTILITY_H
#define DH_UTILITY_H

// Since the system can only poll for keyboard input at one time, 
// this is a globally re-uable storage of that input.
extern unsigned char global_input_ch;  

extern unsigned char g_i;  //< Global loop integer, only use once at a time, not nest-able
extern unsigned char g_enter_result;  //< To be used when calling wait_for_ENTER

// Uses rand() to return a random value between 0 and N-1 inclusive; this function exists
// to avoid excessively repeating opcode instructions necessary for the MOD(%) operator.
unsigned char rand_mod(unsigned char n);

// Wait for the PKEY_VIEW to return to the value FFh, which ensures
// all keys have been released.  Since using GET_PKEY_VIEW can be
// a little bit sensitive or "touchy", waiting for a flush can help
// accidentally spamming keyboard inputs (generally used in transitions
// from one scene to another).
void flush_keyboard_buffer();

// A utility function to explicitly wait for the ENTER/RETURN key to be pressed.
// However, there is one slight feature: the user can press "X" and cause this function
// to return TRUE (otherwise when pressing RETURN, FALSE is returned instead).
// The calling function can then interpret the "X" in a custom way, such as to
// abort the current activity or "go back".
//
// This function has one other purpose: it can be used to detect between 40XX 
// and 30XX/20XX PETs.  The value of the RETURN key code varies across these models.
// Within this code, the 30XX/20XX are referred to as the "B"-series keyboards,
// even though they were built first.  I couldn't call them 3020_KEY since declarations
// in C can not start with digits.
//
// We default to assume/expecting a 40XX code - but if we instead get a 30XX/20XX code,
// we then initialize all the other key values into the B-series codes (so that on
// all subsequent keyboard queries, the B-series becames the "natural"/default preference
// from there on.  So conserve space, we should only do so for keys that the program
// actually uses.
unsigned char flush_keyboard_and_wait_for_ENTER();

// These STYLE option match the DIRECTION values, though they don't have to.
// These are simply a way to indicate different styles of banners.
#define BANNER_STYLE_1 0  // corresponds to FORWARD_YONI_BLACK
#define BANNER_STYLE_2 1  // corresponds to BACKWARD_LINGA_WHITE
#define BANNER_STYLE_3 2  // corresponds to SOMETHING_ELSE   (alternate option, generally for "press return" prompts)


// This function initializes the border characters used in the different
// style options for banners.
#define buffer_ch_T      0
#define buffer_ch_B      1
#define buffer_ch_TL     2
#define buffer_ch_TR     3
#define buffer_ch_BL     4
#define buffer_ch_BR     5
#define buffer_ch_LEFT   6
#define buffer_ch_RIGHT  7
#define buffer_ch_FILLER 8
#define buffer_ch_COUNT  9

// to invert the banner
void INVERT_BANNER_STYLE(unsigned char x);

// A banner displays the provided "text_to_say" but with a certain border style
// around it (see BANNER_STYLE_XXX constants).  The banner is slightly wider than
// the text, as defined by the BANNER_SPACE_ADJUSTMENT (the value is for 1 side,
// but the adjustment will be applied equally to both sides of the text).
//
// NOTE this function uses printf in case the banner characters are REVERSE mode.
void text_banner_len(unsigned char x, unsigned char y, const char* text_to_say, unsigned char str_len, unsigned char style);

// Draws a banner such that the text_to_say is center on the given row y,
// likewise using the given style-tyle.
void text_banner_center(unsigned char y, const char* text_to_say, unsigned char style);

// This function is used to print strings in a "story" type fashion,
// where there is a slight delay between the printing of each character.
// Instead of just a simple delay, during that delay a random set of
// symbols is projected, as if magic is preparing the next character
// to be displayed, hence a "fancy" way to output a string.
//
// The delay should be one of the JIFFIES_xx_SECOND constants.  The
// actual delay is RNG up to that number of delay, in jiffies.
//
// The user can press space to "speed read" through the text.
//void print_fancy(unsigned char x, unsigned char y, const char* temp_str, unsigned char delay);

#endif
