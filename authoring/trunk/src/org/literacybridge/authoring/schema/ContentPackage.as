package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	
	[Bindable]
	public class ContentPackage extends ContentContainer {
		public var packageFile:File;
		public var version:String;
		public var precision:String;
		
		public function addFile(audioFile:ContentFile):void {
			children.addItem(audioFile);
		}
	}
}