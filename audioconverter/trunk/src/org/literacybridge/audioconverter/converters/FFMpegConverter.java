package org.literacybridge.audioconverter.converters;

import java.io.File;

public class FFMpegConverter extends BaseAudioConverter {
	final static String TARGET_EXTENSION = ".wav";
	
	public FFMpegConverter() {
		super(TARGET_EXTENSION);
	}
	
	@Override
	public ConversionResult doConvertFile(File audioFile, File targetDir, File targetFile)
			throws ConversionException {
		String cmd = getConverterEXEPath()
						+ " -i \"" + audioFile.getAbsolutePath() + "\""
						+ " -ar 16000"   // 16000 sampling rate
						+ " -ac 1"       // 1 channel = mono
						+ " -y"          // overwrite output file
						+ " \"" + targetFile.getAbsolutePath() + "\"";

		ConversionResult result = new ConversionResult();
		result.outputFile = targetFile;
		result.response = BaseAudioConverter.executeConversionCommand(cmd,
																	  true,  // important! ffmpeg prints to stderr, not stdout
																	  audioFile.getName());
		
		return result;
	}

	public void validateConverter() throws AudioConverterInitializationException {
		BaseAudioConverter.validateConverterExecutable(getConverterEXEPath(), 
													   true,   // important! ffmpeg prints to stderr, not stdout
													   "FFmpeg version");
	}
	
	public String getConverterEXEPath() {
		// as we assume our converter directory located in the same
		// directory as we are, get out location
		return System.getProperty("user.dir") + "/converters/ffmpeg/ffmpeg.exe";
	}

	@Override
	public String getShortDescription() {
		return "Convert any audio file to .wav";
	}

	@Override
	public String getSourceFileExtension() {
		return "*";
	}
}
