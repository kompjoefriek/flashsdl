package
{
	import flash.display.Bitmap;
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.display.StageScaleMode;

	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Dictionary;

	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	import sdl.LibSDL;
	import sdl.video.VideoSurface;
	
	//import utils.FPSCounter;
	import utils.LogWindow;
	
	[SWF(width=800,height=600,frameRate=60,backgroundColor="0x000000")]
	public class flashsdl extends Sprite
	{
		// Resources!
		[Embed(source="test.bmp",mimeType="application/octet-stream")]
		private const ImgTest:Class;
		[Embed(source="circle.png")]
		private const ImgCircle:Class;
		
		// Variables
		private var surface:VideoSurface;
		private var surfaceContainer:Sprite;

		private var resLoader:URLLoader;
		
		private var libSDL:LibSDL;
		
		private var logger:LogWindow;

		private var resources:Dictionary;

		public function flashsdl()
		{			
			stage.scaleMode = StageScaleMode.NO_SCALE;

			// The logger
			logger = new LogWindow(300,100);
			logger.textColor = 0xCCCCCC;
			addChild(logger);

			resources = new Dictionary(true);

			resources["test.bmp"] = new ImgTest() as ByteArray;
			resources["circle.png"] = new ImgCircle() as DisplayObject;

			//resLoader = new URLLoader();
			//resLoader.dataFormat = URLLoaderDataFormat.BINARY;
			//resLoader.addEventListener( Event.COMPLETE, onResLoaded );
			//resLoader.load(new URLRequest( "test.bmp" ));

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


			var imgcircle:DisplayObject = resources["circle.png"];
			imgcircle.x = 800 - (imgcircle.width + 10);
			imgcircle.y = 600 - (imgcircle.height + 10);
			addChildAt(imgcircle,numChildren-1);

			for (var i:Number=1; i<=10;i++)
			{
				logger.write("test "+i);
			}
			logger.write("Done!");
		}
		
		private function onResLoaded(e:Event):void
		{
			//resLoader.data
			logger.write("Loaded resource! "+e);
			//resources[???] = resLoader.data;
		}

		public function doLog(txt:String):void
		{
			logger.write(txt);			
		}
	}
}
