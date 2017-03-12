#include "hires.h"

#include "glm.h"
#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

hiresdata_t pic;
hiresdata_t pic2;

hiresdata_t sprites;

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

segment_asm_c hiresvarit;
segment_asm_c hireskuva;

int cursprset = 0x10;
int ilmestyspos = 0x11;
int havicunter = 0x12;

void effy()
{
	
}

void genis()
{
	CTX("valo")
	int x,y,i;

	hiresvarit.begin(0x400);

	for (i = 0; i < 40*25; i++)
	{
		B(0x1F);
	}


		auto emitsprite = [](hiresdata_t &s, ivec2 bp)
		{
			int x,y;
			for (y = 0; y < 21; y++)
			{
				for (x = 0; x < 3; x++)
				{
					int tbx = x + bp.x;
					int ty = y + bp.y;

					int tby = ty/8;
					int tyi = ty&7;

					int arvo = s.m_chars[(tby * s.m_bsize.x + tbx) * 8 + tyi];

					B(arvo);
				}
			}

			B(0);
		};



	startvec.begin(if_startvec);

	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

	hireskuva.begin(0x2000);

	for (y = 0; y < 25; y++)
	{
		for (x = 0; x < 40; x++)
		{
			for (i = 0; i < 8; i++)
			{
				int arvo;
				arvo = pic2.m_chars[(y * 40 + x) * 8 + i];
				arvo &= 0xFF;
				B(arvo);
			}
		}
	}

	actualprg.begin(0x4000);

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

	SEG->setpc(0x6000);

	for (i = 0; i < 40*25;i++)
	{
		//B(0x20);
		int cval = ((pic.m_col2[i] & 0x0F) << 4) | (pic.m_col1[i] & 0x0f);
		//int cval = rajahdys.m_col1[i] & 0x0f;
		B(cval);
	}

	SEG->setpc(0x6400);

	for (i = 0; i < 14; i++)
	{
		int yc = i * 21;
		LPC("sprites%i", i)
		emitsprite(sprites, ivec2(0,yc));
		emitsprite(sprites, ivec2(3,yc));
		emitsprite(sprites, ivec2(6,yc));
		emitsprite(sprites, ivec2(9,yc));
		emitsprite(sprites, ivec2(12,yc));
		emitsprite(sprites, ivec2(15,yc));
	}
	LPC("spritesend");

	PRINTPOS("sprites end")



	PRINTPOS("tassa koodi")
LPC("actualbeg")

	MOV8i(0xd011, 0x0B);


	MOV8i(0xd020, 0xf);
	MOV8i(0xd021, 0xf);

	MOV8zi(cursprset, 0);


	int firstsprnum = (L("sprites0") & 0x3FFF) / 64;
	LDXi(firstsprnum)
	STX(0x63F8);
	INX();
	STX(0x63F9);
	INX();
	STX(0x63FA);
	INX();
	STX(0x63FB);
	INX();
	STX(0x63FC);
	INX();
	STX(0x63FD);
	INX();
	STX(0x63FE);
	INX();
	STX(0x63FF);

	for (i = 0; i < 7; i++)
	{
		MOV8i(0xd000+i*2, 195+i*24);
		MOV8i(0xd001+i*2, 128);
		MOV8i(0xd027+i, 0x1);
	}

	MOV8i(0xd015, 0x00);
	MOV8i(0xd017, 0x00);
	MOV8i(0xd01D, 0x00);
	MOV8i(0xd010, 0x38);

	MOV8i(0xd01C, 0x00);
	MOV8i(0xd01B, 0x00);



	LDXi(0);
	LDAi(0x0F)
LPC("fillcolmem")
	STAx(0xD800)
	STAx(0xD900)
	STAx(0xDA00)
	STAx(0xDB00)
	INX();
	BNE(L("fillcolmem"))

	MOV8zi(ilmestyspos, 255);

	setirq(L("irqrut"), 1);
	//JSR(L("vaihda_pimea_logo"))





LPC("ikiloop")
	effy();

	JSR(L("liiskarun"))

//	JMP(L("ikiloop"))
	jump_if_not_time(0x900, L("ikiloop"));

	JSR(if_unsetirq1);

	MOV8i(0xd011, 0);

	//JMP(if_executenext_nowait);

	LDXi(LB(0xa00))
	LDYi(HB(0xa00))
	JMP(if_executenext);



	LPC("liiskarun")
	RTS();

	// sileen että ylemmil ja alemmil riveil olis suurempi todennäköisyys
	LDYi(0);
	LPC("liiskatestloop")
	for (y = 0; y < 25; y++)
	{
		LDA(0xd41b)
		ADCi(abs(y-12)*3+40);

		BMI(L("skippi%i",y))
		JSR(L("liiska-vasen-%i", y))
		LPC("skippi%i",y)
		ANDi(0x07);
		STA(0xd016);
	}
	INY();
	CPYi(8);
	BEQ(L("liiskatestloopend"))
	JMP(L("liiskatestloop"))

	LPC("liiskatestloopend")

	RTS();

	for (y = 0; y < 25; y++)
	{
		LPC("liiska-vasen-%i", y)
		for (x = 19; x >= 1; x--)
		{
			LDAy(0x2000 + y * 8 * 40 + x * 8 - 8)
			//RORa();
//			RORa();
			STAy(0x2000 + y * 8 * 40 + x * 8)
		}
		for (x = 20; x < 39; x++)
		{
			LDAy(0x2000 + y * 8 * 40 + x * 8 + 8)
			//RORa();
//			ROLa();
			STAy(0x2000 + y * 8 * 40 + x * 8)
		}

			LDAi(0);
			STAy(0x2000 + y * 8 * 40 + 0 * 8)
			STAy(0x2000 + y * 8 * 40 + 39 * 8)

		RTS();
	}

	RTS();


	int movetabi1[256];
	int movetabi2[256];
	for (i = 0; i < 256; i++)
	{
		float a1 = 0x97 + sin(i * 0.1073f) * 60;
		float a2 = 0x97 + sin(i * 0.0663f) * 60;
		a1 += sin(i * 0.1973f) * 30;
		a2 += sin(i * 0.1463f) * 30;

		float t1 = glm::min(1.f, (float)i / 200.f);
		t1 = powf(t1, 2.0f);
		float t2 = glm::max(0.f, (float)(i-200)/55.f );

		a1 = glm::mix((float)0x30, a1, t1);
		a2 = glm::mix((float)0xfd, a2, t1);

		a1 = glm::mix(a1, (float)0xfd, t2);
		a2 = glm::mix(a2, (float)0xfd, t2);

		if (a1 > a2)
		{
			float t = a1;
			a1 = a2;
			a2 = t;
		}


		movetabi1[255-i] = glm::clamp((int)a1, 0x32, 0xfe);
		movetabi2[255-i] = glm::clamp((int)a2, 0x32, 0xfe);
	}

LPC("ilmestys-sinus1")
	for (i = 0; i < 256; i++)
		B(movetabi1[i])
LPC("ilmestys-sinus2")
	for (i = 0; i < 256; i++)
		B(movetabi2[i])


LPC("irqrut")

	CLI();

	MOV8i(0xd011, 0x3B);

	MOV8i(0xd018, 0x80);
	MOV8i(0xdd02, 0x3d);

	LDXz(ilmestyspos)
	BEQ(L("ei-ilmestys-kikkaa"))
	DEX();
	STXz(ilmestyspos)

	LDXz(ilmestyspos)
	LDAx(L("ilmestys-sinus1"))
	LPC("waitforraster")
	CMP(0xd012)
	BCS(L("waitforraster"))

	MOV8i(0xd011, 0x1B);

	LDAi(0);
	STA(0xd000)
	STA(0xd002)
	STA(0xd004)
	STA(0xd006)

	STA(0xd008)
	STA(0xd00a)
	STA(0xd00c)
	STA(0xd010)


	LDXz(ilmestyspos)
	LDAx(L("ilmestys-sinus2"))
	LPC("waitforraster")
	CMP(0xd012)
	BCS(L("waitforraster"))

	MOV8i(0xd010, 0x38);
	MOV8i(0xd011, 0x3B);
//	MOV8i(0xd015, 0x3F);

	JSR(	L("ei-ilmestys-kikkaa"))

	RTS();

	LPC("ei-ilmestys-kikkaa")

	for (i = 0; i < 7; i++)
	{
		MOV8i(0xd000+i*2, 195+i*24);
	}
	MOV8i(0xd015, 0x3F);

	LDAz(framecounter)
	ANDi(0x0F);
	TAX();
	LDAx(L("spritemovetab"))
	for (i = 0; i < 7; i++)
	{
		STA(0xd001+i*2)
	}


	LDAz(framecounter)
	ANDi(0x07);
	BEQ(L("changesprites"))


//	MOV8i(0xdd02, 0x3c);
//	MOV8i(0xd018, 0x18);

	RTS();

	LPC("spritemovetab")
	for(i = 0; i < 16; i++)
	{
		float yp = 124;

		yp += powf((i/15.f-0.5f)*2.f, 2.0f) * 5.f;

		B((int)yp);

	}

	LPC("spritesets")

	int sprstate = 0;
	for (i = 0; i < 80; i++) 
	{
		static int lastarv = 0;
		int arv = 0;
		
		if (sprstate == 0)
		{
			do
			{
				arv = rand() % 13;
			} while(arv == lastarv);

			if ((rand() % 100) < 10)
			{
				sprstate = 1;
			}
		}
		else if (sprstate == 1)
		{
			arv = 13;

			if ((rand() % 100) < 40)
			{
				sprstate = 0;
			}
		}

		lastarv = arv;
		B(firstsprnum+arv*6)
	}

/*	B(firstsprnum+0*6)
	B(firstsprnum+1*6)
	B(firstsprnum+2*6)
	B(firstsprnum+9*6)
	B(firstsprnum+10*6)
	B(firstsprnum+0*6)
	B(firstsprnum+1*6)
	B(firstsprnum+2*6)
	B(firstsprnum+6*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+7*6)
	B(firstsprnum+6*6)
	B(firstsprnum+11*6)
	B(firstsprnum+3*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+4*6)
	B(firstsprnum+5*6)
	B(firstsprnum+9*6)
	B(firstsprnum+8*6)
	B(firstsprnum+12*6)
	B(firstsprnum+7*6)
	B(firstsprnum+8*6)
	B(firstsprnum+3*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+4*6)
	B(firstsprnum+5*6)
	B(firstsprnum+10*6)
	B(firstsprnum+11*6)
	B(firstsprnum+12*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+4*6)
	B(firstsprnum+8*6)
	B(firstsprnum+12*6)
	B(firstsprnum+7*6)
	B(firstsprnum+2*6)
	B(firstsprnum+1*6)
	B(firstsprnum+0*6)
	B(firstsprnum+3*6)
	B(firstsprnum+12*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+4*6)
	B(firstsprnum+8*6)
	B(firstsprnum+12*6)
	B(firstsprnum+7*6)
	B(firstsprnum+2*6)
	B(firstsprnum+1*6)
*/
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)
	B(firstsprnum+13*6)

	B(0);

	int lastsprnum = (L("spritesend") & 0x3FFF) / 64;
	LPC("changesprites")
	LDXz(cursprset)
	INX();
	STXz(cursprset)
	LDAx(L("spritesets"))
	BEQ(L("vaihda_pimea_logo"))
	TAX();
	for (i = 0; i < 6; i++)
	{
		STX(0x63f8+i);
		INX();
	}

	RTS();

	LPC("regtab");
	B(0xD8); B(0x1B); B(0x01); B(0x01)
	B(0xE8); B(0x1B); B(0x0F); B(0x01)
	B(0x98); B(0x1B); B(0x01); B(0x01)
	B(0xD8); B(0x1B); B(0x01); B(0x01)
	B(0xC8); B(0x1B); B(0x0F); B(0x0F)
	B(0xB8); B(0x1B); B(0x01); B(0x01)
	B(0xA8); B(0x1B); B(0x0F); B(0x01)
	B(0x98); B(0x1B); B(0x0F); B(0x01)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0C)
	B(0x18); B(0x3B); B(0x0F); B(0x0B)
	B(0x98); B(0x1B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x98); B(0x1B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)
	B(0x18); B(0x3B); B(0x0F); B(0x0F)

	LPC("vaihda_pimea_logo")
	setirq(L("pimea_logo_irq"), 1);
		MOV8i(0xdd02, 0x3c);
		MOV8i(0xd018, 0x18);
		MOV8i(0xd011, 0x3B);

		MOV8i(0xdd02, 0x3c);
		MOV8i(0xd018, 0xC8);
		MOV8i(0xd011, 0x1B);

		MOV8i(0xd015, 0);

		MOV8zi(havicunter, 0);

	LDXi(0);
	LDAi(0x01)
