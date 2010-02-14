package org.literacybridge.audioconverter.api;

public class MP3Format extends AudioConversionFormat {

		public MP3Format (String BitRate, String SampleRate){
			
			this.BitRate = BitRate;
			this.SampleRate = SampleRate;
			this.FileEnding = "MP3";
			
		}
	}

	

