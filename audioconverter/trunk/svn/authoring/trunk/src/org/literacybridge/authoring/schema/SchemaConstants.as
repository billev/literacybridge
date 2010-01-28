package org.literacybridge.authoring.schema {
	import mx.controls.Text;
	
	public final class SchemaConstants {
		// container attributes
		public static const Container_Att_Name:String = "name";
		
		// top-level elements
		public static const Package:String = "package";
			public static const Package_Att_Version:String = "version";
			public static const Package_Att_Precision:String = "precision";
		
		public static const File:String = "file";
		
		public static const Group:String = "group";
		
		public static const Block:String = "block";
			public static const Block_Att_Start:String = "start";
			public static const Block_Att_End:String = "end";
			public static const Block_Att_Class:String = "class";
			
		public static const Hyperlink:String = "hyperlink";
			public static const Hyperlink_Att_Start:String = "start";
			public static const Hyperlink_Att_End:String = "end";
					
		// eventHandlerElements
		public static const OnEnter:String = "onEnter";
		public static const OnExit:String = "onExit";
		public static const OnStart:String = "onStart";
		public static const OnEnd:String = "onEnd";

		public static const OnButtonClick:String = "onButtonClick";
		public static const OnButtonHold:String = "onButtonHold";
			public static const ButtonEvent_Att_Button:String = "button";
			public static const ButtonEvent_Att_InsertSoundBlock:String = "insertSoundBlock";
			public static const ButtonEvent_Att_SetSpeed:String = "setSpeed";

		/*
		 *   Buttons
		 */
		public static const PlayPauseButton:String = "playPause";
		public static const PlusButton:String = "plus";
		public static const MinusButton:String = "minus";
		public static const LeftButton:String = "left";
		public static const RightButton:String = "right";
		public static const UpButton:String = "up";
		public static const DownButton:String = "down";
		public static const SelectButton:String = "select";
		public static const HomeButton:String = "home";
		public static const StarButton:String = "star";

		/*
		 *  Actions
		 */

		// simple actions
		public static const ActionNoOp:String = "noOp";
		public static const ActionStop:String = "stop";
		public static const ActionPause:String = "pause";
		public static const ActionReturn:String = "return";
		public static const ActionPlayPause:String = "playPause";

		// Goto
		public static const ActionGoto:String = "goto";
			public static const ActionGoto_Att_ContainerName:String = "containerName";
			
		// LoadPackage
		public static const ActionLoadPackage:String = "loadPackage";
			public static const ActionLoadPackage_Att_Name:String = "name";
			
		// CallBlock
		public static const ActionCallBlock:String = "callBlock";
			public static const ActionCallBlock_Att_BlockName:String = "blockName";
			public static const ActionCallBlock_Att_ReturnRewindSeconds:String = "returnRewindSeconds";

		// RelativeTimeJump
		public static const ActionRelativeTimeJump:String = "relativeTimeJump";
			public static const ActionRelativeTimeJump_Att_RelativeTime:String = "relativeTime";

		// NextPreviousBlock actions
		public static const ActionNextBlock:String = "nextBlock";
		public static const ActionPreviousBlock:String = "previousBlock";
			public static const ActionNextPreviousBlock_Att_Class:String = "class";

		// SetLight
		public static const ActionSetLight:String = "setLight";
			public static const ActionSetLight_Att_Mode:String = "mode";
			public static const ActionSetLight_Att_Light:String = "light";

		// SetVolume
		public static const ActionSetVolume:String = "setVolume";

		// SetSpeed
		public static const ActionSetSpeed:String = "setSpeed";

		// SetUSBMode
		public static const ActionSetUSBMode:String = "setUSBMode";
			public static const ActionSetUSBMode_Att_HostOrDevice:String = "hostOrDevice";
			public static const ActionSetUSBMode_Att_Active:String = "active";

		/*
		 * Simple types
		 */
		// adjustSpeedVolumeType
		public static const AdjustSpeedVolumeType_Att_Relative:String = "relative";

		// relativeSpeedVolumeType
		public static const RelativeSpeedVolumeType_AttVal_Up:String = "up";
		public static const RelativeSpeedVolumeType_AttVal_Down:String = "down";
		public static const RelativeSpeedVolumeType_AttVal_Normal:String = "normal";

		// hostOrDeviceType
		public static const HostOrDeviceType_AttVal_Host:String = "host";
		public static const HostOrDeviceType_AttVal_Device:String = "device";

		// lightColorType
		public static const LightColorType_AttVal_Red:String = "red";
		public static const LightColorType_AttVal_Green:String = "green";  

		// lightModeType
		public static const LightModeType_AttVal_Off:String = "off";
		public static const LightModeType_AttVal_On:String = "on";  
		
		
	}
}