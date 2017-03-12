.include "memconfig.asm"

; bitmap $a000-$bf40
; colormap $8800-$8c00 (kopioidaan $8400)
; sinidata $8200-$8400
; spritet $8000-$8200
; koodin pituus datoineen n. $500 voi olla missä vaan
	
			* = $c000

			thebeginning:


			JSR if_unsetirq1
			JSR if_unsetirq2
			
			lda #0
			sta $d020
			sta $d021
			sta $d017

			;lda #$c5
			;sta $dd00
			lda #$3e
			sta $dd02

			lda #0
			ldx #0
			clrcolor
			sta $8400,x
			sta $8500,x
			sta $8600,x
			sta $8700,x
			inx
			bne clrcolor
											
			jsr init
					
;			sei
;			lda #<vec
;			sta $0314
;			lda #>vec
;			sta $0315


	ldy #>vec
	ldx #<vec
	JSR if_setirq1

;			lda #1
;			sta $dc0d
;			sta $d01a

			lda #$3b
			sta $d011
			
			lda #$18
			sta $d018
			
			lda #$c8
			sta $d016

;			jmp $c90
						
ikilop:
	jsr exitpart
	jmp ikilop

exitpart:
	rts

	lda #$60
	sta $d011


	jmp if_executenext_nowait
;	jmp $fff0

			
vec
;			lda #0
;			sta $d012		
						
			jsr movesprup
			jsr movesprup2
			jsr movesprup2
			jsr bubbles
			jsr kuvapois
				
			rts
			
wt1		.byte 3
wt2		.byte 233
kuvapois

			dec wt1
			lda wt1
			bne oi
			
			lda #3
			sta wt1
			
			dec wt2
			lda wt2
			bne oi
			
			lda #$60
			sta bubbles
			lda #$ea
			sta movesprup2
			
			lda #24
			sta xp+1
			lda #250-13
			sta bace+1
			lda #1
			sta spnext+1
			jsr peittospriteup

oi		

			rts			
			
bubbles
			rts
		
			lda #%10000000
			sta $d010	
			lda #$00
			sta $d01d
			lda #$ff
			sta $d015
		
			ldy #0
			ldx #0
bachu

			lda sinlis,x
			sta sinli+1					
			
sinli		lda sin,x
			adc bubx,x
			sta $d000,y
			
			lda buby,x
			sta $d001,y
			
			lda bubcol,x
			sta $d027,x
			
			lda bubsp,x
			sta $87f8,x
		
			inc sinlis,x	
					
			lda hid
			beq eist		
					
			lda buby,x
			sec
			sbc bubspd,x
			sta buby,x
			;cmp #31
			;bcs eist
			
			;lda #0
			;sta bubspd,x	
eist

			iny
			iny
			inx
			cpx #8
			bne bachu
					
			lda hid 
			eor #1
			sta hid
		
			ldy #0	
			ldx #0
jicc	lda buby,x
			eor #255
			tay
			lda scale,y
			sta bubcol,x
			inx
			cpx #8
			bne jicc
			
			rts			
			
hid		.byte 0			

bubcol .byte 6,6,6,6,6,6,6,6

bubspd	.byte 1,2,3,3,2,2,2,2
			
buby 	.byte 200,220,240,230,205,255,225,225

bubx 	.byte 40,100,200,90,155,150,180,10			
		
bubsp	.byte 2,3,4,3,3,4,2,3		
			
sinlis	.byte 10,20,30,40,50,60,70,80
			
;			* = *&$ff00+256
	.align $100

		
scale	.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
			.byte 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4	
			.byte $e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e,$e
			.byte 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
			.byte 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			.byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			
			
			
movesprup			
			dec $d001
			dec $d003
			dec $d005
			dec $d007
			dec $d009
			dec $d00b
			dec $d00d

			lda $d00d
			cmp #$1c
			bne stop1
		
			lda #$60
			sta movesprup

			lda #$ea
			sta bubbles
			rts	


stop1
			lda $d001
			and #$07
			cmp #7
			bne eikop
			
			ldx #0
cop1	lda $8800+(24*40),x
cop2	sta $8400+(24*40),x
			inx
			cpx #40
			bne cop1
		
			lda cop1+1
			sec
			sbc #40
			sta cop1+1
			lda cop1+2
			sbc #0
			sta cop1+2
			cmp #$87
			bne eil
			
			lda #$60
			sta stop1
			rts
			
eil
			lda cop2+1
			sec
			sbc #40
			sta cop2+1
			lda cop2+2
			sbc #0
			sta cop2+2
				
			
eikop						
			rts
			
			


movesprup2			
			rts
			dec $d001
			dec $d003
			dec $d005
			dec $d007
			dec $d009
			dec $d00b
			dec $d00d

			lda $d00d
			cmp #$1c
			bne stop2

	LDA #60
	sta $d011

			lda #$ea
			sta exitpart
		
			lda #$60
			sta movesprup2
			rts	

stop2
			lda $d001
			and #$07
			cmp #7
			bne eilz
			
			ldx #0
copp1	lda #0
copp2	sta $8400+(24*40),x
			inx
			cpx #40
			bne copp1
		
			lda copp2+1
			sec
			sbc #40
			sta copp2+1
			lda copp2+2
			sbc #0
			sta copp2+2
			cmp #$83
			bne eilz
			
			lda #$60
			sta stop2
eilz	rts


init
peittospriteup

			lda #%01111111
			sta $d015
			sta $d01d
			
			lda #%01100000
			sta $d010
			
			ldx #0
			ldy #0
bace	lda #250-8
			sta $d001,y
			
			lda #0
			sta $d027,x
			
xp		lda #24
			sta $d000,y
		
			lda xp+1
			clc
			adc #48
			sta xp+1
			
spnext	lda #0
			sta $87f8,x
			
			iny
			iny
			inx
			cpx #8
			bne bace
			rts

		
		* = $8800
		
		.binary "../assets/annabel/annabelcol"
		
		* = $a000		
		.binary "../assets/annabel/annabel"
		
		
		* = $8000
		
		.binary "../assets/annabel/peittoin"
		.binary "../assets/annabel/peittoout"
		.binary "../assets/annabel/kupla1"
		.binary "../assets/annabel/kupla2"
		.binary "../assets/annabel/kupla3"
		
		* = $8200
	
sin	
		.binary "../assets/annabel/sin0to35"
		.binary "../assets/annabel/sin0to35"

		* = if_startvec
		jmp thebeginning
