package org.literacybridge.authoring.audio {
	import flash.media.Sound;
	import flash.utils.ByteArray;
	
	public class AudioSpectrum {
		
		// the sampling rate Flex 3.2 uses for Sound.extract;
		// currently there is no constant in the Sound class for this
		// TODO: change appropriately in case the sampling rate in Sound 
		// changes or won't be constant in the future anymore
		private static const SOUND_SAMPLING_RATE:int = 44100;  // value is in Hz (1/s) 
		
		// currently the precision of the physical device is 128ms,
		// so we don't have to visualize sound in a higher precision either
		public static const DEFAULT_PRECISION:Number = 0.128;   // in s
		
		private static const maxBufferSize:int = 1024 * 1024; // 1 MB buffer size for streaming
		
		public var precision:Number = DEFAULT_PRECISION; 
		
		[Bindable]
		public var data:Array;
		[Bindable]
		public var max:Number = -Infinity;
		[Bindable]
		public var min:Number = Infinity;
		
		public var audio:Sound;
		
		public function AudioSpectrum(precision:Number = DEFAULT_PRECISION) {
			this.precision = precision;
			data = new Array();
		}
		
		public static function load(audio:Sound, precision:Number = DEFAULT_PRECISION):AudioSpectrum {
			var s:AudioSpectrum = new AudioSpectrum(precision);
			s.audio = audio;
			s.processSound();			
			return s;
		}
		
		private function processSound():void {
			var interval:int = Math.floor(SOUND_SAMPLING_RATE * precision);
			var numIntervals:int = Math.floor(maxBufferSize / interval);
			var bufferSize:int = numIntervals * interval * 8;
			
			var buffer:ByteArray = new ByteArray();

			var numSoundSamples:int = audio.length / 1000 * SOUND_SAMPLING_RATE;
			var pos:int = 0;
			var skip:int = 0;
			
			while (numSoundSamples > 0) {
				var toLoad:int = Math.min(numSoundSamples, bufferSize / 8);
				audio.extract(buffer, toLoad, pos);
				pos += toLoad;
				numSoundSamples -= toLoad;
				var end:int = toLoad * 8;
				var incr:int = interval * 8 - 4;
				
				for (buffer.position = 0; buffer.position < end; buffer.position += incr) {
					var v:Number = buffer.readFloat();
					max = Math.max(max, v);
					min = Math.min(min, v);
					data.push(v);
				}
				
			}

			
		}
		
//		public function getSubSpectrum(startTime:int, numDataPoint:int):AudioSpectrum {
//			var s:AudioSpectrum = new AudioSpectrum();
//			var numDataPoints:
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
//			
//			return s;
//		}

	}
}