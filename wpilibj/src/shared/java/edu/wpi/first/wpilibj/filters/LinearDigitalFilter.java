/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.filters;

import edu.wpi.first.wpilibj.filters.Filter;
import edu.wpi.first.wpilibj.CircularBuffer;
import edu.wpi.first.wpilibj.PIDSource;

/**
 * This class implements a linear, digital filter. All types of FIR and IIR
 * filters are supported. Static factory methods are provided to create commonly
 * used types of filters.
 *
 * Filters are of the form:
 *  y[n] = (b0*x[n] + b1*x[n-1] + ... + bP*x[n-P) - (a0*y[n-1] + a2*y[n-2] + ... + aQ*y[n-Q])
 *
 * Where:
 *  y[n] is the output at time "n"
 *  x[n] is the input at time "n"
 *  y[n-1] is the output from the LAST time step ("n-1")
 *  x[n-1] is the input from the LAST time step ("n-1")
 *  b0...bP are the "feedforward" (FIR) gains
 *  a0...aQ are the "feedback" (IIR) gains
 * IMPORTANT! Note the "-" sign in front of the feedback term! This is a common
 *            convention in signal processing.
 *
 * What can linear filters do? Basically, they can filter, or diminish, the
 * effects of undesirable input frequencies. High frequencies, or rapid changes,
 * can be indicative of sensor noise or be otherwise undesirable. A "low pass"
 * filter smooths out the signal, reducing the impact of these high frequency
 * components.  Likewise, a "high pass" filter gets rid of slow-moving signal
 * components, letting you detect large changes more easily.
 *
 * Example FRC applications of filters:
 *  - Getting rid of noise from an analog sensor input (note: the roboRIO's FPGA
 *    can do this faster in hardware)
 *  - Smoothing out joystick input to prevent the wheels from slipping or the
 *    robot from tipping
 *  - Smoothing motor commands so that unnecessary strain isn't put on
 *    electrical or mechanical components
 *  - If you use clever gains, you can make a PID controller out of this class!
 *
 * For more on filters, I highly recommend the following articles:
 *  http://en.wikipedia.org/wiki/Linear_filter
 *  http://en.wikipedia.org/wiki/Iir_filter
 *  http://en.wikipedia.org/wiki/Fir_filter
 *
 * Note 1: PIDGet() should be called by the user on a known, regular period.
 * You can set up a Notifier to do this (look at the WPILib PIDController
 * class), or do it "inline" with code in a periodic function.
 *
 * Note 2: For ALL filters, gains are necessarily a function of frequency. If
 * you make a filter that works well for you at, say, 100Hz, you will most
 * definitely need to adjust the gains if you then want to run it at 200Hz!
 * Combining this with Note 1 - the impetus is on YOU as a developer to make
 * sure PIDGet() gets called at the desired, constant frequency!
 */
public class LinearDigitalFilter extends Filter {
  private CircularBuffer m_inputs;
  private CircularBuffer m_outputs;
  private double[] m_inputGains;
  private double[] m_outputGains;

  /**
   * Create a linear FIR or IIR filter
   *
   * @param source The PIDSource object that is used to get values
   * @param ffGains The "feed forward" or FIR gains
   * @param fbGains The "feed back" or IIR gains
   */
  public LinearDigitalFilter(PIDSource source, double[] ffGains,
                             double[] fbGains) {
    super(source);
    m_inputs = new CircularBuffer(ffGains.length);
    m_outputs = new CircularBuffer(fbGains.length);
    m_inputGains = ffGains;
    m_outputGains = fbGains;
  }

  /**
   * Creates a one-pole IIR low-pass filter of the form:
   *   y[n] = (1-gain)*x[n] + gain*y[n-1]
   * where gain = e^(-dt / T), T is the time constant in seconds
   *
   * This filter is stable for time constants greater than zero
   *
   * @param source The PIDSource object that is used to get values
   * @param timeConstant The discrete-time time constant in seconds
   * @param period The period in seconds between samples taken by the user
   */
  public static LinearDigitalFilter singlePoleIIR(PIDSource source,
                                                  double timeConstant,
                                                  double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {1.0 - gain};
    double[] fbGains = {-gain};

    return new LinearDigitalFilter(source, ffGains, fbGains);
  }

  /**
   * Creates a first-order high-pass filter of the form:
   *   y[n] = gain*x[n] + (-gain)*x[n-1] + gain*y[n-1]
   * where gain = e^(-dt / T), T is the time constant in seconds
   *
   * This filter is stable for time constants greater than zero
   *
   * @param source The PIDSource object that is used to get values
   * @param timeConstant The discrete-time time constant in seconds
   * @param period The period in seconds between samples taken by the user
   */
  public static LinearDigitalFilter highPass(PIDSource source,
                                             double timeConstant,
                                             double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {gain, -gain};
    double[] fbGains = {-gain};

    return new LinearDigitalFilter(source, ffGains, fbGains);
  }

  /**
   * Creates a K-tap FIR moving average filter of the form:
   *   y[n] = 1/k * (x[k] + x[k-1] + ... + x[0])
   *
   * This filter is always stable.
   *
   * @param source The PIDSource object that is used to get values
   * @param taps The number of samples to average over. Higher = smoother but
   *        slower
   * @throws IllegalArgumentException if number of taps is less than 1
   */
  public static LinearDigitalFilter movingAverage(PIDSource source, int taps) {
    if (taps <= 0) {
      throw new IllegalArgumentException("Number of taps was not at least 1");
    }

    double[] ffGains = new double[taps];
    for (int i = 0; i < ffGains.length; i++) {
      ffGains[i] = 1.0 / taps;
    }

    double[] fbGains = new double[0];

    return new LinearDigitalFilter(source, ffGains, fbGains);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public double get() {
    double retVal = 0.0;

    // Calculate the new value
    for (int i = 0; i < m_inputGains.length; i++) {
      retVal += m_inputs.get(i) * m_inputGains[i];
    }
    for (int i = 0; i < m_outputGains.length; i++) {
      retVal -= m_outputs.get(i) * m_outputGains[i];
    }

    return retVal;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void reset() {
     m_inputs.reset();
     m_outputs.reset();
  }

  /**
   * Calculates the next value of the filter
   *
   * @return The filtered value at this step
   */
  @Override
  public double pidGet() {
    double retVal = 0.0;

    // Rotate the inputs
    m_inputs.pushFront(pidGetSource());

    // Calculate the new value
    for (int i = 0; i < m_inputGains.length; i++) {
      retVal += m_inputs.get(i) * m_inputGains[i];
    }
    for (int i = 0; i < m_outputGains.length; i++) {
      retVal -= m_outputs.get(i) * m_outputGains[i];
    }

    // Rotate the outputs
    m_outputs.pushFront(retVal);

    return retVal;
  }
}
