class ActionChoose implements IAction {
	
	var actionSets:Array;
	
	function ActionChoose() {
		actionSets = new Array();
	}
	
	function doAction(context:PlayerContext) : Boolean {
		
		for (var i:Number = 0;i<actionSets.length;i++) {
			
			var currentActionSet:ActionSet = actionSets[i];
			
			if (currentActionSet.evaluate(context)) 
				return currentActionSet.doActions(context);

			}
	
		}
		
	function readXml(node:XMLNode) {
		
		this.actionSets = XmlParser.readChildren(node);
		
		}

	
	
}