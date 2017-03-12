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

	actualprg.begin(0x4000);

LPC("actualbeg")

	MOV8i(0xd011, 0x1B);

	MOV8i(0xd018, 0x14);
	MOV8i(0xdd02, 0x3C);

	LDXi(0);
	LPC("merktestlop")
	TXA();
	STAx(0x400)
	STAx(0x500)
	STAx(0x600)
	STAx(0x700)
	DEX();
	BNE(L("merktestlop"))




	setirq(L("irqrut"), 2);

LPC("ikiloop")
	effy();
	JMP(L("ikiloop"))


LPC("irqrut")
	RTS();
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