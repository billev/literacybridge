class ConditionalActionSet extends ActionSet {

    var test:IExpression;
	
	function ConditionalActionSet() {
		
		}
		
	function doAction(context:PlayerContext):Boolean {
		
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
		super.readXml(node);


		}

    }
	
