package org.literacybridge.authoring.schema.actions {
	[Bindable]
	public class AdjustSpeedVolumeAction extends Action {
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
	}
}