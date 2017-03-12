.include "memconfig.asm"


; koodi generoidaan $a400-$c021
; datat $2c00-$9e00
; charset $2000-
; toinen scriini $2800-$2c00
; screenbuffer $2300-$2800

; vec1 pyöritetään rutiinia
; vec2 vaihdetaan $d800 värit,normivärit,suunta ja palataan takas vec1

; jos gen ja copyscr kutsut ei mahdu sun musakutsujen väliin ni sit pitää siirtää rivejä
; copyscr rutiinits copyscr2 rutiiniin sen verran että mahtuu (lisäksi samat siirrot 
; copyscrb ja copyscr2b koska doublebuffer)

	* = $c100



gen	= $a400

thebeginning:

		jsr cyl_loop

		lda #$1b
		sta $d011

	ldy #>vec
	ldx #<vec
	JSR if_setirq1

ikilop:
	jsr iskes
	jmp ikilop

iskes:
	rts

	LDA #00
	sta $d011

	jmp if_executenext_nowait

;		sei
;		lda #<vec
;		sta $0314
;		lda #>vec
;		sta $0315
;		lda #1
;		sta $dc0d
;		sta $d01a
;		cli
;		jmp *

		

CYL_irq
	jsr vec
	rts
	
CYL_loop		
	jsr init

	lda #$60
	sta CYL_loop
	rts

swap	.byte 0

init
	jsr gencode

;	lda #1+8
;	sta $d800
;	lda #7+8
;	sta $d801
;	sta $d802
;	lda #3+8
;	sta $d803
;	sta $d804
;	sta $d805
;	lda #5+8
;	sta $d806
;	sta $d807
;	sta $d808
;	sta $d809
;	sta $d80a
;	sta $d80b
;	sta $d80c
;	sta $d80d
;	sta $d80e
;	lda #3+8
;	sta $d80f
;	sta $d810
;	sta $d811
;	lda #7+8
;	sta $d812
;	lda #1+8
;	sta $d813
;	

	ldy #0
	
ala	ldx #0
	
ac lda colsi,x
	clc 
	adc #8
ac2	sta $d800,x
	inx
	cpx #$28
	bne ac

	lda ac2+1
	clc
	adc #40
	sta ac2+1
	lda ac2+2
	adc #0
	sta ac2+2
	
o2
	
	iny
	cpy #25
	bne ala

toki	ldx #0
	lda cls,x;#$08
	sta $d023	
	lda cls2,x;#$06
	sta $d022
	lda cls3,x;#$00
	sta $d021
	
	lda #$d8
	sta $d016
	lda #$00
	sta $d020
	
	lda #$18
	sta $d018
	
	ldx #0
jekx	lda #$7f
	sta $0400,x
	sta $0500,x
	sta $0600,x
	sta $0700,x
	inx
	bne jekx
	
;	ldx #0
;jhj lda #$ff
;	sta $23f8,x
;	sta $0bf8,x
;	inx
;	cpx #8
;	bne jhj
	
	ldy #0
h2 ldx #0
h lda #0
fut sta $2300,x

	inc h+1
	inx
	cpx #20
	bne h
	
	lda h+1
	clc
	adc #4
	sta h+1
	
	lda fut+1
	clc
	adc #$28
	sta fut+1
	bcc je
	inc fut+2
	
je
	iny
	cpy #5
	bne h2

	ldy #0
	
jekc
	ldx #0
juuu
bf1	lda $2300,x
bf2	sta $2378,x
	inx
	cpx #3*40
	bne juuu
	
	lda bf1+1
	clc
	adc #$78
	sta bf1+1
	lda bf1+2
	adc #0
	sta bf1+2
	
	lda bf2+1
	clc
	adc #$78
	sta bf2+1
	lda bf2+2
	adc #0
	sta bf2+2
	
	iny
	cpy #9  ; (fix)
	bne jekc
	
	lda #$60
	sta init
	
	lda #1
	sta $05
	rts
	
vec
	cli	
	
fnt	lda #$18
		sta $d018

rr	ldx #95

		lda #0
		sta $d021
		sta $d020
	
	jsr gen
	
	lda frame 
	bne tof1
	jsr copyscr2
	jmp skipi	
tof1
	jsr copyscr2b
		
skipi
	
	lda yscr
	sta $d011	

	lda frame 
	bne tof2
	jsr copyscr
	jmp skipi2	
tof2
	jsr copyscrb
