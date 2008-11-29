package org.literacybridge.authoring.schema {
	import mx.controls.Text;
	
	public final class SchemaConstants {
		// container attributes
		public static const Container_Att_Name:String = "Name";
		
		// top-level elements
		public static const Package:String = "Package";
			public static const Package_Att_Version:String = "Version";
			public static const Package_Att_Precision:String = "Precision";
		
		public static const File:String = "File";
		
		public static const Group:String = "Group";
		
		public static const Block:String = "Block";
			public static const Block_Att_Start:String = "Start";
			public static const Block_Att_End:String = "End";
			public static const Block_Att_Class:String = "Class";
					
		// eventHandlerElements
		public static const OnEnter:String = "OnEnter";
		public static const OnExit:String = "OnExit";
		public static const OnStart:String = "OnStart";
		public static const OnEnd:String = "OnEnd";

		public static const OnButtonClick:String = "OnButtonClick";
		public static const OnButtonHold:String = "OnButtonHold";
			public static const ButtonEvent_Att_Button1:String = "Button1";
			public static const ButtonEvent_Att_Button2:String = "Button2";
			public static const ButtonEvent_Att_InsertSoundBlock:String = "InsertSoundBlock";
			public static const ButtonEvent_Att_SetSpeed:String = "SetSpeed";

		/*
		 *   Buttons
		 */
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

		/*
		 *  Actions
		 */

		// simple actions
		public static const ActionNoOp:String = "NoOp";
		public static const ActionStop:String = "Stop";
		public static const ActionPause:String = "Pause";
		public static const ActionReturn:String = "Return";
		public static const ActionPlayPause:String = "PlayPause";

		// Goto
		public static const ActionGoto:String = "Goto";
			public static const ActionGoto_Att_ContainerName:String = "ContainerName";
			
		// LoadPackage
		public static const ActionLoadPackage:String = "LoadPackage";
			public static const ActionLoadPackage_Att_Name:String = "Name";
			
		// CallBlock
		public static const ActionCallBlock:String = "CallBlock";
			public static const ActionCallBlock_Att_BlockName:String = "BlockName";
			public static const ActionCallBlock_Att_ReturnRewindSeconds:String = "ReturnRewindSeconds";

		// RelativeTimeJump
		public static const ActionRelativeTimeJump:String = "RelativeTimeJump";
			public static const ActionRelativeTimeJump_Att_RelativeTime:String = "RelativeTime";

		// NextPreviousBlock actions
		public static const ActionNextBlock:String = "NextBlock";
		public static const ActionPreviousBlock:String = "PreviousBlock";
			public static const ActionNextPreviousBlock_Att_Class:String = "Class";

		// SetLight
		public static const ActionSetLight:String = "SetLight";
			public static const ActionSetLight_Att_Mode:String = "Mode";
			public static const ActionSetLight_Att_Light:String = "Light";

		// SetVolume
		public static const ActionSetVolume:String = "SetVolume";

		// SetSpeed
		public static const ActionSetSpeed:String = "SetSpeed";

		// SetUSBMode
		public static const ActionSetUSBMode:String = "SetUSBMode";
			public static const ActionSetUSBMode_Att_HostOrDevice:String = "HostOrDevice";
			public static const ActionSetUSBMode_Att_Active:String = "Active";

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
		public static const LightColorType_AttVal_Red:String = "Red";
		public static const LightColorType_AttVal_Green:String = "Green";  

		// lightModeType
		public static const LightModeType_AttVal_Off:String = "Off";
		public static const LightModeType_AttVal_On:String = "On";  
		
		
	}
}