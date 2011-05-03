package utils
{	
	import utils.ResourceLoadedEvent;
	
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.EventDispatcher;

	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	import flash.utils.Dictionary;

	public class ResourceLoader extends EventDispatcher
	{	
		private var resLoader:URLLoader;	
		private var resourcename:String;

		/**
		 * Constructor. Following construction, use getSurface() to build an SDL video surface,
		 * and setEventTarget() to receive user input.
		 */
		public function ResourceLoader()
		{
			resLoader = new URLLoader();
			resLoader.dataFormat = URLLoaderDataFormat.BINARY;
			resLoader.addEventListener( Event.COMPLETE, onResLoaded );
			resLoader.addEventListener(IOErrorEvent.IO_ERROR, onResIOError); //Error handling
		}

		/**
		 * Load a resource as a ByteArray using URLLoader.
		 * 
		 * @param	name of the resource to load.
		 */
		public function LoadResource(resname:String):void
		{
			try
			{
				resourcename = resname;
				resLoader.load(new URLRequest( resname ));
            }
			catch (error:Error)
			{
				var newevt:ResourceLoadedEvent = new ResourceLoadedEvent(ResourceLoadedEvent.RESOURCE_ERROR, null);
				newevt.resourcename = resname;
				newevt.text = "Exception on load: "+error.toString();
				this.dispatchEvent(newevt);
            }
		}

		private function onResLoaded(evt:Event):void
		{
			var newevt:ResourceLoadedEvent = new ResourceLoadedEvent(ResourceLoadedEvent.RESOURCE_LOADED, null);
			newevt.resourcename = resourcename;
			newevt.data = evt.target.data;
			newevt.text = "Resource loaded.";
			this.dispatchEvent(newevt);
		}

		private function onResIOError(evt:IOErrorEvent):void
		{
			var newevt:ResourceLoadedEvent = new ResourceLoadedEvent(ResourceLoadedEvent.RESOURCE_ERROR, null);
			newevt.resourcename = resourcename;
			newevt.text = "Error: "+evt.toString();
			this.dispatchEvent(newevt);
		}
	}
}
