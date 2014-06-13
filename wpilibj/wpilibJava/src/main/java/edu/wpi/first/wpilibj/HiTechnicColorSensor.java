/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.parsing.ISensor;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * HiTechnic NXT Color Sensor.
 *
 * This class allows access to a HiTechnic NXT Color Sensor on an I2C bus.
 * These sensors do not allow changing addresses so you cannot have more
 * than one on a single bus.
 *
 * Details on the sensor can be found here:
 *   http://www.hitechnic.com/index.html?lang=en-us&target=d17.html
 *
 */
public class HiTechnicColorSensor extends SensorBase implements ISensor, LiveWindowSendable {

    /**
     * An exception dealing with connecting to and communicating with the
     * HiTechnicCompass
     */
    public class ColorSensorException extends RuntimeException {

        /**
         * Create a new exception with the given message
         * @param message the message to pass with the exception
         */
        public ColorSensorException(String message) {
            super(message);
        }

    }

    /**
     * A set of three color values bundled into one object
     */
    public class RGB {
        public double red, green, blue;

        public double getRed() {
            return red;
        }
        public double getGreen() {
            return green;
        }
        public double getBlue() {
            return blue;
        }
    }

    public static class tColorSensorMode {
        public final int value;
        static final int kActive_val = 0;
        static final int kPassive_val = 1;
        static final int kRaw_val = 3;
        public static final tColorSensorMode kActive = new tColorSensorMode(kActive_val);
        public static final tColorSensorMode kPassive = new tColorSensorMode(kPassive_val);
        public static final tColorSensorMode kRaw = new tColorSensorMode(kRaw_val);

        private tColorSensorMode(int value) {
            this.value = value;
        }
    }

    private static final byte kAddress = 0x02;
    private static final byte kManufacturerBaseRegister = 0x08;
    private static final byte kManufacturerSize = 0x08;
    private static final byte kSensorTypeBaseRegister = 0x10;
    private static final byte kSensorTypeSize = 0x08;
    private static final byte kModeRegister = 0x41;
    private static final byte kColorRegister = 0x42;
    private static final byte kRedRegister = 0x43;
    private static final byte kGreenRegister = 0x44;
    private static final byte kBlueRegister = 0x45;
    private static final byte kRawRedRegister = 0x43;
    private static final byte kRawGreenRegister = 0x45;
    private static final byte kRawBlueRegister = 0x47;
    private I2C m_i2c;
    private int m_mode = tColorSensorMode.kActive.value;

    /**
     * Constructor.
     *
     * @param slot The slot of the digital module that the sensor is plugged into.
     */
    public HiTechnicColorSensor(int slot) {
        DigitalModule module = DigitalModule.getInstance(slot);
        m_i2c = module.getI2C(kAddress);

        // Verify Sensor
        final byte[] kExpectedManufacturer = "HiTechnc".getBytes();
        final byte[] kExpectedSensorType = "ColorPD ".getBytes();
        if (!m_i2c.verifySensor(kManufacturerBaseRegister, kManufacturerSize, kExpectedManufacturer)) {
            throw new ColorSensorException("Invalid Color Sensor Manufacturer");
        }
        if (!m_i2c.verifySensor(kSensorTypeBaseRegister, kSensorTypeSize, kExpectedSensorType)) {
            throw new ColorSensorException("Invalid Sensor type");
        }

        LiveWindow.addSensor("HiTechnicColorSensor", slot, this);
        UsageReporting.report(tResourceType.kResourceType_HiTechnicColorSensor, module.getModuleNumber()-1);
    }

    /**
     * Destructor.
     */
    public void free() {
        if (m_i2c != null) {
            m_i2c.free();
        }
        m_i2c = null;
    }

    /**
     * Get the estimated color.
     *
     * Gets a color estimate from the sensor corresponding to the
     * table found with the sensor or at the following site:
     * http://www.hitechnic.com/cgi-bin/commerce.cgi?preadd=action&key=NCO1038
     *
     * @return The estimated color.
     */
    public byte getColor() {
        byte[] color = new byte[1];
        if(m_mode != tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kActive);
        }
        m_i2c.read(kColorRegister, (byte) color.length, color);

