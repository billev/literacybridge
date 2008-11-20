package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentBlock extends ContentContainer {
		public var start:int = -1;
		public var end:int = -1;
		
		public function addSubBlock(block:ContentBlock):void {
			children.addItem(block);
		}
	}
}