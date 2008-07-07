class EventHandler implements IXmlSerializable {

    var actionSet:ActionSet;
	var parent:Container;
	
	function EventHandler() {
		this.actionSet = new ActionSet();
		}
		
	function handleEvent(context:PlayerContext,e:EventArgs):Boolean {
		
		return this.actionSet.doActions(context);
		
		}
	
	function readXml(node:XMLNode) {
	
		// -- Parse children
		this.actionSet.readXml(node);
		
		}
	
    
    }