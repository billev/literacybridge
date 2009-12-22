package org.literacybridge.audioconverter.converters;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.regex.Matcher;
import java.util.regex.Pattern;



public class AudioConverterForA18 implements IAudioConverter {

	final String ID = "A18toWAV";
	
	private String converterPath = null;
	// extracted conversion result
	private Pattern pattern = Pattern.compile("^Decode\\s'([^']*)'.*");
	private String resultString;
	
	public String getConverterEXEPath() {
		// as we assume our converter directory located in the same
		// directory as we are, get out location
		return System.getProperty("user.dir") + "/converters/a18/AudioBatchConverter.exe";
	}
	
	
	public boolean validateConverterExecutable() 
	{
		if (converterPath == null)
			return false;

		File exeFile = new File(converterPath);
		if (!exeFile.exists())
			return false;

		try {
			boolean success = false;
			String cmd = converterPath;

			Process proc = Runtime.getRuntime().exec(cmd);
			InputStream stderr = proc.getInputStream();
			InputStreamReader isr = new InputStreamReader(stderr);
			BufferedReader br = new BufferedReader(isr);
			String line = null;

			while ((line = br.readLine()) != null) {
				if (line.startsWith("Generalplus Audio Batch Converter Tool")) {
					success = true;
				}
			}
			proc.waitFor();
			return success;
		} catch (Exception e) {
			return false;
		}
	}
	
	
	
	
	public boolean doConvertFile(File audioFile, File targetDir) throws IOException, InterruptedException {
		boolean success = false;
		resultString 	= null;

		System.out.println("Path to converter: " + getConverterEXEPath());
		
		String cmd = getConverterEXEPath() 
						+ " -d "
						+ "-s 16000 " 
						+ "-o \"" + targetDir.getAbsolutePath()+ "\" "
						+ "\"" + audioFile.getAbsolutePath() + "\"";

		Process proc = Runtime.getRuntime().exec(cmd);
		InputStream stderr = proc.getInputStream();
		InputStreamReader isr = new InputStreamReader(stderr);
		BufferedReader br = new BufferedReader(isr);
		String line = null;

		while ((line = br.readLine()) != null) {
			Matcher matcher = pattern.matcher(line);
			if (matcher.matches()) {
				String fileName = matcher.group(1);
				if (line.endsWith("Succeeded")) {
					success = true;
					resultString = fileName;
				} else {
					success = false; // JTBD
				}
			}
		}

		// check for converter error
		if (success && proc.waitFor() != 0) {
			resultString = "Converter: Internal error while converting file: '"
				+ audioFile.getName() + "'";
		}
			
		return success;
	}

	public String getResult() {
		return resultString;
	}


	public String getID() {
		return ID;
	}


	public String getShortDescription() {
		return "Convert *.a18 to WAV audio files";
	}


	public String getSourceFileExtension() {
		return "a18";
	}
}
