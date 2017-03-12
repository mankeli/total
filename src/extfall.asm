.include "memconfig.asm"


; spritet kopioidaan > $4b00-$4fff
; bitmap $6000-$8000
; generoitu unrolled koodi $2000-$3e40
; generoitu sin scrollaus $5800-5e00
; datoja $8000-$9000
; spritet $9000-$9400

bfal = $2000
sinscr = $5800 

*=if_startvec

JMP EXTFALL_loop


        *=$5000	
extfall_loop

;		jsr if_executenext_nowait


		lda #0
		sta $d015
		sta $d01d
		sta $d017
		sta $d010

		jsr gensinscr

		ldx #0
jeicj	lda pirst,x
		sta $4b00,x
		lda pirst+$100,x
		sta $4c00,x
		lda pirst+$200,x
		sta $4d00,x
		lda pirst+$300,x
		sta $4e00,x
		lda pirst+$400,x
		sta $4f00,x		
		inx
		bne jeicj

;		lda #$3f
;		sta $47f8
;		sta $47f9
;		sta $47fa
;		sta $47fb
;		sta $47fc
;		sta $47fd
;		sta $47fe
;		sta $47ff

		lda #$00
		ldx #64
jekkusprite
		sta $4b00-64-1,x
		dex
		bne jekkusprite
		
		jsr initz
		
		lda #$30
		sta $d011

	JSR if_unsetirq2

	ldy #>EXTFALL_irq
	ldx #<EXTFALL_irq
	JSR if_setirq2

;EXTFALL_irq

terve
	jsr iskes
	jmp terve

				lda #$f6
bax			cmp $d012
				bne bax
		
			ldx #10
		venaarasteriii
			dex
			bne venaarasteriii

				
				lda #$0e
				sta $d020		

;				lda #$30
;bax2			cmp $d012
;			bne	bax2

		
;		jmp terve
	jsr iskes
	jmp terve

iskes:
	rts
	pla
	pla

;	.byte 2

	JSR if_unsetirq1
	JSR if_unsetirq2

	LDA #00
	sta $d011
	sta $d020
	sta $d021


	jmp if_executenext_nowait





			
initz
				lda #0
				sta $d011

				lda #0
				sta $d016
				
				lda #6
				sta $d020
				sta $d021
				
				ldx #0
				lda #$16
huce		sta $4400,x
				sta $4500,x
				sta $4600,x
				sta $4700,x
				inx
				bne huce

				lda #$3f
				sta $47f8
				sta $47f9
				sta $47fa
				sta $47fb

				sta $47fc
				sta $47fd
				sta $47fe
				sta $47ff

				ldx #0
coli		lda #$46
				sta $4400,x
				sta $4400+(1*40),x
				lda #$e6
				sta $4400+(2*40),x
				lda #$36
				sta $4400+(3*40),x
				lda #$76
				sta $4400+(4*40),x
				inx
				cpx #40
				bne coli
					
				ldy #0
lop2		ldx #0
				lda #0
lop1		sta $6000,x 
				inx
				bne lop1
				inc lop1+2
				iny
				cpy #32
				bne lop2
				
				lda #$18
				sta $d018
				lda #$3d
				sta $dd02
				
				jsr genytab1
				
				jsr genpixtabs
								
				jsr gens
				
				jsr spriteup
				
				lda #255
				sta sinix+255
				rts		
								
EXTFALL_irq
vec	
				cli
;				inc $d020
				lda #$06
				sta $d020

				jsr bfal+2 


				jsr goo2
	
				jsr span	

;				dec $d020			

;				lda #$f6
;ba			cmp $d012
;				bne ba
				
;				ldy #12
;ww2			dey
;				bne ww2
				
;				lda #$0e
;				sta $d020		


				rts
							
spw2		.byte 1
spw			.byte 230
				
wt1			.byte 1
wt2			.byte 50
wt3			.byte 2
wt4			.byte 5
				
gens
				lda #<bfal
				sta $50
				lda #>bfal
				sta $51
				
next
				ldy #0
				lda #$a0
				sta ($50),y
				iny
lks			lda #$00
				sta ($50),y
;				
				lda $50
				clc
				adc #2
				sta $50
				lda $51
				adc #0
				sta $51
				
ng1			ldy #0
				
;				
neap		lda apu	
skp			sta count
				
nxtpxl	ldx #0
				lda hisk,x
				sta hiz+2
				lda hisk2,x
				sta hiz2+2
				
				
nef			ldx #$00
				lda pixel,x
				sta ch0+1
				lda unpixel,x
				sta ch0b+1

nef2		ldx #0
				lda #250
				sec
hiz			sbc logo,x
				sta ffal1+1
				
				lda #250
				sec
