package org.literacybridge.authoring.schema {
	[Bindable]
	public class ButtonEventHandler extends EventHandler {
		public static const ACTION_CLICK:int = 1;
		public static const ACTION_HOLD:int = 2;
		
		public static const PlayPauseButton:String = "PlayPause";
		public static const PlusButton:String = "Plus";
		public static const MinusButton:String = "Minus";
		public static const LeftButton:String = "Left";
		public static const RightButton:String = "Right";
		public static const UpButton:String = "Up";
		public static const DownButton:String = "Down";
		public static const SelectButton:String = "Select";
		public static const HomeButton:String = "Home";
		public static const StarButton:String = "Star";

		
		public var buttonAction:int;
		public var insertSoundBlock:String;
		public var setSpeed:int;
		public var button1:String;
		public var button2:String;
	}
}