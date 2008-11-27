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
			var p:ContentPackage = parseSinglePackage(data);
			for each (var file:XML in data.File) {
				var f : ContentFile = new ContentFile();
				f.label = file.@Name;
				p.addFile(f);
				addSubBlocksRecursively(f, file);
			}
			return p;
		}

		private function parseSinglePackage(data:XML):ContentPackage {
			var p:ContentPackage = new ContentPackage();
			p.label = data.@Name;
			p.precision = data.@Precision;
			p.version = data.@Version;
			return p;			
		}
		
		private function addSubBlocksRecursively(container:SubBlockAppendable, data:XML):void {
			for each (var subBlock:XML in data.Block) {
				var b : ContentBlock = parseSingleBlock(subBlock);
				container.appendSubBlock(b);
				addSubBlocksRecursively(b, subBlock);
			}			
		}

		private function parseSingleBlock(block:XML):ContentBlock {
			var b : ContentBlock = new ContentBlock();
			b.label = block.@Name;
			if (block.@Start != undefined) {
				b.start = Number(block.@Start);
			}
			if (block.@End != undefined) {
				b.end = Number(block.@End);
			}		
			if (block.@Class != undefined) {
				b.className = block.@Class;
			}
			return b;	
		}

	}
}