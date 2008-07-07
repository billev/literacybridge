class CounterTest implements ICondition, IXmlSerializable {

	var counter;
    var comparisonType;
	var comparisonVal;

	function readXml(node:XMLNode) {
	
		this.counter = node.attributes.Counter;
		this.comparisonType = CounterComparisonType[node.attributes.Comparison];
		this.comparisonVal = node.attributes.Value;
		
		
		}
		
	function evaluate(context:PlayerContext):Boolean {
		

		var workVal:Number = context.getCounter(this.counter);
		var testVal = Number(comparisonVal);

		_root.addLog("Counter Test " + counter + " (" + workVal + ") " + comparisonType + " " + testVal);		
		switch (this.comparisonType) {
			case CounterComparisonType.Equal:
				return workVal == testVal;
			case CounterComparisonType.LessThan:
				return workVal < testVal;
			case CounterComparisonType.LessThanOrEqual:
				return workVal <= testVal;
			case CounterComparisonType.GreaterThan:
				return workVal > testVal;
			case CounterComparisonType.GreaterThanOrEqual:
				return workVal >= testVal;
			case CounterComparisonType.NotEqual:
				return workVal <> testVal;
		
			}
			
		return true;
	
		}
	
    }
	