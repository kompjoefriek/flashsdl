#include <stdio.h>

#include "SDL.h"
#include "AS3.h"

// Predefinitions
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
AS3_Val setup(void *data, AS3_Val args);
AS3_Val quitApplication();
AS3_Val tick();
AS3_Val FLASH_getDisplayPointer();
AS3_Val FLASH_setEventManager(void *data, AS3_Val args);

extern AS3_Val FLASH_EVENT_MANAGER_OBJECT;

// Tmp global vars
SDL_Surface *TMPFLASH_screen;
int			TMPFLASH_quit = 0;
SDL_Event	TMPFLASH_event;
int			TMPFLASH_x, TMPFLASH_y;
Uint32		TMPFLASH_yellow;
Uint32		TMPFLASH_red;
SDL_Surface* TMPFlash_bitmap;
AS3_Val		FLASH_LibSDL;

/*
 * Lib Initialization
 */
int main(int argc, char **argv)
{    
	// Create callbacks
	AS3_Val setupMethod = AS3_Function(NULL, setup);
	AS3_Val tickMethod = AS3_Function(NULL, tick);
	AS3_Val getDisplayPointerMethod = AS3_Function(NULL, FLASH_getDisplayPointer);
	AS3_Val quitApplicationMethod = AS3_Function(NULL, quitApplication);
	AS3_Val setEventManagerMethod = AS3_Function(NULL, FLASH_setEventManager);

	AS3_Val libSDL = AS3_Object(
		"setup:AS3ValType, tick:AS3ValType, getDisplayPointer:AS3ValType, quit:AS3ValType, setEventManager:AS3ValType", 
		setupMethod, tickMethod, getDisplayPointerMethod, quitApplicationMethod, setEventManagerMethod
	);

	AS3_Release( setupMethod );
	AS3_Release( tickMethod );
	AS3_Release( getDisplayPointerMethod );
	AS3_Release( quitApplicationMethod );
	AS3_Release( setEventManagerMethod );

	AS3_LibInit(libSDL);
	return 0;
}

/*
* This method gets called on enterFrame. You should refactor your SDL Application's main()
* loop so that one loop is executed on tick(). Tick's are followed by frame draws.
*/
AS3_Val tick()
{
	int i_counter=10;
	// Poll for events
	while( SDL_PollEvent( &TMPFLASH_event ) )
	{
		switch( TMPFLASH_event.type )
		{
			case SDL_KEYUP:
				if(TMPFLASH_event.key.keysym.sym == SDLK_ESCAPE)
					TMPFLASH_quit = 1;
				if(TMPFLASH_event.key.keysym.sym == SDLK_F1)
					SDL_WM_ToggleFullScreen(TMPFLASH_screen); // Only on X11
				break;
			case SDL_QUIT:
				TMPFLASH_quit = 1;
				break;
			default:
				break;
		}
	}

	// Lock the screen for direct access to the pixels
	if ( SDL_MUSTLOCK(TMPFLASH_screen) )
	{
		if ( SDL_LockSurface(TMPFLASH_screen) < 0 )
		{
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return AS3_Int(-3);
		}
	}

	while(i_counter>0)
	{
		// Plot the Pixel
		putpixel(TMPFLASH_screen, TMPFLASH_x, TMPFLASH_y, (i_counter%2)?TMPFLASH_yellow:TMPFLASH_red);

		switch(rand()%4)
		{
			case 0:
				--TMPFLASH_x;
				if (TMPFLASH_x<0) TMPFLASH_x = 0;
				break;
			case 1:
				++TMPFLASH_x;
				if (TMPFLASH_x>=TMPFLASH_screen->w) TMPFLASH_x = TMPFLASH_screen->w-1;
				break;
			case 2:
				--TMPFLASH_y;
				if (TMPFLASH_y<0) TMPFLASH_y = 0;
				break;
			case 3:
				++TMPFLASH_y;
				if (TMPFLASH_y>=TMPFLASH_screen->h) TMPFLASH_y = TMPFLASH_screen->h-1;
				break;
		}
		i_counter=i_counter-1;
	}

	// Unlock Surface if necessary
	if ( SDL_MUSTLOCK(TMPFLASH_screen) )
	{
		SDL_UnlockSurface(TMPFLASH_screen);
	}

	// Update just the part of the display that we've changed
	SDL_UpdateRect(TMPFLASH_screen, TMPFLASH_x-1, TMPFLASH_y-1, 3, 3);

	if (TMPFLASH_quit)
	{
		quitApplication();
	}

	return AS3_Int(0);
}


/*
 * Set up the application before the main loop. Move/rename variables to exist in the global
 * ns.
 */
AS3_Val setup(void *data, AS3_Val args)
{
	// Grab the provided buffer width and height
	AS3_Val bufWidth, bufHeight;
	AS3_Val tmpData;
	AS3_ArrayValue( args, "AS3ValType, AS3ValType, AS3ValType", &tmpData, &bufWidth, &bufHeight );

	// Pointer to ActionScript LibSDL class
	FLASH_LibSDL = tmpData;

	fprintf(stderr, "C: setup()");

	// Initialize defaults, Video and Audio
	if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1))
	{
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		return AS3_Int(-1);
	}

	TMPFLASH_screen = SDL_SetVideoMode(AS3_IntValue(bufWidth), AS3_IntValue(bufHeight), 32, SDL_SWSURFACE | SDL_FULLSCREEN);
	if ( TMPFLASH_screen == NULL )
	{
		fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
		return AS3_Int(-2);
	}

	SDL_ShowCursor(SDL_DISABLE); // Disable SDL cursor

	// Map the color yellow to this display (R=0xff, G=0xFF, B=0x00)
	TMPFLASH_yellow = SDL_MapRGB(TMPFLASH_screen->format, 0xff, 0xff, 0x00);
	TMPFLASH_red = SDL_MapRGB(TMPFLASH_screen->format, 0xff, 0x00, 0x00);

	// Make the dot at the center of the screen
	TMPFLASH_x = TMPFLASH_screen->w / 2;
	TMPFLASH_y = TMPFLASH_screen->h / 2;

	fprintf(stderr, "C: calling getResource()");
	//TMPFlash_bitmap = SDL_LoadBMP("test.bmp");
/*
	// Typical SDL example
	SDL_Surface * const image = SDL_LoadBMP( "MyImage.bmp");
	//Blit image to screen
	SDL_BlitSurface( image, 0, screen, 0);
*/
	// Call getResource function from FLASH_LibSDL class in actionscript
	tmpData = AS3_CallS("getResource", FLASH_LibSDL, AS3_Array("StringType", "test.bmp"));

	fprintf(stderr, "C: called getResource()");
 
	return AS3_Int(0);
}


AS3_Val quitApplication()
{
	SDL_Quit();
	return AS3_Int(0);
}


AS3_Val FLASH_getDisplayPointer()
{
	return AS3_Ptr(TMPFLASH_screen->pixels);
}


AS3_Val FLASH_setEventManager( void *data, AS3_Val args )
{
	AS3_Val eventManager;
	AS3_ArrayValue( args, "AS3ValType", &eventManager );
	
	FLASH_EVENT_MANAGER_OBJECT = eventManager;
	
	return AS3_Int(0);
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	// Print pixel
	switch(bpp)
	{
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			sztrace("bbp==3");
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}
