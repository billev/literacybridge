package org.literacybridge.audioconverter.converters;

import java.io.File;
import java.util.regex.Pattern;

public class WavToA18Converter extends A18BaseConverter {
	public WavToA18Converter() throws AudioConverterInitializationException {
		super(".a18");

	}
	
	@Override
	protected String getCommand(File audioFile, File targetFile) {
		return getConverterEXEPath() 
		+ " -e A1800"
		+ " -s 16000" 
		+ " -o \"" + targetFile.getAbsolutePath()+ "\""
		+ " \"" + audioFile.getAbsolutePath() + "\"";
	}

	@Override
	protected Pattern getPattern() {
		return Pattern.compile("^Encode\\s'([^']*)'.*");
	}
}
