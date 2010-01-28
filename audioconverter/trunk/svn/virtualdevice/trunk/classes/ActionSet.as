class ActionSet implements ICondition, IAction {

    var conditions:Array;
    var actions:Array;
	
	function ActionSet() {
	
		this.conditions = new Array();
		this.actions = new Array();
		
		}
		
	function evaluate(context:PlayerContext):Boolean {
	
		for (var i=0;i < conditions.length;i++)
			if (!conditions[i].evaluate(context))
				return false;
			
		return true;

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
		var childNodes = node.childNodes;
		var childCount = childNodes.length;
		for (var i=0;i < childCount;i++) {

			var childObject:Object;
			var cNode = node.childNodes[i];
			var nName = cNode.nodeName;
			if (nName == "FlagTest") {
				childObject = new FlagTest();
				}
			if (nName == "CounterTest") {
				childObject = new CounterTest();
				}
			if (nName == "SetLight") {
				childObject = new ActionSetLight();
				}
			if (nName == "Show") {
				childObject = new ActionShow();
				}
			if (nName == "Play") {
				childObject = new ActionPlay();
				}
			if (nName == "Pause") {
				childObject = new ActionPause();
				}
			if (nName == "Goto") {
				childObject = new ActionGoto();
				}
			if (nName == "SetFlag") {
				childObject = new ActionSetFlag();
				}
			if (nName == "SetVolume") {
				childObject = new ActionSetVolume();
				}
			if (nName == "PushStack") {
				childObject = new ActionPushStack();
				}
				
			if (childObject instanceof IXmlSerializable)
				childObject.readXml(cNode);
				
			if (childObject instanceof IAction)
				actions.push(childObject);
				
			if (childObject instanceof ICondition)
				conditions.push(childObject);
			

			}
			
		}
		
		
	

    }
