/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                   */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.Enumeration;
import java.util.jar.Manifest;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 * Implement a Robot Program framework.
 * The RobotBase class is intended to be subclassed by a user creating a robot program.
 * Overridden autonomous() and operatorControl() methods are called at the appropriate time
 * as the match proceeds. In the current implementation, the Autonomous code will run to
 * completion before the OperatorControl code could start. In the future the Autonomous code
 * might be spawned as a task, then killed at the end of the Autonomous period.
 */
public abstract class RobotBase {
    /**
     * The VxWorks priority that robot code should work at (so Java code should run at)
     */
    public static final int ROBOT_TASK_PRIORITY = 101;

    /**
     * Boolean System property. If true (default), send System.err messages to the driver station.
     */
    public final static String ERRORS_TO_DRIVERSTATION_PROP = "first.driverstation.senderrors";

    protected final DriverStation m_ds;

    /**
     * Constructor for a generic robot program.
     * User code should be placed in the constructor that runs before the Autonomous or Operator
     * Control period starts. The constructor will run to completion before Autonomous is entered.
     *
     * This must be used to ensure that the communications code starts. In the future it would be
     * nice to put this code into it's own task that loads on boot so ensure that it runs.
     */
    protected RobotBase() {
        // TODO: StartCAPI();
        // TODO: See if the next line is necessary
        // Resource.RestartProgram();

//        if (getBooleanProperty(ERRORS_TO_DRIVERSTATION_PROP, true)) {
//            Utility.sendErrorStreamToDriverStation(true);
//        }
        NetworkTable.setServerMode();//must be before b
        m_ds = DriverStation.getInstance();
        NetworkTable.getTable("");  // forces network tables to initialize
        NetworkTable.getTable("LiveWindow").getSubTable("~STATUS~").putBoolean("LW Enabled", false);
    }

    /**
     * Free the resources for a RobotBase class.
     */
    public void free() {
    }

    /**
     * Determine if the Robot is currently disabled.
     * @return True if the Robot is currently disabled by the field controls.
     */
    public boolean isDisabled() {
        return m_ds.isDisabled();
    }

    /**
     * Determine if the Robot is currently enabled.
     * @return True if the Robot is currently enabled by the field controls.
     */
    public boolean isEnabled() {
        return m_ds.isEnabled();
    }

    /**
     * Determine if the robot is currently in Autonomous mode.
     * @return True if the robot is currently operating Autonomously as determined by the field controls.
     */
    public boolean isAutonomous() {
        return m_ds.isAutonomous();
    }

    /**
     * Determine if the robot is currently in Test mode
     * @return True if the robot is currently operating in Test mode as determined by the driver station.
     */
    public boolean isTest() {
        return m_ds.isTest();
    }

    /**
     * Determine if the robot is currently in Operator Control mode.
     * @return True if the robot is currently operating in Tele-Op mode as determined by the field controls.
     */
    public boolean isOperatorControl() {
        return m_ds.isOperatorControl();
    }

    /**
     * Indicates if new data is available from the driver station.
     * @return Has new data arrived over the network since the last time this function was called?
     */
    public boolean isNewDataAvailable() {
        return m_ds.isNewControlData();
    }

    /**
     * Provide an alternate "main loop" via startCompetition().
     */
    public abstract void startCompetition();

    public static boolean getBooleanProperty(String name, boolean defaultValue) {
        String propVal = System.getProperty(name);
        if (propVal == null) {
            return defaultValue;
        }
        if (propVal.equalsIgnoreCase("false")) {
            return false;
        } else if (propVal.equalsIgnoreCase("true")) {
            return true;
        } else {
            throw new IllegalStateException(propVal);
        }
    }

