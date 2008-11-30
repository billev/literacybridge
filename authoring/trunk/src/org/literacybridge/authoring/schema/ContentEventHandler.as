package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	[Bindable]
	public class ContentEventHandler extends EventHandler {
		public static const Start:int = 1;
		public static const End:int = 2;
		public static const Enter:int = 3;
		public static const Exit:int = 4;
		
		public var event:int;
		private static var eventList:ArrayCollection;
		
		public static function getEventList():ArrayCollection {
			if (eventList == null) {
				eventList = new ArrayCollection();
				eventList.addItem(Start);
				eventList.addItem(End);
				eventList.addItem(Enter);
				eventList.addItem(Exit);
			}
			
			return eventList;
		}

		
		public function ContentEventHandler(event:int) {
			this.event = event;
		}
	}
}