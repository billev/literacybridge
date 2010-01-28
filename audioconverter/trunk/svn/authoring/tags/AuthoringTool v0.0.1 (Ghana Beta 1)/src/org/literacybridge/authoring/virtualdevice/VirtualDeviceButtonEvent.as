package org.literacybridge.authoring.virtualdevice {
	import flash.events.Event;

	public class VirtualDeviceButtonEvent extends Event	{
		public static const TYPE:String = "virtualDeviceButtonEvent";
		
		public var button:int;
		public var action:int;
		
		public function VirtualDeviceButtonEvent(type:String, button:int, action:int, bubbles:Boolean=false, cancelable:Boolean=false) {
			super(TYPE, bubbles, cancelable);
			this.button = button;
			this.action = action;
		}
		
	}
}