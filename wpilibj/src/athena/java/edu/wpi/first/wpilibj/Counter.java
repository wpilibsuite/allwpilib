/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.AnalogTriggerOutput.AnalogTriggerType;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.CounterJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.util.BoundaryException;

/**
 * Class for counting the number of ticks on a digital input channel. This is a
 * general purpose class for counting repetitive events. It can return the
 * number of counts, the period of the most recent cycle, and detect when the
 * signal being counted has stopped by supplying a maximum cycle time.
 *
 * All counters will immediately start counting - reset() them if you need them
 * to be zeroed before use.
 */
public class Counter extends SensorBase implements CounterBase, LiveWindowSendable, PIDSource {

  /**
   * Mode determines how and what the counter counts
   */
  public static enum Mode {
    /**
     * mode: two pulse
     */
    kTwoPulse(0),
    /**
     * mode: semi period
     */
    kSemiperiod(1),
    /**
     * mode: pulse length
     */
    kPulseLength(2),
    /**
     * mode: external direction
     */
    kExternalDirection(3);

    /**
     * The integer value representing this enumeration
     */
    public final int value;

    private Mode(int value) {
      this.value = value;
    }
  }

  protected DigitalSource m_upSource; // /< What makes the counter count up.
  protected DigitalSource m_downSource; // /< What makes the counter count down.
  private boolean m_allocatedUpSource;
  private boolean m_allocatedDownSource;
  private long m_counter; // /< The FPGA counter object.
  private int m_index; // /< The index of this counter.
  private PIDSourceType m_pidSource;
  private double m_distancePerPulse; // distance of travel for each tick

  private void initCounter(final Mode mode) {
    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_counter = CounterJNI.initializeCounter(mode.value, index.asIntBuffer());
    m_index = index.asIntBuffer().get(0);

    m_allocatedUpSource = false;
    m_allocatedDownSource = false;
    m_upSource = null;
    m_downSource = null;

    setMaxPeriod(.5);

    UsageReporting.report(tResourceType.kResourceType_Counter, m_index, mode.value);
  }

  /**
   * Create an instance of a counter where no sources are selected. Then they
   * all must be selected by calling functions to specify the upsource and the
   * downsource independently.
   *
   * The counter will start counting immediately.
   */
  public Counter() {
    initCounter(Mode.kTwoPulse);
  }

  /**
   * Create an instance of a counter from a Digital Input. This is used if an
   * existing digital input is to be shared by multiple other objects such as
   * encoders or if the Digital Source is not a DIO channel (such as an Analog
   * Trigger)
   *
   * The counter will start counting immediately.
   *
   * @param source the digital source to count
   */
  public Counter(DigitalSource source) {
    if (source == null)
      throw new NullPointerException("Digital Source given was null");
    initCounter(Mode.kTwoPulse);
    setUpSource(source);
  }

  /**
   * Create an instance of a Counter object. Create an up-Counter instance given
   * a channel.
   *
   * The counter will start counting immediately.
   *
   * @param channel the DIO channel to use as the up source. 0-9 are on-board,
   *        10-25 are on the MXP
   */
  public Counter(int channel) {
    initCounter(Mode.kTwoPulse);
    setUpSource(channel);
  }

  /**
   * Create an instance of a Counter object. Create an instance of a simple
   * up-Counter given an analog trigger. Use the trigger state output from the
   * analog trigger.
   *
   * The counter will start counting immediately.
   *
   * @param encodingType which edges to count
   * @param upSource first source to count
   * @param downSource second source for direction
   * @param inverted true to invert the count
   */
  public Counter(EncodingType encodingType, DigitalSource upSource, DigitalSource downSource,
      boolean inverted) {
    initCounter(Mode.kExternalDirection);
    if (encodingType != EncodingType.k1X && encodingType != EncodingType.k2X) {
      throw new RuntimeException("Counters only support 1X and 2X quadreature decoding!");
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
      CounterJNI.setCounterAverageSize(m_counter, 1);
    } else {
      setUpSourceEdge(true, true);
      CounterJNI.setCounterAverageSize(m_counter, 2);
    }

    setDownSourceEdge(inverted, true);
  }

