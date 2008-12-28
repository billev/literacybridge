package org.literacybridge.authoring.schema.actions {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class AdjustSpeedVolumeAction extends Action {
		// volume
		private static var volumeList:ArrayCollection = null;
		// speed
		private static var speedList:ArrayCollection = null;
		
		public static const Speed:int = 1;
		public static const Volume:int = 2;
		
		public static const Up:int = 1;
		public static const Down:int = 2;
		public static const Normal:int = 3;

		
		public var mode:int;
		public var relative:int;
		
		public function AdjustSpeedVolumeAction(mode:int) {
			this.mode = mode;
		}
		
		public static function getVolumeList():ArrayCollection {
			if (volumeList == null) {
				volumeList = new ArrayCollection();
				volumeList.addItem(Up);
				volumeList.addItem(Down);
				volumeList.addItem(Normal);
			}
			return volumeList;
		}
		
		public static function getSpeedList():ArrayCollection {
			if (speedList == null) {
				speedList = new ArrayCollection();
				speedList.addItem(Up);
				speedList.addItem(Down);
				speedList.addItem(Normal);
			}
			return speedList;
		}
	}
}