skipi2

	lda gogo+1
	clc
speed	adc #1
	sta gogo+1
	
	ldx frame
	lda font,x
	sta fnt+1

	lda frame
	eor #1
	sta frame
	
	jsr roll
jkr

gogo	ldy #0

	lda frame 
	bne tof3
	jsr scrollit
	jmp skipi3
tof3
	jsr scrollitb
skipi3

	jsr chvec
	jsr lopetus
	
	rts

lopetus
		rts
		dec wff
		lda wff
		bne ojec

		LDA #$ea
		sta iskes
		jsr if_unsetirq1
		
		lda #0 
		sta yscr
		lda #$60
		sta scrollit
		sta scrollitb
		rts

wff	.byte 255

chvec
		dec wff
		lda wff
		bne ojec
		
		lda #0
		sta $d011
		lda #1
		sta $d020
		sta $d021
		
		lda #2
		sta speed+1

	ldy #>vec2
	ldx #<vec2
	JSR if_setirq1
		

	ldx #1
	lda cls,x;#$08
	sta $d023	
	lda cls2,x;#$06
	sta $d022
	lda cls3,x;#$00
	sta $d021

		inc dir
		
		lda #$60
		sta chvec
ojec
		rts
		
back
		rts

	ldy #>vec
	ldx #<vec
	JSR if_setirq1


		lda #$ea 
		sta lopetus
		rts		

vec2

	cli

	;bit $d011
	;bmi *-3
	;bit $d011
	;bpl *-3 


		jsr back
		
		
;		lda #$1a
;jyy	cmp $d012
;		bne jyy
		
		; change screen colors
		
		
	ldx #0
	
acc lda colsi,x
	clc 
	adc #8
	sta $d800,x
	sta $d800+(1*40),x
	sta $d800+(2*40),x
	sta $d800+(3*40),x
	sta $d800+(4*40),x
	sta $d800+(5*40),x
	sta $d800+(6*40),x
	sta $d800+(7*40),x
	sta $d800+(8*40),x
	sta $d800+(9*40),x
	sta $d800+(10*40),x
	sta $d800+(11*40),x
	sta $d800+(12*40),x
	sta $d800+(13*40),x
	sta $d800+(14*40),x
	sta $d800+(15*40),x
	sta $d800+(16*40),x
	sta $d800+(17*40),x
	sta $d800+(18*40),x
	sta $d800+(19*40),x
	sta $d800+(20*40),x
	sta $d800+(21*40),x
	sta $d800+(22*40),x
	sta $d800+(23*40),x
	sta $d800+(24*40),x
	inx
	cpx #$28
	bne acc

;		lda #$ff
;jyc	cmp $d012
;		bne jyc
				
		lda #$ea
		sta back

		rts

dir .byte 0;

roll
	lda dir
	beq right
	
	inc rr+1
	lda rr+1
	cmp #128
	bne jkrr
	
	lda #0
	sta rr+1
jkrr	rts

right
	dec rr+1
	
	lda rr+1
	cmp #255
	bne jkrr
	
	lda #128
	sta rr+1

	rts

frame	.byte 0
font	.byte $18,$a8
	
	
los		.byte <$2300,<$2300+(1*40),<$2300+(2*40),<$2300+(3*40),<$2300+(4*40),<$2300+(5*40),<$2300+(6*40),<$2300+(7*40),<$2300+(8*40),<$2300+(9*40)
			.byte <$2300+(10*40),<$2300+(11*40),<$2300+(12*40),<$2300+(13*40),<$2300+(14*40),<$2300+(15*40),<$2300+(16*40),<$2300+(17*40),<$2300+(18*40),<$2300+(19*40)
			.byte <$2300+(20*40),<$2300+(21*40),<$2300+(22*40),<$2300+(23*40),<$2300+(24*40),<$2300+(25*40),<$2300+(26*40),<$2300+(27*40),<$2300+(28*40),<$2300+(29*40)
			.byte <$2300+(30*40),<$2300+(31*40),<$2300+(32*40),<$2300+(33*40),<$2300+(34*40),<$2300+(35*40),<$2300+(36*40),<$2300+(37*40),<$2300+(38*40),<$2300+(39*40)
			.byte <$2300+(40*40),<$2300+(41*40),<$2300+(42*40),<$2300+(43*40),<$2300+(44*40),<$2300+(45*40),<$2300+(46*40),<$2300+(47*40),<$2300+(48*40),<$2300+(49*40)
			.byte <$2300+(50*40),<$2300+(51*40),<$2300+(52*40),<$2300+(53*40),<$2300+(54*40),<$2300+(55*40),<$2300+(56*40),<$2300+(57*40),<$2300+(58*40),<$2300+(59*40)
			.byte <$2300+(60*40),<$2300+(61*40),<$2300+(62*40),<$2300+(63*40),<$2300+(64*40),<$2300+(65*40),<$2300+(66*40),<$2300+(67*40),<$2300+(68*40),<$2300+(69*40)
			.byte <$2300+(70*40),<$2300+(71*40),<$2300+(72*40),<$2300+(73*40),<$2300+(74*40),<$2300+(75*40),<$2300+(76*40),<$2300+(77*40),<$2300+(78*40),<$2300+(79*40)

