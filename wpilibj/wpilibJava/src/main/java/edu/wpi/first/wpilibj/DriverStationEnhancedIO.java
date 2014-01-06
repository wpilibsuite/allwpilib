/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.util.Arrays;
import java.util.List;

import org.omg.CosNaming.NamingContextExtPackage.AddressHelper;

import com.sun.jna.Pointer;
import com.sun.jna.PointerUtils;
import com.sun.jna.Structure;

import edu.wpi.first.wpilibj.communication.FRC_NetworkCommunicationsLibrary;
import edu.wpi.first.wpilibj.parsing.IInputOutput;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * 
 * @author dtjones
 */
public class DriverStationEnhancedIO implements IInputOutput {

	/**
	 * Reads the specified shorts from memory into the given array of shorts.
	 * 
	 * @param pointer
	 *            The memory location to copy from
	 * @param offset
	 *            The offset in memory to start copying at
	 * @param shorts
	 *            The array to store the shorts in
	 * @param low
	 *            The offset in the array to start storing at
	 * @param number
	 *            The number of shorts to read from memory
	 */
	private static void getShorts(Pointer pointer, long offset, short[] shorts,
			int low, int number) {
		for (int i = 0; i < number; i++) {
			shorts[low + i] = pointer.getShort(offset + i);
		}
	}

	/**
	 * Reads the specified bytes from memory into the given array of bytes.
	 * 
	 * @param pointer
	 *            The memory location to copy from
	 * @param offset
	 *            The offset in memory to start copying at
	 * @param shorts
	 *            The array to store the bytes in
	 * @param low
	 *            The offset in the array to start storing at
	 * @param number
	 *            The number of bytes to read from memory
	 */
	private static void getBytes(Pointer pointer, long offset, byte[] bytes,
			int low, int number) {
		for (int i = 0; i < number; i++) {
			bytes[low + i] = pointer.getByte(offset + i);
		}
	}

	/**
	 * Writes the given array of shorts into memory, with the given offset,
	 * starting at the given low and writing the number of shorts specified.
	 * 
	 * @param pointer
	 *            The memory pointer to write to
	 * @param offset
	 *            The memory offset to start writing to
	 * @param shorts
	 *            The list of shorts to write
	 * @param low
	 *            The array number in the list to start at
	 * @param number
	 *            The number of shorts to write, starting at low
	 */
	private static void setShorts(Pointer pointer, long offset, short[] shorts,
			int low, int number) {
		for (int i = 0; i + low < number; i++) {
			pointer.setShort(offset + i, shorts[i + low]);
		}
	}

	/**
	 * Writes the given array of bytes into memory, with the given offset,
	 * starting at the given low and writing the number of bytes specified.
	 * 
	 * @param pointer
	 *            The memory pointer to write to
	 * @param offset
	 *            The memory offset to start writing to
	 * @param bytes
	 *            The list of bytes to write
	 * @param low
	 *            The array number in the list to start at
	 * @param number
	 *            The number of bytes to write, starting at low
	 */
	private static void setBytes(Pointer pointer, long offset, byte[] bytes,
			int low, int number) {
		for (int i = 0; i + low < number; i++) {
			pointer.setByte(offset + i, bytes[i + low]);
		}
	}

	/**
	 * Copies bytes from the source pointer to the destination.
	 * 
	 * @param src
	 *            The source pointer
	 * @param srcOffset
	 *            The offset to start copying memory at
	 * @param dst
	 *            The destination pointer
	 * @param dstOffset
	 *            The offset to copy to
	 * @param len
	 *            The number of bytes to copy
	 */
	private static void copyBytes(Pointer src, int srcOffset, Pointer dst,
			int dstOffset, int len) {
		for (int i = 0; i < len; i++) {
			dst.setByte(dstOffset + i, src.getByte(srcOffset + i));
		}
	}

	static class output_t extends Structure {

		short digital = 0;
		short digital_oe = 0;
		short digital_pe = 0;
		short[] pwm_compare = new short[4];
		short[] pwm_period = new short[2];
		byte[] dac = new byte[2];
		byte leds = 0;
		private byte enables = 0;
		byte pwm_enable = 0; // :4
		byte comparator_enable = 0; // :2
		byte quad_index_enable = 0; // :2
		// union
		// {
		// struct
		// {
		// // Bits are inverted from cypress fw because of big-endian!
		// UINT8 pwm_enable : 4;
		// UINT8 comparator_enable : 2;
		// UINT8 quad_index_enable : 2;
		// };
		// UINT8 enables;
		// };
		byte fixed_digital_out = 0;

		final static int size = 23;

		output_t(Pointer backingMemory) {
			useMemory(backingMemory);
		}

		public void setEnables(byte enablesByte) {
			enables = enablesByte;
			pwm_enable = (byte) ((enablesByte & (byte) 0xF0) >> 4);
			comparator_enable = (byte) ((enablesByte & (byte) 0x0C) >> 2);
			quad_index_enable = (byte) ((enablesByte & (byte) 0x03));
		}

		public byte getEnables() {
			enables = (byte) (((pwm_enable << 4) & (byte) 0xF0)
					| ((comparator_enable << 2) & (byte) 0x0C) | ((quad_index_enable) & (byte) 0x03));
			return enables;
		}

