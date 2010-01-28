package org.literacybridge.authoring.events
{
	import flash.events.Event;
	
	import org.literacybridge.authoring.schema.actions.Action;

	public class RemoveActionEvent extends Event
	{
		public static const REMOVE_ACTION_EVENT:String = "removeActionEvent";
		
		public var action:Action;
		
		public function RemoveActionEvent(action:Action, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(REMOVE_ACTION_EVENT, bubbles, cancelable);
			this.action = action;
		}
		
		override public function clone():Event {
			 return new RemoveActionEvent(action, bubbles, cancelable);
		}

		
	}
}