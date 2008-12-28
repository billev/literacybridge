package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	import mx.resources.ResourceManager;
	
	[Bindable]
	public class SetLightAction	extends Action {
		
		static private var colorList:ArrayCollection;
		static private var stateList:ArrayCollection;
		
		public static const Red:int = 1;
		public static const Green:int = 2;
		
		public static const On:int = 1;
		public static const Off:int = 2;
		
		public var color:int;
		public var mode:int;
		
		
		static public function getColorList():ArrayCollection {
			if (colorList == null) {
				colorList = new ArrayCollection();
				colorList.addItem(Red);
				colorList.addItem(Green);
			}
			return colorList;
		}

		static public function getStateList():ArrayCollection {
			if (stateList == null) {
				stateList = new ArrayCollection();
				stateList.addItem(On);
				stateList.addItem(Off);
			}
			return stateList;
		}		
	}
}