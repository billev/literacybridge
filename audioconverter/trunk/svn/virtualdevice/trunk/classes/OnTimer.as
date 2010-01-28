class EventHandler implements IXmlSerializable {

    var actionSets:Array;
	var parent:Container;
	
	function EventHandler() {
	
		this.actionSets = new Array();
		
		}
		
	function handleEvent(context:PlayerContext,e:EventArgs):Boolean {
		
		for(var i=0;i<actionSets.length;i++) {
			var curActionSet:ActionSet = this.actionSets[i];
			if (curActionSet.evaluate(context)) {
				return curActionSet.doAction(context);
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
			if (nName == "ActionSet") {
				var newActionSet:ActionSet = new ActionSet();
				newActionSet.readXml(cNode);
				actionSets.push(newActionSet);
				}
			}
		
		}
	
    
    }