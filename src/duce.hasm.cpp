#include "glm.h"
#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

int plasmacounter1 = 0x11; // 2
int plasmacounter2 = 0x13; // 2
int plasmacounter3 = 0x15; // 2

void effy()
{
	int x,y;

	LPC("effy")

	ADD16i(plasmacounter1, 0x297);
	ADD16i(plasmacounter2, 0x389);
	ADD16i(plasmacounter3, 0x153);


	LDXz(plasmacounter1+1)
	LDYz(plasmacounter2+1)

	for (y = 0; y < 13; y++)
	{
		for (x = 0; x < 20; x++)
		{
			const int bv = 0x100000;
			LDAx(L("taabelisin") + (bv+(x * 10) & 0xFF))
			ADCx(L("taabelisin") + (bv+(y * 12) & 0xFF))
	LDYz(plasmacounter2+1)
			ADCy(L("taabelisin2") + (bv+(x * -20) & 0xFF))
			ADCy(L("taabelisin2") + (bv+(y * -24) & 0xFF))
	LDYz(plasmacounter3+1)
			ADCy(L("taabelisin2") + (bv+((x+y) * 16) & 0xFF))
			ADCy(L("taabelisin2") + (bv+((x-y) * 19) & 0xFF))

			ANDi(0xfc);

			STA(0x400 + (y*2+0)*40+x*2 + 0)
			EORi(0x01);
			STA(0x400 + (y*2+0)*40+x*2 + 1)

			if (y*2+1 < 25)
			{
				EORi(0x02);
				STA(0x400 + (y*2+1)*40+x*2 + 1)
				EORi(0x01);
				STA(0x400 + (y*2+1)*40+x*2 + 0)
			}
		}
	}

	RTS();

	int i;
	PAGE
	LPC("taabelisin")	
	for (i = 0; i < 512; i++)
	{
		int arvo = (int)(0x100 + sin(i * 1.f * 3.14159f / 128.f) * 110.f) & 0xFF;
		B(arvo);
	}
	LPC("taabelisin2")	
	for (i = 0; i < 512; i++)
	{
		int arvo = (int)(0x100 + sin(i * 1.f * 3.14159f / 128.f) * 70.f) & 0xFF;
		B(arvo);
	}

}

void emitcharset()
{
	int i,j;

	auto emitmerkki = [](int *graffa, ivec2 offu)
	{
		int j, k;
		for (j = 0; j < 8; j++)
		{
//			int merq = fgetc(f);
//			if (setti >= 1)
//				merq &= hash(setti+j) &  0xFF;
			int merq = 0;
			for (k = 0; k < 4; k++)
			{
				merq |= graffa[(j + offu.y) * 16 + (k*2 + offu.x)] << (6 - k*2);
			}

			B(merq);
		}
	};

	auto makemerk = [](int *graffa, float ang)
	{
		int x,y;
		for (y = 0; y < 16; y++)
		{
			for (x = 0; x < 16; x++)
			{
				vec2 p = vec2(x, y) - vec2(8);

				float c = (glm::atan(p.y, p.x) / 3.14159f) * 0.5f + 0.5f;
				c = glm::fract(c + ang);

				//float ar = ang * 3.14159f * 2.f;
				//vec2 dirri = vec2(cos(ar), sin(ar));
				//float c = glm::dot(p, dirri) * 0.1f + 0.5f;


				c *= 4.f;

				//c = (8+p.y) / 4.f;

//				if (x > 2 && x < 14 && y > 2 && y < 14)
//					c *= 0.5f;

				c += glm::linearRand(-1.0f, 1.0f) * 0.6f;

				int ci = glm::clamp(0, 3, (int)c);


				graffa[y * 16 + x] = ci;
			}
		}
	};


	for (i = 0; i < 64; i++)
	{

			int graffa[16*16];
			makemerk(graffa, (float)i / 64.f);
			emitmerkki(graffa, ivec2(0, 0));
			emitmerkki(graffa, ivec2(8, 0));
			emitmerkki(graffa, ivec2(0, 8));
			emitmerkki(graffa, ivec2(8, 8));
	}

}

