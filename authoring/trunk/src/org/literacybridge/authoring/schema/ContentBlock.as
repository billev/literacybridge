package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentBlock extends ContentContainer {
		public var start:int;
		public var end:int;
		
		public function addSubBlock(block:ContentBlock):void {
			children.addItem(block);
		}
	}
}