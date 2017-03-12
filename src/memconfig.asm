framecounter = $02 ; size 2
musicflag = $04
executewait = $05 ; size 2

; ainakin 0xF9-0xFF musaplayerilla

irqfuk_begin = $F000

; irq alustus yms.
if_startirq = irqfuk_begin   + 0

; asettaa ylempaan musarutiinikutsuun oman callbackin (X/Y rekuissa osoite)
if_setirq1 = irqfuk_begin    + 3

; poistaa ylemman musarutiinikutsun callbackin
if_unsetirq1 = irqfuk_begin  + 6

; samat alemmalle musarutiinikutsulle
if_setirq2 = irqfuk_begin    + 9
if_unsetirq2 = irqfuk_begin  + 12

; kopioi musiikin E000 -> 1000, alustaa sen ja aloittaa soittamisen
if_startmusic = irqfuk_begin + 15

; lataa seuraavan partin, odottelee framecounterin arvoa ja ajaa sen
; (X/Y rekuissa framecounterin vertailuarvo)
if_executenext = irqfuk_begin + 18

; sama kuin ylempi mutta ajaa seuraavan partin valittomasti
if_executenext_nowait = irqfuk_begin + 21

if_startvec = $fff0