class UnaryExpression implements IExpression {
	
	static var Not:Number = 1;
	
	var expressionOperation:Number;	
	var innerExpression:IExpression;
	
	public function readXml(node:XMLNode) {
		
		this.expressionOperation = BinaryExpression[node.nodeName];
		this.innerExpression = IExpression(XmlParser.readNode(node.childNodes[0]));
		
		}
	
	public function evaluate(context:PlayerContext):Number {
		
		var innerVal:Number = innerExpression.evaluate(context);
		
		switch (expressionOperation) {
			case Not:
				if (innerVal == 0)
					return 1;
				else
					return 0;
		}
		
		
		return 0;			
			
			
			
		
		
	}
	
	
}
