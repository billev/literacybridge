package org.literacybridge.authoring.schema {
	import org.literacybridge.authoring.schema.actions.Action;
	
	[Bindable]
	public class Hyperlink {
		public var parent:ContentFile;
		
		public var start:int;
		public var end:int;
		
		public var action:Action;
	}
}