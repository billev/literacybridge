package org.literacybridge.audioconverter.converters;

import java.io.File;
import java.io.IOException;

public class A18ToMP3Converter extends BaseAudioConverter {
	private FFMpegConverter ffmpegConverter = new FFMpegConverter();
	private A18ToWavConverter a18ToWavConverter = new A18ToWavConverter();
	
	public A18ToMP3Converter() throws AudioConverterInitializationException {
		super(".mp3");
	}

	@Override
	public ConversionResult doConvertFile(File inputFile, File targetDir,
			File targetFile) throws ConversionException {
		File tmp = null;
		try {
			ConversionResult r1 = a18ToWavConverter.doConvertFile(inputFile, targetDir, targetFile);
			tmp = r1.outputFile;
			ConversionResult r2 = ffmpegConverter.doConvertFile(r1.outputFile, targetDir, targetFile);
			
			r2.response = r1.response + "\n" + r2.response;
			return r2;
		} finally {
			if (tmp != null) {
				tmp.delete();
			}
		}
	}

	@Override
	public String getShortDescription() {
		return "Convert .a18 audio file to .mp3";
	}

	@Override
	public String getSourceFileExtension() {
		return "a18";
	}

	@Override
	public void validateConverter()
			throws AudioConverterInitializationException {
		ffmpegConverter.validateConverter();
		a18ToWavConverter.validateConverter();
	}

}
