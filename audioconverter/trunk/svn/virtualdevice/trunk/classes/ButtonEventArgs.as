class ButtonEventArgs extends EventArgs {

    var button;
    var action;
	
	function ButtonEventArgs(btn,act) {
	
		this.button = btn;
		this.action = act;
		}


    }
