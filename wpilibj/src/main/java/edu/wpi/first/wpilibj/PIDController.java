/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.TimerTask;
import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.wpilibj.filters.LinearDigitalFilter;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.util.BoundaryException;

import static java.util.Objects.requireNonNull;

/**
 * Class implements a PID Control Loop.
 *
 * <p>Creates a separate thread which reads the given PIDSource and takes care of the integral
 * calculations, as well as writing the given PIDOutput.
 *
 * <p>This feedback controller runs in discrete time, so time deltas are not used in the integral
 * and derivative calculations. Therefore, the sample rate affects the controller's behavior for a
 * given set of PID constants.
 */
public class PIDController extends SendableBase implements PIDInterface, Sendable, Controller {
  public static final double kDefaultPeriod = .05;
  private static int instances = 0;
  @SuppressWarnings("MemberName")
  private double m_P; // factor for "proportional" control
  @SuppressWarnings("MemberName")
  private double m_I; // factor for "integral" control
  @SuppressWarnings("MemberName")
  private double m_D; // factor for "derivative" control
  @SuppressWarnings("MemberName")
  private double m_F; // factor for feedforward term
  private double m_maximumOutput = 1.0; // |maximum output|
  private double m_minimumOutput = -1.0; // |minimum output|
  private double m_maximumInput = 0.0; // maximum input - limit setpoint to this
  private double m_minimumInput = 0.0; // minimum input - limit setpoint to this
  private double m_inputRange = 0.0; // input range - difference between maximum and minimum
  // do the endpoints wrap around? eg. Absolute encoder
  private boolean m_continuous = false;
  private boolean m_enabled = false; // is the pid controller enabled
  // the prior error (used to compute velocity)
  private double m_prevError = 0.0;
  // the sum of the errors for use in the integral calc
  private double m_totalError = 0.0;
  // the tolerance object used to check if on target
  private Tolerance m_tolerance;
  private double m_setpoint = 0.0;
  private double m_prevSetpoint = 0.0;
  @SuppressWarnings("PMD.UnusedPrivateField")
  private double m_error = 0.0;
  private double m_result = 0.0;
  private double m_period = kDefaultPeriod;

  PIDSource m_origSource;
  LinearDigitalFilter m_filter;

  ReentrantLock m_thisMutex = new ReentrantLock();

  // Ensures when disable() is called, pidWrite() won't run if calculate()
  // is already running at that time.
  ReentrantLock m_pidWriteMutex = new ReentrantLock();

  protected PIDSource m_pidInput;
  protected PIDOutput m_pidOutput;
  java.util.Timer m_controlLoop;
  Timer m_setpointTimer;

  /**
   * Tolerance is the type of tolerance used to specify if the PID controller is on target.
   *
   * <p>The various implementations of this class such as PercentageTolerance and AbsoluteTolerance
   * specify types of tolerance specifications to use.
   */
  public interface Tolerance {
    boolean onTarget();
  }

  /**
   * Used internally for when Tolerance hasn't been set.
   */
  public class NullTolerance implements Tolerance {
    @Override
    public boolean onTarget() {
      throw new RuntimeException("No tolerance value set when calling onTarget().");
    }
  }

  public class PercentageTolerance implements Tolerance {
    private final double m_percentage;

    PercentageTolerance(double value) {
      m_percentage = value;
    }

    @Override
    public boolean onTarget() {
      return Math.abs(getError()) < m_percentage / 100 * m_inputRange;
    }
  }

  public class AbsoluteTolerance implements Tolerance {
    private final double m_value;

    AbsoluteTolerance(double value) {
      m_value = value;
    }

    @Override
    public boolean onTarget() {
      return Math.abs(getError()) < m_value;
    }
  }

  private class PIDTask extends TimerTask {
    private PIDController m_controller;

    PIDTask(PIDController controller) {
      requireNonNull(controller, "Given PIDController was null");

      m_controller = controller;
    }

    @Override
    public void run() {
      m_controller.calculate();
    }
  }

