package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.schema.actions.Action;
	import org.literacybridge.authoring.schema.actions.AdjustSpeedVolumeAction;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	import org.literacybridge.authoring.schema.actions.LoadPackageAction;
	import org.literacybridge.authoring.schema.actions.NextPreviousBlockAction;
	import org.literacybridge.authoring.schema.actions.RelativeTimeJumpAction;
	import org.literacybridge.authoring.schema.actions.SetLightAction;
	import org.literacybridge.authoring.schema.actions.SetUSBModeAction;
	import org.literacybridge.authoring.schema.actions.SimpleAction;
	import org.literacybridge.authoring.util.XMLUtils;
	
	public class ContentXMLWriter {
		public function write(content:ContentPackage):void {
			var data:XML = createXMLData(content);
			XMLUtils.writeXMLData(data, content.packageFile);
		}
		
		private function createXMLData(content:ContentPackage):XML {
			var data : XML;			
			
			data = <{SchemaConstants.Package}/>
			appendContainerAttributes(data, content);

			data.@[SchemaConstants.Package_Att_Precision] = content.precision;
			data.@[SchemaConstants.Package_Att_Version] = content.version;
			
			appendEventHandlers(data, content.eventHandlers);
			if (content.children != null) {
				appendFiles(data, content.children);
			}
		    return data;
		}

		private function appendContainerAttributes(data:XML, container:ContentContainer):void {
			data.@[SchemaConstants.Container_Att_Name] = container.label;
		}
		
		private function appendFiles(data:XML, files:ArrayCollection):void {
			for (var i:int=0; i<files.length; i++) {
				var file:ContentFile = files.getItemAt(i) as ContentFile;
				var fileData:XML;
				fileData = <{SchemaConstants.File}/>;
				appendContainerAttributes(fileData, file);
				if (file.eventHandlers != null) {
			    	appendEventHandlers(fileData, file.eventHandlers);
			 	}
			 	if (file.children != null) {
					appendBlocks(fileData, file.children);
				}
				if (file.hyperlinks != null) {
					appendHyperlinks(fileData, file.hyperlinks);
				}
				data.appendChild(fileData);
			}
		}

		private function appendBlocks(data:XML, blocks:ArrayCollection):void {
			for (var i:int=0; i<blocks.length; i++) {
				var block:ContentBlock = blocks.getItemAt(i) as ContentBlock;
				var blockData:XML;
				blockData = <{SchemaConstants.Block}/>
				appendContainerAttributes(blockData, block);
				
				blockData.@[SchemaConstants.Block_Att_Start] = block.start;
				blockData.@[SchemaConstants.Block_Att_End] = block.end;
				blockData.@[SchemaConstants.Block_Att_Class] = block.className;
				
				if (block.eventHandlers != null) {
					appendEventHandlers(blockData, block.eventHandlers);
				}
				if (block.children != null) {
					appendBlocks(blockData, block.children);
				}
				data.appendChild(blockData);
			}
		}

		private function appendHyperlinks(data:XML, hyperlinks:ArrayCollection):void {
			for (var i:int=0; i<hyperlinks.length; i++) {
				var hyperlink:Hyperlink = hyperlinks.getItemAt(i) as Hyperlink;
				var hyperlinkData:XML;
				hyperlinkData = <{SchemaConstants.Hyperlink}/>
				
				hyperlinkData.@[SchemaConstants.Hyperlink_Att_Start] = hyperlink.start;
				hyperlinkData.@[SchemaConstants.Hyperlink_Att_End] = hyperlink.end;

				appendAction(hyperlinkData, hyperlink.action);
				data.appendChild(hyperlinkData);
			}
		}
		
		private function appendEventHandlers(data:XML, eventHandlers:ArrayCollection):void {
			for (var i:int=0; i<eventHandlers.length; i++) {
				var eventHandler:Object = eventHandlers.getItemAt(i);
				if (eventHandler is ContentEventHandler) {
					appendContentEventHandler(data, eventHandler as ContentEventHandler);
				} else {
					appendButtonEventHandler(data, eventHandler as ButtonEventHandler);
				}
			}
		} 
		
		private function appendContentEventHandler(data:XML, eventHandler:ContentEventHandler):void {
			var contentEvent:XML;
			switch (eventHandler.event) {
				case ContentEventHandler.Enter : {
					contentEvent = <{SchemaConstants.OnEnter}/>
					break;
				}
				case ContentEventHandler.Exit : {
					contentEvent = <{SchemaConstants.OnExit}/>
					break;
				}
				case ContentEventHandler.Start : {
					contentEvent = <{SchemaConstants.OnStart}/>
					break;
				}
				case ContentEventHandler.End : {
					contentEvent = <{SchemaConstants.OnEnd}/>
					break;
				}					
			}
			appendActions(contentEvent, eventHandler.actions);
			data.appendChild(contentEvent);
		}

		private function appendButtonEventHandler(data:XML, eventHandler:ButtonEventHandler):void {
			var buttonEvent:XML;
			switch (eventHandler.buttonAction) {
				case ButtonEventHandler.ClickAction : {
					buttonEvent = <{SchemaConstants.OnButtonClick}/>
					break;
				}
				case ButtonEventHandler.HoldAction : {
					buttonEvent = <{SchemaConstants.OnButtonHold}/>
					break;
				}
			}

			buttonEvent.@[SchemaConstants.ButtonEvent_Att_Button] = determineButtonName(eventHandler.button);
			buttonEvent.@[SchemaConstants.ButtonEvent_Att_InsertSoundBlock] = eventHandler.insertSoundBlock;
			buttonEvent.@[SchemaConstants.ButtonEvent_Att_SetSpeed] = determineRelativeValue(eventHandler.setSpeed);
			
			appendActions(buttonEvent, eventHandler.actions);
			data.appendChild(buttonEvent);			
		}
		
		private function appendActions(event:XML, actions:ArrayCollection):void {
			for (var i:int=0; i < actions.length; i++) {
				appendAction(event, actions.getItemAt(i) as Action);
			}			
		}
		
		private function appendAction(data:XML, action:Action):void {
			var a:XML;
			if (action is SimpleAction) {
				a = getSimpleAction(action as SimpleAction);
			}
			if (action is AdjustSpeedVolumeAction) {
				a = getAdjustSpeedVolumeAction(action as AdjustSpeedVolumeAction);
			}
			if (action is CallBlockAction) {
				a = getCallBlockAction(action as CallBlockAction);
			}
			if (action is GotoAction) {
				a = getGotoAction(action as GotoAction);
			}
			if (action is LoadPackageAction) {
				a = getLoadPackageAction(action as LoadPackageAction);
			}
			if (action is NextPreviousBlockAction) {
				a = getNextPreviousBlockAction(action as NextPreviousBlockAction);
			}
			if (action is RelativeTimeJumpAction) {
				a = getRelativeTimeJumpAction(action as RelativeTimeJumpAction);
			}
			if (action is SetLightAction) {
				a = getSetLightAction(action as SetLightAction);
			}
			if (action is SetUSBModeAction) {
				a = getSetUSBModeAction(action as SetUSBModeAction);
			}
			data.appendChild(a);
		}
		
		private function getSimpleAction(action:SimpleAction):XML {
			var a:XML;

			switch (action.action) {
				case SimpleAction.NoOp : {
					a = <{SchemaConstants.ActionNoOp}/>
					break;
				}
				case SimpleAction.Pause : {
					a = <{SchemaConstants.ActionPause}/>
					break;
				}
				case SimpleAction.PlayPause : {
					a = <{SchemaConstants.ActionPlayPause}/>
					break;
				}
				case SimpleAction.Return : {
					a = <{SchemaConstants.ActionReturn}/>
					break;
				}
				case SimpleAction.Stop : {
					a = <{SchemaConstants.ActionStop}/>
					break;
				}

			}
			
			return a;
		}

		private function getAdjustSpeedVolumeAction(action:AdjustSpeedVolumeAction):XML {
			var a:XML;
			if (action.mode == AdjustSpeedVolumeAction.Speed) {
				a = <{SchemaConstants.ActionSetSpeed}/>;
			} else {
				a = <{SchemaConstants.ActionSetVolume}/>;
			}
			
			a.@[SchemaConstants.AdjustSpeedVolumeType_Att_Relative] = determineRelativeValue(action.relative);
			return a;
		}
		
		private function getCallBlockAction(action:CallBlockAction):XML {
			var a:XML = <{SchemaConstants.ActionCallBlock}/>
			a.@[SchemaConstants.ActionCallBlock_Att_BlockName] = action.blockName;
			a.@[SchemaConstants.ActionCallBlock_Att_ReturnRewindSeconds] = action.returnRewindSeconds;
			return a;
		}
		
		private function getGotoAction(action:GotoAction):XML {
			var a:XML = <{SchemaConstants.ActionGoto}/>
			a.@[SchemaConstants.ActionGoto_Att_ContainerName] = action.containerName;
			return a;
		}
		
		private function getLoadPackageAction(action:LoadPackageAction):XML {
			var a:XML = <{SchemaConstants.ActionLoadPackage}/>
			a.@[SchemaConstants.ActionLoadPackage_Att_Name] = action.name;
			return a;
		}
		
		private function getNextPreviousBlockAction(action:NextPreviousBlockAction):XML {
			var a:XML;
			if (action.mode == NextPreviousBlockAction.Next) {
				a = <{SchemaConstants.ActionNextBlock}/>
			} else {
				a = <{SchemaConstants.ActionPreviousBlock}/>
			}
			a.@[SchemaConstants.ActionNextPreviousBlock_Att_Class] = action.className;
			return a;
		}
		
		private function getRelativeTimeJumpAction(action:RelativeTimeJumpAction):XML {
			var a:XML = <{SchemaConstants.ActionRelativeTimeJump}/>
			a.@[SchemaConstants.ActionRelativeTimeJump_Att_RelativeTime] = action.relatveTime;
			return a;
		}
		
		private function getSetLightAction(action:SetLightAction):XML {
			var a:XML = <{SchemaConstants.ActionSetLight}/>
			if (action.color == SetLightAction.Red) {
				a.@[SchemaConstants.ActionSetLight_Att_Light] = SchemaConstants.LightColorType_AttVal_Red;
			} else {
				a.@[SchemaConstants.ActionSetLight_Att_Light] = SchemaConstants.LightColorType_AttVal_Green;
			}
			if (action.mode == SetLightAction.On) {
				a.@[SchemaConstants.ActionSetLight_Att_Mode] = SchemaConstants.LightModeType_AttVal_On;
			} else {
				a.@[SchemaConstants.ActionSetLight_Att_Mode] = SchemaConstants.LightModeType_AttVal_Off;
			}
			
			return a;
		}
		
		private function getSetUSBModeAction(action:SetUSBModeAction):XML {
			var a:XML = <{SchemaConstants.ActionSetUSBMode}/>
			
			if (action.mode == SetUSBModeAction.Device) {
				a.@[SchemaConstants.ActionSetUSBMode_Att_HostOrDevice] = SchemaConstants.HostOrDeviceType_AttVal_Device;
			} else {
				a.@[SchemaConstants.ActionSetUSBMode_Att_HostOrDevice] = SchemaConstants.HostOrDeviceType_AttVal_Host;
			}
			a.@[SchemaConstants.ActionSetUSBMode_Att_Active] = determineTrueFalse(action.active);
			
			return a;
		}
		
		private static function determineTrueFalse(value:Boolean):String {
			if (value) {
				return "true";
			} else {
				return "false";
			}
		}
		
		private static function determineRelativeValue(relative:int):String {
			switch (relative) {
				case AdjustSpeedVolumeAction.Up : return SchemaConstants.RelativeSpeedVolumeType_AttVal_Up;
				case AdjustSpeedVolumeAction.Down : return SchemaConstants.RelativeSpeedVolumeType_AttVal_Down;
				case AdjustSpeedVolumeAction.Normal : return SchemaConstants.RelativeSpeedVolumeType_AttVal_Normal;
				default : return ""; // TODO: throw exception here			
			}
		}
		
		private static function determineButtonName(button:int):String {
			switch (button) {
				case ButtonEventHandler.UpButton : return SchemaConstants.UpButton;
				case ButtonEventHandler.DownButton : return SchemaConstants.DownButton;
				case ButtonEventHandler.LeftButton : return SchemaConstants.LeftButton;
				case ButtonEventHandler.RightButton : return SchemaConstants.RightButton;
				case ButtonEventHandler.SelectButton : return SchemaConstants.SelectButton;
				case ButtonEventHandler.PlusButton : return SchemaConstants.PlusButton;
				case ButtonEventHandler.MinusButton : return SchemaConstants.MinusButton;
				case ButtonEventHandler.HomeButton : return SchemaConstants.HomeButton;
				case ButtonEventHandler.StarButton : return SchemaConstants.StarButton;
				case ButtonEventHandler.PlayPauseButton : return SchemaConstants.PlayPauseButton;
				default : return ""; // TODO: throw exception here
			}
		}

	}
}