his
			.byte >$2300,>$2300+(1*40),>$2300+(2*40),>$2300+(3*40),>$2300+(4*40),>$2300+(5*40),>$2300+(6*40),>$2300+(7*40),>$2300+(8*40),>$2300+(9*40)
			.byte >$2300+(10*40),>$2300+(11*40),>$2300+(12*40),>$2300+(13*40),>$2300+(14*40),>$2300+(15*40),>$2300+(16*40),>$2300+(17*40),>$2300+(18*40),>$2300+(19*40)
			.byte >$2300+(20*40),>$2300+(21*40),>$2300+(22*40),>$2300+(23*40),>$2300+(24*40),>$2300+(25*40),>$2300+(26*40),>$2300+(27*40),>$2300+(28*40),>$2300+(29*40)
			.byte >$2300+(30*40),>$2300+(31*40),>$2300+(32*40),>$2300+(33*40),>$2300+(34*40),>$2300+(35*40),>$2300+(36*40),>$2300+(37*40),>$2300+(38*40),>$2300+(39*40)
			.byte >$2300+(40*40),>$2300+(41*40),>$2300+(42*40),>$2300+(43*40),>$2300+(44*40),>$2300+(45*40),>$2300+(46*40),>$2300+(47*40),>$2300+(48*40),>$2300+(49*40)
			.byte >$2300+(50*40),>$2300+(51*40),>$2300+(52*40),>$2300+(53*40),>$2300+(54*40),>$2300+(55*40),>$2300+(56*40),>$2300+(57*40),>$2300+(58*40),>$2300+(59*40)
			.byte >$2300+(60*40),>$2300+(61*40),>$2300+(62*40),>$2300+(63*40),>$2300+(64*40),>$2300+(65*40),>$2300+(66*40),>$2300+(67*40),>$2300+(68*40),>$2300+(69*40)
			.byte >$2300+(70*40),>$2300+(71*40),>$2300+(72*40),>$2300+(73*40),>$2300+(74*40),>$2300+(75*40),>$2300+(76*40),>$2300+(77*40),>$2300+(78*40),>$2300+(79*40)

yscr .byte $10


copyscr
			ldx #9
blu		lda $0400,x
			sta $0400+(3*40),x
			sta $0400+(6*40),x
			sta $0400+(9*40),x
			sta $0400+(12*40),x
			sta $0400+(15*40),x
			sta $0400+(18*40),x
			sta $0400+(21*40),x
			sta $0400+(24*40),x			

			sta $0414,x
			sta $0414+(3*40),x
			sta $0414+(6*40),x
			sta $0414+(9*40),x
			sta $0414+(12*40),x
			sta $0414+(15*40),x
			sta $0414+(18*40),x
			sta $0414+(21*40),x
			sta $0414+(24*40),x
					
			lda $0400+(1*40),x
			sta $0414+(1*40),x				
			sta $0400+(4*40),x
			sta $0400+(7*40),x
			sta $0400+(10*40),x
			sta $0400+(13*40),x
			sta $0400+(16*40),x
			sta $0400+(19*40),x
			sta $0400+(22*40),x									

			sta $0414+(1*40),x
			sta $0414+(4*40),x
			sta $0414+(7*40),x
			sta $0414+(10*40),x
			sta $0414+(13*40),x
			sta $0414+(16*40),x
			sta $0414+(19*40),x
			sta $0414+(22*40),x
												
			dex
			bmi blu2
			
			jmp blu
blu2
			rts

copyscrb
			ldx #9
