class Literal implements IExpression {
	
	var literalValue:Number = 0;
	
	function evaluate(context:PlayerContext):Number {
		return this.literalValue;
	}
	
	function readXml(node:XMLNode) {
		
		this.literalValue = Number(node.firstChild.nodeValue);
	}
	
}