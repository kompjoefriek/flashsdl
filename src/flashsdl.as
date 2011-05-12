package
{
	import flash.display.Sprite;
	import flash.display.StageScaleMode;

	import sdl.LibSDL;
	import sdl.video.VideoSurface;
	
	import utils.FPSCounter;
	import utils.LogWindow;
	
	[SWF(width=800,height=600,frameRate=50,backgroundColor="0x000000")]
	public class flashsdl extends Sprite
	{	
		// Variables
		private var surface:VideoSurface;	
		private var libSDL:LibSDL;
		
		private var logger:LogWindow;

		public function flashsdl()
		{			
			stage.scaleMode = StageScaleMode.NO_SCALE;

			// The logger
			logger = new LogWindow(500,120);
			logger.textColor = 0xCCCCCC;
			logger.y += 20;
			addChild(logger);

			doLog("Initializing SDL...");

			// C Library setup
			this.libSDL = new LibSDL();
			libSDL.setLog( doLog );

			this.surface = libSDL.getSurface( 800, 600 );
			addChildAt(surface,numChildren-1);
			libSDL.setEventTarget( surface );

			// Monitor FPS performance
			var myFPS:FPSCounter = new FPSCounter();
			addChildAt(myFPS,numChildren-1);

			doLog("Done!");
		}
		
		public function doLog(txt:String):void
		{
			logger.write(txt);			
		}
	}
}
