package sdl
{	
	import cmodule.libSDL.CLibInit;
	
	import sdl.events.ListenerManager;
	import sdl.video.VideoSurface;

	import utils.LogWindow;
	import utils.ResourceManager;

	import flash.display.DisplayObject;
	import flash.display.PixelSnapping;	

	/**
	 * This class contans the public interface for an SDL application.
	 */
	public class LibSDL
	{
		
		/** @private */
		protected var videoSurface:VideoSurface;
		
		/** @private */
		protected var eventManager:ListenerManager;
		
		/** @default 320 */
		protected var SDLWidth:int = 320;
		
		/** @default 240 */
		protected var SDLHeight:int = 240;
		
		/**
		 * @private
		 * 
		 * Loads and initializes the C library.
		 */
		internal var cLoader:CLibInit;
		
		/**
		 * Reference to the initialized C application. Contains available C callbacks.
		 */
		public var cLib:Object;

		private var doLog:Function;
		
		private var resManager:ResourceManager;

		/**
		 * Constructor. Following construction, use getSurface() to build an SDL video surface,
		 * and setEventTarget() to receive user input.
		 * 
		 * <p>If your application requires some special initialization process, add it here.</p>
		 */
		public function LibSDL()
		{
			cLoader = new CLibInit();
			cLoader.putEnv("SDL_VIDEODRIVER", "flash");
			cLib = cLoader.init();

			resManager = new ResourceManager(cLoader);

			this.doLog = dummyLog;
		}

		/**
		 * Debug functionality.
		 * 
		 * @param	function pointer to log function (default set to dummy function)
		 */
		public function setLog( logger:Function ):void
		{
			this.doLog = logger;
			doLog("LibSDL: logger set");

			resManager.setLog( logger );
		}
		
		private function dummyLog(s:String):void
		{
		}

		/**
		 * Request a resource to be loaded
		 * 
		 * @param	name of the resource to retrieve.
		 */
		public function LoadResource(resname:String):void
		{
			doLog("LibSDL.LoadResource( "+resname+" )");
			resManager.LoadResource( resname );
		}

		/**
		 * Ask if the specified resource has been loaded
		 * 
		 * @param	name of the resource.
		 *
		 * @return	true when the specified resource has been loaded, false otherwise
		 */
		public function isResourceLoaded(resname:String):Boolean
		{
			return resManager.isResourceLoaded( resname );
		}

		/**
		 * Initializes an SDL video surface and attaches required event listeners. Returned
		 * bitmap must be added to the display hierarchy.
		 * 
		 * <p>Application-specific code required to initialize the display should be added here.</p>
		 * 
		 * @param	displayWidth	The SDL video surface desired width. [Optional]
		 * @param	displayHeight	The SDL video surface desired height. [Optional]
		 * 
		 * @return	A bitmap mapped to the SDL Video Surface.
		 */
		public function getSurface( width:int=0, height:int=0 ):VideoSurface
		{
			if (!videoSurface)
			{
				
				this.SDLWidth = width;
				this.SDLHeight = height;
				
				cLib.setup( this, width, height );
				videoSurface = new VideoSurface( this, width, height );
			}
			return videoSurface;
		}
		
		/**
		 * Registers the given display object to receive keypress and mouse events. MouseMove events are
		 * recorded relative to this display object.
		 * 
		 * @param	eventTarget	The display object to register for keypress and mouse events.
		 */
		public function setEventTarget( eventTarget:DisplayObject ):void
		{
			if (!eventManager)
			{
				eventManager = new ListenerManager( eventTarget );
				cLib.setEventManager( eventManager );	// pass manager reference to c lib for event retrieval
				
				doLog("LibSDL: EventTarget set");
			}
		}
	}
}
