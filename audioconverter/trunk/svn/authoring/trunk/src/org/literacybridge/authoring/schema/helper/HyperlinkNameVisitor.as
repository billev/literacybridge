package org.literacybridge.authoring.schema.helper
{
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.schema.Hyperlink;
	import org.literacybridge.authoring.schema.actions.Action;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	
	public class HyperlinkNameVisitor extends BaseActionVisitor
	{
		private var name:String = null;
		private var setNameMode:Boolean = false;	// false==get name; true==set name
		
		private var actionList:ArrayCollection = null;
		private var error:Boolean = false;
				
		public function HyperlinkNameVisitor(actionList:ArrayCollection=null)
		{
			this.actionList = actionList;	// can be null; if not collection will be checked for duplicated names-
			name = new String();
		}
		
		public function getActionName(action:Action):String {
			if (action == null) {
				throw new Error("class NameVisitor: Action is null!");
			}
			action.accept(this);
			if (name == null || name == '' || error) {
				throw new Error("class NameVisitor: Name is null, '' or type is undefined!");
			}
			return name;
		}
		
		/**
		 * true if name set successfully; false otherwise  
		 */
		public function setActionName(action:Action, newName:String):Boolean {
			setNameMode = true; // enable set mode
			name = newName;

			// if actionList != null, check if name already exists within this list
			if (actionList != null) {
				if (nameAlreadyExists(name)) {
					return false;
				}
			}
			
			if (action == null) {
				throw new Error("class NameVisitor: Action is null!");
			}
			action.accept(this);
			
			return true;
		}
		
		override public function visitCallBlockAction(action:CallBlockAction):void {
			if (setNameMode) {
				action.blockName = name;
			} else {
				name = action.blockName;
			}
		}
		
		override public function visitGotoAction(action:GotoAction):void {
			if (setNameMode) {
				action.containerName = name;
			} else {
				name = action.containerName;				
			}	
		}	

		override protected function defaultHandling(action:Action):void {
			name = "Action is not supported";
			error = true;
		}
		
		private function nameAlreadyExists(name:String):Boolean {
			if (actionList == null) return false;	// assuming user knows what he's doing
			
			var hyperlinkVisitor:HyperlinkNameVisitor = new HyperlinkNameVisitor();
			
			for (var i:int; i<actionList.length; ++i) {
				var curHyperlink:Hyperlink = actionList.getItemAt(i) as Hyperlink;
				var curHyperlinkName:String = ''; 
				try {
					curHyperlinkName = hyperlinkVisitor.getActionName(curHyperlink.action);
				} catch (e:Error) {
					trace ("Fatal error: Hyperlink without name in hyperlink list found.");
					break;
				}
				
				if (curHyperlinkName == name) {
					return true;
				}				
			}
			return false
		}
	}
}