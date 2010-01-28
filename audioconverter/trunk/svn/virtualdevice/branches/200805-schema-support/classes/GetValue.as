class GetValue implements IExpression {
	
	var name:String;
	
	function evaluate(context:PlayerContext):Number {

		return context.getVariable(this.name);

	}
	
	function readXml(node:XMLNode) {
		
		this.name = node.attributes.Name;
	}
	
}