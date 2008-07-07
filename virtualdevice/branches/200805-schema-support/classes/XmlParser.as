class XmlParser {
	
	static function readChildren(node:XMLNode):Array {
		
		var outElements:Array = new Array();
		var childNodes = node.childNodes;
		for (var i:Number = 0;i<childNodes.length;i++) {
			var currentElement:IXmlSerializable = readNode(childNodes[i]);
			if (currentElement != null)
				outElements.push(currentElement);
		}
		
		return outElements;
		
	}
	
	static function readFirstChild(parent:XMLNode):IXmlSerializable {

		var childNodes = parent.childNodes;
		for (var i:Number = 0;i<childNodes.length;i++) {
			var curNode:XMLNode = childNodes[i];
			if (curNode.nodeType == 1)
				return readNode(curNode);
		}
		return null;

		
	}
	
	static function readNode( node:XMLNode ):IXmlSerializable {

		var item:IXmlSerializable = null;
		
			switch ( String(node.nodeName) ) {

			// -- Containers
			case "Device":
				item = new Device();
				break;
			case "Package":
				item = new Package();
				break;
			case "Folder":
				item = new Folder();
				break;
			case "File":
				item = new File();
				break;
			case "Block":
				item = new Block();
				break;
				
			// -- Event Handlers
			case "OnStart":
			case "OnFinish":
			case "OnLeave":
			case "OnEnter":
				item = new EventHandler();
				break;
			case "OnButton":
				item = new ButtonEventHandler();
				break;
			
			// -- Actions
			case "Play":
				item = new ActionPlay();
				break;
			case "Goto":
				item = new ActionGoto();
				break;
			case "Pause":
				item = new ActionPause();
				break;
			case "Exit":
				item = new ActionExit();
				break;
			case "Choose":
				item = new ActionChoose();
				break;
			case "Call":
				item = new ActionCall();
				break;
			case "SetValue":
				item = new ActionSetValue();
				break;
			case "PushStack":
				item = new ActionPushStack();
				break;
			case "SetLight":
				item = new ActionSetLight();
				break;
				
			//-- Action Supporting Elements
			case "ActionSet":
				item = new NamedActionSet();
				break;
			case "Otherwise":
				item = new ActionSet();
				break;
			case "When":
				item = new ConditionalActionSet();
				break;
				

			// -- Expressions
			case "Eq":
			case "Neq":
			case "Gt":
			case "Lt":
			case "Geq":
			case "Leq":
			case "And":
			case "Or":
			case "Add":
			case "Sub":
				item = new BinaryExpression();
				break;
			case "Not":
				item = new UnaryExpression();
				break;
			case "Literal":
				item = new Literal();
				break;
			case "GetValue":
				item = new GetValue();
				break;
		}
				
		if (item != null)
			item.readXml(node);
		
		return item;
			
		
	}
	
}