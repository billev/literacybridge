package org.literacybridge.authoring.schema.actions {
	[Bindable]
	public class SetUSBModeAction extends Action {
		public static const Host:int = 1;
		public static const Device:int = 2;
		
		public var active:Boolean;
		public var mode:int;
	}
}