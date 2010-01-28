package org.literacybridge.authoring.views.waveform {
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.utils.Timer;
	
	import mx.collections.ArrayCollection;
	import mx.core.IToolTip;
	import mx.managers.ToolTipManager;
	
	import org.literacybridge.authoring.events.SelectContainerEvent;
	import org.literacybridge.authoring.schema.ContainerTimeSpan;
	
	public class WaveFormTimeSpan extends VisualizerBase {
		private static const resizeAnchorWidth:int = 3;
		
		public var spans:ArrayCollection;
		
		private var mode:int = 0;
		private static const MODE_RESIZE_LEFT:int = 1;
		private static const MODE_RESIZE_RIGHT:int = 2;
		private static const MODE_MOVE:int = 3;
		private var mouseDownX:int;
		private var selectedSpan:ContainerTimeSpan;
		private var selectedSpanStart:int;
		private var selectedSpanEnd:int;
		
		private var autoScrollTimer:Timer = new Timer(5);
		private var autoScrollMode:int = 0;
		private static const MODE_AUTO_SCROLL_LEFT:int = 1;
		private static const MODE_AUTO_SCROLL_RIGHT:int = 2;
		private var autoScrollPositionX:int;
		private var scrollSpeed:int;
		private static const INITIAL_SCROLL_SPEED:int = 200;
		
		private var spanToolTip:IToolTip = null;
		
		public var displayedSpans:ArrayCollection = new ArrayCollection();
		
		public function init():void {
			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
			this.addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
			autoScrollTimer.addEventListener(TimerEvent.TIMER, autoScroll);
		}
		
		public function selectSpan(span:ContainerTimeSpan):void {
			this.selectedSpan = span;
			if (span != null) {
				this.selectedSpanStart = span.start;
				this.selectedSpanEnd = span.end;
			}
		}
		
		public function mouseDown(event:MouseEvent):void {
			if (contentMouseX < 0) return;
			mouseDownX = contentMouseX;
			// find block
			selectedSpan = null;
			for (var i:int = 0; i < displayedSpans.length && selectedSpan == null; i++) {
				var b:ContainerTimeSpan = displayedSpans.getItemAt(i) as ContainerTimeSpan;
				var startPixel:int = waveFormState.getPixel(b.start);
				var endPixel:int = waveFormState.getPixel(b.end);
				
				if (mouseDownX >= startPixel - resizeAnchorWidth && mouseDownX <= startPixel + resizeAnchorWidth) {
					mode = MODE_RESIZE_LEFT;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedSpan = b;
					selectedSpanStart = b.start;
					selectedSpanEnd = b.end;
					spanToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				} else if (mouseDownX >= endPixel - resizeAnchorWidth && mouseDownX <= endPixel + resizeAnchorWidth) {
					mode = MODE_RESIZE_RIGHT;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedSpan = b;
					selectedSpanStart = b.start;
					selectedSpanEnd = b.end;
					spanToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				} else if (mouseDownX > startPixel && mouseDownX < endPixel) {
					mode = MODE_MOVE;
					scrollSpeed = INITIAL_SCROLL_SPEED;
					selectedSpan = b;
					selectedSpanStart = b.start;
					selectedSpanEnd = b.end;
					spanToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
				}
				updateToolTip();
			}
			
			if (selectedSpan != null) {
				dispatchEvent(new SelectContainerEvent(SelectContainerEvent.SELECT, selectedSpan));
				draw();
			}
		}

		public function mouseOut(event:MouseEvent):void {
			if (autoScrollMode == 0) {
				mode = 0;
				if (spanToolTip != null) {
					ToolTipManager.destroyToolTip(spanToolTip);
					spanToolTip = null;
				}
			}				
		}

		public function mouseUp(event:MouseEvent):void {
			mode = 0;
			autoScrollMode = 0;
			if (spanToolTip != null) {
				ToolTipManager.destroyToolTip(spanToolTip);
				spanToolTip = null;
			}
		}
		
		public function mouseMove(event:MouseEvent):void {
			if (contentMouseX < 0) return;
			if (mode == 0) {
				if (spanToolTip != null) {
					ToolTipManager.destroyToolTip(spanToolTip);
					spanToolTip = null;
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
			var newStart:int = selectedSpanStart + delta;
			var newEnd:int = selectedSpanEnd + delta;
			switch (mode) {
				case MODE_MOVE : {
					if (newStart >= 0 && newEnd <= waveFormState.length) { 
						selectedSpan.start = selectedSpanStart + delta;
						selectedSpan.end = selectedSpanEnd + delta;
						if (selectedSpan.end > waveFormState.end) {
							waveFormState.view.offset = selectedSpan.end - this.width * waveFormState.interval;
							autoScrollMode = MODE_AUTO_SCROLL_RIGHT;
							waveFormState.view.draw();
							autoScrollTimer.start();							
						} else 	if (selectedSpan.start < waveFormState.start) {
							waveFormState.view.offset = selectedSpan.start;
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
					if (newStart < selectedSpan.end) {
						selectedSpan.start = newStart;
						draw();
					}
					break;
				}
				case MODE_RESIZE_RIGHT : {
					if (newEnd > selectedSpan.start) {
						selectedSpan.end = newEnd;
						draw();
					}
					break;
				}

			}
			updateToolTip();
		}
		
		private function updateToolTip():void {
			if (spanToolTip != null && selectedSpan != null) {
				var x:int = waveFormState.getPixel(selectedSpan.start);
				
				spanToolTip.text = selectedSpan.start.toString() + " ms - " + selectedSpan.end.toString() + " ms";
				if (x + spanToolTip.width > this.width - 10) {
					x = this.width - 10 - spanToolTip.width;
				}
				var p:Point = localToGlobal(new Point(x)); 
				spanToolTip.x = p.x;
				
				spanToolTip.y = p.y + 40;
			}
		}

		private function autoScroll(event:Event):void {
			if (autoScrollMode == MODE_AUTO_SCROLL_LEFT) {
				if (selectedSpan.start == 0) {
					autoScrollMode = 0;
					autoScrollTimer.stop();
				}
				if (selectedSpan.start < scrollSpeed) {
					selectedSpan.end -= selectedSpan.start;
					selectedSpan.start = 0;
				} else {
					selectedSpan.start -= scrollSpeed;
					selectedSpan.end -= scrollSpeed;
				}
				selectedSpanStart = selectedSpan.start;
				selectedSpanEnd = selectedSpan.end;
				mouseDownX = contentMouseX;
				waveFormState.view.offset = selectedSpan.start;
				waveFormState.view.draw();
			} else if (autoScrollMode == MODE_AUTO_SCROLL_RIGHT) {
				if (selectedSpan.start == waveFormState.length) {
					autoScrollMode = 0;
					autoScrollTimer.stop();
				}
				if (selectedSpan.end > waveFormState.length-scrollSpeed) {
					selectedSpan.start += (waveFormState.length - selectedSpan.end);
					selectedSpan.end = waveFormState.length;
				} else {
					selectedSpan.start += scrollSpeed;
					selectedSpan.end += scrollSpeed;
				}
				selectedSpanStart = selectedSpan.start;
				selectedSpanEnd = selectedSpan.end;
				mouseDownX = contentMouseX;

				waveFormState.view.offset = selectedSpan.end - this.width * waveFormState.interval;
				waveFormState.view.draw();				
			}
			
			updateToolTip();
		}

		override public function draw():void {
            this.graphics.clear();
			this.displayedSpans.removeAll();

			if (spans == null) return; // nothing to do
			
			for (var i:int = 0; i < spans.length; i++) {
				var b:ContainerTimeSpan = spans.getItemAt(i) as ContainerTimeSpan;
				if (waveFormState.isInDisplayRange(b.start, b.end)) {
					displayedSpans.addItem(b);
					drawSpan(b, b == selectedSpan);
				}
			}
		}

		protected function drawSpan(span:ContainerTimeSpan, selected:Boolean):void {
			
		}
	}
}