package sdl.audio
{
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.events.Event;
	import flash.events.SampleDataEvent;
	import flash.utils.Endian;
	import flash.utils.ByteArray;
	import flashx.textLayout.property.IntPropertyHandler;

	import sdl.LibSDL;
	
	public class AudioBridge
	{
		private var soundChannel:SoundChannel;
		private var sound:Sound;

		private var mute:Boolean;

		internal var libSDL:LibSDL;
		
		public var requestedSamples:int;

		public function AudioBridge( libSDL:LibSDL )
		{
			mute = false;
			this.libSDL = libSDL;		
			requestedSamples = 300;
			
			// Audio
			sound = new Sound;
			sound.addEventListener( SampleDataEvent.SAMPLE_DATA, onSampleData );			
		}
		
		public function set muted( muted:Boolean ):void
		{
			mute = muted;
			//cLib.setMute( muted as int );
		}
		public function get muted():Boolean
		{
			return mute;
		}

		private function onSoundComplete( e:Event ):void
		{
			this.soundChannel.removeEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			this.soundChannel = null;
		}
		
		private function onSampleData( e:SampleDataEvent ):void
		{
			e.data.endian = Endian.LITTLE_ENDIAN;
			libSDL.cLib.paintSound( e.data );
		}

		public function updateAudio():void
		{
			if (!mute && !soundChannel)
			{
				soundChannel = sound.play();
				soundChannel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			}
		}
	}
}
