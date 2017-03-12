.include "memconfig.asm"


; bitmap $2000-$3f40
; koodi+taulukot $4000-$982f
; bitmap kopioidaan merkistöihin $5800-$8000

fnt1	= $18
fnt2 	= $1a
fnt3  = $1c
fnt4	= $1e
fnt5 	= $16


*=if_startvec
        	jmp alkukoodi

        *=$a000
col1 = $d020
col2 = $d021
col3 = $d025


mirror	= 0

	alkukoodi:

		;	.byte 2
	;jsr if_startmusic

		
			jsr KALLO_LOOP

;		jsr gencle		
;					      sei
                ;lda #<vec
                ;sta $0314
                ;lda #>vec
                ;sta $0315


                lda #$01
                sta $dc0d
                sta $d01a
                lda #$1b
                sta $d011
                
                lda #$c8
                sta $d016


	ldy #>vec
	ldx #<vec
	JSR if_setirq1
	JSR if_unsetirq2

ikilop:
	jsr iskes
	jmp ikilop

iskes:
	rts

	jmp if_executenext_nowait


								* = $4800                
vec     
;m                lda #$30
;m               sta $d012 


; RAAH!!!!!!
; eli tällä d011 = 1b eka badline on rivi $33
; varmaan orkkis stabilointi menee jotenkin niin että se osuu siihen
; ekaan badlineen jotenkin ja sitten vakautuu hienosti
								lda #$31
ew							cmp $d012
								bne ew
								
								ldy #10
www							dey
								bne www
								nop
								bit $ea
								lda #$3d
								sta $dd02
																			
								nop
								bit $ea			
								jsr raz1		
								nop
								bit $ea			
						
nff								lda #0
								sta $d020
								sta $d021

																
								jsr chgvec					
								jsr colorin							
		
								jsr colorout
								
								jsr brdfls
							  	
							  	rts
								
vec2			
;			lda #$30
;			sta $d012
			lda #$18
			sta $d018	
			lda #$3b
			sta $d011
			lda #$3c
			sta $dd02

			jsr clearcolmap			
			jsr invertbmp1

;			lda #$ea
;			sta vecback

; oota ruudun loppua?
;			lda #$ff
;ww		cmp $d012
;			bne ww
		
			jsr vaihto
						
			jsr vecback
			
			rts
			
vaihto
			rts

			
			lda #$3d
			sta $dd02
			lda #$1b
			sta $d011
			lda #$12
			sta $d018
	ldy #>vec
	ldx #<vec
	JSR if_setirq1
	JSR if_unsetirq2

			rts			
			
chgvec	
			rts
	JSR if_unsetirq1
	ldy #>vec2
	ldx #<vec2
	JSR if_setirq2
			
			lda #$60
			sta chgvec					
			rts	
			
vecback
			rts

			lda #$ea
			sta frw
			
frw		rts
			
			ldx #0
keox	lda #7
ss		sta $d800,x
			inx
			bne keox
			inc ss+2
			lda ss+2
			cmp #$dc
			bne eiva

			lda #$60
			sta vecback					
		
			lda #$ea
			sta vaihto					
eiva			
			rts				
			
clearcolmap
			ldx #0
			lda #0
hjee	sta alas1,x
			inx
			cpx #200
			bne hjee
			
			lda #$60
			sta clearcolmap
			rts			
			
invertbmp1
			ldx #0
il1		lda $5800,x
			eor #255
il2		sta $5800,x
il3		lda $5900,x
			eor #255
il4		sta $5900,x
			inx
			bne il1
		
			inc il1+2
			inc il2+2
			inc il3+2
			inc il4+2
			inc il1+2
			inc il2+2
			inc il3+2
			inc il4+2
			
			lda il1+2
			cmp #$80
			bne eivv
		
			lda #$60
			sta invertbmp1
			
			lda #$ea
			sta vecback
							
eivv			
			rts
			
invertbmp2
			rts			
			
bw1		.byte 155	
			
brdfls			
			rts
			dec bw1	
			lda bw1
			bne eiviz
			
			inc bw1
			
mv		lda brdscale
			cmp #$ff
			bne eilp

			jsr if_unsetirq1
			jsr if_unsetirq2
			lda #$ea
			sta iskes
		
			lda #$60
			sta brdfls
			rts	
eilp
			sta $d020
			sta nff+1

			inc mv+1

eiviz			
			rts								

oye	.byte 80

