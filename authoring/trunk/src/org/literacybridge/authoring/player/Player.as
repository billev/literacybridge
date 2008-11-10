package org.literacybridge.authoring.player
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.net.URLRequest;
	
	public class Player extends EventDispatcher
	{
		public static const FILE_LOADED:String = "fileLoaded";
		
		private var sound:Sound;
		private var channel:SoundChannel;
		private var pausePos:Number = 0;	// current position in the sound file (used for pausing)
		
		private var filePath:String;	// path to current sound file
	
				
		public function Player() {
			super();
		}

		public function loadSound(filePath:String):void	{	
			this.filePath = filePath;
			sound = new Sound();
			sound.addEventListener(Event.COMPLETE, onLoadingComplete);
			sound.load(new URLRequest(filePath));
		}


		public function startPlayer():Number {
			if (channel != null) {
				channel.stop();	// we must stop old playing
			}
			channel = sound.play(pausePos*1000);
			return getSoundLengthInSecs();	
		}
		
		public function pausePlayer():void {
			pausePos = channel.position;
			channel.stop();
		}

		public function stopPlayer():Boolean {
			var isRunning:Boolean = false; 
			if (channel != null) {
			isRunning = (channel.position != 0);
			pausePos = 0;	
			channel.stop();
			}
			return isRunning;
		}	

		public function setPosition(newPos:Number):void {			
			var wasRunning:Boolean = stopPlayer();
			pausePos = newPos;
			if (wasRunning) {	// restart player
				startPlayer();
			}
		}
		
		public function getSoundLengthInSecs():Number {
			return sound.length / 1000;
		}

		public function getSoundLengthInMilliSecs():Number {
			return sound.length;
		}		
		
		public function isPlayerReady():Boolean {
			return (sound != null);
		}
			        
        private function onLoadingComplete(event:Event):void {
        	dispatchEvent(new Event(Player.FILE_LOADED));
        }
	}
}