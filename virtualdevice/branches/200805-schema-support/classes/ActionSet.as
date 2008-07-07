class ActionSet implements IXmlSerializable {

    var actions:Array;
	var name:String;
	
	function ActionSet() {
	
		this.name = null;
		this.actions = new Array();
		
		}
		
	function evaluate(context:PlayerContext):Boolean {
	
		return true;

		}
		
	function doActions(context:PlayerContext):Boolean {
		
		for (var i=0;i < actions.length;i++) {
			var currentAction : IAction;
			currentAction = actions[i];
			if (!currentAction.doAction(context))
				return false;
			}
			return true;
	
		}
	
	function readXml(node:XMLNode) {
	
		
		// -- Parse children
		this.name = node.attributes["name"];
		this.actions = XmlParser.readChildren(node);

		}

    }
	