  /**
   * Create an instance of a Counter object. Create an instance of a simple
   * up-Counter given an analog trigger. Use the trigger state output from the
   * analog trigger.
   *
   * The counter will start counting immediately.
   *
   * @param trigger the analog trigger to count
   */
  public Counter(AnalogTrigger trigger) {
    if (trigger == null) {
      throw new NullPointerException("The Analog Trigger given was null");
    }
    initCounter(Mode.kTwoPulse);
    setUpSource(trigger.createOutput(AnalogTriggerType.kState));
  }

  @Override
  public void free() {
    setUpdateWhenEmpty(true);

    clearUpSource();
    clearDownSource();

    CounterJNI.freeCounter(m_counter);

    m_upSource = null;
    m_downSource = null;
    m_counter = 0;
  }

  /**
   * @return the Counter's FPGA index
   */
  public int getFPGAIndex() {
    return m_index;
  }

  /**
   * Set the upsource for the counter as a digital input channel.
   *
   * @param channel the DIO channel to count 0-9 are on-board, 10-25 are on the
   *        MXP
   */
  public void setUpSource(int channel) {
    setUpSource(new DigitalInput(channel));
    m_allocatedUpSource = true;
  }

  /**
   * Set the source object that causes the counter to count up. Set the up
   * counting DigitalSource.
   *
   * @param source the digital source to count
   */
  public void setUpSource(DigitalSource source) {
    if (m_upSource != null && m_allocatedUpSource) {
      m_upSource.free();
      m_allocatedUpSource = false;
    }
    m_upSource = source;
    CounterJNI.setCounterUpSource(m_counter, source.getChannelForRouting(),
        source.getAnalogTriggerForRouting());
  }

  /**
   * Set the up counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Up
   *        Source
   * @param triggerType The analog trigger output that will trigger the counter.
   */
  public void setUpSource(AnalogTrigger analogTrigger, AnalogTriggerType triggerType) {
    if (analogTrigger == null) {
      throw new NullPointerException("Analog Trigger given was null");
    }
    if (triggerType == null) {
      throw new NullPointerException("Analog Trigger Type given was null");
    }
    setUpSource(analogTrigger.createOutput(triggerType));
    m_allocatedUpSource = true;
  }

  /**
   * Set the edge sensitivity on an up counting source. Set the up source to
   * either detect rising edges or falling edges.
   *
   * @param risingEdge true to count rising edge
   * @param fallingEdge true to count falling edge
   */
  public void setUpSourceEdge(boolean risingEdge, boolean fallingEdge) {
    if (m_upSource == null)
      throw new RuntimeException("Up Source must be set before setting the edge!");
    CounterJNI.setCounterUpSourceEdge(m_counter, risingEdge, fallingEdge);
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

    CounterJNI.clearCounterUpSource(m_counter);
  }

  /**
   * Set the down counting source to be a digital input channel.
   *
   * @param channel the DIO channel to count 0-9 are on-board, 10-25 are on the
   *        MXP
   */
  public void setDownSource(int channel) {
    setDownSource(new DigitalInput(channel));
    m_allocatedDownSource = true;
  }

  /**
   * Set the source object that causes the counter to count down. Set the down
   * counting DigitalSource.
   *
   * @param source the digital source to count
   */
  public void setDownSource(DigitalSource source) {
    if (source == null) {
      throw new NullPointerException("The Digital Source given was null");
    }

    if (m_downSource != null && m_allocatedDownSource) {
      m_downSource.free();
      m_allocatedDownSource = false;
    }
    CounterJNI.setCounterDownSource(m_counter, source.getChannelForRouting(),
        source.getAnalogTriggerForRouting());
    m_downSource = source;
  }

