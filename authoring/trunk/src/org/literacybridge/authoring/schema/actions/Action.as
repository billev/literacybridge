package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class Action {
		private static var actionList:ArrayCollection;
		
		public static const GotoBlock:int = 1;
		
		public static function getActionList():ArrayCollection {
			if (actionList == null) {
				actionList = new ArrayCollection();
				actionList.addItem(GotoBlock);
			}
			
			return actionList;
		}

	}
}