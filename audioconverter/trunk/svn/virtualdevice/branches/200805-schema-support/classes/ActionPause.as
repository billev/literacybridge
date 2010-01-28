class ActionPause implements IAction {

	
	function doAction(context:PlayerContext) : Boolean {
	
		context.pause();
		return true;
		}
		
	function readXml(node:XMLNode) {
		}

	}