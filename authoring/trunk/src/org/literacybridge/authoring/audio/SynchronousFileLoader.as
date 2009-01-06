package org.literacybridge.authoring.audio {
	import flash.events.Event;
	import flash.filesystem.File;
	import flash.media.Sound;
	import flash.net.URLRequest;
	
	import mx.collections.ArrayCollection;
	import mx.core.UIComponent;
	import mx.managers.PopUpManager;
	import mx.managers.SystemManager;
	
	import org.literacybridge.authoring.schema.ContentFile;
	
	public class SynchronousFileLoader extends UIComponent {
		private var files:ArrayCollection;
		private var index:int;
		private var file:ContentFile;
		private var dirUrl:String;
		private var sound:Sound;
		private var progress:ProgressIndicator;
		
		public function SynchronousFileLoader(files:ArrayCollection, dirUrl:String) {
			this.files = files;
			this.dirUrl = dirUrl;
		}
	
		public function load():void {
			progress = ProgressIndicator(PopUpManager.createPopUp(SystemManager.getSWFRoot(this), ProgressIndicator, true));
			PopUpManager.centerPopUp(progress);
			index = 0;
			loadFile();
		}
		
		private function loadFile():void {
			if (index == files.length) {	
				PopUpManager.removePopUp(progress);
				dispatchEvent(new Event(Event.COMPLETE));
			} else {
				file = files.getItemAt(index) as ContentFile;
				var filePath:String = dirUrl + File.separator + file.label + ".mp3";
				sound = new Sound();
				sound.load(new URLRequest(filePath));            
	            sound.addEventListener(Event.COMPLETE, onLoadingComplete);
	  		}
				
		}
		
		private function onLoadingComplete(event:Event):void {
			file.spectrum = AudioSpectrum.load(sound);
			
			sound.removeEventListener(Event.COMPLETE, onLoadingComplete);
			index++;
			var fileName:String = null;
			if (index < files.length) {
				fileName = (files.getItemAt(index) as ContentFile).label;
			}
			progress.setProgress(fileName, index, files.length);
			loadFile();
		}

	}
}