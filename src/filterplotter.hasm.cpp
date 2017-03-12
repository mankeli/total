#include "glm.h"
#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

#include "../assets/plotteri_taustavari4.c"

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

struct char_t
{
	int data[64];
};

char_t charset[256];
int tables[64*256];

void emitcharset()
{
	int i,x,y;

	for (i = 0; i < 256; i++)
	{

		int merq;
		int *data = charset[i].data;

		for (y = 0; y < 8; y++)
		{
			int merq = 0;
			for (x = 0; x < 8; x++)
			{
				//merq |= data[(y) * 8 + (x)] << x;
				merq |= (data[(y) * 8 + (x)] > 0 ? 1 : 0) << (7-x);
			}

			B(merq);
		}

	}
}

void emitfiltertables()
{
	int i,j;
	for (i = 0; i < 64; i++)
	{
		LPC("ftab%i", i);
	//	PRINTPOS("ftab%i",i);
		for (j = 0; j < 256; j++)
			B(tables[i*256+j]);
	}
	LPC("ftab%i", 64);

}

value_t runningflag = 0x10;
value_t cunt = 0x11; // 2
value_t cunt2 = 0x13; // 2

value_t cunt_b = 0x15; // 2
value_t cunt2_b = 0x17; // 2

value_t cuntxx = 0x19; // 2
value_t cuntxx2 = 0x1b; // 2


value_t xtmp = 0x20;
value_t ytmp = 0x21;

value_t xtmp_ch = 0x22;

value_t plotterloopcounter = 0x23; // 2

value_t curscreen = 0x25;

value_t clearptr = 0x26; // 2
value_t lastclearptr = 0x28; // 2

value_t routine_start = 0x60; // 0x80


