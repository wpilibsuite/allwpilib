/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.communication.FRCCommonControlData;
import edu.wpi.first.wpilibj.communication.FRCCommonControlMasks;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.parsing.IInputOutput;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
public class DriverStation implements IInputOutput {

    /**
     * The size of the user status data
     */
    public static final int USER_STATUS_DATA_SIZE = FRCNetworkCommunicationsLibrary.USER_STATUS_DATA_SIZE;
    /**
     * Slot for the analog module to read the battery
     */
    public static final int kBatterySlot = 1;
    /**
     * Analog channel to read the battery
     */
    public static final int kBatteryChannel = 8;
    /**
     * Number of Joystick Ports
     */
    public static final int kJoystickPorts = 4;
    /**
     * Number of Joystick Axes
     */
    public static final int kJoystickAxes = 6;
    /**
     * Convert from raw values to volts
     */
    public static final double kDSAnalogInScaling = 5.0 / 1023.0;

    /**
     * The robot alliance that the robot is a part of
     */
    public static class Alliance {

        /** The integer value representing this enumeration. */
        public final int value;
        /** The Alliance name. */
        public final String name;
        public static final int kRed_val = 0;
        public static final int kBlue_val = 1;
        public static final int kInvalid_val = 2;
        /** alliance: Red */
        public static final Alliance kRed = new Alliance(kRed_val, "Red");
        /** alliance: Blue */
        public static final Alliance kBlue = new Alliance(kBlue_val, "Blue");
        /** alliance: Invalid */
        public static final Alliance kInvalid = new Alliance(kInvalid_val, "invalid");

        private Alliance(int value, String name) {
            this.value = value;
            this.name = name;
        }
    } /* Alliance */


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
    private FRCCommonControlData m_controlData;
    private AnalogChannel m_batteryChannel;
    private Thread m_thread;
    private final Object m_semaphore;
    private final Object m_dataSem;
    private int m_digitalOut;
    private volatile boolean m_thread_keepalive = true;
    private final Dashboard m_dashboardDefaultHigh;
    private final Dashboard m_dashboardDefaultLow;
    private IDashboard m_dashboardInUseHigh;
    private IDashboard m_dashboardInUseLow;
    private int m_updateNumber = 0;
    private double m_approxMatchTimeOffset = -1.0;
    private boolean m_userInDisabled = false;
    private boolean m_userInAutonomous = false;
    private boolean m_userInTeleop = false;
    private boolean m_userInTest = false;
    private boolean m_newControlData;
    private final ByteBuffer m_packetDataAvailableSem;
    // XXX: Add DriverStationEnhancedIO back
    // private DriverStationEnhancedIO m_enhancedIO = new DriverStationEnhancedIO();

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
        m_controlData = new FRCCommonControlData();
        m_semaphore = new Object();
        m_dataSem = new Object();

        m_dashboardInUseHigh = m_dashboardDefaultHigh = new Dashboard(m_semaphore);
        m_dashboardInUseLow = m_dashboardDefaultLow = new Dashboard(m_semaphore);

        // m_controlData is initialized in constructor FRCCommonControlData.

	// XXX: Uncomment when analogChannel is fixed
        //m_batteryChannel = new AnalogChannel(kBatterySlot, kBatteryChannel);

        m_packetDataAvailableSem = HALUtil.initializeMutexNormal();
        
        // set the byte order
        m_packetDataAvailableSem.order(ByteOrder.LITTLE_ENDIAN);
        
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
                // XXX: Add DriverStationEnhancedIO back
                // m_enhancedIO.updateData();
                setData();
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
    	FRCNetworkCommunicationsLibrary.getCommonControlData(m_controlData);

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
     * Copy status data from the DS task for the user.
     * This is used primarily to set digital outputs on the DS.
     */
    protected void setData() {
        synchronized (m_semaphore) {
            FRCNetworkCommunicationsLibrary.setStatusData((float) getBatteryVoltage(),
            						 (byte) m_digitalOut,
                                     (byte) m_updateNumber,
                                     new String(m_dashboardInUseHigh.getBytes()),
                                     m_dashboardInUseHigh.getBytesLength(),
                                     new String(m_dashboardInUseLow.getBytes()),
                                     m_dashboardInUseLow.getBytesLength());
            m_dashboardInUseHigh.flush();
            m_dashboardInUseLow.flush();
        }
    }

