.include "memconfig.asm"

varri = $10
varrid = $11

varri1 = $10
varrid1 = $11
varri2 = $10
varrid2 = $11

*=if_startvec
	JMP actualbegin

; paaohjelma

*=$cf00

actualbegin:
	jsr if_startmusic

	LDA #0
	sta varri1
	sta varri2
	LDA #1
	sta varrid1
	sta varrid2

	ldy #>incbgrut
	ldx #<incbgrut
;	JSR if_setirq2

	ldx #<$C0
	ldy #>$C0
	jmp if_executenext_nowait

ikiloop:
	jmp ikiloop


; irq

incbgrut:
	LDA framecounter
	AND #$01
	BNE toine


	lda varri1
	sta varri
	lda varrid1
	sta varrid
	jsr liikus
	lda varri
	sta varri1
	ora #$08
	sta $d016
	lda varrid
	sta varrid1

toine:

	LDA framecounter
	AND #$03
	BNE poistusirkista

	lda varri2
	sta varri
	lda varrid2
	sta varrid
	jsr liikus
	lda varri
	sta varri2
	ora #$18
	sta $d011
	lda varrid
	sta varrid2

poistusirkista:
	rts


korotas:
	inc $D020
	rts

liikus:
	lda varri
	clc
	adc varrid
	sta varri
dirchgcheck1:
	cmp #$7
	bne dirchgcheck2
	ldx #$ff
	stx varrid
	jmp dirchgcheckend
dirchgcheck2:
	cmp #$0
	bne dirchgcheckend
	ldx #$1
	stx varrid
dirchgcheckend:

	rts
