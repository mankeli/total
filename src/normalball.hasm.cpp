#include "hires.h"

#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"
#include "glm.h"


#define BORDER_NMI

using namespace hashembler;

segment_asm_c actualprg;

hiresdata_t pic;

	value_t gfxmemout = 0x4000;

value_t ctptr1 = 0x40; // s2

value_t ctptr2 = 0x42; // s2
value_t ctptr3 = 0x44; // s2
value_t ctptr4 = 0x46; // s2


value_t ang = 0x48; // s2

value_t which_irq = 0x4a;

int sprytab[13];
vector<uint8_t> spritedata;

int piccolors[40*25];

float roundjitter(float in)
{
	float dither = 0.5f;
	dither = glm::linearRand(0.f, 1.f);
	if (glm::fract(in) < dither)
		return floor(in);
	else
		return ceil(in);

}

int coltab[65536];
int coltab_used[65536];
int coltab_len = 0;
int coltab_offsets[256];

int coltab_num = 110;

void msdoseffu()
{
	CTX("valo")

	actualprg.begin(0x2000);
	set_segment(&actualprg);

	LPC("begin");
	PRINTPOS("begin");

	JMP(L("actualbeg"))
LPC("actualbeg")

{
	//JSR(if_startmusic)


	LDAi(0x9)
	STA(0xd020)


	LDAi(0x3D);
	STA(0xDD02);
	LDAi(0x08);
	STA(0xd018);


	LDAi(0x33)
	STA(0xd011)

	LDAi(0x00+3)
	STA(0xd016)

	// sprite positions set at topirq

	LDAi(0x60);
	STA(0xd010);

	LDAi(0x7f)
	STA(0xd015);

	LDAi(0xFF)
//	STA(0xd017); // dh
	STA(0xd01D); // dw

	LDAi(0x00);
	STA(0xd017); // dh
	STA(0xd01b); //prio


	INC(0xd021);

	LDXi(LB(L("topirq")))
	LDYi(HB(L("topirq")))
	JSR(if_setirq2)

	LDXi(LB(L("colorshit")))
	LDYi(HB(L("colorshit")))
	JSR(if_setirq1)


	LPC("loopba")
	jump_if_not_time(0x400, L("loopba"));

	JSR(if_unsetirq1);
	JSR(if_unsetirq2);
	JMP(if_executenext_nowait);

}

LPC("topirq")
{
	//DEC(0XD020);
	LDAi(0x7F)
	STA(0xDD0D);

	LDA(framecounter)
	ANDi(1);
	CLC();
	ADCi(24+2);

	LDXi(0x9);
	int i;
	for (i = 0; i < 7; i++)
	{
//		LDAi(24 + i*48)
		STA(0xd000 + i*2)
		CLC();
		ADCi(48);
		STX(0xd027+i)
	}


	LDAi(LB(L("nmi")))
	STA(0xfffa)
	LDAi(HB(L("nmi")))
	STA(0xfffb)

	LDAi(0x0);
	STA(0xDD0E);
	STA(0xDD0F);

	value_t first_nmi = 63*73;
	LDAi(LB(first_nmi))
	STA(0xdd04)
	LDAi(HB(first_nmi))
	STA(0xdd05)


	LDAi(0x1C);
LPC("wait_for_line")
	CMP(0xd012);
	BNE(L("wait_for_line"))

	// start counter, irq
	LDAi(0x19);
	STA(0xDD0E);

	LDA(0xDD0D)
	LDAi(0x01)
	STA(0xDD0D);

#ifdef BORDER_NMI
	INC(0xd020);
	DEC(0xd020);
#endif

	RTS();
}

{
	int i;

LPC("nmi")
	PHA();
	TXA();
	PHA();
	TYA();
	PHA();
#ifdef BORDER_NMI
//	LDAi(0x02)
//	STA(0xd020);
	INC(0xd020);
#endif
	value_t first_nmi = 63*16;

	// cia timer begin
	LDAi(LB(first_nmi))
	STA(0xdd04)
	LDAi(HB(first_nmi))
	STA(0xdd05)

	LDAi(0x11);
	STA(0xDD0E);

	LDAi(LB(L("nmi2")))
	STA(0xfffa)
	LDAi(HB(L("nmi2")))
	STA(0xfffb)

	LDAi(0x81)
	STA(0xDD0D);
	LDA(0xDD0D)

	LDAi(50)
	for (i = 0; i < 7; i++)
	{
		STA(0xd001 + i * 2)
	}

	for (i = 0; i < 7; i++)
	{
		int sn = (0x400) / 64 + i;
		LDAi(sn)
		STA(0x4000+0x3f8 + i)
	}

	LDXi(0x01);
	STXz(which_irq)


#ifdef BORDER_NMI
//	LDAi(0xf)
//	STA(0xd020);
	DEC(0xd020);
#endif


	PLA();
	TAY();
	PLA();
	TAX();
	PLA();

	RTI();
}

{
	int i;

LPC("nmi2")
	PHA();
	TXA();
	PHA();
//	TYA();
//	PHA();


#ifdef BORDER_NMI
//	LDAi(0x01)
//	STA(0xd020);
	INC(0xd020);

#endif
	LDXz(which_irq)

//	B(2);

	for (i = 0; i < 7; i++)
	{
		LDAx(L("sprptrtab%i", i))
		STA(0x4000+0x3f8 + i)
	}

	LDAx(L("sprytab"))
	for (i = 0; i < 7; i++)
	{
		STA(0xd001 + i * 2)
	}



	LDA(0xDD0D)

	INX();
	STXz(which_irq)
	CPXi(13);
	BNE(L("endnmi"))

	LDAi(0x7F)
	STA(0xDD0D);


LPC("endnmi")

#ifdef BORDER_NMI
//	LDAi(0xf)
//	STA(0xd020);
	DEC(0xd020);

#endif

//	PLA();
//	TAY();
	PLA();
	TAX();
	PLA();

	RTI();
}


LPC("sprytab")
{
	int i;
	for (i = 0; i < 13; i++)
	{
		int sprite_y = sprytab[i];
		B(sprite_y)
	}

	int sn = (0x400) / 64;

	int j;

	for (i = 0; i < 7; i++)
	{
		LPC("sprptrtab%i", i)
		for (j = 0; j < 13; j++)
		{
			int ptr = sn + j * 7 + i;
//			ptr = sn+7 + (j&1) ? 8 : 0;
			B(ptr)
		}
	}
}

LPC("colorshit")
{

#if 0
		LDA(0xd011);
		EORi(0x03);
		ANDi(0x7F);
		STA(0xd011);

		ANDi(0x01);
	BNE(L("skipd016"))
		LDA(0xd016);
		EORi(0x03);
		STA(0xd016);
	LPC("skipd016")
#endif


	ADD16zi(ang, 0x105);

	LDAz(ctptr3+0)
	STAz(ctptr4+0)
	LDAz(ctptr3+1)
	STAz(ctptr4+1)

	LDAz(ctptr2+0)
	STAz(ctptr3+0)
	LDAz(ctptr2+1)
	STAz(ctptr3+1)

	LDAz(ctptr1+0)
	STAz(ctptr2+0)
	LDAz(ctptr1+1)
	STAz(ctptr2+1)


	LDXz(ang + 1)
	LDAx(L("colptrtab_l"))
	STAz(ctptr1+0)
	LDAx(L("colptrtab_h"))
	STAz(ctptr1+1)




	CLI();

	int x,y;
	for (y = 0; y < 25; y++)
	{
		for (x = 0; x < 40; x++)
		{
			int offs = y*40+x;
//			LDY(gfxmemin + offs)
			LDYi(piccolors[offs]);

#if 0
			if ((x&1) == (y&1))
				LDAizy(ctptr1)
			else
				LDAizy(ctptr2)
#else

#if 1
			int kusi = x;
			kusi += y;
#else
			int kusi = (int)glm::length(vec2(x-20,y-12));
#endif

			kusi %= 6;
			if (kusi == 0)
				LDAizy(ctptr1)
			else if (kusi == 1)
				LDAizy(ctptr2)
			else if (kusi == 2)
				LDAizy(ctptr3)
			else if (kusi == 3)
				LDAizy(ctptr4)
			else if (kusi == 4)
				LDAizy(ctptr3)
			else if (kusi == 5)
				LDAizy(ctptr2)

#endif

//			LDAi(0x01);
			STA(gfxmemout + offs)
		}
	}
	RTS();
}

	PRINTPOS("COLOR DATA")
{
	int x,y,i;

SEG->setpc(0x4000);

	for (i = 0; i < 40*25;i++)
	{
		//B(0x20);
		int cval = ((pic.m_col2[i] & 0x0F) << 4) | (pic.m_col1[i] & 0x0f);
		//int cval = rajahdys.m_col1[i] & 0x0f;
		B(cval);
	}

	PRINTPOS("SPRITE DATA")

SEG->setpc(0x4000+0x400);

	for (i = 0; i < spritedata.size();i++)
	{
		B(spritedata[i]);
	}


	PRINTPOS("HIRES_DATA")

SEG->setpc(0x6000);

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
}


SEG->setpc(0x8000);

	PRINTPOS("end of eff2")

LPC("coltab")
{
	int i;
	for (i = 0; i < coltab_len;i++)
	{
		B(coltab[i]);
//		B(0x01);
	}
}


LPC("colptrtab")
{
	int i;
	int offs[512];
	for (i = 0; i < 512;i++)
	{
		float ct_f = (float)(i) / (float)256.f;
		int ct = roundjitter(ct_f * coltab_num);
		//ct = glm::clamp(ct, 0, coltab_num - 1);
		ct %= coltab_num;
		offs[i] = coltab_offsets[ct] + L("coltab");
	}

	LPC("colptrtab_l")
	for (i = 0; i < 512;i++)
		B(LB(offs[i]));

	LPC("colptrtab_h")
	for (i = 0; i < 512;i++)
		B(HB(offs[i]));

}


}

