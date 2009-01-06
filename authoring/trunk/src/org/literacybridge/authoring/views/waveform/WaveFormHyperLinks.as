package org.literacybridge.authoring.views.waveform
{
import flash.display.Graphics;

import mx.collections.ArrayCollection;

import org.literacybridge.authoring.audio.AudioSpectrum;
import org.literacybridge.authoring.schema.Hyperlink;
    
    public class WaveFormHyperLinks extends VisualizerBase
    {
        public var color1:uint = 0x00FF00;
        public var color2:uint = 0x03880c;
        
        private var current : Number = 0;
        protected var hInterval : Number = 0;
        protected var vInterval : Number = 0;
        private var heightInterval : uint = 0;
        private var gap:Number = 10000;
        
        public var scale:Number = 1;
        public var spectrum:AudioSpectrum;
        public var hyperlinks:ArrayCollection;
        
        override protected function updateDisplayList(w:Number, h:Number):void {
            super.updateDisplayList(w, h);
            hInterval = w/gap;
            vInterval = Math.floor( h/2 );
           
            
        }
        
        override public function draw() : void {
        	if (spectrum == null || hyperlinks == null) return;
            var g : Graphics = this.graphics;
            g.clear();
            vInterval = Math.floor( this.height/2 );
                          
        	var factor:Number = vInterval / Math.max(Math.abs(spectrum.max), Math.abs(spectrum.min));
        	
        	for (var i:int = 0; i < hyperlinks.length; i++) {
        		var hyperlink:Hyperlink = hyperlinks.getItemAt(i) as Hyperlink;
        		if (waveFormState.isInDisplayRange(hyperlink.start, hyperlink.end)) {
	        		var x_start:int = waveFormState.getPixel(hyperlink.start);
	        		var x_end:int = waveFormState.getPixel(hyperlink.end);
					
					g.lineStyle( 3, color2, 1);
					if (x_start >= 0) {
			        	g.moveTo(x_start, this.height + 7);
		        		g.lineTo(x_start, this.height + 13);
		   			} else {
		   				x_start = 0;
		   			}
					
					if (x_end >= 0) {
			        	g.moveTo(x_end, this.height + 7);
		        		g.lineTo(x_end, this.height + 13);						
					} else {
						x_end = 0;
					}
					
		        	g.moveTo(x_start, this.height + 10);
		        	g.lineTo(x_end, this.height + 10);
	
	        		
	        		g.lineStyle( 1, color1, 1);
	        		g.moveTo(x_start, vInterval);
		        	for (; x_start < x_end && x_start < spectrum.data.length; x_start++) {
		        		var index:int = waveFormState.getMilliseconds(x_start) / spectrum.precision / 1000;
	        			drawDataPoint(x_start, spectrum.data[index] * factor + vInterval);
		        	}
		        }
	        	
        	}
        }
                
        
//        public function drawVisualization() : void
//        {
//            if( !this.visible ) return;      
//            
//            
//            var g : Graphics = this.graphics;
//            g.clear();
//            g.endFill();
//                          
//            g.moveTo(0, vInterval);
//            g.lineStyle( 0, lColor, 1);
//            
//            if (spectrum.bytesAvailable == 0) return;
//            
//            var numBytes:Number = spectrum.bytesAvailable;
//            var numDataPoints:int = numBytes / 4 / 2;
//            var s:Number = Math.ceil(numDataPoints / width / scale);
//            trace(s);
//            s = 5644;            
//            
//            spectrum.position = 0;
//            var yValues:Array = new Array();
//            var yMax:Number = 0;
//            
//            for (var x:int=0; x < width-1; x++) {
//            	//spectrum.position = x * s * 8;
//            	var y_pos : Number = 0;
//            	var y_neg : Number = 0;
//            	var y : Number = 0;
//            	for (var i:int = 0; i < s; i++) {
//            		var v:Number = spectrum.readFloat();
//            		if (v > 0) {
//            			y_pos += v;
//            		} else {
//            			y_neg += v;
//            		}
////					y += v;
//            		
////            		if (Math.abs(v) > Math.abs(y)) {
////            			y = v;
////            		}
//            		spectrum.readFloat(); // discard right channel
//            	}
//
////        		if (Math.abs(y_pos) > Math.abs(y_neg)) {
////        			y = y_pos;
////        		} else {
////        			y = y_neg;
////        		}
//            	
//            	y_pos/=s;
//            	y_neg/=s;
//            	if (Math.abs(y_pos) > Math.abs(y_neg)) {
//            		y = y_pos;
//            	} else {
//            		y = y_neg;
//            	}
//            	
//            	yMax = Math.max(Math.abs(y), yMax);
//				
//				//yValues.push(y_pos);
//				yValues.push(y);
//
//            }
//
//			var factor:Number = vInterval / yMax;
//
//			for (x=0; x < width-1; x++) {
//					//g.moveTo(x, vInterval);
//					drawDataPoint(x, yValues[x] * (factor) + vInterval);
////					g.moveTo(x, vInterval);
////					drawDataPoint(x, yValues[x * 2 + 1] * (factor) + vInterval);
//			}            
//            
//            
//			            
////            var avg:Number = 0;
////            var x:int = 0;
////			for (var i:int = 0; i < numDataPoints; i++) {
////				if (i % scale == 0) {
////					var y : Number = avg * (vInterval * 4) + vInterval;
////					drawDataPoint(x, y);
////					x++;
////					avg = 0;
////				}
////				var left:Number = spectrum.readFloat();
////				var right:Number = spectrum.readFloat();
////				if (Math.abs(left) > Math.abs(avg)) {
////					avg = left;
////				}
////			}
//        }
//        
        private function drawDataPoint(x:Number, y:Number):void {
        	this.graphics.lineTo(x, y);
        }
        
    }
}