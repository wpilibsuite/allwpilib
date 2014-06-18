/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;


/**
 *
 * @author dtjones
 */
public class ADXL345_I2C extends SensorBase {

    private static final byte kAddress = 0x1D;
    private static final byte kPowerCtlRegister = 0x2D;
    private static final byte kDataFormatRegister = 0x31;
    private static final byte kDataRegister = 0x32;
    private static final double kGsPerLSB = 0.00390625;
    private static final byte kPowerCtl_Link = 0x20, kPowerCtl_AutoSleep = 0x10, kPowerCtl_Measure = 0x08, kPowerCtl_Sleep = 0x04;
    private static final byte kDataFormat_SelfTest = (byte) 0x80, kDataFormat_SPI = 0x40, kDataFormat_IntInvert = 0x20, kDataFormat_FullRes = 0x08, kDataFormat_Justify = 0x04;

    public static class DataFormat_Range {

        /**
         * The integer value representing this enumeration
         */
        public final byte value;
        static final byte k2G_val = 0x00;
        static final byte k4G_val = 0x01;
        static final byte k8G_val = 0x02;
        static final byte k16G_val = 0x03;
        public static final DataFormat_Range k2G = new DataFormat_Range(k2G_val);
        public static final DataFormat_Range k4G = new DataFormat_Range(k4G_val);
        public static final DataFormat_Range k8G = new DataFormat_Range(k8G_val);
        public static final DataFormat_Range k16G = new DataFormat_Range(k16G_val);

        private DataFormat_Range(byte value) {
            this.value = value;
        }
    }

    public static class Axes {

        /**
         * The integer value representing this enumeration
         */
        public final byte value;
        static final byte kX_val = 0x00;
        static final byte kY_val = 0x02;
        static final byte kZ_val = 0x04;
        public static final Axes kX = new Axes(kX_val);
        public static final Axes kY = new Axes(kY_val);
        public static final Axes kZ = new Axes(kZ_val);

        private Axes(byte value) {
            this.value = value;
        }
    }

    public static class AllAxes {

        public double XAxis;
        public double YAxis;
        public double ZAxis;
    }
    private I2C m_i2c;

    /**
     * Constructor.
     *
     * @param range The range (+ or -) that the accelerometer will measure.
     */
    public ADXL345_I2C(I2C.Port port, DataFormat_Range range) {
        m_i2c = new I2C(port, kAddress);
		
        // Turn on the measurements
        m_i2c.write(kPowerCtlRegister, kPowerCtl_Measure);
        // Specify the data format to read
        m_i2c.write(kDataFormatRegister, kDataFormat_FullRes | range.value);

        UsageReporting.report(tResourceType.kResourceType_ADXL345, tInstances.kADXL345_I2C);
    }

    /**
     * Get the acceleration of one axis in Gs.
     *
     * @param axis The axis to read from.
     * @return Acceleration of the ADXL345 in Gs.
     */
    public double getAcceleration(Axes axis) {
        byte[] rawAccel = new byte[2];
        m_i2c.read(kDataRegister + axis.value, rawAccel.length, rawAccel);

        // Sensor is little endian... swap bytes
        return accelFromBytes(rawAccel[0], rawAccel[1]);
    }

    private double accelFromBytes(byte first, byte second) {
        short tempLow = (short) (first & 0xff);
        short tempHigh = (short) ((second << 8) & 0xff00);
        return (tempLow | tempHigh) * kGsPerLSB;
    }

    /**
     * Get the acceleration of all axes in Gs.
     *
     * @return Acceleration measured on all axes of the ADXL345 in Gs.
     */
    public AllAxes getAccelerations() {
        AllAxes data = new AllAxes();
        byte[] rawData = new byte[6];
        m_i2c.read(kDataRegister, rawData.length, rawData);

        // Sensor is little endian... swap bytes
        data.XAxis = accelFromBytes(rawData[0], rawData[1]);
        data.YAxis = accelFromBytes(rawData[2], rawData[3]);
        data.ZAxis = accelFromBytes(rawData[4], rawData[5]);
        return data;
    }

    // TODO: Support LiveWindow
}
