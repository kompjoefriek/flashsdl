package utils
{	
	import cmodule.libSDL.CLibInit;

	import utils.ResourceLoader;
	//import utils.LogWindow;

	import flash.events.*;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	import flash.utils.Dictionary;

	/**
	 * This class contans the public interface for an SDL application.
	 */
	public class ResourceManager
	{
		private var resources:Dictionary;
		private var resourceloader:Dictionary;

		private var doLog:Function;

		private var cLoader:CLibInit;

		public function ResourceManager(cLoad:CLibInit)
		{
			cLoader = cLoad;
			resources = new Dictionary(true);
			resourceloader = new Dictionary(true);
		}

		public function setLog( logger:Function ):void
		{
			this.doLog = logger;
			doLog("ResourceManager: logger set");
		}
		
		public function LoadResource(resname:String):void
		{
			doLog("ResourceManager.LoadResource( "+resname+" )");
			if (isResourceLoaded(resname))
			{
				doLog("ResourceManager: "+resname+" already loaded");
				return;
			}
			if (containsKey(resourceloader,resname))
			{
				doLog("ResourceManager: "+resname+" already being loaded");
				return;
			}
			resourceloader[resname] = new ResourceLoader();
			resourceloader[resname].addEventListener( ResourceLoadedEvent.RESOURCE_LOADED, onResLoaded );
			resourceloader[resname].addEventListener( ResourceLoadedEvent.RESOURCE_ERROR, onResError ); //Error handling

			resourceloader[resname].LoadResource(resname);
		}

		public function isResourceLoaded(resname:String):Boolean
		{
			if (containsKey(resources,resname))
			{
				return resources[resname];
			}
			return false;
		}

		private function onResLoaded(evt:ResourceLoadedEvent):void
		{
			//etv.target.data
			doLog("onResLoaded: "+evt.resourcename);
			resources[evt.resourcename] = evt.data;
			cLoader.supplyFile( evt.resourcename, resources[evt.resourcename] );
			removeResourceloader( evt.resourcename );
		}

		private function onResError(evt:ResourceLoadedEvent):void
		{
			doLog("onResError: "+evt.resourcename);
			removeResourceloader( evt.resourcename );
		}

		private function removeResourceloader( resname:String ):void
		{
			resourceloader[resname].removeEventListener( ResourceLoadedEvent.RESOURCE_LOADED, onResLoaded );
			resourceloader[resname].removeEventListener( ResourceLoadedEvent.RESOURCE_ERROR, onResError );
			delete resourceloader[resname];
			resourceloader[resname] = null;			
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