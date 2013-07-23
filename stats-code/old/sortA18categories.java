import java.io.File;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.RandomAccessFile;
import org.apache.commons.io.FileUtils;

/**  
 Converting binary data into different forms.
 
 <P>Reads binary data into memory, and writes it back out.
 (If your're actually copying a file, there are better ways to do this.)
 
 <P>Buffering is used when reading and writing files, to minimize the number 
 of interactions with the disk.
*/
public final class sortA18categories {

  /** Change these settings before running this class. */
	private static final String INPUT_PATH = "C:\\Users\\Cliff\\Dropbox\\Phase-II-TBs\\TB Content statistics\\";
	private static final String OUTPUT_PATH = "C:\\Users\\Cliff\\Dropbox\\User Recordings\\";

  /** Run the example. */
  public static void main(String... aArgs) {
	new sortA18categories();
	int temp=0,totalElements,id,len=0,totalFiles=0;
	String category ="";
	String communityName;
	RandomAccessFile in = null;
	try {
	    File dir = new File(INPUT_PATH);
	    for (File community : dir.listFiles()) {
		    communityName = community.getName();
		    if (".".equals(communityName) || "..".equals(communityName) || community.isFile()) {
			      continue;  // Ignore the self and parent aliases.
			    }
		    log(communityName);
		    for (File idTB : community.listFiles()) {
			    if (".".equals(idTB.getName()) || "..".equals(idTB.getName()) || idTB.isFile()) {
			      continue;  // Ignore the self and parent aliases.
			    }
			    FilenameFilter justUserRecordingsFilter = new FilenameFilter() {
			        public boolean accept(File dir, String name) {
			            return name.equalsIgnoreCase("user-recordings");
			        }
			    };
			    for (File recordingsFolder : idTB.listFiles(justUserRecordingsFilter)) {
				    FilenameFilter a18Filter = new FilenameFilter() {
				        public boolean accept(File dir, String name) {
				            return name.contains(".a18");
				        }
				    };
				    for (File idMsg : recordingsFolder.listFiles(a18Filter)) {
				    	totalFiles++;
				    	//log(idMsg.getAbsolutePath());
				    	long fileSize = idMsg.length();
				    	final int TOO_SMALL = 10;
				    	if (fileSize > TOO_SMALL) { //should change to official min size for good a18 
					    	in = new RandomAccessFile(idMsg.toString(), "r");
							len = readShort(in);
							temp = (readShort(in) & 0xffff) << 16;
							len += temp + 4; // 4: skip header 
							temp = readShort(in);				    		
				    	}
						if (temp != 0x3e80 || len > fileSize || fileSize <= TOO_SMALL) {
							// bad header - skip processing and move file into bad folder dir
							in.close();
							File newLocation = new File (OUTPUT_PATH + communityName.substring(0,3) + "_bad_" + idMsg.getName());
							if (!newLocation.exists()) {
								FileUtils.copyFile(idMsg, newLocation, false);
								log("COPIED unreadable to: " + newLocation.getAbsolutePath());
								// idMsg.renameTo(newLocation);
							}
							continue;
						}
						category = "bad"; // to capture log exceptions when no metadata for instance
						in.seek(len);
						temp = readInteger(in);
						if (temp == 1) {  // metadatat version -- otherwise there's a problem
							totalElements = readInteger(in); // number of metadata elements
							for (int i = 0; i < totalElements; i++) {
								// read next metadata item
								id = readShort(in); // metadata id
								len = readInteger(in);
								if (id == 0) { // category id
									in.skipBytes(3);
									category = "";
									for (int j = 0; j<len-3; j++) {
										char c;
										c = (char) in.readUnsignedByte();
										category += c;
									}
									break;
								} else { // skip other metadata
									in.skipBytes(len);
								}
							}
						}
						in.close();
						File newLocation = new File (OUTPUT_PATH + communityName.substring(0,3) + "_" + category + "_" + idMsg.getName());
						if (!newLocation.exists()) {
							FileUtils.copyFile(idMsg, newLocation, false);
							log("COPIED to " + newLocation.getAbsolutePath());
						} 
				    }
			    }
		    }
	    }
	    log("total files processed: " + String.valueOf(totalFiles));
//		byte[] fileContents = test.read(INPUT_FILE_NAME);
		//test.readAlternateImpl(INPUT_FILE_NAME);
		//write it back out to a different file name
//		test.write(fileContents, OUTPUT_FILE_NAME);
	}
    catch (FileNotFoundException ex) {
        log("File not found.");
        ex.printStackTrace();
      }
      catch (IOException ex) {
        log(ex);
        ex.printStackTrace();
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

  static int readShort (RandomAccessFile rd) {
	 int ret = 0, b, i;

	 try {
		 for (int l= 0; l<2; l++) {
			 b = rd.readByte() & 0xFF; // remove sign
			 i = b << (8 * l);
			 ret += i;
		 }
	 }
    catch (FileNotFoundException ex) {
        log("File not found.");
        log(ex);
      }
    catch (IOException ex) {
    	  ex.printStackTrace();
      }
	return ret;
  }
  
  private static void log(Object aThing){
    System.out.println(String.valueOf(aThing));
  }
} 
