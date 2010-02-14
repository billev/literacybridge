package org.literacybridge.audioconverter.api;

import java.io.File;
import java.util.LinkedHashMap;
import java.util.Map;

import javax.sound.sampled.AudioFormat;

import org.literacybridge.audioconverter.converters.A18ToMP3Converter;
import org.literacybridge.audioconverter.converters.A18ToWavConverter;
import org.literacybridge.audioconverter.converters.AnyToA18Converter;
import org.literacybridge.audioconverter.converters.BaseAudioConverter;
import org.literacybridge.audioconverter.converters.BaseAudioConverter.AudioConverterInitializationException;
import org.literacybridge.audioconverter.converters.BaseAudioConverter.ConversionException;

public class ExternalConverter {

	private A18ToMP3Converter A18ToMP3Conv;
	private A18ToWavConverter A18ToWAVConv;
	private AnyToA18Converter AnyToA18Conv;
	
	Map<String, String> parameters = new LinkedHashMap<String, String>();

	
	public boolean override = false;
	
	public ExternalConverter() throws ConversionException 
	{
		try {
			A18ToMP3Conv = new A18ToMP3Converter();
			A18ToWAVConv = new A18ToWavConverter();
			AnyToA18Conv = new AnyToA18Converter();
		} catch (AudioConverterInitializationException e) {
			
			e.printStackTrace();
		}

	}
	
	public void convert(File sourceFile, File targetFile, AudioConversionFormat targetFormat) 
		throws ConversionException {
		
		
		
		if (targetFormat.FileEnding == "A18")
		{
				SetParameters(targetFormat);
				AnyToA18Conv.convertFile(sourceFile, targetFile, this.override, this.parameters);
		}
		if (targetFormat.FileEnding == "WAV")
		{
				SetParameters(targetFormat);
				A18ToWAVConv.convertFile(sourceFile, targetFile, this.override, this.parameters);
		}
		if (targetFormat.FileEnding == "MP3")
		{
				SetParameters(targetFormat);
				A18ToMP3Conv.convertFile(sourceFile, targetFile, this.override, this.parameters);
		}
	}
	
	
	private void SetParameters(AudioConversionFormat paramFormat) {
		
		parameters.clear();
		
		parameters.put(BaseAudioConverter.BIT_RATE, paramFormat.BitRate);
		parameters.put(BaseAudioConverter.SAMPLE_RATE, paramFormat.SampleRate);
		
		if (paramFormat.FileEnding == "A18")
		{

			parameters.put(AnyToA18Converter.USE_HEADER, ((A18Format)paramFormat).usedHeader);
			parameters.put(AnyToA18Converter.ALGORITHM, ((A18Format)paramFormat).usedAlgo);
		}
	}
	
}

