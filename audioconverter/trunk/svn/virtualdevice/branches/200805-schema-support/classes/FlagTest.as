class FlagTest implements ICondition, IXmlSerializable {

	var flag = null;
    var testVal = null;

	function readXml(node:XMLNode) {
	
		this.flag = node.attributes.Flag;
		this.testVal = (node.attributes.Test == "IsTrue")
		
		}
		
	function evaluate(context:PlayerContext):Boolean {
		
		var outVal:Boolean = (context.getFlag(this.flag) == this.testVal);
		_root.addLog("Flag: eval " + this.flag + "," + this.testVal + " " +  outVal);
		return outVal;
	
		}
	
    }
	