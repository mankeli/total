#include "hires.h"

#include "glm.h"

#include "debugf.h"
#include "hashembler.h"

#include <math.h>
#include "memconfig.h"
#include "lib.h"


#include <vector>
using std::vector;

hiresdata_t pic;


using namespace hashembler;

segment_asm_c initprg;
segment_asm_c startvec;
int plasmacounter1 = 0x11; // 2
int plasmacounter2 = 0x13; // 2
int plasmacounter3 = 0x15; // 2

int plasmaviewptr = 0x20; // 2

int scrollrow = 17;

void irqroutine()
{
	int i;
	LPC("irqrut");
	PRINTPOS("irqrut");

	CLI();

//	DEC(0xd020);
//	DEC(0xd020);

//	ADD16i(plasmacounter1, 0x57);
//	ADD16i(plasmacounter2, 0x99);
//	ADD16i(plasmacounter3, 0x123);
	ADD16i(plasmacounter1, 0x97);
	ADD16i(plasmacounter2, 0x89);
	ADD16i(plasmacounter3, 0x73);

	// tuplaviivan ylaosa
//	MOV8i(0xd020, 0x09);

	JSR(L("maxrutiini"))

//	INC(0x400);
//	MOV8i(0xd020, 0x09);

	//JSR(0x1003);

//	MOV8i(0xd020, 0x09);

	RTS();



}

