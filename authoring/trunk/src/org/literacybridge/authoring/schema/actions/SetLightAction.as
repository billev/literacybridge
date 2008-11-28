package org.literacybridge.authoring.schema.actions {
	[Bindable]
	public class SetLightAction	extends Action {
		public static const Red:int = 1;
		public static const Green:int = 2;
		
		public static const On:int = 1;
		public static const Off:int = 2;
		
		public var color:int;
		public var mode:int;
	}
}