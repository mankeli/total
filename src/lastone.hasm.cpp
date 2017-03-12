#include "glm.h"
#include <hashembler.h>

#include "memconfig.h"

#include <math.h>
#include "lib.h"

using namespace hashembler;

segment_asm_c startvec;
segment_asm_c actualprg;

void effy()
{
	
}

void genis()
{
	CTX("valo")

	startvec.begin(if_startvec);

	LPC("begin");
	PRINTPOS("begin");
	JMP(L("actualbeg"))

	actualprg.begin(0x2000);

	LPC("MULQ")
	JMP(L("irqrut"))

LPC("actualbeg")

	MOV8i(0xd011, 0x00);

	MOV8i(0xd018, 0x14);
	MOV8i(0xdd02, 0x3C);

	MOV8i(0xd015, 0);

	MOV8i(0xd020, 0);
	MOV8i(0xd021, 0);

	LDAi(0x20);
	LDXi(0);
	LPC("merktestlop")
	STAx(0x400)
	STAx(0x500)
	STAx(0x600)
	STAx(0x700)
	DEX();
	BNE(L("merktestlop"))


	MOV8i(0xd011, 0x14);

	JSR(L("inittxt"))


		LDAi(0x7);
		LDXi(0);
		LPC("colmemclear")
		STAx(0xD800)
		STAx(0xD900)
		STAx(0xDA00)
		STAx(0xDB00)
		INX();
		BNE(L("colmemclear"))



	setirq(L("irqrut"), 2);

LPC("ikiloop")
	effy();
	JMP(L("ikiloop"))

		LDAi(0xf6);
		LPC("terv")
		CMP(0xd012);
		BNE(L("terv"))

		LDXi(11);
		LPC("terv2")
		DEX();
		BNE(L("terv2"))

		LDAi(0xe);
		STA(0xd020);

		LDAi(0x20);
		LPC("terv3")
		CMP(0xd012);
		BNE(L("terv3"))

		LDAi(0x6);
		STA(0xd020);

	JMP(L("ikiloop"))


	value_t txtptr = 0x10;
	value_t scrptr = 0x12;
	value_t scrptr2 = 0x14;

	value_t kuolema = 0x16;

	value_t vanhad011 = 0x17;
	value_t varad011 = 0x18;

	value_t disabledtime = 0x19;
	value_t stop = 0x1a;

	value_t spd = 0x1b;

	value_t scrlspd = 0x1c;

LPC("inittxt")
	MOV16zi(txtptr, L("teksti"));

	MOV16zi(scrptr, 0x400+40+6);
	MOV16zi(scrptr2, 0x400+40+40+6);

	MOV8zi(disabledtime, 0);

	MOV8zi(spd, 1);

	MOV8zi(stop, 0);

	MOV8zi(scrlspd, 7);

	RTS();


LPC("stopwriter")
	MOV8i(scrlspd, 0);

#if 1
	MOV16z(scrptr, scrptr2);
	SUB16zi(scrptr, 40*13-19);

	LDAz(scrptr)
	STA(L("stopwriter_mod")+1)
	LDAz(scrptr+1)
	STA(L("stopwriter_mod")+2)
#endif

LPC("stopwriter_mod")
	INC(0x644-40)
	JMP(L("skipwriter"))




LPC("irqrut")

	LDAz(stop)
	BNE(L("stopwriter"))

	DECz(disabledtime)
	BPL(L("skipwriter"))

	//INCz(disabledtime)
	LDAz(spd)
	STAz(disabledtime)

	DECz(spd)
	BPL(L("dontlowerspeed"))
	INCz(spd)
	LPC("dontlowerspeed")


	LDYi(0);
	LDAizy(txtptr)
	CMPi(0xFF)
	BEQ(L("rowchange"))
	CMPi(0xFE)
	BEQ(L("stopnow"))

	LPC("normalchar")
	STAizy(scrptr)
	ADD16zi(scrptr, 1);
	JMP(L("end"))

	LPC("rowchange")
	MOV16z(scrptr, scrptr2);
	ADD16zi(scrptr2, 40);

	SEI();
	LDXz(scrptr+1)
	LDAz(scrptr+0);
	CLI();
	SEC();
	SBCi(LB(0x400+24*40+39))
	TXA();
	SBCi(HB(0x400+24*40+39))
	BCS(L("disablefortime"))

	JMP(L("end"))

LPC("stopnow")
	MOV8i(stop, 1);
	JMP(L("end"))

LPC("disablefortime")
	MOV8i(disabledtime, 60);
	MOV8i(spd, 20);
	//JMP(L("end"))


LPC("end")

	ADD16zi(txtptr, 1);

	JMP(L("skipwriter"))


LPC("skipwriter")

	LDAz(kuolema)
	SEC();
	SBCz(scrlspd)
	STAz(kuolema)
	LSRa();
	LSRa();
	LSRa();
	LSRa();
	LSRa();
	ORAi(0x10);
	STA(0xd011)

	STAz(varad011)
	CMPi(0x17);
	BEQ(L("asd"))

LPC("paluuirkista")
	LDAz(varad011)
	STAz(vanhad011)



	RTS();

LPC("asd")
	LDAz(vanhad011)
	CMPi(0x10);
	BEQ(L("scroll"))

	JMP(L("paluuirkista"))

LPC("scroll")
	LDXi(26)
	STXz(varad011) // HACK: pistää ettei d011 compare triggaa
LPC("pollo")
	int i;
	for (i = 1; i < 25; i++)
	{
		LDAx(0x400+40*i+5)
		STAx(0x400+40*i-40+5)
	}

	LDAi(0x20);
	STAx(0x400+40*24+5)

	DEX();
	BMI(L("polloend"))
	JMP(L("pollo"))

LPC("polloend")

	SUB16zi(scrptr, 40);
	SUB16zi(scrptr2, 40);

	JMP(L("paluuirkista"));

LPC("teksti")
	SEG->add_string(R"(









TOTAL * BY EXTEND

RELEASED AT X'17

---

C: BARFLY, MANKELI
G: DUCE, ELECTRIC
M: BARRACUDA
P: TBB
L: MANKELI

---

We salute:

64mula
8 Bits High
Accession
Artline
Beyond Force
Booze
Byterapers
Camelot
Censor
Contex
Crest
Cyberpunx
Da Jormas
Damones
Dekadence
Dual Crew
Fairlight
FIT
Genesis
Hack'n'Trade
Hackers
iSO
Jumalauta
Kryo
MON
Neu Basic
NoName
Offence
Origo
Oxyron
Plush
Side B
Singular
Svenonacid
Topaz
Triad
ZOO
Yleisradio
X


See you at ZOO 2017














EXTEND - DEMOS FOR * NEEDS











!)", [](char byte)
		{
			char bbb;

			if((byte >= 'a') && (byte <= 'z'))
					bbb = (char)(byte - 97+1);      // shift uppercase down
			else if((byte >= 'A') && (byte <= 'Z'))
					bbb = (char)(byte - 65+1);      // shift uppercase down
			else if((byte >= '[') && (byte <= '_'))
					bbb = (char)(byte - 64);      // shift [\]^_ down
			else if(byte == '`')
					bbb = (char)64;     // shift ` down
			else if(byte == '@')
					bbb = 0;      // shift @ down
			else if (byte == 0xa)
					bbb = 0xFF;
			else if (byte == '*')
					bbb = 0x2A;
			else if (byte == '!')
					bbb = 0xFE;
			else if (byte == '-')
					bbb = 0x2D;
			else
				bbb = byte;

			return(bbb);
		});
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
	make_mprg("lastone.mprg", segs);
}