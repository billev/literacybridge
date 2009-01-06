package org.literacybridge.authoring.views.waveform
{
	import mx.containers.Canvas;
	
	public class VisualizerBase extends Canvas {
		public var waveFormState : WaveFormState;
		
        override protected function updateDisplayList(w:Number, h:Number):void {
            super.updateDisplayList(w, h);
            draw();
        }

		public function draw():void {
			
		}
	}
}