#include <destiny_structs.h>

Destiny_status global_destiny_status;

Persona_status g_pvec_personas[MAX_PERSONAS_TO_SELECT];
unsigned char g_pvec_personas_count = 0;
Persona_status* g_ptr_persona_status;	

Challenge challenges[MAX_CHALLENGES_PER_STAGE];
unsigned char challenges_count = 0;

#if defined(TARGET_A2)

//                            F#    G   G#    A   A#    B    C   C#    D   D#    E    F   F#    G   G#    A
unsigned char audio_frq[] = {247, 233, 220, 207, 195, 184, 174, 164, 155, 146, 137, 130, 122, 115, 109, 102};
                            //                               1        2         3     4        5

#else
/*
// From https://gist.github.com/matozoid/18cddcbc9cfade3c455bc6230e1f6da6
unsigned int audio_freq[] = {
  7217,   // 0   A
	7647,   // 1   A#
	8101,   // 2   B
	8583,   // 3   C
	9094,   // 4   C#
	9634,   // 5   D
	10207,  // 6   D#
	10814,  // 7   E
	11457,  // 8   F
	12139,  // 9   F#
	12860,  // 10  G
	13625,  // 11  G#
	14435,  // 12  A
	15294,  // 13  A#
	16203,  // 14  B
	17167,  // 15  C
	18188,  // 16  C#
	19269,  // 17  D
	20415,  // 18  D#
	21629,  // 19  E
	22915,  // 20  F
	24278,  // 21  F#
	25721,  // 22  G
	27251,  // 23  G#
	28871,  // 24  A
	30588   // 25  A#
};		
*/
//unsigned char audio_octv[] = 
//                           { 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51};
unsigned char audio_frq0[] = {140,133,125,118,110,104, 99, 93, 88, 83, 78, 74, 69, 65, 125,118,110,104, 99, 93, 88, 83, 78, 74, 69, 65};
//                              0   1   2   3   4   5   6   7   8   9  10  11  12  13   14  15  16  17  18  19  20  21  22  23  24  25
//                              A   A#  B   C   C#  D   D#  E   F   F# G   G#  A   A#   B   C   C#  D   D#  E   F   F#  G   G#  A   A#
#endif

// GAME OVER
//                                                          3                  2            1 
//                                                  6                  5                4
//                                          9                   8              7

// END GAME
//                                          1                          2                    3               4
//                                      5                       6                       7           8
//                              9                           10                 11           12
