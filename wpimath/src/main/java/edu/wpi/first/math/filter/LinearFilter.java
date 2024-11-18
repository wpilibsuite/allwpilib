// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUsageId;
import edu.wpi.first.util.DoubleCircularBuffer;
import java.util.Arrays;
import org.ejml.simple.SimpleMatrix;

/**
 * This class implements a linear, digital filter. All types of FIR and IIR filters are supported.
 * Static factory methods are provided to create commonly used types of filters.
 *
 * <p>Filters are of the form: y[n] = (b0 x[n] + b1 x[n-1] + ... + bP x[n-P]) - (a0 y[n-1] + a2
 * y[n-2] + ... + aQ y[n-Q])
 *
 * <p>Where: y[n] is the output at time "n" x[n] is the input at time "n" y[n-1] is the output from
 * the LAST time step ("n-1") x[n-1] is the input from the LAST time step ("n-1") b0...bP are the
 * "feedforward" (FIR) gains a0...aQ are the "feedback" (IIR) gains IMPORTANT! Note the "-" sign in
 * front of the feedback term! This is a common convention in signal processing.
 *
 * <p>What can linear filters do? Basically, they can filter, or diminish, the effects of
 * undesirable input frequencies. High frequencies, or rapid changes, can be indicative of sensor
 * noise or be otherwise undesirable. A "low pass" filter smooths out the signal, reducing the
 * impact of these high frequency components. Likewise, a "high pass" filter gets rid of slow-moving
 * signal components, letting you detect large changes more easily.
 *
 * <p>Example FRC applications of filters: - Getting rid of noise from an analog sensor input (note:
 * the roboRIO's FPGA can do this faster in hardware) - Smoothing out joystick input to prevent the
 * wheels from slipping or the robot from tipping - Smoothing motor commands so that unnecessary
 * strain isn't put on electrical or mechanical components - If you use clever gains, you can make a
 * PID controller out of this class!
 *
 * <p>For more on filters, we highly recommend the following articles:<br>
 * <a
 * href="https://en.wikipedia.org/wiki/Linear_filter">https://en.wikipedia.org/wiki/Linear_filter</a>
 * <br>
 * <a href="https://en.wikipedia.org/wiki/Iir_filter">https://en.wikipedia.org/wiki/Iir_filter</a>
 * <br>
 * <a href="https://en.wikipedia.org/wiki/Fir_filter">https://en.wikipedia.org/wiki/Fir_filter</a>
 * <br>
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
  private final DoubleCircularBuffer m_inputs;
  private final DoubleCircularBuffer m_outputs;
  private final double[] m_inputGains;
  private final double[] m_outputGains;
  private double m_lastOutput;

  private static int instances;

  /**
   * Create a linear FIR or IIR filter.
   *
   * @param ffGains The "feedforward" or FIR gains.
   * @param fbGains The "feedback" or IIR gains.
   */
  public LinearFilter(double[] ffGains, double[] fbGains) {
    m_inputs = new DoubleCircularBuffer(ffGains.length);
    m_outputs = new DoubleCircularBuffer(fbGains.length);
    m_inputGains = Arrays.copyOf(ffGains, ffGains.length);
    m_outputGains = Arrays.copyOf(fbGains, fbGains.length);

    instances++;
    MathSharedStore.reportUsage(MathUsageId.kFilter_Linear, instances);
  }

  /**
   * Creates a one-pole IIR low-pass filter of the form: y[n] = (1-gain) x[n] + gain y[n-1] where
   * gain = e<sup>-dt / T</sup>, T is the time constant in seconds.
   *
   * <p>Note: T = 1 / (2πf) where f is the cutoff frequency in Hz, the frequency above which the
   * input starts to attenuate.
   *
   * <p>This filter is stable for time constants greater than zero.
   *
   * @param timeConstant The discrete-time time constant in seconds.
   * @param period The period in seconds between samples taken by the user.
   * @return Linear filter.
   */
  public static LinearFilter singlePoleIIR(double timeConstant, double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {1.0 - gain};
    double[] fbGains = {-gain};

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Creates a first-order high-pass filter of the form: y[n] = gain x[n] + (-gain) x[n-1] + gain
   * y[n-1] where gain = e<sup>-dt / T</sup>, T is the time constant in seconds.
   *
   * <p>Note: T = 1 / (2πf) where f is the cutoff frequency in Hz, the frequency below which the
   * input starts to attenuate.
   *
   * <p>This filter is stable for time constants greater than zero.
   *
   * @param timeConstant The discrete-time time constant in seconds.
   * @param period The period in seconds between samples taken by the user.
   * @return Linear filter.
   */
  public static LinearFilter highPass(double timeConstant, double period) {
    double gain = Math.exp(-period / timeConstant);
    double[] ffGains = {gain, -gain};
    double[] fbGains = {-gain};

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Creates a K-tap FIR moving average filter of the form: y[n] = 1/k (x[k] + x[k-1] + ... + x[0]).
   *
   * <p>This filter is always stable.
   *
   * @param taps The number of samples to average over. Higher = smoother but slower.
   * @return Linear filter.
   * @throws IllegalArgumentException if number of taps is less than 1.
   */
  public static LinearFilter movingAverage(int taps) {
    if (taps <= 0) {
      throw new IllegalArgumentException("Number of taps was not at least 1");
    }

    double[] ffGains = new double[taps];
    Arrays.fill(ffGains, 1.0 / taps);

    double[] fbGains = new double[0];

    return new LinearFilter(ffGains, fbGains);
  }

  /**
   * Creates a finite difference filter that computes the nth derivative of the input given the
   * specified stencil points.
   *
   * <p>Stencil points are the indices of the samples to use in the finite difference. 0 is the
   * current sample, -1 is the previous sample, -2 is the sample before that, etc. Don't use
   * positive stencil points (samples from the future) if the LinearFilter will be used for
   * stream-based online filtering (e.g., taking derivative of encoder samples in real-time).
   *
   * @param derivative The order of the derivative to compute.
   * @param stencil List of stencil points. Its length is the number of samples to use to compute
   *     the given derivative. This must be one more than the order of the derivative or higher.
   * @param period The period in seconds between samples taken by the user.
   * @return Linear filter.
   * @throws IllegalArgumentException if derivative &lt; 1, samples &lt;= 0, or derivative &gt;=
   *     samples.
   */
  public static LinearFilter finiteDifference(int derivative, int[] stencil, double period) {
    // See
    // https://en.wikipedia.org/wiki/Finite_difference_coefficient#Arbitrary_stencil_points
    //
    // For a given list of stencil points s of length n and the order of
    // derivative d < n, the finite difference coefficients can be obtained by
    // solving the following linear system for the vector a.
    //
    // [s₁⁰   ⋯  sₙ⁰ ][a₁]      [ δ₀,d ]
    // [ ⋮    ⋱  ⋮   ][⋮ ] = d! [  ⋮   ]
    // [s₁ⁿ⁻¹ ⋯ sₙⁿ⁻¹][aₙ]      [δₙ₋₁,d]
    //
    // where δᵢ,ⱼ are the Kronecker delta. The FIR gains are the elements of the
    // vector 'a' in reverse order divided by hᵈ.
    //
    // The order of accuracy of the approximation is of the form O(hⁿ⁻ᵈ).

    if (derivative < 1) {
      throw new IllegalArgumentException(
          "Order of derivative must be greater than or equal to one.");
    }

    int samples = stencil.length;

    if (samples <= 0) {
      throw new IllegalArgumentException("Number of samples must be greater than zero.");
    }

    if (derivative >= samples) {
      throw new IllegalArgumentException(
          "Order of derivative must be less than number of samples.");
    }

    var S = new SimpleMatrix(samples, samples);
    for (int row = 0; row < samples; ++row) {
      for (int col = 0; col < samples; ++col) {
        S.set(row, col, Math.pow(stencil[col], row));
      }
    }

    // Fill in Kronecker deltas: https://en.wikipedia.org/wiki/Kronecker_delta
    var d = new SimpleMatrix(samples, 1);
    for (int i = 0; i < samples; ++i) {
      d.set(i, 0, (i == derivative) ? factorial(derivative) : 0.0);
    }

    var a = S.solve(d).divide(Math.pow(period, derivative));

    // Reverse gains list
    double[] ffGains = new double[samples];
    for (int i = 0; i < samples; ++i) {
      ffGains[i] = a.get(samples - i - 1, 0);
    }

    return new LinearFilter(ffGains, new double[0]);
  }

  /**
   * Creates a backward finite difference filter that computes the nth derivative of the input given
   * the specified number of samples.
   *
   * <p>For example, a first derivative filter that uses two samples and a sample period of 20 ms
   * would be
   *
   * <pre><code>
   * LinearFilter.backwardFiniteDifference(1, 2, 0.02);
   * </code></pre>
   *
   * @param derivative The order of the derivative to compute.
   * @param samples The number of samples to use to compute the given derivative. This must be one
   *     more than the order of derivative or higher.
   * @param period The period in seconds between samples taken by the user.
   * @return Linear filter.
   */
  public static LinearFilter backwardFiniteDifference(int derivative, int samples, double period) {
    // Generate stencil points from -(samples - 1) to 0
    int[] stencil = new int[samples];
    for (int i = 0; i < samples; ++i) {
      stencil[i] = -(samples - 1) + i;
    }

    return finiteDifference(derivative, stencil, period);
  }

  /** Reset the filter state. */
  public void reset() {
    m_inputs.clear();
    m_outputs.clear();
  }

  /**
   * Resets the filter state, initializing internal buffers to the provided values.
   *
   * <p>These are the expected lengths of the buffers, depending on what type of linear filter used:
   *
   * <table>
   * <tr><th>Type</th><th>Input Buffer Length</th><th>Output Buffer Length</th></tr>
   * <tr><td>Unspecified</td><td>length of {@code ffGains}</td><td>length of {@code fbGains}</td>
   * </tr>
   * <tr><td>Single Pole IIR</td><td>1</td><td>1</td></tr>
   * <tr><td>High-Pass</td><td>2</td><td>1</td></tr>
   * <tr><td>Moving Average</td><td>{@code taps}</td><td>0</td></tr>
   * <tr><td>Finite Difference</td><td>length of {@code stencil}</td><td>0</td></tr>
   * <tr><td>Backward Finite Difference</td><td>{@code samples}</td><td>0</td></tr>
   * </table>
   *
   * @param inputBuffer Values to initialize input buffer.
   * @param outputBuffer Values to initialize output buffer.
   * @throws IllegalArgumentException if length of inputBuffer or outputBuffer does not match the
   *     length of ffGains and fbGains provided in the constructor.
   */
  public void reset(double[] inputBuffer, double[] outputBuffer) {
    // Clear buffers
    reset();

    if (inputBuffer.length != m_inputGains.length || outputBuffer.length != m_outputGains.length) {
      throw new IllegalArgumentException("Incorrect length of inputBuffer or outputBuffer");
    }

    for (double input : inputBuffer) {
      m_inputs.addFirst(input);
    }
    for (double output : outputBuffer) {
      m_outputs.addFirst(output);
    }
  }

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   * @return The filtered value at this step
   */
  public double calculate(double input) {
    double retVal = 0.0;

    // Rotate the inputs
    if (m_inputGains.length > 0) {
      m_inputs.addFirst(input);
    }

    // Calculate the new value
    for (int i = 0; i < m_inputGains.length; i++) {
      retVal += m_inputs.get(i) * m_inputGains[i];
    }
    for (int i = 0; i < m_outputGains.length; i++) {
      retVal -= m_outputs.get(i) * m_outputGains[i];
    }

    // Rotate the outputs
    if (m_outputGains.length > 0) {
      m_outputs.addFirst(retVal);
    }

    m_lastOutput = retVal;
    return retVal;
  }

  /**
   * Returns the last value calculated by the LinearFilter.
   *
   * @return The last value.
   */
  public double lastValue() {
    return m_lastOutput;
  }

  /**
   * Factorial of n.
   *
   * @param n Argument of which to take factorial.
   */
  private static int factorial(int n) {
    if (n < 2) {
      return 1;
    } else {
      return n * factorial(n - 1);
    }
  }
}
