package org.literacybridge.authoring.schema {
	[Bindable]
	public class ContentEventHandler extends EventHandler {
		public static const Start:int = 1;
		public static const End:int = 2;
		public static const Enter:int = 3;
		public static const Exit:int = 4;
		
		public var event:int;
		
		public function ContentEventHandler(event:int) {
			this.event = event;
		}
	}
}