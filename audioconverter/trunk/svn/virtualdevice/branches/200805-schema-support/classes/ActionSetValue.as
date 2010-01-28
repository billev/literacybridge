class ActionSetValue implements IAction {

	var name:String;
	var expression:IExpression;


	function doAction(context:PlayerContext) : Boolean {
		
		context.setVariable(this.name, expression.evaluate(context) );
		return true;
		
		}
		
	function readXml(node:XMLNode) {
		
		this.name = node.attributes["Name"];
		this.expression = IExpression(XmlParser.readFirstChild(node));
		
		}

	}
	