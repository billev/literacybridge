package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentContainer {
		public var label:String;
		public var children:ArrayCollection = new ArrayCollection();
		public var eventHandlers:ArrayCollection = new ArrayCollection();		
	}
}