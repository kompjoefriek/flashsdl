package sdl
{	
	import cmodule.libSDL.CLibInit;
	
	import flash.display.DisplayObject;
	import flash.display.PixelSnapping;
	import flash.utils.ByteArray;
	
	import sdl.events.ListenerManager;
	import sdl.video.VideoSurface;
	
	import utils.LogWindow;
	import utils.ResourceManager;

	import flash.events.*;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Dictionary;

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

		private var counter:int;

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

			resManager = new ResourceManager( cLoader );
			counter = 0;
		}

		public function setLog( logger:Function ):void
		{
			this.doLog = logger;
			doLog("LibSDL: logger set");
			resManager.setLog( logger );
		}
		
		/**
		 * Retrieve a resource as a ByteArray using URLLoader.
		 * 
		 * @param	name of the resource to retrieve.
		 *
		 * @return	A ByteArray containing the resource.
		 */
		public function LoadResource(resname:String):void
		{
			if (counter>1) return;
			doLog("LibSDL.LoadResource( "+resname+" )");
			resManager.LoadResource( resname );

			counter++;
		}

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

		// Note: function stolen from http://code.google.com/p/as3-commons/source/browse/trunk/as3-commons-lang/src/main/actionscript/org/as3commons/lang/DictionaryUtils.as?spec=svn877&r=877
		/**
		 * Check whether the given dictionary contains the given key.
		 *
		 * @param dictionary the dictionary to check for a key
		 * @param key the key to look up in the dictionary
		 * @return <code>true</code> if the dictionary contains the given key, <code>false</code> if not
		 */
		private static function containsKey(dictionary:Dictionary, key:Object):Boolean
		{
			var result:Boolean = false;
			
			for (var k:*in dictionary)
			{
				if (key === k)
				{
						result = true;
						break;
				}
			}
			return result;
		}

	}
}