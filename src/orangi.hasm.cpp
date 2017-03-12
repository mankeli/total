#include "glm.h"
#include "hires.h"

#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

hiresdata_t pic;

hiresdata_t tex;

value_t clearpos = 0x10;

void effy()
{
	int y,x,i,j;

	// generate basic shape

	ivec2 muoto[160*50];
	for (y = 0; y < 50; y++)
	{
		for (x = 0; x < 160; x++)
		{
			vec2 pos = vec2(x,y) / vec2(160.f, 100.f);


			pos -= vec2(0.5f, 0.5f);


			//pos += vec2(pos.y, pos.x);
			vec2 origpos = pos;

			//pos.y += sin(x*0.1965f) * 0.01123f;
			//pos.x += sin(y*0.16523f) * 0.01542f;


			vec2 pos2;

			pos2 = pos;
			pos2.y = 0.3f / glm::length(pos);
			pos2.x = atan2(pos.y, pos.x) / 3.14159f;

			pos2 *= vec2(60.0f, 10.f);



			ivec2 mo;
			mo.x = (int)(0x10000+pos2.x) & 0x3f;
			mo.y = (int)(0x10000+pos2.y) & 7;

			// IF THERE'S UNUSED LINE
//			if (mo.y == 7)
//				mo.y = -1;

			muoto[y*160+x] = mo;

//			if (sin(x*0.3f+y*0.4f) > 0.5f)
//				muoto[y*160+x].y = -1;
		}
	}

	// 48-79 palkki!

	class charrioffet_t
	{
	public:
		ivec2 offe[4];
		std::vector<ivec2> posset;

    	bool operator==(const charrioffet_t &other)
    	{
    		bool out = true;
    		int i;
    		for (i = 0; i < 4; i++)
    			out &= offe[i] == other.offe[i];

    		return out;
    	}
	};

	charrioffet_t megaoffe[40*200];


	for (i = 0; i < 40*200; i++)
	{
		megaoffe[i].offe[0] = ivec2(-1,-1);
		megaoffe[i].offe[1] = ivec2(-1,-1);
		megaoffe[i].offe[2] = ivec2(-1,-1);
		megaoffe[i].offe[3] = ivec2(-1,-1);
	}

	for (y = 0; y < 48; y++)
	{
		for (x = 0; x < 40; x++)
		{
			charrioffet_t tempoffet;

			tempoffet.offe[0] = muoto[y*160+x*4+0];
			tempoffet.offe[1] = muoto[y*160+x*4+1];
			tempoffet.offe[2] = muoto[y*160+x*4+2];
			tempoffet.offe[3] = muoto[y*160+x*4+3];

			megaoffe[y * 2 * 40 + x] = tempoffet;

			bool kalu = glm::linearRand(0.f,48.f) < y;

			if (kalu)
				megaoffe[(91-y) * 2 * 40 + x] = tempoffet;
		}
	}

	ivec2 monoliittimuoto[32*100];

	for (y = 0; y < 100; y++)
	{
		for (x = 0; x < 32; x++)
		{
			ivec2 tmp;

			int yfixi, yfixi1, yfixi2;
			yfixi1 = glm::max(0, x-27);
			yfixi2 = glm::max(0, 3-x);

			yfixi = powf(yfixi1,1.5f) + powf(yfixi2,1.5f);
			//yfixi = powf(cos(2.f*(x-16)/16.f),0.5f) * 10.f;

			tmp.x = (x/4-10) & 0x3f;
			tmp.y = ((y+x/9+yfixi)/5) & 7;

			monoliittimuoto[y*32+x] = tmp;
		}
		monoliittimuoto[y*32+0] = ivec2(-1);
		monoliittimuoto[y*32+31] = ivec2(-1);
	}

	for (y = 0; y < 20*4; y++)
	{
		for (x = 0; x < 8; x++)
		{
			charrioffet_t tempoffet;

			tempoffet.offe[0] = monoliittimuoto[y*32+x*4+0];
			tempoffet.offe[1] = monoliittimuoto[y*32+x*4+1];
			tempoffet.offe[2] = monoliittimuoto[y*32+x*4+2];
			tempoffet.offe[3] = monoliittimuoto[y*32+x*4+3];

			megaoffe[y * 2 * 40 + 12 + x] = tempoffet;

			int y2 = 2*20*4 - y;

			if (y2 < 99 && y2 > 20*4)
				megaoffe[y2 * 2 * 40 + 12 + x] = tempoffet;
		}
	}

#if 0
	for (y = 0; y < 200; y++)
	{
		for (x = 0; x < 40; x++)
		{
			megaoffe[y * 40 + x].offe[0].x = x;
			megaoffe[y * 40 + x].offe[0].y = y;
		}
	}
#endif

	int offetcount = 0;

	charrioffet_t totaloffe[40*200];

	for (y = 0; y < 200; y++)
	{
		for (x = 0; x < 40; x++)
		{
			charrioffet_t thisoffet = megaoffe[y * 40 + x];

			int old_idx = -1;
			for (i = 0; i < offetcount; i++)
			{
				if (totaloffe[i] == thisoffet)
					old_idx = i;
			}

			if (old_idx >= 0)
				totaloffe[old_idx].posset.push_back(ivec2(x,y));
			else
			{
				totaloffe[offetcount] = thisoffet;
				totaloffe[offetcount].posset.push_back(ivec2(x,y));
				offetcount++;
			}


		}
	}

	for (i = 0; i < offetcount; i++)
	{
		int sizz = totaloffe[i].posset.size();
//		if (sizz > 1)
//			printf("offet %i: %i tgtposses\n", i, sizz);
	}

	printf("uniikkeja offeja: %i\n", offetcount);

	//stopassembly("NO NY");



#if 1
	LDAz(0x11);
	CLC();
	ADCi(1);
	ANDi(0x3F);
	STAz(0x11);
	TAX();
#else
	LDXz(0x11);
#endif
	//LDXi(0)
	// - - . - . . - . . . -
	int ustat[9];

	for (i = 0; i < offetcount; i++)
	{
		charrioffet_t thisoffet = totaloffe[i];

		ivec2 offe0 = thisoffet.offe[0];
		ivec2 offe1 = thisoffet.offe[1];
		ivec2 offe2 = thisoffet.offe[2];
		ivec2 offe3 = thisoffet.offe[3];

		int ces = 0;

		auto emitloads = [](ivec2 offe, int lutti, int& cessi, int *ustat2)
		{
			if (offe.y < 0)
				return;

			ustat2[lutti]++;

			value_t ossa = L("tekstuuri%i,%i", lutti, offe.y) + offe.x;

			if (cessi == 0)
				LDAx(ossa)
			else
				ORAx(ossa)

			cessi++;
		};

		if (offe0 == offe1 && offe0 == offe2 && offe0 == offe3)
			{ emitloads(offe0, 4, ces, ustat); offe0.y = -2; offe1.y = -2; offe2.y = -2; offe3.y = -2; }

		if (offe0 == offe1 && offe0 == offe2)
			{ emitloads(offe0, 7, ces, ustat); offe0.y = -2; offe1.y = -2; offe2.y = -2; }
		if (offe1 == offe2 && offe1 == offe3)
			{ emitloads(offe1, 8, ces, ustat); offe1.y = -2; offe2.y = -2; offe3.y = -2; }

		if (offe0 == offe1)
			{ emitloads(offe0, 5, ces, ustat); offe0.y = -2; offe1.y = -2; }
		if (offe2 == offe3)
			{ emitloads(offe2, 6, ces, ustat); offe2.y = -2; offe3.y = -2; }

		emitloads(offe0, 0, ces, ustat);
		emitloads(offe1, 1, ces, ustat);
		emitloads(offe2, 2, ces, ustat);
		emitloads(offe3, 3, ces, ustat);

		if (ces > 0)
		{
			auto offu = [](int x, int y) { return (y / 8)*40*8+(y&7)+x*8; };
			//LDAi(0x55);

			int sizz = thisoffet.posset.size();

			for (j = 0; j < sizz; j++)
			{
				ivec2 pos = thisoffet.posset[j];
				STA(0x2000+offu(pos.x, pos.y))
			}
		}
	}

	for (i = 0; i < 9; i++)
	{
		printf("use for tex %i: %i\n", i, ustat[i]);
	}

}

