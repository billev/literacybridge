package org.literacybridge.authoring.player
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.net.URLRequest;
	
	public class Player extends EventDispatcher
	{
		// event
		public static const FILE_LOADED:String = "fileLoaded";
		
		// internal state
		public static const PLAYER_RUNNING:String = "Running";
		public static const PLAYER_PAUSING:String = "Pausing";
		public static const PLAYER_STOPPED:String = "Stopped";
		private var state:String = PLAYER_STOPPED;	// init state
		
		
		private var filePath:String;	// path to current sound file

		private var sound:Sound;
		private var channel:SoundChannel;
		private var pausePos:Number = 0;	// current position in the sound file (used for pausing)
		
		
				
		public function Player() {
			super();
		}

		public function loadSound(filePath:String):void	{	
			this.filePath = filePath;
			sound = new Sound();
			sound.addEventListener(Event.COMPLETE, onLoadingComplete);
			sound.load(new URLRequest(filePath));
		}


		public function startPlayer():void {
			if (channel != null) {
				channel.stop();	// we must stop old playing
			}
			channel = sound.play(pausePos);
			state = PLAYER_RUNNING;
		}
		
		public function pausePlayer():void {
			pausePos = channel.position;
			channel.stop();
			state = PLAYER_PAUSING;
		}

		public function stopPlayer():void {
			if (channel != null) {
				pausePos = 0;	
				channel.stop();
			}
			state = PLAYER_STOPPED;
		}	

		public function setPositionInSecs(newPos:Number):void {	
			var oldState:String = state;		
			stopPlayer();
			pausePos = newPos*1000; // we need milli secs
			if (oldState == PLAYER_RUNNING) {
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
			     
		public function getCurrentState():String {
			return state;
		}
			        
        private function onLoadingComplete(event:Event):void {
        	dispatchEvent(new Event(Player.FILE_LOADED));
        }
	}
}
