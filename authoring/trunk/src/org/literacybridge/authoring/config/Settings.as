package org.literacybridge.authoring.config {
	import flash.filesystem.*;
	
	import mx.resources.ResourceManager;
	
	import org.literacybridge.authoring.util.XMLUtils;
	
	[Bindable]
	final public class Settings {
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
		
		public var locale:String;
		public var showWizard:Boolean;
		public var lastOpenFile:File;
				
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
			if (settings.showWelcomeWizard == "true") {
				showWizard = true;
			} else {
				showWizard = false;
			}
			var path:String = settings.lastOpenFile;
			if (path!=null && path.length>0) {
				lastOpenFile = new File(settings.lastOpenFile);
			}
		}
		
		public function saveSettings():void {
			XMLUtils.writeXMLData(createXMLData(), settingsFile);
		}
		
		private function createXMLData():XML {
			var settings : XML = new XML();
			settings = <settings/>;
			settings.locale = locale;
			settings.showWelcomeWizard = showWizard;
			if (lastOpenFile != null) {
				settings.lastOpenFile = lastOpenFile.nativePath;
			}
			return settings;
		}
	}
}