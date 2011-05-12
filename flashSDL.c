#include <stdio.h>

#include "SDL.h"
#include "AS3.h"

#include <math.h>

// Predefinitions
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void LoadResource( AS3_Val pClass, const char* fileName, SDL_Surface** pSurface );
void mixnewsamples( int numSamples );
void mixaudio(void *unused, Uint8 *stream, int len);
void PlaySound(char *file);

AS3_Val setup(void *data, AS3_Val args);
AS3_Val quitApplication();
AS3_Val tick(void *data, AS3_Val args);
AS3_Val FLASH_getDisplayPointer();
AS3_Val FLASH_setEventManager(void *data, AS3_Val args);
AS3_Val Flash_paintSound( void *data, AS3_Val args );

extern AS3_Val FLASH_EVENT_MANAGER_OBJECT;

// Tmp global vars
SDL_Surface *TMPFLASH_screen;
int			TMPFLASH_quit = 0;
SDL_Event	TMPFLASH_event;
int			TMPFLASH_x, TMPFLASH_y;
Uint32		TMPFLASH_yellow;
Uint32		TMPFLASH_red;
Uint32		TMPFLASH_black;
AS3_Val		FLASH_LibSDL;

SDL_Surface* TMPFLASH_image = NULL;
SDL_Rect	imgRect;

int			TMPFLASH_dir_h = 0; // 0 is left, 1 is right
int			TMPFLASH_dir_v = 0; // 0 is up, 1 is down

#define NUM_SOUNDS 4
#define MIN_SAMPLES 2048
#define MAX_SAMPLES 8192
struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} sounds[NUM_SOUNDS];

SDL_AudioSpec fmt;
Uint32 bufferedSamples = 0;
Uint8* SDLSoundBuffer;
float* FlashSoundBuffer;

