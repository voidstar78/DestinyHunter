
INTRODUCTION
============

*Destiny Hunter* is a tribute-game made in *2021* for the Commodore PET (the original personal computer from 1977 that was based on the 6502 microprocessor).  32K system RAM is required, the game can be loaded by cassette or disk (or use of emulators).   The game was written in C and was also an exercise in using the cc65 compiler.   Release builds are available in the *BIN* folder: dhunter_final.zip is a complete package (with TXT file instructions and both PET and C64 PRG files), or access the individual PRG files.  [DHUNTERPET.PRG for Commodore PET] [DHUNTERC64.PRG for Commodore C64]

As a tribute-game, DH is intentionally short, about 5-10 minutes long (but maximizes the full 32K available of the PET system).  The C64 version is functionally equivalent and only adds the addition of color.


HOW TO PLAY
===========
When the game starts, press any key to decide a DIRECTION and ring CHIMES.  These are explained in a moment. Eventually you will be asked to press RETURN to continue (or press X to choose DIRECTION and CHIMES again).

DIRECTION is mostly cosmetic (e.g. character will be FILLED circle or HOLLOW circle).  CHIMES offer gifts of BLESSINGS or PERSISTENCY at the start of the game.  These are not essential, but having them will help reaching the end easier.  (BLESSINGS slow down the creatures, while PERSISTENCY recover HEALTH)

NOTE: Internally, DIRECTION and CHIMES are being used to initialize the random number generator!  It really *doing stuff* :)

After the CHIME section, you then select a PERSONA.    Press 1-6 to choose.  For new players, I suggest #6 SIDON (good health and defense).  Choice #1 is the expected "normal" difficult.   #4 RUDRA is a good choice also, as he carries the most arrows.    #3 CYRENE is probably the hardest difficulty.  Press RETURN to select and begin the game.

On the first map, you start empty handed!  The story is that you are in a dream and your chosen PERSONA (your ancestor hunter) has told you to begin your training at this island.  Therefore, you are starting along a beach.   So to begin, collect the BOW and ARROWS that you see randomly located along the beach.  That is the "adventure" part of the game: observe and find them (they are not far :) )

When you defeat creatures, you will automatically gain some PERSISTENCY, and have the option to collect ARROWS from those creatures.  But the CHIMES at the beginning is the only way to get BLESSINGS.



Once you do so, then you will begin the game, which requires you to defeat CHALLENGES that appear by aiming and firing arrows.  Once all the CHALLENGES are defeated, a notice will display indicating to press F to FINISH that stage and proceed to the next one. 


If using real/physical Commodore hardware, both versions support the SNES GAMEPAD adapter that is available here:
https://texelec.com/product/snes-adapter-commodore/


Thank You for visiting!
voidstar


PET CONTROLS
============

(if playing on a laptop that has no NUMPAD, I apologies for the inconvenience - the VICE emulator has provisions for remapping keys; you may consider trying the C64 version instead)

SPACE    = FIRE ARROW  (must have some arrows collected.  When run out of arrows, spirits will gift another set - but you must go collect them)

W/A/S/D  = MOVEMENT UP/LEFT/DOWN/RIGHT

NUMPAD
4/6      = AIM BOW LEFT/RIGHT
           (first acquire bow on first map)
					
NUMPAD 0 = USE PERSISTENCY
            (PERSISTENCY is like a healing potion, used
						one PERSISTENCY to recover back 1 HEALTH)
						
NUMPAD 8 = "FLIP" BOW (180 degrees)
           This is earned after about 200 steps (movements).
					 Use this to quickly aim in the opposite direction.

B        = USE A BLESSINGS
           These are gifted from the spirit, and they slow down
					 all the current creatures on the map (by roughly a factor of 2).
					 
O        = ACTIVE THE ORB
           There is only one ORB in the game.  It is quite visible,
					 but I leave it as a surprise to players to find it.
					 You may only activate it once!  It will basically give you
					 unlimited STAMINA.

P       = PAUSE (press RETURN to resume)


				
C64 CONTROLS
============
Since the C64 has no NUMPAD, the only difference in the controls
for the C64 is:

J/L     = AIM BOW LEFT/RIGHT

I       = "FLIP" BOW
K       = USE PERSISTENCY

(all other keys remain the same as the PET version)

NOTE: The C64 version does support the C64 JOYSTICK in PORT #2,
which is also supported by VICE emulator (I use a NES USB gamepad).
JOYSTICK controls will take a little getting used to.  

MOVEMENT-MODE
D-PAD    = MOVEMENT  (when not HOLDING the button)
BUTTON 1 = FIRE ARROW

ACTION-MODE
HOLD BUTTON 1

When in ACTION-MODE:
D-PAD LEFT/RIGHT = AIM BOW
D-PAD UP         = USE PERSISTENCY
D-PAD DOWN       = FLIP BOW
D-PAD DOWN (2x)  = CANCEL FIRING ARROW 
