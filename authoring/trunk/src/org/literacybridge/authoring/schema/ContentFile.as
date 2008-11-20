package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentFile extends ContentContainer {
		public function addBlock(block:ContentBlock):void {
			children.addItem(block);
		}
	}
}