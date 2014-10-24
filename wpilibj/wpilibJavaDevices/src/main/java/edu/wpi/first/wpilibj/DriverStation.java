/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.communication.HALControlWord;
import edu.wpi.first.wpilibj.communication.HALAllianceStationID;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.hal.PowerJNI;
import edu.wpi.first.wpilibj.Timer;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
public class DriverStation implements RobotState.Interface {

    /**
     * The size of the user status data
     */
    public static final int USER_STATUS_DATA_SIZE = FRCNetworkCommunicationsLibrary.USER_STATUS_DATA_SIZE;
    /**
     * Number of Joystick Ports
     */
    public static final int kJoystickPorts = 6;
    /**
     * Convert from raw values to volts
     */
    public static final double kDSAnalogInScaling = 5.0 / 1023.0;

    /**
     * The robot alliance that the robot is a part of
     */
    public enum Alliance { Red, Blue, Invalid }

    private static class DriverStationTask implements Runnable {

        private DriverStation m_ds;

        DriverStationTask(DriverStation ds) {
            m_ds = ds;
        }

        public void run() {
            m_ds.task();
        }
    } /* DriverStationTask */

    private static DriverStation instance = new DriverStation();

    private HALControlWord m_controlWord;
    private HALAllianceStationID m_allianceStationID;
    private short[][] m_joystickAxes = new short[kJoystickPorts][FRCNetworkCommunicationsLibrary.kMaxJoystickAxes];
    private short[][] m_joystickPOVs = new short[kJoystickPorts][FRCNetworkCommunicationsLibrary.kMaxJoystickPOVs];
    private int[] m_joystickButtons = new int[kJoystickPorts];

    private Thread m_thread;
    private final Object m_semaphore;
    private final Object m_dataSem;
    private int m_digitalOut;
    private volatile boolean m_thread_keepalive = true;
    private int m_updateNumber = 0;
    private double m_approxMatchTimeOffset = -1.0;
    private boolean m_userInDisabled = false;
    private boolean m_userInAutonomous = false;
    private boolean m_userInTeleop = false;
    private boolean m_userInTest = false;
    private boolean m_newControlData;
    private final ByteBuffer m_packetDataAvailableSem;

    /**
     * Gets an instance of the DriverStation
     *
     * @return The DriverStation.
     */
    public static DriverStation getInstance() {
        return DriverStation.instance;
    }

    /**
     * DriverStation constructor.
     *
     * The single DriverStation instance is created statically with the
     * instance static member variable.
     */
    protected DriverStation() {
        m_semaphore = new Object();
        m_dataSem = new Object();

        m_packetDataAvailableSem = HALUtil.initializeMutexNormal();
        FRCNetworkCommunicationsLibrary.setNewDataSem(m_packetDataAvailableSem);

        m_thread = new Thread(new DriverStationTask(this), "FRCDriverStation");
        m_thread.setPriority((Thread.NORM_PRIORITY + Thread.MAX_PRIORITY) / 2);

        m_thread.start();
    }

    /**
     * Kill the thread
     */
    public void release() {
        m_thread_keepalive = false;
    }

    /**
     * Provides the service routine for the DS polling thread.
     */
    private void task() {
        int safetyCounter = 0;
        while (m_thread_keepalive) {
        	HALUtil.takeMutex(m_packetDataAvailableSem);
        	synchronized (this) {
                getData();
            }
            synchronized (m_dataSem) {
                m_dataSem.notifyAll();
            }
            if (++safetyCounter >= 5) {
                //               System.out.println("Checking safety");
                MotorSafetyHelper.checkMotors();
                safetyCounter = 0;
            }
            if (m_userInDisabled) {
                FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramDisabled();
            }
            if (m_userInAutonomous) {
            	FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramAutonomous();
            }
            if (m_userInTeleop) {
            	FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramTeleop();
            }
            if (m_userInTest) {
            	FRCNetworkCommunicationsLibrary.FRCNetworkCommunicationObserveUserProgramTest();
            }
        }
    }

    /**
     * Wait for new data from the driver station.
     */
    public void waitForData() {
        waitForData(0);
    }