int BYTES_PER_SAMPLE = 1; // 1 byte/sample = 8bit sound

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
	AS3_Val paintSoundMethod = AS3_Function(NULL, Flash_paintSound);
	
	AS3_Val libSDL = AS3_Object(
		"setup:AS3ValType, tick:AS3ValType, getDisplayPointer:AS3ValType, quit:AS3ValType, setEventManager:AS3ValType, paintSound:AS3ValType", 
		setupMethod, tickMethod, getDisplayPointerMethod, quitApplicationMethod, setEventManagerMethod, paintSoundMethod
	);

	AS3_Release( paintSoundMethod );
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
AS3_Val tick(void *data, AS3_Val args)
{
	int i_counter=10;
	int requestedSampleSize;
	AS3_ArrayValue( args, "IntType", &requestedSampleSize );

	mixnewsamples( requestedSampleSize );

	// Poll for events
	while( SDL_PollEvent( &TMPFLASH_event ) )
	{
		switch( TMPFLASH_event.type )
		{
			case SDL_MOUSEMOTION:
				fprintf(stderr,"Mouse moved by %d,%d to (%d,%d)", 
                       TMPFLASH_event.motion.xrel, TMPFLASH_event.motion.yrel,
                       TMPFLASH_event.motion.x, TMPFLASH_event.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                fprintf(stderr,"Mouse button %d pressed at (%d,%d)",
                       TMPFLASH_event.button.button, TMPFLASH_event.button.x, TMPFLASH_event.button.y);
			case SDL_KEYUP:
				if(TMPFLASH_event.key.keysym.sym == SDLK_ESCAPE)
					TMPFLASH_quit = 1;
				if(TMPFLASH_event.key.keysym.sym == SDLK_F1)
					SDL_WM_ToggleFullScreen(TMPFLASH_screen); // Only on X11
				if(TMPFLASH_event.key.keysym.sym == SDLK_1)
					PlaySound("test2.wav");
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
			fprintf(stderr, "Can't lock screen: %s", SDL_GetError());
			return AS3_Int(-3);
		}
	}

	if (TMPFLASH_image)
	{
		SDL_FillRect( TMPFLASH_screen, &imgRect, TMPFLASH_black );

		if (TMPFLASH_dir_h == 0)
		{
			imgRect.x -= 5;
		}
		else
		{
			imgRect.x += 5;
		}
		if (TMPFLASH_dir_v == 0)
		{
			imgRect.y -= 5;
		}
		else
		{
			imgRect.y += 5;
		}

		if (imgRect.x > (TMPFLASH_screen->w-TMPFLASH_image->w))
		{
			imgRect.x = (TMPFLASH_screen->w-TMPFLASH_image->w) + ((TMPFLASH_screen->w-TMPFLASH_image->w) - imgRect.x);
			TMPFLASH_dir_h = 0;
			PlaySound("test.wav");
		}
		else if (imgRect.x < 0)
		{
			imgRect.x *= -1;
			TMPFLASH_dir_h = 1;
			PlaySound("test.wav");
		}
		if (imgRect.y > (TMPFLASH_screen->h-TMPFLASH_image->h))
		{
			imgRect.y = (TMPFLASH_screen->h-TMPFLASH_image->h) + ((TMPFLASH_screen->h-TMPFLASH_image->h) - imgRect.y);
			TMPFLASH_dir_v = 0;
			PlaySound("test.wav");
		}
		else if (imgRect.y < 0)
		{
			imgRect.y *= -1;
			TMPFLASH_dir_v = 1;
			PlaySound("test.wav");
		}
		SDL_BlitSurface( TMPFLASH_image, 0, TMPFLASH_screen, &imgRect);
	}
	else
	{
		LoadResource( FLASH_LibSDL, "test.bmp", &TMPFLASH_image );
		imgRect.x = 10;
		imgRect.y = TMPFLASH_screen->h-(TMPFLASH_image->h+10);
		imgRect.w = TMPFLASH_image->w;
		imgRect.h = TMPFLASH_image->h;
	}

	// Unlock Surface if necessary
	if ( SDL_MUSTLOCK(TMPFLASH_screen) )
	{
		SDL_UnlockSurface(TMPFLASH_screen);
	}

	// Update just the part of the display that we've changed
	SDL_UpdateRect(TMPFLASH_screen, 0, 0, TMPFLASH_screen->w, TMPFLASH_screen->h);

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
	char szDriverName[100] = "\0";
	int idxSound;

	// Grab the provided buffer width and height
	AS3_Val bufWidth, bufHeight;
	AS3_Val tmpData;
	AS3_ArrayValue( args, "AS3ValType, AS3ValType, AS3ValType", &tmpData, &bufWidth, &bufHeight );

	// Pointer to ActionScript LibSDL class
	FLASH_LibSDL = tmpData;

	// Initialize defaults, Video and Audio
	if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1))
	{
		printf("Could not initialize SDL: %s.", SDL_GetError());
		return AS3_Int(-1);
	}

	TMPFLASH_screen = SDL_SetVideoMode(AS3_IntValue(bufWidth), AS3_IntValue(bufHeight), 32, SDL_SWSURFACE | SDL_FULLSCREEN);
	if ( TMPFLASH_screen == NULL )
	{
		fprintf(stderr, "Couldn't set video mode: %s", SDL_GetError());
		return AS3_Int(-2);
	}

	SDL_ShowCursor(SDL_DISABLE); // Disable SDL cursor

	// Map the color yellow to this display (R=0xff, G=0xFF, B=0x00)
	TMPFLASH_yellow = SDL_MapRGB(TMPFLASH_screen->format, 0xff, 0xff, 0x00);
	TMPFLASH_red = SDL_MapRGB(TMPFLASH_screen->format, 0xff, 0x00, 0x00);
	TMPFLASH_black = SDL_MapRGB(TMPFLASH_screen->format, 0x00, 0x00, 0x00);

	// Make the dot at the center of the screen
	TMPFLASH_x = TMPFLASH_screen->w / 2;
	TMPFLASH_y = TMPFLASH_screen->h / 2;

	SDL_AudioDriverName( szDriverName, 100 );
	fprintf(stderr, "Driver: %s.", szDriverName);

	// Reset sound buffers
    for ( idxSound=0; idxSound<NUM_SOUNDS; ++idxSound )
	{
        sounds[idxSound].data = NULL;
	}

	/* Specify what kind of audio to output */
	fmt.freq = 22050;
	fmt.format = AUDIO_S8;
	fmt.channels = 2;
	fmt.samples = MIN_SAMPLES;        /* A good value for games = 512 */
	fmt.callback = mixaudio;
	fmt.userdata = NULL;

	if (fmt.format != AUDIO_U8 && fmt.format != AUDIO_S8)
	{
		// Must be 16bit
		//BYTES_PER_SAMPLE = 2;
	}

	SDLSoundBuffer = malloc( sizeof(Uint8) * MAX_SAMPLES * BYTES_PER_SAMPLE * fmt.channels );
	FlashSoundBuffer = malloc( sizeof(float) * MAX_SAMPLES * fmt.channels );

	/* Open the audio device and start playing sound! */
	if ( SDL_OpenAudio(&fmt, NULL) < 0 )
	{
		fprintf(stderr, "Unable to open audio: %s.", SDL_GetError());
		AS3_Int(-1);
	}

	AS3_CallS("LoadResource", FLASH_LibSDL, AS3_Array("StrType", "test.bmp"));
	AS3_CallS("LoadResource", FLASH_LibSDL, AS3_Array("StrType", "test.wav"));
	AS3_CallS("LoadResource", FLASH_LibSDL, AS3_Array("StrType", "test2.wav"));

	SDL_PauseAudio(0);

	return AS3_Int(0);
}