    /**
     * Read the battery voltage from the specified AnalogChannel.
     *
     * This accessor assumes that the battery voltage is being measured
     * through the voltage divider on an analog breakout.
     *
     * @return The battery voltage.
     */
    public double getBatteryVoltage() {
        // The Analog bumper has a voltage divider on the battery source.
        // Vbatt *--/\/\/\--* Vsample *--/\/\/\--* Gnd
        //         680 Ohms            1000 Ohms
	// XXX: Uncomment this when analog channel is fixed
        //return m_batteryChannel.getAverageVoltage() * (1680.0 / 1000.0);
	return 12.0;
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
        if (axis < 1 || axis > kJoystickAxes) {
            return 0.0;
        }

        int value;
        switch (stick) {
        case 1:
            value = m_controlData.stick0Axes[axis - 1];
            break;
        case 2:
            value = m_controlData.stick1Axes[axis - 1];
            break;
        case 3:
            value = m_controlData.stick2Axes[axis - 1];
            break;
        case 4:
            value = m_controlData.stick3Axes[axis - 1];
            break;
        default:
            return 0.0;
        }

        double result;
        if (value < 0) {
            result = ((double) value) / 128.0;
        } else {
            result = ((double) value) / 127.0;
        }

        // wpi_assert(result <= 1.0 && result >= -1.0);
        if (result > 1.0) {
            result = 1.0;
        } else if (result < -1.0) {
            result = -1.0;
        }
        return result;
    }

    /**
     * The state of the buttons on the joystick.
     * 12 buttons (4 msb are unused) from the joystick.
     *
     * @param stick The joystick to read.
     * @return The state of the buttons on the joystick.
     */
    public int getStickButtons(final int stick) {
        switch (stick) {
        case 1:
            return m_controlData.stick0Buttons;
        case 2:
            return m_controlData.stick1Buttons;
        case 3:
            return m_controlData.stick2Buttons;
        case 4:
            return m_controlData.stick3Buttons;
        default:
            return 0;
        }
    }

    /**
     * Get an analog voltage from the Driver Station.
     * The analog values are returned as voltage values for the Driver Station analog inputs.
     * These inputs are typically used for advanced operator interfaces consisting of potentiometers
     * or resistor networks representing values on a rotary switch.
     *
     * @param channel The analog input channel on the driver station to read from. Valid range is 1 - 4.
     * @return The analog voltage on the input.
     */
    public double getAnalogIn(final int channel) {
        switch (channel) {
        case 1:
            return kDSAnalogInScaling * m_controlData.analog1;
        case 2:
            return kDSAnalogInScaling * m_controlData.analog2;
        case 3:
            return kDSAnalogInScaling * m_controlData.analog3;
        case 4:
            return kDSAnalogInScaling * m_controlData.analog4;
        default:
            return 0.0;
        }
    }

    /**
     * Get values from the digital inputs on the Driver Station.
     * Return digital values from the Drivers Station. These values are typically used for buttons
     * and switches on advanced operator interfaces.
     * @param channel The digital input to get. Valid range is 1 - 8.
     * @return The value of the digital input
     */
    public boolean getDigitalIn(final int channel) {
        return ((m_controlData.dsDigitalIn >> (channel - 1)) & 0x1) == 0x1;
    }

    /**
     * Set a value for the digital outputs on the Driver Station.
     *
     * Control digital outputs on the Drivers Station. These values are typically used for
     * giving feedback on a custom operator station such as LEDs.
     *
     * @param channel The digital output to set. Valid range is 1 - 8.
     * @param value The state to set the digital output.
     */
    public void setDigitalOut(final int channel, final boolean value) {
        m_digitalOut &= ~(0x1 << (channel - 1));
        m_digitalOut |= ((value ? 1 : 0) << (channel - 1));
    }

