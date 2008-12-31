package org.literacybridge.authoring.schema.helper
{
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

	public class BaseActionVisitor implements IBaseActionVisitor
	{
		public function BaseActionVisitor()
		{
		}

		public function visitSimpleAction(action:SimpleAction):void	{
			defaultHandling(action);
		}
		
		public function visitAdjustSpeedVolumeAction(action:AdjustSpeedVolumeAction):void {
			defaultHandling(action);
		}
		
		public function visitCallBlockAction(action:CallBlockAction):void {
			defaultHandling(action);
		}
		
		public function visitGotoAction(action:GotoAction):void	{
			defaultHandling(action);
		}
		
		public function visitLoadPackageAction(action:LoadPackageAction):void {
			defaultHandling(action);
		}
		
		public function visitNextPreviousBlockAction(action:NextPreviousBlockAction):void {
			defaultHandling(action);
		}
		
		public function visitRelativeTimeJumpAction(action:RelativeTimeJumpAction):void	{
			defaultHandling(action);
		}
		
		public function visitSetLightAction(action:SetLightAction):void	{
			defaultHandling(action);
		}
		
		public function visitSetUSBModeAction(action:SetUSBModeAction):void {
			defaultHandling(action);
		}
		
		protected function defaultHandling(action:Action):void {
			// default: do nothing
		}
	}
}