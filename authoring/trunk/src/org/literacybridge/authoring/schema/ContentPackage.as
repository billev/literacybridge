package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	
	[Bindable]
	public class ContentPackage extends ContentContainer {
		public var packageFile:File;
		
		public function addFile(audioFile:ContentFile):void {
			children.addItem(audioFile);
		}
	}
}