colorout
			rts	
			
			dec oye
			lda oye
			bne eiviz
		 	inc oye
		 	
			ldx #0
cl2		lda alas1+1,x
			sta alas1,x
			sta ylos1,x
			inx
			cpx #199
			bne cl2
							
nv2		lda scale2
			sta alas1+199
			
			inc nv2+1	
			bne eiee
			inc nv2+2
			
eiee			
			lda nv2+2
			cmp #>scale2+$200
			bne ei

			lda #$ea
			sta brdfls
			
			lda nv2+1
			cmp #170
			bne ei
			
			lda #$60
			sta colorout+1
			
			;PARTTI VALMIS!		
			rts

								
colorin
			ldx #0
cl1		lda alas1+1,x
			sta alas1,x
			inx
			cpx #200
			bne cl1
			
nv1		lda scale1
			sta alas1+199
			
			inc nv1+1	
			bne eie
			inc nv1+2
			

eie
			lda nv1+2
			cmp #>scale1+$200
			bne ei
			
			lda nv1+1
			cmp #170
			bne ei
			
			lda #$60
			sta colorin
			lda #$ea			
			sta chgvec
			sta colorout
			
			lda #$3c
			sta $dd02
			lda #$3b
			sta $d011
			lda #$18
			sta $d018
				
	JSR if_unsetirq1
	ldy #>vec2
	ldx #<vec2
	JSR if_setirq2

ei
			rts  
   			 		

KALLO_LOOP
		ldx #0
ju1	lda #7
		sta $0400,x
		sta $0500,x
		sta $0600,x
		sta $0700-24,x
		lda #0
		sta $d800,x
		sta $d900,x
		sta $da00,x
		sta $db00-24,x
		inx
		bne ju1

		lda #0
		sta $d011			
	  sta $d020
	  sta $d021
				
		lda #$05
		sta $d022
		lda #$00
		sta $d023

		lda #0
		sta $d015
				
		ldx #0
jf	txa 
		sta $4400,x
		sta $44f0,x
		sta $45e0,x
		sta $46d0,x		
		inx
		cpx #240
		bne jf
				
; last line of screen
		ldx #0
be1	lda $4400,x
		sta $47c0,x
		inx
		cpx #40
		bne be1

		ldy #0		
kc	ldx #0
k1	lda $2000,x
		eor #mirror
k2	sta $6000,x
		
k3 	lda $2780,x
		eor #mirror
k4 	sta $6800,x

k5 	lda $2780+$780,x
		eor #mirror
k6 	sta $7000,x

k7 	lda $2780+(2*$780),x
		eor #mirror
k8 	sta $7800,x		
		inx
		bne k1	
		inc k1+2
		inc k2+2		
		inc k3+2
		inc k4+2
		inc k5+2
		inc k6+2
		inc k7+2
		inc k8+2
		iny
		cpy #8
		bne kc

		ldx #0
vim	
		lda $3e00,x
		eor #mirror
		sta $5800,x
		lda $3f00,x
		eor #mirror
		sta $5900,x
		inx 
		bne vim			
			rts
   			 		
 			* = $2000
 			
 			.binary "../assets/kallo/kallo.bin",2 ;gfx\psyke4.fr1 ;gfx\psyke3.fr1
 			
						* = $4000
