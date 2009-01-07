package org.literacybridge.authoring.views.waveform
{
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.utils.Timer;
	
	import mx.collections.ArrayCollection;
	import mx.core.IToolTip;
	import mx.managers.CursorManager;
	import mx.managers.ToolTipManager;
	
	import org.literacybridge.authoring.schema.ContentBlock;
	
	public class WaveFormBlocks extends VisualizerBase {
        // Embed the cursor symbol.
        [Embed(source="/../assets/resize-cursor.png")]
        private var resizeCursorSymbol:Class;

        // Embed the cursor symbol.
        [Embed(source="/../assets/move-cursor.png")]
        private var moveCursorSymbol:Class;		
		
		private var cursorID:Number = 0;
		
		public var blocks:ArrayCollection;
		public var displayedBlocks:ArrayCollection = new ArrayCollection();
		private static const curveSize:int = 5;
		
		private static const resizeAnchorWidth:int = 3;
		
		private var mode:int = 0;
		private static const MODE_RESIZE_LEFT:int = 1;
		private static const MODE_RESIZE_RIGHT:int = 2;
		private static const MODE_MOVE:int = 3;
		private var mouseDownX:int;
		private var selectedBlock:ContentBlock;
		private var selectedBlockStart:int;
		private var selectedBlockEnd:int;
		
		private var autoScrollTimer:Timer = new Timer(5);
		private var autoScrollMode:int = 0;
		private static const MODE_AUTO_SCROLL_LEFT:int = 1;
		private static const MODE_AUTO_SCROLL_RIGHT:int = 2;
		private var autoScrollPositionX:int;
		private var scrollSpeed:int;
		private static const INITIAL_SCROLL_SPEED:int = 200;
		
		private var blockToolTip:IToolTip = null;
		
		public function init():void {
			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
			this.addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
			autoScrollTimer.addEventListener(TimerEvent.TIMER, autoScroll);
		}
		
		public function mouseDown(event:MouseEvent):void {
			if (contentMouseX < 0) return;
			mouseDownX = contentMouseX;
			// find block
			selectedBlock = null;
			for (var i:int = 0; i < displayedBlocks.length && selectedBlock == null; i++) {
				var b:ContentBlock = displayedBlocks.getItemAt(i) as ContentBlock;
				var startPixel:int = waveFormState.getPixel(b.start);
				var endPixel:int = waveFormState.getPixel(b.end);
				
				if (mouseDownX >= startPixel - resizeAnchorWidth && mouseDownX <= startPixel + resizeAnchorWidth) {
					mode = MODE_RESIZE_LEFT;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedBlock = b;
					selectedBlockStart = b.start;
					selectedBlockEnd = b.end;
					blockToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				} else if (mouseDownX >= endPixel - resizeAnchorWidth && mouseDownX <= endPixel + resizeAnchorWidth) {
					mode = MODE_RESIZE_RIGHT;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedBlock = b;
					selectedBlockStart = b.start;
					selectedBlockEnd = b.end;
					blockToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				} else if (mouseDownX > startPixel && mouseDownX < endPixel) {
					mode = MODE_MOVE;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedBlock = b;
					selectedBlockStart = b.start;
					selectedBlockEnd = b.end;
					blockToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				}
				updateToolTip();
			}
			
			if (selectedBlock != null) draw();
		}

		public function mouseOut(event:MouseEvent):void {
			if (autoScrollMode == 0) {
				mode = 0;
				if (blockToolTip != null) {
					ToolTipManager.destroyToolTip(blockToolTip);
					blockToolTip = null;
				}
			}				
		}

		public function mouseUp(event:MouseEvent):void {
			mode = 0;
			autoScrollMode = 0;
			if (blockToolTip != null) {
				ToolTipManager.destroyToolTip(blockToolTip);
				blockToolTip = null;
			}
		}
		
		private function updateCursor():void {
			for (var i:int = 0; i < displayedBlocks.length; i++) {
				var b:ContentBlock = displayedBlocks.getItemAt(i) as ContentBlock;
				var startPixel:int = waveFormState.getPixel(b.start);
				var endPixel:int = waveFormState.getPixel(b.end);
				
				if (contentMouseX >= startPixel - resizeAnchorWidth && contentMouseX <= startPixel + resizeAnchorWidth) {
					cursorID = cursorManager.setCursor(resizeCursorSymbol);
					return;
				} else if (contentMouseX >= endPixel - resizeAnchorWidth && contentMouseX <= endPixel + resizeAnchorWidth) {
					cursorID = cursorManager.setCursor(resizeCursorSymbol);
					return;
				} else if (contentMouseX > startPixel && contentMouseX < endPixel) {
					cursorID = cursorManager.setCursor(moveCursorSymbol);
					return;
				}
			}
			
			cursorManager.removeAllCursors();
			
		}

		public function mouseMove(event:MouseEvent):void {
			if (contentMouseX < 0) return;
			if (mode == 0) {
				if (blockToolTip != null) {
					ToolTipManager.destroyToolTip(blockToolTip);
					blockToolTip = null;
				}
				//updateCursor();
			}
			
			if (autoScrollMode == MODE_AUTO_SCROLL_LEFT) {
				if (contentMouseX > autoScrollPositionX + 5) {
					autoScrollMode = 0;
					autoScrollTimer.stop();					
				} else if (contentMouseX < autoScrollPositionX && contentMouseX >= 0) {
					//scrollSpeed += ((autoScrollPositionX - contentMouseX) / 5);
					autoScrollPositionX = contentMouseX;
				}
				return;
			} else if (autoScrollMode == MODE_AUTO_SCROLL_RIGHT) {
				if (contentMouseX < autoScrollPositionX - 5) {
					autoScrollMode = 0;
					autoScrollTimer.stop();					
				} else if (contentMouseX > autoScrollPositionX && contentMouseX <= this.width) {
					//scrollSpeed += ((contentMouseX - autoScrollPositionX) / 5);
					autoScrollPositionX = contentMouseX;
				}				
				return;
			}
			var delta:int =  (contentMouseX - mouseDownX) * waveFormState.interval;
			var newStart:int = selectedBlockStart + delta;
			var newEnd:int = selectedBlockEnd + delta;
			switch (mode) {
				case MODE_MOVE : {
					if (newStart >= 0 && newEnd <= waveFormState.length) { 
						selectedBlock.start = selectedBlockStart + delta;
						selectedBlock.end = selectedBlockEnd + delta;
						if (selectedBlock.end > waveFormState.end) {
							waveFormState.view.offset = selectedBlock.end - this.width * waveFormState.interval;
							autoScrollMode = MODE_AUTO_SCROLL_RIGHT;
							waveFormState.view.draw();
							autoScrollTimer.start();							
						} else 	if (selectedBlock.start < waveFormState.start) {
							waveFormState.view.offset = selectedBlock.start;
							autoScrollMode = MODE_AUTO_SCROLL_LEFT;
							waveFormState.view.draw();
							autoScrollTimer.start();
						} else {
							draw();
						}
					}
					break;
				}
				case MODE_RESIZE_LEFT : {
					if (newStart < selectedBlock.end) {
						selectedBlock.start = newStart;
						draw();
					}
					break;
				}
				case MODE_RESIZE_RIGHT : {
					if (newEnd > selectedBlock.start) {
						selectedBlock.end = newEnd;
						draw();
					}
					break;
				}

			}
			updateToolTip();
		}
		
		private function updateToolTip():void {
			if (blockToolTip != null && selectedBlock != null) {
				var x:int = waveFormState.getPixel(selectedBlock.start);
				
				blockToolTip.text = selectedBlock.start.toString() + " ms - " + selectedBlock.end.toString() + " ms";
				if (x + blockToolTip.width > this.width - 10) {
					x = this.width - 10 - blockToolTip.width;
				}
				var p:Point = localToGlobal(new Point(x)); 
				blockToolTip.x = p.x;
				
				blockToolTip.y = p.y + 40;
				trace(blockToolTip.x);
				trace("w" + this.width);
			}
		}

		private function autoScroll(event:Event):void {
			if (autoScrollMode == MODE_AUTO_SCROLL_LEFT) {
				if (selectedBlock.start == 0) {
					autoScrollMode = 0;
					autoScrollTimer.stop();
				}
				if (selectedBlock.start < scrollSpeed) {
					selectedBlock.end -= selectedBlock.start;
					selectedBlock.start = 0;
				} else {
					selectedBlock.start -= scrollSpeed;
					selectedBlock.end -= scrollSpeed;
				}
				selectedBlockStart = selectedBlock.start;
				selectedBlockEnd = selectedBlock.end;
				mouseDownX = contentMouseX;
				waveFormState.view.offset = selectedBlock.start;
				waveFormState.view.draw();
			} else if (autoScrollMode == MODE_AUTO_SCROLL_RIGHT) {
				if (selectedBlock.start == waveFormState.length) {
					autoScrollMode = 0;
					autoScrollTimer.stop();
				}
				if (selectedBlock.end > waveFormState.length-scrollSpeed) {
					selectedBlock.start += (waveFormState.length - selectedBlock.end);
					selectedBlock.end = waveFormState.length;
				} else {
					selectedBlock.start += scrollSpeed;
					selectedBlock.end += scrollSpeed;
				}
				selectedBlockStart = selectedBlock.start;
				selectedBlockEnd = selectedBlock.end;
				mouseDownX = contentMouseX;

				waveFormState.view.offset = selectedBlock.end - this.width * waveFormState.interval;
				waveFormState.view.draw();				
			}
			
			updateToolTip();
		}
		
		override public function draw():void {
			if (blocks == null) return; // nothing to do

            this.graphics.clear();
			this.displayedBlocks.removeAll();
			
			for (var i:int = 0; i < blocks.length; i++) {
				var b:ContentBlock = blocks.getItemAt(i) as ContentBlock
				if (waveFormState.isInDisplayRange(b.start, b.end)) {
					displayedBlocks.addItem(b);
					drawBlock(b, b == selectedBlock);
				}
			}
		}
		
		public function drawBlock(block:ContentBlock, selected:Boolean):void {
			if (selected) {
	            this.graphics.lineStyle( 2, 0xff0000 );
	            this.graphics.beginFill(0xff0000, 0.2);				
			} else {	 	
	            this.graphics.lineStyle( 2, 0x1d02be );
	            this.graphics.beginFill(0x1d02be, 0.2);
	  		}
            
            var x1:int = waveFormState.getPixel(block.start);
            var x2:int = waveFormState.getPixel(block.end);

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

			if (x2 - curveRight < 0) {
				curveRight = x2;
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
            // right line
            this.graphics.lineTo(x2, this.height - curveRight);
            // lower-right curve
            this.graphics.curveTo(x2, this.height, x2-curveRight, this.height);
            // lower line
            this.graphics.lineTo(x1 + curveLeft, this.height);
            

            //this.graphics.drawRoundRect(x, 0, y-x, this.height, 10, 10);
            this.graphics.endFill();			
		}
		
	}
}