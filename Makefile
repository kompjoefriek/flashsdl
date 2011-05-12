
src/flashsdl.swf: libSDL.swc
	mxmlc.exe -library-path+=libSDL.swc --target-player=10.0.0 src/flashsdl.as

libSDL.swc: $(ALCHEMY_HOME)/usr/local/lib/SDL.l.bc
	gcc flashSDL.c -dirty -DFLASH -Isdl/include -Lsdl -lSDL -swc -O3 -o libSDL.swc

$(ALCHEMY_HOME)/usr/local/lib/SDL.l.bc: sdl/SDL.l.bc
	cp sdl/SDL.l.bc $(ALCHEMY_HOME)/usr/local/lib/SDL.l.bc

sdl/SDL.l.bc:
	cd sdl; make -f Makefile.flash all; cd ..

clean:
	cd sdl; make -f Makefile.flash clean; cd ..
	rm -f src/flashsdl.swf
	rm -f libSDL.swc
