class ActionShow implements IAction {

	var append:Boolean = false;
	var innerXml:String = "";
	
	function doAction(context:PlayerContext) : Boolean {
	
		_root.ShowArea = this.innerXml;
		return true;
	
		}
		
		
	function readXml(node:XMLNode) {

		var atts = node.attributes;
		this.append = atts["Append"];
		
		var childNodes = node.childNodes;
		var childCount = childNodes.length;
		for (var i=0;i<childCount;i++) {
			var cNode = node.childNodes[i];
			this.innerXml = this.innerXml + cNode.toString();
		}
		
	}
		

}