void tekstuurit()
{
	int tekstuuri[128*16];

	int x,y,i;


	for (y = 0; y < 8; y++)
	{
		for(x = 0; x < 128; x++)
		{
			//int 

			vec2 pos = vec2(x,y)/vec2(16.f,1.f) - vec2(4,4);

			int arvo = 0;
			//arvo += glm::length(pos)*1.f;
			//arvo = x/8+y+sin(x*0.1f)*5.f;
			//arvo += glm::length(pos+vec2(3,5))*1.f;

			pos.x += sin(pos.y * 0.1f) * 6.f;

			arvo = pos.x+pos.y;

			//int arvo = y;
			arvo += 0x1000;
			arvo &= 3;

			arvo = 0;
//			if (x & 8) arvo = 1;
//			if (y == 0) arvo = 0;
//			if (y == 7) arvo = 1-arvo;

			int kuva[] = {
				0,1,1,1,1,1,1,0,
				1,1,0,0,0,0,0,0,
				1,1,0,0,0,0,0,0,
				1,1,1,1,1,1,0,0,
				0,0,0,0,0,0,1,0,
				0,0,0,0,0,0,1,0,
				0,1,1,1,1,1,1,0,
				0,0,0,0,0,0,0,0,

				0,1,1,1,1,1,1,0,
				1,1,0,0,0,0,0,0,
				1,1,0,0,0,0,0,0,
				1,1,1,1,0,0,0,0,
				1,1,0,0,0,0,0,0,
				1,1,0,0,0,0,0,0,
				0,1,1,1,1,1,1,0,
				0,0,0,0,0,0,0,0,

				1,1,0,0,0,0,1,0,
				0,1,1,0,0,1,1,0,
				0,0,1,0,1,1,0,0,
				0,0,0,1,1,0,0,0,
				0,0,1,1,0,0,0,0,
				0,1,1,0,1,1,0,0,
				1,1,0,0,0,0,1,0,
				0,0,0,0,0,0,0,0,

				0,1,1,1,1,1,1,0,
				0,0,0,1,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,1,0,0,0,0,
				0,0,0,0,0,0,0,0,

				0,1,1,1,1,1,1,0,
				1,1,0,0,0,0,0,0,
				1,1,0,0,0,0,0,0,
				1,1,1,1,0,0,0,0,
				1,1,0,0,0,0,0,0,
				1,1,0,0,0,0,0,0,
				0,1,1,1,1,1,1,0,
				0,0,0,0,0,0,0,0,

				1,1,1,0,0,0,1,0,
				1,1,1,1,0,0,1,0,
				1,1,0,1,1,0,1,0,
				1,1,0,0,1,0,1,0,
				1,1,0,0,0,1,1,0,
				1,1,0,0,0,1,1,0,
				1,1,0,0,0,0,1,0,
				0,0,0,0,0,0,0,0,

				1,1,1,1,1,1,0,0,
				1,1,0,0,0,0,1,0,
				1,1,0,0,0,0,1,0,
				1,1,0,0,0,0,1,0,
				1,1,0,0,0,0,1,0,
				1,1,0,0,0,0,1,0,
				1,1,1,1,1,1,0,0,
				0,0,0,0,0,0,0,0,

				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,

			};

			//arvo = kuva[y*8+(x&7)  + glm::mod((x>>3), 8)*64];
			arvo = kuva[y*8+(x&7)  + glm::mod((x>>3), 8)*64];

	//		tekstuuri[y*128+x] = arvo;
			tekstuuri[y*128+x] = tex.m_pic[y*tex.m_size.x + x];
		}
	}


	auto populatebits = [](int pixie, int bb)
	{
		int out = 0;
		if (bb & 0xF000)
			out |= pixie << 6;
		if (bb & 0x0F00)
			out |= pixie << 4;
		if (bb & 0x00F0)
			out |= pixie << 2;
		if (bb & 0x000F)
			out |= pixie << 0;
		return out;
	};

	for (i = 0; i < 9; i++)
	{
		int bitshit = 0;
		switch(i)
		{
			case 0: bitshit = 0xF000; break;
			case 1: bitshit = 0x0F00; break;
			case 2: bitshit = 0x00F0; break;
			case 3: bitshit = 0x000F; break;

			case 4: bitshit = 0xFFFF; break;
			case 5: bitshit = 0xFF00; break;
			case 6: bitshit = 0x00FF; break;
			case 7: bitshit = 0xFFF0; break;
			case 8: bitshit = 0x0FFF; break;
		}

		for (y = 0; y < 8; y++)
		{
			LPC("tekstuuri%i,%i",i, y);
			for(x = 0; x < 128; x++)
				B(populatebits((tekstuuri[y*128+(x&63)] & 0x1)*3, bitshit));
		}

		/*


		STA 

		*/


		#if 0
		for (i = 0; i < 4; i++)
		{
			LPC("tekstuuri%i,%i",i, y);
			for(x = 0; x < 256; x++)
			{
				//B((tekstuuri[y*128+(x&127)] >> (i*2)) & 0x03);
				B(((tekstuuri[y*128+(x&127)] & 0x1)*3) << (i*2));
			}
		}
		#endif
	}
}

