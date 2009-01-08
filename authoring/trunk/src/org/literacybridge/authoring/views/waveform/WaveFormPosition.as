package org.literacybridge.authoring.views.waveform
{
	import flash.display.Graphics;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	
	import mx.binding.utils.BindingUtils;
	import mx.core.IToolTip;
	import mx.managers.ToolTipManager;
	
	import org.literacybridge.authoring.views.PlayerControl;
	
	public class WaveFormPosition extends VisualizerBase {
		[Bindable] public var pos:int = 0; 
		public var playerControl:PlayerControl;
		private var positionToolTip:IToolTip = null;
		private var mouseButtonDown:Boolean = false;
		
		public function init():void {
			BindingUtils.bindSetter(update, this, "pos");
			this.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
			this.addEventListener(MouseEvent.MOUSE_OUT, mouseOut);
			this.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
			this.addEventListener(MouseEvent.MOUSE_UP, mouseUp);

			var p:Point = localToGlobal(new Point(waveFormState.getPixel(pos), 0));
			if (positionToolTip == null) {
				positionToolTip = ToolTipManager.createToolTip("", 0, 0, null, this);
			}
			updateToolTip();
		}
		
		public function mouseDown(event:MouseEvent):void {
			if (contentMouseY > WaveFormView.POSITION_SLIDER_HEIGHT || contentMouseX < 0) return;
			pos = waveFormState.getMilliseconds(contentMouseX);
			mouseButtonDown = true;		
			playerControl.detachPosWatching();		
		}

		public function mouseOut(event:MouseEvent):void {
			mouseButtonDown = false;		
			playerControl.reattachPosWatching();		
		}

		public function mouseUp(event:MouseEvent):void {
			if (mouseButtonDown) {
				pos = waveFormState.getMilliseconds(contentMouseX);
				mouseButtonDown = false;
				playerControl.setCurrentSoundPos();
				playerControl.reattachPosWatching();
			}				
		}

		public function mouseMove(event:MouseEvent):void {
			if (mouseButtonDown && contentMouseX >= 0) {
				pos = waveFormState.getMilliseconds(contentMouseX);
			}				
		}
		
		private function update(args:*):void {
			draw();
		}
		
		override public function draw():void {
            var g : Graphics = this.graphics;
            g.clear();
			if (waveFormState.isInDisplayRange(pos, pos)) {
                var x:int = waveFormState.getPixel(pos);          
	            g.moveTo(x, WaveFormView.POSITION_SLIDER_HEIGHT);
	            g.lineStyle( 2, 0 );
	            g.lineTo(x, this.height);
	            g.lineStyle( 2, 0xff0000 );
	            g.beginFill(0xff0000, 1);
	            g.moveTo(x, WaveFormView.POSITION_SLIDER_HEIGHT);
	            g.lineTo(x-4, 0);
	            g.lineTo(x+4, 0);
	            g.lineTo(x, WaveFormView.POSITION_SLIDER_HEIGHT);
	            g.endFill();
	            
	            if (positionToolTip != null) {
	            	positionToolTip.visible = true;
					updateToolTip()
	            }
	  		} else {
	            if (positionToolTip != null) {
	            	positionToolTip.visible = false;
	            }	  			
	  		}
		}
		
		private function updateToolTip():void {
			var p:Point = localToGlobal(new Point(waveFormState.getPixel(pos)));
			positionToolTip.x = p.x + 4;
			positionToolTip.y = p.y - 17;
			positionToolTip.text = pos.toString() + " ms";

		}
		
	}
}