scale1
;4
.byte $06,$00,$00,$00,$00,$00,$00,$00,$00,$00,$06,$00,$00,$00,$00,$00,$00,$00,$00,$00,$06,$00,$00,$00,$00,$00,$00,$00,$00,$06,$00,$00
.byte $00,$00,$00,$00,$00,$06,$00,$00,$00,$00,$00,$06,$00,$00,$00,$00,$06,$00,$00,$00,$06,$00,$00,$06,$00,$06,$00,$06,$00,$06,$00,$06
.byte $06,$00,$06,$06,$06,$00,$06,$06,$06,$06,$00,$06,$06,$06,$06,$06,$00,$06,$06,$06,$06,$06,$06,$00,$06,$06,$06,$06,$06,$06,$06,$00
.byte $06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$04,$06,$06,$06,$06,$06,$06,$06,$06,$04,$06,$06,$06,$06,$06,$06,$06,$04,$06
.byte $06,$06,$06,$06,$06,$04,$06,$06,$06,$06,$06,$04,$06,$06,$06,$06,$04,$06,$06,$06,$04,$06,$06,$04,$06,$04,$06,$04,$06,$04,$06,$04
.byte $04,$06,$04,$04,$04,$06,$04,$04,$04,$04,$06,$04,$04,$04,$04,$04,$06,$04,$04,$04,$04,$04,$04,$06,$04,$04,$04,$04,$04,$04,$04,$06
.byte $04,$04,$04,$04,$04,$04,$04,$04,$04,$04,$04,$04,$04,$0a,$04,$04,$04,$04,$04,$04,$04,$04,$0a,$04,$04,$04,$04,$04,$04,$04,$0a,$04
.byte $04,$04,$04,$04,$04,$0a,$04,$04,$04,$04,$04,$0a,$04,$04,$04,$04,$0a,$04,$04,$04,$0a,$04,$04,$0a,$04,$0a,$04,$0a,$04,$0a,$04,$0a
.byte $0a,$04,$0a,$0a,$0a,$04,$0a,$0a,$0a,$0a,$04,$0a,$0a,$0a,$0a,$0a,$04,$0a,$0a,$0a,$0a,$0a,$0a,$04,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$04
.byte $0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0f,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0f,$0a,$0a,$0a,$0a,$0a,$0a,$0a,$0f,$0a
.byte $0a,$0a,$0a,$0a,$0a,$0f,$0a,$0a,$0a,$0a,$0a,$0f,$0a,$0a,$0a,$0a,$0f,$0a,$0a,$0a,$0f,$0a,$0a,$0f,$0a,$0f,$0a,$0f,$0a,$0f,$0a,$0f
.byte $0f,$0a,$0f,$0f,$0f,$0a,$0f,$0f,$0f,$0f,$0a,$0f,$0f,$0f,$0f,$0f,$0a,$0f,$0f,$0f,$0f,$0f,$0f,$0a,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0a
.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$07,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$07,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$07,$0f
.byte $0f,$0f,$0f,$0f,$0f,$07,$0f,$0f,$0f,$0f,$0f,$07,$0f,$0f,$0f,$0f,$07,$0f,$0f,$0f,$07,$0f,$0f,$07,$0f,$07,$0f,$07,$0f,$07,$0f,$07
.byte $07,$0f,$07,$07,$07,$0f,$07,$07,$07,$07,$0f,$07,$07,$07,$07,$07,$0f,$07,$07,$07,$07,$07,$07,$0f,$07,$07,$07,$07,$07,$07,$07,$0f
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07

				* = $8000
								
ylos1
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
				

				* = $8100
alas1
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
				.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			
				* = $9500				
scale2	
				.byte $09,$00,$00,$00,$00,$00,$00,$00,$00,$00,$09,$00,$00,$00,$00,$00,$00,$00,$00,$00,$09,$00,$00,$00,$00,$00,$00,$00,$00,$09,$00,$00
				.byte $00,$00,$00,$00,$00,$09,$00,$00,$00,$00,$00,$09,$00,$00,$00,$00,$09,$00,$00,$00,$09,$00,$00,$09,$00,$09,$00,$09,$00,$09,$00,$09
				.byte $09,$00,$09,$09,$09,$00,$09,$09,$09,$09,$00,$09,$09,$09,$09,$09,$00,$09,$09,$09,$09,$09,$09,$00,$09,$09,$09,$09,$09,$09,$09,$00
				.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$08,$09,$09,$09,$09,$09,$09,$09,$09,$08,$09,$09,$09,$09,$09,$09,$09,$08,$09
				.byte $09,$09,$09,$09,$09,$08,$09,$09,$09,$09,$09,$08,$09,$09,$09,$09,$08,$09,$09,$09,$08,$09,$09,$08,$09,$08,$09,$08,$09,$08,$09,$08
				.byte $08,$09,$08,$08,$08,$09,$08,$08,$08,$08,$09,$08,$08,$08,$08,$08,$09,$08,$08,$08,$08,$08,$08,$09,$08,$08,$08,$08,$08,$08,$08,$09
				.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$05,$08,$08,$08,$08,$08,$08,$08,$08,$05,$08,$08,$08,$08,$08,$08,$08,$05,$08
				.byte $08,$08,$08,$08,$08,$05,$08,$08,$08,$08,$08,$05,$08,$08,$08,$08,$05,$08,$08,$08,$05,$08,$08,$05,$08,$05,$08,$05,$08,$05,$08,$05
				.byte $05,$08,$05,$05,$05,$08,$05,$05,$05,$05,$08,$05,$05,$05,$05,$05,$08,$05,$05,$05,$05,$05,$05,$08,$05,$05,$05,$05,$05,$05,$05,$08
				.byte $05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$03,$05,$05,$05,$05,$05,$05,$05,$05,$03,$05,$05,$05,$05,$05,$05,$05,$03,$05
				.byte $05,$05,$05,$05,$05,$03,$05,$05,$05,$05,$05,$03,$05,$05,$05,$05,$03,$05,$05,$05,$03,$05,$05,$03,$05,$03,$05,$03,$05,$03,$05,$03
				.byte $03,$05,$03,$03,$03,$05,$03,$03,$03,$03,$05,$03,$03,$03,$03,$03,$05,$03,$03,$03,$03,$03,$03,$05,$03,$03,$03,$03,$03,$03,$03,$05
				.byte $03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$07,$03,$03,$03,$03,$03,$03,$03,$03,$07,$03,$03,$03,$03,$03,$03,$03,$07,$03
				.byte $03,$03,$03,$03,$03,$07,$03,$03,$03,$03,$03,$07,$03,$03,$03,$03,$07,$03,$03,$03,$07,$03,$03,$07,$03,$07,$03,$07,$03,$07,$03,$07
				.byte $07,$03,$07,$07,$07,$03,$07,$07,$07,$07,$03,$07,$07,$07,$07,$07,$03,$07,$07,$07,$07,$07,$07,$03,$07,$07,$07,$07,$07,$07,$07,$03
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07
				.byte $07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07,$07

