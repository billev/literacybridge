package org.literacybridge.authoring.events
{
	import flash.events.Event;
	
	public class SchemaUpdateCommand
	{
		public function execute(event:Event):void {
			AuthoringTool.getInstance().schemaUpdated();
		}
	}
}