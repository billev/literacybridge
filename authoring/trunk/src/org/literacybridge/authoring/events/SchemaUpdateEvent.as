package org.literacybridge.authoring.events
{
	import flash.events.Event;
	
	public class SchemaUpdateEvent extends Event
   {
      public static const UPDATE: String = "schemaUpdateEvent";
      
      public function SchemaUpdateEvent(type:String=UPDATE, bubbles:Boolean=true, cancelable:Boolean=false)
      {
         super(type, bubbles, cancelable);
      }
      
   }
}