package org.literacybridge.authoring.schema {
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	
	public class ContentXMLParser {
		public function parse(file:File):ContentPackage {
			var stream:FileStream = new FileStream();
			stream.open(file, FileMode.READ);
		    var data : XML = XML(stream.readUTFBytes(stream.bytesAvailable));
		    stream.close();
		    
		    var content:ContentPackage = parseData(data);
		    content.packageFile = file;
		    return content;
		}
		
		private function parseData(data:XML):ContentPackage {
			var p:ContentPackage = new ContentPackage();
			p.label = data.@name;
			for each (var file:XML in data.File) {
				var f : ContentFile = new ContentFile();
				f.label = file.@name;
				p.addFile(f);
				parseFile(f, file);
			}
			return p;
		}

		private function parseFile(contentFile:ContentFile, file:XML):void {
			for each (var block:XML in file.Block) {
				var b : ContentBlock = parseSingleBlock(block);
				contentFile.addBlock(b);
				parseBlock(b, block);
			}
		}

		private function parseBlock(contentBlock:ContentBlock, block:XML):void {
			for each (var subBlock:XML in block.Block) {
				var b : ContentBlock = parseSingleBlock(subBlock);
				contentBlock.addSubBlock(b);
				parseBlock(b, subBlock);
			}
		}

		private function parseSingleBlock(block:XML):ContentBlock {
			var b : ContentBlock = new ContentBlock();
			b.label = block.@name;
			if (block.@start != undefined) {
				b.start = Number(block.@start);
			}
			if (block.@end != undefined) {
				b.end = Number(block.@end);
			}		
			return b;	
		}

	}
}