  /**
   * Set the down counting source to be an analog trigger.
   *
   * @param analogTrigger The analog trigger object that is used for the Down
   *        Source
   * @param triggerType The analog trigger output that will trigger the counter.
   */
  public void setDownSource(AnalogTrigger analogTrigger, AnalogTriggerType triggerType) {
    if (analogTrigger == null) {
      throw new NullPointerException("Analog Trigger given was null");
    }
    if (triggerType == null) {
      throw new NullPointerException("Analog Trigger Type given was null");
    }

    setDownSource(analogTrigger.createOutput(triggerType));
    m_allocatedDownSource = true;
  }

  /**
   * Set the edge sensitivity on a down counting source. Set the down source to
   * either detect rising edges or falling edges.
   *
   * @param risingEdge true to count the rising edge
   * @param fallingEdge true to count the falling edge
   */
  public void setDownSourceEdge(boolean risingEdge, boolean fallingEdge) {
    if (m_downSource == null)
      throw new RuntimeException(" Down Source must be set before setting the edge!");
    CounterJNI.setCounterDownSourceEdge(m_counter, risingEdge, fallingEdge);
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

    CounterJNI.clearCounterDownSource(m_counter);
  }

  /**
   * Set standard up / down counting mode on this counter. Up and down counts
   * are sourced independently from two inputs.
   */
  public void setUpDownCounterMode() {
    CounterJNI.setCounterUpDownMode(m_counter);
  }

  /**
   * Set external direction mode on this counter. Counts are sourced on the Up
   * counter input. The Down counter input represents the direction to count.
   */
  public void setExternalDirectionMode() {
    CounterJNI.setCounterExternalDirectionMode(m_counter);
  }

  /**
   * Set Semi-period mode on this counter. Counts up on both rising and falling
   * edges.
   *
   * @param highSemiPeriod true to count up on both rising and falling
   */
  public void setSemiPeriodMode(boolean highSemiPeriod) {
    CounterJNI.setCounterSemiPeriodMode(m_counter, highSemiPeriod);
  }

  /**
   * Configure the counter to count in up or down based on the length of the
   * input pulse. This mode is most useful for direction sensitive gear tooth
   * sensors.
   *
   * @param threshold The pulse length beyond which the counter counts the
   *        opposite direction. Units are seconds.
   */
  public void setPulseLengthMode(double threshold) {
    CounterJNI.setCounterPulseLengthMode(m_counter, threshold);
  }

  /**
   * Read the current counter value. Read the value at this instant. It may
   * still be running, so it reflects the current value. Next time it is read,
   * it might have a different value.
   */
  @Override
  public int get() {
    return CounterJNI.getCounter(m_counter);
  }

  /**
   * Read the current scaled counter value. Read the value at this instant,
   * scaled by the distance per pulse (defaults to 1).
   *
   * @return The distance since the last reset
   */
  public double getDistance() {
    return get() * m_distancePerPulse;
  }

  /**
   * Reset the Counter to zero. Set the counter value to zero. This doesn't
   * effect the running state of the counter, just sets the current value to
   * zero.
   */
  @Override
  public void reset() {
    CounterJNI.resetCounter(m_counter);
  }

  /**
   * Set the maximum period where the device is still considered "moving". Sets
   * the maximum period where the device is considered moving. This value is
   * used to determine the "stopped" state of the counter using the GetStopped
   * method.
   *
   * @param maxPeriod The maximum period where the counted device is considered
   *        moving in seconds.
   */
  @Override
  public void setMaxPeriod(double maxPeriod) {
    CounterJNI.setCounterMaxPeriod(m_counter, maxPeriod);
  }

  /**
   * Select whether you want to continue updating the event timer output when
   * there are no samples captured. The output of the event timer has a buffer
   * of periods that are averaged and posted to a register on the FPGA. When the
   * timer detects that the event source has stopped (based on the MaxPeriod)
   * the buffer of samples to be averaged is emptied. If you enable the update
   * when empty, you will be notified of the stopped source and the event time
   * will report 0 samples. If you disable update when empty, the most recent
   * average will remain on the output until a new sample is acquired. You will
   * never see 0 samples output (except when there have been no events since an
   * FPGA reset) and you will likely not see the stopped bit become true (since
   * it is updated at the end of an average and there are no samples to
   * average).
   *
   * @param enabled true to continue updating
   */
  public void setUpdateWhenEmpty(boolean enabled) {
    CounterJNI.setCounterUpdateWhenEmpty(m_counter, enabled);
  }