hiz2		sbc logo+$100,x
				sta ffal2+1
				
				lda count
				beq apuskip
				
lopex			
				ldy #0
				ldx #0
gf1			lda potk,x
				sta ($50),y
				iny
				inx
				cpx #end_potk-potk
				bne gf1
				
				lda $50
				clc
				adc #end_potk-potk
				sta $50
				lda $51
				adc #0
				sta $51
				
				inc nxtpxl+1
				
				dec count
				lda count
				bne nxtpxl
	
apuskip		
				lda #0
				sta nxtpxl+1
				inc nef+1
				inc nef+1  ; skip every second
				inc nef2+1
				inc ng1+1
				
				lda fcount
				and #3
				cmp #3
				bne einextcol
				
				lda lks+1
				clc
				adc #8
				sta lks+1
				bne eiyliv
				inc yl2+2
				inc yl2b+2
eiyliv
;				
;				inc yl1+2
;				inc yl2+2
;				inc yl1b+2
;				inc yl2b+2

einextcol
				inc neap+1
		;		inc neap+1
				
				inc fcount
				lda fcount
				cmp #128+29 ;width
				beq ng2
				jmp next
				rts
				
ng2
				ldy #0
				lda #$60
				sta ($50),y
				rts					
				
fcount 	.byte 0
				
hisk		.byte >logo			,>logo+$200,>logo+$400
hisk2		.byte >logo+$100,>logo+$300,>logo+$500
				
count		.byte 0

gensinscr
				lda #<sinscr
				sta $50
				lda #>sinscr
				sta $51
				
				ldy #0
backu	
				tya
				pha							
				ldy #0
				
				ldx #0
lp1			lda sins,x
				sta ($50),y
				iny
				inx
				cpx #sinse-sins
				bne lp1
				
				lda $50
				clc
				adc #6
				sta $50
				lda $51
				adc #0
				sta $51
		
				inc ss1+1
				bne ei1
				inc ss1+2
				
ei1
				inc ss2+1
				bne ei2
				inc ss2+2
ei2				
				pla				
				tay
				iny
				cpy #239
				bne backu
		
				ldy #0
				lda #$60
				sta ($50),y
				rts

sins
ss1			lda sini+1
ss2			sta sini
sinse

goo2

;				ldx #0
;hehu		lda sini+1,x
;				sta sini,x
;				inx
;				cpx #239
;				bne hehu
				
				jsr sinscr
				
mono		lda sinix
				cmp #$ff
				beq eject
				sta sini+239
				inc mono+1		
				rts
				
eject		dec mono+1
				rts
	
went		.byte 128
went2		.byte 2
				
potk
ffal1 	ldx sini
yl1			lda ylos,x
				sta $50
yl2			lda yhis,x
				sta $51		
				
ch0			lda #%10000000
				ora ($50),y
				sta ($50),y				

ffal2		ldx sini			
yl1b		lda ylos,x
				sta $50
yl2b		lda yhis,x
				sta $51		
		
				lda ($50),y
ch0b		and #0
				sta ($50),y	
						
end_potk
				rts

genytab1
				lda #0
				sta yls+1	
				lda #$60
				sta yhs+1
				jsr geny
				
				lda #8
				sta yls+1	
				lda #$60
				sta yhs+1
				jsr geny
				rts
				
geny
				
				ldx #0
yls			lda #0
ns				sta ylos,x
yhs			lda #$60           ;bitmap
ns2				sta yhis,x
				clc
				adc #1
				sta yhis+$100,x
				
				inc yls+1
				lda yls+1
				and #7
				bne no
	
				lda yls+1
				clc
				adc #$38
				sta yls+1
							
				lda yhs+1
				adc #1
				sta yhs+1						
				
no				
				inx
				cpx #200
				bne yls
				
				ldx #200
jie			lda #$60
				sta yhis,x
				sta yhis+$100,x
				sta yhis+$200,x
				inx
				bne jie
			
				inc ns+2
				inc ns+2
				inc ns2+2
				inc ns2+2
				rts						
								
				
genpixtabs
				lda #$80				
				ldx #0
jiu			
				sta pixel,x
				eor #255
				sta unpixel,x
				eor #255
				lsr
				bcc noz
				lda #$80
noz
				inx
				bne jiu												
eic2		
			rts
		
span		
				dec spw2
				bne eic2
				inc spw2
				
				dec spw
				bne eic2
				
				lda #3
				sta spw
				
spo 		ldx #0
				lda spani,x
				cmp #255
				beq endsp1
				clc
				adc #$2b
				sta $47f8
				inc spo+1
				jmp sk1
endsp1	
				lda #217
				sta $d000
				
sk1 		ldx #0
				lda spaniw1,x
				cmp #255
				beq endsp2
				clc
				adc #$2b
				sta $47f9
				inc sk1+1
				jmp sk2
