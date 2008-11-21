package org.literacybridge.authoring.player
{
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.media.Sound;
	import flash.media.SoundChannel;
	import flash.net.URLRequest;
	
	public class Player extends EventDispatcher implements IPlayer
	{
		// event
		public static const PLAYER_INITIALIZED:String 	= "playerInitialized";
		public static const PLAYER_FINISHED:String 		= "playerFinished";
		
		// internal state
		private var _state:String 					= PlayerStates.PLAYER_STOPPED;
		// path to current sound file
		private var filePath:String = "";

		// Sound
		private var sound:Sound 		 	= null;
		private var channel:SoundChannel 	= null;
		private var position:Number	= 0;
		
		// Constructor		
		public function Player() 
		{
			super();
		}

		/*
		 * Methods 
		 */
		
		public function init(filePath:String):void	
		{	
			this.filePath = filePath;
			position = 0;
			channel = null;
			sound = new Sound();
			sound.addEventListener(Event.COMPLETE, onLoadingComplete);
			sound.load(new URLRequest(filePath));
		}

		public function start():void 
		{
			if (channel != null) // JTDB
			{
				channel.stop();
			}
			channel = sound.play(position);
			channel.addEventListener(Event.SOUND_COMPLETE, onSoundComplete);
			state = PlayerStates.PLAYER_RUNNING;
		}
		
		public function pause():void 
		{
			if (channel != null) 
			{
				position = channel.position;
				channel.stop();
				state = PlayerStates.PLAYER_PAUSING; 
			}
		}

		public function stop():void 
		{
			if (channel != null) 
			{
				position = 0;	
				channel.stop();
			}
			state = PlayerStates.PLAYER_STOPPED;
		}	

		public function playFromPosition(newPosition:Number):Boolean
		{	
			var oldState:String = _state;		
			stop();
			position = newPosition;
			if (oldState == PlayerStates.PLAYER_RUNNING) 
			{	
				start();
				return true;
			}
			return false; 
		}
		
		public function get soundLength():Number 
		{
			if (isInitialized()) return sound.length;
			else return 0;
		}		
		
		public function isInitialized():Boolean
		 {
			return (sound != null);
		}
			
		[Bindable]
		public function get state():String 
		{
			return _state;
		}
		private function set state(newState:String):void
		{
			_state = newState;
		}
		
		public function get currentPosition():Number
		{
			if (channel != null)
			{
				return channel.position;
			}
			return 0;
		}
		

		/*
		 * Events
		 */
		
        private function onLoadingComplete(event:Event):void 
        {
        	dispatchEvent(new Event(Player.PLAYER_INITIALIZED));
        }
        
        private function onSoundComplete(event:Event):void 
        {
        	state = PlayerStates.PLAYER_STOPPED;
        	position = 0;
        	dispatchEvent(new Event(Player.PLAYER_FINISHED));
        }
	}
}
