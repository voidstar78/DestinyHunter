#ifndef DH_01_DESTINY_H
#define DH_01_DESTINY_H

/*
An intro sequence to initialize several aspects of the game:

- the seed for the RNG engine (called "player affinity"). 

- "direction" of the player (sort of like a gender)

The direction is based on a 50/50 chance initially.   The direction effects certain visual aspects
of the game, but also sets the "direction" of the seed_counter (positive or negative).  Either direction
will eventually hit the natural "wrap around" of an integer-seed variable (16-bit).

But the initial selection  seeds the "MOD" value.  A counter is started - if player hits SPACE/KEY 
and the COUNTER%MOD==0, they get a "persistent" point (and "try again" to pass the current CHIME).  
If the number of RETRIES exeeds the MOD value itself,  the player gets a "blessing" point (and 
proceeds to the next CHIME).

SECRET:  After the first CHIME, the MOD value naturally "gravitates" towards 5.  This is a "natural" way
to let later CHIMES be more 50/50 AGAINs (i.e. increasing luck or decreasing luck).
The result of this is -- a low initial MOD is easier to get a BLESSING, and nearly impossible with
a high initial MOD.  But if the first CHIME doesn't give a BLESSING, it's still possible with the
subsequent tries.

It sounds complicated - but it's just a way to choose DIRECTION(gender) and SEED(rng) values.
While also being a way to make getting a BLESSING rare, and medium possibility of getting PERSISTENCE.
BLESSINGs should be one-time-use only bonuses, very hard to get.

PERSISTENCE will be like health potions.  But instead of the idea of literally carrying potions and drinking
them, PERSISTENCE will be like "will power" and used to restore health -- but "urge" to keep fighting.

*/
void determine_destiny();

#endif