brdscale .byte $9,$9,$9,$8,$8,$8,$5,$5,$5,$3,$3,$3,$7,$7,$7,255

				* = $8400				
raz1		
				ldx #5
				dex		  
				bne *-1
				nop
				nop
		;		bit $ea				
razgo		ldy #$00
razgo2	ldx #0

				lda #fnt1

;bdl								
				sta $d018				;4
p1			lda ylos1     ;4*
				sta  col1				;4
pp1 		lda alas1			;4*
				sta col2				;4
				bit $ea					;3
												;23
		
;first normal line		
				lda #fnt1			;2
				sta $d018			;4
p2  		lda ylos1+1 ;4*
				sta col1	    ;4
pp2			lda alas1+1 ;4*	
  			sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
											;63				
;second normal line
				lda #fnt1			;2
				sta $d018			;4
p3			lda ylos1+2 ;4*
				sta col1	    ;4
pp3			lda alas1+2 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
p4			lda ylos1+3 ;4*
				sta col1	    ;4
pp4			lda alas1+3 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
p5			lda ylos1+4 ;4*
				sta col1	    ;4
pp5			lda alas1+4 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
p6			lda ylos1+5 ;4*
				sta col1	    ;4
pp6			lda alas1+5 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
p7			lda ylos1+6 ;4*
				sta col1	    ;4
pp7			lda alas1+6 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
p8			lda ylos1+7 ;4*
				sta col1	    ;4
pp8			lda alas1+7 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt1		  ;2				

;second badline
				sta $d018				;4
p9			lda ylos1+8     ;4*
	  		sta  col1				;4
pp9			lda alas1+8			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+9 ;4*
				sta col1	    ;4
				lda alas1+9 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+10 ;4*
				sta col1	    ;4
				lda alas1+10 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+11 ;4*
				sta col1	    ;4
				lda alas1+11 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+12 ;4*
				sta col1	    ;4
				lda alas1+12 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+13 ;4*
				sta col1	    ;4
				lda alas1+13 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+14 ;4*
				sta col1	    ;4
				lda alas1+14 ;4*	
				sta col2	  	;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+15 ;4*
				sta col1	    ;4
				lda alas1+15 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop				
				lda #fnt1		  ;2				
;
;third badline
				sta $d018				;4
				lda ylos1+16     ;4*
				sta  col1				;4
				lda alas1+16			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+17 ;4*
				sta col1	    ;4
				lda alas1+17 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+18 ;4*
				sta col1	    ;4
				lda alas1+18 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+19 ;4*
				sta col1	    ;4
				lda alas1+19 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+20 ;4*
				sta col1	    ;4
				lda alas1+20 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+21 ;4*
				sta col1	    ;4
				lda alas1+21 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+22 ;4*
				sta col1	    ;4
				lda alas1+22 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;

				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+23 ;4*
				sta col1	    ;4
				lda alas1+23 ;4*	
				sta col2			;4				
				jsr wait2			; strangepos
				nop
				lda #fnt1		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+24     ;4*
				sta  col1				;4
				lda alas1+24			;4*
				sta col2				;4
				bit $ea					;3			
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+25 ;4*
				sta col1	    ;4
				lda alas1+25 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+26 ;4*
				sta col1	    ;4
				lda alas1+26
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+27 ;4*
				sta col1	    ;4
				lda alas1+27 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+28 ;4*
				sta col1	    ;4
				lda alas1+28 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+29 ;4*
				sta col1	    ;4
				lda alas1+29 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+30 ;4*
				sta col1	    ;4
				lda alas1+30 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+31 ;4*
				sta col1	    ;4
				lda alas1+31 ;4*	
				sta col2			;4				
				jsr wait2			;39
				nop				
				lda #fnt1		  ;2				
