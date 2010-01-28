class ButtonEventArgs extends EventArgs {

    var button:Number;
    var action:Number;
	
	function ButtonEventArgs(btn:Number,act:Number) {
	
		this.button = btn;
		this.action = act;
		
		}


    }
