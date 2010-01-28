package org.literacybridge.authoring.player
{
	import flash.media.Sound;
	
	public interface IPlayer
	{
		// Call this to initialize the player with a sound file
		function init(sound:Sound):void;	
		function start():void;
		function pause():void;
		function stop():void;
		// if the player is already running, it will continue playing from the new position.
		// if the player is not running, you have to call 'start' yourself
		//
		// @param newPosition sets new position in milli seconds
		// @return true if the player will continue playing itself; false otherwise
		function playFromPosition(newPosition:Number):Boolean;
		// @return sound length in milli seconds
		function get soundLength():Number;
		// is player initialized
		function isInitialized():Boolean
		// @return current internal player state (see class PlayerStates)
		function get state():String;
		// @return current position
		function get currentPosition():Number;
	}
}