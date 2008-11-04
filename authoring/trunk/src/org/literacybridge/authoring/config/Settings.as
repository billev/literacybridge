package org.literacybridge.authoring.config {
	import flash.events.EventDispatcher;
	import flash.filesystem.*;
	
	import mx.resources.ResourceManager;
	
	final public class Settings extends EventDispatcher {
		private static var instance : Settings = new Settings();
		
		public static function getInstance():Settings {
			return instance;
		}				

		public function Settings() {
			settingsFile = File.applicationStorageDirectory;
			settingsFile = settingsFile.resolvePath("settings.xml"); 
		}

		private var settingsFile:File;   
		private var stream:FileStream;
		
		private var locale:String;
		
		public function setLocale(locale:String) : void {
			this.locale = locale;
		}
		
		public function getLocale() : String {
			return this.locale;
		}
		
		public function loadSettings():void {
			stream = new FileStream();
			if (settingsFile.exists) {
    			stream.open(settingsFile, FileMode.READ);
			    var settings : XML = XML(stream.readUTFBytes(stream.bytesAvailable));
			    stream.close();
			    processXML(settings);
			} else {
				initDefaults();
			    saveSettings();
			}
		}
		
		private function initDefaults():void {
			this.locale = String(ResourceManager.getInstance().localeChain[0]);
		}
		
		private function processXML(settings : XML):void {
			locale = settings.locale;
		}
		
		public function saveSettings():void {
			writeXMLData(createXMLData());
		}
		
		private function createXMLData():XML {
			var settings : XML = new XML();
			settings = <settings/>;
			settings.locale = locale;
			return settings;
		}
		
		private function writeXMLData(settings : XML):void {
			var outputString:String = '<?xml version="1.0" encoding="utf-8"?>\n';
			outputString += settings.toXMLString();
			outputString = outputString.replace(/\n/g, File.lineEnding);
			stream = new FileStream();
			stream.open(settingsFile, FileMode.WRITE);
			stream.writeUTFBytes(outputString);
			stream.close();
		}

 
	}
}