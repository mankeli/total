#pragma once

value_t framecounter = 0x02; // size 2
value_t musicflag = 0x04;
value_t executewait = 0x05; // size 2

// ainakin 0xF9-0xFF musaplayerilla

value_t irqfuk_begin = 0xF000;

value_t if_startirq = irqfuk_begin   + 0;
value_t if_setirq1 = irqfuk_begin    + 3;
value_t if_unsetirq1 = irqfuk_begin  + 6;
value_t if_setirq2 = irqfuk_begin    + 9;
value_t if_unsetirq2 = irqfuk_begin  + 12;
value_t if_startmusic = irqfuk_begin + 15;
value_t if_executenext = irqfuk_begin + 18;
value_t if_executenext_nowait = irqfuk_begin + 21;

value_t if_startvec = 0xFFF0;


//#define NO_LOADING

