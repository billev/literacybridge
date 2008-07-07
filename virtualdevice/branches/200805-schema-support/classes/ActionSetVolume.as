class ActionSetVolume implements IAction {

	var level:Number;
    var relative:Boolean;

	function doAction(context:PlayerContext):Boolean {
		
		_root.addLog("Action: Set Volume");
		if (relative) {
			context.volume = context.volume + level;
		} else {
			context.volume = this.level;
			}

		return true;
	
		}
		
	function readXml(node:XMLNode) {
	
		this.level = Number(node.attributes.Level);
		this.relative = (node.attributes.Value.toLowerCase() != "false")
		
		}

	
    }
	