package org.literacybridge.authoring.util {
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	
	public class XMLUtils {
		public static function writeXMLData(data : XML, file:File):void {
			var outputString:String = '<?xml version="1.0" encoding="utf-8"?>\n';
			outputString += data.toXMLString();
			outputString = outputString.replace(/\n/g, File.lineEnding);
			var stream:FileStream = new FileStream();
			stream.open(file, FileMode.WRITE);
			stream.writeUTFBytes(outputString);
			stream.close();
		}

	}
}