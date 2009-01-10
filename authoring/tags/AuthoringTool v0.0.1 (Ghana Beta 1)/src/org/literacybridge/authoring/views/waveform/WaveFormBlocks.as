package org.literacybridge.authoring.views.waveform
{
	import org.literacybridge.authoring.schema.ContainerTimeSpan;
	
	public class WaveFormBlocks extends WaveFormTimeSpan {		
		private static const curveSize:int = 5;
		
		
//		private function updateCursor():void {
//			for (var i:int = 0; i < displayedBlocks.length; i++) {
//				var b:ContentBlock = displayedBlocks.getItemAt(i) as ContentBlock;
//				var startPixel:int = waveFormState.getPixel(b.start);
//				var endPixel:int = waveFormState.getPixel(b.end);
//				
//				if (contentMouseX >= startPixel - resizeAnchorWidth && contentMouseX <= startPixel + resizeAnchorWidth) {
//					cursorID = cursorManager.setCursor(resizeCursorSymbol);
//					return;
//				} else if (contentMouseX >= endPixel - resizeAnchorWidth && contentMouseX <= endPixel + resizeAnchorWidth) {
//					cursorID = cursorManager.setCursor(resizeCursorSymbol);
//					return;
//				} else if (contentMouseX > startPixel && contentMouseX < endPixel) {
//					cursorID = cursorManager.setCursor(moveCursorSymbol);
//					return;
//				}
//			}
//			
//			cursorManager.removeAllCursors();
//			
//		}
		
		
		override protected function drawSpan(span:ContainerTimeSpan, selected:Boolean):void {
			if (selected) {
	            this.graphics.lineStyle( 2, 0xff0000 );
	            this.graphics.beginFill(0xff0000, 0.2);				
			} else {	 	
	            this.graphics.lineStyle( 2, 0x1d02be );
	            this.graphics.beginFill(0x1d02be, 0.2);
	  		}
            
            var x1:int = waveFormState.getPixel(span.start);
            var x2:int = waveFormState.getPixel(span.end);

			var drawLeftLine:Boolean;
			var drawRightLine:Boolean;

			var curveLeft:int = curveSize;
			var curveRight:int = curveSize;

			if (x1 >= 0) {
				drawLeftLine = true;
			} else {
				drawLeftLine = false;
				curveLeft = Math.max(0, Math.min(x1 + curveSize, curveSize));
				x1 = 0;
			}
			
			if (x2 <= this.width) {
				drawRightLine = true;
			} else {
				drawRightLine = false;
				curveRight = Math.max(0, Math.min(this.width - x2 + curveSize, curveSize));
				x2 = this.width;
			}

			if (x2 - curveRight < 0) {
				curveRight = x2;
			}

			if (x1 + curveLeft > this.width) {
				curveLeft = this.width - x1;
			}

			// draw round block clockwise
			// start at lower-left corner
			this.graphics.moveTo(x1 + curveLeft, this.height);
			this.graphics.curveTo(x1, this.height, x1, this.height - curveLeft);
			
			if (drawLeftLine) {
				// left line
				this.graphics.lineTo(x1, curveLeft);
			} else {
				this.graphics.lineStyle( 1 , 0, 0.2);
				this.graphics.lineTo(0, curveLeft);
				if (selected) {
					this.graphics.lineStyle( 2, 0xff0000 );
				} else {
					this.graphics.lineStyle( 2, 0x1d02be );
				}
			}
			// top-left curve
            this.graphics.curveTo(x1, 0, x1 + curveLeft, 0);
            // top line, left to right
            this.graphics.lineTo(x2 - curveRight, 0);
            // top-right curve
            this.graphics.curveTo(x2, 0, x2, curveRight);
			if (drawRightLine) {
	            // right line
	            this.graphics.lineTo(x2, this.height - curveRight);				
			} else {
				this.graphics.lineStyle( 1 , 0, 0.2);
				this.graphics.lineTo(x2, this.height - curveRight);
				if (selected) {
					this.graphics.lineStyle( 2, 0xff0000 );
				} else {
					this.graphics.lineStyle( 2, 0x1d02be );
				}
			}
            
            // lower-right curve
            this.graphics.curveTo(x2, this.height, x2-curveRight, this.height);
            // lower line
            this.graphics.lineTo(x1 + curveLeft, this.height);
            

            //this.graphics.drawRoundRect(x, 0, y-x, this.height, 10, 10);
            this.graphics.endFill();			
		}
		
	}
}