package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class Action {
		private static var actionList:ArrayCollection;
		
		// Simple Actions
		public static const Stop:int 		= 1;	// class SimpleAction
		public static const Pause:int 		= 2;	// class SimpleAction
		public static const Return:int 		= 3;	// class SimpleAction
		public static const PlayPause:int 	= 4;	// class SimpleAction
		
		// Complex Actions
		public static const SetSpeed:int	= 10;	// class AdjustSpeedVolumeAction
		public static const SetVolume:int	= 11;	// class AdjustSpeedVolumeAction
		public static const CallBlock:int	= 12;	// class CallBlockAction
		public static const Goto:int 		= 13;	// class GotoAction
		public static const NextBlock:int	= 14;	// class NextPreviousBlockAction
		public static const PrevBlock:int	= 15;	// class NextPreviousBlockAction
		public static const RelTimeJump:int	= 16;	// class RelativeTimeJumpAction
		public static const SetLight:int	= 17;	// class SetLightAction 
		public static const SetUSBMode:int	= 18;	// class SetUSBModeAction
		
		
		public static function getActionList():ArrayCollection {
			if (actionList == null) {
				actionList = new ArrayCollection();
				actionList.addItem(Stop);
				actionList.addItem(Pause);
				actionList.addItem(Return);
				actionList.addItem(PlayPause);
				
				actionList.addItem(SetSpeed);
				actionList.addItem(SetVolume);
				actionList.addItem(CallBlock);
				actionList.addItem(Goto);
				actionList.addItem(NextBlock);
				actionList.addItem(PrevBlock);
				actionList.addItem(RelTimeJump);
				actionList.addItem(SetLight);
				actionList.addItem(SetUSBMode);
			}
			return actionList;
		}
		
		// returns the constant that represents this action object or -1 as action is unknown
		public static function getActionConstant(action:Action):int {
			if (action is SimpleAction) {
				var tmp:SimpleAction = action as SimpleAction;	// SimpleActions
				switch (tmp.action) {
					case SimpleAction.Stop:
						return Stop;
					break;
					case SimpleAction.Pause:
						return Pause;
					break;
					case SimpleAction.Return:
						return Return;
					break;
					case SimpleAction.PlayPause:
						return PlayPause;
					break;
					default:
						return -1;	// should not happen
				}
			} else if (action is AdjustSpeedVolumeAction) {
				if ((action as AdjustSpeedVolumeAction).mode == AdjustSpeedVolumeAction.Speed) {	// speed
					return SetSpeed;
				} else {
					return SetVolume;	// volume
				}
			} else if (action is CallBlockAction) {	// callBlock
				return CallBlock;
			} else if (action is GotoAction) {	// Goto
				return Goto;
			} else if (action is NextPreviousBlockAction) {
				if ((action as NextPreviousBlockAction).mode == NextPreviousBlockAction.Next) {	// next
					return NextBlock;
				} else {
					return PrevBlock;	// prev
				}
			} else if (action is RelativeTimeJumpAction) {	// jumpTimeJump
				return RelTimeJump;
			} else if (action is SetLightAction) {	// setLight
				return SetLight;
			} else if (action is SetUSBModeAction) {	// SetUSBmode
				return SetUSBMode;
			}
			
			return -1;
		}
	}
}