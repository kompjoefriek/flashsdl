package
{
	import flash.display.Bitmap;
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.display.StageScaleMode;

	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Dictionary;

	import sdl.LibSDL;
	import sdl.video.VideoSurface;
	
	//import utils.FPSCounter;
	import utils.LogWindow;
	
	[SWF(width=800,height=600,frameRate=60,backgroundColor="0x000000")]
	public class flashsdl extends Sprite
	{	
		// Variables
		private var surface:VideoSurface;
		private var surfaceContainer:Sprite;
		
		private var libSDL:LibSDL;
		
		private var logger:LogWindow;

		public function flashsdl()
		{			
			stage.scaleMode = StageScaleMode.NO_SCALE;

			// The logger
			logger = new LogWindow(500,120);
			logger.textColor = 0xCCCCCC;
			addChild(logger);

			logger.write("Initializing SDL...");

			// C Library setup
			this.libSDL = new LibSDL();
			libSDL.setLog( doLog );

			this.surface = libSDL.getSurface( 800, 600 );
			
			// Build container for input events
			surfaceContainer = new Sprite();
			surfaceContainer.addChild( surface );
			addChildAt(surfaceContainer,numChildren-1);
			
			libSDL.setEventTarget( surfaceContainer );

/*
			var imgcircle:DisplayObject = resources["circle.png"];
			imgcircle.x = 800 - (imgcircle.width + 10);
			imgcircle.y = 600 - (imgcircle.height + 10);
			addChildAt(imgcircle,numChildren-1);
*/
			logger.write("Done!");
		}
		
		public function doLog(txt:String):void
		{
			logger.write(txt);			
		}
	}
}