;5th badline
				sta $d018				;4
				lda ylos1+32     ;4*
				sta  col1				;4
				lda alas1+32			;4*
				sta col2				;4
				bit $ea					;3				
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+33 ;4*
				sta col1	    ;4
				lda alas1+33 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				

				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+34 ;4*
				sta col1	    ;4
				lda alas1+34 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+35 ;4*
				sta col1	    ;4
				lda alas1+35 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+36 ;4*
				sta col1	    ;4
				lda alas1+36 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+37 ;4*
				sta col1	    ;4
				lda alas1+37 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+38 ;4*
				sta col1	    ;4
				lda alas1+38 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+39 ;4*
				sta col1	    ;4
				lda alas1+39 ;4*	
				sta col2			;4				
				jsr wait2			;39 strange				
				nop
				lda #fnt1		  ;2								
;4th badline								 
				sta $d018				;4
				lda ylos1+40     ;4*
				sta  col1				;4
				lda alas1+40			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+41 ;4*
				sta col1	    ;4
				lda alas1+41 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+42 ;4*
				sta col1	    ;4
				lda alas1+42 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+43 ;4*
				sta col1	    ;4
				lda alas1+43 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2	   
				sta $d018			;4
				lda ylos1+44 ;4*
				sta col1	    ;4
				lda alas1+44 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+45 ;4*
				sta col1	    ;4
				lda alas1+45 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018			;4
				lda ylos1+46 ;4*
				sta col1	    ;4
				lda alas1+46 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt1		  ;2				
;
				lda #fnt1			;2
				sta $d018	  	;4
				lda ylos1+47 ;4*
				sta col1	    ;4
				lda alas1+47 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt2		  ;2												
;4th badline												; eka fontin vaihto
				sta $d018				;4
				lda ylos1+48     ;4*
				sta  col1				;4
				lda alas1+48			;4*
				sta col2				;4
				bit $ea					;3				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+49 ;4*
				sta col1	    ;4
				lda alas1+49 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+50 ;4*
				sta col1	    ;4
				lda alas1+50 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+51 ;4*
				sta col1	    ;4
				lda alas1+51 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+52 ;4*
				sta col1	    ;4
				lda alas1+52 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+53 ;4*
				sta col1	    ;4
				lda alas1+53 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+54 ;4*
				sta col1	    ;4
				lda alas1+54 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+55 ;4*
				sta col1	    ;4
				lda alas1+55 ;4*	
				sta col2			;4				
				jsr wait2			;39				strange
				nop
				lda #fnt2		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+56     ;4*
				sta  col1				;4
				lda alas1+56			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+57 ;4*
				sta col1	    ;4
				lda alas1+57 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+58 ;4*
				sta col1	    ;4
				lda alas1+58 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+59 ;4*
				sta col1	    ;4
				lda alas1+59 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+60 ;4*
				sta col1	    ;4
				lda alas1+60 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+61 ;4*
				sta col1	    ;4
				lda alas1+61 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+62 ;4*
				sta col1	    ;4
				lda alas1+62 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+63 ;4*
				sta col1	    ;4
				lda alas1+63 ;4*	
				sta col2			;4							
				;jsr wait1			;39				
				;jsr halt			; another strangeline 
				jsr wait2
				nop
				lda #fnt2		  ;2								
