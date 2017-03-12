HASMBIN=~/src/hashembler/hasm.sh
#X64SCBIN=/Applications/vice/x64sc.app/Contents/MacOS/x64sc
X64SCBIN=x64sc
#X64BIN=/Applications/vice/x64.app/Contents/MacOS/x64
X64BIN=x64
SPINBIN=~/src/spindle-2.3/spindle/spin
64TASSBIN=~/src/64tass/64tass


.PHONY: all
all: next.d64
	@true

obj/%_h.prg: src/%.hasm.cpp src/memconfig.h
	$(HASMBIN) $<
	mv $*.prg $@

obj/%_h.mprg: src/%.hasm.cpp src/memconfig.h
	$(HASMBIN) $<
	mv $*.mprg $@

obj/%_t.mprg: src/%.asm
	$(64TASSBIN) -n -o $@ $< -Ltass.lst


normalballsrc=src/normalball.hasm.cpp src/lode/lodepng.cpp
obj/normalball_h.prg: $(normalballsrc) assets/extnor.png assets/extnor_sprites.png src/hires.h src/memconfig.h
	$(HASMBIN) $(normalballsrc)
	mv normalball.prg $@

burnersrc=src/burner.hasm.cpp src/lode/lodepng.cpp
obj/burner_h.mprg: $(burnersrc) assets/burner.png src/hires.h src/memconfig.h
	$(HASMBIN) $(burnersrc)
	mv burner.mprg $@

orangisrc=src/orangi.hasm.cpp src/lode/lodepng.cpp
obj/orangi_h.mprg: $(orangisrc) assets/maisema.png src/hires.h src/memconfig.h
	$(HASMBIN) $(orangisrc)
	mv orangi.mprg $@

logosrc=src/logo.hasm.cpp src/lode/lodepng.cpp
obj/logo_h.mprg: $(logosrc) assets/logo/total-0005.png src/hires.h src/memconfig.h
	$(HASMBIN) $(logosrc)
	mv logo.mprg $@

boot666src=src/boot666.hasm.cpp src/lode/lodepng.cpp
obj/boot666_h.mprg: $(boot666src) src/hires.h src/memconfig.h
	$(HASMBIN) $(boot666src)
	mv boot666.mprg $@


BANKS = \
	obj/irqfukker_h.prg \
	obj/normalball_h.prg \
	obj/tasstest_t.mprg \
	obj/extfall_t.mprg \
	obj/kallo_t.mprg \
	obj/rad_h.mprg \
	obj/burner_h.mprg \
	obj/filterplotter_h.mprg \
	obj/orangi_h.mprg \
	obj/rasterit_t.mprg \
	obj/cylinder_t.mprg \
	obj/duce_h.mprg \
	obj/logo_h.mprg \
	obj/boot666_h.mprg \
	obj/annabel_t.mprg \
	obj/lastone_h.mprg \
	assets/song.prg

DIRART=assets/dirart.txt
next.d64: $(BANKS) script.spindle $(DIRART)
	$(SPINBIN) -vv -o next.d64 -t "TOTAL" --my-magic 0xDEAD32 --next-magic 0xDEAD32 script.spindle

.PHONY: run
run: next.d64
	$(X64BIN) -truedrive $<

.PHONY: runsilent
runsilent: next.d64
	$(X64BIN) -truedrive $< +sound

.PHONY: runsc
runsc: next.d64
	$(X64SCBIN) -truedrive $<

.PHONY: clean
clean:
	rm obj/*.prg obj/*.mprg obj/*.hires next.d64
