class NamedActionSet extends ActionSet  {

    var actions:Array;
	var name:String;
	
	function NamedActionSet() {
	
		super();
		this.name = null;
		
		}
		
	
	function readXml(node:XMLNode) {
	
		
		// -- Parse children
		this.name = node.attributes.Name;
		super.readXml(node);

		}

    }
