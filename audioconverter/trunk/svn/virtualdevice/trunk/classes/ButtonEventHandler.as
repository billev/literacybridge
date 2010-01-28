class ButtonEventHandler extends EventHandler {

    var button;
    var action;

	function readXml(node:XMLNode) {
	
		this.button = ButtonType[node.attributes.Button];
		this.action = ButtonActionType[node.attributes.Action];
		
		super.readXml(node);
		}
	
	function handleEvent(context:PlayerContext, evt:ButtonEventArgs) : Boolean {
		
		
		if (evt.button != this.button || evt.action != this.action)
			return false;
			
		_root.addLog("Button: OnButton Match " + evt.button + "=" + this.button + " " + evt.action + "=" + this.action);

			
		for(var i=0;i < actionSets.length;i++) {
			var curActionSet:ActionSet = this.actionSets[i];
			if (curActionSet.evaluate(context)) {
				curActionSet.doAction(context);
				return true;
				}
			}
			
		return false;
	
		}
    }
	
	