ivec2 plw = ivec2(18,10);
int plsiz = plw.x * plw.y;
	int pl[40*25 * 5]; // 40*25 * ptc
	int ptc = 4;
	void calcplasma()
	{
		int t,x,y;

		printf("plasma size: %i\n", plsiz);

		for (t = 0; t < ptc; t++)
		{
			for (y = 0; y < 25; y++)
			{
				for (x = 0; x < 40; x++)
				{
					vec2 lp = vec2(x,y) * vec2(plw) / vec2(40.f, 25.f);
					lp += glm::gaussRand(vec2(-1.f), vec2(1.f)) * 0.2f;
					ivec2 lpi = clamp(ivec2(lp), ivec2(0), plw - ivec2(1));
					int pp = (int)lpi.y * plw.x + (int)lpi.x;
					pl[t * 40*25 + y * 40 + x] = pp;
				}
			}
		}
	}

	void makepalette(int* tabbe, int tabbelen)
	{
		int i;
	for (i = 0; i < 512; i++)
	{
		//int ii2 = (i + 128) & 0xFF;
		int ii2 = i;
		//float bupop = 0.5f + sin(ii2 * 3.14159f / 128.f) * 0.5f;
		float bupop = (ii2 / 256.f);
		bupop += glm::gaussRand(-1.f, 1.f) * 0.05f;
		if (bupop > 1.f)
			bupop -= 1.f;
		int idx = bupop * tabbelen;
		idx = clamp(idx, 0, tabbelen - 1);

		int col = tabbe[idx];

		B(col);

	}

	}

	void renderplasma()
	{
		int t,i,x,y;

			PRINTPOS("taessa lasketaan lo-res plasma");

			LDYz(plasmacounter1+1)
			//LDYi(0);

			for (i = 0; i < plsiz; i++)
			{
//		if (plu[i] > 0)

				
				value_t ta = L("taabelisin") + (rand() & 0);
			//int px = x; int py = y;
				int px = i % plw.x;
				int py = i / plw.x;

				vec2 pc = vec2(px,py);

			//ADCx(ta+((0x100000+px*11+py*11)&0xFF))

				LDXz(plasmacounter2+1)
			//		LDXi(90);
//			LDAy(ta+(((px+py)*4)&0xFF))
//			ADCy(ta+((py*5+px*3)&0xFF))
//			STAz(0x80+y*8+x)
				float sizfac = 0.64532f;
				LDAx(ta+(0x100000+((int)(dot(pc, vec2(1,5))*sizfac+10)&0xFF)))
				ADCx(ta+(0x100000+((int)(dot(pc, vec2(3,2))*sizfac+93)&0xFF)))
				ADCy(ta+(0x100000+((int)(dot(pc, vec2(-7,8))*sizfac+57)&0xFF)))
				ADCy(ta+(0x100000+((int)(dot(pc, vec2(5,2))*sizfac+15)&0xFF)))
				LDXz(plasmacounter3+1)
				ADCx(ta+(0x100000+((int)(dot(pc, vec2(19,-14))*sizfac+198)&0xFF)))
				ADCx(ta+(0x100000+((int)(dot(pc, vec2(-16,-12))*sizfac+122)&0xFF)))

#if 0
				LDAx(ta+((0x100000+px*1+py*5+10)&0xFF))
				ADCx(ta+((0x100000+px*3+py*2+93)&0xFF))
				ADCy(ta+((0x100000+px*-7+py*8+57)&0xFF))
				ADCy(ta+((0x100000+px*5+py*2+15)&0xFF))
				LDXz(plasmacounter3+1)
				ADCx(ta+((0x100000+px*19+py*-14+198)&0xFF))
				ADCx(ta+((0x100000+px*-16+py*-12+122)&0xFF))
				#endif
				//ROLa();
				TAX();
			//LDXi(y);
//			ANDi(0x0f);
//			STA(0xd016);

//				LDAi(HB(L("exitlogicpixel%i", i)-1))
//				PHA();
//				LDAi(LB(L("exitlogicpixel%i", i)-1))
//				PHA();

			//	LDAx(L("taabeli"))
			//	TAX();
			#if 0
			for (y = 0; y < 25; y++)
			{
				for (x = 0; x < 40; x++)
				{
					if (i == pl[y * 40 + x])
						STA(0x400 + y * 40 + x)
				}
			}
			#endif
			//RTS();
			//JMP(L("plasmo%i", i))
			//JMPi(L("plasmoptr%i", i))

#if 0
			JMP(L("plasmo%i,%i", 0, i))
			LPC("exitlogicpixel%i", i);
#else
LPC("plasmo%i,%i", t,i);
				for (y = 0; y < 25; y++)
				{
					for (x = 0; x < 40; x++)
					{
						if (i == pl[t*40*25 + y*40 + x])
						{
							int cblock=y*40+x;
							int offi = pic.m_col1[cblock];

							if (offi > 0)
							{
								if (offi <= 3)
									LDAx(L("paletteS")+((offi*6)&0xFF))
								else if (offi == 4)
									LDAx(L("palette8"))
								else if (offi == 5)
									LDAx(L("palette7"))
								else if (offi == 6)
									LDAx(L("palette6"))
								else if (offi == 7)
									LDAx(L("palette5"))
								else if (offi == 8)
									LDAx(L("palette4"))
								else if (offi == 9)
									LDAx(L("palette3"))
								else if (offi == 10)
									LDAx(L("palette2"))
								else if (offi == 11)
									LDAx(L("palette1"))

								STA(0x400 + y * 40 + x)
							}
						}
					}
				}
#endif
			//LDAi(i);
			//STA(0xd020);

			//if (i == 170)
				//break;

#if 0
			TXA();
			ANDi(0x2);
			STA(0xd016);
#endif

			//INC(0xd020);
		}

		//MOV8i(0xd020, 0x00);

		RTS();

		if (PC() & 1)
			B(1);


			PRINTPOS("taessa on nykyiset plasmopointteri");

		PRINTPOS("tassa on plot rutiinit");

#if 0
		for (t = 0; t < 1; t++)
		{
			for (i = 0; i < plsiz; i++)
			{
				LPC("plasmo%i,%i", t,i);
				for (y = 0; y < 25; y++)
				{
					for (x = 0; x < 40; x++)
					{
						if (i == pl[t*40*25 + y*40 + x])
						{
							int cblock=y*40+x;
							int offi = pic.m_col1[cblock];

							if (offi > 0)
							{
								if (offi <= 3)
									LDAx(L("palette1")+((offi*6)&0xFF))
								else
									LDAx(L("palette2")+(((offi-4)*6)&0xFF))
								STA(0x400 + y * 40 + x)
							}
						}
					}
				}
				//RTS();
				JMP(L("exitlogicpixel%i", i));
			}

		}
#endif
	}


	void genis()
	{
		CTX("main")
		RESERVE(0x00);
		RESERVE(0x01);

	startvec.begin(if_startvec);

	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))


		initprg.begin(0x2000);

	PRINTPOS("HIRES_DATA")

	int x,y,i;
SEG->setpc(0x2000);

	for (y = 0; y < 25; y++)
	{
		for (x = 0; x < 40; x++)
		{
			for (i = 0; i < 8; i++)
			{
				int arvo;
				arvo = pic.m_chars[(y * 40 + x) * 8 + i];
				arvo &= 0xFF;
				B(arvo);
			}
		}
	}



		irqroutine();

LPC("actualbeg")

	JSR(if_startmusic)


		MOV8i(0xD011, 0x3B);

	MOV8i(0xD018, 0x18); // 00011000 0x18
	MOV8i(0xdd02, 0x3c);

	MOV8i(0xD020, 0x00);

	MOV8i(0xd020, 0x00);
	MOV8i(0xd021, 0x02);
	MOV8i(0xd022, 0x08);
	MOV8i(0xd023, 0x0a);
	MOV8i(0xd024, 0x07);
	MOV8i(0xd016, 0x08);


#if 0
	LDAi(0x20)
	LDXi(40)
	LPC("loopcol2")
	STAx(0x400+(scrollrow+0)*40)
	DEX();
	BNE(L("loopcol2"))
