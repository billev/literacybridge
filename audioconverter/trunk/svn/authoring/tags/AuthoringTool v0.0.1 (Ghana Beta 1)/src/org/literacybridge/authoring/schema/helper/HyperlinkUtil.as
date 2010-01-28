package org.literacybridge.authoring.schema.helper
{
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.schema.Hyperlink;
	
	public class HyperlinkUtil
	{
		public function HyperlinkUtil() { /* nothing */ }


		public static function validateRanges(hyperlinkList:ArrayCollection):Boolean {
			if (hyperlinkList.length <= 1) return true;
			
			var clashFound:Boolean = false;
			// as the start values are sorted, we must compare the end value of the current hyperlink with the previous one...
			for (var i:int=1; i<hyperlinkList.length && !clashFound; ++i) {
				// previous hyperlink
				var prevHyperlink:Hyperlink = hyperlinkList.getItemAt(i-1) as Hyperlink;
				var currHyperlink:Hyperlink = hyperlinkList.getItemAt(i) as Hyperlink;
				
				if (prevHyperlink.end >= currHyperlink.start) {
					clashFound = true;
					// clash found
					var nameVisitor:HyperlinkNameVisitor = new HyperlinkNameVisitor();
					var nameOld:String = 'Unknown hyperlink';
					var nameNew:String = 'Unknown hyperlink';
					try {
						nameOld = nameVisitor.getActionName(currHyperlink.action);						
					} catch (e:Error) {
						trace ("Fatal error: Hyperlink without name in hyperlink list found!");
					}
					
					try {
						nameNew = nameVisitor.getActionName(prevHyperlink.action);						
					} catch (e:Error) {
						trace ("Fatal error: Hyperlink without name in hyperlink list found!");
					}
					
					trace ("Hyperlink: Range clash found.\n\t '" 
									+ nameOld + "' start=" + currHyperlink.start + " ; end=" + currHyperlink.end + " and '" 
									+ nameNew + "' start=" + prevHyperlink.start + " ; end=" + prevHyperlink.end);
				}
			}
			
			return !clashFound;
		}
	}
}













