package org.literacybridge.authoring.views.waveform
{
	public class WaveFormState {
			[Bindable]
			public var length:int;    // length in milliseconds
		
			[Bindable]
			public var start:int;    // start of range to show in milliseconds
			[Bindable]
			public var end:int;    // end of range to show in milliseconds
			[Bindable]
			public var interval:int;    // how many milliseconds one pixel covers
			
			public function autoZoom(width:int):void {
				interval = 128;
				while (((end - start) / width) > interval) {
					interval *= 2;
				}
				end = width * interval;
			}
			
	        public final function getPixel(milliseconds:int):int {
	        	var p:int =  (milliseconds - start) / interval;
	        	return p;
	        }

	        public final function getMilliseconds(pixelX:int):int {
	        	return pixelX * interval + start;
	        }

			public function isInDisplayRange(start:int, end:int):Boolean {	        
				return ((start >= this.start && start <= this.end)
							|| (end >= this.start && end <= this.end));
			}

	}
}