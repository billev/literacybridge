package org.literacybridge.audioconverter.api;

public class WAVFormat extends AudioConversionFormat {

	public WAVFormat (String BitRate, String SampleRate){
		
		this.BitRate = BitRate;
		this.SampleRate = SampleRate;
		this.FileEnding = "WAV";
		
	}
}
