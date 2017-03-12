#include "glm.h"

#include "debugf.h"
#include "hashembler.h"

#include <math.h>
#include "memconfig.h"
#include "lib.h"

#include "../assets/timantti_demoon.c"
#include "../assets/timanttikentta3.c"


#include <vector>
using std::vector;


using namespace hashembler;

segment_asm_c initprg;
segment_asm_c startvec;


const int eka_tausta_char = 6*39;


value_t siirtocounter = 0x10;
value_t siirtocounter_spd = 0x11;

int mapping[256];
int uniikkimerkki[256];

void analysoikuva()
{
	int uniikit[256];
	int i;

	for (i=0;i<256;i++)
		uniikit[i]=0;

	for (i = 0; i < 40*25; i++)
	{
		uniikit[frame0000[2+i]]++;
	}

	int uniikkimaara = 0;

	for (i=0;i<256;i++)
	{
		if (uniikit[i] > 0)
		{
			uniikkimerkki[uniikkimaara]=i;
			printf("%i:%i\n", i, uniikit[i]);

			uniikkimaara++;
		}
	}

	printf("uniikki maara: %i\n", uniikkimaara);

	for (i = 0; i < uniikkimaara; i++)
	{
		mapping[uniikkimerkki[i]] = i;
	}
}

void irqroutine()
{
	int i;
	LPC("irqrut");
	PRINTPOS("irqrut");

	//INC(0xd020);
/*
	39 charria = 234 merkkiä, eli 6 settiä

*/

	PRINTPOS("tässä itse looppi")

	LDXz(siirtocounter)
	INX();
	STXz(siirtocounter)
	CPXz(siirtocounter_spd)
	BNE(L("eiliikutetanyt"))

	LDXi(0);
	STXz(siirtocounter)

	LDXz(siirtocounter_spd)
	DEX();
	CPXi(2);
	BEQ(L("eienaamuutetanopeutta"))
	STXz(siirtocounter_spd)

	LPC("eienaamuutetanopeutta")



//	LDAz(framecounter)
//	ANDi(0x03);
//	BNE(L("eiliikutetanyt"))
	//JSR(0x1003); 
//	INC(0xd020);

#if 1
	int ossa = 0x2000 + eka_tausta_char * 8;
	//int ossa = 0x2000 + eka_tausta_char * 8;
  
	LDA(ossa);
	PHA();
	LDA(ossa+16);
	PHA();

	LDXi(0);
LPC("liikutacharrei")
	LDAx(ossa+1)
	STAx(ossa)
	INX();
	CPXi(32)
	BNE(L("liikutacharrei"))

	PLA();
LPC("clearpatch1")
//	LDAi(0);
	NOP(); NOP();
	STA(ossa+16+15);
	PLA();
LPC("clearpatch2")
	//LDAi(0);
	NOP(); NOP();
	STA(ossa+15);
#endif

//	DEC(0xd020);

LPC("eiliikutetanyt")
//	DEC(0xd020);





	LDAz(framecounter);
//	LSRa();
	ASLa();
	ANDi(0x7F);
	TAX();
	int x,y;
	for (y = 0; y < 25; y++)
	{
		for (x = 0; x < 40; x++)
		{

			int pask = y * 40 + x;
			int merk = frame0000[pask + 2];
			int col = frame0000[pask + 2 + 40*25];

			float xx = (x - 20) / 20.f;
			float yy = (y - 11) / 20.f;
			float offsetto = 0.f;

			float disti = sqrt(xx*xx+yy*yy);
			//offsetto += disti * -4.0f;

			offsetto += sqrt(xx*xx+yy*yy) * -3.4f;
			offsetto += 4.0f * (atan2f(yy,xx) / 3.14159f + 1.f) * 0.5f;

			int ofs = (int)(offsetto * 0x80) & 0x7F;
			//if (merk != 32 && col != 14)
			if (disti < 0.5f || (merk != 32 && col != 14))
			{
				//LDAi(mapping[merk])
				LDAx(L("mappingses%i", mapping[merk])+ofs)
				//printf("mappings at %04x\n", L("mappingses%i", mapping[merk]));
				STA(0x400+pask)
				LDAi(col)
				STA(0xd800+pask)
			}
		}
	}




	RTS();


}



uint32_t hash( uint32_t a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}

