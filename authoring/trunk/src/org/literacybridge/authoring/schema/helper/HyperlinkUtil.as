package org.literacybridge.authoring.schema.helper
{
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.schema.Hyperlink;
	
	public class HyperlinkUtil
	{
		public function HyperlinkUtil() { /* nothing */ }


		public static function checkForRangeClashes(hyperlinkList:ArrayCollection, hyperlink:Hyperlink):Boolean {
			var i:int = 0;
			while (i < hyperlinkList.length) {
				var curHyperlink:Hyperlink = hyperlinkList.getItemAt(i) as Hyperlink;
				
				if (curHyperlink.start <= hyperlink.start && hyperlink.end <= curHyperlink.end) {
					
					// clash found
					var nameVisitor:HyperlinkNameVisitor = new HyperlinkNameVisitor();
					var nameOld:String = 'Unknown hyperlink';
					var nameNew:String = 'Unknown hyperlink';
					try {
						nameOld = nameVisitor.getActionName(curHyperlink.action);						
					} catch (e:Error) {
						trace ("Fatal error: Hyperlink without name in hyperlink list found!");
					}
					
					try {
						nameNew = nameVisitor.getActionName(hyperlink.action);						
					} catch (e:Error) {
						trace ("New hyperlink has no name");
					}
					
					trace ("Hyperlink: Range clash found.\n\t '" 
									+ nameOld + "' start=" + curHyperlink.start + " ; end=" + curHyperlink.end + " and '" 
									+ nameNew + "' start=" + hyperlink.start + " ; end=" + hyperlink.end);

					
					return false;
				}
				++i;
			}		
			return true;
		}
	}
}