void genis()
{

	int i;

	CTX("valo")

	startvec.begin(if_startvec);
	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

	actualprg.begin(0x2000);


LPC("charset")
	emitcharset();

	SEG->setpc(0x2C00);

	PRINTPOS("actualbeg")


LPC("actualbeg")


	//JSR(if_startmusic);

	MOV8i(0xd018, 0x18);
	MOV8i(0xdd02, 0x3c);

	MOV8i(0xd011, 0x00);

	MOV8i(0xd020, 0x00);
	MOV8i(0xd021, 0x00);


	MOV8zi(runningflag, 0x02);
	MOV8zi(curscreen, 0);

	setirq(L("filtertest"), 2);

	LDXi(0);
LPC("fillloop3")
	LDAx(L("colmem")+0x000)
	STAx(0xD800);
	LDAx(L("colmem")+0x100)
	STAx(0xD900);
	LDAx(L("colmem")+0x200)
	STAx(0xDA00);
	LDAx(L("colmem")+0x300)
	STAx(0xDB00);

	INX();
	BNE(L("fillloop3"))



	LDXi(0x00);
	//TXA();
	LDAi(0);
LPC("fillloop")
	STAx(0x400);
	STAx(0x500);
	STAx(0x600);
	STAx(0x700);
	STAx(0x2800);
	STAx(0x2900);
	STAx(0x2A00);
	STAx(0x2B00);
	INX();
	BNE(L("fillloop"))

	MOV16zi(lastclearptr, L("clear1"));
	MOV16zi(clearptr, L("clear1"));



// TÄSTÄ ALKAA PÄÄ OHJELMA

	LPC("loopba")
//	LDAz(runningflag)
//	BNE(L("loopba"))

	//MOV8i(0xd020,3);

// joo varmaan on hyvä suorittaa näyttömuistin switch ja sitten heti sen jälkeen alottaa toisen bufferin clearaus ja piirto??+

#if 1
	LDYi(0)
	LDAi(0x8D) // sta abs
	STAizy(lastclearptr)
	MOV16z(lastclearptr, clearptr);
#endif


	LDAz(curscreen)
	BNE(L("settobuf2"))
	LPC("settobuf1")

	MOV8zi(curscreen, 1);
	MOV8i(0xd018, 0xA8);

#if 0
	LDXi(0x00);
	//TXA();
	LDAi(0);
LPC("fillloop")
	STAx(0x400);
	STAx(0x500);
	STAx(0x600);
	STAx(0x700);
	INX();
	BNE(L("fillloop"))
#endif
	LDAi(0);
	JSR(L("clear1"))

	LDAi(0x04);
	STA(L("plottgtbufmod")+1)

	MOV16zi(clearptr, L("clear1"));

	JMP(L("startplotter"))


	LPC("settobuf2")

	MOV8zi(curscreen, 0);
	MOV8i(0xd018, 0x18);

#if 0
	LDXi(0x00);
	//TXA();
	LDAi(0);
LPC("fillloop")
	STAx(0x2800);
	STAx(0x2900);
	STAx(0x2A00);
	STAx(0x2B00);
	INX();
	BNE(L("fillloop"))
#endif
	LDAi(0);
	JSR(L("clear2"))

	LDAi(0x28);
	STA(L("plottgtbufmod")+1)

	MOV16zi(clearptr, L("clear2"));

	JMP(L("startplotter"))



LPC("startplotter")


	//MOV8i(0xd020,2);

	MOV16z(cunt_b,cunt);
	MOV16z(cunt2_b,cunt2);

	LDXz(cuntxx+1)
	LDAx(L("bigsintabl"))
	STA(L("cuntbmod")+1)
	LDAx(L("bigsintabh"))
	STA(L("cuntbmod2")+1)

	LDXz(cuntxx2+1)
	LDAx(L("bigsintabl"))
	STA(L("cunt2bmod")+1)
	LDAx(L("bigsintabh"))
	STA(L("cunt2bmod2")+1)


	PRINTPOS("pisteitden määr")
	//LDAi(0x54);
	//LDAi(0xdf);
	LDAi(0xd0);
	STAz(plotterloopcounter)
	LDAi(0x01);
	STAz(plotterloopcounter+1)

PRINTPOS("plotterloop alkaa")

LPC("plotterloop")

//	CLC();
	LDAz(cunt_b+0)
LPC("cuntbmod")
	ADCi(0x74);
	STAz(cunt_b+0)
	LDAz(cunt_b+1)
LPC("cuntbmod2")
	ADCi(0x01);
	STAz(cunt_b+1)
	TAY();

	//CLC();
	LDAz(cunt2_b+0)
LPC("cunt2bmod")
	ADCi(0x32);
	STAz(cunt2_b+0)
	LDAz(cunt2_b+1)
LPC("cunt2bmod2")
	ADCi(0x01);
	STAz(cunt2_b+1)
	TAX();
	LDAx(L("sintab2"))
	TAX();


	CLC();
	LDAy(L("sintab_and8"))
	ADCx(L("jyge"))
	STA(L("ftabmod")+2)


	CLC();
	LDAx(L("40multab_l"))
	ADCy(L("sintab_ch"))
	STA(L("plotmod_1")+1)
	STA(L("plotmod_2")+1)

	LDAx(L("40multab_h"))
LPC("plottgtbufmod")
	ADCi(0x04)
	STA(L("plotmod_1")+2)
	STA(L("plotmod_2")+2)
	

LPC("plotmod_1")
	LDX(0xe000);

	BEQ(L("addtoclear"))

LPC("ftabmod")
	LDAx(L("ftab%i",0))

	//LDAi(0xFF);

LPC("plotmod_2")
	STA(0xe000);

//	DECz(plotterloopcounter+0)
//	BNE(L("plotterloop"))
//	DECz(plotterloopcounter+1)

	LDAz(runningflag)
	BNE(L("plotterloop"))

	JMP(L("movetoscreen"))


LPC("addtoclear")
	LDYi(1)
	LDA(L("plotmod_1")+1)
	STAizy(clearptr)
	INY();
	LDA(L("plotmod_1")+2)
	STAizy(clearptr)


	ADD16zi(clearptr, 3);

	JMP(L("ftabmod"))

PRINTPOS("plotterloop looppuu")

PRINTPOS("clear1")
LPC("clear1")
	for (i = 0; i < 256; i++)
	{
		STA(0x400);
	}
	RTS();

PRINTPOS("clear2")
LPC("clear2")
	for (i = 0; i < 256; i++)
	{
		STA(0x400);
	}
	RTS();


LPC("movetoscreen")


#if 1
	LDYi(0)
	LDAi(0x60) // rts
	STAizy(clearptr)
#endif


#if 0
	LDXi(0);

LPC("fillloop2")
	TXA();
	STAx(0x400);
	INX();
	BNE(L("fillloop2"))
#endif


//	MOV8i(0xd020,0);

	MOV8zi(runningflag, 0x02);

	MOV8i(0xd011, 0x1B);


	jump_if_not_time(0xe80, L("loopba_jmp"));
	
	MOV8i(0xd011, 0);
	MOV8i(0xd020, 0);

	setirq(0, 1);
	setirq(0, 2);
	LDXi(LB(0xf00))
	LDYi(HB(0xf00))
	JMP(if_executenext);

	LPC("loopba_jmp")
	JMP(L("loopba"))


//PRINTPOS("jgy")
PAGE

LPC("jyge")
	for (i = 0; i < 256; i++)
	{
		int xx = (i&7)*8+HB(L("ftab%i",0));
		B(xx);
	}

LPC("div8tab")
	for (i = 0; i < 256; i++)
	{
		B(i/8);
	}

auto megasin = [](float a) { return sin(a)*0.9f+sin(a*5.f+2.23f)*0.1f; };

LPC("sintab")
	for (i = 0; i < 256; i++)
	{
		float ang = (float)i*3.14159f/128.f;
		int xx = 128 + megasin(ang) * 120;
		B(xx);
	}

LPC("sintab_ch")
	for (i = 0; i < 256; i++)
	{
		float ang = (float)i*3.14159f/128.f;
		int xx = 160 + megasin(ang) * 100;
		B(xx / 8);
	}

LPC("sintab_and8")
	for (i = 0; i < 256; i++)
	{
		float ang = (float)i*3.14159f/128.f;
		int xx = 160 + megasin(ang) * 100;
		B(xx & 7);
	}



LPC("sintab2")
	for (i = 0; i < 256; i++)
	{
		float ang = (float)i*3.14159f/128.f;
		int xx = 100 + megasin(ang) * 80;
		B(xx);
	}


 	int bigsintab[256];
	for (i = 0; i < 256; i++)
	{
		float ang = (float)i*3.14159f/128.f;
		int xx = 0x310 + cos(ang) * 0x70;
		bigsintab[i] = xx;
	}

LPC("bigsintabl")
	for (i = 0; i < 256; i++)
		B(LB(bigsintab[i]))
LPC("bigsintabh")
	for (i = 0; i < 256; i++)
		B(HB(bigsintab[i]))


 	int mul40tab[256];
	for (i = 0; i < 256; i++)
	{
		int xx = i/8;
		xx *= 40;
		int ptr = xx;
		//ptr += (40-32)/2;
		//ptr += 0xe000
		//ptr += 0x400;
		mul40tab[i]=ptr;
	}

LPC("40multab_l")
	for (i = 0; i < 256; i++)
		B(LB(mul40tab[i]))
LPC("40multab_h")
	for (i = 0; i < 256; i++)
		B(HB(mul40tab[i]))


LPC("filtertest")

#if 0
	ADD16i(cunt, 0x71);
	ADD16i(cunt2, 0xe9);

	ADD16i(cuntxx, 0xd7);
	ADD16i(cuntxx2, 0xa5);
#endif
	ADD16i(cunt, 0xa1);
	ADD16i(cunt2, 0xb9);

	LDAz(framecounter)
	CLC();
	ADCi(8);
	ANDi(15);
	BEQ(L("korotamusalla"))

	ADD16i(cuntxx, 0x97);
	ADD16i(cuntxx2, 0x85);
	JMP(L("jeeskorotettu"))

	LPC("korotamusalla")
	ADD16i(cuntxx, 0x1497);
	ADD16i(cuntxx2, 0x2185);

LPC("jeeskorotettu")
#if 0
       LDAz(cuntxx+0)
       CLC();
       ADCi(0x13);
       STAz(cuntxx+0)
       LDA(0xd41c);
       ANDi(0x7);
       ADCz(cuntxx+1)
       STAz(cuntxx+1)

       LDAz(cuntxx2+0)
       CLC();
       ADCi(0x43);
       STAz(cuntxx2+0)
       LDA(0xd41b);
       ANDi(0x7);
       ADCz(cuntxx2+1)
       STAz(cuntxx2+1)
#endif


/*	LDAz(runningflag)
	BNE(L("runactualtest"))
	RTS();

	LPC("runactualtest")

	MOV8zi(runningflag, 0x00);
*/
	DEC(runningflag)

	RTS();

	int x,y;
	LPC("colmem")
	for (y = 0; y < 25; y++)
		for (x = 0; x < 40; x++)
		{
			vec2 p = vec2(x,y) - vec2(20.f, 12.5f);

			float d = glm::length(p) + glm::linearRand(-5.f, 5.f);

			int arvo;
			if (d > 21.f)
				arvo = 0x6;
			else if (d > 16.f)
				arvo = 0x4;
			else if (d > 12.f)
				arvo = 0xe;
			else if (d > 7.f)
				arvo = 0x3;
			else
				arvo = 0xd;

			arvo = frame0000[y*40+x+2+40*25];

			B(arvo);
		}

	PAGE
	LPC("filtertables")
	PRINTPOS("tässä filtertablet");
	emitfiltertables();


}

