package org.literacybridge.authoring.player
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.TimerEvent;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.net.URLRequest;
	import flash.utils.Timer;
	
	public class Player extends EventDispatcher
	{
		// event
		public static const FILE_LOADED:String = "fileLoaded";
		public static const FILE_COMPLETE:String = "fileComplete";
		
		// internal state
		public static const PLAYER_RUNNING:String = "Running";
		public static const PLAYER_PAUSING:String = "Pausing";
		public static const PLAYER_STOPPED:String = "Stopped";
		private var state:String = PLAYER_STOPPED;	// init state
		
		
		private var filePath:String;	// path to current sound file

		private var sound:Sound;
		private var channel:SoundChannel;
		private var pausePos:Number = 0;	// current position in the sound file (used for pausing)
		
		private var posTimer:Timer;
		private var posTimerDelay:Number = 500;
		private var _currentChannelPosition:Number = 0;	// tmp variable to get the current pos
				
		public function Player() {
			super();
		}

		public function loadSound(filePath:String):void	{	
			this.filePath = filePath;
			sound = new Sound();
			sound.addEventListener(Event.COMPLETE, onLoadingComplete);
			sound.load(new URLRequest(filePath));
			// timer
			posTimer = new Timer(posTimerDelay);
			posTimer.addEventListener(TimerEvent.TIMER, onPositionTimer);
		}


		public function startPlayer():void {
			if (channel != null) {
				channel.stop();	// we must stop old playing
			}
			channel = sound.play(pausePos);
			channel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			posTimer.start();
			state = PLAYER_RUNNING;
		}
		
		public function pausePlayer():void {
			if (channel != null) {
				pausePos = channel.position;
				channel.stop();
				state = PLAYER_PAUSING;
				// stop timer and set current pos 
				posTimer.reset();
				currentChannelPosition = pausePos;	// fire update
			}
		}

		public function stopPlayer():void {
			if (channel != null) {
				pausePos = 0;	
				channel.stop();
				posTimer.stop();
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
		
		public function set currentChannelPosition( newValue:Number ):void {
			_currentChannelPosition = newValue;
		}
		
		[Bindable]
		public function get currentChannelPosition():Number {
			return _currentChannelPosition;
		}
		
		private function onPositionTimer(e:TimerEvent):void {
			if (channel != null) {
				this.currentChannelPosition = channel.position;		
			} 
		}
			        
        private function onLoadingComplete(event:Event):void {
        	dispatchEvent(new Event(Player.FILE_LOADED));
        }
        
        private function onSoundComplete(event:Event):void {
        	state = PLAYER_STOPPED;
        	pausePos = 0;
        	dispatchEvent(new Event(Player.FILE_COMPLETE));
        }
	}
}