int *create_sprite_mappings(int spritecount, int firstspriteline, int firstnmiline)
{
	int y;

	int offs = firstnmiline - firstspriteline;


	const int maxvisi = 5;

	int sprvisi[spritecount];
	int sprvisid[spritecount];
	memset(sprvisi, 0, sizeof(int) * spritecount);
	memset(sprvisid, 0, sizeof(int) * spritecount);
	int sprnum = -1;

	int *sprite_linemap = new int[sizeof(int) * spritecount * 21];
	memset(sprite_linemap, 0, sizeof(int) * spritecount * 21);

	sprnum = 0;
	sprvisi[0] = maxvisi;

	int i;

	for (y = 0; y < 200; y++)
	{
		int barrel_y = y % 21;

		if (y % 16 == offs)
		{
			if (sprnum >= 0)
				sprvisid[sprnum] = -1;
			sprnum++;

			sprvisi[sprnum] = maxvisi;
		}

		for (i = 0; i < spritecount; i++)
		{
			sprvisi[i] += sprvisid[i];
			if (sprvisi[i] < 0)
			{
				sprvisi[i] = 0;
				sprvisid[i] = 0;
			}
		}


		printf("y: %02X, %02X: ", y, barrel_y);

		for (i = 0; i < spritecount; i++)
		{
			if (sprvisi[i] > 0)
				printf("%X", i);
			else
				printf(" ");

			if (sprvisi[i] > 0)
			{
				int lmo = barrel_y * spritecount + i;

				if (sprite_linemap[lmo])
					printf("!");
				else
					printf("+");

				sprite_linemap[lmo] = y;
			}
			else
				printf(" ");

			printf(" ");

		}
		printf("\n");
		if (barrel_y == 20)
		{
			printf("---------\n");
		}


	}

	printf("\n\n");
	for (i = 0; i < spritecount; i++)
	{
		printf("sprite %i:\n", i);
		for (y = 0; y < 21; y++)
		{
			printf("%02X: %02X\n", y, sprite_linemap[y * spritecount + i]);
		}

	}

	return sprite_linemap;
}

