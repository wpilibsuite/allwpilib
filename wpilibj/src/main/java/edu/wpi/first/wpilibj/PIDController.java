/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.ArrayDeque;
import java.util.Queue;
import java.util.TimerTask;

import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
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
public class PIDController implements PIDInterface, LiveWindowSendable, Controller {

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
  // do the endpoints wrap around? eg. Absolute encoder
  private boolean m_continuous = false;
  private boolean m_enabled = false; // is the pid controller enabled
  // the prior error (used to compute velocity)
  private double m_prevError = 0.0;
  // the sum of the errors for use in the integral calc
  private double m_totalError = 0.0;
  // the tolerance object used to check if on target
  private Tolerance m_tolerance;
  private int m_bufLength = 1;
  private Queue<Double> m_buf;
  private double m_bufTotal = 0.0;
  private double m_setpoint = 0.0;
  private double m_prevSetpoint = 0.0;
  private double m_error = 0.0;
  private double m_result = 0.0;
  private double m_period = kDefaultPeriod;
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
      return isAvgErrorValid() && Math.abs(getAvgError()) < m_percentage / 100 * (m_maximumInput
          - m_minimumInput);
    }
  }

  public class AbsoluteTolerance implements Tolerance {
    private final double m_value;

    AbsoluteTolerance(double value) {
      m_value = value;
    }

    @Override
    public boolean onTarget() {
      return isAvgErrorValid() && Math.abs(getAvgError()) < m_value;
    }
  }

  private class PIDTask extends TimerTask {

    private PIDController m_controller;

    public PIDTask(PIDController controller) {
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
    requireNonNull(source, "Null PIDSource was given");
    requireNonNull(output, "Null PIDOutput was given");

    m_controlLoop = new java.util.Timer();
    m_setpointTimer = new Timer();
    m_setpointTimer.start();

    m_P = Kp;
    m_I = Ki;
    m_D = Kd;
    m_F = Kf;

    m_pidInput = source;
    m_pidOutput = output;
    m_period = period;

    m_controlLoop.schedule(new PIDTask(this), 0L, (long) (m_period * 1000));

    instances++;
    HLUsageReporting.reportPIDController(instances);
    m_tolerance = new NullTolerance();

    m_buf = new ArrayDeque<Double>(m_bufLength + 1);
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
  public void free() {
    m_controlLoop.cancel();
    synchronized (this) {
      m_pidOutput = null;
      m_pidInput = null;
      m_controlLoop = null;
    }
    removeListeners();
  }

  /**
   * Read the input, calculate the output accordingly, and write to the output. This should only be
   * called by the PIDTask and is created during initialization.
   */
  protected void calculate() {
    boolean enabled;
    PIDSource pidInput;

    synchronized (this) {
      if (m_pidInput == null) {
        return;
      }
      if (m_pidOutput == null) {
        return;
      }
      enabled = m_enabled; // take snapshot of these values...
      pidInput = m_pidInput;
    }

    if (enabled) {
      double input;
      double result;
      final PIDOutput pidOutput;
      synchronized (this) {
        input = pidInput.pidGet();
      }
      synchronized (this) {
        m_error = getContinuousError(m_setpoint - input);

        if (m_pidInput.getPIDSourceType().equals(PIDSourceType.kRate)) {
          if (m_P != 0) {
            double potentialPGain = (m_totalError + m_error) * m_P;
            if (potentialPGain < m_maximumOutput) {
              if (potentialPGain > m_minimumOutput) {
                m_totalError += m_error;
              } else {
                m_totalError = m_minimumOutput / m_P;
              }
            } else {
              m_totalError = m_maximumOutput / m_P;
            }

            m_result = m_P * m_totalError + m_D * m_error
                + calculateFeedForward();
          }
        } else {
          if (m_I != 0) {
            double potentialIGain = (m_totalError + m_error) * m_I;
            if (potentialIGain < m_maximumOutput) {
              if (potentialIGain > m_minimumOutput) {
                m_totalError += m_error;
              } else {
                m_totalError = m_minimumOutput / m_I;
              }
            } else {
              m_totalError = m_maximumOutput / m_I;
            }
          }

          m_result = m_P * m_error + m_I * m_totalError
              + m_D * (m_error - m_prevError) + calculateFeedForward();
        }
        m_prevError = m_error;

        if (m_result > m_maximumOutput) {
          m_result = m_maximumOutput;
        } else if (m_result < m_minimumOutput) {
          m_result = m_minimumOutput;
        }
        pidOutput = m_pidOutput;
        result = m_result;

        // Update the buffer.
        m_buf.add(m_error);
        m_bufTotal += m_error;
        // Remove old elements when the buffer is full.
        if (m_buf.size() > m_bufLength) {
          m_bufTotal -= m_buf.remove();
        }
      }

      pidOutput.pidWrite(result);
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
  public synchronized void setPID(double p, double i, double d) {
    m_P = p;
    m_I = i;
    m_D = d;

    if (m_pEntry != null) {
      m_pEntry.setDouble(p);
    }
    if (m_iEntry != null) {
      m_iEntry.setDouble(i);
    }
    if (m_dEntry != null) {
      m_dEntry.setDouble(d);
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
  public synchronized void setPID(double p, double i, double d, double f) {
    m_P = p;
    m_I = i;
    m_D = d;
    m_F = f;

    if (m_pEntry != null) {
      m_pEntry.setDouble(p);
    }
    if (m_iEntry != null) {
      m_iEntry.setDouble(i);
    }
    if (m_dEntry != null) {
      m_dEntry.setDouble(d);
    }
    if (m_fEntry != null) {
      m_fEntry.setDouble(f);
    }
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public synchronized double getP() {
    return m_P;
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public synchronized double getI() {
    return m_I;
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public synchronized double getD() {
    return m_D;
  }

  /**
   * Get the Feed forward coefficient.
   *
   * @return feed forward coefficient
   */
  public synchronized double getF() {
    return m_F;
  }

  /**
   * Return the current PID result This is always centered on zero and constrained the the max and
   * min outs.
   *
   * @return the latest calculated output
   */
  public synchronized double get() {
    return m_result;
  }

  /**
   * Set the PID controller to consider the input to be continuous, Rather then using the max and
   * min in as constraints, it considers them to be the same point and automatically calculates the
   * shortest route to the setpoint.
   *
   * @param continuous Set to true turns on continuous, false turns off continuous
   */
  public synchronized void setContinuous(boolean continuous) {
    m_continuous = continuous;
  }

  /**
   * Set the PID controller to consider the input to be continuous, Rather then using the max and
   * min in as constraints, it considers them to be the same point and automatically calculates the
   * shortest route to the setpoint.
   */
  public synchronized void setContinuous() {
    setContinuous(true);
  }

  /**
   * Sets the maximum and minimum values expected from the input and setpoint.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the input
   */
  public synchronized void setInputRange(double minimumInput, double maximumInput) {
    if (minimumInput > maximumInput) {
      throw new BoundaryException("Lower bound is greater than upper bound");
    }
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
    setSetpoint(m_setpoint);
  }

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum percentage to write to the output
   * @param maximumOutput the maximum percentage to write to the output
   */
  public synchronized void setOutputRange(double minimumOutput, double maximumOutput) {
    if (minimumOutput > maximumOutput) {
      throw new BoundaryException("Lower bound is greater than upper bound");
    }
    m_minimumOutput = minimumOutput;
    m_maximumOutput = maximumOutput;
  }

  /**
   * Set the setpoint for the PIDController Clears the queue for GetAvgError().
   *
   * @param setpoint the desired setpoint
   */
  public synchronized void setSetpoint(double setpoint) {
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

    m_buf.clear();
    m_bufTotal = 0;

    if (m_setpointEntry != null) {
      m_setpointEntry.setDouble(m_setpoint);
    }
  }

  /**
   * Returns the current setpoint of the PIDController.
   *
   * @return the current setpoint
   */
  public synchronized double getSetpoint() {
    return m_setpoint;
  }

  /**
   * Returns the change in setpoint over time of the PIDController.
   *
   * @return the change in setpoint over time
   */
  public synchronized double getDeltaSetpoint() {
    return (m_setpoint - m_prevSetpoint) / m_setpointTimer.get();
  }

  /**
   * Returns the current difference of the input from the setpoint.
   *
   * @return the current error
   */
  public synchronized double getError() {
    return getContinuousError(getSetpoint() - m_pidInput.pidGet());
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
   * Returns the current difference of the error over the past few iterations. You can specify the
   * number of iterations to average with setToleranceBuffer() (defaults to 1). getAvgError() is
   * used for the onTarget() function.
   *
   * @return the current average of the error
   */
  public synchronized double getAvgError() {
    double avgError = 0;
    // Don't divide by zero.
    if (m_buf.size() != 0) {
      avgError = m_bufTotal / m_buf.size();
    }
    return avgError;
  }

  /**
   * Returns whether or not any values have been collected. If no values have been collected,
   * getAvgError is 0, which is invalid.
   *
   * @return True if {@link #getAvgError()} is currently valid.
   */
  private synchronized boolean isAvgErrorValid() {
    return m_buf.size() != 0;
  }

  /**
   * Set the PID tolerance using a Tolerance object. Tolerance can be specified as a percentage of
   * the range or as an absolute value. The Tolerance object encapsulates those options in an
   * object. Use it by creating the type of tolerance that you want to use: setTolerance(new
   * PIDController.AbsoluteTolerance(0.1))
   *
   * @param tolerance a tolerance object of the right type, e.g. PercentTolerance or
   *                  AbsoluteTolerance
   */
  public void setTolerance(Tolerance tolerance) {
    m_tolerance = tolerance;
  }

  /**
   * Set the absolute error which is considered tolerable for use with OnTarget.
   *
   * @param absvalue absolute error which is tolerable in the units of the input object
   */
  public synchronized void setAbsoluteTolerance(double absvalue) {
    m_tolerance = new AbsoluteTolerance(absvalue);
  }

  /**
   * Set the percentage error which is considered tolerable for use with OnTarget. (Input of 15.0 =
   * 15 percent)
   *
   * @param percentage percent error which is tolerable
   */
  public synchronized void setPercentTolerance(double percentage) {
    m_tolerance = new PercentageTolerance(percentage);
  }

  /**
   * Set the number of previous error samples to average for tolerancing. When determining whether a
   * mechanism is on target, the user may want to use a rolling average of previous measurements
   * instead of a precise position or velocity. This is useful for noisy sensors which return a few
   * erroneous measurements when the mechanism is on target. However, the mechanism will not
   * register as on target for at least the specified bufLength cycles.
   *
   * @param bufLength Number of previous cycles to average.
   */
  public synchronized void setToleranceBuffer(int bufLength) {
    m_bufLength = bufLength;

    // Cut the existing buffer down to size if needed.
    while (m_buf.size() > bufLength) {
      m_bufTotal -= m_buf.remove();
    }
  }

  /**
   * Return true if the error is within the percentage of the total input range, determined by
   * setTolerance. This assumes that the maximum and minimum input were set using setInput.
   *
   * @return true if the error is less than the tolerance
   */
  public synchronized boolean onTarget() {
    return m_tolerance.onTarget();
  }

  /**
   * Begin running the PIDController.
   */
  @Override
  public synchronized void enable() {
    m_enabled = true;

    if (m_enabledEntry != null) {
      m_enabledEntry.setBoolean(true);
    }
  }

  /**
   * Stop running the PIDController, this sets the output to zero before stopping.
   */
  @Override
  public synchronized void disable() {
    m_pidOutput.pidWrite(0);
    m_enabled = false;

    if (m_enabledEntry != null) {
      m_enabledEntry.setBoolean(false);
    }
  }

  /**
   * Return true if PIDController is enabled.
   */
  @Override
  public boolean isEnabled() {
    return m_enabled;
  }

  /**
   * Reset the previous error,, the integral term, and disable the controller.
   */
  @Override
  public synchronized void reset() {
    disable();
    m_prevError = 0;
    m_totalError = 0;
    m_result = 0;
  }

  @Override
  public String getSmartDashboardType() {
    return "PIDController";
  }

  @SuppressWarnings("MemberName")
  private NetworkTableEntry m_pEntry;
  @SuppressWarnings("MemberName")
  private NetworkTableEntry m_iEntry;
  @SuppressWarnings("MemberName")
  private NetworkTableEntry m_dEntry;
  @SuppressWarnings("MemberName")
  private NetworkTableEntry m_fEntry;
  private NetworkTableEntry m_setpointEntry;
  private NetworkTableEntry m_enabledEntry;
  @SuppressWarnings("MemberName")
  private int m_pListener;
  @SuppressWarnings("MemberName")
  private int m_iListener;
  @SuppressWarnings("MemberName")
  private int m_dListener;
  @SuppressWarnings("MemberName")
  private int m_fListener;
  private int m_setpointListener;
  private int m_enabledListener;

  private void removeListeners() {
    if (m_pEntry != null) {
      m_pEntry.removeListener(m_pListener);
    }
    if (m_iEntry != null) {
      m_iEntry.removeListener(m_iListener);
    }
    if (m_dEntry != null) {
      m_dEntry.removeListener(m_dListener);
    }
    if (m_fEntry != null) {
      m_fEntry.removeListener(m_fListener);
    }
    if (m_setpointEntry != null) {
      m_setpointEntry.removeListener(m_setpointListener);
    }
    if (m_enabledEntry != null) {
      m_enabledEntry.removeListener(m_enabledListener);
    }
  }

  @Override
  public void initTable(NetworkTable table) {
    removeListeners();
    if (table != null) {
      m_pEntry = table.getEntry("p");
      m_pEntry.setDouble(getP());
      m_iEntry = table.getEntry("i");
      m_iEntry.setDouble(getI());
      m_dEntry = table.getEntry("d");
      m_dEntry.setDouble(getD());
      m_fEntry = table.getEntry("f");
      m_fEntry.setDouble(getF());
      m_setpointEntry = table.getEntry("setpoint");
      m_setpointEntry.setDouble(getSetpoint());
      m_enabledEntry = table.getEntry("enabled");
      m_enabledEntry.setBoolean(isEnabled());

      m_pListener = m_pEntry.addListener((entry) -> {
        synchronized (this) {
          m_P = entry.value.getDouble();
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);

      m_iListener = m_iEntry.addListener((entry) -> {
        synchronized (this) {
          m_I = entry.value.getDouble();
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);

      m_dListener = m_dEntry.addListener((entry) -> {
        synchronized (this) {
          m_D = entry.value.getDouble();
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);

      m_fListener = m_fEntry.addListener((entry) -> {
        synchronized (this) {
          m_F = entry.value.getDouble();
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);

      m_setpointListener = m_setpointEntry.addListener((entry) -> {
        double val = entry.value.getDouble();
        if (getSetpoint() != val) {
          setSetpoint(val);
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);

      m_enabledListener = m_enabledEntry.addListener((entry) -> {
        boolean val = entry.value.getBoolean();
        if (isEnabled() != val) {
          if (val) {
            enable();
          } else {
            disable();
          }
        }
      }, EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);
    } else {
      m_pEntry = null;
      m_iEntry = null;
      m_dEntry = null;
      m_fEntry = null;
      m_setpointEntry = null;
      m_enabledEntry = null;
    }
  }

  /**
   * Wraps error around for continuous inputs. The original error is returned if continuous mode is
   * disabled. This is an unsynchronized function.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  protected double getContinuousError(double error) {
    if (m_continuous && Math.abs(error) > (m_maximumInput - m_minimumInput) / 2) {
      if (error > 0) {
        return error - (m_maximumInput - m_minimumInput);
      } else {
        return error + (m_maximumInput - m_minimumInput);
      }
    }

    return error;
  }

  @Override
  public void updateTable() {
  }


  @Override
  public void startLiveWindowMode() {
    disable();
  }


  @Override
  public void stopLiveWindowMode() {
  }
}
