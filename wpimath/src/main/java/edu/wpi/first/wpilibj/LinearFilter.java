/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Arrays;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.wpiutil.CircularBuffer;

/**
 * This class implements a linear, digital filter. All types of FIR and IIR filters are supported.
 * Static factory methods are provided to create commonly used types of filters.
 *
 * <p>Filters are of the form: y[n] = (b0*x[n] + b1*x[n-1] + ... + bP*x[n-P]) - (a0*y[n-1] +
 * a2*y[n-2] + ... + aQ*y[n-Q])
 *
 * <p>Where: y[n] is the output at time "n" x[n] is the input at time "n" y[n-1] is the output from
 * the LAST time step ("n-1") x[n-1] is the input from the LAST time step ("n-1") b0...bP are the
 * "feedforward" (FIR) gains a0...aQ are the "feedback" (IIR) gains IMPORTANT! Note the "-" sign in
 * front of the feedback term! This is a common convention in signal processing.
 *
 * <p>What can linear filters do? Basically, they can filter, or diminish, the effects of
 * undesirable input frequencies. High frequencies, or rapid changes, can be indicative of sensor
 * noise or be otherwise undesirable. A "low pass" filter smooths out the signal, reducing the
 * impact of these high frequency components.  Likewise, a "high pass" filter gets rid of
 * slow-moving signal components, letting you detect large changes more easily.
 *
 * <p>Example FRC applications of filters: - Getting rid of noise from an analog sensor input (note:
 * the roboRIO's FPGA can do this faster in hardware) - Smoothing out joystick input to prevent the
 * wheels from slipping or the robot from tipping - Smoothing motor commands so that unnecessary
 * strain isn't put on electrical or mechanical components - If you use clever gains, you can make a
 * PID controller out of this class!
 *
 * <p>For more on filters, we highly recommend the following articles:<br>
 * https://en.wikipedia.org/wiki/Linear_filter<br>
 * https://en.wikipedia.org/wiki/Iir_filter<br>
 * https://en.wikipedia.org/wiki/Fir_filter<br>
 *
 * <p>Note 1: calculate() should be called by the user on a known, regular period. You can use a
 * Notifier for this or do it "inline" with code in a periodic function.
 *
 * <p>Note 2: For ALL filters, gains are necessarily a function of frequency. If you make a filter
 * that works well for you at, say, 100Hz, you will most definitely need to adjust the gains if you
 * then want to run it at 200Hz! Combining this with Note 1 - the impetus is on YOU as a developer
 * to make sure calculate() gets called at the desired, constant frequency!
 */
public class LinearFilter {
  private final CircularBuffer m_inputs;
  private final CircularBuffer m_outputs;
  private final double[] m_inputGains;
  private final double[] m_outputGains;

  private static int instances;

  /**
   * Create a linear FIR or IIR filter.
   *
   * @param ffGains The "feed forward" or FIR gains.
   * @param fbGains The "feed back" or IIR gains.
   */
  public LinearFilter(double[] ffGains, double[] fbGains) {
    m_inputs = new CircularBuffer(ffGains.length);
    m_outputs = new CircularBuffer(fbGains.length);
    m_inputGains = Arrays.copyOf(ffGains, ffGains.length);
    m_outputGains = Arrays.copyOf(fbGains, fbGains.length);

    instances++;
    MathSharedStore.reportUsage(MathUsageId.kFilter_Linear, instances);
  }

  /**
   * Creates a one-pole IIR low-pass filter of the form: y[n] = (1-gain)*x[n] + gain*y[n-1] where
   * gain = e^(-dt / T), T is the time constant in seconds.
   *
   * <p>This filter is stable for time constants greater than zero.
   *
   * @param timeConstant The discrete-time time constant in seconds.
   * @param period       The period in seconds between samples taken by the user.
   */
  public static LinearFilter singlePoleIIR(double timeConstant,
                                           double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {1.0 - gain};
    double[] fbGains = {-gain};

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Creates a first-order high-pass filter of the form: y[n] = gain*x[n] + (-gain)*x[n-1] +
   * gain*y[n-1] where gain = e^(-dt / T), T is the time constant in seconds.
   *
   * <p>This filter is stable for time constants greater than zero.
   *
   * @param timeConstant The discrete-time time constant in seconds.
   * @param period       The period in seconds between samples taken by the user.
   */
  public static LinearFilter highPass(double timeConstant,
                                      double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {gain, -gain};
    double[] fbGains = {-gain};

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Creates a K-tap FIR moving average filter of the form: y[n] = 1/k * (x[k] + x[k-1] + ... +
   * x[0]).
   *
   * <p>This filter is always stable.
   *
   * @param taps The number of samples to average over. Higher = smoother but slower.
   * @throws IllegalArgumentException if number of taps is less than 1.
   */
  public static LinearFilter movingAverage(int taps) {
    if (taps <= 0) {
      throw new IllegalArgumentException("Number of taps was not at least 1");
    }

    double[] ffGains = new double[taps];
    for (int i = 0; i < ffGains.length; i++) {
      ffGains[i] = 1.0 / taps;
    }

    double[] fbGains = new double[0];

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Reset the filter state.
   */
  public void reset() {
    m_inputs.clear();
    m_outputs.clear();
  }

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   *
   * @return The filtered value at this step
   */
  public double calculate(double input) {
    double retVal = 0.0;

    // Rotate the inputs
    m_inputs.addFirst(input);

    // Calculate the new value
    for (int i = 0; i < m_inputGains.length; i++) {
      retVal += m_inputs.get(i) * m_inputGains[i];
    }
    for (int i = 0; i < m_outputGains.length; i++) {
      retVal -= m_outputs.get(i) * m_outputGains[i];
    }

    // Rotate the outputs
    m_outputs.addFirst(retVal);

    return retVal;
  }
}
