#include <destiny_structs.h>

Destiny_status global_destiny_status;

Persona_status g_pvec_personas[MAX_PERSONAS_TO_SELECT];
unsigned char g_pvec_personas_count = 0;
Persona_status* g_ptr_persona_status;	

Challenge challenges[MAX_CHALLENGES_PER_STAGE];
unsigned char challenges_count = 0;

