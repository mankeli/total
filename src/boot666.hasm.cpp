#include "hires.h"

#include "glm.h"
#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

hiresdata_t sprites;

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

value_t spriteposses_ptr = 0x10; // 2
value_t state_tmp = 0x12;
value_t exitcunt = 0x13;
value_t counter = 0x20;
value_t counter_spd = 0x21;
value_t screenmem_ptr1 = 0x22; // 2
value_t screenmem_ptr2 = 0x24; // 2

void effy()
{
	
}

void genis()
{
	CTX("valo")
	int x,y,i;

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

	actualprg.begin(0xA000);

	for (i = 0; i < 16; i++)
	{
		int yc = i * 21;
		LPC("sprites%i", i)
		emitsprite(sprites, ivec2(0,yc));
	}
	LPC("spritesend");

	PRINTPOS("sprites end")



	PRINTPOS("tassa koodi")
LPC("actualbeg")

	JSR(if_startmusic)



	LDXi(0)
	LPC("loopcol2")
	LDAx(0x400)
	STAx(0x8400)
	LDAx(0x500)
	STAx(0x8500)
	LDAx(0x600)
	STAx(0x8600)
	LDAx(0x700)
	STAx(0x8700)
	INX();
	BNE(L("loopcol2"))

	MOV8i(0xDD02, 0x3e);

	MOV8i(0xd011, 0x1B);

//	LPC("Asskis");
//	jump_if_not_time(0x666, L("Asskis"));

	LDXi(0x4*8);
LPC("clearspritestates")
	STAx(counter-1)
	DEX();
	BNE(L("clearspritestates"))

	MOV8i(exitcunt, 0);



	int firstsprnum = (L("sprites0") & 0x3FFF) / 64;
	LDXi(firstsprnum)

	// alusta positiot ehkä?
	LDAi(0xE);
	for (i = 0; i < 8; i++)
	{
		STA(0xd027+i)
	}

	MOV8i(0xd015, 0xFF);
	MOV8i(0xd017, 0xff);
	MOV8i(0xd01D, 0x00);
	MOV8i(0xd010, 0x00);

	MOV8i(0xd01C, 0x00);
	MOV8i(0xd01B, 0x00);


	LDAi(0x0);
	for (i = 0; i < 8; i++)
	{
		value_t baseptr = i * 6;
		STAz(baseptr+counter)
	}

	for (i = 0; i < 8; i++)
	{
		LDAi((i*9) & 0x0f);
		value_t baseptr = i * 6;
		STAz(baseptr+counter_spd)
	}



	MOV16i(spriteposses_ptr, L("spriteposses"));
	//B(2);

//	MOV8i(counter_spd, 1);



	setirq(L("irqrut"), 1);


	JSR(0xC90);

LPC("ikiloop")
	effy();

	for (i = 0; i < 8; i++)
	{
		LDX(0xd001+i*2)
		INX();
		BNE(L("ikiloop"))
	}
	setirq(0, 1);
//	JSR(if_executenext_nowait)
	JMP(0xFFF0);
//	JMP(L("ikiloop"))

LPC("irqrut")

	CLI();

	LDAz(framecounter+1)
	BNE(L("eivarifadea"))
	LDXz(framecounter)
	CPXi(64);
	BCS(L("eivarifadea"))
	LDAx(L("varitab"))
	STA(0xd020);

	LPC("eivarifadea")

	JSR(L("vitun tilakone"))

	RTS();


	struct blokki_t
	{
		ivec2 sprpos;
		int ylabit;
		uint16_t scrpos1;
		uint16_t scrpos2;
		int used;
	};

	/*
		8 threadia.

		tila kaikilla.

		tila = counter >> 4;
		jos tila = 0, ota listasta seuraava blokki, ota random nopeus
		tila suoraan framen  cnumero???

	*/

	const int maxblkcount = 20*13*2;

	blokki_t blokit[maxblkcount];

	int blkcount = 0;

	for (y = 0; y < 13; y++)
	{
		for (x = 0; x < 20; x++)
		{
			blokki_t bs;
			int yadd = (x & 1);
			int ry = y;
			if (y == 12 && yadd == 1)
				ry = -1;

			bs.sprpos = ivec2(x,ry) * 16 + ivec2(0,yadd) * 8 + ivec2(20,37);

			int final_y = (ry)*2 + yadd;

			if (final_y >= 0)
				bs.scrpos1 = 0x8400+(final_y)*40+x*2;
			else
				bs.scrpos1 = 0x400;

			if (final_y < 24)
				bs.scrpos2 = 0x8400+(final_y+1)*40+x*2;
			else
				bs.scrpos2 = 0x400;

			bs.ylabit = bs.sprpos.x >= 256 ? 1 : 0;
			bs.used = 0;
			blokit[blkcount] = bs;

			blkcount++;

		}
	}

	int id[blkcount];

#if 0
	for (i = 0; i < 20*13; i++)
	{
		id[i] = i;
	}
	for (i = 0; i < 20*13; i++)
	{
		int old;
		int ridx = rand() % (20*13);
		old = id[i];
		id[i] = id[ridx];
		id[ridx] = old;
	}
#endif
	vec2 cp = vec2(20+155,37+95);
	for (i = 0; i < blkcount; i++)
	{
		int j;
		float l_disti = 0.f;
		int l_i = 0;
		vec2 l_cp ;
		for (j = 0; j < blkcount; j++)
		{
			vec2 pp = vec2(blokit[j].sprpos);
			float disti = glm::length(pp - cp);

			if (!blokit[j].used && disti > l_disti)
			{
				l_disti = disti;
				l_i = j;
				l_cp = pp;

			}
		}

		blokit[l_i].used = 1;
		id[i] = l_i;
//		cp = l_cp;
	}


	LPC("spriteposses")
	for (i = 0; i < blkcount; i++)
	{
		int i2 = id[i];
		blokki_t bs = blokit[i2];
		B(bs.ylabit);
		B(bs.sprpos.x);
		B(bs.sprpos.y);
		W(bs.scrpos1);
		W(bs.scrpos2);
	}
	B(0xFF);


	LPC("vitun tilakone")

	for (i = 0; i < 8; i++)
	{
		PRINTPOS("tilakone for sprite %i", i);
		value_t baseptr = i * 6;
	LDAz(baseptr+counter)
	ADCz(baseptr+counter_spd)
	STAz(baseptr+counter)

	LSRa();
	LSRa();
	LSRa();
	LSRa();
	STAz(state_tmp)
	CLC();
	ADCi(firstsprnum);
	STA(0x87F8+i);

	LDAz(state_tmp)

	LPC("supermaaginen%i", i)
	CMPi(10)
	BNE(L("donotreplacemerk%i",i))

	LDYi(0);
	LDAi(233);
	STAizy(baseptr+screenmem_ptr1)
	LDAi(95);
	STAizy(baseptr+screenmem_ptr2)

	LDYi(1);
	LDAi(223);
	STAizy(baseptr+screenmem_ptr1)
	LDAi(105);
	STAizy(baseptr+screenmem_ptr2)


	// VÄRIMUISTI - NÄYTTÖMUISTI
	value_t addioffs = 0xD800 - 0x8400;
	ADD16zi(baseptr+screenmem_ptr1, addioffs);
	ADD16zi(baseptr+screenmem_ptr2, addioffs);

	LDAi(0xe);
	LDYi(0);
	STAizy(baseptr+screenmem_ptr1)
	STAizy(baseptr+screenmem_ptr2)
	LDYi(1);
	STAizy(baseptr+screenmem_ptr1)
	STAizy(baseptr+screenmem_ptr2)

	LDAi(0xa9)
	STA(L("supermaaginen%i", i))
	LDAi(1)
	STA(L("supermaaginen%i", i)+1)

LPC("donotreplacemerk%i",i)

	LDAz(state_tmp)
	CMPi(0)
	BNE(L("donothingtothis%i",i))

/*
	readmerkki(graffa_d1, f, 233);
	readmerkki(graffa_d2, f, 95);
	readmerkki(graffa_d3, f, 223);
	readmerkki(graffa_d4, f, 105);
*/

	LDYi(0);


	LDAizy(spriteposses_ptr)
	BMI(L("donothingtothis_exit%i",i))

	BNE(L("set8bit%i", i))
	LDA(0xd010);
	ANDi((1<<i) ^ 0xFF);
	STA(0xd010)
	JMP(L("ok8bithandled%i",i))

	LPC("set8bit%i",i)
	LDA(0xd010);
	ORAi((1<<i));
	STA(0xd010)

	LPC("ok8bithandled%i", i)
	INY();

	LDAizy(spriteposses_ptr)
	STA(0xd000 + i * 2)
	INY();

	LDAizy(spriteposses_ptr)
	STA(0xd001 + i * 2)
	INY();

	LDAizy(spriteposses_ptr)
	STA(baseptr + screenmem_ptr1+0)
	INY();

	LDAizy(spriteposses_ptr)
	STA(baseptr + screenmem_ptr1+1)
	INY();

	LDAizy(spriteposses_ptr)
	STA(baseptr + screenmem_ptr2+0)
	INY();

	LDAizy(spriteposses_ptr)
	STA(baseptr + screenmem_ptr2+1)


	ADD16zi(spriteposses_ptr, 7);

	LDAi(0xc9)
	STA(L("supermaaginen%i", i))
	LDAi(10)
	STA(L("supermaaginen%i", i)+1)



	LDX(0xD41B);
	LDAx(L("hashtab")+i)
	STAz(baseptr + counter_spd)
	LDAi(0x10);
	STAz(baseptr + counter)

	JMP(L("donothingtothis%i",i))

	LPC("donothingtothis_exit%i",i)
	MOV8i(0xd001+i*2, 0xFF);

	LPC("donothingtothis%i",i)
}

	RTS();

	LPC("hashtab")
	for (i = 0; i < 256+8; i++)
	{
		B((rand() % 4)+11);

	}


	int varit[] = {0xe, 0x8, 0x4,   0xB, 0x2, 0x9, 0x6};
	int varic = 7;

	LPC("varitab")
	for (i = 0; i < 64; i++)
	{
		float ff = (float)i / 63.f;
		ff *= varic;
		ff += glm::linearRand(-0.5f, 0.5f);

		int ffi = glm::clamp((int)ff, 0, varic - 1);
		B(varit[ffi]);

	}


}