#endif
	LDAi(0x0)
	LDXi(0)
	LPC("loopcol2")
	STAx(0x400)
	STAx(0x500)
	STAx(0x600)
	STAx(0x700)
	INX();
	BNE(L("loopcol2"))

	LDXi(LB(L("irqrut")))
	LDYi(HB(L("irqrut")))
	JSR(if_setirq1)


	LPC("loopba")
	//INC(0xd021);
	//JMP(L("loopba"))
	jump_if_not_time(0x2700, L("loopba"));

	JSR(if_unsetirq1);

	MOV8i(0xd011, 0);

	JMP(if_executenext_nowait);




		LPC("maxrutiini")
#if 0
	//INC(0xd020);

			LDYz(framecounter)
	for (y = 0; y < 17; y++)
	{
		for (x = 0; x < 20; x++)
		{
			//INC(0x400 + (y + 19)*40 + x)
			//LDX(L("imgdata") + (y + 19)*40 + x)
			//INX();
			//STX(L("imgdata") + (y + 19)*40 + x)

			int px,py;
			px = (x * 20) / 20;
			py = (y * 12) / 12;

			int pxd, pyd;
			pxd = px; pyd = py;

//			px += (rand() & 3) - 2;
//			py += (rand() & 3) - 2;

			if (pxd > 39)
				pxd = 39;
			if (pyd > 24)
				pyd = 24;

			value_t ta = L("taabelisin");


			LDAy(ta+(((px+py)*7)&0xFF))
			#if 1
			ADCy(ta+((py*5+px*3)&0xFF))
			ADCy(ta+((px)&0xFF))
			ADCy(ta+((py*3+px*5)&0xFF))
			LDXz(framecounter2)
			ADCx(ta+((py)&0xFF))
			ADCx(ta+((py*1+px*9)&0xFF))
			ADCx(ta+((py*4+px*1)&0xFF))
			#endif

			TAX();
			LDAx(L("taabeli"))
			STA(0x400 + (pyd + 0)*40 + (pxd+0))
			//STA(0x400 + (pyd + 0)*40 + (pxd+1))
			//STA(0x400 + (pyd + 1)*40 + (pxd+0))
			//STA(0x400 + (pyd + 1)*40 + (pxd+1))

		}

	}
	#endif

	renderplasma();

	PAGE
	LPC("taabelisin")	

	for (i = 0; i < 512+128; i++)
	{
		int arvo = (int)(0x100 + sin(i * 1.f * 3.14159f / 128.f) * 60.f) & 0xFF;
		B(arvo);
	}

	int pal1[]={0x2};
	int pal2[]={0x2};
	int pal3[]={0x2,0x8};
	int pal4[]={0x2,0x8};
	int pal5[]={0x2,0x8,0xa,0x8};
	int pal6[]={0x2,0x8,0xa,0xf,0xa,0x8};
	int pal7[]={0x2,0x8,0xa,0xf,0x7,0xf,0xa,0x8};
	int pal8[]={0x2,0x8,0xa,0xf,0x7,0x1,0x7,0xf,0xa,0x8};


	int palS[]={0x9,0xb,0x4,0xb};


	PAGE
	LPC("palette1")	
	makepalette(pal1, sizeof(pal1)/sizeof(pal1[0]));
	LPC("palette2")	
	makepalette(pal2, sizeof(pal2)/sizeof(pal2[0]));
	LPC("palette3")	
	makepalette(pal3, sizeof(pal3)/sizeof(pal3[0]));
	LPC("palette4")	
	makepalette(pal4, sizeof(pal4)/sizeof(pal4[0]));
	LPC("palette5")
	makepalette(pal5, sizeof(pal5)/sizeof(pal5[0]));
	LPC("palette6")	
	makepalette(pal6, sizeof(pal6)/sizeof(pal6[0]));
	LPC("palette7")	
	makepalette(pal7, sizeof(pal7)/sizeof(pal7[0]));
	LPC("palette8")	
	makepalette(pal8, sizeof(pal8)/sizeof(pal8[0]));

	LPC("paletteS")
	makepalette(palS, sizeof(palS)/sizeof(palS[0]));

}

void ekat()
{
	calcplasma();

		pic.load("assets/burner.png");
	//pic.tohires();
	pic.findcolors();

	int i;
	int swapped = 0;
	for (i = 0; i < 40*25; i++)
	{
		int p = pic.m_col2[i];
		if (p > 0)
		{
			pic.m_col2[i] = pic.m_col1[i];
			pic.m_col1[i] = p;
			swapped++;
		}
	}
	printf("swapped %i colors\n", swapped);

	pic.findchars();


}

int main(int argc, char **argv)
{
	ekat();

	assemble(genis);

//	segment_c fontprg;
//	fontprg.load_prg("veritionfont.prg", true);

	list<segment_c *> segs;
	segs.push_back(&initprg);
	segs.push_back(&startvec);

	make_mprg("burner.mprg", segs);

	return 0;
}
