class ActionGoto implements IAction {

	var location:Location;
	var tryPopStack:Boolean;

	function ActionGoto() {
		tryPopStack = false;
		}
		
	function doAction(context:PlayerContext) : Boolean {
	
		_root.addLog("Action: Goto ");
		if (tryPopStack) {
			if (context.popStack())
				return false;
			}
		
		if (location != null) {
			var pos:Position = context.evaluateLocation(location)
			if (pos != null) {
				context.position = pos;
				return false;
				}
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
			if (nName == "PopStack") {
				this.tryPopStack = true;
				}
			}
		
		}

	}