void colmem()
{
	int x,y;
	LPC("colmem")
	for (y = 0; y < 25; y++)
		for (x = 0; x < 40; x++)
		{
			int arvo = 0x06;
			if (y == 10 || y == 11)
				arvo = 0x0e;
			if (y > 11)
				arvo = 0x4e;

			if (x >= 12 && x < 20)
			{
				if (y < 20)
					arvo = 0x70;
				else
					arvo = 0x56;
			}


//	pic.savehiresfile("normalball.hires");

			int cidx = y * 40 + x;
			arvo = ((pic.m_col2[cidx] & 0x0F) << 4) | (pic.m_col1[cidx] & 0x0f);


			B(arvo);
		}

}

void clearjekku()
{
	LDXz(clearpos);

	int i;
	LDAi(0x0);
	for (i = 0; i < 0x20; i++)
	{
		STAx(0x2000+i*0x100)
	}

	INX();

	//LDAi(0xFF);
	LDAi(0);
	for (i = 0; i < 0x20; i++)
	{
		STAx(0x2000+i*0x100)
	}

	CPXi(0xFF);
	BEQ(L("clearstop"))

	INX();
	STXz(clearpos);

	RTS();

	LPC("clearstop");

	setirq(L("toinenirq"), 2);


}

void msdoseffu()
{
	CTX("valo")

	startvec.begin(if_startvec);

	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

	actualprg.begin(0x3c00);

	colmem();


	SEG->setpc(0x4000);

LPC("actualbeg")

//	JSR(if_startmusic);
	LDAi(0x35);
	STAz(1);


	//LPC("waitforstart")
	//jump_if_not_time(0x1f00, L("waitforstart"));


	MOV8i(0xd020, 0x0);
	MOV8i(0xd021, 0x0);
	MOV8i(0xd011, 0x3B);

	MOV8i(0xd018, 0x18);
	MOV8i(0xd016, 0xC8);

	MOV8i(0xdd02, 0x3c);

	MOV8zi(clearpos, 0);

	LDXi(0x00);
	//TXA();
	LDAi(0x10);
#if 0
LPC("fillloop")
	STAx(0x400);
	STAx(0x500);
	STAx(0x600);
	STAx(0x700);
	INX();
	BNE(L("fillloop"))
#else
LPC("fillloop")
	LDAx(L("colmem")+0x000)
	STAx(0x400);
	LDAx(L("colmem")+0x100)
	STAx(0x500);
	LDAx(L("colmem")+0x200)
	STAx(0x600);
	LDAx(L("colmem")+0x300)
	STAx(0x700);
	INX();
	BNE(L("fillloop"))
#endif


int i;
	LDXi(0);
LPC("hiresclearloop")
	LDAi(0x0);
	for (i = 0; i < 0x20; i++)
	{
		STAx(0x2000+i*0x100)
	}

	INX();

	BEQ(L("hiresclearstop"))
	JMP(L("hiresclearloop"))

	LPC("hiresclearstop");

//	setirq(L("jekkuirq"), 2);
setirq(L("toinenirq"), 2);

	PRINTPOS("effy alkaa");
LPC("ikiloop")
//	INC(0xd020);
	LDAi(0x30);
	STAz(1);
	effy();
	LDAi(0x35);
	STAz(1);
//	DEC(0xd020);


	jump_if_not_time(0x2400, L("loopba_jmp"));
	
	setirq(0, 1);
	setirq(0, 2);

	MOV8i(0xd011, 0);
	JMP(if_executenext_nowait);

	LPC("loopba_jmp")
	JMP(L("ikiloop"))


LPC("jekkuirq")
	clearjekku();
	RTS();

LPC("toinenirq")
	RTS();

	PAGE
	PRINTPOS("tekstuurit alkaa");

	tekstuurit();


}


void ekat()
{
			pic.load("assets/maisema.png");
			pic.findcolors(1);

			tex.load("assets/maisema_tekstuuri2.png");
			tex.set_monochrome(0, 1);

			printf("%i,%i\n", tex.m_size.x, tex.m_size.y);

}

int main()
{
	srand(time(NULL));
	ekat();
	assemble(msdoseffu);

	list<segment_c *> segs;
	segs.push_back(&actualprg);
	segs.push_back(&startvec);
	make_mprg("orangi.mprg", segs);
}