// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/deprecated.h>
#include <wpi/mutex.h>

#include "frc/Base.h"
#include "frc/LinearFilter.h"
#include "frc/PIDInterface.h"
#include "frc/PIDOutput.h"
#include "frc/PIDSource.h"
#include "frc/Timer.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * Class implements a PID Control Loop.
 *
 * Creates a separate thread which reads the given PIDSource and takes care of
 * the integral calculations, as well as writing the given PIDOutput.
 *
 * This feedback controller runs in discrete time, so time deltas are not used
 * in the integral and derivative calculations. Therefore, the sample rate
 * affects the controller's behavior for a given set of PID constants.
 *
 * @deprecated All APIs which use this have been deprecated.
 */
class PIDBase : public PIDInterface,
                public PIDOutput,
                public Sendable,
                public SendableHelper<PIDBase> {
 public:
  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   */
  WPI_DEPRECATED("All APIs which use this have been deprecated.")
  PIDBase(double p, double i, double d, PIDSource& source, PIDOutput& output);

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp     the proportional coefficient
   * @param Ki     the integral coefficient
   * @param Kd     the derivative coefficient
   * @param source The PIDSource object that is used to get values
   * @param output The PIDOutput object that is set to the output value
   */
  WPI_DEPRECATED("All APIs which use this have been deprecated.")
  PIDBase(double p, double i, double d, double f, PIDSource& source,
          PIDOutput& output);

  ~PIDBase() override = default;

  /**
   * Return the current PID result.
   *
   * This is always centered on zero and constrained the the max and min outs.
   *
   * @return the latest calculated output
   */
  virtual double Get() const;

  /**
   * Set the PID controller to consider the input to be continuous,
   *
   * Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param continuous true turns on continuous, false turns off continuous
   */
  virtual void SetContinuous(bool continuous = true);

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the output
   */
  virtual void SetInputRange(double minimumInput, double maximumInput);

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum value to write to the output
   * @param maximumOutput the maximum value to write to the output
   */
  virtual void SetOutputRange(double minimumOutput, double maximumOutput);

  /**
   * Set the PID Controller gain parameters.
   *
   * Set the proportional, integral, and differential coefficients.
   *
   * @param p Proportional coefficient
   * @param i Integral coefficient
   * @param d Differential coefficient
   */
  void SetPID(double p, double i, double d) override;

  /**
   * Set the PID Controller gain parameters.
   *
   * Set the proportional, integral, and differential coefficients.
   *
   * @param p Proportional coefficient
   * @param i Integral coefficient
   * @param d Differential coefficient
   * @param f Feed forward coefficient
   */
  virtual void SetPID(double p, double i, double d, double f);

  /**
   * Set the Proportional coefficient of the PID controller gain.
   *
   * @param p proportional coefficient
   */
  void SetP(double p);

  /**
   * Set the Integral coefficient of the PID controller gain.
   *
   * @param i integral coefficient
   */
  void SetI(double i);

  /**
   * Set the Differential coefficient of the PID controller gain.
   *
   * @param d differential coefficient
   */
  void SetD(double d);

  /**
   * Get the Feed forward coefficient of the PID controller gain.
   *
   * @param f Feed forward coefficient
   */
  void SetF(double f);

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  double GetP() const override;

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  double GetI() const override;

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  double GetD() const override;

  /**
   * Get the Feed forward coefficient.
   *
   * @return Feed forward coefficient
   */
  virtual double GetF() const;

  /**
   * Set the setpoint for the PIDBase.
   *
   * @param setpoint the desired setpoint
   */
  void SetSetpoint(double setpoint) override;

  /**
   * Returns the current setpoint of the PIDBase.
   *
   * @return the current setpoint
   */
  double GetSetpoint() const override;

  /**
   * Returns the change in setpoint over time of the PIDBase.
   *
   * @return the change in setpoint over time
   */
  double GetDeltaSetpoint() const;

  /**
   * Returns the current difference of the input from the setpoint.
   *
   * @return the current error
   */
  virtual double GetError() const;

  /**
   * Returns the current average of the error over the past few iterations.
   *
   * You can specify the number of iterations to average with
   * SetToleranceBuffer() (defaults to 1). This is the same value that is used
   * for OnTarget().
   *
   * @return the average error
   */
  WPI_DEPRECATED("Use a LinearFilter as the input and GetError().")
  virtual double GetAvgError() const;

  /**
   * Sets what type of input the PID controller will use.
   */
  virtual void SetPIDSourceType(PIDSourceType pidSource);

  /**
   * Returns the type of input the PID controller is using.
   *
   * @return the PID controller input type
   */
  virtual PIDSourceType GetPIDSourceType() const;

  /**
   * Set the percentage error which is considered tolerable for use with
   * OnTarget.
   *
   * @param percentage error which is tolerable
   */
  WPI_DEPRECATED("Use SetPercentTolerance() instead.")
  virtual void SetTolerance(double percent);

  /**
   * Set the absolute error which is considered tolerable for use with
   * OnTarget.
   *
   * @param percentage error which is tolerable
   */
  virtual void SetAbsoluteTolerance(double absValue);

  /**
   * Set the percentage error which is considered tolerable for use with
   * OnTarget.
   *
   * @param percentage error which is tolerable
   */
  virtual void SetPercentTolerance(double percentValue);

  /**
   * Set the number of previous error samples to average for tolerancing. When
   * determining whether a mechanism is on target, the user may want to use a
   * rolling average of previous measurements instead of a precise position or
   * velocity. This is useful for noisy sensors which return a few erroneous
   * measurements when the mechanism is on target. However, the mechanism will
   * not register as on target for at least the specified bufLength cycles.
   *
   * @param bufLength Number of previous cycles to average. Defaults to 1.
   */
  WPI_DEPRECATED("Use a LinearDigitalFilter as the input.")
  virtual void SetToleranceBuffer(int buf = 1);

  /**
   * Return true if the error is within the percentage of the total input range,
   * determined by SetTolerance. This asssumes that the maximum and minimum
   * input were set using SetInput.
   *
   * Currently this just reports on target as the actual value passes through
   * the setpoint. Ideally it should be based on being within the tolerance for
   * some period of time.
   *
   * This will return false until at least one input value has been computed.
   */
  virtual bool OnTarget() const;

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  void Reset() override;

  /**
   * Passes the output directly to SetSetpoint().
   *
   * PIDControllers can be nested by passing a PIDController as another
   * PIDController's output. In that case, the output of the parent controller
   * becomes the input (i.e., the reference) of the child.
   *
   * It is the caller's responsibility to put the data into a valid form for
   * SetSetpoint().
   */
  void PIDWrite(double output) override;

  void InitSendable(SendableBuilder& builder) override;

 protected:
  // Is the pid controller enabled
  bool m_enabled = false;

  mutable wpi::mutex m_thisMutex;

  // Ensures when Disable() is called, PIDWrite() won't run if Calculate()
  // is already running at that time.
  mutable wpi::mutex m_pidWriteMutex;

  PIDSource* m_pidInput;
  PIDOutput* m_pidOutput;
  Timer m_setpointTimer;

  /**
   * Read the input, calculate the output accordingly, and write to the output.
   * This should only be called by the Notifier.
   */
  virtual void Calculate();

  /**
   * Calculate the feed forward term.
   *
   * Both of the provided feed forward calculations are velocity feed forwards.
   * If a different feed forward calculation is desired, the user can override
   * this function and provide his or her own. This function does no
   * synchronization because the PIDBase class only calls it in synchronized
   * code, so be careful if calling it oneself.
   *
   * If a velocity PID controller is being used, the F term should be set to 1
   * over the maximum setpoint for the output. If a position PID controller is
   * being used, the F term should be set to 1 over the maximum speed for the
   * output measured in setpoint units per this controller's update period (see
   * the default period in this class's constructor).
   */
  virtual double CalculateFeedForward();

  /**
   * Wraps error around for continuous inputs. The original error is returned if
   * continuous mode is disabled. This is an unsynchronized function.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  double GetContinuousError(double error) const;

 private:
  // Factor for "proportional" control
  double m_P;

  // Factor for "integral" control
  double m_I;

  // Factor for "derivative" control
  double m_D;

  // Factor for "feed forward" control
  double m_F;

  // |maximum output|
  double m_maximumOutput = 1.0;

  // |minimum output|
  double m_minimumOutput = -1.0;

  // Maximum input - limit setpoint to this
  double m_maximumInput = 0;

  // Minimum input - limit setpoint to this
  double m_minimumInput = 0;

  // input range - difference between maximum and minimum
  double m_inputRange = 0;

  // Do the endpoints wrap around? eg. Absolute encoder
  bool m_continuous = false;

  // The prior error (used to compute velocity)
  double m_prevError = 0;

  // The sum of the errors for use in the integral calc
  double m_totalError = 0;

  enum {
    kAbsoluteTolerance,
    kPercentTolerance,
    kNoTolerance
  } m_toleranceType = kNoTolerance;

  // The percetage or absolute error that is considered on target.
  double m_tolerance = 0.05;

  double m_setpoint = 0;
  double m_prevSetpoint = 0;
  double m_error = 0;
  double m_result = 0;

  LinearFilter<double> m_filter{{}, {}};
};

}  // namespace frc
