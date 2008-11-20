package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentPackage extends ContentContainer {
		public function addFile(file:ContentFile):void {
			children.addItem(file);
		}
	}
}