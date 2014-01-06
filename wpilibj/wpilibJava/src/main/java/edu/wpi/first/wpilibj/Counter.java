/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;

import com.sun.jna.Pointer;

import edu.wpi.first.wpilibj.communication.FRC_NetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * Class for counting the number of ticks on a digital input channel. This is a
 * general purpose class for counting repetitive events. It can return the
 * number of counts, the period of the most recent cycle, and detect when the
 * signal being counted has stopped by supplying a maximum cycle time.
 */
public class Counter extends SensorBase implements CounterBase,
		LiveWindowSendable, PIDSource {

	/**
	 * Mode determines how and what the counter counts
	 */
	public static class Mode {

		/**
		 * The integer value representing this enumeration
		 */
		public final int value;
		static final int kTwoPulse_val = 0;
		static final int kSemiperiod_val = 1;
		static final int kPulseLength_val = 2;
		static final int kExternalDirection_val = 3;
		/**
		 * mode: two pulse
		 */
		public static final Mode kTwoPulse = new Mode(kTwoPulse_val);
		/**
		 * mode: semi period
		 */
		public static final Mode kSemiperiod = new Mode(kSemiperiod_val);
		/**
		 * mode: pulse length
		 */
		public static final Mode kPulseLength = new Mode(kPulseLength_val);
		/**
		 * mode: external direction
		 */
		public static final Mode kExternalDirection = new Mode(
				kExternalDirection_val);

		private Mode(int value) {
			this.value = value;
		}
	}

	private DigitalSource m_upSource; // /< What makes the counter count up.
	private DigitalSource m_downSource; // /< What makes the counter count down.
	private boolean m_allocatedUpSource;
	private boolean m_allocatedDownSource;
	private Pointer m_counter; // /< The FPGA counter object.
	private int m_index; // /< The index of this counter.
	private PIDSourceParameter m_pidSource;
	private double m_distancePerPulse; // distance of travel for each tick

	private void initCounter(final Mode mode) {
		IntBuffer status = IntBuffer.allocate(1), index = IntBuffer.allocate(1);
		m_counter = HALLibrary.initializeCounter(mode.value, index, status);
		HALUtil.checkStatus(status);
		m_index = index.get();

		m_allocatedUpSource = false;
		m_allocatedDownSource = false;
		m_upSource = null;
		m_downSource = null;

		UsageReporting.report(tResourceType.kResourceType_Counter, m_index,
				mode.value);
	}

	/**
	 * Create an instance of a counter where no sources are selected. Then they
	 * all must be selected by calling functions to specify the upsource and the
	 * downsource independently.
	 */
	public Counter() {
		initCounter(Mode.kTwoPulse);
	}

	/**
	 * Create an instance of a counter from a Digital Input. This is used if an
	 * existing digital input is to be shared by multiple other objects such as
	 * encoders.
	 * 
	 * @param source
	 *            the digital source to count
	 */
	public Counter(DigitalSource source) {
		if (source == null)
			throw new NullPointerException("Source given was null");
		initCounter(Mode.kTwoPulse);
		setUpSource(source);
	}

	/**
	 * Create an instance of a Counter object. Create an up-Counter instance
	 * given a channel. The default digital module is assumed.
	 * 
	 * @param channel
	 *            the digital input channel to count
	 */
	public Counter(int channel) {
		initCounter(Mode.kTwoPulse);
		setUpSource(channel);
	}

	/**
	 * Create an instance of a Counter object. Create an instance of an
	 * up-Counter given a digital module and a channel.
	 * 
	 * @param slot
	 *            The cRIO chassis slot for the digital module used
	 * @param channel
	 *            The channel in the digital module
	 */
	public Counter(int slot, int channel) {
		initCounter(Mode.kTwoPulse);
		setUpSource(slot, channel);
	}

	/**
	 * Create an instance of a Counter object. Create an instance of a simple
	 * up-Counter given an analog trigger. Use the trigger state output from the
	 * analog trigger.
	 * 
	 * @param encodingType
	 *            which edges to count
	 * @param upSource
	 *            first source to count
	 * @param downSource
	 *            second source for direction
	 * @param inverted
	 *            true to invert the count
	 */
	public Counter(EncodingType encodingType, DigitalSource upSource,
			DigitalSource downSource, boolean inverted) {
		initCounter(Mode.kExternalDirection);
		if (encodingType != EncodingType.k1X
				&& encodingType != EncodingType.k2X) {
			throw new RuntimeException(
					"Counters only support 1X and 2X quadreature decoding!");
		}
		if (upSource == null)
			throw new NullPointerException("Up Source given was null");
		setUpSource(upSource);
		if (downSource == null)
			throw new NullPointerException("Down Source given was null");
		setDownSource(downSource);

		if (encodingType == null)
			throw new NullPointerException("Encoding type given was null");

		if (encodingType == EncodingType.k1X) {
			setUpSourceEdge(true, false);
		} else {
			setUpSourceEdge(true, true);
		}

		setDownSourceEdge(inverted, true);
	}

	/**
	 * Create an instance of a Counter object. Create an instance of a simple
	 * up-Counter given an analog trigger. Use the trigger state output from the
	 * analog trigger.
	 * 
	 * @param trigger
	 *            the analog trigger to count
	 */
	public Counter(AnalogTrigger trigger) {
		initCounter(Mode.kTwoPulse);
		setUpSource(trigger.createOutput(HALLibrary.AnalogTriggerType.kState));
	}

	public void free() {
		setUpdateWhenEmpty(true);

		clearUpSource();
		clearDownSource();

		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.freeCounter(m_counter, status);
		HALUtil.checkStatus(status);

		m_upSource = null;
		m_downSource = null;
		m_counter = null;
	}

	/**
	 * Set the up source for the counter as digital input channel and slot.
	 * 
	 * @param slot
	 *            the location of the digital module to use
	 * @param channel
	 *            the digital port to count
	 */
	public void setUpSource(int slot, int channel) {
		setUpSource(new DigitalInput(slot, channel));
		m_allocatedUpSource = true;
	}

	/**
	 * Set the upsource for the counter as a digital input channel. The slot
	 * will be the default digital module slot.
	 * 
	 * @param channel
	 *            the digital port to count
	 */
	public void setUpSource(int channel) {
		setUpSource(new DigitalInput(channel));
		m_allocatedUpSource = true;
	}

	/**
	 * Set the source object that causes the counter to count up. Set the up
	 * counting DigitalSource.
	 * 
	 * @param source
	 *            the digital source to count
	 */
	public void setUpSource(DigitalSource source) {
		if (m_upSource != null && m_allocatedUpSource) {
			m_upSource.free();
			m_allocatedUpSource = false;
		}
		m_upSource = source;
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterUpSourceWithModule(m_counter,
				(byte) source.getModuleForRouting(),
				source.getChannelForRouting(),
				(byte) (source.getAnalogTriggerForRouting() ? 1 : 0), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set the up counting source to be an analog trigger.
	 * 
	 * @param analogTrigger
	 *            The analog trigger object that is used for the Up Source
	 * @param triggerType
	 *            The analog trigger output that will trigger the counter.
	 */
	public void setUpSource(AnalogTrigger analogTrigger, int triggerType) {
		analogTrigger.createOutput(triggerType);
		m_allocatedUpSource = true;
	}

	/**
	 * Set the edge sensitivity on an up counting source. Set the up source to
	 * either detect rising edges or falling edges.
	 * 
	 * @param risingEdge
	 *            true to count rising edge
	 * @param fallingEdge
	 *            true to count falling edge
	 */
	public void setUpSourceEdge(boolean risingEdge, boolean fallingEdge) {
		if (m_upSource == null)
			throw new RuntimeException(
					"Up Source must be set before setting the edge!");
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterUpSourceEdge(m_counter,
				(byte) (risingEdge ? 1 : 0), (byte) (fallingEdge ? 1 : 0),
				status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Disable the up counting source to the counter.
	 */
	public void clearUpSource() {
		if (m_upSource != null && m_allocatedUpSource) {
			m_upSource.free();
			m_allocatedUpSource = false;
		}
		m_upSource = null;

		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.clearCounterUpSource(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set the down counting source to be a digital input channel. The slot will
	 * be set to the default digital module slot.
	 * 
	 * @param channel
	 *            the digital port to count
	 */
	public void setDownSource(int channel) {
		setDownSource(new DigitalInput(channel));
		m_allocatedDownSource = true;
	}

	/**
	 * Set the down counting source to be a digital input slot and channel.
	 * 
	 * @param slot
	 *            the location of the digital module to use
	 * @param channel
	 *            the digital port to count
	 */
	public void setDownSource(int slot, int channel) {
		setDownSource(new DigitalInput(slot, channel));
		m_allocatedDownSource = true;
	}

	/**
	 * Set the source object that causes the counter to count down. Set the down
	 * counting DigitalSource.
	 * 
	 * @param source
	 *            the digital source to count
	 */
	public void setDownSource(DigitalSource source) {
		if (m_downSource != null && m_allocatedDownSource) {
			m_downSource.free();
			m_allocatedDownSource = false;
		}
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterDownSourceWithModule(m_counter,
				(byte) source.getModuleForRouting(),
				source.getChannelForRouting(),
				(byte) (source.getAnalogTriggerForRouting() ? 1 : 0), status);
		if (status.get() == HALLibrary.PARAMETER_OUT_OF_RANGE) {
			throw new IllegalArgumentException(
					"Counter only supports DownSource in TwoPulse and ExternalDirection modes.");
		}
		HALUtil.checkStatus(status);
		m_downSource = source;
	}

	/**
	 * Set the down counting source to be an analog trigger.
	 * 
	 * @param analogTrigger
	 *            The analog trigger object that is used for the Down Source
	 * @param triggerType
	 *            The analog trigger output that will trigger the counter.
	 */
	public void setDownSource(AnalogTrigger analogTrigger, int triggerType) {
		setDownSource(analogTrigger.createOutput(triggerType));
		m_allocatedDownSource = true;
	}

	/**
	 * Set the edge sensitivity on a down counting source. Set the down source
	 * to either detect rising edges or falling edges.
	 * 
	 * @param risingEdge
	 *            true to count the rising edge
	 * @param fallingEdge
	 *            true to count the falling edge
	 */
	public void setDownSourceEdge(boolean risingEdge, boolean fallingEdge) {
		if (m_downSource == null)
			throw new RuntimeException(
					" Down Source must be set before setting the edge!");
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterDownSourceEdge(m_counter, (byte) (risingEdge ? 1
				: 0), (byte) (fallingEdge ? 0 : 1), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Disable the down counting source to the counter.
	 */
	public void clearDownSource() {
		if (m_downSource != null && m_allocatedDownSource) {
			m_downSource.free();
			m_allocatedDownSource = false;
		}
		m_downSource = null;

		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.clearCounterDownSource(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set standard up / down counting mode on this counter. Up and down counts
	 * are sourced independently from two inputs.
	 */
	public void setUpDownCounterMode() {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterUpDownMode(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set external direction mode on this counter. Counts are sourced on the Up
	 * counter input. The Down counter input represents the direction to count.
	 */
	public void setExternalDirectionMode() {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterExternalDirectionMode(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set Semi-period mode on this counter. Counts up on both rising and
	 * falling edges.
	 * 
	 * @param highSemiPeriod
	 *            true to count up on both rising and falling
	 */
	public void setSemiPeriodMode(boolean highSemiPeriod) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterSemiPeriodMode(m_counter,
				(byte) (highSemiPeriod ? 1 : 0), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Configure the counter to count in up or down based on the length of the
	 * input pulse. This mode is most useful for direction sensitive gear tooth
	 * sensors.
	 * 
	 * @param threshold
	 *            The pulse length beyond which the counter counts the opposite
	 *            direction. Units are seconds.
	 */
	public void setPulseLengthMode(double threshold) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterPulseLengthMode(m_counter, (float) threshold,
				status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Start the Counter counting. This enables the counter and it starts
	 * accumulating counts from the associated input channel. The counter value
	 * is not reset on starting, and still has the previous value.
	 */
	public void start() {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.startCounter(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Read the current counter value. Read the value at this instant. It may
	 * still be running, so it reflects the current value. Next time it is read,
	 * it might have a different value.
	 */
	public int get() {
		IntBuffer status = IntBuffer.allocate(1);
		int value = HALLibrary.getCounter(m_counter, status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Read the current scaled counter value. Read the value at this instant,
	 * scaled by the distance per pulse (defaults to 1).
	 * 
	 * @return
	 */
	public double getDistance() {
		return get() * m_distancePerPulse;
	}

	/**
	 * Reset the Counter to zero. Set the counter value to zero. This doesn't
	 * effect the running state of the counter, just sets the current value to
	 * zero.
	 */
	public void reset() {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.resetCounter(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Stop the Counter. Stops the counting but doesn't effect the current
	 * value.
	 */
	public void stop() {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.stopCounter(m_counter, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Set the maximum period where the device is still considered "moving".
	 * Sets the maximum period where the device is considered moving. This value
	 * is used to determine the "stopped" state of the counter using the
	 * GetStopped method.
	 * 
	 * @param maxPeriod
	 *            The maximum period where the counted device is considered
	 *            moving in seconds.
	 */
	public void setMaxPeriod(double maxPeriod) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterMaxPeriod(m_counter, maxPeriod, status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Select whether you want to continue updating the event timer output when
	 * there are no samples captured. The output of the event timer has a buffer
	 * of periods that are averaged and posted to a register on the FPGA. When
	 * the timer detects that the event source has stopped (based on the
	 * MaxPeriod) the buffer of samples to be averaged is emptied. If you enable
	 * the update when empty, you will be notified of the stopped source and the
	 * event time will report 0 samples. If you disable update when empty, the
	 * most recent average will remain on the output until a new sample is
	 * acquired. You will never see 0 samples output (except when there have
	 * been no events since an FPGA reset) and you will likely not see the
	 * stopped bit become true (since it is updated at the end of an average and
	 * there are no samples to average).
	 * 
	 * @param enabled
	 *            true to continue updating
	 */
	public void setUpdateWhenEmpty(boolean enabled) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterUpdateWhenEmpty(m_counter,
				(byte) (enabled ? 1 : 0), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Determine if the clock is stopped. Determine if the clocked input is
	 * stopped based on the MaxPeriod value set using the SetMaxPeriod method.
	 * If the clock exceeds the MaxPeriod, then the device (and counter) are
	 * assumed to be stopped and it returns true.
	 * 
	 * @return Returns true if the most recent counter period exceeds the
	 *         MaxPeriod value set by SetMaxPeriod.
	 */
	public boolean getStopped() {
		IntBuffer status = IntBuffer.allocate(1);
		boolean value = HALLibrary.getCounterStopped(m_counter, status) != 0;
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * The last direction the counter value changed.
	 * 
	 * @return The last direction the counter value changed.
	 */
	public boolean getDirection() {
		IntBuffer status = IntBuffer.allocate(1);
		boolean value = HALLibrary.getCounterDirection(m_counter, status) != 0;
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Set the Counter to return reversed sensing on the direction. This allows
	 * counters to change the direction they are counting in the case of 1X and
	 * 2X quadrature encoding only. Any other counter mode isn't supported.
	 * 
	 * @param reverseDirection
	 *            true if the value counted should be negated.
	 */
	public void setReverseDirection(boolean reverseDirection) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterReverseDirection(m_counter,
				(byte) (reverseDirection ? 1 : 0), status);
		HALUtil.checkStatus(status);
	}

	/**
	 * Get the Period of the most recent count. Returns the time interval of the
	 * most recent count. This can be used for velocity calculations to
	 * determine shaft speed.
	 * 
	 * @returns The period of the last two pulses in units of seconds.
	 */
	public double getPeriod() {
		IntBuffer status = IntBuffer.allocate(1);
		double value = HALLibrary.getCounterPeriod(m_counter, status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Get the current rate of the Counter. Read the current rate of the counter
	 * accounting for the distance per pulse value. The default value for
	 * distance per pulse (1) yields units of pulses per second.
	 * 
	 * @return The rate in units/sec
	 */
	public double getRate() {
		return m_distancePerPulse / getPeriod();
	}

	/**
	 * Set the Samples to Average which specifies the number of samples of the
	 * timer to average when calculating the period. Perform averaging to
	 * account for mechanical imperfections or as oversampling to increase
	 * resolution.
	 * 
	 * @param samplesToAverage
	 *            The number of samples to average from 1 to 127.
	 */
	public void setSamplesToAverage(int samplesToAverage) {
		IntBuffer status = IntBuffer.allocate(1);
		HALLibrary.setCounterSamplesToAverage(m_counter, samplesToAverage,
				status);
		if (status.get() == HALLibrary.PARAMETER_OUT_OF_RANGE) {
			throw new BoundaryException(BoundaryException.getMessage(
					samplesToAverage, 1, 127));
		}
		HALUtil.checkStatus(status);
	}

	/**
	 * Get the Samples to Average which specifies the number of samples of the
	 * timer to average when calculating the period. Perform averaging to
	 * account for mechanical imperfections or as oversampling to increase
	 * resolution.
	 * 
	 * @return SamplesToAverage The number of samples being averaged (from 1 to
	 *         127)
	 */
	public int getSamplesToAverage() {
		IntBuffer status = IntBuffer.allocate(1);
		int value = HALLibrary.getCounterSamplesToAverage(m_counter, status);
		HALUtil.checkStatus(status);
		return value;
	}

	/**
	 * Set the distance per pulse for this counter. This sets the multiplier
	 * used to determine the distance driven based on the count value from the
	 * encoder. Set this value based on the Pulses per Revolution and factor in
	 * any gearing reductions. This distance can be in any units you like,
	 * linear or angular.
	 * 
	 * @param distancePerPulse
	 *            The scale factor that will be used to convert pulses to useful
	 *            units.
	 */
	public void setDistancePerPulse(double distancePerPulse) {
		m_distancePerPulse = distancePerPulse;
	}

	/**
	 * Set which parameter of the encoder you are using as a process control
	 * variable. The counter class supports the rate and distance parameters.
	 * 
	 * @param pidSource
	 *            An enum to select the parameter.
	 */
	public void setPIDSourceParameter(PIDSourceParameter pidSource) {
		BoundaryException.assertWithinBounds(pidSource.value, 0, 1);
		m_pidSource = pidSource;
	}

	public double pidGet() {
		switch (m_pidSource.value) {
		case PIDSourceParameter.kDistance_val:
			return getDistance();
		case PIDSourceParameter.kRate_val:
			return getRate();
		default:
			return 0.0;
		}
	}

	/**
	 * Live Window code, only does anything if live window is activated.
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
			m_table.putNumber("Value", get());
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public void startLiveWindowMode() {
	}

	/**
	 * {@inheritDoc}
	 */
	public void stopLiveWindowMode() {
	}
}