endsp2	
				lda #241
				sta $d002
				
sk2 		ldx #0
				lda spaniw2,x
				cmp #255
				beq endsp3
				clc
				adc #$2b
				sta $47fa
				inc sk2+1
				jmp sk3
endsp3	
				lda #9
				sta $d004				
				lda #%00000100
				sta $d010
				
sk3 		ldx #0
				lda spaniw3,x
				cmp #255
				beq endsp4
				clc
				adc #$2b
				sta $47fb
				inc sk3+1
				jmp sk4
endsp4
				lda #33
				sta $d006
				lda #%00001100
				sta $d010
				
sk4			ldx #0
				lda spaniw4,x
				cmp #255
				beq endsp5
				clc
				adc #$2b
				sta $47fc
				inc sk4+1
				jmp sk5
endsp5
				lda #57
				sta $d008
				lda #%00011100
				sta $d010
				
sk5			ldx #0
				lda spaniw5,x
				cmp #255
				beq sk6
				clc
				adc #$2b
				sta $47fd
				inc sk5+1
				
sk6			ldx #0
				lda spaniw6,x
				cmp #255
				beq sk7
				clc
				adc #$2b
				sta $47fe
				inc sk6+1
				
sk7			ldx #0
				lda spaniw7,x
				cmp #255
				beq sk8
				clc
				adc #$2b
				sta $47ff
				inc sk7+1		
				
sk8			ldx #0
				lda spaniw8,x
				beq eivi1 ; 0 = do nothing
				cmp #255
				beq sk9
				clc
				adc #$2b
				sta $47f8				
eivi1		inc sk8+1	
						
sk9		
				ldx #0
				lda spaniw9,x
				beq eivi2 ; 0 = do nothing
				cmp #255
				beq sk10		
				clc
				adc #$2b
				sta $47f9	
eivi2		inc sk9+1

sk10		ldx #0
				lda spaniw10,x
				beq eivi3 ; 0 = do nothing
				cmp #255
				beq sk11
				clc
				adc #$2b
				sta $47fa
eivi3		inc sk10+1

sk11		ldx #0
				lda spaniw11,x
				beq eivi4 ; 0 = do nothing
				cmp #255
				beq sk12
				clc
				adc #$2b
				sta $47fb
eivi4		inc sk11+1

sk12		ldx #0
				lda spaniw12,x
				beq eivi5 ; 0 = do nothing
				cmp #255
				beq sk13_end
				clc
				adc #$2b
				sta $47fc
eivi5		inc sk12+1

sk13		
eic
				rts


sk13_end
				lda #$ea
				sta iskes
				rts

spaniw13 .byte 0,0,0,0,0
spaniw12 .byte 0,0,0,0,0
spaniw11 .byte 0,0,0,0,0
spaniw10 .byte 0,0,0,0,0
spaniw9	 .byte 0,0,0,0,0
spaniw8	 .byte 0,0,0,0,0
spaniw7	 .byte 0,0,0,0,0
spaniw6	 .byte 0,0,0,0,0
spaniw5	 .byte 0,0,0,0,0
spaniw4	 .byte 0,0,0,0,0	
spaniw3	 .byte 0,0,0,0,0
spaniw2	 .byte 0,0,0,0,0
spaniw1	 .byte 0,0,0,0,0

spani		.byte 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
				.byte 255,255,255,255,255,255,255,255,255,255,255,255
				
spriteup
				lda #$ff
				sta $d015
				
				ldx #0
				lda #0
hec		sta $0340,x
				inx
				cpx #63
				bne hec
				
				lda #25
				sta $d000
				clc
				adc #24
				sta $d002
				adc #24
				sta $d004
				adc #24
				sta $d006
				adc #24
				sta $d008
				adc #24
				sta $d00a
				adc #24
				sta $d00c
				adc #24
				sta $d00e
				
				ldy #0
				ldx #0
splo
				lda #224
				sta $d001,y
				lda #$01
				sta $d027,x
				lda #$3f
				sta $07f8,x
				iny
				iny
				inx
				cpx #8
				bne splo
				rts	
				
cw1			.byte 255
cw2			.byte 80

			* = $8000		
pixel					

			* = $8100 
unpixel	
				
		* = $8200
ylos
	
		* = $8300
		
yhis
			
			* = $8600
logo		.binary "../assets/extfall/extend3.bin" ;.binary extend.bin

			
apu
	.binary "../assets/extfall/extendapu3.bin"


sini .fill 256,0 

		* = sini+$100
sinix		.binary "../assets/extfall/stable3.bin"
				;.binary stable3.bin

pirst
				.binary "../assets/extfall/pirsto.bin"
