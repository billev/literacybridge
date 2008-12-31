package org.literacybridge.authoring.schema.helper
{
	import org.literacybridge.authoring.schema.actions.AdjustSpeedVolumeAction;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	import org.literacybridge.authoring.schema.actions.LoadPackageAction;
	import org.literacybridge.authoring.schema.actions.NextPreviousBlockAction;
	import org.literacybridge.authoring.schema.actions.RelativeTimeJumpAction;
	import org.literacybridge.authoring.schema.actions.SetLightAction;
	import org.literacybridge.authoring.schema.actions.SetUSBModeAction;
	import org.literacybridge.authoring.schema.actions.SimpleAction;
	
	public interface IBaseActionVisitor
	{
		function visitSimpleAction(action:SimpleAction):void;
		function visitAdjustSpeedVolumeAction(action:AdjustSpeedVolumeAction):void;
		function visitCallBlockAction(action:CallBlockAction):void;
		function visitGotoAction(action:GotoAction):void;
		function visitLoadPackageAction(action:LoadPackageAction):void;
		function visitNextPreviousBlockAction(action:NextPreviousBlockAction):void;
		function visitRelativeTimeJumpAction(action:RelativeTimeJumpAction):void;
		function visitSetLightAction(action:SetLightAction):void;
		function visitSetUSBModeAction(action:SetUSBModeAction):void;
	}
}