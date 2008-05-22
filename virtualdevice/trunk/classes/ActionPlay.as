class ActionPlay implements IAction {

	
	function doAction(context:PlayerContext) : Boolean {
	
		_root.addLog("Action: Play");
		context.play();
		return true;
		}
		
	function readXml(node:XMLNode) {
		}

	}