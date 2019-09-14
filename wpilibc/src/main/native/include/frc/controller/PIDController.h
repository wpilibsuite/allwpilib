/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <limits>

#include <units/units.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc2 {

/**
 * Implements a PID control loop.
 */
class PIDController : public frc::Sendable,
                      public frc::SendableHelper<PIDController> {
 public:
  /**
   * Allocates a PIDController with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp     The proportional coefficient.
   * @param Ki     The integral coefficient.
   * @param Kd     The derivative coefficient.
   * @param period The period between controller updates in seconds. The
   *               default is 20 milliseconds.
   */
  PIDController(double Kp, double Ki, double Kd,
                units::second_t period = 20_ms);

  ~PIDController() override = default;

  PIDController(const PIDController&) = default;
  PIDController& operator=(const PIDController&) = default;
  PIDController(PIDController&&) = default;
  PIDController& operator=(PIDController&&) = default;

  /**
   * Sets the PID Controller gain parameters.
   *
   * Sets the proportional, integral, and differential coefficients.
   *
   * @param Kp Proportional coefficient
   * @param Ki Integral coefficient
   * @param Kd Differential coefficient
   */
  void SetPID(double Kp, double Ki, double Kd);

  /**
   * Sets the proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  void SetP(double Kp);

  /**
   * Sets the integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  void SetI(double Ki);

  /**
   * Sets the differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  void SetD(double Kd);

  /**
   * Gets the proportional coefficient.
   *
   * @return proportional coefficient
   */
  double GetP() const;

  /**
   * Gets the integral coefficient.
   *
   * @return integral coefficient
   */
  double GetI() const;

  /**
   * Gets the differential coefficient.
   *
   * @return differential coefficient
   */
  double GetD() const;

  /**
   * Gets the period of this controller.
   *
   * @return The period of the controller.
   */
  units::second_t GetPeriod() const;

  /**
   * Sets the setpoint for the PIDController.
   *
   * @param setpoint The desired setpoint.
   */
  void SetSetpoint(double setpoint);

  /**
   * Returns the current setpoint of the PIDController.
   *
   * @return The current setpoint.
   */
  double GetSetpoint() const;

  /**
   * Returns true if the error is within the tolerance of the error.
   *
   * This will return false until at least one input value has been computed.
   */
  bool AtSetpoint() const;

  /**
   * Enables continuous input.
   *
   * Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  void EnableContinuousInput(double minimumInput, double maximumInput);

  /**
   * Disables continuous input.
   */
  void DisableContinuousInput();

  /**
   * Sets the minimum and maximum values for the integrator.
   *
   * When the cap is reached, the integrator value is added to the controller
   * output rather than the integrator value times the integral gain.
   *
   * @param minimumIntegral The minimum value of the integrator.
   * @param maximumIntegral The maximum value of the integrator.
   */
  void SetIntegratorRange(double minimumIntegral, double maximumIntegral);

  /**
   * Sets the error which is considered tolerable for use with AtSetpoint().
   *
   * @param positionTolerance Position error which is tolerable.
   * @param velociytTolerance Velocity error which is tolerable.
   */
  void SetTolerance(
      double positionTolerance,
      double velocityTolerance = std::numeric_limits<double>::infinity());

  /**
   * Returns the difference between the setpoint and the measurement.
   */
  double GetPositionError() const;

  /**
   * Returns the velocity error.
   */
  double GetVelocityError() const;

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   */
  double Calculate(double measurement);

  /**
   * Returns the next output of the PID controller.
   *
   * @param measurement The current measurement of the process variable.
   * @param setpoint The new setpoint of the controller.
   */
  double Calculate(double measurement, double setpoint);

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  void Reset();

  void InitSendable(frc::SendableBuilder& builder) override;

 protected:
  /**
   * Wraps error around for continuous inputs. The original error is returned if
   * continuous mode is disabled.
   *
   * @param error The current error of the PID controller.
   * @return Error for continuous inputs.
   */
  double GetContinuousError(double error) const;

 private:
  // Factor for "proportional" control
  double m_Kp;

  // Factor for "integral" control
  double m_Ki;

  // Factor for "derivative" control
  double m_Kd;

  // The period (in seconds) of the control loop running this controller
  units::second_t m_period;

  double m_maximumIntegral = 1.0;

  double m_minimumIntegral = -1.0;

  // Maximum input - limit setpoint to this
  double m_maximumInput = 0;

  // Minimum input - limit setpoint to this
  double m_minimumInput = 0;

  // Input range - difference between maximum and minimum
  double m_inputRange = 0;

  // Do the endpoints wrap around? eg. Absolute encoder
  bool m_continuous = false;

  // The error at the time of the most recent call to Calculate()
  double m_positionError = 0;
  double m_velocityError = 0;

  // The error at the time of the second-most-recent call to Calculate() (used
  // to compute velocity)
  double m_prevError = 0;

  // The sum of the errors for use in the integral calc
  double m_totalError = 0;

  // The error that is considered at setpoint.
  double m_positionTolerance = 0.05;
  double m_velocityTolerance = std::numeric_limits<double>::infinity();

  double m_setpoint = 0;

  /**
   * Sets the minimum and maximum values expected from the input.
   *
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  void SetInputRange(double minimumInput, double maximumInput);
};

}  // namespace frc2
