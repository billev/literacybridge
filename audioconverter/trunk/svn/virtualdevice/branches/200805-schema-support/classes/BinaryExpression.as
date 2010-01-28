class BinaryExpression implements IExpression {
	
	static var Eq:Number = 1;
	static var Neq:Number = 2;
	static var Gt:Number = 3;
	static var Lt:Number = 4;
	static var Geq:Number = 5;
	static var Leq:Number = 6;
	static var And:Number = 7;
	static var Or:Number = 8;
	static var Add:Number = 9;
	static var Sub:Number = 10;
	
	
	var expressionOperation:Number;	
	var leftExpression:IExpression;
	var rightExpression:IExpression;
	
	public function readXml(node:XMLNode) {
		
		this.expressionOperation = BinaryExpression[node.nodeName];
		this.leftExpression = IExpression(XmlParser.readNode(node.childNodes[0]));
		this.rightExpression = IExpression(XmlParser.readNode(node.childNodes[1]));
		
		trace("Read Binary Expression " + node.nodeName + " " + this.expressionOperation);
		
		}
	
	public function evaluate(context:PlayerContext):Number {
		
		var leftVal = leftExpression.evaluate(context);
		var rightVal = rightExpression.evaluate(context);
		
		switch (expressionOperation) {
			case Eq:
				if (leftVal == rightVal)
					return 1;
				else
					return 0;
			case Neq:
				if (leftVal != rightVal)
					return 1;
				else
					return 0;
			case Geq:
				if (leftVal >= rightVal)
					return 1;
				else
					return 0;
			case Leq:
				if (leftVal <= rightVal)
					return 1;
				else
					return 0;
			case And:
				return leftVal & rightVal;
			case Or:
				return leftVal | rightVal;
			case Add:
				return leftVal + rightVal;
			case Sub:
				return leftVal - rightVal;
		}
		
		
		return 0;			
			
			
			
		
		
	}
	
	
}
