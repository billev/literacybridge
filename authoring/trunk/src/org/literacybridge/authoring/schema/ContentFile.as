package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentFile extends ContentContainer implements SubBlockAppendable {
		public function appendSubBlock(subBlock:ContentBlock):void {
			children.addItem(subBlock);
		}
	}
}