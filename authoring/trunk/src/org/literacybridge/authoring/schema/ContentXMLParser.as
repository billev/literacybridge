package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	
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
			for each (var file:XML in data.File) {
				var f:ContentFile = parseSingleFile(file);				
				p.addFile(f);
				addSubBlocksRecursively(f, file);
			}
			return p;
		}
		
		private function parseEventHandlers(container:ContentContainer, data:XML):void {
			var event:XML;
			
			// Content Events
			for each (event in data.OnEnter) {
				parseContentEvent(container, event, ContentEventHandler.Enter);
			}
			for each (event in data.OnExit) {
				parseContentEvent(container, event, ContentEventHandler.Exit);
			}
			for each (event in data.OnStart) {
				parseContentEvent(container, event, ContentEventHandler.Start);
			}
			for each (event in data.OnEnd) {
				parseContentEvent(container, event, ContentEventHandler.End);
			}

			// Button events			
			for each (event in data.OnButtonClick) {
				parseButtonEvent(container, event, ButtonEventHandler.ACTION_CLICK);
			}
			for each (event in data.OnButtonHold) {
				parseButtonEvent(container, event, ButtonEventHandler.ACTION_HOLD);
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
			b.button1 = buttonEvent.@Button1;
			b.button2 = buttonEvent.@Button2;
			b.insertSoundBlock = buttonEvent.@InsertSoundBlock;
			
			if (buttonEvent.@SetSpeed != undefined) {
				if (buttonEvent.@SetSpeed == "up") {
					b.setSpeed = AdjustSpeedVolumeAction.Up;
				}
				if (buttonEvent.@SetSpeed == "down") {
					b.setSpeed = AdjustSpeedVolumeAction.Down;
				}
				if (buttonEvent.@SetSpeed == "normal") {
					b.setSpeed = AdjustSpeedVolumeAction.Normal;
				}
			}
			parseActions(b, buttonEvent);
			container.eventHandlers.addItem(b);
		}
		
		private function parseActions(handler:EventHandler, event:XML):void {
			if (event.Stop != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Stop));				
			} 
			if (event.Pause != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Pause));				
			} 
			if (event.Return != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.Return));				
			} 
			if (event.PlayPause != undefined) {
				handler.actions.addItem(new SimpleAction(SimpleAction.PlayPause));				
			} 
			
			// Goto action
			if (event.Goto != undefined) {
				var goto:GotoAction = new GotoAction();
				goto.containerName = event.Goto.@ContainerName;
				handler.actions.addItem(goto);
			}
			
			// LoadPackage action
			if (event.LoadPackage != undefined) {
				var loadPackage:LoadPackageAction = new LoadPackageAction();
				loadPackage.name = event.LoadPackage.@Name;
				handler.actions.addItem(loadPackage);
			}
			
			// CallBlock action
			if (event.CallBlock != undefined) {
				var callBlock:CallBlockAction = new CallBlockAction();
				callBlock.blockName = event.CallBlock.@BlockName;
				callBlock.returnRewindSeconds = Number(event.CallBlock.@ReturnRewindSeconds);
				handler.actions.addItem(callBlock);
			}
			
			// RelativeTimeJump action
			if (event.RelativeTimeJump != undefined) {
				var relativeTimeJump:RelativeTimeJumpAction = new RelativeTimeJumpAction();
				relativeTimeJump.relatveTime = Number(event.RelativeTimeJump.@RelativeTime);
				handler.actions.addItem(relativeTimeJump);
			}
			
			// NextPreviousBlock actions
			if (event.NextBlock != undefined) {
				var nextBlock:NextPreviousBlockAction = new NextPreviousBlockAction(NextPreviousBlockAction.Next);
				nextBlock.className = event.NextBlock.@ClassName;
				handler.actions.addItem(nextBlock);
			}
			
			if (event.PreviousBlock != undefined) {
				var previousBlock:NextPreviousBlockAction = new NextPreviousBlockAction(NextPreviousBlockAction.Previous);
				previousBlock.className = event.PreviousBlock.@ClassName;
				handler.actions.addItem(previousBlock);
			}
			
			// SetLight action
			for each (var s:XML in event.SetLight) {
				var setLight:SetLightAction = new SetLightAction();
				if (s.@Light=="Red") {
					setLight.color=SetLightAction.Red;
				} else {
					setLight.color=SetLightAction.Green;
				}
				if (s.@Mode=="On") {
					setLight.mode = SetLightAction.On;
				} else {
					setLight.mode = SetLightAction.Off;
				}
				handler.actions.addItem(setLight);
			}
			
			// AdjustSpeenVolume actions
			if (event.SetVolume != undefined) {
				var setVolume:AdjustSpeedVolumeAction = new AdjustSpeedVolumeAction(AdjustSpeedVolumeAction.Volume);
				if (event.SetVolume.@relative == "up") {
					setVolume.relative = AdjustSpeedVolumeAction.Up;
				}
				if (event.SetVolume.@relative == "down") {
					setVolume.relative = AdjustSpeedVolumeAction.Down;
				}
				if (event.SetVolume.@relative == "normal") {
					setVolume.relative = AdjustSpeedVolumeAction.Normal;
				}
				handler.actions.addItem(setVolume);
			}
			
			if (event.SetSpeed != undefined) {
				var setSpeed:AdjustSpeedVolumeAction = new AdjustSpeedVolumeAction(AdjustSpeedVolumeAction.Speed);
				if (event.SetSpeed.@relative == "up") {
					setSpeed.relative = AdjustSpeedVolumeAction.Up;
				}
				if (event.SetSpeed.@relative == "down") {
					setSpeed.relative = AdjustSpeedVolumeAction.Down;
				}
				if (event.SetSpeed.@relative == "normal") {
					setSpeed.relative = AdjustSpeedVolumeAction.Normal;
				}
				handler.actions.addItem(setSpeed);
			}
			
			// SetUSBMode action
			if (event.SetUSBMode != undefined) {
				var setUSBMode:SetUSBModeAction = new SetUSBModeAction();
				if (event.SetUSBMode.@Active=="true") {
					setUSBMode.active = true;
				} else {
					setUSBMode.active = false;
				}
				if (event.SetUSBMode.@HostOrDevice=="host") {
					setUSBMode.mode = SetUSBModeAction.Host;
				} else {
					setUSBMode.mode = SetUSBModeAction.Device;
				}
				handler.actions.addItem(setUSBMode);
			}
		}

		private function parseSingleFile(data:XML):ContentFile {
			var f:ContentFile = new ContentFile();
			f.label = data.@Name;
			parseEventHandlers(f, data);
			return f;			
		}

		private function parseSinglePackage(data:XML):ContentPackage {
			var p:ContentPackage = new ContentPackage();
			p.label = data.@Name;
			p.precision = data.@Precision;
			p.version = data.@Version;
			parseEventHandlers(p, data);
			return p;			
		}
		
		private function addSubBlocksRecursively(container:SubBlockAppendable, data:XML):void {
			for each (var subBlock:XML in data.Block) {
				var b : ContentBlock = parseSingleBlock(subBlock);
				container.appendSubBlock(b);
				addSubBlocksRecursively(b, subBlock);
			}			
		}

		private function parseSingleBlock(block:XML):ContentBlock {
			var b : ContentBlock = new ContentBlock();
			b.label = block.@Name;
			if (block.@Start != undefined) {
				b.start = Number(block.@Start);
			}
			if (block.@End != undefined) {
				b.end = Number(block.@End);
			}		
			if (block.@Class != undefined) {
				b.className = block.@Class;
			}
			if (block.@Hyperlinked != undefined) {
				if (block.@Hyperlinked=="true") {
					b.hyperlinked = true;
				} else {
					b.hyperlinked = false;
				}
			}
			parseEventHandlers(b, block);
			return b;	
		}

	}
}