package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentPackage extends ContentContainer {
		public var packageFile:File;
		public var version:String;
		public var precision:String;
		
		public function addFile(audioFile:ContentFile):void {
			if (children == null) {
				children = new ArrayCollection();
			}

			children.addItem(audioFile);
		}
	}
}