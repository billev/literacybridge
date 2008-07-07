class ActionPause implements IAction {

	
	function doAction(context:PlayerContext) : Boolean {
	
		_root.addLog("Action: Pause");
		context.pause();
		return true;
		}
		
	function readXml(node:XMLNode) {
		}

	}