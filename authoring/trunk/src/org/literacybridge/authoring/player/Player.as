package org.literacybridge.authoring.player
{
	import flash.events.Event;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.net.URLRequest;
	
	public class Player
	{
		
		private var sound:Sound;
		private var channel:SoundChannel;
		private var pausePos:int = 0;
		
		private var filePath:String; // "F:\\PROJECTS\\Literacy\\sources\\PieceOfMe.mp3"

		
		public function Player(newFilePath:String)
		{
			this.filePath = newFilePath;
			loadSound();
		}

		public function startPlayer():void
		{
			if (channel != null)
			{
				channel.stop();	// we must stop old playing
			}

			channel = sound.play(pausePos);	// start from beginning	
		}
		
		public function pausePlayer():void
		{
			pausePos = channel.position;
			channel.stop();
		}

		public function stopPlayer():void
		{
			pausePos = 0;
			channel.stop();
		}	

		private function loadSound():void
		{
			
			sound = new Sound();
			
			// add listener for - SOUND_COMPLETE
			sound.addEventListener(Event.SOUND_COMPLETE, soundComplete);
			
			sound.load(new URLRequest(filePath));
			pausePos = 0;
		}
		
		private function soundComplete(event:Event):void
		{
			// ... do something useful
		}
	}
}