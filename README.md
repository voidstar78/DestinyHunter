
INTRODUCTION
============

www.destinyhunter.org

*Destiny Hunter* is a tribute-game made in *2021* for the Commodore PET (the original personal computer from 1977 that was based on the 6502 microprocessor).  32K system RAM is required, the game can be loaded by cassette or disk (or use of emulators).   The game was written in C and is also an exercise in using the cc65 compiler.  It is also hoped that the code-baseline here can be a reference for future C base projects on the upcoming Commander 16 system!   

Release builds are available in the *BIN* folder: dhunter_final.zip is a complete package (with TXT file instructions and both PET and C64 PRG files), or you may access the individual PRG files directly if you prefer.  [DHUNTERPET.PRG for Commodore PET] [DHUNTERC64.PRG for Commodore C64]

As a tribute-game, DH is intentionally standalone and short (to support 20XX PET tape-based systems), about 5-10 minutes long (though maximizes the full 32K available of the PET system).  The C64 version is functionally equivalent and only adds the addition of color (i.e. this is a PET game that happens to run also on the C64, but clearly does not take advantage of the extra potential of the C64 system - but it does demonstrate that commonality of the PETSCII symbol set between the two systems).

Thank You for visiting!  Chuck Peddle, the electrical engineer creator of the highly affordable 6502 processor and KIM-1 single-board computer (that became the Commodore PET), passed away in 2019.  So as a tribute to the origin of home/office personal computers, this game is dedicated to the amazing hardware capability that was introduced to the world in that decade of the 1970s - and recognition that we are the last generation that knew the world before personal computers.  Just as we are curious to know exactly "how it happened" 2000 years ago, centuries from now historians may want to know "how it happened" about the Computer Age - what was it like to be non-networked, and how was this new-media of Software introduced to the world.  DH won't answer these questions, but I will be providing various thoughts along these lines at links available in the "reference" section of the DH blog:  www.destinyhunter.org 

v/r - voidstar, May 2021


HOW TO PLAY
===========
When the game starts, press any key to decide a DIRECTION and ring CHIMES.  These are explained in a moment. After you "press any key" for several times in a row, eventually you will be asked to press RETURN to continue (or press X to choose DIRECTION and CHIMES again).

DIRECTION is a cosmetic attribute (e.g. character will be FILLED circle {LINGA} or HOLLOW circle {YONI} based on the result).  CHIMES offer gifts of BLESSINGS or PERSISTENCY at the start of the game.  These are not essential, but having them will help reaching the end-game easier.  (BLESSINGS slow down the creatures, while PERSISTENCY recover HEALTH)

NOTE: Internally, DIRECTION and CHIMES are being used to initialize the random number generator!  It really is *doing stuff* :)

After the CHIME section, you then select a PERSONA.    Press 1-6 to choose.  For new players, I suggest #6 SIDON (good health and defense) to get familiar with the controls.  Choice #1 is the expected "normal" difficult.   #4 RUDRA is a good choice also, as he carries the most arrows.    #3 CYRENE is probably the hardest difficulty.  Press RETURN to select and begin the game.

On the first map, you start empty handed!  The story is that you are in a dream and your chosen PERSONA (your ancestor hunter) has told you to begin your training at this island.  Therefore, you are starting along a beach (observe that moving in the WATER will consume more STAMINA and slow you down faster).  The game is in a TOP-DOWN perspective (like the original Zelda was).

To begin, use the MOVEMENT keys (WASD) to collect the BOW and ARROWS that you see randomly located along the beach.  That is the "adventure" part of the game: observe and find them (they are not far!).  Once you are armed, then the game begins as the first CHALLENGES will enter the STAGE.   When you defeat creatures (by firing ARROWS from the BOW), you will automatically gain some PERSISTENCY, and have the option to collect ARROWS from those creatures.  But the CHIMES at the beginning is the only way to get BLESSINGS.

Once all the CHALLENGES are defeated, a notice will display indicating to press F to FINISH that stage and proceed to the next one. 


Some more tips:

- Your number of ARROWS (up arrow) and remaining HEALTH (heart) is along the bottom of the STAGE map.  

- CHALLENGES can attack you up to "two spaces" away, so you do want to evade them (CHALLENGES will chase you, depending on your STEALTH attributes).    

- Use PERSISTENCY to recover health.  The screen will "flicker" when you are HIT (and your health will drop by 1).

- When you HIT a challenge, their health will also reduce by 1.  The number of health of the last CHALLENGE you hit is displayed at the TOP RIGHT next to the name of the STAGE you are on.

- STAGE 4 is an "optional stage" that is a "dream within a dream."  This means you can't die on STAGE 4, and your original health before entering the stage is restored upon exit.  If you play well, STAGE 4 can be used to gather up a lot of arrows and PERSISTENCY.   STAGE 4 is entered by collecting the Divinity Gem at the center of the island, and then using that GEM to read the BOOK that appears (which transports you to Divinity Island).  NOTE: Just by reading the book, you gain a BLESSING!

