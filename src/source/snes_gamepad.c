#include <snes_gamepad.h>

void read_gamepad()
{
	__asm__("lda #$28");
	__asm__("sta $e843");	
  __asm__("lda #$00");
	__asm__("sta $033a");
	__asm__("sta $033b");
	__asm__("lda #$08");  
  __asm__("sta $033c");
	__asm__("ldx #$0c");
	__asm__("ldy #$01");
  __asm__("lda #$20");
	__asm__("sta $e841");
	__asm__("lda #$00");
	__asm__("sta $e841");  
target3:
  __asm__("lda $e841");
	__asm__("and #$40");
	__asm__("cmp #$40");
	__asm__("beq %g", target1);
	__asm__("lda $033c");
	__asm__("ora $033a,y");
	__asm__("sta $033a,y");
target1:		
  __asm__("lda $033c");
	__asm__("lsr a");
	__asm__("bne %g", target2);
	__asm__("dey");
	__asm__("lda #$80");
target2:	
  __asm__("sta $033c");
	__asm__("lda #$08");
	__asm__("sta $e841");
	__asm__("lda #$00");
  __asm__("sta $e841");
	__asm__("dex");
	__asm__("bne %g", target3);
  // RTS implied by end of function scope}
}
