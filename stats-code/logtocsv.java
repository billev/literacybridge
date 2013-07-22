import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.Writer;

import org.omg.CORBA.PUBLIC_MEMBER;

/* V2 of logtocsv - for new folder structure and log format */

public class logtocsv {
	// Chg from orig version from Cliff - added \\raw-data
	private static final String INPUT_FILE_NAME = "C:\\Documents and Settings\\Arthur\\My Documents\\My Business\\LiteracyBridge\\Collected-data\\2012-02";
	private static final String OUTPUT_FILE_NAME = "C:\\Documents and Settings\\Arthur\\My Documents\\My Business\\LiteracyBridge\\Collected-data\\2012-02\\log-plays.csv";

	public static void main(String args[]) {
		Writer output;
		String logFileName;

		try{
		    output = new BufferedWriter(new FileWriter(OUTPUT_FILE_NAME));

		    File dir = new File(INPUT_FILE_NAME);
		    
		    // Communities -> TB id's -> Timestamps -> 
		    //  (1) Ignore self and parent, other folders and files, look for log.txt
		    //  (2) Ignore other folders and look for folder "log-archive"
		    //          a) Ignore self and parent, other folders and files, 
		    //											look for files log_ .txt
		    
		    for (File community : dir.listFiles()) {
			    if (".".equals(community.getName()) || "..".equals(community.getName()) || community.isFile()) {
				      continue;  // Ignore the self and parent aliases and if not a folder
				    }
			    for (File idTB : community.listFiles()) {
				    if (".".equals(idTB.getName()) || "..".equals(idTB.getName()) 
				    												|| idTB.isFile()) {
				      continue;  // Ignore the self and parent aliases and if not a folder
				    }
				    for (File timeStamp : idTB.listFiles()) {
					    if (".".equals(timeStamp.getName()) || "..".equals(timeStamp.getName()) 
					    												|| timeStamp.isFile()) {
					      continue;  // Ignore the self and parent aliases and if it's not a folder
					   // Add code to find earliest timeStamp
					    }
					// Assume timeStamp is the earliest    
				    
/*				    output.write(community.getName() + ",");				    
				    output.write(idTB.getName() + ",");
				    output.write(timeStamp.getName());
				    output.write("," + "\\log\\log.txt");
				    output.write("\n");*/
				    
				    logFileName = timeStamp.getPath() + "\\log\\log.txt";
				    LogFile aLogFile = new LogFile(logFileName, community.getName(), 
							idTB.getName(), output);
				    
				    // Look for log-archive under \timeStamp
				    for (File logArchive : timeStamp.listFiles()) {
				    	if (".".equals(logArchive.getName()) || "..".equals(logArchive.getName())
				    			|| logArchive.isFile()) {
				    		continue;
				    	}
				    	
//				    	}
					    if ( "log-archive".equals(logArchive.getName()) )  {
					//    	File[] listFiles = logArchive.listFiles();
					//		for (int i = 0; i < listFiles.length; i++) {
					    	
							for (File logFile : logArchive.listFiles()) {
					//			File logFile = listFiles[i];
								// Process log_ files
/*							    output.write(community.getName() + ",");
							    output.write(idTB.getName() + ",");
							    output.write(timeStamp.getName() + ",");
							    output.write(logArchive.getName() + ",");
							    output.write(logFile.getName() + ",");
							    output.write("\n");*/
							    
								logFileName = timeStamp.getPath() + "\\log-archive\\" + logFile.getName();
								
							    LogFile anotherLogFile = new LogFile(logFileName, community.getName(), 
	    								idTB.getName(), output);

							}
					      break;
					    }
				    }
				    
				   
				    }
			    }
		    }
			output.close();
			System.out.println("all done");
			}
	    catch (Exception e){//Catch exception if any
				System.err.println("Error: " + e.getMessage());
				e.printStackTrace();
			} finally {
			}
	}
}