package org.literacybridge.acm.repository;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;

import org.literacybridge.acm.config.Configuration;
import org.literacybridge.acm.content.LocalizedAudioItem;

public class Repository {
	private final File baseDir = Configuration.GetRepositoryDirectory();
	
	private final static Repository instance = new Repository();
	
	private Repository() {
		// singleton
	}
	
	public static Repository getRepository() {
		return instance;
	}
	
	public void initialize() throws IOException {
		if (!baseDir.exists()) {
			baseDir.mkdirs();
		}
	}

	public static String getNewUUID() throws IOException {
		Configuration config = Configuration.getConfiguration();
		String value = config.getRecordingCounter();
		int counter = (value == null) ? 0 : Integer.parseInt(value, Character.MAX_RADIX);
		counter++;
		value = Integer.toString(counter, Character.MAX_RADIX);
		String uuid = "LB-2" + "_"  + config.getDeviceID() + "_" + value;
		
		// make sure we remember that this uuid was already used
		config.setRecordingCounter(value);
		config.writeProps();
		
		return uuid;
	}
	
	/**
	 * Copies the given file to the directory of the localizedAudioItem
	 */
	public void store(File file, String targetFileName, LocalizedAudioItem localizedAudioItem) throws IOException {
		File dir = resolveName(localizedAudioItem);
		File toFile = new File(dir, targetFileName);
		copy(file, toFile);
	}
	
	public File getWAVFile(LocalizedAudioItem localizedAudioItem) {
		File path = resolveName(localizedAudioItem);
		File[] files = path.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String name) {
				return name.toLowerCase().endsWith(".wav");
			}
		});
		
		if (files == null || files.length == 0) {
			return null;
		}
		
		return files[0];
	}
	
	public void delete(LocalizedAudioItem localizedAudioItem) {
		File dir = resolveName(localizedAudioItem);
		File[] files = dir.listFiles();
		for (File f : files) {
			f.delete();
		}
		dir.delete();
	}
	
	// Returns the path of this audioitem in the repository
	public File resolveName(LocalizedAudioItem localizedAudioItem) {
		// TODO: For now we just use the unique ID of the audio item; in the future, we might want to use
		// a different way to construct the path
		
		StringBuilder builder = new StringBuilder();
		builder.append(baseDir.getAbsolutePath());
		builder.append(File.separator);
		builder.append("org");
		builder.append(File.separator);
		builder.append("literacybridge");
		builder.append(File.separator);
		builder.append(localizedAudioItem.getParentAudioItem().getUuid());
		
		String path = builder.toString();
		File dir = new File(path);
		if (!dir.exists()) {
			dir.mkdirs();
		}

		return dir;
	}
	
	public static void copy(File fromFile, File toFile) throws IOException {
		copy(fromFile, toFile, fromFile.length());
	}
	
	public static void copy(File fromFile, File toFile, long numBytes) throws IOException {
	    FileInputStream from = null;
	    FileOutputStream to = null;
	    try {
	      from = new FileInputStream(fromFile);
	      to = new FileOutputStream(toFile);
	      byte[] buffer = new byte[4096];
	
	      while (true) {
		      int numToRead = buffer.length;
		      if (numToRead > numBytes) {
		    	  numToRead = (int) numBytes;
		      }
		      from.read(buffer, 0, numToRead);
		      to.write(buffer, 0, numToRead); // write
		      numBytes -= numToRead;
		      if (numBytes <= 0) {
		    	  break;
		      }
	      }
	    } finally {
	      if (from != null)
	        try {
	          from.close();
	        } catch (IOException e) {
	          // ignore
	        } finally {
		      if (to != null)
		        try {
		          to.close();
		        } catch (IOException e) {
		          // ignore
		        }
	        }
	    }
	}
}
