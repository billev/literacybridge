import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.Writer;


public class usageTocsv {
	private static final String INPUT_FILE_NAME = "V:\\Content Statistics\\raw-data";
	private static final String OUTPUT_FILE_NAME = "V:\\Content Statistics\\log-usage.csv";

	public static void main(String args[]) {
		Writer output;
		String strLine, cycle, strPeriod, time, logFile, strStart, strEnd;
		int intPeriod;
		try{
			// Open the file that is the first 
			// command line parameter
		    output = new BufferedWriter(new FileWriter(OUTPUT_FILE_NAME));
			output.write("community,id,period,cycle,day,start,on12am,end,off12am,");
			output.write("e1,p1,e1-2,p1-2,e2,p2,e9,p9,e0,p0,e0-1,p0-1,browsed,played,length,");
			output.write("corr,shutdown,except,notes\n");
		    File dir = new File(INPUT_FILE_NAME);
		    for (File community : dir.listFiles()) {
			    if (".".equals(community.getName()) || "..".equals(community.getName()) || community.isFile()) {
				      continue;  // Ignore the self and parent aliases.
				    }
			    log(community.getName());
			    for (File idTB : community.listFiles()) {
				    if (".".equals(idTB.getName()) || "..".equals(idTB.getName()) || idTB.isFile()) {
				      continue;  // Ignore the self and parent aliases.
				    }
				    logFile = idTB.getPath() + "\\log.txt";
				    log(logFile);
				    try {
				    	boolean abnormalShutdown = false;
				    	boolean midnightStartTime = false;
				    	boolean possibleCorruption = false;
				    	boolean newPeriod = false;
				    	boolean exception = false;
				    	int enteredCat1, enteredCat2, enteredCat12, enteredCat9, enteredCat0, enteredCat01;
				    	int playedCat1, playedCat2, playedCat12, playedCat9, playedCat0, playedCat01;
				    	int currentCat, totalPlays, playLength, totalPlayLength, browseCount;
				    	int realPoweredDays = 0;
				    	int intPrevPoweredDays = 0;
				    	int onMidnights, offMidnights;
				    	int intPoweredDays;
				    	String earlyTime, startTime, endTime, poweredDays, prevPoweredDays, notes;
				    	FileInputStream fstream = new FileInputStream(logFile);
					    // Get the object of DataInputStream
						DataInputStream in = new DataInputStream(fstream);
				        BufferedReader br = new BufferedReader(new InputStreamReader(in));
						//Read File Line By Line
				        intPeriod = 1;
				        cycle = "?";
						poweredDays = prevPoweredDays = "0000";
				        time = endTime = "0000h00m00s";
						earlyTime = "0000h01m30s";
						strLine = br.readLine();
						while (strLine != null) {
							if (strLine.startsWith("------")) {
						    	enteredCat1 = 0; 
						    	enteredCat2 = 0;
						    	enteredCat12 = 0;
						    	enteredCat9 = 0;
						    	enteredCat0 = 0;
						    	enteredCat01 = 0;
						    	playedCat1 = 0;
						    	playedCat2 = 0;
						    	playedCat12 = 0;
						    	playedCat9 = 0;
						    	playedCat0 = 0;
						    	playedCat01 = 0;
						    	onMidnights = 0;
						    	browseCount = 0;
						    	totalPlayLength = 0;
						    	intPoweredDays = 0;
						    	totalPlays = 0;
						    	currentCat = -1;
						    	notes = "";
						    	exception = false;

								strLine = br.readLine();
								if (strLine == null)
									break;
								if (strLine.startsWith("srn.") && strLine.length() >= 38) {
									poweredDays = strLine.substring(37);
									intPoweredDays = Integer.parseInt(poweredDays);
								}
								if ((intPoweredDays==0) || (intPoweredDays < intPrevPoweredDays)) {
									possibleCorruption = true;
									newPeriod = true;
								}
								strLine = br.readLine();
								if (strLine == null)
									break;								
								if (strLine.startsWith("CYCLE"))
									cycle = strLine.substring(6, 10);
								strLine = br.readLine();
								strLine = br.readLine();
								if (strLine == null)
									break;								
								if (strLine.startsWith("Apparently ABNORMAL shutdown")) {
									abnormalShutdown = true;
									newPeriod = true;
								}
								if (!strLine.matches("....h..m..s")) {
									strLine = br.readLine();
									if (strLine == null)
										break;
								}
								startTime = strLine;
								if (intPrevPoweredDays == intPoweredDays && startTime.compareTo(endTime) < 0)
									newPeriod = true;
								intPrevPoweredDays = intPoweredDays;
								if (newPeriod)
									intPeriod++;
								while ((strLine = br.readLine()) != null) {
									if (strLine.startsWith("poweredDays=")) {
										onMidnights++;
									} else
									if (strLine.startsWith("Halting") || strLine.startsWith("Reset")) {
										strLine = br.readLine();
										if (strLine == null)
											break;								
										endTime = strLine;
										break;
									} else
									if (strLine.startsWith("*** #")) {
										notes += strLine;
										exception = true;
									} else
									if (strLine.startsWith("Category: ")) {
										if (strLine.substring(10).equals("1")) {
											currentCat = 1;
											enteredCat1++;
										} else if (strLine.substring(10).equals("2")) {
											currentCat = 2;
											enteredCat2++;											
										} else if (strLine.substring(10).equals("1-2")) {
											currentCat = 12;
											enteredCat12++;											
										} else if (strLine.substring(10).equals("9")) {
											currentCat = 9;
											enteredCat9++;											
										} else if (strLine.substring(10).equals("0")) {
											currentCat = 0;
											enteredCat0++;											
										} else if (strLine.substring(10).equals("$0-1")) {
											currentCat = 10;
											enteredCat01++;											
										}
									} else 
									if (strLine.startsWith("TIME PLAYED: ")) {
										try {
											playLength = Integer.parseInt(strLine.substring(13, 16));
										} catch (java.lang.NumberFormatException e) {
											e.printStackTrace();
											playLength = 0;
										}
										if (playLength >= 10) {
											totalPlays++;
											totalPlayLength += playLength;
											switch (currentCat) {
												case 1: playedCat1++;
														break;
												case 2: playedCat2++;
														break;
												case 12: playedCat12++;
														break;
												case 0:	playedCat0++;
														break;
												case 9: playedCat9++;
														break;
												case 10: playedCat01++;
														break;
											}
										} else
											browseCount++;
									}
								}
								// off time
								offMidnights = 0;
								while ((strLine = br.readLine()) != null) {
									if (strLine.startsWith("------"))
										break;
									if (strLine.startsWith("poweredDays=")) {
										strLine = br.readLine();
										if (strLine == null)
											break;								
										if (strLine.startsWith("back from Halt - RTC Alarm fired"))
											offMidnights++;
									}									
								}
								
								output.write(community.getName());
								output.write(",");
								output.write(idTB.getName());
								output.write(",");
								strPeriod = idTB.getName() + ":" + String.valueOf(intPeriod);
								output.write(strPeriod);
								output.write(",");
								output.write(cycle);
								output.write(",");
								output.write(poweredDays);
								output.write(",");					
								output.write(startTime);
								output.write(",");
								output.write(String.valueOf(onMidnights));
								output.write(",");
								output.write(endTime);
								output.write(",");
								output.write(String.valueOf(offMidnights));
								output.write(",");
								output.write(String.valueOf(enteredCat1));
								output.write(",");
								output.write(String.valueOf(playedCat1));
								output.write(",");
								output.write(String.valueOf(enteredCat12));
								output.write(",");
								output.write(String.valueOf(playedCat12));
								output.write(",");
								output.write(String.valueOf(enteredCat2));
								output.write(",");
								output.write(String.valueOf(playedCat2));
								output.write(",");
								output.write(String.valueOf(enteredCat9));
								output.write(",");
								output.write(String.valueOf(playedCat9));
								output.write(",");
								output.write(String.valueOf(enteredCat0));
								output.write(",");
								output.write(String.valueOf(playedCat0));
								output.write(",");
								output.write(String.valueOf(enteredCat01));
								output.write(",");
								output.write(String.valueOf(playedCat01));
								output.write(",");
								output.write(String.valueOf(browseCount));
								output.write(",");
								output.write(String.valueOf(totalPlays));
								output.write(",");
								output.write(String.valueOf(totalPlayLength));
								output.write(",");
								if (possibleCorruption) 
									output.write("C,");
								else
									output.write("_,");
								if (abnormalShutdown) 
									output.write("A,");
								else
									output.write("_,");

								if (exception) 
									output.write("X,");
								else
									output.write("_,");
								output.write(notes);
								output.write(",");
								output.write(logFile);
								output.write("\n");
								if (strLine == null) {
									strLine = br.readLine();
								}
							} else 
								strLine = br.readLine();

							/*else if (strLine.startsWith("poweredDays=")) {
									poweredDays = strLine.substring(12);
							else if (strLine.matches("....h..m..s")) {
								if (strLine.compareTo(time) < 0) // time has been reset
									poweredDays = "?";
								time = strLine;
							}
							*/
						}
						//Close the input stream
						in.close();
				    }
				    catch (FileNotFoundException e) {
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
  private static void log(Object aThing){
	  System.out.println(String.valueOf(aThing));
  }

}
