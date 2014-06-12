/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.simulation.MainNode;
import gazebo.msgs.GzDriverStation;
import gazebo.msgs.GzDriverStation.DriverStation.State;
import gazebo.msgs.GzJoystick.Joystick;

import org.gazebosim.transport.SubscriberCallback;

/**
 * Provide access to the network communication data to / from the Driver Station.
 */
public class DriverStation {
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


    private static DriverStation instance = new DriverStation();
    private final Object m_dataSem;
    private boolean m_userInDisabled = false;
    private boolean m_userInAutonomous = false;
    private boolean m_userInTeleop = false;
    private boolean m_userInTest = false;
    private boolean m_newControlData;
    private GzDriverStation.DriverStation state;
    private Joystick joysticks[] = new Joystick[4];

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
        m_dataSem = new Object();

        MainNode.subscribe("ds/state", GzDriverStation.DriverStation.getDefaultInstance(),
    		new SubscriberCallback<GzDriverStation.DriverStation>() {
				@Override public void callback(GzDriverStation.DriverStation msg) {
					synchronized (m_dataSem) {
						state = msg;
					}
				}
			}
        );
        
        for (int i = 1; i <= 4; i++) {
        	final int j = i;
        	MainNode.subscribe("ds/joysticks/"+i, Joystick.getDefaultInstance(),
        		new SubscriberCallback<Joystick>() {
					@Override public void callback(Joystick msg) {
						synchronized (m_dataSem) {
							joysticks[j] = msg;
						}
					}
				}
			);
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

    /**
     * Read the battery voltage from the specified AnalogChannel.
     *
     * This accessor assumes that the battery voltage is being measured
     * through the voltage divider on an analog breakout.
     *
     * @return The battery voltage.
     */
    public double getBatteryVoltage() {
    	return 12.0; // 12 volts all the time!
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
        if (axis < 1 || axis > kJoystickAxes || joysticks[stick] == null) {
            return 0.0;    		
    	}
    	return joysticks[stick].getAxes(axis - 1);
    }

    /**
     * The state of the buttons on the joystick.
     * 12 buttons (4 msb are unused) from the joystick.
     *
     * @param stick The joystick to read.
     * @return The state of the buttons on the joystick.
     */
    public boolean getStickButton(int stick, int button) {
    	if (joysticks[stick] == null) {
            return false;    		
    	}
    	return joysticks[stick].getButtons(button - 1);
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
    	throw new RuntimeException("The simulated DriverStation doesn't have analog inputs");
    }

    /**
     * Get values from the digital inputs on the Driver Station.
     * Return digital values from the Drivers Station. These values are typically used for buttons
     * and switches on advanced operator interfaces.
     * @param channel The digital input to get. Valid range is 1 - 8.
     * @return The value of the digital input
     */
    public boolean getDigitalIn(final int channel) {
    	throw new RuntimeException("The simulated DriverStation doesn't have digital inputs");
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
    	throw new RuntimeException("The simulated DriverStation doesn't have digital outputs");
    }

    /**
     * Get a value that was set for the digital outputs on the Driver Station.
     * @param channel The digital ouput to monitor. Valid range is 1 through 8.
     * @return A digital value being output on the Drivers Station.
     */
    public boolean getDigitalOut(final int channel) {
    	throw new RuntimeException("The simulated DriverStation doesn't have digital outputs");
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be enabled.
     *
     * @return True if the robot is enabled, false otherwise.
     */
    public boolean isEnabled() {
    	return state != null ? state.getEnabled() : false;
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
    	return state != null ? state.getState() == State.AUTO : false;
    }

    /**
     * Gets a value indicating whether the Driver Station requires the
     * robot to be running in test mode.
     * @return True if test mode should be enabled, false otherwise.
     */
    public boolean isTest() {
    	return state != null ? state.getState() == State.TEST : false;
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
        switch ('I') { // TODO: Always invalid
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
        return -1; // TODO: always -1
    }

    /**
     * Return the team number that the Driver Station is configured for
     * @return The team number
     */
    public int getTeamNumber() {
        return 348; // TODO
    }
    
    /**
     * Is the driver station attached to a Field Management System?
     * Note: This does not work with the Blue DS.
     * @return True if the robot is competing on a field being controlled by a Field Management System
     */
    public boolean isFMSAttached() {
        return false;
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
    	throw new RuntimeException("The simulated DriverStation doesn't support match times"); // TODO:
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
