package org.literacybridge.authoring.events
{
	import org.literacybridge.authoring.events.SelectContainerEvent;
	
	public class SelectContainerCommand
	{
		public function execute(event:SelectContainerEvent):void {
			AuthoringTool.getInstance().selectContainer(event.container);
		}

	}
}