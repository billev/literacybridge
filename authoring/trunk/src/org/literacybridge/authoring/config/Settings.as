package org.literacybridge.authoring.config {
	import flash.events.EventDispatcher;
	import flash.filesystem.*;
	
	import mx.resources.ResourceManager;
	
	import org.literacybridge.authoring.util.XMLUtils;
	
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
			XMLUtils.writeXMLData(createXMLData(), settingsFile);
		}
		
		private function createXMLData():XML {
			var settings : XML = new XML();
			settings = <settings/>;
			settings.locale = locale;
			return settings;
		}
	}
}