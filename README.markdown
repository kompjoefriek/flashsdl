FlashSDL - Quick Guide
=======================
Port created by Ed McManus


## License ##

This library is distributed under GNU LGPL version 2, the same license shipped with SDL 1.2. This license allows you to use SDL freely in commercial programs as long as you link with the dynamic library.


##General Pointers##

Most bridging logic is contained in the AS3 portion of the project (`/src`). I'd prefer to build the flash driver in C, rather than a mix of C and AS3, so the code can be contributed to the SDL trunk. But for now it's easier to stub in some C methods and write the core control logic in AS3.

Keyboard and Mouse events are supported.

The display buffer is simply mapped to a BitmapData object and updates every time the playhead enters a new frame.

This port does not support sound yet. However, stubs are in place to add that functionality.


##Important Locations##

  - `/flashSDL.c` The C portion of a simple test application.
  - `/Makefile` Make target aliases to ease the build process.
  - `/src/` The AS3 portion of the test application. It should be used as scaffolding for your own work.
  - `/sdl/` The source for SDL.
  - `/sdl/Makefile.flash` Make targets for the Flash port.
  - `/sdl/README.Porting` Helpful information on Porting the library to another platform.
  - `/sdl/src/audio/flash/` The Flash audio driver.
  - `/sdl/src/video/flash/` The Flash video, mouse, and event drivers.


##Build##

####First Steps####

Turn on Alchemy.

`alc-on`

Follow the instructions below. Keep in mind that you may need to specify paths to your Alchemy lib/ and include/ directories when running configure scripts for an application built on SDL.


####Library####

In `./sdl/`:

`make -f Makefile.flash clean all;`

Next, see "Install."


####Test Application#####

Build the library. (See above.)

Build libSDL.swc:

In the repository root (./):

`gcc flashSDL.c -DFLASH -Isdl/include -Lsdl -lSDL -swc -O3 -o libSDL.swc`

If all goes well, you'll see libSDL.swc in the repository root. This SWC is the Alchemy version of a static library, which you can link against in your Flash application. In `./src/` I've included the AS3 side of a test application.

Make sure libSDL.swc is in your classpath. In FlashBuilder you can do this by right clicking the project, selecting "Properties", and "ActionScript Build Path."

Now that you have linked against the SWC, build `/src/flashsdl.as`. The SWF is the finished test application.



##Install##

Right now this is a manual process. After building the library, copy sdl/libSDL.a to your Alchemy /lib directory. Copy all header files in sdl/include to your Alchemy /include file. In my setup, this is ~/alchemy/usr/local/[lib/ or include/].


##Porting your application to use FlashSDL###

Perhaps this is best understood by example. Examine `/flashsdl.c`. Most immediately you will have to refactor your C application's main loop to run iteratively in the tick() method, assuming you end up using the application scaffolding in `./src/`.

Make sure you've properly built and installed FlashSDL by building the test application. Then try running your application's ./configure. You may have to manually alter the build file to successfully compile with Alchemy.

Once you've successfully compiled without any errors, you may have to flesh out the AS3 scaffolding to polish the application.