bluu	lda $2800,x
			sta $2800+(3*40),x
			sta $2800+(6*40),x
			sta $2800+(9*40),x
			sta $2800+(12*40),x
			sta $2800+(15*40),x
			sta $2800+(18*40),x
			sta $2800+(21*40),x
			sta $2800+(24*40),x			

			sta $2814,x
			sta $2814+(3*40),x
			sta $2814+(6*40),x
			sta $2814+(9*40),x
			sta $2814+(12*40),x
			sta $2814+(15*40),x
			sta $2814+(18*40),x
			sta $2814+(21*40),x
			sta $2814+(24*40),x
					
			lda $2800+(1*40),x
			sta $2814+(1*40),x				
			sta $2800+(4*40),x
			sta $2800+(7*40),x
			sta $2800+(10*40),x
			sta $2800+(13*40),x
			sta $2800+(16*40),x
			sta $2800+(19*40),x
			sta $2800+(22*40),x									

			sta $2814+(1*40),x
			sta $2814+(4*40),x
			sta $2814+(7*40),x
			sta $2814+(10*40),x
			sta $2814+(13*40),x
			sta $2814+(16*40),x
			sta $2814+(19*40),x
			sta $2814+(22*40),x
												
			dex
			bmi bluu2
			
			jmp bluu
bluu2
			rts


copyscr2
			ldx #9
blu3
									
			lda $0400+(2*40)+10,x
			sta $0414+(2*40)+10,x
			sta $0400+(5*40)+10,x
			sta $0400+(8*40)+10,x
			sta $0400+(11*40)+10,x
			sta $0400+(14*40)+10,x
			sta $0400+(17*40)+10,x
			sta $0400+(20*40)+10,x
			sta $0400+(23*40)+10,x

			sta $0414+(2*40)+10,x
			sta $0414+(5*40)+10,x
			sta $0414+(8*40)+10,x
			sta $0414+(11*40)+10,x
			sta $0414+(14*40)+10,x
			sta $0414+(17*40)+10,x
			sta $0414+(20*40)+10,x
			sta $0414+(23*40)+10,x
					
									
			lda $0400+(2*40),x
			sta $0414+(2*40),x
			sta $0400+(5*40),x
			sta $0400+(8*40),x
			sta $0400+(11*40),x
			sta $0400+(14*40),x
			sta $0400+(17*40),x
			sta $0400+(20*40),x
			sta $0400+(23*40),x

			sta $0414+(2*40),x
			sta $0414+(5*40),x
			sta $0414+(8*40),x
			sta $0414+(11*40),x
			sta $0414+(14*40),x
			sta $0414+(17*40),x
			sta $0414+(20*40),x
			sta $0414+(23*40),x

			lda $0400+10,x
			sta $0400+(3*40)+10,x
			sta $0400+(6*40)+10,x
			sta $0400+(9*40)+10,x
			sta $0400+(12*40)+10,x
			sta $0400+(15*40)+10,x
			sta $0400+(18*40)+10,x
			sta $0400+(21*40)+10,x
			sta $0400+(24*40)+10,x			

			sta $0414+10,x
			sta $0414+(3*40)+10,x
			sta $0414+(6*40)+10,x
			sta $0414+(9*40)+10,x
			sta $0414+(12*40)+10,x
			sta $0414+(15*40)+10,x
			sta $0414+(18*40)+10,x
			sta $0414+(21*40)+10,x
			sta $0414+(24*40)+10,x
					
			lda $0400+(1*40)+10,x
			sta $0414+(1*40)+10,x				
			sta $0400+(4*40)+10,x
			sta $0400+(7*40)+10,x
			sta $0400+(10*40)+10,x
			sta $0400+(13*40)+10,x
			sta $0400+(16*40)+10,x
			sta $0400+(19*40)+10,x
			sta $0400+(22*40)+10,x									

			sta $0414+(1*40)+10,x
			sta $0414+(4*40)+10,x
			sta $0414+(7*40)+10,x
			sta $0414+(10*40)+10,x
			sta $0414+(13*40)+10,x
			sta $0414+(16*40)+10,x
			sta $0414+(19*40)+10,x
			sta $0414+(22*40)+10,x
												
			dex
			bmi blu4
			
			jmp blu3
blu4
			rts


copyscr2b
			ldx #9