- If your health reaches 0 (on any STAGE besides the 4th one), you will get G.O. (Game Over), press RETURN to try again from the beginning.


If using real/physical Commodore hardware, both versions support the SNES GAMEPAD adapter that is available here:
https://texelec.com/product/snes-adapter-commodore/




PET CONTROLS
============

NOTE: If playing on a laptop that has no NUMPAD, I apologies for the inconvenience (you might consider trying the C64 version instead, since the C64 has no KEYPAD either; or try some "remapping" features of your system).  The limited memory and performance of the PET does make it impractical to simutaneously support a variety of keyboard configurations.  If you would prefer a different configuration, the source is available for making a custom build :)

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
					 but I leave it as a surprise to players to actually locate it.
					 You may only activate it once!  It will basically give you
					 unlimited STAMINA.

F       = FINISH MAP

P       = PAUSE (press RETURN to resume)

				
C64 CONTROLS (and Apple ][)
============
Since the C64 has no NUMPAD, the only difference in the controls
for the C64 is:

J/L     = AIM BOW LEFT/RIGHT    (NUMPAD 4/6 for the PET build)

I       = "FLIP" BOW            (NUMPAD 8 for the PET build)

K       = USE PERSISTENCY       (NUMPAD 0 for the PET build)

(all other keys remain the same as the PET version)

NOTE: The C64 version does support the C64 JOYSTICK in PORT #2, which is also supported by VICE emulator (I use a NES USB gamepad). JOYSTICK controls will take a little getting used to, but recall the VICE emulator may have some key-mapping options to help adjust to your preference.

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



SNES GAMEPAD CONTROLS
=====================
NOTE: GAMEPAD is active only during STAGE maps, not in the menus.

D-PAD               = MOVEMENT (up/down/left/riht)

TOP LEFT/TOP RIGHT  = AIM BOW LEFT/RIGHT

A-BUTTON            = FIRE ARROW

X-BUTTON            = FLIP BOW 180 (active after about 200 movement steps)

B-BUTTON            = USE 1 PERSISTENCY to RESTORE 1 HEALTH

Y-BUTTON            = ACTIVATE ORB (if equipped)

START/SELECT        = FINISH MAP (once all CHALLENGES defeated)


APPLE ][ JOYSTICK/PADDLE CONTROLS
=================================
With no buttons pressed, joystick does normal W/A/S/D movement.

Button 2 FIRES arrows.

When Button 1 is pressed, the joystick is now in AIM-MODE.  When in AIM-MODE, the D-PAD behavior changes as follows:
  D-PAD UP = FLIP  (I-key)
  D-PAD DOWN = PERSISTENCY  (K-key)
  D-PAD LEFT/RIGHT = AIM bow left/right  (J/L keys)
  
If BOTH buttons are pressed at the same time, you are in UTILITY-MODE, the D-PAD behavior changes as follows:
  D-PAD UP = FINISH MAP (F-key)
  D-PAD DOWN = PAUSE (press any key on keyboard to resume)
  D-PAD RIGHT = USE ORB (if in possession)
  



SOURCE CODE
===========
The full source code is available in the "src" folder (including the STAGE editor and associated data files that represent the level-data while editing them).

The main compiler used to build DH is cc65.  But the C code has been arranged, such that it should be fairly easy to "port" the code to other systems that offer a native 40x25 "text-mode" option.

I did not prepare formal makefiles.  I just depend on the "go_XXX.bat" scripts in the src/source folder.

CORE.h/.c are my "core" macros, function, declarations that I think any application/program would likely need.  These files may be of interest to many programmers.

UTILITY.h/.c are more specialized functions that I think a game-type application would likely need.  Specifically, thinks like "press RETURN" or displaying centered banners.

GAME_STRINGS.h is all the string-related data of the game.  Each of these must be declared using SCREEN codes, not actual ASCII codes.  A reference list of these codes is available in the "DOC" folder.

SNES_GAMEPAD.h/.c is code to poll the buton states of the SNES GAMEPAD.

DESTINY_STRUCTS.h/.c are data structures associated more specifically to the DH gameplay (player state, challenge/persona state, etc.).

EDITOR.C is the STAGE editor code.  DH itself does not depend on any external files.  The EDITOR creates level data in the format required by the DH code.  These are stored in the D64 file, and the generated codes must be copy-pasted into the DH code when updated.   In the "DOCS" folder are more notes about this.

01_xxx, 02_yyy, 03_zzz.c are in the order of the main "flow" of the game: "choose destiny", "select persona", "show intro".  

main.c is the main "game engine" that draws the encoded maps, moves the challenges along their target paths, monitors input controls, and animates or updates the game "icons."

