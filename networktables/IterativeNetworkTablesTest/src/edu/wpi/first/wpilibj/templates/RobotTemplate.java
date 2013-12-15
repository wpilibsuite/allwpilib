/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.templates;


import com.sun.squawk.GC;
import com.sun.squawk.VM;
import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import java.io.IOException;
import java.util.Hashtable;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the IterativeRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class RobotTemplate extends IterativeRobot {
    int numVars = 100;

    static {
        try {
            NetworkTable.setServerMode();
            NetworkTable.initialize();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    NetworkTable table = NetworkTable.getTable("SmartDashboard");
    //Hashtable table = new Hashtable();
    String[] vars = new String[numVars];
    {
        for (int i = 0; i < numVars; i++) {
            vars[i] = "Variable"+i;
        }
    }
    
    double counter = 1;
    boolean value = false;

    public void teleopInit() {
        totTime = System.currentTimeMillis() - startTime;
        waitTime = VM.Stats.getTotalWaitTime() - waitTime;
        System.out.println("Baseline CPU Usage: "+(100*(1-(waitTime/((double) totTime))))+"%");
        System.out.println("\n");
        counter = 0;
        initProfiling(); 
    }
    
    /**
     * This function is called periodically during operator control
     */
    public void teleopPeriodic() {
        counter++;
        value = !value;
        for (int v = 0; v < numVars; v++) {
            table.putBoolean(vars[v], value);
            //table.putNumber(vars[v], counter);
            //table.putBoolean(vars[v], value);
            //table.putString(vars[v], "String");
        }
    }
    
    public void disabledInit() {
        endProfiling();
        printReport(numVars, counter);
        startTime = System.currentTimeMillis();
        waitTime = VM.Stats.getTotalWaitTime();
    }
    
    long startTime, totTime, waitTime, gcTime, bytesAllocated;
    Object monitor;
    public void initProfiling() {
        contentions = VM.Stats.getContendedMontorEnterCount();
        monitors = VM.Stats.getMonitorsAllocatedCount();
        threadSwitches = VM.Stats.getThreadSwitchCount();
        exceptions = VM.Stats.getThrowCount();
        waitTime = VM.Stats.getTotalWaitTime();

        monitor = new Object();
        bytesAllocated = GC.getCollector().getBytesAllocatedTotal();
        VM.collectGarbage(true);
        gcTime = GC.getCollector().getTotalGCTime();
        startTime = System.currentTimeMillis();
    }
    
    double runTime;
    int contentions, monitors, threadSwitches, exceptions;
    public void endProfiling() {
        totTime = System.currentTimeMillis() - startTime;
        bytesAllocated = GC.getCollector().getBytesAllocatedTotal() - bytesAllocated;
        //VM.collectGarbage(true);
        gcTime = GC.getCollector().getTotalGCTime() - gcTime;

        runTime = ((double) totTime)/1000;
        contentions = VM.Stats.getContendedMontorEnterCount() - contentions;
        VM.Stats.getMonitorsAllocatedCount();
        monitors = VM.Stats.getMonitorsAllocatedCount() - monitors;
        threadSwitches = VM.Stats.getThreadSwitchCount() - threadSwitches;
        exceptions = VM.Stats.getThrowCount() - exceptions;
        waitTime = VM.Stats.getTotalWaitTime() - waitTime;
    }
    
    public void printReport(int numVars, double loops) {
        System.out.println("REPORT:");
        System.out.println("Test of "+numVars+" vars and "+loops+" loops took "
                +runTime+" seconds.");
        System.out.println("CPU Usage: "+(100*(1-(waitTime/((double) totTime))))+"%");
        System.out.println();
        System.out.println("Update Frequency (updates/second): "+
                (((double)(numVars*loops))/(runTime-(((double)waitTime)/1000))));
        System.out.println("Time per update (microseconds/update): "+
                (1000000*(runTime-(((double)waitTime)/1000))/((double)(numVars*loops))));
        System.out.println("Bytes Allocated: "+bytesAllocated);
        System.out.println("Bytes per update (bytes/update): "+
                (bytesAllocated/((double)(numVars*loops))));
        System.out.println("GC Time (ms): "+gcTime);
        System.out.println("GCTime per update (microseconds/update): "+
                (1000*gcTime/((double)(numVars*loops))));
        System.out.println();
        System.out.println("Number of contentions: "+contentions);
        System.out.println("Number of monitors used: "+monitors);
        System.out.println("Number of thread switches: "+threadSwitches);
        System.out.println("Number of exceptions: "+exceptions);
        System.out.println("Time spent waiting on IO: "+waitTime);
        System.out.println();
        //VM.Stats.printHeapStats(monitor, false);

        
        System.out.println(); System.out.println();
    }
}