		public void read() {
			digital = getPointer().getShort(0);
			digital_oe = getPointer().getShort(2);
			digital_pe = getPointer().getShort(4);
			getShorts(getPointer(), 6, pwm_compare, 0, pwm_compare.length);
			getShorts(getPointer(), 14, pwm_period, 0, pwm_period.length);
			getBytes(getPointer(), 18, dac, 0, dac.length);
			leds = getPointer().getByte(20);
			setEnables(getPointer().getByte(21));
			fixed_digital_out = getPointer().getByte(22);
		}

		public void write() {
			getPointer().setShort(0, digital);
			getPointer().setShort(2, digital_oe);
			getPointer().setShort(4, digital_pe);
			setShorts(getPointer(), 6, pwm_compare, 0, pwm_compare.length);
			setShorts(getPointer(), 14, pwm_period, 0, pwm_period.length);
			setBytes(getPointer(), 18, dac, 0, dac.length);
			getPointer().setByte(20, leds);
			getPointer().setByte(21, getEnables());
			getPointer().setByte(22, fixed_digital_out);
		}

		public int size() {
			return size;
		}

		@Override
		protected List getFieldOrder() {
			// XXX: I'm just returning the order from the class declaration,
			// don't know if this is correct
			return Arrays.asList(new String[] { "digital", "digital_oe",
					"digital_pe", "pwm_compare", "pwm_period", "dac", "leds",
					"enables", "pwm_enable", "comparator_enable",
					"quad_index_enable" });

		}
	} // data to IO (23 bytes)

	// Dynamic block definitions
	// END: Definitions from the Cypress firmware
	static class input_t extends Structure {

		byte api_version;
		byte fw_version;
		short[] analog = new short[8];
		short digital;
		short[] accel = new short[3];
		short[] quad = new short[2];
		byte buttons;
		byte capsense_slider;
		byte capsense_proximity;

		final static int size = 33;

		input_t(Pointer backingMemory) {
			useMemory(backingMemory);
		}

		public void read() {
			api_version = getPointer().getByte(0);
			fw_version = getPointer().getByte(1);
			getShorts(getPointer(), 2, analog, 0, analog.length);
			digital = getPointer().getShort(18);
			getShorts(getPointer(), 20, accel, 0, accel.length);
			getShorts(getPointer(), 26, quad, 0, quad.length);
			buttons = getPointer().getByte(30);
			capsense_slider = getPointer().getByte(31);
			capsense_proximity = getPointer().getByte(32);
		}

		public void write() {
			getPointer().setByte(0, api_version);
			getPointer().setByte(1, fw_version);
			setShorts(getPointer(), 2, analog, 0, analog.length);
			getPointer().setShort(18, digital);
			setShorts(getPointer(), 20, accel, 0, accel.length);
			setShorts(getPointer(), 26, quad, 0, quad.length);
			getPointer().setByte(30, buttons);
			getPointer().setByte(31, capsense_slider);
			getPointer().setByte(32, capsense_proximity);
		}

		public int size() {
			return size;
		}

		protected List getFieldOrder() {
			// XXX: I'm just returning the order from the class declaration,
			// don't know if this is correct
			return Arrays.asList(new String[] { "api_version", "fw_version",
					"analog", "digital", "accel", "quad", "buttons",
					"capsense_slider", "capsense_proximity" });

		}
	} // data from IO (33 bytes)

	class status_block_t extends Structure {

		byte size = 25; // Must be 25 (size remaining in the block not counting
						// the size variable)
		byte id = kOutputBlockID; // Must be 18
		output_t data;
		byte flags;

		{
			allocateMemory();
			// XXX: Need to determine if this is actually enough, or if we need to find a way to specifiy size as well
			data = new output_t(new Pointer(PointerUtils.getAddress(getPointer()) + 2));
//			data = new output_t(new Pointer(getPointer().address().toUWord()
//					.toPrimitive() + 2, output_t.size));
		}

		public void read() {

			size = getPointer().getByte(0);
			id = getPointer().getByte(1);
			data.read();
			flags = getPointer().getByte(25);
		}

		public void write() {
			getPointer().setByte(0, size);
			getPointer().setByte(1, id);
			data.write();
			getPointer().setByte(25, flags);
		}

		public int size() {
			return 26;
		}

		public void copy(status_block_t dest) {
			write();
			copyBytes(getPointer(), 0, dest.getPointer(), 0, size());
			dest.read();
		}

		protected List getFieldOrder() {
			// XXX: I'm just returning the order from the class declaration,
			// don't know if this is correct
			return Arrays
					.asList(new String[] { "size", "id", "data", "flags" });
		}
	}

	class control_block_t extends Structure {

		byte size = 34; // Must be 34
		byte id = kInputBlockID; // Must be 17
		input_t data;

		{
			allocateMemory();
			// XXX: Need to determine if this is actually enough, or if we need to find a way to specifiy size as well
			data = new input_t(new Pointer(PointerUtils.getAddress(getPointer()) + 2));
//			data = new input_t(new Pointer(getPointer().address().toUWord()
//					.toPrimitive() + 2, input_t.size));
		}

