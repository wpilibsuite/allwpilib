/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <limits>

#include <wpi/mutex.h>

#include "frc/experimental/controller/Controller.h"
#include "frc/experimental/controller/MeasurementSource.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {
namespace experimental {

// TODO: remove when this class is moved into frc namespace
using frc::SendableBase;
using frc::SendableBuilder;

/**
 * Class implements a PID Control Loop.
 *
 * Creates a separate thread which reads the given PIDSource and takes care of
 * the integral calculations, as well as writing the given PIDOutput.
 *
 * This feedback controller runs in discrete time, so time deltas are not used
 * in the integral and derivative calculations. Therefore, the sample rate
 * affects the controller's behavior for a given set of PID constants.
 */
class PIDController : public Controller, public SendableBase {
 public:
  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp          The proportional coefficient.
   * @param Ki          The integral coefficient.
   * @param Kd          The derivative coefficient.
   * @param measurement The function used to take measurements.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  PIDController(double Kp, double Ki, double Kd,
                std::function<double()> measurement, double period = 0.005);

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp                The proportional coefficient.
   * @param Ki                The integral coefficient.
   * @param Kd                The derivative coefficient.
   * @param measurementSource The function used to take measurements.
   * @param period            The period between controller updates in seconds.
   *                          The default is 5ms.
   */
  PIDController(double Kp, double Ki, double Kd,
                MeasurementSource& measurementSource, double period = 0.005);

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp          The proportional coefficient.
   * @param Ki          The integral coefficient.
   * @param Kd          The derivative coefficient.
   * @param feedforward The arbitrary feedforward function.
   * @param measurement The function used to take measurements.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  PIDController(double Kp, double Ki, double Kd,
                std::function<double()> feedforward,
                std::function<double()> measurement, double period = 0.005);

  /**
   * Allocate a PID object with the given constants for Kp, Ki, and Kd.
   *
   * @param Kp                The proportional coefficient.
   * @param Ki                The integral coefficient.
   * @param Kd                The derivative coefficient.
   * @param feedforward       The arbitrary feedforward function.
   * @param measurementSource The function used to take measurements.
   * @param period            The period between controller updates in seconds.
   *                          The default is 5ms.
   */
  PIDController(double Kp, double Ki, double Kd,
                std::function<double()> feedforward,
                MeasurementSource& measurementSource, double period = 0.005);

  ~PIDController() override = default;

  PIDController(PIDController&&) = default;
  PIDController& operator=(PIDController&&) = default;

  /**
   * Set the PID Controller gain parameters.
   *
   * Set the proportional, integral, and differential coefficients.
   *
   * @param Kp Proportional coefficient
   * @param Ki Integral coefficient
   * @param Kd Differential coefficient
   */
  void SetPID(double Kp, double Ki, double Kd);

  /**
   * Set the Proportional coefficient of the PID controller gain.
   *
   * @param Kp proportional coefficient
   */
  void SetP(double Kp);

  /**
   * Set the Integral coefficient of the PID controller gain.
   *
   * @param Ki integral coefficient
   */
  void SetI(double Ki);

  /**
   * Set the Differential coefficient of the PID controller gain.
   *
   * @param Kd differential coefficient
   */
  void SetD(double Kd);

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  double GetP() const;

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  double GetI() const;

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  double GetD() const;

  /**
   * Return the current controller output.
   *
   * This is always centered around zero and constrained to the min and max
   * outputs.
   *
   * @return The latest calculated output.
   */
  double GetOutput() const;

  /**
   * Set the reference for the PIDController.
   *
   * @param reference the desired reference
   */
  void SetReference(double reference);

  /**
   * Returns the current reference of the PIDController.
   *
   * @return the current reference
   */
  double GetReference() const;

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
  bool AtReference() const;

  /**
   * Set the PID controller to consider the input to be continuous,
   *
   * Rather then using the max and min input range as constraints, it considers
   * them to be the same point and automatically calculates the shortest route
   * to the setpoint.
   *
   * @param continuous true turns on continuous, false turns off continuous
   */
  void SetContinuous(bool continuous = true);

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the output
   */
  void SetInputRange(double minimumInput, double maximumInput);

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minimumOutput the minimum value to write to the output
   * @param maximumOutput the maximum value to write to the output
   */
  void SetOutputRange(double minimumOutput, double maximumOutput);

  /**
   * Set the absolute error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance      Error which is tolerable.
   * @param deltaTolerance Change in error per second which is tolerable.
   */
  void SetAbsoluteTolerance(
      double tolerance,
      double deltaTolerance = std::numeric_limits<double>::infinity());

  /**
   * Set the percentage error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance      Percent error which is tolerable.
   * @param deltaTolerance Change in percent error per second which is
   *                       tolerable.
   */
  void SetPercentTolerance(
      double tolerance,
      double deltaTolerance = std::numeric_limits<double>::infinity());

  /**
   * Returns the difference between the reference and the measurement.
   *
   * @return The error.
   */
  double GetError() const;

  /**
   * Returns the change in error per second.
   */
  double GetDeltaError() const;

  double Update() override;

  /**
   * Reset the previous error, the integral term, and disable the controller.
   */
  void Reset();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  mutable wpi::mutex m_thisMutex;

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
  double m_Kp;

  // Factor for "integral" control
  double m_Ki;

  // Factor for "derivative" control
  double m_Kd;

  // Arbitrary feedforward function
  std::function<double()> m_feedforward;

  // Function for taking measurements
  std::function<double()> m_measurementSource;

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

  enum class Tolerance { kAbsolute, kPercent };

  Tolerance m_toleranceType = Tolerance::kAbsolute;

  // The percentage or absolute error that is considered at reference.
  double m_tolerance = 0.05;
  double m_deltaTolerance = std::numeric_limits<double>::infinity();

  double m_reference = 0;
  double m_output = 0;
};

}  // namespace experimental
}  // namespace frc

// TODO: remove when deprecated class is removed
#include "frc/PIDController.h"