void ekat_spritegen()
{
	int i,y,x;
	int set;

int firstnmiline = 62;
int firstspriteline = 50;
int sprites_per_col = 13;

sprytab[0] = firstspriteline;

	for (i = 0; i < sprites_per_col; i++)
	{
		int nmi_y = firstnmiline + i * 16;
		int gfx_y = (nmi_y - firstspriteline);
		int sprite_set = (nmi_y - firstspriteline) / 21 + 1;
		int sprite_set_y = sprite_set * 21 + firstspriteline;
		int line_in_sprite = (nmi_y - firstspriteline) % 21;
		printf("sprite_y tab %i: nmi %i, gfx_y %i, sprset %i, sprset_y %i, spriteline %i\n", i, nmi_y, gfx_y, sprite_set, sprite_set_y, line_in_sprite);

		sprytab[i + 1] = sprite_set_y;
	}


int *sprlinemap = create_sprite_mappings(sprites_per_col, firstspriteline, firstnmiline);

	hiresdata_t spr;
	spr.load("assets/extnor_sprites.png");
	spr.scale_halfwidth();
	spr.set_monochrome(1, 0);
	spr.findchars();

#if 0
	for (x = 0; x < spr.m_bcount*8; x++)
	{
		spr.m_chars[x] = 0xAA;
	}
#endif

	for (set = 0; set < sprites_per_col; set++)
	{
		for (i = 0; i < 7; i++)
		{
			for (y = 0; y < 21; y++)
			{
				//int co = (y+0) * 20 + i * 3;
				//int ys = y + set * 21;

				int ys = sprlinemap[y * sprites_per_col + set];

//				int ryd = ys/16;
//				int dd = ryd*21;

//				dd = 0xAA;


				int yb = ys/8;
				int ybo = ys&7;
				int co = yb*spr.m_bsize.x*8 + (i*3)*8 + ybo;

				//spr.m_chars[co + 8] ^= 0xFF;

				spritedata.push_back(spr.m_chars[co + 0]);
				spritedata.push_back(spr.m_chars[co + 8]);
				spritedata.push_back(spr.m_chars[co + 16]);

			}
			spritedata.push_back(0);
		}
	}

}





