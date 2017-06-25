/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.EncoderJNI;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.util.AllocationException;

/**
 * Class to read quadrature encoders.
 *
 * <p>Quadrature encoders are devices that count shaft rotation and can sense direction. The output
 * of the Encoder class is an integer that can count either up or down, and can go negative for
 * reverse direction counting. When creating Encoders, a direction can be supplied that inverts the
 * sense of the output to make code more readable if the encoder is mounted such that forward
 * movement generates negative values. Quadrature encoders have two digital outputs, an A Channel
 * and a B Channel, that are out of phase with each other for direction sensing.
 *
 * <p>All encoders will immediately start counting - reset() them if you need them to be zeroed
 * before use.
 */
public class Encoder extends SensorBase implements CounterBase, PIDSource, LiveWindowSendable {

  public enum IndexingType {
    kResetWhileHigh(0), kResetWhileLow(1), kResetOnFallingEdge(2), kResetOnRisingEdge(3);

    @SuppressWarnings("MemberName")
    public final int value;

    IndexingType(int value) {
      this.value = value;
    }
  }

  /**
   * The a source.
   */
  @SuppressWarnings("MemberName")
  protected DigitalSource m_aSource; // the A phase of the quad encoder
  /**
   * The b source.
   */
  @SuppressWarnings("MemberName")
  protected DigitalSource m_bSource; // the B phase of the quad encoder
  /**
   * The index source.
   */
  protected DigitalSource m_indexSource = null; // Index on some encoders
  private boolean m_allocatedA;
  private boolean m_allocatedB;
  private boolean m_allocatedI;
  private PIDSourceType m_pidSource;

  private int m_encoder; // the HAL encoder object


