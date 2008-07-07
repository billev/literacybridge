class NamedActionSet extends ActionSet  {

    var actions:Array;
	var name:String;
	
	function NamedActionSet() : base() {
	
		this.name = null;
		
		}
		
	
	function readXml(node:XMLNode) {
	
		
		// -- Parse children
		this.name = node.attributes["name"];
		super.readXml(node);

		}

    }