void genis()
{
	msdoseffu();
}

void putct(int ctnum, vec3 lightdir, vec3 *normals, int *colcombs)
{
	int i;
//	int palette[] = {0x6, 0x2, 0x4, 0xe, 0xa, 0x7, 0x1};
	int palette[] = {0x9, 0xb, 0x8, 0x5, 0xf, 0xd, 0x1};
	int palsize = sizeof(palette) / sizeof(palette[0]);
	int thisct[16];

	for (i = 0; i < 16; i++)
	{
		float dotti = glm::dot(normals[i], lightdir);

		dotti = glm::clamp(dotti, 0.f, 1.f);
//		dotti = 0.f;
		//dotti += normals[i].z * 0.04f;
		int idx = glm::clamp((int)roundjitter(dotti * palsize * 1.1f), 0, palsize - 1);

		thisct[i] = palette[idx];
	}

//	thisct[0] = 0;




	int mapoffs = 0;

	trytabmakingagain:;

	for (i = 0; i < 256; i++)
	{
		int ltpos = i + mapoffs;

		if (colcombs[i] > 0)
		{
			if (coltab_used[ltpos])
			{
				mapoffs++;
				goto trytabmakingagain;
			}
		}
	}

	coltab_offsets[ctnum] = mapoffs;
	printf("ok, putting mapping %i to offs %04X\n", ctnum, mapoffs);

	for (i = 0; i < 256; i++)
	{
		if (colcombs[i] > 0)
		{
			int c1o = (i >> 4) & 0x0F;
			int c2o = (i >> 0) & 0x0F;

			int c1 = thisct[c1o];
			int c2 = thisct[c2o];


			int cval = (c1 << 4) | c2;


			int ltpos = i + mapoffs;

			coltab[ltpos] = cval;

			coltab_used[ltpos] = 1;
			coltab_len = glm::max(coltab_len, ltpos+1);

		}
	}


}

