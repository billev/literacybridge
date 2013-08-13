import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.Writer;

public class LogFile {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
	}

	String strLine, category, fileName, timePlayed;
	String cycle, maxvolume, poweredDays, time;
	int spaceAfterPLAYED, spaceAfterFilename;
	int volumeAdjust;
		
	public LogFile() {
	}
		
	public LogFile(String name, String communityName, 
								String idTBName, Writer output) {
	    try {
			FileInputStream fstream = new FileInputStream(name);
		    // Get the object of DataInputStream
			DataInputStream in = new DataInputStream(fstream);
	        BufferedReader br = new BufferedReader(new InputStreamReader(in));
			//Read File Line By Line
			category = "?";
			
			while ((strLine = br.readLine())!= null) {
							
				// keep track of latest category in file
				if (strLine.contains("Category")) {
					category = strLine.substring(45);
				}
				else if (strLine.contains(":PLAYED ")) {

					output.write(communityName); 	output.write(",");
					output.write(idTBName); 		output.write(",");
					output.write("?");				output.write(",");					
					output.write("?");				output.write(",");

					if (category.equals("1"))
						category="AGRIC";
					else if (category.equals("1-2"))
						category="LIVESTOCK";
					else if (category.equals("2"))
						category = "HEALTH";
					else if (category.equals("9"))
						category = "FEEDBACK";
					else if (category.equals("0"))
						category = "OTHER";
					else if (category.equals("$0-1"))
						category = "TB";

					output.write(category); 		output.write(",");
					
					// Extract filename
					
					spaceAfterPLAYED = strLine.indexOf(" ");
					spaceAfterFilename = strLine.indexOf(" ", spaceAfterPLAYED + 1);
					fileName = strLine.substring(spaceAfterPLAYED + 1, spaceAfterFilename);
				    output.write(fileName);			output.write(",");
				    
				    // Extract timePlayed
				    
				    timePlayed = strLine.substring(spaceAfterFilename+1, spaceAfterFilename + 5);
				    output.write(timePlayed);		output.write(",");
					
					output.write("\n");
				}

				
//				    catch (Exception e){//Catch exception if any
//						System.err.println("Error: " + e.getMessage());
//						e.printStackTrace();
//					} 
	    	}
			in.close();
	    }
			
			    catch (Exception e){//Catch exception if any
					System.err.println("Error: " + e.getMessage());
					e.printStackTrace();
			    }

		} 
//    catch (FileNotFoundException e) {
//    }	
}
