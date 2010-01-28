package org.literacybridge.authoring.events
{
	import flash.events.Event;
	
	import org.literacybridge.authoring.schema.ContentContainer;
	
	public class SelectContainerEvent extends Event
	{
		public static const SELECT:String = "selectContainerEvent";	
		public var container:ContentContainer;
		
      	public function SelectContainerEvent(type:String, container:ContentContainer, bubbles:Boolean=true, cancelable:Boolean=false)
        {
            super(type, bubbles, cancelable);
	        this.container = container;
	    }

	}
}