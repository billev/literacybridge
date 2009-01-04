package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	import mx.collections.Sort;
	import mx.collections.SortField;
	
	[Bindable]
	public class ContentFile extends ContentContainer implements SubBlockAppendable {
		public var hyperlinks:ArrayCollection = new ArrayCollection();
		
		public function ContentFile() {
			// sort hyperlink list according to the hyperlink's start values
			var sort:Sort = new Sort();
			sort.fields = [new SortField("start")];
			hyperlinks.sort = sort;
			hyperlinks.refresh();	
		}

		
		public function appendSubBlock(subBlock:ContentBlock):void {
			if (children == null) {
				children = new ArrayCollection();
			}
			children.addItem(subBlock);
		}
	}
}