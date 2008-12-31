package org.literacybridge.authoring.schema.helper
{
	import org.literacybridge.authoring.schema.actions.Action;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	import org.literacybridge.authoring.schema.actions.LoadPackageAction;
	import org.literacybridge.authoring.schema.actions.NextPreviousBlockAction;
	
	public class NameVisitor extends BaseActionVisitor
	{
		private var name:String = null;
		
		public function NameVisitor()
		{
			name = new String();
		}
		
		public function getActionName(action:Action):String {
			if (action == null) {
				throw new Error("class NameVisitor: Action is null!");
			}
			action.accept(this);
			return name;
		}
		
		override public function visitCallBlockAction(action:CallBlockAction):void {
			name = action.blockName;
		}
		
		override public function visitGotoAction(action:GotoAction):void {
			name = action.containerName;	
		}	

		override protected function defaultHandling(action:Action):void {
			name = "Action is not supported or has no name";
		}
	}
}