    /**
     * Wait for new data or for timeout, which ever comes first.  If timeout is
     * 0, wait for new data only.
     *
     * @param timeout The maximum time in milliseconds to wait.
     */
    public void waitForData(long timeout) {
        synchronized (m_dataSem) {
            try {
                m_dataSem.wait(timeout);
            } catch (InterruptedException ex) {
            }
        }
    }
    private static boolean lastEnabled = false;

    /**
     * Copy data from the DS task for the user.
     * If no new data exists, it will just be returned, otherwise
     * the data will be copied from the DS polling loop.
     */
    protected synchronized void getData() {
        // Get the status data
        m_controlWord = FRCNetworkCommunicationsLibrary.HALGetControlWord();

        // Get the location/alliance data
        m_allianceStationID = FRCNetworkCommunicationsLibrary.HALGetAllianceStation();

        // Get the status of all of the joysticks
        for(byte stick = 0; stick < kJoystickPorts; stick++) {
            m_joystickButtons[stick] = FRCNetworkCommunicationsLibrary.HALGetJoystickButtons(stick);
            m_joystickAxes[stick] = FRCNetworkCommunicationsLibrary.HALGetJoystickAxes(stick);
            m_joystickPOVs[stick] = FRCNetworkCommunicationsLibrary.HALGetJoystickPOVs(stick);
        }

        if (!lastEnabled && isEnabled()) {
            // If starting teleop, assume that autonomous just took up 15 seconds
            if (isAutonomous()) {
                m_approxMatchTimeOffset = Timer.getFPGATimestamp();
            } else {
                m_approxMatchTimeOffset = Timer.getFPGATimestamp() - 15.0;
            }
        } else if (lastEnabled && !isEnabled()) {
            m_approxMatchTimeOffset = -1.0;
        }
        lastEnabled = isEnabled();

        m_newControlData = true;
    }

    /**
     * Read the battery voltage.
     *
     * @return The battery voltage.
     */
    public double getBatteryVoltage() {
        IntBuffer status = ByteBuffer.allocateDirect(4).asIntBuffer();
        float voltage = PowerJNI.getVinVoltage(status);
        HALUtil.checkStatus(status);

        return voltage;
    }

    /**
     * Get the value of the axis on a joystick.
     * This depends on the mapping of the joystick connected to the specified port.
     *
     * @param stick The joystick to read.
     * @param axis The analog axis value to read from the joystick.
     * @return The value of the axis on the joystick.
     */
    public double getStickAxis(int stick, int axis) {
        if(stick < 0 || stick >= kJoystickPorts) {
            throw new RuntimeException("Joystick index is out of range, should be 0-3");
        }

        if (axis < 0 || axis >= m_joystickAxes[stick].length) {
            throw new RuntimeException("Joystick axis is out of range");
        }

        byte value = (byte)m_joystickAxes[stick][axis];

        if(value < 0) {
            return value / 128.0;
        } else {
            return value / 127.0;
        }
    }

    /**
     * Get the state of a POV on the joystick.
     *
     * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
     */
    public int getStickPOV(int stick, int pov) {
        if(stick < 0 || stick >= kJoystickPorts) {
            throw new RuntimeException("Joystick index is out of range, should be 0-3");
        }

        if (pov < 0 || pov >= m_joystickPOVs[stick].length) {
            throw new RuntimeException("Joystick POV is out of range");
        }

        return m_joystickPOVs[stick][pov];
    }