		public void read() {
			size = getPointer().getByte(0);
			id = getPointer().getByte(1);
			data.read();
		}

		public void write() {
			getPointer().setByte(0, size);
			getPointer().setByte(1, id);
			data.write();
		}

		public int size() {
			return 35;
		}

		public void copy(control_block_t dest) {
			write();
			copyBytes(getPointer(), 0, dest.getPointer(), 0, size());
			dest.read();
		}

		protected List getFieldOrder() {
			// XXX: I'm just returning the order from the class declaration,
			// don't know if this is correct
			return Arrays.asList(new String[] { "size", "id", "data" });
		}
	}

	public static class EnhancedIOException extends Exception {

		public EnhancedIOException(String msg) {
			super(msg);
		}
	}

	public static final double kAnalogInputResolution = ((double) ((1 << 14) - 1));
	public static final double kAnalogInputReference = 3.3;
	public static final double kAnalogOutputResolution = ((double) ((1 << 8) - 1));
	public static final double kAnalogOutputReference = 4.0;
	public static final double kAccelOffset = 8300;
	public static final double kAccelScale = 3300.0;
	public static final int kSupportedAPIVersion = 1;
	control_block_t m_inputData;
	status_block_t m_outputData;
	final Object m_inputDataSemaphore;
	final Object m_outputDataSemaphore;
	boolean m_inputValid;
	boolean m_outputValid;
	boolean m_configChanged;
	boolean m_requestEnhancedEnable;
	short[] m_encoderOffsets = new short[2];

	/**
	 * Digital configuration for enhanced IO
	 */
	public static class tDigitalConfig {

		/**
		 * The integer value representing this enumeration
		 */
		public final int value;
		static final int kUnknown_val = 0;
		static final int kInputFloating_val = 1;
		static final int kInputPullUp_val = 2;
		static final int kInputPullDown_val = 3;
		static final int kOutput_val = 4;
		static final int kPWM_val = 5;
		static final int kAnalogComparator_val = 6;
		public static final tDigitalConfig kUnknown = new tDigitalConfig(
				kUnknown_val);
		public static final tDigitalConfig kInputFloating = new tDigitalConfig(
				kInputFloating_val);
		public static final tDigitalConfig kInputPullUp = new tDigitalConfig(
				kInputPullUp_val);
		public static final tDigitalConfig kInputPullDown = new tDigitalConfig(
				kInputPullDown_val);
		public static final tDigitalConfig kOutput = new tDigitalConfig(
				(kOutput_val));
		public static final tDigitalConfig kPWM = new tDigitalConfig((kPWM_val));
		public static final tDigitalConfig kAnalogComparator = new tDigitalConfig(
				(kAnalogComparator_val));

		private tDigitalConfig(int value) {
			this.value = value;
		}
	}

	/**
	 * Accelerometer channel for enhanced IO
	 */
	public static class tAccelChannel {

		/**
		 * The integer value representing this enumeration
		 */
		public final int value;
		static final int kAccelX_val = 0;
		static final int kAccelY_val = 1;
		static final int kAccelZ_val = 2;
		public static final tAccelChannel kAccelX = new tAccelChannel(
				kAccelX_val);
		public static final tAccelChannel kAccelY = new tAccelChannel(
				kAccelY_val);
		public static final tAccelChannel kAccelZ = new tAccelChannel(
				kAccelZ_val);

		private tAccelChannel(int value) {
			this.value = value;
		}
	}

	/**
	 * PWM period channels for enhanced IO
	 */
	public static class tPWMPeriodChannels {

		/**
		 * The integer value representing this enumeration
		 */
		public final int value;
		static final int kPWMChannels1and2_val = 0;
		static final int kPWMChannels3and4_val = 1;
		public static final tPWMPeriodChannels kPWMChannels1and2 = new tPWMPeriodChannels(
				kPWMChannels1and2_val);
		public static final tPWMPeriodChannels kPWMChannels3and4 = new tPWMPeriodChannels(
				kPWMChannels3and4_val);

		private tPWMPeriodChannels(int value) {
			this.value = value;
		}
	}

	static final byte kInputBlockID = 17, kOutputBlockID = 18;
	static final int kStatusValid = 0x01, kStatusConfigChanged = 0x02,
			kForceEnhancedMode = 0x04;

	/**
	 * DriverStationEnhancedIO constructor.
	 * 
	 * This is only called once when the DriverStation constructor is called.
	 */
	DriverStationEnhancedIO() {
		m_inputValid = false;
		m_outputValid = false;
		m_configChanged = false;
		m_requestEnhancedEnable = false;
		m_inputData = new control_block_t();
		m_outputData = new status_block_t();
		m_outputData.size = (byte) (m_outputData.size() - 1);
		m_outputData.id = kOutputBlockID;
		// Expected to be active low, so initialize inactive.
		m_outputData.data.fixed_digital_out = 0x3;
		m_inputDataSemaphore = new Object();
		m_outputDataSemaphore = new Object();
		m_encoderOffsets[0] = 0;
		m_encoderOffsets[1] = 0;
	}

	status_block_t tempOutputData = new status_block_t();
	control_block_t tempInputData = new control_block_t();

