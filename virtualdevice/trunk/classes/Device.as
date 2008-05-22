class Device extends Container {

	var flags:Object;
	var containers:Object;

	function Device() {
		flags = new Object();
		containers = new Object();
		}

	function get scopeName():String {
		return "#device";
		}	
		
	}