// mode 0 shrink  1 expand
void pruc(int *graffa, int mode)
{
	int wg[24*24];
	memcpy(wg, graffa, sizeof(int)*24*24);
	int j,k;
			for (j = 1; j < 23; j++)
			{
				for (k = 1; k < 23; k++)
				{
					float naapurit = 0;
					naapurit += wg[(j-1)*24+(k-1)];
					naapurit += wg[(j+0)*24+(k-1)];
					naapurit += wg[(j+1)*24+(k-1)];
					naapurit += wg[(j-1)*24+(k+0)];
					naapurit += wg[(j+0)*24+(k+0)];
					naapurit += wg[(j+1)*24+(k+0)];
					naapurit += wg[(j-1)*24+(k+1)];
					naapurit += wg[(j+0)*24+(k+1)];
					naapurit += wg[(j+1)*24+(k+1)];

					naapurit += (rand() & 65535) / 65535.f - 0.5f;

					if (mode == 0)
					{
						graffa[(j)*24+(k)] = naapurit > 6.f ? 1 : 0;
					}

					if (naapurit > 1 && mode == 1)
						graffa[(j)*24+(k)] = 1;
				}
			}
}


	void genis()
	{
		CTX("main")

	startvec.begin(if_startvec);
	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

		initprg.begin(0x2000);

	PRINTPOS("no tässä on nyt charreja lol");

	FILE *f = fopen("assets/chargen", "rb");
	if (!f)
	{
		printf("can't open CHARGEN\n");
		exit(1);
	}
	//printf("tää on nyt tässä: %p\n", f);

	int i,j,k;

	auto readmerkki = [](int *graffa, FILE *f, int inputmerkki)
	{
		fseek(f, inputmerkki * 8, SEEK_SET);

		int j, k;
		for (j = 0; j < 24*24; j++)
		{
			graffa[j] = 0;
		}

		for (j = 0; j < 8; j++)
		{
			int merq = fgetc(f);
			for (k = 0; k < 8; k++)
			{
				graffa[(j+8) * 24 + (k+8)] = (merq >> k) & 1;
			}
		}
	};

	auto emitmerkki = [](int *graffa)
	{
		int j, k;
		for (j = 0; j < 8; j++)
		{
//			int merq = fgetc(f);
//			if (setti >= 1)
//				merq &= hash(setti+j) &  0xFF;
			int merq = 0;
			for (k = 0; k < 8; k++)
			{
				merq |= graffa[(j+8) * 24 + (k+8)] << k;
			}

			B(merq);
		}
	};


	for (i = 0; i < 39*6; i++)
	{
		int paska = i % 39;
		int setti = i / 39;
		int inputmerkki = uniikkimerkki[paska];

		int graffa[24*24];

		readmerkki(graffa, f, inputmerkki);

#if 0
		for (j = 0; j < 24; j++)
		{
			for (k = 0; k < 24; k++)
			{
				int new_j = j, new_k = k;

				if (new_j < 8) new_j = 8;
				else if (new_j > 15) new_j = 15;

				if (new_k < 8) new_k = 8;
				else if (new_k > 15) new_k = 15;

				graffa[(j) * 24 + (k)] = graffa[(new_j) * 24 + (new_k)];
			}
		}
#endif


		int shrinks = 0;
		int expands = 0;

		int midpoint = 4;

		shrinks = setti - midpoint;
		shrinks = shrinks > 0 ? 0 : -shrinks * 1;

		expands = setti - midpoint;
		expands = expands < 0 ? 0 : expands;

		while(shrinks-- > 0)
			pruc(graffa, 0);
		while(expands-- > 0)
			pruc(graffa, 1);


		emitmerkki(graffa);
	}

	int graffa_d1[24*24];
	int graffa_d2[24*24];
	int graffa_d3[24*24];
	int graffa_d4[24*24];
	readmerkki(graffa_d1, f, 233);
	readmerkki(graffa_d2, f, 95);
	readmerkki(graffa_d3, f, 223);
	readmerkki(graffa_d4, f, 105);

	emitmerkki(graffa_d1);
	emitmerkki(graffa_d2);
	emitmerkki(graffa_d3);
	emitmerkki(graffa_d4);


	fclose(f);

	// nyt pitas olla ekas merkis

	// jyy 233 223 95 105

#if 0
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
	B(/**/0x80+0x40+0x20+0x10+0x08+0x04+0x02+0x01);
#endif

	PRINTPOS("tablet alkaa");

	for (i = 0; i < 39; i++)
	{
		LPC("mappingses%i", i);
		for (j = 0; j < 256; j++)
		{
			float kulli = (float)j / 128.f;
			float settif = 2.f + sin(kulli * 3.14159f * 2.f) * 2.f;
			settif += 1.f * ((rand() & 65535) / 65535.f - 0.5f);
			int setti = settif;
			setti += ((rand() & 65535) / 65535.f) < glm::fract(settif) ? 1 : 0;

			if (setti < 0) setti = 0;
			else if (setti >= 5) setti = 5;
			B(i + setti * 39);
			//B(i);
		}
	}

	PRINTPOS("tablet loppuu");

	printf("file offset: %04X\n", SEG->m_datapos);

		irqroutine();

LPC("actualbeg")

		PRINTPOS("pc at begin");



		MOV8i(siirtocounter, 0);
		MOV8i(siirtocounter_spd, 10);

		MOV8i(0xD011, 0x1B);
		MOV8i(0xd020, frame0000[0]);
		MOV8i(0xd021, frame0000[1]);

#if 0
	LDAi(mapping[32]);
	//LDAi(6*39);
	//LDAi(1);
	LDXi(0);
	LPC("clrcol")
	STAx(0x400)
	STAx(0x500)
	STAx(0x600)
	STAx(0x700)

	INX();
	BNE(L("clrcol"))
#endif

#if 0
	LDXi(0);
	LPC("clrcol")

	TXA();
	ANDi(1);
	TAY();

	LDAy(L("timi"))
	STAx(0x400)
	STAx(0x500)
	STAx(0x600)
	STAx(0x700)

	INX();
	BNE(L("clrcol"))
#endif

const int eka_tausta_char = 6*39;

#if 0
	LDYi(24);
LPC("xrivialku")
	LDXi(39);

	TYA();
	ANDi(1);
	BNE(L("patsaatoiset"))
	LPC("patsaaekat")
	LDAi(eka_tausta_char+2);
	STA(L("dibg_mod1")+1)
	LDAi(eka_tausta_char+0);
	STA(L("dibg_mod2")+1)
	
	JMP(L("yksimerkkirivi"))
	LPC("patsaatoiset")
	LDAi(eka_tausta_char+3);
	STA(L("dibg_mod1")+1)
	LDAi(eka_tausta_char+1);
	STA(L("dibg_mod2")+1)

LPC("yksimerkkirivi")
	LPC("dibg_mod1")
	LDAi(4);
	JSR(L("mergele"))
	DEX();
	LPC("dibg_mod2")
	LDAi(5);
	JSR(L("mergele"))
	DEX();
	BPL(L("yksimerkkirivi"))

	CLC();
	LDA(L("mergele")+1)
	ADCi(40);
	STA(L("mergele")+1)
	LDA(L("mergele")+2)
	ADCi(0);
	STA(L("mergele")+2)

	DEY();
	BPL(L("xrivialku"))

	JMP(L("ylitimin"))

	LPC("mergele")
		STAx(0x400);
		RTS();
#else
	LDAi(14);
	LDXi(0);
	LPC("maketimants")
	LDAx(L("timanttikentta")+0x000)
	STAx(0x400)
	LDAx(L("timanttikentta")+0x100)
	STAx(0x500)
	LDAx(L("timanttikentta")+0x200)
	STAx(0x600)
	LDAx(L("timanttikentta")+0x300)
	STAx(0x700)

	INX();
	BNE(L("maketimants"))
#endif

	LPC("ylitimin")


		MOV8i(0xD018, 0x18);
		MOV8i(0xDD02, 0x3c);

#if 0
	LDAi(4);
	STA(0x400+0+0);
	LDAi(5);
	STA(0x400+0+1);
	LDAi(6);
	STA(0x400+40+0);
	LDAi(7);
	STA(0x400+40+1);
#endif

	LDAi(14);
	LDXi(0);
	LPC("clrcol2")
	STAx(0xd800)
	STAx(0xd900)
	STAx(0xda00)
	STAx(0xdb00)

	INX();
	BNE(L("clrcol2"))


	LDXi(LB(L("irqrut")))
	LDYi(HB(L("irqrut")))
	JSR(if_setirq1)


	LPC("loopba1")
	jump_if_not_time(0x380, L("loopba1"));
	MOV8i(L("clearpatch1")+0, 0xA9);
	MOV8i(L("clearpatch1")+1, 0);
	MOV8i(L("clearpatch2")+0, 0xA9);
	MOV8i(L("clearpatch2")+1, 0);

	LPC("loopba")
	//JMP(L("loopba"))
	jump_if_not_time(0x440, L("loopba"));

	JSR(if_unsetirq1);


	MOV8i(0xd011, 0);
	MOV8i(0xd020, 0xF);


	JMP(if_executenext_nowait);

	LPC("timanttikentta")

/*
	readmerkki(graffa_d1, f, 233);
	readmerkki(graffa_d2, f, 95);
	readmerkki(graffa_d3, f, 223);
	readmerkki(graffa_d4, f, 105);
233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,
95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,95,105,233,223,

*/

	int x,y;
	for (y = 0; y < 25; y++)
	{
		for (x = 0; x < 40; x++)
		{
			/*
			0 2 1 3 0 2 1 3
			1 3 0 2 1 3 0 2 
			0 2 1 3 0 2 1 3
			1 3 0 2 1 3 0 2 
			*/
			int tc = eka_tausta_char;

			tc += ((y + ((x&2)>>1)) & 1) + ((x) & 1) * 2;
			B(tc);

			if (x < 10)
			printf("%02X ", tc);
		}
		printf("\n");
		
	}

	PRINTPOS("tahan loppuu koodi");



}

int main(int argc, char **argv)
{
	analysoikuva();
	assemble(genis);

	list<segment_c *> segs;
	segs.push_back(&initprg);
	segs.push_back(&startvec);

	make_mprg("rad.mprg", segs);

	return 0;
}