;4th badline
				sta $d018				;4
				lda ylos1+64     ;4*
				sta  col1				;4
				lda alas1+64			;4*
				sta col2				;4
				bit $ea					;3			
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+65 ;4*
				sta col1	    ;4
				lda alas1+65 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+66 ;4*
				sta col1	    ;4
				lda alas1+66 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+67 ;4*
				sta col1	    ;4
				lda alas1+67 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+68 ;4*
				sta col1	    ;4
				lda alas1+68 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+69 ;4*
				sta col1	    ;4
				lda alas1+69 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+70 ;4*
				sta col1	    ;4
				lda alas1+70 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+71 ;4*
				sta col1	    ;4
				lda alas1+71 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt2		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+72     ;4*
				sta  col1				;4
				lda alas1+72			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+73 ;4*
				sta col1	    ;4
				lda alas1+73 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+74 ;4*
				sta col1	    ;4
				lda alas1+74 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+75 ;4*
				sta col1	    ;4
				lda alas1+75 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+76 ;4*
				sta col1	    ;4
				lda alas1+76 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+77 ;4*
				sta col1	    ;4
				lda alas1+77 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+78 ;4*
				sta col1	    ;4
				lda alas1+78 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+79 ;4*
				sta col1	    ;4
				lda alas1+79 ;4*	
				sta col2			;4				
				jsr wait2			;39				 strange
				nop
				lda #fnt2		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+80     ;4*
				sta  col1				;4
				lda alas1+80			;4*
				sta col2				;4
				bit $ea					;3
;				
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+81 ;4*
				sta col1	    ;4
				lda alas1+81 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+82 ;4*
				sta col1	    ;4
				lda alas1+82 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+83 ;4*
				sta col1	    ;4
				lda alas1+83 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+84 ;4*
				sta col1	    ;4
				lda alas1+84 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+85 ;4*
				sta col1	    ;4
				lda alas1+85 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+86 ;4*
				sta col1	    ;4
				lda alas1+86 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+87 ;4*
				sta col1	    ;4
				lda alas1+87 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt2		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+88     ;4*
				sta  col1				;4
				lda alas1+88			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+89 ;4*
				sta col1	    ;4
				lda alas1+89 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+90 ;4*
				sta col1	    ;4
				lda alas1+90 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+91 ;4*
				sta col1	    ;4
				lda alas1+91 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+92 ;4*
				sta col1	    ;4
				lda alas1+92 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+93 ;4*
				sta col1	    ;4
				lda alas1+93 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+94 ;4*
				sta col1	    ;4
				lda alas1+94 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt2		  ;2				
;
				lda #fnt2			;2
				sta $d018			;4
				lda ylos1+95 ;4*
				sta col1	    ;4
				lda alas1+95 ;4*	
				sta col2			;4				
				jsr wait2			;39				;strange
				nop
				lda #fnt3		  ;2				
;4th badline								
				sta $d018				;4			; toka fontin vaihto
				lda ylos1+96     ;4*
				sta  col1				;4
				lda alas1+96			;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+97 ;4*
				sta col1	    ;4
				lda alas1+97 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+98 ;4*
				sta col1	    ;4
				lda alas1+98 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+99 ;4*
				sta col1	    ;4
				lda alas1+99 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2				
				sta $d018			;4
				lda ylos1+100 ;4*
				sta col1	    ;4
				lda alas1+100 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+101 ;4*
				sta col1	    ;4
				lda alas1+101 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+102 ;4*
				sta col1	    ;4
				lda alas1+102 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+103 ;4*
				sta col1	    ;4
				lda alas1+103 ;4*	
				sta col2			;4				
				jsr wait2			;39				; strange
				lda #fnt3		  ;2
				nop				