blux3									
			lda $2800+(2*40)+10,x
			sta $2814+(2*40)+10,x
			sta $2800+(5*40)+10,x
			sta $2800+(8*40)+10,x
			sta $2800+(11*40)+10,x
			sta $2800+(14*40)+10,x
			sta $2800+(17*40)+10,x
			sta $2800+(20*40)+10,x
			sta $2800+(23*40)+10,x

			sta $2814+(2*40)+10,x
			sta $2814+(5*40)+10,x
			sta $2814+(8*40)+10,x
			sta $2814+(11*40)+10,x
			sta $2814+(14*40)+10,x
			sta $2814+(17*40)+10,x
			sta $2814+(20*40)+10,x
			sta $2814+(23*40)+10,x
					
									
			lda $2800+(2*40),x
			sta $2814+(2*40),x
			sta $2800+(5*40),x
			sta $2800+(8*40),x
			sta $2800+(11*40),x
			sta $2800+(14*40),x
			sta $2800+(17*40),x
			sta $2800+(20*40),x
			sta $2800+(23*40),x

			sta $2814+(2*40),x
			sta $2814+(5*40),x
			sta $2814+(8*40),x
			sta $2814+(11*40),x
			sta $2814+(14*40),x
			sta $2814+(17*40),x
			sta $2814+(20*40),x
			sta $2814+(23*40),x

			lda $2800+10,x
			sta $2800+(3*40)+10,x
			sta $2800+(6*40)+10,x
			sta $2800+(9*40)+10,x
			sta $2800+(12*40)+10,x
			sta $2800+(15*40)+10,x
			sta $2800+(18*40)+10,x
			sta $2800+(21*40)+10,x
			sta $2800+(24*40)+10,x			

			sta $2814+10,x
			sta $2814+(3*40)+10,x
			sta $2814+(6*40)+10,x
			sta $2814+(9*40)+10,x
			sta $2814+(12*40)+10,x
			sta $2814+(15*40)+10,x
			sta $2814+(18*40)+10,x
			sta $2814+(21*40)+10,x
			sta $2814+(24*40)+10,x
					
			lda $2800+(1*40)+10,x
			sta $2814+(1*40)+10,x				
			sta $2800+(4*40)+10,x
			sta $2800+(7*40)+10,x
			sta $2800+(10*40)+10,x
			sta $2800+(13*40)+10,x
			sta $2800+(16*40)+10,x
			sta $2800+(19*40)+10,x
			sta $2800+(22*40)+10,x									

			sta $2814+(1*40)+10,x
			sta $2814+(4*40)+10,x
			sta $2814+(7*40)+10,x
			sta $2814+(10*40)+10,x
			sta $2814+(13*40)+10,x
			sta $2814+(16*40)+10,x
			sta $2814+(19*40)+10,x
			sta $2814+(22*40)+10,x
												
			dex
			bmi blux4
			
			jmp blux3
blux4
			rts


scrollit
			lda sine,y
			tay
			eor #$ff
			and #$07
			ora #$10
			sta yscr
			tya
			lsr
			lsr
			lsr
			sta ypos+1
									
ypos	ldx #0
			lda los,x
			sta th01+1
			lda his,x
			sta th01+2
			inx
			lda los,x
			sta th02+1
			lda his,x
			sta th02+2
			inx
			lda los,x
			sta th03+1
			lda his,x
			sta th03+2
			
		
			
			ldx #19
th01	lda $4400,x
			sta $0400,x

th02	lda $4400,x
			sta $0400+(1*40),x

th03	lda $4400,x
			sta $0400+(2*40),x

			dex
			bmi juuo
			jmp th01
juuo
			rts
			

scrollitb
			lda sine,y
			tay
			eor #$ff
			and #$07
			ora #$10
			sta yscr
			tya
			lsr
			lsr
			lsr
			sta yypos+1
									
yypos	ldx #0
			lda los,x
			sta tth01+1
			lda his,x
			sta tth01+2
			inx
			lda los,x
			sta tth02+1
			lda his,x
			sta tth02+2
			inx
			lda los,x
			sta tth03+1
			lda his,x
			sta tth03+2
			
		
			
			ldx #19
tth01	lda $4400,x
			sta $2800,x

tth02	lda $4400,x
			sta $2800+(1*40),x

tth03	lda $4400,x
			sta $2800+(2*40),x

			dex
			bmi jueo
			jmp tth01
jueo
			rts
			

		
gencode
		lda #<gen
		sta $50
		lda #>gen
		sta $51
		
