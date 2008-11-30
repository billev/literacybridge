package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	
	import org.literacybridge.authoring.schema.actions.AdjustSpeedVolumeAction;
	import org.literacybridge.authoring.schema.actions.CallBlockAction;
	import org.literacybridge.authoring.schema.actions.GotoAction;
	import org.literacybridge.authoring.schema.actions.LoadPackageAction;
	import org.literacybridge.authoring.schema.actions.NextPreviousBlockAction;
	import org.literacybridge.authoring.schema.actions.RelativeTimeJumpAction;
	import org.literacybridge.authoring.schema.actions.SetLightAction;
	import org.literacybridge.authoring.schema.actions.SetUSBModeAction;
	import org.literacybridge.authoring.schema.actions.SimpleAction;
	
	public class ContentXMLParser {
		public function parse(file:File):ContentPackage {
			var stream:FileStream = new FileStream();
			stream.open(file, FileMode.READ);
		    var data : XML = XML(stream.readUTFBytes(stream.bytesAvailable));
		    stream.close();
		    
		    var content:ContentPackage = parseData(data);
		    content.packageFile = file;
		    return content;
		}
		
		private function parseData(data:XML):ContentPackage {
			var p:ContentPackage = parseSinglePackage(data);
			for each (var file:XML in data.elements(SchemaConstants.File)) {
				var f:ContentFile = parseSingleFile(file);
				f.parent = p;				
				p.addFile(f);
				addSubBlocksRecursively(f, file);
			}
			return p;
		}
		
		private function addSubBlocksRecursively(container:SubBlockAppendable, data:XML):void {
			for each (var subBlock:XML in data.Block) {
				var b : ContentBlock = parseSingleBlock(subBlock);
				b.parent = container as ContentContainer;
				container.appendSubBlock(b);
				addSubBlocksRecursively(b, subBlock);
			}			
		}

		
		/*
		 *   Top-level Elements 
		 */
		
		private function parseSinglePackage(data:XML):ContentPackage {
			var p:ContentPackage = new ContentPackage();
			p.label = data.attribute(SchemaConstants.Container_Att_Name);
			p.precision = data.attribute(SchemaConstants.Package_Att_Precision);
			p.version = data.attribute(SchemaConstants.Package_Att_Version);
			parseEventHandlers(p, data);
			return p;			
		}

		private function parseSingleFile(data:XML):ContentFile {
			var f:ContentFile = new ContentFile();
			f.label = data.attribute(SchemaConstants.Container_Att_Name);
			parseEventHandlers(f, data);
			return f;			
		}

		private function parseSingleBlock(block:XML):ContentBlock {
			var b : ContentBlock = new ContentBlock();
			b.label = block.attribute(SchemaConstants.Container_Att_Name);

			if (block.attribute(SchemaConstants.Block_Att_Start) != undefined) {
				b.start = Number(block.attribute(SchemaConstants.Block_Att_Start));
			}
			if (block.attribute(SchemaConstants.Block_Att_End) != undefined) {
				b.end = Number(block.attribute(SchemaConstants.Block_Att_End));
			}		
			if (block.attribute(SchemaConstants.Block_Att_Class) != undefined) {
				b.className = block.attribute(SchemaConstants.Block_Att_Class);
			}
			parseEventHandlers(b, block);
			return b;	
		}

		
		private function parseEventHandlers(container:ContentContainer, data:XML):void {
			var event:XML;
			
			// Content Events
			for each (event in data.elements(SchemaConstants.OnEnter)) {
				parseContentEvent(container, event, ContentEventHandler.Enter);
			}
			for each (event in data.elements(SchemaConstants.OnExit)) {
				parseContentEvent(container, event, ContentEventHandler.Exit);
			}
			for each (event in data.elements(SchemaConstants.OnStart)) {
				parseContentEvent(container, event, ContentEventHandler.Start);
			}
			for each (event in data.elements(SchemaConstants.OnEnd)) {
				parseContentEvent(container, event, ContentEventHandler.End);
			}

			// Button events			
			for each (event in data.elements(SchemaConstants.OnButtonClick)) {
				parseButtonEvent(container, event, ButtonEventHandler.ClickAction);
			}
			for each (event in data.elements(SchemaConstants.OnButtonHold)) {
				parseButtonEvent(container, event, ButtonEventHandler.HoldAction);
			}			
		}

		private function parseContentEvent(container:ContentContainer, event:XML, eventType:int) :void {
			var h:ContentEventHandler = new ContentEventHandler(eventType);
			parseActions(h, event);
			container.eventHandlers.addItem(h);
		}
		
		private function parseButtonEvent(container:ContentContainer, buttonEvent:XML, buttonAction:int) :void {
			var b:ButtonEventHandler = new ButtonEventHandler();
			b.buttonAction = buttonAction;
			b.button1 = determineButton(buttonEvent.attribute(SchemaConstants.ButtonEvent_Att_Button1));
			b.button2 = determineButton(buttonEvent.attribute(SchemaConstants.ButtonEvent_Att_Button2));
			b.insertSoundBlock = buttonEvent.attribute(SchemaConstants.ButtonEvent_Att_InsertSoundBlock);
			
			if (buttonEvent.attribute(SchemaConstants.ButtonEvent_Att_SetSpeed) != undefined) {
				switch (buttonEvent.attribute(SchemaConstants.ButtonEvent_Att_SetSpeed)) {
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Up : {
						b.setSpeed = AdjustSpeedVolumeAction.Up;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Down : { 
						b.setSpeed = AdjustSpeedVolumeAction.Down;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Normal : { 
						b.setSpeed = AdjustSpeedVolumeAction.Normal;
						break;						
					}
				}
			}
			parseActions(b, buttonEvent);
			container.eventHandlers.addItem(b);
		}
		
		private function parseActions(handler:EventHandler, event:XML):void {
			if (event.elements(SchemaConstants.ActionStop) != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Stop));				
			} 
			if (event.elements(SchemaConstants.ActionPause) != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Pause));				
			} 
			if (event.elements(SchemaConstants.ActionReturn) != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Return));				
			} 
			if (event.elements(SchemaConstants.ActionPlayPause) != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.PlayPause));				
			} 
			
			// Goto action
			if (event.elements(SchemaConstants.ActionGoto) != undefined) {
				var goto:GotoAction = new GotoAction();
				goto.containerName = event.elements(SchemaConstants.ActionGoto).attribute(SchemaConstants.ActionGoto_Att_ContainerName);
				handler.actions.addItem(goto);
			}
			
			// LoadPackage action
			if (event.elements(SchemaConstants.ActionLoadPackage) != undefined) {
				var loadPackage:LoadPackageAction = new LoadPackageAction();
				loadPackage.name = event.elements(SchemaConstants.ActionLoadPackage).attribute(SchemaConstants.ActionLoadPackage_Att_Name);
				handler.actions.addItem(loadPackage);
			}
			
			// CallBlock action
			if (event.elements(SchemaConstants.ActionCallBlock) != undefined) {
				var callBlock:CallBlockAction = new CallBlockAction();
				callBlock.blockName = 
					event.elements(SchemaConstants.ActionCallBlock).attribute(SchemaConstants.ActionCallBlock_Att_BlockName);
				callBlock.returnRewindSeconds = 
					Number(event.elements(SchemaConstants.ActionCallBlock).attribute(SchemaConstants.ActionCallBlock_Att_ReturnRewindSeconds));
				handler.actions.addItem(callBlock);
			}
			
			// RelativeTimeJump action
			if (event.elements(SchemaConstants.ActionRelativeTimeJump) != undefined) {
				var relativeTimeJump:RelativeTimeJumpAction = new RelativeTimeJumpAction();
				relativeTimeJump.relatveTime = 
					Number(event.elements(SchemaConstants.ActionRelativeTimeJump).attribute(SchemaConstants.ActionRelativeTimeJump_Att_RelativeTime));
				handler.actions.addItem(relativeTimeJump);
			}
			
			// NextPreviousBlock actions
			if (event.elements(SchemaConstants.ActionNextBlock) != undefined) {
				var nextBlock:NextPreviousBlockAction = new NextPreviousBlockAction(NextPreviousBlockAction.Next);
				nextBlock.className = 
					event.elements(SchemaConstants.ActionNextBlock).attribute(SchemaConstants.ActionNextPreviousBlock_Att_Class);
				handler.actions.addItem(nextBlock);
			}
			
			if (event.elements(SchemaConstants.ActionPreviousBlock) != undefined) {
				var previousBlock:NextPreviousBlockAction = new NextPreviousBlockAction(NextPreviousBlockAction.Previous);
				previousBlock.className = event.elements(SchemaConstants.ActionPreviousBlock).attribute(SchemaConstants.ActionNextPreviousBlock_Att_Class);
				handler.actions.addItem(previousBlock);
			}
			
			// SetLight action
			for each (var s:XML in event.elements(SchemaConstants.ActionSetLight)) {
				var setLight:SetLightAction = new SetLightAction();
				if (s.attribute(SchemaConstants.ActionSetLight_Att_Light)==SchemaConstants.LightColorType_AttVal_Red) {
					setLight.color=SetLightAction.Red;
				} else {
					setLight.color=SetLightAction.Green;
				}
				if (s.attribute(SchemaConstants.ActionSetLight_Att_Mode)==SchemaConstants.LightModeType_AttVal_On) {
					setLight.mode = SetLightAction.On;
				} else {
					setLight.mode = SetLightAction.Off;
				}
				handler.actions.addItem(setLight);
			}
			
			// AdjustSpeenVolume actions
			if (event.elements(SchemaConstants.ActionSetVolume) != undefined) {
				var setVolume:AdjustSpeedVolumeAction = new AdjustSpeedVolumeAction(AdjustSpeedVolumeAction.Volume);
				switch (event.elements(SchemaConstants.ActionSetVolume).@relative) {
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Up: {
						setVolume.relative = AdjustSpeedVolumeAction.Up;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Down: {
						setVolume.relative = AdjustSpeedVolumeAction.Down;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Normal: {
						setVolume.relative = AdjustSpeedVolumeAction.Normal;
						break;
					}
				}

				handler.actions.addItem(setVolume);
			}
			
			if (event.elements(SchemaConstants.ActionSetSpeed) != undefined) {
				var setSpeed:AdjustSpeedVolumeAction = new AdjustSpeedVolumeAction(AdjustSpeedVolumeAction.Speed);
				switch (event.elements(SchemaConstants.ActionSetSpeed).attribute(SchemaConstants.AdjustSpeedVolumeType_Att_Relative)) {
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Up: {
						setSpeed.relative = AdjustSpeedVolumeAction.Up;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Down: {
						setSpeed.relative = AdjustSpeedVolumeAction.Down;
						break;
					}
					case SchemaConstants.RelativeSpeedVolumeType_AttVal_Normal: {
						setSpeed.relative = AdjustSpeedVolumeAction.Normal;
						break;
					}
				}
				handler.actions.addItem(setSpeed);
			}
			
			// SetUSBMode action
			if (event.elements(SchemaConstants.ActionSetUSBMode) != undefined) {
				var setUSBMode:SetUSBModeAction = new SetUSBModeAction();
				if (event.elements(SchemaConstants.ActionSetUSBMode).attribute(SchemaConstants.ActionSetUSBMode_Att_Active)=="true") {
					setUSBMode.active = true;
				} else {
					setUSBMode.active = false;
				}
				if (event.elements(SchemaConstants.ActionSetUSBMode).attribute(SchemaConstants.ActionSetUSBMode_Att_HostOrDevice)==
							SchemaConstants.HostOrDeviceType_AttVal_Host) {
					setUSBMode.mode = SetUSBModeAction.Host;
				} else {
					setUSBMode.mode = SetUSBModeAction.Device;
				}
				handler.actions.addItem(setUSBMode);
			}
		}
		
		private static function determineButton(buttonName:String):int {
			switch (buttonName) {
				case SchemaConstants.UpButton : return ButtonEventHandler.UpButton;
				case SchemaConstants.DownButton : return ButtonEventHandler.DownButton;
				case SchemaConstants.LeftButton : return ButtonEventHandler.LeftButton;
				case SchemaConstants.RightButton : return ButtonEventHandler.RightButton;
				case SchemaConstants.SelectButton : return ButtonEventHandler.SelectButton;
				case SchemaConstants.PlusButton : return ButtonEventHandler.PlusButton;
				case SchemaConstants.MinusButton : return ButtonEventHandler.MinusButton;
				case SchemaConstants.HomeButton : return ButtonEventHandler.HomeButton;
				case SchemaConstants.StarButton : return ButtonEventHandler.StarButton;
				case SchemaConstants.PlayPauseButton : return ButtonEventHandler.PlayPauseButton;
				default : return 0; 
			}
		}

	}
}