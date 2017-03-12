#include "hires.h"

#include "debugf.h"
#include "hashembler.h"


using namespace hashembler;

hiresdata_t pic;


int main()
{
segment_asm_c hireskuva;

	pic.load("assets/ducekallo.png");
	pic.set_monochrome(1, 0);
	pic.findchars();


		hireskuva.begin(0x2000);
SEG->setpc(0x2000);

int x,y,i;

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


	list<segment_c *> segs;
	segs.push_back(&hireskuva);
	make_prg("assets/kallo/kallo.bin", 0x2000, segs);
}