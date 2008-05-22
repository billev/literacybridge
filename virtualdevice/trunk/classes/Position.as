
class Position {

	var url:String;
	var offset:Number;


	function Position(fileUrl:String,fileOffset:Number) {

		url = fileUrl;
		offset = fileOffset;

		}

	function clone():Position {

		return new Position(this.url,this.offset);

		}
		
	function difference(fromPosition:Position):Number {
		
		if (this.url != fromPosition.url)
			return Number.NaN;
		else 
			return this.offset - fromPosition.offset;
	
		}
		
	function toString():String {
		
		return this.url + " " + this.offset;
		
		}

	}