	/**
	 * Called by the DriverStation class when data is available. This function
	 * will set any modified configuration / output, then read the input and
	 * configuration from the IO.
	 */
	void updateData() {
		int retVal;
		synchronized (m_outputDataSemaphore) {
			if (m_outputValid || m_configChanged || m_requestEnhancedEnable) {
				m_outputData.flags = kStatusValid;
				if (m_requestEnhancedEnable) {
					// Someone called one of the get config APIs, but we are not
					// in enhanced mode.
					m_outputData.flags |= kForceEnhancedMode;
				}
				if (m_configChanged) {
					if (!m_outputValid) {
						// Someone called one of the set config APIs, but we are
						// not in enhanced mode.
						m_outputData.flags |= kForceEnhancedMode;
					}
					m_outputData.flags |= kStatusConfigChanged;
				}
				// XXX: Needs Heavy testing
				FRC_NetworkCommunicationsLibrary.overrideIOConfig(m_outputData
						.getPointer().getString(0), 5);
			}
			// XXX: Needs heavy testing
			retVal = FRC_NetworkCommunicationsLibrary.getDynamicControlData(
					kOutputBlockID,
					tempOutputData.getPointer().getByteBuffer(0,
							tempOutputData.size()), tempOutputData.size(), 5);
			if (retVal == 0) {
				if (m_outputValid) {
					if (m_configChanged) {
						// If our config change made the round trip then clear
						// the flag.
						if (isConfigEqual(tempOutputData, m_outputData)) {
							m_configChanged = false;
						}
					} else {
						// TODO: This won't work until artf1128 is fixed
						// if (tempOutputData.flags & kStatusConfigChanged)
						{
							// Configuration was updated on the DS, so update
							// our local cache.
							mergeConfigIntoOutput(tempOutputData, m_outputData);
						}
					}
				} else {
					// Initialize the local cache.
					mergeConfigIntoOutput(tempOutputData, m_outputData);
				}
				m_requestEnhancedEnable = false;
				m_outputValid = true;
			} else {
				m_outputValid = false;
				m_inputValid = false;
			}
		}

		synchronized (m_inputDataSemaphore) {
			// XXX: Needs heavy testing
			retVal = FRC_NetworkCommunicationsLibrary.getDynamicControlData(
					kInputBlockID,
					tempInputData.getPointer().getByteBuffer(0,
							tempInputData.size()), tempInputData.size(), 5);
			if (retVal == 0
					&& tempInputData.data.api_version == kSupportedAPIVersion) {
				tempInputData.copy(m_inputData);
				m_inputValid = true;
			} else {
				m_outputValid = false;
				m_inputValid = false;
			}
		}
	}

	/**
	 * Merge the config portion of the DS output block into the local cache.
	 */
	void mergeConfigIntoOutput(status_block_t dsOutputBlock,
			status_block_t localCache) {
		localCache.data.digital = (short) ((localCache.data.digital & dsOutputBlock.data.digital_oe) | (dsOutputBlock.data.digital & ~dsOutputBlock.data.digital_oe));
		localCache.data.digital_oe = dsOutputBlock.data.digital_oe;
		localCache.data.digital_pe = dsOutputBlock.data.digital_pe;
		localCache.data.pwm_period[0] = dsOutputBlock.data.pwm_period[0];
		localCache.data.pwm_period[1] = dsOutputBlock.data.pwm_period[1];
		localCache.data.setEnables(dsOutputBlock.data.getEnables());
	}

	/**
	 * Compare the config portion of the output blocks.
	 */
	boolean isConfigEqual(status_block_t dsOutputBlock,
			status_block_t localCache) {
		if (localCache.data.digital_oe != dsOutputBlock.data.digital_oe) {
			return false;
		}
		if ((localCache.data.digital & ~dsOutputBlock.data.digital) != (dsOutputBlock.data.digital & ~dsOutputBlock.data.digital)) {
			return false;
		}
		if (localCache.data.digital_pe != dsOutputBlock.data.digital_pe) {
			return false;
		}
		if (localCache.data.pwm_period[0] != dsOutputBlock.data.pwm_period[0]) {
			return false;
		}
		if (localCache.data.pwm_period[1] != dsOutputBlock.data.pwm_period[1]) {
			return false;
		}
		if (localCache.data.getEnables() != dsOutputBlock.data.getEnables()) {
			return false;
		}
		return true;
	}