void ekat()
{
	ekat_spritegen();


//	pic.load("c64normalball.png");
//	pic.load("c64normalball-dither.png");
	pic.load("assets/extnor.png");
	pic.tohires();

//	pic.savehiresfile("normalball.hires");

	int i;


	int colcombs[256];
	memset(colcombs, 0, sizeof(int)*256);

	for (i = 0; i < 40*25;i++)
	{
		//B(0x20);
		int cval;
		cval = ((pic.m_col2[i] & 0x0F) << 4) | (pic.m_col1[i] & 0x0f);
		colcombs[cval]++;
	}

	colcombs[0x11]++;

	int combcount = 0;

	for (i = 0; i < 256; i++)
	{
		if (colcombs[i] > 0)
		{
			printf("comb %02X: %i instances\n", i, colcombs[i]);
			combcount++;
		}
	}

	printf("combcount %i\n", combcount);

	for (i = 0; i < 256; i++)
	{
		if (colcombs[i] > 0)
		{
			printf(".");
		}
		else
			printf(" ");

		if ((i & 15) == 15)
			printf("|\n");
	}



	vec3 normals[16];

	for (i = 0; i < 16; i++)
	{
		ivec3 rgbi;
		rgbi.x = pic.m_palette[i * 4 + 0];
		rgbi.y = pic.m_palette[i * 4 + 1];
		rgbi.z = pic.m_palette[i * 4 + 2];

		vec3 n = glm::normalize(vec3(rgbi - ivec3(0x80)));
		printf("nc %i: %02X %02X %02X (%.2f, %.2f, %.2f)\n", i, rgbi.x, rgbi.y, rgbi.z, n.x, n.y, n.z);

		normals[i] = n;
	}

	for (i = 0; i < coltab_num; i++)
	{
		float ang = (float)i/(float)coltab_num;
//		ang *= 360.f;
		ang *= 3.14159f * 2.f;


		glm::mat4 rot_mtx = glm::mat4(1.0);

//		rot_mtx = glm::rotate(rot_mtx, ang * 5.f, glm::normalize(vec3(0.0f, 0.0f, 1.0f)));
//		rot_mtx = glm::rotate(rot_mtx, ang * 2.f + 70.f, glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
//		rot_mtx = glm::rotate(rot_mtx, ang * 2.f, glm::normalize(vec3(1.0f, 0.0f, 0.0f)));
//		rot_mtx = glm::rotate(rot_mtx, ang * 1.f + 30.f, glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
//		rot_mtx = glm::rotate(rot_mtx, ang * 1.f + 130.f, glm::normalize(vec3(1.0f, 0.0f, 0.0f)));

		rot_mtx = glm::rotate(rot_mtx, sinf(ang) * 90.f, glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
		rot_mtx = glm::rotate(rot_mtx, ang * 2.f, glm::normalize(vec3(1.0f, 0.0f, 0.0f)));

		vec3 lightdir = vec3(0.0f, 0.0f, -1.f);
		lightdir = glm::mat3(rot_mtx) * lightdir;


		lightdir = glm::normalize(lightdir);
		putct(i, lightdir, normals, colcombs);
	}

	for (i = 0; i < 40*25; i++)
	{
		//B(0x20);
		int cval;
		cval = ((pic.m_col2[i] & 0x0F) << 4) | (pic.m_col1[i] & 0x0f);
		piccolors[i] = cval;
	}
}

int main()
{
	srand(time(NULL));
	ekat();
	assemble(genis);

	list<segment_c *> segs;
	segs.push_back(&actualprg);
	make_prg("normalball.prg", 0x2000, segs);
}