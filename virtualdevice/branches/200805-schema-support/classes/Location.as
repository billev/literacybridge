class Location implements IXmlSerializable {

	var ref:String;
	var className:String;
	var target:Number;
	var offset:Number;

	function readXml(node:XMLNode) {
	
		with (node.attributes) {
		
			if (Target != null)
				this.target = TargetType[node.attributes.Target];
			else
				this.target = TargetType.Current;
				
			this.className = ClassName;

			if (Offset != null)
				this.offset = Offset;
			else
				this.offset = 0;
				
			if (Ref != null)
				this.ref = Ref;
			
			this.className = ClassName;
			
			}
		
		}
	
	}
	

	