    /**
     * Get a value that was set for the digital outputs on the Driver Station.
     * @param channel The digital ouput to monitor. Valid range is 1 through 8.
     * @return A digital value being output on the Drivers Station.
     */
    public boolean getDigitalOut(final int channel) {
        return ((m_digitalOut >> (channel - 1)) & 0x1) == 0x1;
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be enabled.
     *
     * @return True if the robot is enabled, false otherwise.
     */
    public boolean isEnabled() {
        return (m_controlData.control & FRCCommonControlMasks.ENABLED) != 0;
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
        return (m_controlData.control & FRCCommonControlMasks.AUTONOMOUS) != 0;
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be running in test mode.
     * @return True if test mode should be enabled, false otherwise.
     */
    public boolean isTest() {
        return (m_controlData.control & FRCCommonControlMasks.TEST) != 0;
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
     * Return the DS packet number.
     * The packet number is the index of this set of data returned by the driver station.
     * Each time new data is received, the packet number (included with the sent data) is returned.
     *
     * @return The DS packet number.
     */
    public int getPacketNumber() {
        return m_controlData.packetIndex;
    }

    /**
     * Get the current alliance from the FMS
     * @return the current alliance
     */
    public Alliance getAlliance() {
        switch (m_controlData.dsID_Alliance) {
        case 'R':
            return Alliance.kRed;
        case 'B':
            return Alliance.kBlue;
        default:
            return Alliance.kInvalid;
        }
    }

    /**
     * Gets the location of the team's driver station controls.
     *
     * @return the location of the team's driver station controls: 1, 2, or 3
     */
    public int getLocation() {
        return m_controlData.dsID_Position - '0';
    }

    /**
     * Return the team number that the Driver Station is configured for
     * @return The team number
     */
    public int getTeamNumber() {
        return m_controlData.teamID;
    }

    /**
     * Sets the dashboard packer to use for sending high priority user data to a
     * dashboard receiver. This can idle or restore the default packer.
     * (Initializing SmartDashboard sets the high priority packer in use, so
     * beware that the default packer will then be idle. You can restore the
     * default high priority packer by calling
     * {@code setDashboardPackerToUseHigh(getDashboardPackerHigh())}.)
     *
     * @param dashboard any kind of IDashboard object
     */
    public void setDashboardPackerToUseHigh(IDashboard dashboard) {
        m_dashboardInUseHigh = dashboard;
    }

    /**
     * Gets the default dashboard packer for sending high priority user data to
     * a dashboard receiver. This instance stays around even after a call to
     * {@link #setDashboardPackerToUseHigh} changes which packer is in use.
     *
     * @return the default Dashboard object; it may be idle
     */
    public Dashboard getDashboardPackerHigh() {
        return m_dashboardDefaultHigh;
    }

    /**
     * Gets the dashboard packer that's currently in use for sending high
     * priority user data to a dashboard receiver. This can be any kind of
     * IDashboard.
     *
     * @return the current IDashboard object
     */
    public IDashboard getDashboardPackerInUseHigh() {
        return m_dashboardInUseHigh;
    }

    /**
     * Sets the dashboard packer to use for sending low priority user data to a
     * dashboard receiver. This can idle or restore the default packer.
     *
     * @param dashboard any kind of IDashboard object
     */
    public void setDashboardPackerToUseLow(IDashboard dashboard) {
        m_dashboardInUseLow = dashboard;
    }

    /**
     * Gets the default dashboard packer for sending low priority user data to
     * a dashboard receiver. This instance stays around even after a call to
     * {@link #setDashboardPackerToUseLow} changes which packer is in use.
     *
     * @return the default Dashboard object; it may be idle
     */
    public Dashboard getDashboardPackerLow() {
        return m_dashboardDefaultLow;
    }

    /**
     * Gets the dashboard packer that's currently in use for sending low
     * priority user data to a dashboard receiver. This can be any kind of
     * IDashboard.
     *
     * @return the current IDashboard object
     */
    public IDashboard getDashboardPackerInUseLow() {
        return m_dashboardInUseLow;
    }

    /**
     * Gets the status data monitor
     * @return The status data monitor for use with IDashboard objects which must
     * send data across the network.
     */
    public Object getStatusDataMonitor() {
        return m_semaphore;
    }

    /**
     * Increments the internal update number sent across the network along with
     * status data.
     */
    void incrementUpdateNumber() {
        synchronized (m_semaphore) {
            m_updateNumber++;
        }
    }

    /**
     * Is the driver station attached to a Field Management System?
     * Note: This does not work with the Blue DS.
     * @return True if the robot is competing on a field being controlled by a Field Management System
     */
    public boolean isFMSAttached() {
        return (m_controlData.control & FRCCommonControlMasks.FMS_ATTATCHED) > 0;
    }

    /**
     * Get the interface to the enhanced IO of the new driver station.
     * @return An enhanced IO object for the advanced features of the driver
     * station.
     */
    // XXX: Add DriverStationEnhancedIO back
    // public DriverStationEnhancedIO getEnhancedIO() {
    //    return m_enhancedIO;
    // }

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
