class File extends Container {

	var href = "";

    function get startPosition():Position {

		return new Position(this.href,0);

		}

	function get first():Container {
		return Container(this);
		}
		
	function get last():Container {
		return Container(this);
		}
		
	function findChild(pos:Position) : Container {
	
		if (pos.url != this.href) {
			return null;
			}
			
		return super.findChild(pos);
		
		}

	}