;4th badline
				sta $d018				;4
				lda ylos1+104     ;4*
				sta  col1				;4
				lda alas1+104		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+105 ;4*
				sta col1	    ;4
				lda alas1+105 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+106 ;4*
				sta col1	    ;4
				lda alas1+106 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+107 ;4*
				sta col1	    ;4
				lda alas1+107 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+108 ;4*
				sta col1	    ;4
				lda alas1+108 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+109 ;4*
				sta col1	    ;4
				lda alas1+109 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+110 ;4*
				sta col1	    ;4
				lda alas1+110 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+111 ;4*
				sta col1	    ;4
				lda alas1+111 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt3		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+112     ;4*
				sta  col1				;4
				lda alas1+112		;4*
				sta col2				;4
				bit $ea					;3			
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+113 ;4*
				sta col1	    ;4
				lda alas1+113 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+114 ;4*
				sta col1	    ;4
				lda alas1+114 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+115 ;4*
				sta col1	    ;4
				lda alas1+115 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+116 ;4*
				sta col1	    ;4
				lda alas1+116 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+117 ;4*
				sta col1	    ;4
				lda alas1+117 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+118 ;4*
				sta col1	    ;4
				lda alas1+118 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+119 ;4*
				sta col1	    ;4
				lda alas1+119 ;4*	
				sta col2			;4				
				jsr wait2			;39		stranger
				nop
				lda #fnt3		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+120     ;4*
				sta  col1				;4
				lda alas1+120		;4*
				sta col2				;4
				bit $ea					;3				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+121 ;4*
				sta col1	    ;4
				lda alas1+121 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2							
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+122 ;4*
				sta col1	    ;4
				lda alas1+122 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+123 ;4*
				sta col1	    ;4
				lda alas1+123 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+124 ;4*
				sta col1	    ;4
				lda alas1+124 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+125 ;4*
				sta col1	    ;4
				lda alas1+125 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+126 ;4*
				sta col1	    ;4
				lda alas1+126 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+127 ;4*
				sta col1	    ;4
				lda alas1+127 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt3		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+128     ;4*
				sta  col1				;4
				lda alas1+128		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+129 ;4*
				sta col1	    ;4
				lda alas1+129 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+130 ;4*
				sta col1	    ;4
				lda alas1+130 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+131 ;4*
				sta col1	    ;4
				lda alas1+131 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+132 ;4*
				sta col1	    ;4
				lda alas1+132 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+133 ;4*
				sta col1	    ;4
				lda alas1+133 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+134 ;4*
				sta col1	    ;4
				lda alas1+134 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+135 ;4*
				sta col1	    ;4
				lda alas1+135 ;4*	
				sta col2			;4				
				jsr wait2			;39 ; stranger				
				nop
				lda #fnt3		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+136     ;4*
				sta  col1				;4
				lda alas1+136		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+137 ;4*
				sta col1	    ;4
				lda alas1+137 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+138 ;4*
				sta col1	    ;4
				lda alas1+138 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+139 ;4*
				sta col1	    ;4
				lda alas1+139 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+140 ;4*
				sta col1	    ;4
				lda alas1+140 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+141 ;4*
				sta col1	    ;4
				lda alas1+141 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+142 ;4*
				sta col1	    ;4
				lda alas1+142 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt3		  ;2				
;
				lda #fnt3			;2
				sta $d018			;4
				lda ylos1+143 ;4*
				sta col1	    ;4
				lda alas1+143 ;4*	
				sta col2			;4				
				;jsr wait1			;39				
				jsr wait2     ;strange
				nop
				lda #fnt4		  ;2				
;4th badline											; kolmas fontin vaihto
				sta $d018,x				;4
				lda ylos1+144     ;4*
				sta  col1				;4
				lda alas1+144		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+145 ;4*
				sta col1	    ;4
				lda alas1+145 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+146 ;4*
				sta col1	    ;4
				lda alas1+146 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+147 ;4*
				sta col1	    ;4
				lda alas1+147 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+148 ;4*
				sta col1	    ;4
				lda alas1+148 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+149 ;4*
				sta col1	    ;4
				lda alas1+149 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+150 ;4*
				sta col1	    ;4
				lda alas1+150 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+151 ;4*
				sta col1	    ;4
				lda alas1+151 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt4		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+152     ;4*
				sta  col1				;4
				lda alas1+152		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+153 ;4*
				sta col1	    ;4
				lda alas1+153 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+154 ;4*
				sta col1	    ;4
				lda alas1+154 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+155 ;4*
				sta col1	    ;4
				lda alas1+155 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+156 ;4*
				sta col1	    ;4
				lda alas1+156 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+157 ;4*
				sta col1	    ;4
				lda alas1+157 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+158 ;4*
				sta col1	    ;4
				lda alas1+158 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+159 ;4*
				sta col1	    ;4
				lda alas1+159 ;4*	
				sta col2			;4				
				jsr wait2			;39 ; stranger				
				nop
				lda #fnt4		  ;2							
