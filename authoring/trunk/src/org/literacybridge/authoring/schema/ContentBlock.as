package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentBlock extends ContentContainer implements SubBlockAppendable {
		public var start:int = -1;
		public var end:int = -1;
		public var className:String = "#all";
		
		public function appendSubBlock(subBlock:ContentBlock):void {
			if (children == null) {
				children = new ArrayCollection();
			}
			children.addItem(subBlock);
		}
	}
}