	/**
	 * Query an accelerometer channel on the DS IO.
	 * 
	 * @param channel
	 *            The channel number to read.
	 * @return The current acceleration on the channel in Gs.
	 */
	public double getAcceleration(tAccelChannel channel)
			throws EnhancedIOException {
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return (m_inputData.data.accel[channel.value] - kAccelOffset)
					/ kAccelScale;
		}
	}

	/**
	 * Query an analog input channel on the DS IO.
	 * 
	 * @param channel
	 *            The channel number to read. [1,8]
	 * @return The analog input voltage for the channel.
	 */
	public double getAnalogIn(int channel) throws EnhancedIOException {
		// 3.3V is the analog reference voltage
		return getAnalogInRatio(channel) * kAnalogInputReference;
	}

	/**
	 * Query an analog input channel on the DS IO in ratiometric form.
	 * 
	 * @param channel
	 *            The channel number to read. [1,8]
	 * @return The analog input percentage for the channel.
	 */
	public double getAnalogInRatio(int channel) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 8);
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return m_inputData.data.analog[channel - 1]
					/ kAnalogInputResolution;
		}
	}

	/**
	 * Query the voltage currently being output.
	 * 
	 * AO1 is pin 11 on the top connector (P2). AO2 is pin 12 on the top
	 * connector (P2).
	 * 
	 * @param channel
	 *            The analog output channel on the DS IO. [1,2]
	 * @return The voltage being output on the channel.
	 */
	public double getAnalogOut(int channel) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 2);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}

		synchronized (m_outputDataSemaphore) {
			int tempData = m_outputData.data.dac[channel - 1];
			tempData = tempData < 0 ? tempData + 256 : tempData;
			return tempData * kAnalogOutputReference / kAnalogOutputResolution;
		}
	}

	/**
	 * Set the analog output voltage.
	 * 
	 * AO1 is pin 11 on the top connector (P2). AO2 is pin 12 on the top
	 * connector (P2). AO1 is the reference voltage for the 2 analog comparators
	 * on DIO15 and DIO16.
	 * 
	 * The output range is 0V to 4V, however due to the supply voltage don't
	 * expect more than about 3V. Current supply capability is only 100uA.
	 * 
	 * @param channel
	 *            The analog output channel on the DS IO. [1,2]
	 * @param value
	 *            The voltage to output on the channel.
	 */
	public void setAnalogOut(int channel, double value)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 2);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		if (value < 0.0) {
			value = 0.0;
		}
		if (value > kAnalogOutputReference) {
			value = kAnalogOutputReference;
		}
		if (value > kAnalogOutputReference) {
			value = kAnalogOutputReference;
		}

		synchronized (m_outputDataSemaphore) {
			m_outputData.data.dac[channel - 1] = (byte) (value
					/ kAnalogOutputReference * kAnalogOutputResolution);
		}
	}

	/**
	 * Get the state of a button on the IO board.
	 * 
	 * Button1 is the physical button "S1". Button2 is pin 4 on the top
	 * connector (P2). Button3 is pin 6 on the top connector (P2). Button4 is
	 * pin 8 on the top connector (P2). Button5 is pin 10 on the top connector
	 * (P2). Button6 is pin 7 on the top connector (P2).
	 * 
	 * Button2 through Button6 are Capacitive Sense buttons.
	 * 
	 * @param channel
	 *            The button channel to read. [1,6]
	 * @return The state of the selected button.
	 */
	public boolean getButton(int channel) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 6);
		return ((getButtons() >> (channel - 1)) & 1) != 0;
	}

	/**
	 * Get the state of all the button channels.
	 * 
	 * @return The state of the 6 button channels in the 6 lsb of the returned
	 *         byte.
	 */
	public byte getButtons() throws EnhancedIOException {
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return m_inputData.data.buttons;
		}
	}

	/**
	 * Set the state of an LED on the IO board.
	 * 
	 * @param channel
	 *            The LED channel to set. [1,8]
	 * @param value
	 *            True to turn the LED on.
	 */
	public void setLED(int channel, boolean value) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 8);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		byte leds;
		synchronized (m_outputDataSemaphore) {
			leds = m_outputData.data.leds;

			leds &= ~(1 << (channel - 1));
			if (value) {
				leds |= 1 << (channel - 1);
			}

			m_outputData.data.leds = leds;
		}
	}

	/**
	 * Set the state of all 8 LEDs on the IO board.
	 * 
	 * @param value
	 *            The state of each LED. LED1 is lsb and LED8 is msb.
	 */
	public void setLEDs(byte value) throws EnhancedIOException {
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			m_outputData.data.leds = value;
		}
	}

	/**
	 * Get the current state of a DIO channel regardless of mode.
	 * 
	 * @param channel
	 *            The DIO channel to read. [1,16]
	 * @return The state of the selected digital line.
	 */
	public boolean getDigital(int channel) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 16);
		return ((getDigitals() >> (channel - 1)) & 1) != 0;
	}

	/**
	 * Get the state of all 16 DIO lines regardless of mode.
	 * 
	 * @return The state of all DIO lines. DIO1 is lsb and DIO16 is msb.
	 */
	public short getDigitals() throws EnhancedIOException {
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return m_inputData.data.digital;
		}
	}

	/**
	 * Set the state of a DIO line that is configured for digital output.
	 * 
	 * @param channel
	 *            The DIO channel to set. [1,16]
	 * @param value
	 *            The state to set the selected channel to.
	 */
	public void setDigitalOutput(int channel, boolean value)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 16);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		short digital;
		synchronized (m_outputDataSemaphore) {

			if ((m_outputData.data.digital_oe & (1 << (channel - 1))) != 0) {
				digital = m_outputData.data.digital;

				digital &= ~(1 << (channel - 1));
				if (value) {
					digital |= 1 << (channel - 1);
				}

				m_outputData.data.digital = digital;
			} else {
				System.err.println("Line not configured for output");
			}
		}
	}

	/**
	 * Get the current configuration for a DIO line.
	 * 
	 * This has the side effect of forcing the Driver Station to switch to
	 * Enhanced mode if it's not when called. If Enhanced mode is not enabled
	 * when this is called, it will return kUnknown.
	 * 
	 * @param channel
	 *            The DIO channel config to get. [1,16]
	 * @return The configured mode for the DIO line.
	 */
	public tDigitalConfig getDigitalConfig(int channel)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 16);
		if (!m_outputValid) {
			m_requestEnhancedEnable = true;
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			if ((channel >= 1) && (channel <= 4)) {
				if ((m_outputData.data.pwm_enable & (1 << (channel - 1))) != 0) {
					return tDigitalConfig.kPWM;
				}
			}
			if ((channel >= 15) && (channel <= 16)) {
				if ((m_outputData.data.comparator_enable & (1 << (channel - 15))) != 0) {
					return tDigitalConfig.kAnalogComparator;
				}
			}
			if ((m_outputData.data.digital_oe & (1 << (channel - 1))) != 0) {
				return tDigitalConfig.kOutput;
			}
			if ((m_outputData.data.digital_pe & (1 << (channel - 1))) == 0) {
				return tDigitalConfig.kInputFloating;
			}
			if ((m_outputData.data.digital & (1 << (channel - 1))) != 0) {
				return tDigitalConfig.kInputPullUp;
			} else {
				return tDigitalConfig.kInputPullDown;
			}
		}
	}

	/**
	 * Override the DS's configuration of a DIO line.
	 * 
	 * If configured to kInputFloating, the selected DIO line will be tri-stated
	 * with no internal pull resistor.
	 * 
	 * If configured to kInputPullUp, the selected DIO line will be tri-stated
	 * with a 5k-Ohm internal pull-up resistor enabled.
	 * 
	 * If configured to kInputPullDown, the selected DIO line will be tri-stated
	 * with a 5k-Ohm internal pull-down resistor enabled.
	 * 
	 * If configured to kOutput, the selected DIO line will actively drive to 0V
	 * or Vddio (specified by J1 and J4). DIO1 through DIO12, DIO15, and DIO16
	 * can source 4mA and can sink 8mA. DIO12 and DIO13 can source 4mA and can
	 * sink 25mA.
	 * 
	 * In addition to the common configurations, DIO1 through DIO4 can be
	 * configured to kPWM to enable PWM output.
	 * 
	 * In addition to the common configurations, DIO15 and DIO16 can be
	 * configured to kAnalogComparator to enable analog comparators on those 2
	 * DIO lines. When enabled, the lines are tri-stated and will accept analog
	 * voltages between 0V and 3.3V. If the input voltage is greater than the
	 * voltage output by AO1, the DIO will read as true, if less then false.
	 * 
	 * @param channel
	 *            The DIO line to configure. [1,16]
	 * @param config
	 *            The mode to put the DIO line in.
	 */
	public void setDigitalConfig(int channel, tDigitalConfig config)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 16);
		if (config == tDigitalConfig.kPWM && ((channel > 4) || (channel < 1))) {
			throw new EnhancedIOException(
					"PWM channels must be between 1 and 4");
		}
		if (config == tDigitalConfig.kAnalogComparator
				&& ((channel < 15) || (channel > 16))) {
			throw new EnhancedIOException(
					"Analog comparator channels must be between 15 and 16");
		}

		synchronized (m_outputDataSemaphore) {
			m_configChanged = true;

			if ((channel >= 1) && (channel <= 4)) {
				if (config == tDigitalConfig.kPWM) {
					m_outputData.data.pwm_enable |= 1 << (channel - 1);
					m_outputData.data.digital &= ~(1 << (channel - 1));
					m_outputData.data.digital_oe |= 1 << (channel - 1);
					m_outputData.data.digital_pe &= ~(1 << (channel - 1));
					return;
				} else {
					m_outputData.data.pwm_enable &= ~(1 << (channel - 1));
				}
			} else if ((channel >= 15) && (channel <= 16)) {
				if (config == tDigitalConfig.kAnalogComparator) {
					m_outputData.data.comparator_enable |= 1 << (channel - 15);
					m_outputData.data.digital &= ~(1 << (channel - 1));
					m_outputData.data.digital_oe &= ~(1 << (channel - 1));
					m_outputData.data.digital_pe &= ~(1 << (channel - 1));
					return;
				} else {
					m_outputData.data.comparator_enable &= ~(1 << (channel - 15));
				}
			}
			if (config == tDigitalConfig.kInputFloating) {
				m_outputData.data.digital &= ~(1 << (channel - 1));
				m_outputData.data.digital_oe &= ~(1 << (channel - 1));
				m_outputData.data.digital_pe &= ~(1 << (channel - 1));
			} else if (config == tDigitalConfig.kInputPullUp) {
				m_outputData.data.digital |= 1 << (channel - 1);
				m_outputData.data.digital_oe &= ~(1 << (channel - 1));
				m_outputData.data.digital_pe |= 1 << (channel - 1);
			} else if (config == tDigitalConfig.kInputPullDown) {
				m_outputData.data.digital &= ~(1 << (channel - 1));
				m_outputData.data.digital_oe &= ~(1 << (channel - 1));
				m_outputData.data.digital_pe |= 1 << (channel - 1);
			} else if (config == tDigitalConfig.kOutput) {
				m_outputData.data.digital_oe |= 1 << (channel - 1);
				m_outputData.data.digital_pe &= ~(1 << (channel - 1));
			} else {
				// Something went wrong.
			}
		}
	}

	/**
	 * Get the period of a PWM generator.
	 * 
	 * This has the side effect of forcing the Driver Station to switch to
	 * Enhanced mode if it's not when called. If Enhanced mode is not enabled
	 * when this is called, it will return 0.
	 * 
	 * @param channels
	 *            Select the generator by specifying the two channels to which
	 *            it is connected.
	 * @return The period of the PWM generator in seconds.
	 */
	public double getPWMPeriod(tPWMPeriodChannels channels)
			throws EnhancedIOException {
		if (!m_outputValid) {
			m_requestEnhancedEnable = true;
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			int tempData = m_outputData.data.pwm_period[channels.value] & 0xFFFF;
			return tempData / 24000000.0;
		}
	}

	/**
	 * Set the period of a PWM generator.
	 * 
	 * There are 2 PWM generators on the IO board. One can generate PWM signals
	 * on DIO1 and DIO2, the other on DIO3 and DIO4. Each generator has one
	 * counter and two compare registers. As such, each pair of PWM outputs
	 * share the output period but have independent duty cycles.
	 * 
	 * @param channels
	 *            Select the generator by specifying the two channels to which
	 *            it is connected.
	 * @param period
	 *            The period of the PWM generator in seconds. [0.0,0.002731]
	 */
	public void setPWMPeriod(tPWMPeriodChannels channels, double period)
			throws EnhancedIOException {
		// Convert to ticks based on the IO board's 24MHz clock
		double ticks = period * 24000000.0;
		// Limit the range of the ticks... warn if too big.
		if (ticks > 65534.0) {
			ticks = 65534.0;
			throw new EnhancedIOException("Enhanced IO PWM Period Out of Range");
		} else if (ticks < 0.0) {
			ticks = 0.0;
		}
		// Preserve the duty cycles.
		double[] dutyCycles = new double[2];
		dutyCycles[0] = getPWMOutput((channels.value << 1) + 1);
		dutyCycles[1] = getPWMOutput((channels.value << 1) + 2);
		synchronized (m_outputDataSemaphore) {
			// Update the period
			m_outputData.data.pwm_period[channels.value] = (short) ticks;
			m_configChanged = true;
		}
		// Restore the duty cycles
		setPWMOutput((channels.value << 1) + 1, dutyCycles[0]);
		setPWMOutput((channels.value << 1) + 2, dutyCycles[1]);
	}

	/**
	 * Get the state being output on a fixed digital output.
	 * 
	 * @param channel
	 *            The FixedDO line to get. [1,2]
	 * @return The state of the FixedDO line.
	 */
	public boolean getFixedDigitalOutput(int channel)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 2);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			return ((m_outputData.data.fixed_digital_out >> (channel - 1)) & 1) != 0;
		}
	}

	/**
	 * Set the state to output on a Fixed High Current Digital Output line.
	 * 
	 * FixedDO1 is pin 5 on the top connector (P2). FixedDO2 is pin 3 on the top
	 * connector (P2).
	 * 
	 * The FixedDO lines always output 0V and 3.3V regardless of J1 and J4. They
	 * can source 4mA and can sink 25mA. Because of this, they are expected to
	 * be used in an active low configuration, such as connecting to the cathode
	 * of a bright LED. Because they are expected to be active low, they default
	 * to true.
	 * 
	 * @param channel
	 *            The FixedDO channel to set.
	 * @param value
	 *            The state to set the FixedDO.
	 */
	public void setFixedDigitalOutput(int channel, boolean value)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 2);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		byte digital;
		synchronized (m_outputDataSemaphore) {
			digital = m_outputData.data.fixed_digital_out;

			digital &= ~(1 << (channel - 1));
			if (value) {
				digital |= 1 << (channel - 1);
			}

			m_outputData.data.fixed_digital_out = digital;
		}
	}

	/**
	 * Get the position of a quadrature encoder.
	 * 
	 * There are two signed 16-bit 4X quadrature decoders on the IO board. These
	 * decoders are always monitoring the state of the lines assigned to them,
	 * but these lines do not have to be used for encoders.
	 * 
	 * Encoder1 uses DIO4 for "A", DIO6 for "B", and DIO8 for "Index". Encoder2
	 * uses DIO5 for "A", DIO7 for "B", and DIO9 for "Index".
	 * 
	 * The index functionality can be enabled or disabled using
	 * SetEncoderIndexEnable().
	 * 
	 * @param encoderNumber
	 *            The quadrature encoder to access. [1,2]
	 * @return The current position of the quadrature encoder.
	 */
	public short getEncoder(int encoderNumber) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(encoderNumber, 1, 2);
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return (short) (m_inputData.data.quad[encoderNumber - 1] - m_encoderOffsets[encoderNumber - 1]);
		}
	}

	/**
	 * Reset the position of an encoder to 0.
	 * 
	 * This simply stores an offset locally. It does not reset the hardware
	 * counter on the IO board. If you use this method with Index enabled, you
	 * may get unexpected results.
	 * 
	 * @param encoderNumber
	 *            The quadrature encoder to reset. [1,2]
	 */
	public void resetEncoder(int encoderNumber) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(encoderNumber, 1, 2);
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			m_encoderOffsets[encoderNumber - 1] = m_inputData.data.quad[encoderNumber - 1];
		}
	}

	/**
	 * Get the current configuration of a quadrature encoder index channel.
	 * 
	 * This has the side effect of forcing the Driver Station to switch to
	 * Enhanced mode if it's not when called. If Enhanced mode is not enabled
	 * when this is called, it will return false.
	 * 
	 * @param encoderNumber
	 *            The quadrature encoder. [1,2]
	 * @return Is the index channel of the encoder enabled.
	 */
	public boolean getEncoderIndexEnable(int encoderNumber)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(encoderNumber, 1, 2);
		if (!m_outputValid) {
			m_requestEnhancedEnable = true;
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			return ((m_outputData.data.quad_index_enable >> (encoderNumber - 1)) & 1) != 0;
		}
	}

	/**
	 * Enable or disable the index channel of a quadrature encoder.
	 * 
	 * The quadrature decoders on the IO board support an active-low index
	 * input.
	 * 
	 * Encoder1 uses DIO8 for "Index". Encoder2 uses DIO9 for "Index".
	 * 
	 * When enabled, the decoder's counter will be reset to 0 when A, B, and
	 * Index are all low.
	 * 
	 * @param encoderNumber
	 *            The quadrature encoder. [1,2]
	 * @param enable
	 *            If true, reset the encoder in an index condition.
	 */
	public void setEncoderIndexEnable(int encoderNumber, boolean enable) {
		BoundaryException.assertWithinBounds(encoderNumber, 1, 2);
		synchronized (m_outputDataSemaphore) {
			m_outputData.data.quad_index_enable &= ~(1 << (encoderNumber - 1));
			if (enable) {
				m_outputData.data.quad_index_enable |= 1 << (encoderNumber - 1);
			}
			m_configChanged = true;
		}
	}

	/**
	 * Get the value of the Capacitive Sense touch slider.
	 * 
	 * @return Value between 0.0 (toward center of board) and 1.0 (toward edge
	 *         of board). -1.0 means no touch detected.
	 */
	public double getTouchSlider() throws EnhancedIOException {
		if (!m_inputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			byte rawValue = m_inputData.data.capsense_slider;
			int value = rawValue < 0 ? rawValue + 256 : rawValue;
			return value == 255 ? -1.0 : value / 254.0;
		}
	}

	/**
	 * Get the percent duty-cycle that the PWM generator channel is configured
	 * to output.
	 * 
	 * @param channel
	 *            The DIO line's PWM generator to get the duty-cycle from. [1,4]
	 * @return The percent duty-cycle being output (if the DIO line is
	 *         configured for PWM). [0.0,1.0]
	 */
	public double getPWMOutput(int channel) throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 4);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_outputDataSemaphore) {
			int tempCompare = m_outputData.data.pwm_compare[channel - 1] & 0xFFFF;
			int tempPeriod = m_outputData.data.pwm_period[(channel - 1) >> 1] & 0xFFFF;
			return (double) tempCompare / (double) tempPeriod;
		}
	}

	/**
	 * Set the percent duty-cycle to output on a PWM enabled DIO line.
	 * 
	 * DIO1 through DIO4 have the ability to output a PWM signal. The period of
	 * the signal can be configured in pairs using SetPWMPeriod().
	 * 
	 * @param channel
	 *            The DIO line's PWM generator to set. [1,4]
	 * @param value
	 *            The percent duty-cycle to output from the PWM generator.
	 *            [0.0,1.0]
	 */
	public void setPWMOutput(int channel, double value)
			throws EnhancedIOException {
		BoundaryException.assertWithinBounds(channel, 1, 4);
		if (!m_outputValid) {
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		if (value > 1.0) {
			value = 1.0;
		} else if (value < 0.0) {
			value = 0.0;
		}
		synchronized (m_outputDataSemaphore) {
			m_outputData.data.pwm_compare[channel - 1] = (short) (value * (double) m_outputData.data.pwm_period[(channel - 1) >> 1]);
		}
	}

	/**
	 * Get the firmware version running on the IO board.
	 * 
	 * This also has the side effect of forcing the driver station to switch to
	 * Enhanced mode if it is not. If you plan to switch between Driver Stations
	 * with unknown IO configurations, you can call this until it returns a
	 * non-0 version to ensure that this API is accessible before proceeding.
	 * 
	 * @return The version of the firmware running on the IO board. 0 if the
	 *         board is not attached or not in Enhanced mode.
	 */
	public byte getFirmwareVersion() throws EnhancedIOException {
		if (!m_inputValid) {
			m_requestEnhancedEnable = true;
			throw new EnhancedIOException("Enhanced IO Missing");
		}
		synchronized (m_inputDataSemaphore) {
			return m_inputData.data.fw_version;
		}
	}
}
