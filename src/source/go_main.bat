REM COMPILER to assembly
set OPTIMIZE=-O -Oi -Or -Os
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include main.c 
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include core.c 
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include utility.c
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 01_destiny.c 
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 02_init_persona.c 
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 03_intro.c
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include destiny_structs.c
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include game_strings.c
..\..\..\cc65-snapshot-win32\bin\cc65 %OPTIMIZE% --target pet --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include snes_gamepad.c

REM ASSEMBLER to object code
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet main.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet core.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet utility.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet 01_destiny.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet 02_init_persona.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet 03_intro.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet destiny_structs.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet game_strings.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target pet snes_gamepad.s

REM LINKER to executable
..\..\..\cc65-snapshot-win32\bin\ld65 --target pet --obj main.o core.o utility.o 01_destiny.o 02_init_persona.o 03_intro.o destiny_structs.o game_strings.o snes_gamepad.o --lib ..\lib\pet.lib -o main.out
