/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimEncoder;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class to read quad encoders. Quadrature encoders are devices that count shaft rotation and can
 * sense direction. The output of the QuadEncoder class is an integer that can count either up or
 * down, and can go negative for reverse direction counting. When creating QuadEncoders, a direction
 * is supplied that changes the sense of the output to make code more readable if the encoder is
 * mounted such that forward movement generates negative values. Quadrature encoders have two
 * digital outputs, an A Channel and a B Channel that are out of phase with each other to allow the
 * FPGA to do direction sensing.
 *
 * All encoders will immediately start counting - reset() them if you need them to be zeroed before
 * use.
 */
public class Encoder extends SensorBase implements CounterBase, PIDSource, LiveWindowSendable {
  private int m_index;
  private double m_distancePerPulse; // distance of travel for each encoder tick
  private EncodingType m_encodingType = EncodingType.k4X;
  private int m_encodingScale; // 1x, 2x, or 4x, per the encodingType
  private boolean m_allocatedA;
  private boolean m_allocatedB;
  private boolean m_allocatedI;
  private boolean m_reverseDirection;
  private PIDSourceType m_pidSource;
  private SimEncoder impl;

  /**
   * Common initialization code for Encoders. This code allocates resources for Encoders and is
   * common to all constructors.
   *
   * The encoder will start counting immediately.
   *
   * @param reverseDirection If true, counts down instead of up (this is all relative)
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *                         selected, then an encoder FPGA object is used and the returned counts
   *                         will be 4x the encoder spec'd value since all rising and falling edges
   *                         are counted. If 1X or 2X are selected then a counter object will be
   *                         used and the returned value will either exactly match the spec'd count
   *                         or be double (2x) the spec'd count.
   */
  private void initEncoder(int aChannel, int bChannel, boolean reverseDirection) {
    m_distancePerPulse = 1.0;
    m_pidSource = PIDSourceType.kDisplacement;
    m_encodingScale = m_encodingType == EncodingType.k4X ? 4
        : m_encodingType == EncodingType.k2X ? 2
        : 1;

    LiveWindow.addSensor("Encoder", aChannel, this);

    if (bChannel < aChannel) { // Swap ports
      int channel = bChannel;
      bChannel = aChannel;
      aChannel = channel;
      m_reverseDirection = !reverseDirection;
    } else {
      m_reverseDirection = reverseDirection;
    }
    impl = new SimEncoder("simulator/dio/" + aChannel + "/" + bChannel);
    setDistancePerPulse(1);

    impl.start();
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * The encoder will start counting immediately.
   *
   * @param aChannel         The a channel digital input channel.
   * @param bChannel         The b channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   */
  public Encoder(final int aChannel, final int bChannel,
                 boolean reverseDirection) {
    m_allocatedA = true;
    m_allocatedB = true;
    m_allocatedI = false;
    initEncoder(aChannel, bChannel, reverseDirection);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * The encoder will start counting immediately.
   *
   * @param aChannel The a channel digital input channel.
   * @param bChannel The b channel digital input channel.
   */
  public Encoder(final int aChannel, final int bChannel) {
    this(aChannel, bChannel, false);
  }

  /**
   * Encoder constructor. Construct a Encoder given a and b channels.
   *
   * The encoder will start counting immediately.
   *
   * @param aChannel         The a channel digital input channel.
   * @param bChannel         The b channel digital input channel.
   * @param reverseDirection represents the orientation of the encoder and inverts the output values
   *                         if necessary so forward represents positive values.
   * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X decoding. If 4X is
   *                         selected, then an encoder FPGA object is used and the returned counts
   *                         will be 4x the encoder spec'd value since all rising and falling edges
   *                         are counted. If 1X or 2X are selected then a counter object will be
   *                         used and the returned value will either exactly match the spec'd count
   *                         or be double (2x) the spec'd count.
   */
  public Encoder(final int aChannel, final int bChannel,
                 boolean reverseDirection, final EncodingType encodingType) {
    m_allocatedA = true;
    m_allocatedB = true;
    m_allocatedI = false;
    if (encodingType == null)
      throw new NullPointerException("Given encoding type was null");
    m_encodingType = encodingType;
    initEncoder(aChannel, bChannel, reverseDirection);
  }

  /**
   * @return the Encoder's FPGA index
   */
  public int getFPGAEncoderIndex() {
    return m_index;
  }

  /**
   * @return the encoding scale factor 1x, 2x, or 4x, per the requested encodingType. Used to divide
   * raw edge counts down to spec'd counts.
   */
  public int getEncodingScale() {
    return m_encodingScale;
  }

  public void free() {
  }

  /**
   * Reset the Encoder distance to zero. Resets the current count to zero on the encoder.
   */
  public void reset() {
    impl.reset();
  }

  // Left this in despite removing other such stubs because this one is needed to compile.
  public boolean getDirection() {
    // XXX: not implemented in simulation yet
    throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
  }

  /**
   * Get the distance the robot has driven since the last reset.
   *
   * @return The distance driven since the last reset as scaled by the value from
   * setDistancePerPulse().
   */
  public double getDistance() {
    return m_distancePerPulse * impl.getM_position();
  }

  /**
   * Gets the current count. Returns the current count on the Encoder. This method compensates for
   * the decoding type.
   *
   * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x sc
   */
  public int get() {
    throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
  }

  /**
   * Returns the period of the most recent pulse. Returns the period of the most recent Encoder
   * pulse in seconds. This method compensates for the decoding type.
   *
   * @return Period in seconds of the most recent pulse.
   * @deprecated Use getRate() in favor of this method.  This returns unscale
   */
  public double getPeriod() {
    throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
  }

  /**
   * Sets the maximum period for stopped detection. Sets the value that represents the maximum
   * period of the Encoder before that the attached device is stopped. This timeout allows users to
   * determ other shaft has stopped rotating. This method compensates for the decoding type.
   *
   * @param maxPeriod The maximum time between rising and falling edges befor report the device
   *                  stopped. This is expressed in seconds.
   */
  public void setMaxPeriod(double maxPeriod) {
    // XXX: not implemented in simulation yet
    throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
  }

  /**
   * Determine if the encoder is stopped. Using the MaxPeriod value, a boolean is returned that is
   * true if the enc stopped and false if it is still moving. A stopped encoder is one where width
   * exceeds the MaxPeriod.
   *
   * @return True if the encoder is considered stopped.
   */
  public boolean getStopped() {
    // XXX: not implemented in simulation yet
    throw new UnsupportedOperationException("Simulation doesn't currently support this method.");
  }

  /**
   * Get the current rate of the encoder. Units are distance per second as scaled by the value from
   * setDistancePerPulse().
   *
   * @return The current rate of the encoder.
   */
  public double getRate() {
    return m_distancePerPulse * impl.getM_velocity();
  }

  /**
   * Set the distance per pulse for this encoder. This sets the multiplier used to determine the
   * distance driven based on the count value from the encoder. Do not include the decoding type in
   * this scale.  The library already compensates for the decoding type. Set this value based on the
   * encoder's rated Pulses per Revolution and factor in gearing reductions following the encoder
   * shaft. This distance can be in any units you like, linear or angular.
   *
   * @param distancePerPulse The scale factor that will be used to convert pulses to useful units.
   */
  public void setDistancePerPulse(double distancePerPulse) {
    System.err.println("NOTE|WPILibJSim: Encoder.setDistancePerPulse() assumes 360 pulses per " +
        "revolution in simulation.");
    if (m_reverseDirection) {
      m_distancePerPulse = -distancePerPulse;
    } else {
      m_distancePerPulse = distancePerPulse;
    }
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

  /**
   * {@inheritDoc}
   */
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

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    switch (m_encodingType) {
      case k4X:
        return "Quadrature Encoder";
      default:
        return "Encoder";
    }
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
      m_table.putNumber("Speed", getRate());
      m_table.putNumber("Distance", getDistance());
      m_table.putNumber("Distance per Tick", m_reverseDirection ? -m_distancePerPulse :
          m_distancePerPulse);
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
