package org.literacybridge.audioconverter.api;

// import javax.sound.sampled.AudioFormat;

public class A18Format extends AudioConversionFormat{

	public String usedAlgo;
	public String usedHeader;
	
	public enum AlgorithmList {
		A1600,
		A1800,
		A3600
	}
	
	public enum useHeaderChoice {
		Yes,
		No
	}
		
	
	public A18Format (String BitRate, String SampleRate, AlgorithmList usedAlgo, useHeaderChoice usedHeader){
		
		this.BitRate = BitRate;
		this.SampleRate = SampleRate;
		this.usedAlgo = String.valueOf(usedAlgo);
		this.usedHeader = String.valueOf(usedHeader);
		this.FileEnding = "A18";
		
	}
	

}
