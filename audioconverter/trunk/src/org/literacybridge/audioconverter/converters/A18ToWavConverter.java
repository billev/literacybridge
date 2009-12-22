package org.literacybridge.audioconverter.converters;

import java.io.File;
import java.util.regex.Pattern;

public class A18ToWavConverter extends A18BaseConverter {
	public A18ToWavConverter() throws AudioConverterInitializationException {
		super(".wav");
	}

	@Override
	protected Pattern getPattern() {
		return Pattern.compile("^Decode\\s'([^']*)'.*");
	}

	@Override
	protected String getCommand(File audioFile, File targetFile) {
		return getConverterEXEPath() 
		+ " -d "
		+ "-s 16000 " 
		+ "-o \"" + targetFile.getAbsolutePath()+ "\" "   
		+ "\"" + audioFile.getAbsolutePath() + "\"";
	}
}
