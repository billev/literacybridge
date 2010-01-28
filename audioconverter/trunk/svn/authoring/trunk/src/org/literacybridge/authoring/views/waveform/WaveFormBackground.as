package org.literacybridge.authoring.views.waveform
{
	import flash.display.Graphics;
	
	public class WaveFormBackground extends VisualizerBase {
		override public function draw():void {
            var g : Graphics = this.graphics;
            g.clear();
                          
            g.lineStyle( 1 , 0, 0.2);
            
            
            for (var i:int = 0; i < 3; i++) {
            	var y:int = this.height / 4 * (i+1);
	            g.moveTo(0, y);
    	        g.lineTo(this.width, y);            	
            }
		}
		
	}
}