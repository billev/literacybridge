package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentBlock extends ContainerTimeSpan implements SubBlockAppendable {
		public var className:String = "#all";
		
		public function appendSubBlock(subBlock:ContentBlock):void {
			if (children == null) {
				children = new ArrayCollection();
			}
			children.addItem(subBlock);
		}
	}
}