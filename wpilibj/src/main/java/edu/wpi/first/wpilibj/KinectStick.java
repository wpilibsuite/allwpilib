/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import edu.wpi.first.wpilibj.communication.FRCControl;
import edu.wpi.first.wpilibj.communication.UsageReporting;

/**
 * @author bradmiller
 * Handles input from the Joystick data sent by the FRC Kinect Server
 * when used with a Kinect device connected to the Driver Station.
 * Each time a value is requested the most recent value is returned.
 * Default gestures embedded in the FRC Kinect Server are described
 * in the document Getting Started with Microsoft Kinect for FRC.
 */
public class KinectStick extends GenericHID {

    private final static byte kJoystickDataID = 24;
    private final static byte kJoystickDataSize = 18;
    private int m_recentPacketNumber;

    private int m_id;

    static class JoystickDataBlock extends Structure {

        byte joystick1[] = new byte[6];
        short button1;
        byte joystick2[] = new byte[6];
        short button2;

        public static final int size = kJoystickDataSize - 2;

        JoystickDataBlock(Pointer backingMemory) {
            useMemory(backingMemory);
        }

        public void read() {
            backingNativeMemory.getBytes(0, joystick1, 0, 6);
            button1 = backingNativeMemory.getShort(6);
            backingNativeMemory.getBytes(8, joystick2, 0, 6);
            button2 = backingNativeMemory.getShort(14);
        }

        public void write() {
            backingNativeMemory.setBytes(0, joystick1, 0, 6);
            backingNativeMemory.setShort(6, button1);
            backingNativeMemory.setBytes(8, joystick2, 0, 6);
            backingNativeMemory.setShort(14, button2);
        }

        public int size() {
            return size;
        }
    }

    class JoystickData extends FRCControl.DynamicControlData {

        JoystickDataBlock data;

        {
            allocateMemory();
            data = new JoystickDataBlock(
                new Pointer(backingNativeMemory.address().toUWord().toPrimitive() + 2,
                            JoystickDataBlock.size));
        }

        public void read() {
            data.read();
        }

        public void write() {
            data.write();
        }

        public int size() {
            return kJoystickDataSize;
        }

        public void copy(JoystickData dest) {
            write();
            Pointer.copyBytes(backingNativeMemory, 0, dest.backingNativeMemory, 0, size());
            dest.read();
        }
    }
    JoystickData tempOutputData = new JoystickData();

    /**
     * Construct an instance of a KinectStick.
     * @param id which KinectStick to read, in the default gestures
     * an id of 1 corresponds to the left arm and 2 to the right arm.
     */
    public KinectStick(int id) {
        m_id = id;

        UsageReporting.report(UsageReporting.kResourceType_KinectStick, id);
    }

    /**
     * Update the data in this class with the latest data from the
     * Driver Station.
     */
    private void getData() {
        if (m_recentPacketNumber !=  DriverStation.getInstance().getPacketNumber()) {
            m_recentPacketNumber = DriverStation.getInstance().getPacketNumber();
            int retVal = FRCControl.getDynamicControlData(kJoystickDataID, tempOutputData, tempOutputData.size(), 5);
            if (retVal != 0) {
                System.err.println("Bad retval: " + retVal);
            }
        }
    }

    /**
     * Convert a value from a byte to a double in the
     * -1 to 1 range
     * @param rawValue the value to convert
     * @return the normalized value
     */
    private double normalize(byte rawValue) {
        if(rawValue >= 0)
            return rawValue / 127.0;
        else
            return rawValue / 128.0;
    }

    /**
     * Get the X value of the KinectStick. This axis
     * is unimplemented in the default gestures but can
     * be populated by teams editing the Kinect Server.
     * See (@link Joystick for axis number mapping)
     * @param hand Unused
     * @return The X value of the KinectStick
     */
    public double getX(Hand hand) {
        getData();
        return getRawAxis(Joystick.kDefaultXAxis);
    }

    /**
     * Get the Y value of the KinectStick. This axis
     * represents arm angle in the default gestures
     * See (@link Joystick for axis number mapping)
     * @param hand Unused
     * @return The Y value of the KinectStick
     */
    public double getY(Hand hand) {
        getData();
        return getRawAxis(Joystick.kDefaultYAxis);
    }

    /**
     * Get the Z value of the KinectStick. This axis
     * is unimplemented in the default gestures but can
     * be populated by teams editing the Kinect Server.
     * See (@link Joystick for axis number mapping)
     * @param hand Unused
     * @return The Z value of the KinectStick
     */
    public double getZ(Hand hand) {
        getData();
        return getRawAxis(Joystick.kDefaultZAxis);
    }

    /**
     * Get the Twist value of the KinectStick. This axis
     * is unimplemented in the default gestures but can
     * be populated by teams editing the Kinect Server.
     * See (@link Joystick for axis number mapping)
     * @return The Twist value of the KinectStick
     */
    public double getTwist() {
        getData();
        return getRawAxis(Joystick.kDefaultTwistAxis);
    }

    /**
     * Get the Throttle value of the KinectStick. This axis
     * is unimplemented in the default gestures but can
     * be populated by teams editing the Kinect Server.
     * See (@link Joystick for axis number mapping)
     * @return The Throttle value of the KinectStick
     */
    public double getThrottle() {
        getData();
        return getRawAxis(Joystick.kDefaultThrottleAxis);
    }

    /**
     * Get the value of the KinectStick axis.
     *
     * @param axis The axis to read [1-6].
     * @return The value of the axis
     */
    public double getRawAxis(int axis) {
        if (axis < 1 || axis > DriverStation.kJoystickAxes)
            return 0.0;

        getData();
        if (m_id == 1) {
            return normalize(tempOutputData.data.joystick1[axis-1]);
        } else {
            return normalize(tempOutputData.data.joystick2[axis-1]);
        }
    }

    /**
     * Get the button value for the button set as the default trigger in
     * (@link Joystick)
     *
     * @param hand Unused
     * @return The state of the button.
     */
    public boolean getTrigger(Hand hand) {
        getData();
        return (tempOutputData.data.button1 & (short) Joystick.kDefaultTriggerButton) != 0;
    }

    /**
     * Get the button value for the button set as the default top in
     * (@link Joystick)
     *
     * @param hand Unused
     * @return The state of the button.
     */
    public boolean getTop(Hand hand) {
        getData();
        return (tempOutputData.data.button1 & (short) Joystick.kDefaultTopButton) != 0;
    }

    /**
     * Get the button value for the button set as the default bumper in
     * (@link Joystick)
     *
     * @param hand Unused
     * @return The state of the button.
     */
    public boolean getBumper(Hand hand) {
        getData();
        return (tempOutputData.data.button1 & (short) 4) != 0;
    }

    /**
     * Get the button value for buttons 1 through 12. The default gestures
     * implement only 9 buttons.
     *
     * The appropriate button is returned as a boolean value.
     *
     * @param button The button number to be read.
     * @return The state of the button.
     */
    public boolean getRawButton(int button) {
        getData();
        return (tempOutputData.data.button1 & (short) (1 << (button - 1))) != 0;
    }
}