;4th badline
				sta $d018				;4
				lda ylos1+160     ;4*
				sta  col1				;4
				lda alas1+160		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+161 ;4*
				sta col1	    ;4
				lda alas1+161 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+162 ;4*
				sta col1	    ;4
				lda alas1+162 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+163 ;4*
				sta col1	    ;4
				lda alas1+163 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+164 ;4*
				sta col1	    ;4
				lda alas1+164 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+165 ;4*
				sta col1	    ;4
				lda alas1+165 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+166 ;4*
				sta col1	    ;4
				lda alas1+166 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+167 ;4*
				sta col1	    ;4
				lda alas1+167 ;4*	
				sta col2			;4				
				jsr wait2		;39				strange
				nop
				lda #fnt4		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+168     ;4*
				sta  col1				;4
				lda alas1+168		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+169 ;4*
				sta col1	    ;4
				lda alas1+169 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+170 ;4*
				sta col1	    ;4
				lda alas1+170 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+171 ;4*
				sta col1	    ;4
				lda alas1+171 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+172 ;4*
				sta col1	    ;4
				lda alas1+172 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+173 ;4*
				sta col1	    ;4
				lda alas1+173 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+174 ;4*
				sta col1	    ;4
				lda alas1+174 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+175 ;4*
				sta col1	    ;4
				lda alas1+175 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				nop
				lda #fnt4		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+176     ;4*
				sta  col1				;4
				lda alas1+176		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+177 ;4*
				sta col1	    ;4
				lda alas1+177 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+178 ;4*
				sta col1	    ;4
				lda alas1+178 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+179 ;4*
				sta col1	    ;4
				lda alas1+179 ;4*	
				sta col2		  ;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+180 ;4*
				sta col1	    ;4
				lda alas1+180 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+181 ;4*
				sta col1	    ;4
				lda alas1+181 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+182 ;4*
				sta col1	    ;4
				lda alas1+182 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+183 ;4*
				sta col1	    ;4
				lda alas1+183 ;4*	
				sta col2			;4				
				;jsr wait1			;39				
				jsr wait2     ;strange
				nop
				lda #fnt4		  ;2				
;4th badline
				sta $d018				;4
				lda ylos1+184     ;4*
				sta  col1				;4
				lda alas1+184		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+185 ;4*
				sta col1	    ;4
				lda alas1+185 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+186 ;4*
				sta col1	    ;4
				lda alas1+186 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+187 ;4*
				sta col1	    ;4
				lda alas1+187 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+188 ;4*
				sta col1	    ;4
				lda alas1+188 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+189 ;4*
				sta col1	    ;4
				lda alas1+189 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+190 ;4*
				sta col1	    ;4
				lda alas1+190 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt4		  ;2				
;
				lda #fnt4			;2
				sta $d018			;4
				lda ylos1+191 ;4*
				sta col1	    ;4
				lda alas1+191 ;4*	
				sta col2			;4				
				jsr wait2			;39				
				bit $ea
				
				lda #fnt5		  ;2				
;4th badline					 	; neljäs fontin vaihto
				sta $d018				;4
				lda ylos1+192     ;4*
				sta  col1				;4
				lda alas1+192		;4*
				sta col2				;4
				bit $ea					;3
;
				lda #fnt5			;2
				sta $d018			;4
				lda ylos1+193 ;4*
				sta col1	    ;4
				lda alas1+193 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt5		  ;2				
;
				lda #fnt5			;2
				sta $d018			;4
				lda ylos1+194 ;4*
				sta col1	    ;4
				lda alas1+194 ;4*	
				sta col2	   	;4				
				jsr wait1			;39				
				lda #fnt5		  ;2				
;
				lda #fnt5			;2
				sta $d018			;4
				lda ylos1+195 ;4*
				sta col1	    ;4
				lda alas1+195 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt5		  ;2				
;
				lda #fnt5			;2
				sta $d018			;4
				lda ylos1+196 ;4*
				sta col1	    ;4
				lda alas1+196 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				lda #fnt5		  ;2				
;			
				lda #fnt5			;2
				sta $d018			;4
				lda ylos1+197 ;4*
				sta col1	    ;4
				lda ylos1+197 ;4*	
				sta col2			;4				
				jsr wait1			;39				
				rts
				lda #fnt5		  ;2				
	
				lda #fnt5			;2          ; jos alin rivi sekoilee niin tästä alaspäin saa säädeltyä...
				sta $d018			;4
				lda ylos1+198 ;4*
				sta col1	    ;4
				lda alas1+198 ;4*	
				sta col2	
			;	nop	
				rts
				jsr wait2			;39				
;				lda #fnt5		  ;2				

				;lda #fnt5			;2
				;sta $d018			;4
				lda ylos1+199 ;4*
				sta col2	    ;4				
				lda alas1+199 ;4*	
				sta col2			;4				
;				jsr wait1			;39				
	;			lda #fnt5		  ;2				
				
				lda ylos1
			;	sta $d020
			;	sta $d021
				rts
				
				rts
				
halt		rts
			
wait2		
				nop
				nop
				nop
				nop
				nop
				jsr halt
				rts
				
wait1		bit $ea ;3
				jsr halt 
				jsr halt
				rts ;6
