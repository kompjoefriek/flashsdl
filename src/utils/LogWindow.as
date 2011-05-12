package utils
{
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;

	public class LogWindow extends Sprite
	{
		private var _tf:TextField;
		private var _fmt:TextFormat;

		public function LogWindow(_w:uint,_h:uint)
		{
			// Background
			//width = _w;
			//height = _h;

			graphics.beginFill(0x333333, 0.50); // second param controls alpha
			graphics.drawRect(0,0,_w,_h); // making rect the size of the tf
			graphics.endFill();

			_fmt = new TextFormat("_sans", 11, 0xFFFFFF);
			_tf = new TextField();
			// Add little margin
			_tf.x=2;
			_tf.y=2;
			_tf.width = _w-4;
			_tf.height = _h-4;
			_tf.multiline = true;
			_tf.selectable = false;
			_tf.defaultTextFormat = _fmt;
			_tf.text = "Test";
			addChild(_tf);
		}

		public function write(txt:String):void
		{
			var now:Date = new Date();
			_tf.appendText("\n"+now.getTime()+": "+txt);
			_tf.scrollV = _tf.maxScrollV;
		}
	
		public function set textColor(col:uint):void
		{
			_tf.textColor = col;
		}
	}
}