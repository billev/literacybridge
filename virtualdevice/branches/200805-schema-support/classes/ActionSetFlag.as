class ActionSetFlag implements IAction {

	var flag;
    var value;

	function doAction(context:PlayerContext):Boolean {
		
		_root.addLog("Action: Set Flag " + this.flag + "=" + this.value);
		context.setFlag(this.flag,this.value);
		
		return true;
	
		}
		
	function readXml(node:XMLNode) {
	
		this.flag = node.attributes.Flag;
		this.value = (node.attributes.Value.toLowerCase() == "true")
		
		}
		
		
	
    }
	