tum
rn	ldx #0

		lda nli1
		clc
		adc angletab,x
		sta gg1+1
		lda nli1+1
		adc #0
		sta gg1+2

		lda nli2
		clc
		adc angletab+1,x
		sta gg2+1
		lda nli2+1
		adc #0
		sta gg2+2
		
		lda nli3
		clc
		adc angletab+2,x
		sta gg3+1
		lda nli3+1
		adc #0
		sta gg3+2

		lda nli4
		clc
		adc angletab+3,x
		sta gg4+1
		lda nli4+1
		adc #0
		sta gg4+2
		
		
		lda rn+1
		clc
		adc #4
		sta rn+1	


		ldy #0
		ldx #0
gl1	lda geny,x
		sta ($50),y
;		sta ($52),y
		inx
		iny
		cpy #endgeny-geny
		bne gl1

		ldx #0
gl2	lda gg5,x
		sta ($50),y
		inx
		iny
		cpx #3
		bne gl2
		
fit
		tya
		sta $54
			
		lda $50
		clc
		adc $54
		sta $50
		lda $51
		adc #0
		sta $51

;		lda $52
;		clc
;		adc $54
;		sta $52
;		lda $53
;		adc #0
;		sta $53
	
		lda gg5+1
		clc
		adc #8
		sta gg5+1
		lda gg5+2
		adc #0
		sta gg5+2
				
		inc cnt1
		lda cnt1
		cmp #20
		beq oh0
		jmp tum
oh0
		inc pos
		bne he
		inc pos+1
he
	
		lda pos
		sta gg5+1
		lda pos+1
		sta gg5+2
		
		lda #0
		sta cnt1
		
		lda #0
		sta rn+1
		
		lda nli1
		clc
		adc #$00
		sta nli1
		lda nli1+1
		adc #$01
		sta nli1+1

		lda nli2
		clc
		adc #$00
		sta nli2
		lda nli2+1
		adc #$01
		sta nli2+1

		lda nli3
		clc
		adc #$00
		sta nli3
		lda nli3+1
		adc #$01
		sta nli3+1

		lda nli4
		clc
		adc #$00
		sta nli4
		lda nli4+1
		adc #$01
		sta nli4+1

		
		inc cnt2
		lda cnt2
		cmp #8
		beq oh1
		jmp tum
oh1
		lda #0
		sta cnt2
		
		lda pos
		clc
		adc #$c0-8
		sta pos
		lda pos+1
		adc #0
		sta pos+1

		lda pos
		sta gg5+1
		lda pos+1
		sta gg5+2

		inc cnt3
		lda cnt3
		cmp #3
		beq oh2
		jmp tum
oh2


; done
		ldy #0
		lda #$60
		sta ($50),y
;		sta ($52),y


; done

		rts
	
		
geny
gg1	lda $7e00,x
gg2	ora $6000,x
gg3	ora $4200,x
gg4	ora $2400,x
endgeny	

gg5	sta $2000


cnt1	.byte 0 ; column count
cnt2	.byte 0	; rowcount
cnt3	.byte 0

pos	.word $2000
		
nli1	.word $7e00+$800
nli2	.word $6000+$800
nli3	.word $4200+$800
nli4	.word $2400+$800

lastval .byte 255  ; last index value storage

	;* = * & $ff00 + 256
	.align $100
	;*= $e000
angletab
		.binary "../assets/cylinder/angletab2"

sine
		.binary "../assets/cylinder/sin0t219"
		
cls	.byte $e
cls2	.byte $4
cls3	.byte $9		
		
colsi
; .byte 7,5,3,3,7,3,7,7,7,1,1,3,7,3,3,5,3,5,5,3,7,5,3,3,7,3,7,7,7,1,1,3,7,3,3,5,3,5,5,3
	.byte 1,7,7,3,3,3,5,5,5,5,5,5,5,5,5,3,3,3,7,1,1,7,7,3,3,3,5,5,5,5,5,5,5,5,5,3,3,3,7,1
	
colsi2
	.byte 3,5,3,3,3,3,7,7,1,1,7,3,3,3,5,5,5,5,5,5,3,5,3,3,3,3,7,7,1,1,7,3,3,3,5,5,5,5,5,5	

		* = if_startvec
		jmp thebeginning
		
		* = $2c00
		
		.binary "../assets/cylinder/bits1.prg"
		.binary "../assets/cylinder/bits2.prg"
		.binary "../assets/cylinder/bits3.prg"
		.binary "../assets/cylinder/bits4.prg"
		
		