  /**
   * Common initialization code for Encoders. This code allocates resources for Encoders and is
   * common to all constructors.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param reverseDirection If true, counts down instead of up (this is all relative)
   */
  private void initEncoder(boolean reverseDirection, final EncodingType type) {
    m_encoder = EncoderJNI.initializeEncoder(m_aSource.getPortHandleForRouting(),
        m_aSource.getAnalogTriggerTypeForRouting(), m_bSource.getPortHandleForRouting(),
        m_bSource.getAnalogTriggerTypeForRouting(), reverseDirection, type.value);

    m_pidSource = PIDSourceType.kDisplacement;

    HAL.report(tResourceType.kResourceType_Encoder, getFPGAIndex(), type.value);
    LiveWindow.addSensor("Encoder", m_aSource.getChannel(), this);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA         The a channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param channelB         The b channel DIO channel. 0-9 are on-board, 10-25 are on the MXP port
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   */
  public Encoder(final int channelA, final int channelB, boolean reverseDirection) {
    m_allocatedA = true;
    m_allocatedB = true;
    m_allocatedI = false;
    m_aSource = new DigitalInput(channelA);
    m_bSource = new DigitalInput(channelB);
    initEncoder(reverseDirection, EncodingType.k4X);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA The a channel digital input channel.
   * @param channelB The b channel digital input channel.
   */
  public Encoder(final int channelA, final int channelB) {
    this(channelA, channelB, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA         The a channel digital input channel.
   * @param channelB         The b channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *                         selected, then an encoder FPGA object is used and the returned counts
   *                         will be 4x the encoder spec'd value since all rising and falling edges
   *                         are counted. If 1X or 2X are selected then a m_counter object will be
   *                         used and the returned value will either exactly match the spec'd count
   *                         or be double (2x) the spec'd count.
   */
  public Encoder(final int channelA, final int channelB, boolean reverseDirection,
                 final EncodingType encodingType) {
    m_allocatedA = true;
    m_allocatedB = true;
    m_allocatedI = false;
    if (encodingType == null) {
      throw new NullPointerException("Given encoding type was null");
    }
    m_aSource = new DigitalInput(channelA);
    m_bSource = new DigitalInput(channelB);
    initEncoder(reverseDirection, encodingType);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels. Using an index pulse forces 4x
   * encoding
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA         The a channel digital input channel.
   * @param channelB         The b channel digital input channel.
   * @param indexChannel     The index channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   */
  public Encoder(final int channelA, final int channelB, final int indexChannel,
                 boolean reverseDirection) {
    m_allocatedA = true;
    m_allocatedB = true;
    m_allocatedI = true;
    m_aSource = new DigitalInput(channelA);
    m_bSource = new DigitalInput(channelB);
    m_indexSource = new DigitalInput(indexChannel);
    initEncoder(reverseDirection, EncodingType.k4X);
    setIndexSource(m_indexSource);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels. Using an index pulse forces 4x
   * encoding
   *
   * <p>The encoder will start counting immediately.
   *
   * @param channelA     The a channel digital input channel.
   * @param channelB     The b channel digital input channel.
   * @param indexChannel The index channel digital input channel.
   */
  public Encoder(final int channelA, final int channelB, final int indexChannel) {
    this(channelA, channelB, indexChannel, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA          The source that should be used for the a channel.
   * @param sourceB          the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, boolean reverseDirection) {
    m_allocatedA = false;
    m_allocatedB = false;
    m_allocatedI = false;
    if (sourceA == null) {
      throw new NullPointerException("Digital Source A was null");
    }
    m_aSource = sourceA;
    if (sourceB == null) {
      throw new NullPointerException("Digital Source B was null");
    }
    m_bSource = sourceB;
    initEncoder(reverseDirection, EncodingType.k4X);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA The source that should be used for the a channel.
   * @param sourceB the source that should be used for the b channel.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB) {
    this(sourceA, sourceB, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels as digital inputs. This is used
   * in the case where the digital inputs are shared. The Encoder class will not allocate the
   * digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA          The source that should be used for the a channel.
   * @param sourceB          the source that should be used for the b channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *                         selected, then an encoder FPGA object is used and the returned counts
   *                         will be 4x the encoder spec'd value since all rising and falling edges
   *                         are counted. If 1X or 2X are selected then a m_counter object will be
   *                         used and the returned value will either exactly match the spec'd count
   *                         or be double (2x) the spec'd count.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, boolean reverseDirection,
                 final EncodingType encodingType) {
    m_allocatedA = false;
    m_allocatedB = false;
    m_allocatedI = false;
    if (encodingType == null) {
      throw new NullPointerException("Given encoding type was null");
    }
    if (sourceA == null) {
      throw new NullPointerException("Digital Source A was null");
    }
    m_aSource = sourceA;
    if (sourceB == null) {
      throw new NullPointerException("Digital Source B was null");
    }
    m_aSource = sourceA;
    m_bSource = sourceB;
    initEncoder(reverseDirection, encodingType);
  }

  /**
   * Encoder constructor. Construct a Encoder given a, b and index channels as digital inputs. This
   * is used in the case where the digital inputs are shared. The Encoder class will not allocate
   * the digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA          The source that should be used for the a channel.
   * @param sourceB          the source that should be used for the b channel.
   * @param indexSource      the source that should be used for the index channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource,
                 boolean reverseDirection) {
    m_allocatedA = false;
    m_allocatedB = false;
    m_allocatedI = false;
    if (sourceB == null) {
      throw new NullPointerException("Digital Source A was null");
    }
    m_aSource = sourceA;
    if (sourceB == null) {
      throw new NullPointerException("Digital Source B was null");
    }
    m_aSource = sourceA;
    m_bSource = sourceB;
    m_indexSource = indexSource;
    initEncoder(reverseDirection, EncodingType.k4X);
    setIndexSource(indexSource);
  }

  /**
   * Encoder constructor. Construct a Encoder given a, b and index channels as digital inputs. This
   * is used in the case where the digital inputs are shared. The Encoder class will not allocate
   * the digital inputs and assume that they already are counted.
   *
   * <p>The encoder will start counting immediately.
   *
   * @param sourceA     The source that should be used for the a channel.
   * @param sourceB     the source that should be used for the b channel.
   * @param indexSource the source that should be used for the index channel.
   */
  public Encoder(DigitalSource sourceA, DigitalSource sourceB, DigitalSource indexSource) {
    this(sourceA, sourceB, indexSource, false);
  }

  /**
   * @return The Encoder's FPGA index.
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public int getFPGAIndex() {
    return EncoderJNI.getEncoderFPGAIndex(m_encoder);
  }

  /**
   * Used to divide raw edge counts down to spec'd counts.
   *
   * @return The encoding scale factor 1x, 2x, or 4x, per the requested encoding type.
   */
  public int getEncodingScale() {
    return EncoderJNI.getEncoderEncodingScale(m_encoder);
  }

  /**
   * Free the resources used by this object.
   */
  public void free() {
    if (m_aSource != null && m_allocatedA) {
      m_aSource.free();
      m_allocatedA = false;
    }
    if (m_bSource != null && m_allocatedB) {
      m_bSource.free();
      m_allocatedB = false;
    }
    if (m_indexSource != null && m_allocatedI) {
      m_indexSource.free();
      m_allocatedI = false;
    }

    m_aSource = null;
    m_bSource = null;
    m_indexSource = null;
    EncoderJNI.freeEncoder(m_encoder);
    m_encoder = 0;
  }

  /**
   * Gets the raw value from the encoder. The raw value is the actual count unscaled by the 1x, 2x,
   * or 4x scale factor.
   *
   * @return Current raw count from the encoder
   */
  public int getRaw() {
    return EncoderJNI.getEncoderRaw(m_encoder);
  }

  /**
   * Gets the current count. Returns the current count on the Encoder. This method compensates for
   * the decoding type.
   *
   * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale factor.
   */
  public int get() {
    return EncoderJNI.getEncoder(m_encoder);
  }

  /**
   * Reset the Encoder distance to zero. Resets the current count to zero on the encoder.
   */
  public void reset() {
    EncoderJNI.resetEncoder(m_encoder);
  }

  /**
   * Returns the period of the most recent pulse. Returns the period of the most recent Encoder
   * pulse in seconds. This method compensates for the decoding type.
   *
   * <p><b>Warning:</b> This returns unscaled periods. Use getRate() for rates that are scaled using
   * the value from setDistancePerPulse().
   *
   * @return Period in seconds of the most recent pulse.
   * @deprecated Use getRate() in favor of this method.
   */
  @Deprecated
  public double getPeriod() {
    return EncoderJNI.getEncoderPeriod(m_encoder);
  }

  /**
   * Sets the maximum period for stopped detection. Sets the value that represents the maximum
   * period of the Encoder before it will assume that the attached device is stopped. This timeout
   * allows users to determine if the wheels or other shaft has stopped rotating. This method
   * compensates for the decoding type.
   *
   * @param maxPeriod The maximum time between rising and falling edges before the FPGA will report
   *                  the device stopped. This is expressed in seconds.
   */
  public void setMaxPeriod(double maxPeriod) {
    EncoderJNI.setEncoderMaxPeriod(m_encoder, maxPeriod);
  }

  /**
   * Determine if the encoder is stopped. Using the MaxPeriod value, a boolean is returned that is
   * true if the encoder is considered stopped and false if it is still moving. A stopped encoder is
   * one where the most recent pulse width exceeds the MaxPeriod.
   *
   * @return True if the encoder is considered stopped.
   */
  public boolean getStopped() {
    return EncoderJNI.getEncoderStopped(m_encoder);
  }

  /**
   * The last direction the encoder value changed.
   *
   * @return The last direction the encoder value changed.
   */
  public boolean getDirection() {
    return EncoderJNI.getEncoderDirection(m_encoder);
  }

  /**
   * Get the distance the robot has driven since the last reset as scaled by the value from {@link
   * #setDistancePerPulse(double)}.
   *
   * @return The distance driven since the last reset
   */
  public double getDistance() {
    return EncoderJNI.getEncoderDistance(m_encoder);
  }

  /**
   * Get the current rate of the encoder. Units are distance per second as scaled by the value from
   * setDistancePerPulse().
   *
   * @return The current rate of the encoder.
   */
  public double getRate() {
    return EncoderJNI.getEncoderRate(m_encoder);
  }

  /**
   * Set the minimum rate of the device before the hardware reports it stopped.
   *
   * @param minRate The minimum rate. The units are in distance per second as scaled by the value
   *                from setDistancePerPulse().
   */
  public void setMinRate(double minRate) {
    EncoderJNI.setEncoderMinRate(m_encoder, minRate);
  }

  /**
   * Set the distance per pulse for this encoder. This sets the multiplier used to determine the
   * distance driven based on the count value from the encoder. Do not include the decoding type in
   * this scale. The library already compensates for the decoding type. Set this value based on the
   * encoder's rated Pulses per Revolution and factor in gearing reductions following the encoder
   * shaft. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
   */
  public void setDistancePerPulse(double distancePerPulse) {
    EncoderJNI.setEncoderDistancePerPulse(m_encoder, distancePerPulse);
  }

  /**
   * Set the direction sensing for this encoder. This sets the direction sensing on the encoder so
   * that it could count in the correct software direction regardless of the mounting.
   *
   * @param reverseDirection true if the encoder direction should be reversed
   */
  public void setReverseDirection(boolean reverseDirection) {
    EncoderJNI.setEncoderReverseDirection(m_encoder, reverseDirection);
  }

  /**
   * Set the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @param samplesToAverage The number of samples to average from 1 to 127.
   */
  public void setSamplesToAverage(int samplesToAverage) {
    EncoderJNI.setEncoderSamplesToAverage(m_encoder, samplesToAverage);
  }

  /**
   * Get the Samples to Average which specifies the number of samples of the timer to average when
   * calculating the period. Perform averaging to account for mechanical imperfections or as
   * oversampling to increase resolution.
   *
   * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
   */
  public int getSamplesToAverage() {
    return EncoderJNI.getEncoderSamplesToAverage(m_encoder);
  }

  /**
   * Set which parameter of the encoder you are using as a process control variable. The encoder
   * class supports the rate and distance parameters.
   *
   * @param pidSource An enum to select the parameter.
   */
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  @Override
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  /**
   * Implement the PIDSource interface.
   *
   * @return The current value of the selected source parameter.
   */
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
   * Set the index source for the encoder. When this source is activated, the encoder count
   * automatically resets.
   *
   * @param channel A DIO channel to set as the encoder index
   */
  public void setIndexSource(int channel) {
    setIndexSource(channel, IndexingType.kResetOnRisingEdge);
  }

  /**
   * Set the index source for the encoder. When this source is activated, the encoder count
   * automatically resets.
   *
   * @param source A digital source to set as the encoder index
   */
  public void setIndexSource(DigitalSource source) {
    setIndexSource(source, IndexingType.kResetOnRisingEdge);
  }

  /**
   * Set the index source for the encoder. When this source rises, the encoder count automatically
   * resets.
   *
   * @param channel A DIO channel to set as the encoder index
   * @param type    The state that will cause the encoder to reset
   */
  public void setIndexSource(int channel, IndexingType type) {
    if (m_allocatedI) {
      throw new AllocationException("Digital Input for Indexing already allocated");
    }
    m_indexSource = new DigitalInput(channel);
    m_allocatedI = true;
    setIndexSource(m_indexSource, type);
  }

  /**
   * Set the index source for the encoder. When this source rises, the encoder count automatically
   * resets.
   *
   * @param source A digital source to set as the encoder index
   * @param type   The state that will cause the encoder to reset
   */
  public void setIndexSource(DigitalSource source, IndexingType type) {
    EncoderJNI.setEncoderIndexSource(m_encoder, source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting(), type.value);
  }

  /**
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    if (EncoderJNI.getEncoderEncodingType(m_encoder) == EncodingType.k4X.value) {
      return "Quadrature Encoder";
    }
    return "Encoder";
  }

  private ITable m_table;

  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  @Override
  public ITable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Speed", getRate());
      m_table.putNumber("Distance", getDistance());
      m_table.putNumber("Distance per Tick", EncoderJNI.getEncoderDistancePerPulse(m_encoder));
    }
  }

  @Override
  public void startLiveWindowMode() {
  }

  @Override
  public void stopLiveWindowMode() {
  }
}
