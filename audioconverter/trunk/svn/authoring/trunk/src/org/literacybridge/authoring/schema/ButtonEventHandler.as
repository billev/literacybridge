package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ButtonEventHandler extends EventHandler {
		public static const ClickAction:int = 1;
		public static const HoldAction:int = 2;
		
		public static const PlayPauseButton:int = 1;
		public static const PlusButton:int = 2;
		public static const MinusButton:int = 3;
		public static const LeftButton:int = 4;
		public static const RightButton:int = 5;
		public static const UpButton:int = 6;
		public static const DownButton:int = 7;
		public static const SelectButton:int = 8;
		public static const HomeButton:int = 9;
		public static const StarButton:int = 10;

		private static var buttonList:ArrayCollection;
		private static var buttonActionList:ArrayCollection;

		public static function getButtonList():ArrayCollection {
			if (buttonList == null) {
				buttonList = new ArrayCollection();
				buttonList.addItem(PlayPauseButton);
				buttonList.addItem(PlusButton);
				buttonList.addItem(MinusButton);
				buttonList.addItem(LeftButton);
				buttonList.addItem(RightButton);
				buttonList.addItem(UpButton);
				buttonList.addItem(DownButton);
				buttonList.addItem(SelectButton);
				buttonList.addItem(HomeButton);
				buttonList.addItem(StarButton);
			}
			
			return buttonList;
		}
		
		public static function getButtonActionList():ArrayCollection {
			if (buttonActionList == null) {
				buttonActionList = new ArrayCollection();
				buttonActionList.addItem(ClickAction);
				buttonActionList.addItem(HoldAction);
			}
			
			return buttonActionList;
		}

		public var buttonAction:int;
		public var insertSoundBlock:String;
		public var setSpeed:int;
		public var button:int;
	}
}