    /**
     * The state of the buttons on the joystick.
     * 12 buttons (4 msb are unused) from the joystick.
     *
     * @param stick The joystick to read.
     * @return The state of the buttons on the joystick.
     */
    public int getStickButtons(final int stick) {
        if(stick < 0 || stick >= kJoystickPorts) {
            throw new RuntimeException("Joystick index is out of range, should be 0-3");
        }

        return (int)m_joystickButtons[stick];
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be enabled.
     *
     * @return True if the robot is enabled, false otherwise.
     */
    public boolean isEnabled() {
        return m_controlWord.getEnabled();
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be disabled.
     *
     * @return True if the robot should be disabled, false otherwise.
     */
    public boolean isDisabled() {
        return !isEnabled();
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be running in autonomous mode.
     *
     * @return True if autonomous mode should be enabled, false otherwise.
     */
    public boolean isAutonomous() {
        return m_controlWord.getAutonomous();
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be running in test mode.
     * @return True if test mode should be enabled, false otherwise.
     */
    public boolean isTest() {
        return m_controlWord.getTest();
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be running in operator-controlled mode.
     *
     * @return True if operator-controlled mode should be enabled, false otherwise.
     */
    public boolean isOperatorControl() {
        return !(isAutonomous() || isTest());
    }

    /**
     * Has a new control packet from the driver station arrived since the last time this function was called?
     * @return True if the control data has been updated since the last call.
     */
    public synchronized boolean isNewControlData() {
        boolean result = m_newControlData;
        m_newControlData = false;
        return result;
    }

    /**
     * Get the current alliance from the FMS
     * @return the current alliance
     */
    public Alliance getAlliance() {
        switch (m_allianceStationID) {
            case Red1:
            case Red2:
            case Red3:
                return Alliance.Red;

            case Blue1:
            case Blue2:
            case Blue3:
                return Alliance.Blue;

            default:
                return Alliance.Invalid;
        }
    }

    /**
     * Gets the location of the team's driver station controls.
     *
     * @return the location of the team's driver station controls: 1, 2, or 3
     */
    public int getLocation() {
        switch (m_allianceStationID) {
            case Red1:
            case Blue1:
                return 1;

            case Red2:
            case Blue2:
                return 2;

            case Blue3:
            case Red3:
                return 3;

            default:
                return 0;
        }
    }

    /**
     * Is the driver station attached to a Field Management System?
     * Note: This does not work with the Blue DS.
     * @return True if the robot is competing on a field being controlled by a Field Management System
     */
    public boolean isFMSAttached() {
        return m_controlWord.getFMSAttached();
    }

    /**
     * Return the approximate match time
     * The FMS does not currently send the official match time to the robots
     * This returns the time since the enable signal sent from the Driver Station
     * At the beginning of autonomous, the time is reset to 0.0 seconds
     * At the beginning of teleop, the time is reset to +15.0 seconds
     * If the robot is disabled, this returns 0.0 seconds
     * Warning: This is not an official time (so it cannot be used to argue with referees)
     * @return Match time in seconds since the beginning of autonomous
     */
    public double getMatchTime() {
        if (m_approxMatchTimeOffset < 0.0) {
            return 0.0;
        }
        return Timer.getFPGATimestamp() - m_approxMatchTimeOffset;
    }
	
	/**
	 * Report error to Driver Station.
	 * Also prints error to System.err
	 * Optionally appends Stack trace to error message
	 * @param printTrace If true, append stack trace to error string
	 */
	public static void reportError(String error, boolean printTrace) {
		String errorString = error;
		if(printTrace) {
			errorString += " at ";
			StackTraceElement[] traces = Thread.currentThread().getStackTrace();
			for (int i=2; i<traces.length; i++)
			{
				errorString += traces[i].toString() + "\n";
			}
		}
		System.err.println(errorString);
		FRCNetworkCommunicationsLibrary.HALSetErrorData(errorString);
	}

    /** Only to be used to tell the Driver Station what code you claim to be executing
     *   for diagnostic purposes only
     * @param entering If true, starting disabled code; if false, leaving disabled code */
    public void InDisabled(boolean entering) {
        m_userInDisabled=entering;
    }

    /** Only to be used to tell the Driver Station what code you claim to be executing
    *   for diagnostic purposes only
     * @param entering If true, starting autonomous code; if false, leaving autonomous code */
    public void InAutonomous(boolean entering) {
        m_userInAutonomous=entering;
    }

    /** Only to be used to tell the Driver Station what code you claim to be executing
    *   for diagnostic purposes only
     * @param entering If true, starting teleop code; if false, leaving teleop code */
    public void InOperatorControl(boolean entering) {
        m_userInTeleop=entering;
    }

    /** Only to be used to tell the Driver Station what code you claim to be executing
     *   for diagnostic purposes only
     * @param entering If true, starting test code; if false, leaving test code */
    public void InTest(boolean entering) {
        m_userInTeleop = entering;
    }
}