        return color[0];
    }

    /**
     * Get the value of all three colors from a single sensor reading.
     * Using this method ensures that all three values come from the
     * same sensor reading, using the individual color methods provides
     * no such guarantee.
     *
     * The sensor must be in active mode to access the regular RGB data
     * if the sensor is not in active mode, it will be placed into active
     * mode by this method.
     *
     * @return RGB object with the three color values
     */
    public RGB getRGB() {
        byte[] colors = new byte[3];
        RGB result = new RGB();
        if(m_mode != tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kActive);
        }
        m_i2c.read(kRedRegister, (byte) colors.length, colors);

        result.red = (colors[0]&0xFFFF);
        result.green = (colors[1]&0xFFFF);
        result.blue = (colors[2]&0xFFFF);
        return result;
    }

    /**
     * Get the Red value.
     *
     * Gets the (0-255) red value from the sensor.
     *
     * The sensor must be in active mode to access the regular RGB data
     * if the sensor is not in active mode, it will be placed into active
     * mode by this method.
     *
     * @return The Red sensor value.
     */
    public int getRed() {
        byte[] red = new byte[1];
        if(m_mode != tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kActive);
        }
        m_i2c.read(kRedRegister, (byte) red.length, red);

        return (red[0]&0xFF);
    }

    /**
     * Get the Green value.
     *
     * Gets the(0-255) green value from the sensor.
     *
     * The sensor must be in active mode to access the regular RGB data
     * if the sensor is not in active mode, it will be placed into active
     * mode by this method.
     *
     * @return The Green sensor value.
     */
    public int getGreen() {
        byte[] green = new byte[1];
        if(m_mode != tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kActive);
        }
        m_i2c.read(kGreenRegister, (byte) green.length, green);

        return (green[0]&0xFF);
    }

    /**
     * Get the Blue value.
     *
     * Gets the raw (0-255) blue value from the sensor.
     *
     * The sensor must be in active mode to access the regular RGB data
     * if the sensor is not in active mode, it will be placed into active
     * mode by this method.
     *
     * @return The Blue sensor value.
     */
    public int getBlue() {
        byte[] blue = new byte[1];
        if(m_mode != tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kActive);
        }
        m_i2c.read(kBlueRegister, (byte) blue.length, blue);

        return (blue[0]&0xFF);
    }

    /**
     * Get the Raw Red value.
     *
     * Gets the (0-65536) raw red value from the sensor.
     *
     * The sensor must be in raw or passive mode to access the regular RGB data
     * if the sensor is not in raw or passive mode, it will be placed into raw
     * mode by this method.
     *
     * @return The Raw Red sensor value.
     */
    public double getRawRed() {
        byte[] rawRed = new byte[2];
        if(m_mode == tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kRaw);
        }
        m_i2c.read(kRawRedRegister, (byte) rawRed.length, rawRed);

        return (((int)rawRed[0]&0xFF) * (int) (1 << 8) + ((int)rawRed[1]&0xFF));
    }

    /**
     * Get the Raw Green value.
     *
     * Gets the (0-65536) raw green value from the sensor.
     *
     * The sensor must be in raw or passive mode to access the regular RGB data
     * if the sensor is not in raw or passive mode, it will be placed into raw
     * mode by this method.
     *
     * @return The Raw Green sensor value.
     */
    public double getRawGreen() {
        byte[] rawGreen = new byte[2];
        if(m_mode == tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kRaw);
        }
        m_i2c.read(kRawGreenRegister, (byte) rawGreen.length, rawGreen);

        return (((int)rawGreen[0]&0xFF) * (int) (1 << 8) + ((int)rawGreen[1]&0xFF));
    }

    /**
     * Get the Raw Blue value.
     *
     * Gets the (0-65536) raw blue value from the sensor.
     *
     * The sensor must be in raw or passive mode to access the regular RGB data
     * if the sensor is not in raw or passive mode, it will be placed into raw
     * mode by this method.
     *
     * @return The Raw Blue sensor value.
     */
    public double getRawBlue() {
        byte[] rawBlue = new byte[2];
        if(m_mode == tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kRaw);
        }
        m_i2c.read(kRawBlueRegister, (byte) rawBlue.length, rawBlue);

        return (((int)rawBlue[0]&0xFF) * (int) (1 << 8) + ((int)rawBlue[1]&0xFF));
    }

    /**
     * Get the raw value of all three colors from a single sensor reading.
     * Using this method ensures that all three values come from the
     * same sensor reading, using the individual color methods provides
     * no such guarantee.
     *
     * Gets the (0-65536) raw color values from the sensor.
     *
     * The sensor must be in raw or passive mode to access the regular RGB data
     * if the sensor is not in raw or passive mode, it will be placed into raw
     * mode by this method.
     *
     * @return An RGB object with the raw sensor values.
     */
    public RGB getRawRGB() {
        byte[] colors = new byte[6];
        RGB result = new RGB();
        if(m_mode == tColorSensorMode.kActive.value) {
            setMode(tColorSensorMode.kRaw);
        }
        m_i2c.read(kRawRedRegister, (byte) colors.length, colors);

        result.red = (((int)colors[0]&0xFF) * (int) (1 << 8) + ((int)colors[1]&0xFF));
        result.green = (((int)colors[2]&0xFF) * (int) (1 << 8) + ((int)colors[3]&0xFF));
        result.blue = (((int)colors[4]&0xFF) * (int) (1 << 8) + ((int)colors[5]&0xFF));
        return result;
    }

    /**
     * Set the Mode of the color sensor
     * This method is used to set the color sensor to one of the three modes,
     * active, passive or raw. The sensor defaults to active mode which uses the
     * internal LED and returns an interpreted color value and 3 8-bit RGB channel
     * values. Raw mode uses the internal LED and returns 3 16-bit RGB channel values.
     * Passive mode disables the internal LED and returns 3 16-bit RGB channel values.
     * @param mode The mode to set
     */
    public void setMode(tColorSensorMode mode) {
        m_i2c.write(kModeRegister, mode.value);
        m_mode = mode.value;
    }

    /*
     * Live Window code, only does anything if live window is activated.
     * TODO: Should this have its own type?
     */
    public String getSmartDashboardType() {
        return "Counter";
    }
    private ITable m_table;

    /**
     * {@inheritDoc}
     */
    public void initTable(ITable subtable) {
        m_table = subtable;
        updateTable();
    }

    /**
     * {@inheritDoc}
     */
    public ITable getTable() {
        return m_table;
    }

    /**
     * {@inheritDoc}
     */
    public void updateTable() {
        if (m_table != null) {
            if(m_mode == tColorSensorMode.kActive.value) {
                m_table.putNumber("Color", getColor());
            } else {
                m_table.putNumber("Color", 99);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void startLiveWindowMode() {}

    /**
     * {@inheritDoc}
     */
    public void stopLiveWindowMode() {}
}