LPC("fillcolmem")
	STAx(0xD800)
	STAx(0xD900)
	STAx(0xDA00)
	STAx(0xDB00)
	INX();
	BNE(L("fillcolmem"))

		RTS();

		LPC("pimea_logo_irq");

		INCz(havicunter);
		LDAz(havicunter)
		CMPi(15);
		BNE(L("eiviela_aloitaliiska"))

		LDAi(0xea)
		STA(L("liiskarun"))

		LPC("eiviela_aloitaliiska")

		CMPi(20);
		BNE(L("eiviela_loppuliiskanalkuglitch"))

		setirq(0, 1);
		RTS();

		LPC("eiviela_loppuliiskanalkuglitch")

		LDAz(havicunter);
		ASLa();
		ASLa();
		TAX();
		LDAx(L("regtab"))
		STA(0xd018)
		INX();
		LDAx(L("regtab"))
		STA(0xd011)
		INX();
		LDAx(L("regtab"))
		STA(0xd021)
		INX();
		LDAx(L("regtab"))
		STA(0xd020)

		RTS();




}


void ekat()
{
	pic.load("assets/logo/total-0005.png");
	pic.tohires();

	pic2.load("assets/logo/total-0005-blur.png");
	pic2.set_monochrome(0, 1);
	pic2.findchars();

	sprites.load("assets/logo/total-sprites.png");
	sprites.set_monochrome(0, 1);
	sprites.findchars();

	printf("sprites x %i y %i\n", sprites.m_bsize.x, sprites.m_bsize.y);

}

int main()
{
	srand(time(NULL));
	ekat();
	assemble(genis);

	list<segment_c *> segs;
	segs.push_back(&actualprg);
	segs.push_back(&hiresvarit);
	segs.push_back(&hireskuva);
	segs.push_back(&startvec);
	make_mprg("logo.mprg", segs);
}