REM COMPILER to assembly
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include main.c 
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include core.c 
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include utility.c
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 01_destiny.c 
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 02_init_persona.c 
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include 03_intro.c
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include destiny_structs.c
..\..\..\cc65-snapshot-win32\bin\cc65 -Oi --target c64 --include-dir ..\include --include-dir ..\..\..\cc65-snapshot-win32\include game_strings.c

REM ASSEMBLER to object code
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 main.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 core.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 utility.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 01_destiny.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 02_init_persona.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 03_intro.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 destiny_structs.s
..\..\..\cc65-snapshot-win32\bin\ca65 --target c64 game_strings.s

REM LINKER to executable
..\..\..\cc65-snapshot-win32\bin\ld65 --target c64 --obj main.o core.o utility.o 01_destiny.o 02_init_persona.o 03_intro.o destiny_structs.o game_strings.o --lib ..\lib\c64.lib -o main_c64.out