AS3_Val quitApplication()
{
	int idxSound;

	SDL_CloseAudio();

	// Clear sound buffers
    for ( idxSound=0; idxSound<NUM_SOUNDS; ++idxSound )
	{
        if (sounds[idxSound].data)
		{
			free( sounds[idxSound].data );
			sounds[idxSound].dpos = 0;
			sounds[idxSound].dlen = 0;
		}
	}
	free( FlashSoundBuffer );
	free( SDLSoundBuffer );

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

// Wait until flash has loaded the resource
void LoadResource( AS3_Val pClass, const char* fileName, SDL_Surface** pSurface )
{
	int i_resourceLoaded=0;
	AS3_Val resourceLoading = AS3_CallS("isResourceLoaded", pClass, AS3_Array("StrType", fileName));
	i_resourceLoaded = AS3_IntValue(resourceLoading);
	if (i_resourceLoaded != 0)
	{
		*pSurface = SDL_LoadBMP(fileName);
		fprintf(stderr, "SDL_LoadBMP(%s)", fileName);
	}
}

AS3_Val Flash_paintSound( void *data, AS3_Val args )
{
	// Unpack
	AS3_Val soundStream;
	AS3_ArrayValue( args, "AS3ValType", &soundStream );

	int idxSample = 0;
	int idxByte = 0;
	Uint32 iTemp;

	// Flush completely
	if ( bufferedSamples < MIN_SAMPLES )
	{
		// Get up to the threshold
		//fprintf(stderr, "Flash_paintSound is filling up %d 'samples'", (MIN_SAMPLES-bufferedSamples) );
		mixaudio( 0, &SDLSoundBuffer[bufferedSamples * BYTES_PER_SAMPLE * fmt.channels], (MIN_SAMPLES-bufferedSamples) * BYTES_PER_SAMPLE * fmt.channels );
		bufferedSamples = MIN_SAMPLES;
	}
	else if (bufferedSamples > MAX_SAMPLES)
	{
		bufferedSamples = MAX_SAMPLES;
	}

	//fprintf(stderr, "Flash_paintSound is pushing %d 'samples'", bufferedSamples );

	// Zero buffer
	memset(FlashSoundBuffer, 0, sizeof(float) * MAX_SAMPLES * fmt.channels);
	for (idxSample=0; idxSample<bufferedSamples*fmt.channels; idxSample++)
	{
		// Attempt to convert multi-byte samples to floats
		//iTemp = 0;
		//for (idxByte=0; idxByte<BYTES_PER_SAMPLE; idxByte++)
		//{
		//	iTemp |= SDLSoundBuffer[idxSample*BYTES_PER_SAMPLE*fmt.channels+idxByte] << (((BYTES_PER_SAMPLE-1)-idxByte)*8);
		//}
		//FlashSoundBuffer[idxSample] = iTemp/(float)(1<<((BYTES_PER_SAMPLE*8)-1));
		FlashSoundBuffer[idxSample] = SDLSoundBuffer[idxSample*fmt.channels]/128.0f; // hard coded signed 8bit = 2^(8-1)
	}
	AS3_ByteArray_writeBytes( soundStream, FlashSoundBuffer, sizeof(float) * bufferedSamples * fmt.channels  ); // Bps * samples * channels
	
	//fprintf(stderr, "Flash_paintSound is pushing %d 'samples', %d bytes", bufferedSamples, sizeof(float) * bufferedSamples * fmt.channels );

	// Zero buffer
	memset(SDLSoundBuffer, 0, sizeof(Uint8) * bufferedSamples * BYTES_PER_SAMPLE * fmt.channels);

	bufferedSamples = 0;
/*
	// Debug code: outputs a sine wave to test data throughput
	if ( bufferedSamples < MIN_SAMPLES )
	{
		bufferedSamples = MIN_SAMPLES;
	}
	else if (bufferedSamples > MAX_SAMPLES)
	{
		bufferedSamples = MAX_SAMPLES;
	}

	static float sineposition = 0.0f;
	memset(FlashSoundBuffer, 0, sizeof(float) * MAX_SAMPLES * fmt.channels);
	for (idxSample=0; idxSample<bufferedSamples; idxSample++)
	{
		FlashSoundBuffer[idxSample] = sin(sineposition);
		sineposition += 0.05;
	}
	AS3_ByteArray_writeBytes( soundStream, FlashSoundBuffer, sizeof(float) * bufferedSamples  );
	bufferedSamples = 0;
*/
	return AS3_Int(0);
}

void mixnewsamples( int numSamples )
{ 
	if ( bufferedSamples + numSamples <= MAX_SAMPLES )
	{
		mixaudio( 0, &SDLSoundBuffer[bufferedSamples * BYTES_PER_SAMPLE * fmt.channels], numSamples * BYTES_PER_SAMPLE * fmt.channels );
		bufferedSamples += numSamples;
	}
}

void mixaudio(void *unused, Uint8 *stream, int len)
{
    int i;
    Uint32 amount;

	//fprintf(stderr, "void mixaudio(void *unused=%d, Uint8 *stream=%d, int len=%d)\n", unused, stream, len );

	for ( i=0; i<NUM_SOUNDS; ++i )
	{
        amount = (sounds[i].dlen-sounds[i].dpos);
		if (amount > 0)
		{
			if ( amount > len )
			{
				amount = len;
			}
			SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
			sounds[i].dpos += amount;
			//fprintf(stderr, "Sample %d now at %d of %d (pushed %d)", i, sounds[i].dpos, sounds[i].dlen, amount );
		}
    }
}

void PlaySound(char *file)
{
    int index;
    SDL_AudioSpec wave;
    Uint8 *data;
    Uint32 dlen;
    SDL_AudioCVT cvt;

	/* Look for an empty (or finished) sound slot */
    for ( index=0; index<NUM_SOUNDS; ++index )
	{
        if ( sounds[index].dpos == sounds[index].dlen )
		{
            break;
        }
    }
    if ( index == NUM_SOUNDS )
	{
		fprintf(stderr, "NUM_SOUNDS HIT!");
		return;
	}

	fprintf(stderr, "void PlaySound(char *file=%s) index=%d", file, index);

    /* Load the sound file and convert it to 8-bit mono at 22kHz */
	if ( SDL_LoadWAV(file, &wave, &data, &dlen) == NULL )
	{
		fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
		return;
	}

    if ( sounds[index].data )
	{
        free(sounds[index].data);
    }
	
	SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
                            fmt.format,  fmt.channels,  fmt.freq);

    cvt.buf = malloc(dlen*cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(data);

    SDL_LockAudio();
    sounds[index].data = cvt.buf;
    sounds[index].dlen = cvt.len_cvt;
    sounds[index].dpos = 0;
    SDL_UnlockAudio();
}
