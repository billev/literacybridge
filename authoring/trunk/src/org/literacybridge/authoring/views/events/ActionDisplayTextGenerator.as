package org.literacybridge.authoring.views.events {
	import org.literacybridge.authoring.schema.actions.Action;
	import org.literacybridge.authoring.schema.actions.AdjustSpeedVolumeAction;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	import org.literacybridge.authoring.schema.actions.LoadPackageAction;
	import org.literacybridge.authoring.schema.actions.NextPreviousBlockAction;
	import org.literacybridge.authoring.schema.actions.RelativeTimeJumpAction;
	import org.literacybridge.authoring.schema.actions.SetLightAction;
	import org.literacybridge.authoring.schema.actions.SetUSBModeAction;
	import org.literacybridge.authoring.schema.actions.SimpleAction;
	
	public class ActionDisplayTextGenerator	{
		public static function getDisplayTextAsHtml(action:Action):String {
			if (action is SimpleAction) {
				return getFromSimpleAction(action as SimpleAction);
			}
			
			if (action is SetLightAction) {
				return getFromSetLightAction(action as SetLightAction);	
			}
			
			if (action is AdjustSpeedVolumeAction) {
				return getFromAdjustSpeedVolumeAction(action as AdjustSpeedVolumeAction);
			}
			
			if (action is CallBlockAction) {
				return getFromCallBlockAction(action as CallBlockAction);
			}
			
			if (action is GotoAction) {
				return getFromGotoAction(action as GotoAction);
			}
			
			if (action is LoadPackageAction) {
				return getFromLoadPackageAction(action as LoadPackageAction);
			}
			
			if (action is NextPreviousBlockAction) {
				return getFromNextPreviousBlockAction(action as NextPreviousBlockAction);
			}
			
			if (action is RelativeTimeJumpAction) {
				return getFromRelativeTimeJumpAction(action as RelativeTimeJumpAction);
			}
			
			if (action is SetUSBModeAction) {
				return getFromSetUSBModeAction(action as SetUSBModeAction);
			}
			
			// throw exception here?
			return "Noop";
		}
		
		private static function getFromSimpleAction(action:SimpleAction):String {
			switch (action.action) {
				case SimpleAction.Pause : return "<b>Pause</b> playback.";
				case SimpleAction.PlayPause : return "<b>Play or pause</b> playback.";
				case SimpleAction.Return : return "<b>Return</b> to calling block.";
				case SimpleAction.Stop : return "<b>Stop</b> playback."; 
				case SimpleAction.NoOp: // fall through
				default : return "None";
			}
		}
		
		private static function getFromSetLightAction(action:SetLightAction):String {
			var s:String = "Turn <b>";
				if (action.color == SetLightAction.Red) {
					s += "<font color=\"#FF0000\">red";
				} else {
					s += "<font color=\"#128b05\">green";
				}
				if (action.mode == SetLightAction.On) {
					s += "</font></b> light <b>on</b>.";
				} else {
					s += "</font></b> light <b>off</b>.";
				}
			
			return s;
		}
		
		private static function getFromAdjustSpeedVolumeAction(action:AdjustSpeedVolumeAction):String {
			if (action.relative == AdjustSpeedVolumeAction.Normal) {
				if (action.mode == AdjustSpeedVolumeAction.Speed) {
					return "Set <b>speed</b> to <b>normal</b>.";
				} else {
					return "Set <b>volume</b> to <b>normal</b>.";
				}
			}
			
			if (action.mode == AdjustSpeedVolumeAction.Speed) {
				if (action.relative == AdjustSpeedVolumeAction.Up) {
					return "<b>Increase speed</b>.";
				} else {
					return "<b>Decrease speed</b>.";
				}
			} else {
				if (action.relative == AdjustSpeedVolumeAction.Up) {
					return "<b>Increase volume</b>.";
				} else {
					return "<b>Decrease volume</b>.";
				}				
			}
		}
		
		private static function getFromCallBlockAction(action:CallBlockAction):String {
			var s:String = "Call block '<b>" + action.blockName + "</b>'";
			if (action.returnRewindSeconds != 0) {
				s += ", rewind <b>" + action.returnRewindSeconds + " secs</b> after return.";
			} else {
				s += ".";
			}
			return s;
		}
		
		private static function getFromGotoAction(action:GotoAction):String {
			return "Goto '<b>" + action.containerName + "</b>'";
		}
		
		private static function getFromLoadPackageAction(action:LoadPackageAction):String {
			return "Load package '<b>" + action.name + "</b>'";
		}
		
		private static function getFromNextPreviousBlockAction(action:NextPreviousBlockAction):String {
			var s:String;
			if (action.mode == NextPreviousBlockAction.Next) {
				s = "Jump to <b>next</b> block";
			} else {
				s = "Jump to <b>previous</b> block";
			}
			
			if (action.className != null && action.className.length > 0) {
				s += " of class '<b>" + action.className + "</b>'.";
			} else {
				s += ".";
			}
			
			return s;
		}
		
		private static function getFromRelativeTimeJumpAction(action:RelativeTimeJumpAction):String {
			if (action.relatveTime < 0) {
				return "Jump <b>" + Math.abs(action.relatveTime) + " ms backward</b>.";
			} else {
				return "Jump <b>" + Math.abs(action.relatveTime) + " ms forward</b>.";
			}
		}
		
		private static function getFromSetUSBModeAction(action:SetUSBModeAction):String {
			var s:String;
			
			if (action.mode == SetUSBModeAction.Device) {
				s = "Set USB mode to '<b>Device</b>' and ";
			} else {
				s = "Set USB mode to '<b>Host</b>' and ";
			}
			
			if (action.active) {
				s += " <b>activate</b> USB port.";
			} else {
				s += " <b>deactivate</b> USB port.";
			}
			
			return s;
		}
	}
}