  /**
   * Allocate a PID object with the given constants for P, I, D, and F.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kf     the feed forward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   * @param period the loop time for doing calculations. This particularly effects calculations of
   *               the integral and differential terms. The default is 50ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kf, PIDSource source,
                       PIDOutput output, double period) {
    super(false);
    requireNonNull(source, "Null PIDSource was given");
    requireNonNull(output, "Null PIDOutput was given");

    m_controlLoop = new java.util.Timer();
    m_setpointTimer = new Timer();
    m_setpointTimer.start();

    m_P = Kp;
    m_I = Ki;
    m_D = Kd;
    m_F = Kf;

    // Save original source
    m_origSource = source;

    // Create LinearDigitalFilter with original source as its source argument
    m_filter = LinearDigitalFilter.movingAverage(m_origSource, 1);
    m_pidInput = m_filter;

    m_pidOutput = output;
    m_period = period;

    m_controlLoop.schedule(new PIDTask(this), 0L, (long) (m_period * 1000));

    instances++;
    HLUsageReporting.reportPIDController(instances);
    m_tolerance = new NullTolerance();
    setName("PIDController", instances);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D and period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source the PIDSource object that is used to get values
   * @param output the PIDOutput object that is set to the output percentage
   * @param period the loop time for doing calculations. This particularly effects calculations of
   *               the integral and differential terms. The default is 50ms.
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, PIDSource source, PIDOutput output,
                       double period) {
    this(Kp, Ki, Kd, 0.0, source, output, period);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D, using a 50ms period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, PIDSource source, PIDOutput output) {
    this(Kp, Ki, Kd, source, output, kDefaultPeriod);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D, using a 50ms period.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param Kf     the feed forward term
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output percentage
   */
  @SuppressWarnings("ParameterName")
  public PIDController(double Kp, double Ki, double Kd, double Kf, PIDSource source,
                       PIDOutput output) {
    this(Kp, Ki, Kd, Kf, source, output, kDefaultPeriod);
  }

