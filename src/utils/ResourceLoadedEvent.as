package utils
{	
	import flash.display.DisplayObject;
	import flash.display.PixelSnapping;
	import flash.utils.ByteArray;
	
	import flash.events.*;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Dictionary;

	public class ResourceLoadedEvent extends Event
	{
        public static const RESOURCE_LOADED:String = "onLoaded";
        public static const RESOURCE_ERROR:String = "onError";

		public var params:Object;
		public var resourcename:String;
		public var text:String;
		public var data:ByteArray;

        public function ResourceLoadedEvent($type:String, $params:Object, $bubbles:Boolean = false, $cancelable:Boolean = false)
        {
            super($type, $bubbles, $cancelable);           
            this.params = $params;
			this.resourcename = "";
			this.text = "Undefined";
			this.data = new ByteArray();
		}

        public override function clone():Event
        {
            var r:ResourceLoadedEvent = new ResourceLoadedEvent(type, this.params, bubbles, cancelable);
			r.resourcename = this.resourcename;
			r.text = this.text;
			r.data = this.data;
			return r;
        }
       
        public override function toString():String
        {
            return formatToString("CustomEvent", "params", "type", "bubbles", "cancelable", "resourcename", "text", "data");
        }
	}
}
