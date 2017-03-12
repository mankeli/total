#pragma once
#include <hashembler.h>

namespace hashembler
{

static void MOV8i(value_t outaddr, value_t inval)
{
	LDAi(inval);
	STA(outaddr);
}

static void MOV8zi(value_t outaddr, value_t inval)
{
	LDAi(inval);
	STAz(outaddr);
}

static void MOV16(value_t outaddr, value_t inaddr)
{
	LDA(inaddr + 0);
	STA(outaddr + 0);
	LDA(inaddr + 1);
	STA(outaddr + 1);
}

static void MOV16z(value_t outaddr, value_t inaddr)
{
	LDAz(inaddr + 0);
	STAz(outaddr + 0);
	LDAz(inaddr + 1);
	STAz(outaddr + 1);
}
static void MOV16i(value_t outaddr, value_t inval)
{
	LDAi(LB(inval));
	STA(outaddr + 0);
	LDAi(HB(inval));
	STA(outaddr + 1);
}

static void MOV16zi(value_t outaddr, value_t inval)
{
	LDAi(LB(inval));
	STAz(outaddr + 0);
	LDAi(HB(inval));
	STAz(outaddr + 1);
}


static void ADD16zi(value_t addr, value_t con)
{
	LDAz(addr + 0);
	CLC();
	ADCi(LB(con));
	STAz(addr + 0);
	LDAz(addr + 1);
	ADCi(HB(con));
	STAz(addr + 1);
}

static void ADD16z(value_t addr, value_t addr2)
{
	LDAz(addr  + 0);
	CLC();
	ADCz(addr2 + 0);
	STAz(addr  + 0);

	LDAz(addr  + 1);
	ADCz(addr2 + 1);
	STAz(addr  + 1);
}

static void ADD16z(value_t outaddr, value_t addr, value_t addr2)
{
	LDAz(addr  + 0);
	CLC();
	ADCz(addr2 + 0);
	STAz(outaddr  + 0);

	LDAz(addr  + 1);
	ADCz(addr2 + 1);
	STAz(outaddr  + 1);
}


static void ADD16i(value_t addr, value_t con)
{
	LDA(addr + 0);
	CLC();
	ADCi(LB(con));
	STA(addr + 0);
	LDA(addr + 1);
	ADCi(HB(con));
	STA(addr + 1);
}

void SUB16z(value_t outaddr, value_t addr1, value_t addr2)
{
	LDA(addr1 + 0);
	SEC();
	SBC(addr2 + 0);
	STA(outaddr + 0);
	LDA(addr1 + 1);
	SBC(addr2 + 1);
	STA(outaddr + 1);
}

static void SUB16zi(value_t addr, value_t con)
{
	LDAz(addr + 0);
	SEC();
	SBCi(LB(con));
	STAz(addr + 0);
	LDAz(addr + 1);
	SBCi(HB(con));
	STAz(addr + 1);
}


void jump_if_not_time(value_t newtime, value_t pos)
{
#if 0
	LDAz(framecounter+0)
	CMPi(LB(newtime))
	BNE(pos)
	LDAz(framecounter+1)
	CMPi(HB(newtime))
	BNE(pos)
#else
#if 0
	LDAz(framecounter+0);
	SEC();
	SBCi(LB(newtime))
	LDAz(framecounter+1)
	SBCi(HB(newtime))
	BCC(pos)
#else
	SEI();
	LDXz(framecounter+1)
	LDAz(framecounter+0);
	CLI();
	SEC();
	SBCi(LB(newtime))
	TXA();
	SBCi(HB(newtime))
	BCC(pos)

#endif
#endif
}

void setirq(value_t pos, int whichirq)
{
	LDXi(LB(pos))
	LDYi(HB(pos))
	if (pos > 0 && whichirq == 1)
		JSR(if_setirq1)
	else if (pos > 0 && whichirq == 2)
		JSR(if_setirq2)
	else if (whichirq == 1)
		JSR(if_unsetirq1)
	else if (whichirq == 2)
		JSR(if_unsetirq2)

}

}