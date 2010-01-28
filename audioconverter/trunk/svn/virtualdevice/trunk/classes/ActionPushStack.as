class ActionPushStack implements IAction {

	var location:Location;

	function doAction(context:PlayerContext) : Boolean {
	
		_root.addLog("Action: PushStack ");
		if (location != null) {
			var pos = context.evaluateLocation(location)
			context.pushStack(pos);
			}
		return true;
		}
		
	function readXml(node:XMLNode) {
		
		// -- Parse children
		var childNodes = node.childNodes;
		var childCount = childNodes.length;
		for (var i=0;i<childCount;i++) {
			var cNode = node.childNodes[i];
			var nName = cNode.nodeName;
			if (nName == "Location") {
				this.location = new Location();
				this.location.readXml(cNode);
				}
			}
		
		}

	}