void ekat()
{
	int x,y;
	int i;
	sprites.init(ivec2(24, 16*21));

	for (i = 0; i < 16; i++)
	{
		for (y = 0; y < 21; y++)
		{
			for (x = 0; x < 24; x++)
			{
				int col;
				vec2 pos = vec2(x,y) - vec2(12.f, 10.0f);
				pos.y *= 2.f;

				//float dist = glm::length(pos) - i;

				float t1 = glm::min(1.f, (float)i / 10.f);
				t1 = powf(t1, 2.0f);
				float t2 = glm::max(0.f, (float)(i-10)/5.f );

				float radi = glm::mix(0.f, 16.f, t1);
				radi = glm::mix(radi, 8.f, t2);


				float dist = (abs(pos.x)+abs(pos.y)) - radi;
				if (dist < 0)
					col = 1;
				else
					col = 0;

				sprites.m_pic[(i*21+y)*24+x] = col;
			}
		}

#if 0
		for (y = 0; y < 16; y++)
		{
			int xo = 4 + y;
			int yo = 2 + y;
			sprites.m_pic[(i*21+2)*24+xo] ^= 1;
			sprites.m_pic[(i*21+2+15)*24+xo] ^= 1;
			sprites.m_pic[(i*21+yo)*24+4] ^= 1;
			sprites.m_pic[(i*21+yo)*24+4+15] ^= 1;
		}
#endif
	}


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
	segs.push_back(&startvec);
	make_mprg("boot666.mprg", segs);
}