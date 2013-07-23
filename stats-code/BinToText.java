
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.io.Writer;

/**  
 Converting binary data into different forms.
 
 <P>Reads binary data into memory, and writes it back out.
 (If your're actually copying a file, there are better ways to do this.)
 
 <P>Buffering is used when reading and writing files, to minimize the number 
 of interactions with the disk.
*/
public final class BinToText {

  /** Change these settings before running this class. */
	private static final String INPUT_FILE_NAME = "V:\\Phase-II-TBs\\TB Content statistics";
	private static final String OUTPUT_FILE_NAME = "V:\\Content Statistics\\stats.csv";

  /** Run the example. */
  public static void main(String... aArgs) {
	BinToText test = new BinToText();
	int iOpen, iClose, iCopy, iSurvey, iApplied, iUseless;
	long fileSize,recordingSeconds ;
	Writer output;
	RandomAccessFile in;
	try {
	    output = new BufferedWriter(new FileWriter(OUTPUT_FILE_NAME));
	    File dir = new File(INPUT_FILE_NAME);
	    for (File community : dir.listFiles()) {
		    if (".".equals(community.getName()) || "..".equals(community.getName()) || community.isFile()) {
			      continue;  // Ignore the self and parent aliases.
			    }
		    for (File idTB : community.listFiles()) {
			    if (".".equals(idTB.getName()) || "..".equals(idTB.getName()) || idTB.isFile()) {
			      continue;  // Ignore the self and parent aliases.
			    }
			    FilenameFilter justStatsFilter = new FilenameFilter() {
			        public boolean accept(File dir, String name) {
			            return name.equalsIgnoreCase("stats");
			        }
			    };
			    for (File statsFolder : idTB.listFiles(justStatsFilter)) {
				    FilenameFilter noSCVFilter = new FilenameFilter() {
				        public boolean accept(File dir, String name) {
				            return !name.contains(".csv");
				        }
				    };
				    for (File idMsg : statsFolder.listFiles(noSCVFilter)) {
					    in = new RandomAccessFile(idMsg.toString(), "r");
						iOpen = readInteger(in);
						iClose = readInteger(in);
						iCopy = readInteger(in);
						iSurvey = readInteger(in);
						iApplied = readInteger(in);
						iUseless = readInteger(in);
						in.close();
						
						if (idMsg.getName().startsWith(idTB.getName())) {
							File recording = new File(idTB.getAbsolutePath() + "\\user-recordings\\" + idMsg.getName() + ".a18");
							fileSize = recording.length();
							recordingSeconds = fileSize / (16 << 7);
						} else 
							recordingSeconds = 0;
					  /** Write fixed content to the given file. */
						//read in the bytes
					    output.write(community.getName());
					    output.write(",");
					    output.write(idTB.getName());
					    output.write(",");
					    output.write(idMsg.getName());
					    output.write(",");					    
					    output.write(Integer.toString(iOpen));
					    output.write(",");
					    output.write(Integer.toString(iClose));
					    output.write(",");
					    output.write(Integer.toString(iCopy));
					    output.write(",");
					    output.write(Integer.toString(iSurvey));
					    output.write(",");
					    output.write(Integer.toString(iApplied));
					    output.write(",");
					    output.write(Integer.toString(iUseless));
					    output.write(",");
					    output.write(String.valueOf(recordingSeconds));
					    output.write("\n");
				    }
			    }
		    }
	    }
	    output.close();
	    log("all done");
//		byte[] fileContents = test.read(INPUT_FILE_NAME);
		//test.readAlternateImpl(INPUT_FILE_NAME);
		//write it back out to a different file name
//		test.write(fileContents, OUTPUT_FILE_NAME);
	}
    catch (FileNotFoundException ex) {
        log("File not found.");
      }
      catch (IOException ex) {
        log(ex);
      } 
  }
  
  static int readInteger(RandomAccessFile rd) {
	 long sum = 0;
	 int b, i, ret;

	 try {
		 for (int l= 0; l<4; l++) {
			 b = rd.readByte() & 0xFF; // remove sign
			 i = b << (8 * l);
			 sum += (0xFFFF & i);
		 }
	 }
    catch (FileNotFoundException ex) {
        log("File not found.");
        log(ex);
        ex.printStackTrace();
      }
      catch (IOException ex) {
    	  ex.printStackTrace();
      }
	 ret = (int) sum;
	 return ret;
  }

  /** Read the given binary file, and return its contents as a byte array.*/ 
  byte[] read(String aInputFileName){
    log("Reading in binary file named : " + aInputFileName);
    File file = new File(aInputFileName);
    log("File size: " + file.length());
    byte[] result = new byte[(int)file.length()];
    try {
      InputStream input = null;
      try {
        int totalBytesRead = 0;
        input = new BufferedInputStream(new FileInputStream(file));
        while(totalBytesRead < result.length){
          int bytesRemaining = result.length - totalBytesRead;
          //input.read() returns -1, 0, or more :
          int bytesRead = input.read(result, totalBytesRead, bytesRemaining); 
          if (bytesRead > 0){
            totalBytesRead = totalBytesRead + bytesRead;
          }
        }
        /*
         the above style is a bit tricky: it places bytes into the 'result' array; 
         'result' is an output parameter;
         the while loop usually has a single iteration only.
        */
        log("Num bytes read: " + totalBytesRead);
      }
      finally {
        log("Closing input stream.");
        input.close();
      }
    }
    catch (FileNotFoundException ex) {
      log("File not found.");
    }
    catch (IOException ex) {
      log(ex);
    }
    return result;
  }
  
  /**
   Write a byte array to the given file. 
   Writing binary data is significantly simpler than reading it. 
  */
  void write(byte[] aInput, String aOutputFileName){
    log("Writing binary file...");
    try {
      OutputStream output = null;
      try {
        output = new BufferedOutputStream(new FileOutputStream(aOutputFileName));
        output.write(aInput);
      }
      finally {
        output.close();
      }
    }
    catch(FileNotFoundException ex){
      log("File not found.");
    }
    catch(IOException ex){
      log(ex);
    }
  }
  
  /** Read the given binary file, and return its contents as a byte array.*/ 
  byte[] readAlternateImpl(String aInputFileName){
    log("Reading in binary file named : " + aInputFileName);
    File file = new File(aInputFileName);
    log("File size: " + file.length());
    byte[] result = null;
    try {
      InputStream input =  new BufferedInputStream(new FileInputStream(file));
      result = readAndClose(input);
    }
    catch (FileNotFoundException ex){
      log(ex);
    }
    return result;
  }
  
  /**
   Read an input stream, and return it as a byte array.  
   Sometimes the source of bytes is an input stream instead of a file. 
   This implementation closes aInput after it's read.
  */
  byte[] readAndClose(InputStream aInput){
    //carries the data from input to output :    
    byte[] bucket = new byte[32*1024]; 
    ByteArrayOutputStream result = null; 
    try  {
      try {
        //Use buffering? No. Buffering avoids costly access to disk or network;
        //buffering to an in-memory stream makes no sense.
        result = new ByteArrayOutputStream(bucket.length);
        int bytesRead = 0;
        while(bytesRead != -1){
          //aInput.read() returns -1, 0, or more :
          bytesRead = aInput.read(bucket);
          if(bytesRead > 0){
            result.write(bucket, 0, bytesRead);
          }
        }
      }
      finally {
        aInput.close();
        //result.close(); this is a no-operation for ByteArrayOutputStream
      }
    }
    catch (IOException ex){
      log(ex);
    }
    return result.toByteArray();
  }
  
  private static void log(Object aThing){
    System.out.println(String.valueOf(aThing));
  }
} 
