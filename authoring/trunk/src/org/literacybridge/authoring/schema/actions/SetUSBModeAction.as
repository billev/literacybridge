package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class SetUSBModeAction extends Action {
		static private var modeList:ArrayCollection;
		
		public static const Host:int = 1;
		public static const Device:int = 2;
	
		
		public var active:Boolean;
		public var mode:int;
		
		
		static public function getModeList():ArrayCollection {
			if (modeList == null) {
				modeList = new ArrayCollection();
				modeList.addItem(Host);
				modeList.addItem(Device);
			}
			return modeList;
		}
	}
}