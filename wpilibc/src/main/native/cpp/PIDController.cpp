/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PIDController.h"

#include "Notifier.h"
#include "PIDOutput.h"
#include "SmartDashboard/SendableBuilder.h"

using namespace frc;

/**
 * Allocate a PID object with the given constants for Kp, Ki, and Kd.
 *
 * @param Kp     the proportional coefficient
 * @param Ki     the integral coefficient
 * @param Kd     the derivative coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly
 *               effects calculations of the integral and differental terms.
 *               The default is 50ms.
 */
PIDController::PIDController(double Kp, double Ki, double Kd, PIDSource* source,
                             PIDOutput* output, double period)
    : PIDController(Kp, Ki, Kd, 0.0, 0.0, *source, *output, period) {}

/**
 * Allocate a PID object with the given constants for Kp, Ki, Kd, and Kv.
 *
 * @param Kp     the proportional coefficient
 * @param Ki     the integral coefficient
 * @param Kd     the derivative coefficient
 * @param Kv     the velocity feedforward coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly
 *               effects calculations of the integral and differental terms.
 *               The default is 50ms.
 */
PIDController::PIDController(double Kp, double Ki, double Kd, double Kv,
                             PIDSource* source, PIDOutput* output,
                             double period)
    : PIDController(Kp, Ki, Kd, Kv, 0.0, *source, *output, period) {}

/**
 * Allocate a PID object with the given constants for Kp, Ki, and Kd.
 *
 * @param Kp     the proportional coefficient
 * @param Ki     the integral coefficient
 * @param Kd     the derivative coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly
 *               effects calculations of the integral and differental terms.
 *               The default is 50ms.
 */
PIDController::PIDController(double Kp, double Ki, double Kd, PIDSource& source,
                             PIDOutput& output, double period)
    : PIDController(Kp, Ki, Kd, 0.0, 0.0, source, output, period) {}

/**
 * Allocate a PID object with the given constants for Kp, Ki, Kd, and Kv.
 *
 * @param Kp     the proportional coefficient
 * @param Ki     the integral coefficient
 * @param Kd     the derivative coefficient
 * @param Kv     the velocity feedforward coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly
 *               effects calculations of the integral and differental terms.
 *               The default is 50ms.
 */
PIDController::PIDController(double Kp, double Ki, double Kd, double Kv,
                             PIDSource& source, PIDOutput& output,
                             double period)
    : PIDController(Kp, Ki, Kd, Kv, 0.0, source, output, period) {}

/**
 * Allocate a PID object with the given constants for Kp, Ki, Kd, Kv, and Ka.
 *
 * @param Kp     the proportional coefficient
 * @param Ki     the integral coefficient
 * @param Kd     the derivative coefficient
 * @param Kv     the velocity feedforward coefficient
 * @param Ka     the acceleration feedforward coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly
 *               effects calculations of the integral and differental terms.
 *               The default is 50ms.
 */
PIDController::PIDController(double Kp, double Ki, double Kd, double Kv,
                             double Ka, PIDSource& source, PIDOutput& output,
                             double period)
    : PIDBase(Kp, Ki, Kd, Kv, Ka, source, output) {
  m_controlLoop = std::make_unique<Notifier>(&PIDController::Calculate, this);
  m_controlLoop->StartPeriodic(period);
  m_period = period;
}

PIDController::~PIDController() {
  // Forcefully stopping the notifier so the callback can successfully run.
  m_controlLoop->Stop();
}

/**
 * Begin running the PIDController.
 */
void PIDController::Enable() {
  {
    std::lock_guard<wpi::mutex> lock(m_thisMutex);
    m_enabled = true;
  }
}

/**
 * Stop running the PIDController, this sets the output to zero before stopping.
 */
void PIDController::Disable() {
  {
    // Ensures m_enabled modification and PIDWrite() call occur atomically
    std::lock_guard<wpi::mutex> pidWriteLock(m_pidWriteMutex);
    {
      std::lock_guard<wpi::mutex> mainLock(m_thisMutex);
      m_enabled = false;
    }

    m_pidOutput->PIDWrite(0);
  }
}

/**
 * Set the enabled state of the PIDController.
 */
void PIDController::SetEnabled(bool enable) {
  if (enable) {
    Enable();
  } else {
    Disable();
  }
}

/**
 * Return true if PIDController is enabled.
 */
bool PIDController::IsEnabled() const {
  std::lock_guard<wpi::mutex> lock(m_thisMutex);
  return m_enabled;
}

/**
 * Calculate the feed forward term.
 *
 * Both of the provided feed forward calculations are velocity feed forwards.
 * If a different feed forward calculation is desired, the user can override
 * this function and provide his or her own. This function does no
 * synchronization because the PIDBase class only calls it in synchronized
 * code, so be careful if calling it oneself.
 *
 * If a velocity PID controller is being used, the Kv term should be set to 1
 * over the maximum setpoint for the output. If a position PID controller is
 * being used, the Kv term should be set to 1 over the maximum speed for the
 * output measured in setpoint units per second.
 */
double PIDController::CalculateFeedForward() {
  if (m_pidInput->GetPIDSourceType() == PIDSourceType::kRate) {
    return (GetV() * GetSetpoint() + GetA() * GetDeltaSetpoint()) * m_period;
  } else {
    const double deltaSetpoint = GetDeltaSetpoint();
    const double output =
        GetV() * deltaSetpoint + GetA() * (deltaSetpoint - m_prevDeltaSetpoint);
    m_prevSetpoint = GetSetpoint();
    m_prevDeltaSetpoint = deltaSetpoint;
    m_setpointTimer.Reset();
    return output * m_period;
  }
}

/**
 * Reset the previous error, the integral term, and disable the controller.
 */
void PIDController::Reset() {
  Disable();

  PIDBase::Reset();
}

void PIDController::InitSendable(SendableBuilder& builder) {
  PIDBase::InitSendable(builder);
  builder.AddBooleanProperty("enabled", [=]() { return IsEnabled(); },
                             [=](bool value) { SetEnabled(value); });
}
