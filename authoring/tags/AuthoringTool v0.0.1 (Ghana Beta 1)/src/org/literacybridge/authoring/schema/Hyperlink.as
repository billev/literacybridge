package org.literacybridge.authoring.schema {
	import org.literacybridge.authoring.schema.actions.Action;
	
	[Bindable]
	public class Hyperlink extends ContainerTimeSpan {
		public var action:Action;
	}
}