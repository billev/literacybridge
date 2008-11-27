package org.literacybridge.authoring.schema {
	import mx.collections.ArrayCollection;
	
	import org.literacybridge.authoring.util.XMLUtils;
	
	public class ContentXMLWriter {
		public function write(content:ContentPackage):void {
			var data:XML = createXMLData(content);
			XMLUtils.writeXMLData(data, content.packageFile);
		}
		
		private function createXMLData(content:ContentPackage):XML {
			var data : XML = new XML();
			data = <Package/>;
			data.@Name=content.label;
			data.@Precision = content.precision;
			data.@Version = content.version;
			appendFiles(data, content.children);
		    return data;
		}
		
		private function appendFiles(data:XML, files:ArrayCollection):void {
			for (var i:int=0; i<files.length; i++) {
				var file:ContentFile = files.getItemAt(i) as ContentFile;
				var fileData:XML = new XML();
				fileData = <File/>;
				fileData.@Name=file.label;
				appendBlocks(fileData, file.children);
				data.appendChild(fileData);
			}
		}

		private function appendBlocks(data:XML, blocks:ArrayCollection):void {
			for (var i:int=0; i<blocks.length; i++) {
				var block:ContentBlock = blocks.getItemAt(i) as ContentBlock;
				var blockData:XML = new XML();
				blockData = <Block/>;
				blockData.@Name=block.label;
				if (block.start >= 0) {
					blockData.@Start=block.start;
				}
				if (block.end >=0) {
					blockData.@End=block.end;
				}
				blockData.@Class=block.className;
				appendBlocks(blockData, block.children);
				data.appendChild(blockData);
			}
		}


	}
}