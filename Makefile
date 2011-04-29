
src/flashsdl.swf: libSDL.swc
	mxmlc.exe -library-path+=libSDL.swc --target-player=10.0.0 src/flashsdl.as

libSDL.swc: libSDL.a
	gcc flashSDL.c -dirty -DFLASH -Isdl/include -Lsdl -lSDL -swc -O3 -o libSDL.swc

libSDL.a:
	cd sdl; make -f Makefile.flash clean all; cd ..

clean:
	cd sdl; make -f Makefile.flash clean; cd ..
	rm src/flashsdl.swf
	rm libSDL.swc
	rm sdl/SDL.l.bc