void ekat()
{
	int i,x,y,j,k;

	// make some charset

	srand(9091);

	auto addplot = [](int xx, char_t& tmpchar, int colo) { if (xx >= 0 && xx <= 63 && tmpchar.data[xx] < colo) tmpchar.data[xx] = colo; };

	int pls = 1;
	int plst = 2;

#if 1

	for (i = 0; i < 256; i++)
	{
		for (x = 0; x < 64; x++)
			charset[i].data[x] = 0;


// switch random charset vs. ordered 4x4
#if 1
		int plc = glm::max(1, i/20);
		//int plc = glm::max(1, (int)powf((float)i/110.f, 5.f));
		if (i == 0) plc = 0;
		for (j = 0; j < plc; j++)
		{
//			charset[i].data[rand() & 63] = 1;
#if 1
			int ppp = rand() & 63;
			for (y = -pls; y <= pls; y++)
				for (x = -pls; x <= pls; x++)
					addplot(ppp-y*8+x, charset[i], 10);
#endif

		}

#else

		if (i >= 1)
		for (j = 0; j < 64; j++)
		{
			int xb1 = (i >> 0) & 0x3;
			int yb1 = (i >> 2) & 0x3;
			int xb2 = (i >> 4) & 0x3;
			int yb2 = (i >> 6) & 0x3;
			int xc = (j >> 1) & 0x3;
			int yc = (j >> 4) & 0x3;

			charset[i].data[j] = ((xb1==xc&&yb1==yc) || (xb2==xc&&yb2==yc)) ? 1 : 0;

		}
#endif

	}
#endif

#if 0
	FILE *f = fopen("assets/chargen", "rb");
	if (!f)
	{
		printf("can't open CHARGEN\n");
		exit(1);
	}

	for (i = 0; i < 256; i++)
	{

		for (j = 0; j < 8; j++)
		{
			int merq = fgetc(f);
			for (k = 0; k < 8; k++)
			{
				charset[i].data[(j) * 8 + (k)] = (merq >> k) & 1;
			}
		}
	}
	fclose(f);
#endif


	//charset[i].data[i & 0x3F] = 1;

	// calculate The Tables

	for (j = 0; j < 64; j++)
	{
		for (i = 0; i < 256; i++)
		{
			char_t tmpchar = charset[i];



#if 1
#if 1
			for (y = -plst; y <= plst; y++)
				for (x = -plst; x <= plst; x++)
					addplot(j-y*8+x, tmpchar, 5);
#endif
			addplot(j, tmpchar, 10);
#else
			int xc = ((j >> 1) & 0x3)*2;
			int yc = ((j >> 4) & 0x3)*2;

			tmpchar.data[(yc+0)*8+(xc+0)] = 10;
			tmpchar.data[(yc+0)*8+(xc+1)] = 10;
			tmpchar.data[(yc+1)*8+(xc+0)] = 10;
			tmpchar.data[(yc+1)*8+(xc+1)] = 10;

#endif
			int simimax_ch = 0;
			int simimax = 0;
			for (k = 0; k < 256; k++)
			{
				int simi = 0;
				for (x = 0; x < 64; x++)
					simi += 10-abs(tmpchar.data[x] - charset[k].data[x]);
					//simi += tmpchar.data[x] == charset[k].data[x] ? 1 : 0;

				bool usethis = false;
				usethis |= simi > simimax;
				//usethis &= k != i;
				if (usethis)
				{
					simimax = simi;
					simimax_ch = k;
				}
			}

#if 0
			if (simimax_ch == 0)
				simimax_ch = 1;

			if (simimax_ch == 0)
				stopassembly("all table entries must be > 0");
#endif

			tables[j * 256 + i] = simimax_ch;
		}

	}

	int inusetab[256];

	for (i = 0; i < 256; i++)
		inusetab[i] = 0;

	inusetab[0] = 1;

	int added;
	while(1)
	{
		added = 0;
		for (i = 0; i < 256; i++)
		{
			for (j = 0; j < 64; j++)
			{
				int newch = tables[j * 256 + i];
				if (inusetab[newch] == 0)
				{
					added++;
				}
				inusetab[newch]++;
			}
		}
		if (added == 0)
			break;
	}

	for (i = 0; i < 256; i++)
	{
		if (inusetab[i] == 0)
			printf("unused char: %i\n", i);
//		else
//			printf("usage for %i: %i\n", i, inusetab[i]);
	}

#if 0
	for (i = 0; i < 256; i++)
		for (x = 0; x < 64; x++)
			charset[i].data[x] |= (x&1)^((x>>3)&1);
#endif

//	stopassembly("valmis");
}

int main()
{
	srand(time(NULL));
	ekat();
	assemble(genis);

	list<segment_c *> segs;
	segs.push_back(&actualprg);
	segs.push_back(&startvec);
	make_mprg("filterplotter.mprg", segs);
}