void genis()
{
	CTX("valo")

	startvec.begin(if_startvec);

	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

	actualprg.begin(0x3800-64);

	int i;
	for (i = 0; i < 16*3; i++)
		B(0xFF);
	for (i = 0; i < 5*3; i++)
		B(0x00);

	SEG->setpc(0x3800);

	emitcharset();

LPC("actualbeg")

	MOV8i(0xd011, 0x1B);

	//MOV8i(0xd018, 0x14);
	MOV8i(0xd018, 0x1E);
	MOV8i(0xd016, 0xD8);
	MOV8i(0xdd02, 0x3C);

	int sprnum = 0x3800/64 - 1;
	LDAi(sprnum)
	STA(0x7F8);
	STA(0x7F9);
	STA(0x7FA);
	STA(0x7FB);
	STA(0x7FC);
	STA(0x7FD);
	STA(0x7FE);
	STA(0x7FF);

	for (i = 0; i < 7; i++)
	{
		MOV8i(0xd000+i*2, 25);
		MOV8i(0xd001+i*2, 50+i*32);
		MOV8i(0xd027+i, 0x7);
	}

//	MOV8i(0xd015, 0xFF);
	MOV8i(0xd015, 0x00); // FINAALIN
	MOV8i(0xd017, 0xFF);
	MOV8i(0xd01D, 0xFF);

	MOV8i(0xd01C, 0x00);
	MOV8i(0xd01B, 0x00);

	//uint8_t pal[] = {0xb, 0xf, 0xc, 0xb, 0x8};
	uint8_t pal[] = {0x7, 0x2, 0x8, 0xa, 0xF};

	LDXi(0);
	LDAi(pal[4])
	LPC("merkcollop")
	STAx(0xD800)
	STAx(0xD900)
	STAx(0xDA00)
	STAx(0xDB00)
	DEX();
	BNE(L("merkcollop"))

	MOV8i(0xd020, pal[0]);
	MOV8i(0xd021, pal[1]);
	MOV8i(0xd022, pal[2]);
	MOV8i(0xd023, pal[3]);

	setirq(L("irqrut"), 2);

//	JMP(0xc90);

LPC("ikiloop")
//	JMP(L("ikiloop"))
	jump_if_not_time(0x1740, L("ikiloop"));

	MOV8i(0xd015, 0);
	MOV8i(0xd011, 0);

	setirq(L("colorjuduirq"), 2);

#if 1
//	JSR(0xC90);
//LPC("ikiloop2")
//	JMP(L("ikiloop"))
//	jump_if_not_time(0x1780, L("ikiloop2"));

//	setirq(0, 1);
//	setirq(0, 2);

	LDXi(LB(0x17C0))
	LDYi(HB(0x17C0))
	JMP(if_executenext);

//	JMP(if_executenext_nowait);
//	JMP(if_startvec);
#else

	JSR(0xC90);
LPC("ikiloop2")
	JMP(L("ikiloop"))
	jump_if_not_time(0x17c0, L("ikiloop2"));

	setirq(0, 1);
	setirq(0, 2);

	JMP(if_startvec);

#endif

LPC("colorjuduirq")

//	INC(0xd020);
	LDAi(40);
LPC("waitforoutscreen")
	CMP(0xd012);
	BNE(L("waitforoutscreen"))

	LDAz(framecounter+1)
	CMPi(0x17);
	BNE(L("stopvarifade"))
	LDXz(framecounter)
	CPXi(0XC0);
	BCS(L("stopvarifade"))
	LDAx(L("varitab")-0x40) // 0x40 == start from 0x1740
	STA(0xd020);
	RTS();

	LPC("stopvarifade")
	setirq(0, 2);
	RTS();


	int varit[] = {0x7,0xd,0x3,0xf,0x5,0xa,0xc,0xe,0x8,0x4,0x2,0xb,0x6,0x9, 0};
	int varic = 15;

	LPC("varitab")
	for (i = 0; i < 128; i++)
	{
		float ff = (float)i / 63.f;
		ff *= varic;
		ff += glm::linearRand(-0.3f, 0.3f);

		int ffi = glm::clamp((int)ff, 0, varic - 1);
		B(varit[ffi]);

	}




LPC("irqrut")
	CLI();

	JSR(L("effy"))

	RTS();

	effy();

}


void ekat()
{
}

int main()
{
	srand(time(NULL));
	ekat();
	assemble(genis);

	list<segment_c *> segs;
	segs.push_back(&actualprg);
	segs.push_back(&startvec);
	make_mprg("duce.mprg", segs);
}