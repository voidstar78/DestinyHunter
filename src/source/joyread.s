.export _joyread                    ; Make the assembly function visible to "C" code
.import _joyX, _joyY, _joyMask      ; Make the global "C" variables visible to the assembly code

.include "apple2.inc"               ; some port locations defined in here

PADDLX  := $C064                    ; Read to get POT msb
PTRIGX  := $C070                    ; Reset PADDLE values so counting can start

; These are the bits set for getting digital values from the Joystick
UP      =  %00000001                ; 1
RIGHT   =  %00000010                ; 2
DOWN    =  %00000100                ; 4
LEFT    =  %00001000                ; 8
BUTTON0 =  %00010000                ; 16
BUTTON1 =  %00100000                ; 32

.proc _joyread
    
    ldx     #$0                     ; start the x counter at 0
    ldy     #$0                     ; start the y counter at 0
    lda     PTRIGX                  ; trigger the fact that the joystick will be read

readBoth:
    lda     PADDLX                  ; get the value for X axis
    bpl     :+                      ; if MSB is zero, done with X
upX:
    inx                             ; increment the counter
    bne     :+                      ; while x <> 0 go read y
    dex                             ; if x reaches 0 it's overflow, keep at 255
:
    lda     PADDLX + 1              ; read the value for the Y axis
    bpl     readX                   ; if MSB is zero Y is done, may need to handle X still
    iny                             ; increment the Y counter
    bne     readBoth                ; branch to the start to check both axis
    dey                             ; if Y reaches 0 it's overflow, keep at 255 and drop through to check x

readX:
    lda     PADDLX                  ; get the value for X axis
    bmi     upX                     ; if not done, go increment x

doneJoy:
    lda     #0                      ; start the digital mask with all off

    cpx     #$20                    ; compare to the low end dead zone
    bcs     :+                      ; if greater than, then not left
    ora     #LEFT                   ; the joystick is left
    bne     chkY                    ; branch always as acc is now non-zero
:
    cpx     #$60                    ; check the upper bound of the dead-zone
    bcc     chkY                    ; if less than, then not right
    ora     #RIGHT                  ; gt high end of dead zone so joystick is right

chkY:
    cpy     #$20                    ; do the same for the Y axis as for the X axis
    bcs     :+
    ora     #UP 
    bne     buttons
:
    cpy     #$60 
    bcc     buttons 
    ora     #DOWN

buttons:
    stx     _joyX                   ; store the "counted" x and y values of the joystick in the
    sty     _joyY                   ; global "C" variables (notice the _ before the name of "C" variables)
    
    ldx     BUTN0                   ; read the button 0 state
    cpx     #$80                    ; if ge 128 the button is fully depressed
    bcc     :+          
    ora     #BUTTON0                ; mark the button as down

:
    ldx     BUTN1                   ; do the same for button 1 as for button 0
    cpx     #$80 
    bcc     :+
    ora     #BUTTON1 

:
    sta     _joyMask                ; save the mask in the global "C" variable

    rts                             ; return to the C code

.endproc