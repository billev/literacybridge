package org.literacybridge.authoring.player
{
	import flash.events.EventDispatcher;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	import mx.binding.utils.BindingUtils;
	import mx.binding.utils.ChangeWatcher;
	
	public class PlayerController extends EventDispatcher
	{	
		// player to control
		protected var player:IPlayer 		= null;
		
		// Current position timer
		private var positionTimer:Timer				= null;
		private var positionTimerDelay:Number 		= 50;
		private var _currentChannelPosition:Number 	= 0;
		
		public function PlayerController(player:IPlayer)
		{			
			this.player = player;
 			positionTimer = new Timer(positionTimerDelay);
 			positionTimer.addEventListener(TimerEvent.TIMER, getPlayerPositionByTimer);		
		}
		
		public function isInitialized():Boolean
		{
			return (player != null && player.isInitialized());
		}
		
		public function startPlayer(startPosition:Number):void
		{
			if (player.isInitialized())
			{
				if (player.state == PlayerStates.PLAYER_PAUSING)
				{
					player.start();
				}
				else if (!player.playFromPosition(startPosition))
				{
					player.start();	
				}
				positionTimer.start();
			}	
		}
	
		public function stopPlayer():void
		{
			if (player.state == PlayerStates.PLAYER_RUNNING
			||  player.state == PlayerStates.PLAYER_PAUSING)
			{
				positionTimer.stop();
				player.stop();
			}
		}
	
		public function pausePlayer():void
		{
			if (player.state == PlayerStates.PLAYER_RUNNING)
			{
				player.pause();
				positionTimer.stop();
			}
		}
	
	
	
	
	
		/****************************************************************************************/
		/* Bindables																			*/
		/****************************************************************************************/
	
		[Bindable]
		public function get currentPlayerPosition():Number
		{
			return _currentChannelPosition;
		}
		
		
		/*
		 * Player position 
		 */

		// internal only !!
		protected function set currentPlayerPosition(newPosition:Number):void
		{
			_currentChannelPosition = newPosition;
		}
		protected  function getPlayerPositionByTimer(e:TimerEvent):void
		{
			currentPlayerPosition = player.currentPosition;
		}
	}
}