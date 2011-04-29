package {
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.display.StageScaleMode;
	
	import sdl.LibSDL;
	import sdl.video.VideoSurface;
	
	//import utils.FPSCounter;
	
	[SWF(width=800,height=600,frameRate=60)]
	public class flashsdl extends Sprite {
		
		private var surface:VideoSurface;
		private var surfaceContainer:Sprite;
		
		private var libSDL:LibSDL;
		
		public function flashsdl() {
			
			stage.scaleMode = StageScaleMode.NO_SCALE;

			// C Library setup
			this.libSDL = new LibSDL();
			this.surface = libSDL.getSurface( 800, 600 );
			
			// Build container for input events
			surfaceContainer = new Sprite();
			surfaceContainer.addChild( surface );
			addChild(surfaceContainer);
			
			libSDL.setEventTarget( surfaceContainer );
			
			// Monitor FPS performance
			//var fps:FPSCounter = new FPSCounter();
			//addChild(fps);

			[Embed("test.png")]
			const ImgTest:Class;
			 
			var imgtest:DisplayObject = new ImgTest();
			imgtest.x = 10;
			imgtest.y = 10;
			addChild(imgtest);

			[Embed("circle.png")]
			const ImgCircle:Class;
			 
			var imgcircle:DisplayObject = new ImgCircle();
			imgcircle.x = 800 - (imgcircle.width + 10);
			imgcircle.y = 600 - (imgcircle.height + 10);
			addChild(imgcircle);
		}
		
	}
}