  /**
   * Determine if the clock is stopped. Determine if the clocked input is
   * stopped based on the MaxPeriod value set using the SetMaxPeriod method. If
   * the clock exceeds the MaxPeriod, then the device (and counter) are assumed
   * to be stopped and it returns true.
   *
   * @return Returns true if the most recent counter period exceeds the
   *         MaxPeriod value set by SetMaxPeriod.
   */
  @Override
  public boolean getStopped() {
    return CounterJNI.getCounterStopped(m_counter);
  }

  /**
   * The last direction the counter value changed.
   *
   * @return The last direction the counter value changed.
   */
  @Override
  public boolean getDirection() {
    return CounterJNI.getCounterDirection(m_counter);
  }

  /**
   * Set the Counter to return reversed sensing on the direction. This allows
   * counters to change the direction they are counting in the case of 1X and 2X
   * quadrature encoding only. Any other counter mode isn't supported.
   *
   * @param reverseDirection true if the value counted should be negated.
   */
  public void setReverseDirection(boolean reverseDirection) {
    CounterJNI.setCounterReverseDirection(m_counter, reverseDirection);
  }

  /**
   * Get the Period of the most recent count. Returns the time interval of the
   * most recent count. This can be used for velocity calculations to determine
   * shaft speed.
   *
   * @return The period of the last two pulses in units of seconds.
   */
  @Override
  public double getPeriod() {
    return CounterJNI.getCounterPeriod(m_counter);
  }

  /**
   * Get the current rate of the Counter. Read the current rate of the counter
   * accounting for the distance per pulse value. The default value for distance
   * per pulse (1) yields units of pulses per second.
   *
   * @return The rate in units/sec
   */
  public double getRate() {
    return m_distancePerPulse / getPeriod();
  }

  /**
   * Set the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period. Perform averaging to account
   * for mechanical imperfections or as oversampling to increase resolution.
   *
   * @param samplesToAverage The number of samples to average from 1 to 127.
   */
  public void setSamplesToAverage(int samplesToAverage) {
    CounterJNI.setCounterSamplesToAverage(m_counter, samplesToAverage);
  }

  /**
   * Get the Samples to Average which specifies the number of samples of the
   * timer to average when calculating the period. Perform averaging to account
   * for mechanical imperfections or as oversampling to increase resolution.
   *
   * @return SamplesToAverage The number of samples being averaged (from 1 to
   *         127)
   */
  public int getSamplesToAverage() {
    return CounterJNI.getCounterSamplesToAverage(m_counter);
  }

  /**
   * Set the distance per pulse for this counter. This sets the multiplier used
   * to determine the distance driven based on the count value from the encoder.
   * Set this value based on the Pulses per Revolution and factor in any gearing
   * reductions. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerPulse The scale factor that will be used to convert
   *        pulses to useful units.
   */
  public void setDistancePerPulse(double distancePerPulse) {
    m_distancePerPulse = distancePerPulse;
  }

  /**
   * Set which parameter of the encoder you are using as a process control
   * variable. The counter class supports the rate and distance parameters.
   *
   * @param pidSource An enum to select the parameter.
   */
  public void setPIDSourceType(PIDSourceType pidSource) {
    if (pidSource == null) {
      throw new NullPointerException("PID Source Parameter given was null");
    }
    BoundaryException.assertWithinBounds(pidSource.value, 0, 1);
    m_pidSource = pidSource;
  }

  /**
   * {@inheritDoc}
   */
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  @Override
  public double pidGet() {
    switch (m_pidSource) {
      case kDisplacement:
        return getDistance();
      case kRate:
        return getRate();
      default:
        return 0.0;
    }
  }

  /**
   * Live Window code, only does anything if live window is activated.
   */
  @Override
  public String getSmartDashboardType() {
    return "Counter";
  }

  private ITable m_table;

  /**
   * {@inheritDoc}
   */
  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {}

  /**
   * {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {}
}
