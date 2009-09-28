package org.literacybridge.audioconverter.converters;

import java.io.File;
import java.io.IOException;

public interface IAudioConverter {
	
	// @return path to the converter executable 
	String getConverterEXEPath(); 

	// @return true if the internally stored converter is valid; false otherwise
	boolean validateConverterExecutable();
	
	// @return true if file could be converted successfully; false otherwise
	boolean doConvertFile(File audioFile, File targetDir) throws IOException, InterruptedException;

	// @return string with information about conversion result; should be null if no error occurred
	String getResult(); 
}
