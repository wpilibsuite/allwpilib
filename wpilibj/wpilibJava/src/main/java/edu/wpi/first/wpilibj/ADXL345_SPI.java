/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;

/**
 *
 * @author dtjones
 * @author mwills
 */
public class ADXL345_SPI extends SensorBase implements Accelerometer {
	private static final int kPowerCtlRegister = 0x2D;
	private static final int kDataFormatRegister = 0x31;
	private static final int kDataRegister = 0x32;
	private static final double kGsPerLSB = 0.00390625;

	private static final int kAddress_Read = 0x80;
	private static final int kAddress_MultiByte = 0x40;

	private static final int kPowerCtl_Link=0x20;
	private static final int kPowerCtl_AutoSleep=0x10;
	private static final int kPowerCtl_Measure=0x08;
	private static final int kPowerCtl_Sleep=0x04;

	private static final int kDataFormat_SelfTest=0x80;
	private static final int kDataFormat_SPI=0x40;
	private static final int kDataFormat_IntInvert=0x20;
	private static final int kDataFormat_FullRes=0x08;
	private static final int kDataFormat_Justify=0x04;

	public static class Axes {

		/**
		 * The integer value representing this enumeration
		 */
		public final byte value;
		static final byte kX_val = 0x00;
		static final byte kY_val = 0x02;
		static final byte kZ_val = 0x04;
		public static final ADXL345_SPI.Axes kX = new ADXL345_SPI.Axes(kX_val);
		public static final ADXL345_SPI.Axes kY = new ADXL345_SPI.Axes(kY_val);
		public static final ADXL345_SPI.Axes kZ = new ADXL345_SPI.Axes(kZ_val);

		private Axes(byte value) {
			this.value = value;
		}
	}

	public static class AllAxes {

		public double XAxis;
		public double YAxis;
		public double ZAxis;
	}

	private SPI m_spi;

	/**
	 * Constructor. Use this when the device is the first/only device on the bus
	 *
	 * @param clk The clock channel
	 * @param mosi The mosi (output) channel
	 * @param miso The miso (input) channel
	 * @param cs The chip select channel
	 * @param range The range (+ or -) that the accelerometer will measure.
	 */
	public ADXL345_SPI(SPI.Port port, Range range) {
		m_spi = new SPI(port);
		init(range);
	}

	public void free(){
		m_spi.free();
	}

	/**
	 * Set SPI bus parameters, bring device out of sleep and set format
	 *
	 * @param range The range (+ or -) that the accelerometer will measure.
	 */
	private void init(Range range){
		m_spi.setClockRate(500000);
		m_spi.setMSBFirst();
		m_spi.setSampleDataOnFalling();
		m_spi.setClockActiveLow();
		m_spi.setChipSelectActiveHigh();

		// Turn on the measurements
		byte[] commands = new byte[2];
		commands[0] = kPowerCtlRegister;
		commands[1] = kPowerCtl_Measure;
		m_spi.write(commands, 2);

        setRange(range);
	}

	/** {inheritdoc} */
	@Override
	public void setRange(Range range) {
		byte value = 0;

		switch(range) {
		case k2G:
			value = 0;
			break;
		case k4G:
			value = 1;
			break;
		case k8G:
			value = 2;
			break;
		case k16G:
			value = 3;
			break;
		}

		// Specify the data format to read
		byte[] commands = new byte[]{
			kDataFormatRegister,
			(byte)(kDataFormat_FullRes | value)
		};
		m_spi.write(commands, commands.length);
	}

	/** {@inheritdoc} */
	@Override
	public double getX() {
		return getAcceleration(Axes.kX);
	}

	/** {@inheritdoc} */
	@Override
	public double getY() {
		return getAcceleration(Axes.kY);
	}

	/** {@inheritdoc} */
	@Override
	public double getZ() {
		return getAcceleration(Axes.kZ);
	}

	/**
	 * Get the acceleration of one axis in Gs.
	 *
	 * @param axis The axis to read from.
	 * @return Acceleration of the ADXL345 in Gs.
	 */
	public double getAcceleration(ADXL345_SPI.Axes axis) {
		byte[] transferBuffer = new byte[3];
		transferBuffer[0] = (byte)((kAddress_Read | kAddress_MultiByte | kDataRegister) + axis.value);
		m_spi.transaction(transferBuffer, transferBuffer, 3);
		//Sensor is little endian... swap bytes
		int rawAccel = transferBuffer[2] << 8 | transferBuffer[1];
		return rawAccel * kGsPerLSB;
	}

	/**
	 * Get the acceleration of all axes in Gs.
	 *
	 * @return Acceleration measured on all axes of the ADXL345 in Gs.
	 */
	public ADXL345_SPI.AllAxes getAccelerations() {
		ADXL345_SPI.AllAxes data = new ADXL345_SPI.AllAxes();
		byte dataBuffer[] = new byte[7];
		int[] rawData = new int[3];
		if (m_spi != null)
		{
			// Select the data address.
			dataBuffer[0] = (byte)(kAddress_Read | kAddress_MultiByte | kDataRegister);
			m_spi.transaction(dataBuffer, dataBuffer, 7);

			for (int i=0; i<3; i++)
			{
				//Sensor is little endian... swap bytes
				rawData[i] = dataBuffer[i*2+2] << 8 | dataBuffer[i*2+1];
			}

			data.XAxis = rawData[0] * kGsPerLSB;
			data.YAxis = rawData[1] * kGsPerLSB;
			data.ZAxis = rawData[2] * kGsPerLSB;
		}
		return data;
	}
}
