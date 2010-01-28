class ActionSetLight implements IAction {

	var light;
	var mode;
	
	function doAction(context:PlayerContext) : Boolean {
	
		_root.addLog("Action: SetLight");
		context.setLight(light,mode);
		return true;
		}
		
		
	function readXml(node:XMLNode) {

		var atts = node.attributes;
		this.light = LightColorType[atts.Light];
		this.mode = LightModeType[atts.Mode];

		}
		

	}