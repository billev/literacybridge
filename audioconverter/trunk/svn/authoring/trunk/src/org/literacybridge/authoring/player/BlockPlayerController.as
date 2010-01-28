package org.literacybridge.authoring.player
{
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	public class BlockPlayerController extends PlayerController
	{
		public static const END_POSITION_UNDEFINED:int = -1;
		
		protected var endPosition:Number	= 0;

		private var endPositionTimer:Timer 			= null;
		private var endPositionTimerDelay:Number 	= 5;
		
		public function BlockPlayerController(player:IPlayer)
		{
			super(player);
			endPositionTimer = new Timer(endPositionTimerDelay);
			endPositionTimer.addEventListener(TimerEvent.TIMER, endPositionCheck);
		}
		
		
		public function playRange(startPosition:Number=0, endPosition:Number=END_POSITION_UNDEFINED):void
		{
			if (player != null && player.isInitialized() && startPosition < endPosition) 
			{
				this.endPosition = endPosition;
				endPositionTimer.start();			
				super.startPlayer(startPosition);		
			}			
		}
		
		override public function stopPlayer():void
		{
			endPositionTimer.stop();
			super.stopPlayer();
		}
		
		
		override public function pausePlayer():void
		{
			endPositionTimer.stop();
			super.pausePlayer();
		}
		
		private function endPositionCheck(e:TimerEvent):void
		{
			if (player && player.isInitialized() && player.state == PlayerStates.PLAYER_RUNNING)
			{
				if ( endPosition <= super.currentPlayerPosition)
				{
					trace ("endPositionCheck called at :" + super.currentPlayerPosition)
					endPositionTimer.stop();
					stopPlayer();
				}
			}
		}	
	}
}