  /**
   * Free the PID object.
   */
  @Override
  public void free() {
    super.free();
    m_controlLoop.cancel();
    m_thisMutex.lock();
    try {
      m_pidOutput = null;
      m_pidInput = null;
      m_controlLoop = null;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Read the input, calculate the output accordingly, and write to the output. This should only be
   * called by the PIDTask and is created during initialization.
   */
  @SuppressWarnings("LocalVariableName")
  protected void calculate() {
    if (m_origSource == null || m_pidOutput == null) {
      return;
    }

    boolean enabled;

    m_thisMutex.lock();
    try {
      enabled = m_enabled;
    } finally {
      m_thisMutex.unlock();
    }

    if (enabled) {
      double input;

      // Storage for function inputs
      PIDSourceType pidSourceType;
      double P;
      double I;
      double D;
      double feedForward = calculateFeedForward();
      double minimumOutput;
      double maximumOutput;

      // Storage for function input-outputs
      double prevError;
      double error;
      double totalError;

      m_thisMutex.lock();
      try {
        input = m_pidInput.pidGet();

        pidSourceType = m_pidInput.getPIDSourceType();
        P = m_P;
        I = m_I;
        D = m_D;
        minimumOutput = m_minimumOutput;
        maximumOutput = m_maximumOutput;

        prevError = m_prevError;
        error = getContinuousError(m_setpoint - input);
        totalError = m_totalError;
      } finally {
        m_thisMutex.unlock();
      }

      // Storage for function outputs
      double result;

      if (pidSourceType.equals(PIDSourceType.kRate)) {
        if (P != 0) {
          totalError = clamp(totalError + error, minimumOutput / P,
              maximumOutput / P);
        }

        result = P * totalError + D * error + feedForward;
      } else {
        if (I != 0) {
          totalError = clamp(totalError + error, minimumOutput / I,
              maximumOutput / I);
        }

        result = P * error + I * totalError + D * (error - prevError)
            + feedForward;
      }

      result = clamp(result, minimumOutput, maximumOutput);

      // Ensures m_enabled check and pidWrite() call occur atomically
      m_pidWriteMutex.lock();
      try {
        m_thisMutex.lock();
        try {
          if (m_enabled) {
            // Don't block other PIDController operations on pidWrite()
            m_thisMutex.unlock();

            m_pidOutput.pidWrite(result);
          }
        } finally {
          if (m_thisMutex.isHeldByCurrentThread()) {
            m_thisMutex.unlock();
          }
        }
      } finally {
        m_pidWriteMutex.unlock();
      }

      m_thisMutex.lock();
      try {
        m_prevError = error;
        m_error = error;
        m_totalError = totalError;
        m_result = result;
      } finally {
        m_thisMutex.unlock();
      }
    }
  }

  /**
   * Calculate the feed forward term.
   *
   * <p>Both of the provided feed forward calculations are velocity feed forwards. If a different
   * feed forward calculation is desired, the user can override this function and provide his or
   * her own. This function  does no synchronization because the PIDController class only calls it
   * in synchronized code, so be careful if calling it oneself.
   *
   * <p>If a velocity PID controller is being used, the F term should be set to 1 over the maximum
   * setpoint for the output. If a position PID controller is being used, the F term should be set
   * to 1 over the maximum speed for the output measured in setpoint units per this controller's
   * update period (see the default period in this class's constructor).
   */
  protected double calculateFeedForward() {
    if (m_pidInput.getPIDSourceType().equals(PIDSourceType.kRate)) {
      return m_F * getSetpoint();
    } else {
      double temp = m_F * getDeltaSetpoint();
      m_prevSetpoint = m_setpoint;
      m_setpointTimer.reset();
      return temp;
    }
  }

  /**
   * Set the PID Controller gain parameters. Set the proportional, integral, and differential
   * coefficients.
   *
   * @param p Proportional coefficient
   * @param i Integral coefficient
   * @param d Differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setPID(double p, double i, double d) {
    m_thisMutex.lock();
    try {
      m_P = p;
      m_I = i;
      m_D = d;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the PID Controller gain parameters. Set the proportional, integral, and differential
   * coefficients.
   *
   * @param p Proportional coefficient
   * @param i Integral coefficient
   * @param d Differential coefficient
   * @param f Feed forward coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setPID(double p, double i, double d, double f) {
    m_thisMutex.lock();
    try {
      m_P = p;
      m_I = i;
      m_D = d;
      m_F = f;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Proportional coefficient of the PID controller gain.
   *
   * @param p Proportional coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setP(double p) {
    m_thisMutex.lock();
    try {
      m_P = p;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Integral coefficient of the PID controller gain.
   *
   * @param i Integral coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setI(double i) {
    m_thisMutex.lock();
    try {
      m_I = i;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Differential coefficient of the PID controller gain.
   *
   * @param d differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setD(double d) {
    m_thisMutex.lock();
    try {
      m_D = d;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the Feed forward coefficient of the PID controller gain.
   *
   * @param f feed forward coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setF(double f) {
    m_thisMutex.lock();
    try {
      m_F = f;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    m_thisMutex.lock();
    try {
      return m_P;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    m_thisMutex.lock();
    try {
      return m_I;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    m_thisMutex.lock();
    try {
      return m_D;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Get the Feed forward coefficient.
   *
   * @return feed forward coefficient
   */
  public double getF() {
    m_thisMutex.lock();
    try {
      return m_F;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Return the current PID result This is always centered on zero and constrained the the max and
   * min outs.
   *
   * @return the latest calculated output
   */
  public double get() {
    m_thisMutex.lock();
    try {
      return m_result;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the PID controller to consider the input to be continuous, Rather then using the max and
   * min input range as constraints, it considers them to be the same point and automatically
   * calculates the shortest route to the setpoint.
   *
   * @param continuous Set to true turns on continuous, false turns off continuous
   */
  public void setContinuous(boolean continuous) {
    if (m_inputRange <= 0) {
      throw new RuntimeException("No input range set when calling setContinuous().");
    }
    m_thisMutex.lock();
    try {
      m_continuous = continuous;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the PID controller to consider the input to be continuous, Rather then using the max and
   * min input range as constraints, it considers them to be the same point and automatically
   * calculates the shortest route to the setpoint.
   */
  public void setContinuous() {
    setContinuous(true);
  }

  /**
   * Sets the maximum and minimum values expected from the input and setpoint.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the input
   */
  public void setInputRange(double minimumInput, double maximumInput) {
    m_thisMutex.lock();
    try {
      if (minimumInput > maximumInput) {
        throw new BoundaryException("Lower bound is greater than upper bound");
      }
      m_minimumInput = minimumInput;
      m_maximumInput = maximumInput;
      m_inputRange = maximumInput - minimumInput;
    } finally {
      m_thisMutex.unlock();
    }

    setSetpoint(m_setpoint);
  }

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum percentage to write to the output
   * @param maximumOutput the maximum percentage to write to the output
   */
  public void setOutputRange(double minimumOutput, double maximumOutput) {
    m_thisMutex.lock();
    try {
      if (minimumOutput > maximumOutput) {
        throw new BoundaryException("Lower bound is greater than upper bound");
      }
      m_minimumOutput = minimumOutput;
      m_maximumOutput = maximumOutput;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the setpoint for the PIDController.
   *
   * @param setpoint the desired setpoint
   */
  public void setSetpoint(double setpoint) {
    m_thisMutex.lock();
    try {
      if (m_maximumInput > m_minimumInput) {
        if (setpoint > m_maximumInput) {
          m_setpoint = m_maximumInput;
        } else if (setpoint < m_minimumInput) {
          m_setpoint = m_minimumInput;
        } else {
          m_setpoint = setpoint;
        }
      } else {
        m_setpoint = setpoint;
      }
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the current setpoint of the PIDController.
   *
   * @return the current setpoint
   */
  public double getSetpoint() {
    m_thisMutex.lock();
    try {
      return m_setpoint;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the change in setpoint over time of the PIDController.
   *
   * @return the change in setpoint over time
   */
  public double getDeltaSetpoint() {
    m_thisMutex.lock();
    try {
      return (m_setpoint - m_prevSetpoint) / m_setpointTimer.get();
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the current difference of the input from the setpoint.
   *
   * @return the current error
   */
  public double getError() {
    m_thisMutex.lock();
    try {
      return getContinuousError(getSetpoint() - m_pidInput.pidGet());
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Returns the current difference of the error over the past few iterations. You can specify the
   * number of iterations to average with setToleranceBuffer() (defaults to 1). getAvgError() is
   * used for the onTarget() function.
   *
   * @deprecated Use getError(), which is now already filtered.
   * @return     the current average of the error
   */
  @Deprecated
  public double getAvgError() {
    m_thisMutex.lock();
    try {
      return getError();
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Sets what type of input the PID controller will use.
   *
   * @param pidSource the type of input
   */
  void setPIDSourceType(PIDSourceType pidSource) {
    m_pidInput.setPIDSourceType(pidSource);
  }

  /**
   * Returns the type of input the PID controller is using.
   *
   * @return the PID controller input type
   */
  PIDSourceType getPIDSourceType() {
    return m_pidInput.getPIDSourceType();
  }

  /**
   * Set the PID tolerance using a Tolerance object. Tolerance can be specified as a percentage of
   * the range or as an absolute value. The Tolerance object encapsulates those options in an
   * object. Use it by creating the type of tolerance that you want to use: setTolerance(new
   * PIDController.AbsoluteTolerance(0.1))
   *
   * @deprecated      Use setPercentTolerance() instead.
   * @param tolerance A tolerance object of the right type, e.g. PercentTolerance or
   *                  AbsoluteTolerance
   */
  @Deprecated
  public void setTolerance(Tolerance tolerance) {
    m_tolerance = tolerance;
  }

  /**
   * Set the absolute error which is considered tolerable for use with OnTarget.
   *
   * @param absvalue absolute error which is tolerable in the units of the input object
   */
  public void setAbsoluteTolerance(double absvalue) {
    m_thisMutex.lock();
    try {
      m_tolerance = new AbsoluteTolerance(absvalue);
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the percentage error which is considered tolerable for use with OnTarget. (Input of 15.0 =
   * 15 percent)
   *
   * @param percentage percent error which is tolerable
   */
  public void setPercentTolerance(double percentage) {
    m_thisMutex.lock();
    try {
      m_tolerance = new PercentageTolerance(percentage);
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Set the number of previous error samples to average for tolerancing. When determining whether a
   * mechanism is on target, the user may want to use a rolling average of previous measurements
   * instead of a precise position or velocity. This is useful for noisy sensors which return a few
   * erroneous measurements when the mechanism is on target. However, the mechanism will not
   * register as on target for at least the specified bufLength cycles.
   *
   * @deprecated      Use a LinearDigitalFilter as the input.
   * @param bufLength Number of previous cycles to average.
   */
  @Deprecated
  public void setToleranceBuffer(int bufLength) {
    m_thisMutex.lock();
    try {
      m_filter = LinearDigitalFilter.movingAverage(m_origSource, bufLength);
      m_pidInput = m_filter;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Return true if the error is within the percentage of the total input range, determined by
   * setTolerance. This assumes that the maximum and minimum input were set using setInput.
   *
   * @return true if the error is less than the tolerance
   */
  public boolean onTarget() {
    m_thisMutex.lock();
    try {
      return m_tolerance.onTarget();
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Begin running the PIDController.
   */
  @Override
  public void enable() {
    m_thisMutex.lock();
    try {
      m_enabled = true;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Stop running the PIDController, this sets the output to zero before stopping.
   */
  @Override
  public void disable() {
    // Ensures m_enabled check and pidWrite() call occur atomically
    m_pidWriteMutex.lock();
    try {
      m_thisMutex.lock();
      try {
        m_enabled = false;
      } finally {
        m_thisMutex.unlock();
      }

      m_pidOutput.pidWrite(0);
    } finally {
      m_pidWriteMutex.unlock();
    }
  }

  /**
   * Set the enabled state of the PIDController.
   */
  public void setEnabled(boolean enable) {
    if (enable) {
      enable();
    } else {
      disable();
    }
  }

  /**
   * Return true if PIDController is enabled.
   */
  @Override
  public boolean isEnabled() {
    m_thisMutex.lock();
    try {
      return m_enabled;
    } finally {
      m_thisMutex.unlock();
    }
  }

  /**
   * Reset the previous error,, the integral term, and disable the controller.
   */
  @Override
  public void reset() {
    disable();

    m_thisMutex.lock();
    try {
      m_prevError = 0;
      m_totalError = 0;
      m_result = 0;
    } finally {
      m_thisMutex.unlock();
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PIDController");
    builder.setSafeState(this::reset);
    builder.addDoubleProperty("p", this::getP, this::setP);
    builder.addDoubleProperty("i", this::getI, this::setI);
    builder.addDoubleProperty("d", this::getD, this::setD);
    builder.addDoubleProperty("f", this::getF, this::setF);
    builder.addDoubleProperty("setpoint", this::getSetpoint, this::setSetpoint);
    builder.addBooleanProperty("enabled", this::isEnabled, this::setEnabled);
  }

  /**
   * Wraps error around for continuous inputs. The original error is returned if continuous mode is
   * disabled. This is an unsynchronized function.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  protected double getContinuousError(double error) {
    if (m_continuous && m_inputRange > 0) {
      error %= m_inputRange;
      if (Math.abs(error) > m_inputRange / 2) {
        if (error > 0) {
          return error - m_inputRange;
        } else {
          return error + m_inputRange;
        }
      }
    }

    return error;
  }

  private static double clamp(double value, double low, double high) {
    return Math.max(low, Math.min(value, high));
  }
}
