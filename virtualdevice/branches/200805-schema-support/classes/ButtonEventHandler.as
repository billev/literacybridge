class ButtonEventHandler extends EventHandler {

    var button:Number;
    var action:Number;

	
	function handleEvent(context:PlayerContext, evt:ButtonEventArgs) : Boolean {
		
		
		if (evt.button != this.button || evt.action != this.action)
			return false;

		return super.handleEvent(context,evt);
	
		}
		
		
	function readXml(node:XMLNode) {
	
		this.button = ButtonType[node.attributes.Button];
		this.action = ButtonActionType[node.attributes.Action];
		
		super.readXml(node);
		}
		
    }
	
	