    /**
     * Starting point for the applications. Starts the OtaServer and then runs
     * the robot.
     * @throws javax.microedition.midlet.MIDletStateChangeException
     */
    public static void main(String args[]) { // TODO: expose main to teams?{
        boolean errorOnExit = false;
        
        // /* JNI Testing */
        // boolean booleanTest = true;
        // byte byteTest = (byte)0xa5;
        // char charTest = 'X';
        // short shortTest = 12346;
        // int intTest = 2987654;
        // long longTest = 897678665;
        // float floatTest = 45.123456f;
        // double doubleTest = 234E16;
        
        // FRCNetworkCommunicationsLibrary.JNIValueParameterTest(booleanTest, byteTest, charTest, shortTest,
        		// intTest, longTest, floatTest, doubleTest);
        
        // boolean booleanReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnBooleanTest(booleanTest);
        // System.out.println("Boolean Return: " + booleanReturn );

        // byte byteReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnByteTest(byteTest);
        // System.out.println("Byte Return: " + byteReturn );
        
        // char charReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnCharTest(charTest);
        // System.out.println("Char Return: " + charReturn );
        
        // short shortReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnShortTest(shortTest);
        // System.out.println("Short Return: " + shortReturn );
        
        // int intReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnIntTest(intTest);
        // System.out.println("Int Return: " + intReturn );
        
        // long longReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnLongTest(longTest);
        // System.out.println("Long Return: " + longReturn );
        
        // float floatReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnFloatTest(floatTest);
        // System.out.println("Float Return: " + floatReturn );
        
        // double doubleReturn = FRCNetworkCommunicationsLibrary.JNIValueReturnDoubleTest(doubleTest);
        // System.out.println("Double Return: " + doubleReturn );
        
        
        
        // String testValue = "This is a test string";
        
        // String returnValue = FRCNetworkCommunicationsLibrary.JNIObjectReturnStringTest(testValue);
        
        // System.out.println("String Return:" + returnValue);
        
        // ByteBuffer directBuffer = ByteBuffer.allocateDirect(4);
        
        // directBuffer.put(0, (byte)0xFA);
        // directBuffer.put(1, (byte)0xAB);
        // directBuffer.put(2, (byte)0xB4);
        // directBuffer.put(3, (byte)0xCD);
        
        // ByteBuffer returnBuffer1 = FRCNetworkCommunicationsLibrary.JNIObjectReturnByteBufferTest(directBuffer);
        
        // System.out.println("Return Buffer Capacity: " + returnBuffer1.capacity());
        // System.out.println("ByteBuffer1 Return0: " + returnBuffer1.get(0) );
        // System.out.println("ByteBuffer1 Return1: " + returnBuffer1.get(1) );
        // System.out.println("ByteBuffer1 Return2: " + returnBuffer1.get(2) );
        // System.out.println("ByteBuffer1 Return3: " + returnBuffer1.get(3) );
        
        
        // ByteBuffer directByteBuffer = ByteBuffer.allocateDirect(4);
        //set to little endian for C++
        // directByteBuffer.order(ByteOrder.LITTLE_ENDIAN);
        
        // directByteBuffer.putInt(0,2874933);
        
        // System.out.println("Param In: " + directByteBuffer.getInt(0));
        // System.out.println("Param In Byte0: " + directByteBuffer.get(0) );
        // System.out.println("Param In Byte1: " + directByteBuffer.get(1) );
        // System.out.println("Param In Byte2: " + directByteBuffer.get(2) );
        // System.out.println("Param In Byte3: " + directByteBuffer.get(3) );
        
        
        // ByteBuffer returnBuffer2 = FRCNetworkCommunicationsLibrary.JNIObjectAndParamReturnIntBufferTest(directByteBuffer.asIntBuffer());
        
        // System.out.println("Param Out: " + directByteBuffer.getInt(0));
        
        
        
        // System.out.println("Return Buffer Capacity: " + returnBuffer2.capacity());
        // System.out.println("ByteBuffer2 Return0: " + returnBuffer2.get(0) );
        // System.out.println("ByteBuffer2 Return1: " + returnBuffer2.get(1) );
        // System.out.println("ByteBuffer2 Return2: " + returnBuffer2.get(2) );
        // System.out.println("ByteBuffer2 Return3: " + returnBuffer2.get(3) );
        
        // System.out.println("Byte Order from C++" + returnBuffer2.order().toString());
        // System.out.println("ByteBuffer2 as Int" + returnBuffer2.getInt(0));
        //change to little endian
        // returnBuffer2.order(ByteOrder.LITTLE_ENDIAN);
        // System.out.println("ByteBuffer2 as Int" + returnBuffer2.getInt(0));
        
        /* End JNI Testing */

        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationReserve();
        FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramStarting();

        UsageReporting.report(tResourceType.kResourceType_Language, tInstances.kLanguage_Java);

		String robotName = "";
		Enumeration<URL> resources = null;
		try {
			resources = RobotBase.class.getClassLoader().getResources("META-INF/MANIFEST.MF");
		} catch (IOException e) {e.printStackTrace();}
		while (resources != null && resources.hasMoreElements()) {
		    try {
		    	Manifest manifest = new Manifest(resources.nextElement().openStream());
		    	robotName = manifest.getMainAttributes().getValue("Robot-Class");
		    } catch (IOException e) {e.printStackTrace();}
		}

        RobotBase robot;
		try {
			robot = (RobotBase) Class.forName(robotName).newInstance();
		} catch (InstantiationException|IllegalAccessException|ClassNotFoundException e) {
			System.err.println("WARNING: Robots don't quit!");
			System.err.println("ERROR: Could not instantiate robot "+robotName+"!");
			return;
		}

        try {
            robot.startCompetition();
        } catch (Throwable t) {
            t.printStackTrace();
            errorOnExit = true;
        } finally {
            // startCompetition never returns unless exception occurs....
            System.err.println("WARNING: Robots don't quit!");
            if (errorOnExit) {
                System.err.println("---> The startCompetition() method (or methods called by it) should have handled the exception above.");
            } else {
                System.err.println("---> Unexpected return from startCompetition() method.");
            }
        }
    }
}
