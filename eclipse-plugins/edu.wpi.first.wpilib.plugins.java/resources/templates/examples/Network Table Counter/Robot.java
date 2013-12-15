package $package;

import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 * An example of a network table counter. 
 * 
 * @author Team $team-number
  */
public class Robot {

    private static NetworkTable table;

    /**
     * @param args
     */
    public static void main(String[] args) {
		NetworkTable.setServerMode();
		NetworkTable.setTeam(190);
		table = NetworkTable.getTable("SmartDashboard");
		System.out.println("Started up");
			
		long i = 0;
		while (true) {
		    System.out.println(i);
		    table.putNumber("i", i);
		    i++;
				
		    try {
		    	Thread.sleep(1000);
		    } catch (InterruptedException e) {
		    	// TODO Auto-generated catch block
		    	e.printStackTrace();
		    }
		}
		
    }

}
