/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "PIDController.h"
#include "Notifier.h"
#include "PIDSource.h"
#include "PIDOutput.h"
#include <math.h>
#include <vector>
#include "HAL/HAL.hpp"

static const std::string kP = "p";
static const std::string kI = "i";
static const std::string kD = "d";
static const std::string kF = "f";
static const std::string kSetpoint = "setpoint";
static const std::string kEnabled = "enabled";

/**
 * Allocate a PID object with the given constants for P, I, D
 * @param Kp the proportional coefficient
 * @param Ki the integral coefficient
 * @param Kd the derivative coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly effects
 * calculations of the
 * integral and differental terms. The default is 50ms.
 */
PIDController::PIDController(float Kp, float Ki, float Kd, PIDSource *source,
                             PIDOutput *output, float period) {
  Initialize(Kp, Ki, Kd, 0.0f, source, output, period);
}

/**
 * Allocate a PID object with the given constants for P, I, D
 * @param Kp the proportional coefficient
 * @param Ki the integral coefficient
 * @param Kd the derivative coefficient
 * @param source The PIDSource object that is used to get values
 * @param output The PIDOutput object that is set to the output value
 * @param period the loop time for doing calculations. This particularly effects
 * calculations of the
 * integral and differental terms. The default is 50ms.
 */
PIDController::PIDController(float Kp, float Ki, float Kd, float Kf,
                             PIDSource *source, PIDOutput *output, float period) {
  Initialize(Kp, Ki, Kd, Kf, source, output, period);
}

void PIDController::Initialize(float Kp, float Ki, float Kd, float Kf,
                               PIDSource *source, PIDOutput *output,
                               float period) {
  m_controlLoop = std::make_unique<Notifier>(PIDController::CallCalculate, this);

  m_P = Kp;
  m_I = Ki;
  m_D = Kd;
  m_F = Kf;

  m_pidInput = source;
  m_pidOutput = output;
  m_period = period;

  m_controlLoop->StartPeriodic(m_period);

  static int32_t instances = 0;
  instances++;
  HALReport(HALUsageReporting::kResourceType_PIDController, instances);
}

/**
 * Call the Calculate method as a non-static method. This avoids having to
 * prepend
 * all local variables in that method with the class pointer. This way the
 * "this"
 * pointer will be set up and class variables can be called more easily.
 * This method is static and called by the Notifier class.
 * @param controller the address of the PID controller object to use in the
 * background loop
 */
void PIDController::CallCalculate(void *controller) {
  PIDController *control = (PIDController *)controller;
  control->Calculate();
}

/**
 * Read the input, calculate the output accordingly, and write to the output.
 * This should only be called by the Notifier indirectly through CallCalculate
 * and is created during initialization.
 */
void PIDController::Calculate() {
  bool enabled;
  PIDSource *pidInput;
  PIDOutput *pidOutput;

  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    pidInput = m_pidInput;
    pidOutput = m_pidOutput;
    enabled = m_enabled;
    pidInput = m_pidInput;
  }

  if (pidInput == nullptr) return;
  if (pidOutput == nullptr) return;

  if (enabled) {
    {
      std::unique_lock<priority_mutex> sync(m_mutex);
      float input = pidInput->PIDGet();
      float result;
      PIDOutput *pidOutput;

      m_error = m_setpoint - input;
      if (m_continuous) {
        if (fabs(m_error) > (m_maximumInput - m_minimumInput) / 2) {
          if (m_error > 0) {
            m_error = m_error - m_maximumInput + m_minimumInput;
          } else {
            m_error = m_error + m_maximumInput - m_minimumInput;
          }
        }
      }

      if (m_I != 0) {
        double potentialIGain = (m_totalError + m_error) * m_I;
        if (potentialIGain < m_maximumOutput) {
          if (potentialIGain > m_minimumOutput)
            m_totalError += m_error;
          else
            m_totalError = m_minimumOutput / m_I;
        } else {
          m_totalError = m_maximumOutput / m_I;
        }
      }

      m_result = m_P * m_error + m_I * m_totalError +
                 m_D * (m_prevInput - input) + m_setpoint * m_F;
      m_prevInput = input;

      if (m_result > m_maximumOutput)
        m_result = m_maximumOutput;
      else if (m_result < m_minimumOutput)
        m_result = m_minimumOutput;

      pidOutput = m_pidOutput;
      result = m_result;

      pidOutput->PIDWrite(result);
    }
  }
}

/**
 * Set the PID Controller gain parameters.
 * Set the proportional, integral, and differential coefficients.
 * @param p Proportional coefficient
 * @param i Integral coefficient
 * @param d Differential coefficient
 */
void PIDController::SetPID(double p, double i, double d) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_P = p;
    m_I = i;
    m_D = d;
  }

  if (m_table != nullptr) {
    m_table->PutNumber("p", m_P);
    m_table->PutNumber("i", m_I);
    m_table->PutNumber("d", m_D);
  }
}

/**
 * Set the PID Controller gain parameters.
 * Set the proportional, integral, and differential coefficients.
 * @param p Proportional coefficient
 * @param i Integral coefficient
 * @param d Differential coefficient
 * @param f Feed forward coefficient
 */
void PIDController::SetPID(double p, double i, double d, double f) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_P = p;
    m_I = i;
    m_D = d;
    m_F = f;
  }

  if (m_table != nullptr) {
    m_table->PutNumber("p", m_P);
    m_table->PutNumber("i", m_I);
    m_table->PutNumber("d", m_D);
    m_table->PutNumber("f", m_F);
  }
}

/**
 * Get the Proportional coefficient
 * @return proportional coefficient
 */
double PIDController::GetP() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_P;
}

/**
 * Get the Integral coefficient
 * @return integral coefficient
 */
double PIDController::GetI() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_I;
}

/**
 * Get the Differential coefficient
 * @return differential coefficient
 */
double PIDController::GetD() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_D;
}

/**
 * Get the Feed forward coefficient
 * @return Feed forward coefficient
 */
double PIDController::GetF() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_F;
}

/**
 * Return the current PID result
 * This is always centered on zero and constrained the the max and min outs
 * @return the latest calculated output
 */
float PIDController::Get() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_result;
}

/**
 *  Set the PID controller to consider the input to be continuous,
 *  Rather then using the max and min in as constraints, it considers them to
 *  be the same point and automatically calculates the shortest route to
 *  the setpoint.
 * @param continuous Set to true turns on continuous, false turns off continuous
 */
void PIDController::SetContinuous(bool continuous) {
  std::unique_lock<priority_mutex> sync(m_mutex);
  m_continuous = continuous;
}

/**
 * Sets the maximum and minimum values expected from the input.
 *
 * @param minimumInput the minimum value expected from the input
 * @param maximumInput the maximum value expected from the output
 */
void PIDController::SetInputRange(float minimumInput, float maximumInput) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_minimumInput = minimumInput;
    m_maximumInput = maximumInput;
  }

  SetSetpoint(m_setpoint);
}

/**
 * Sets the minimum and maximum values to write.
 *
 * @param minimumOutput the minimum value to write to the output
 * @param maximumOutput the maximum value to write to the output
 */
void PIDController::SetOutputRange(float minimumOutput, float maximumOutput) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_minimumOutput = minimumOutput;
    m_maximumOutput = maximumOutput;
  }
}

/**
 * Set the setpoint for the PIDController
 * @param setpoint the desired setpoint
 */
void PIDController::SetSetpoint(float setpoint) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    if (m_maximumInput > m_minimumInput) {
      if (setpoint > m_maximumInput)
        m_setpoint = m_maximumInput;
      else if (setpoint < m_minimumInput)
        m_setpoint = m_minimumInput;
      else
        m_setpoint = setpoint;
    } else {
      m_setpoint = setpoint;
    }
  }

  if (m_table != nullptr) {
    m_table->PutNumber("setpoint", m_setpoint);
  }
}

/**
 * Returns the current setpoint of the PIDController
 * @return the current setpoint
 */
double PIDController::GetSetpoint() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_setpoint;
}

/**
 * Retruns the current difference of the input from the setpoint
 * @return the current error
 */
float PIDController::GetError() const {
  double pidInput;
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    pidInput = m_pidInput->PIDGet();
  }
  return GetSetpoint() - pidInput;
}

/*
 * Set the percentage error which is considered tolerable for use with
 * OnTarget.
 * @param percentage error which is tolerable
 */
void PIDController::SetTolerance(float percent) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_toleranceType = kPercentTolerance;
    m_tolerance = percent;
  }
}

/*
 * Set the percentage error which is considered tolerable for use with
 * OnTarget.
 * @param percentage error which is tolerable
 */
void PIDController::SetPercentTolerance(float percent) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_toleranceType = kPercentTolerance;
    m_tolerance = percent;
  }
}

/*
 * Set the absolute error which is considered tolerable for use with
 * OnTarget.
 * @param percentage error which is tolerable
 */
void PIDController::SetAbsoluteTolerance(float absTolerance) {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_toleranceType = kAbsoluteTolerance;
    m_tolerance = absTolerance;
  }
}

/*
 * Return true if the error is within the percentage of the total input range,
 * determined by SetTolerance. This asssumes that the maximum and minimum input
 * were set using SetInput.
 * Currently this just reports on target as the actual value passes through the
 * setpoint.
 * Ideally it should be based on being within the tolerance for some period of
 * time.
 */
bool PIDController::OnTarget() const {
  double error = GetError();

  std::unique_lock<priority_mutex> sync(m_mutex);
  switch (m_toleranceType) {
    case kPercentTolerance:
      return fabs(error) < m_tolerance / 100 * (m_maximumInput - m_minimumInput);
      break;
    case kAbsoluteTolerance:
      return fabs(error) < m_tolerance;
      break;
    case kNoTolerance:
      // TODO: this case needs an error
      return false;
  }
  return false;
}

/**
 * Begin running the PIDController
 */
void PIDController::Enable() {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_enabled = true;
  }

  if (m_table != nullptr) {
    m_table->PutBoolean("enabled", true);
  }
}

/**
 * Stop running the PIDController, this sets the output to zero before stopping.
 */
void PIDController::Disable() {
  {
    std::unique_lock<priority_mutex> sync(m_mutex);
    m_pidOutput->PIDWrite(0);
    m_enabled = false;
  }

  if (m_table != nullptr) {
    m_table->PutBoolean("enabled", false);
  }
}

/**
 * Return true if PIDController is enabled.
 */
bool PIDController::IsEnabled() const {
  std::unique_lock<priority_mutex> sync(m_mutex);
  return m_enabled;
}

/**
 * Reset the previous error,, the integral term, and disable the controller.
 */
void PIDController::Reset() {
  Disable();

  std::unique_lock<priority_mutex> sync(m_mutex);
  m_prevInput = 0;
  m_totalError = 0;
  m_result = 0;
}

std::string PIDController::GetSmartDashboardType() const {
  return "PIDController";
}

void PIDController::InitTable(std::shared_ptr<ITable> table) {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
  m_table = table;
  if (m_table != nullptr) {
    m_table->PutNumber(kP, GetP());
    m_table->PutNumber(kI, GetI());
    m_table->PutNumber(kD, GetD());
    m_table->PutNumber(kF, GetF());
    m_table->PutNumber(kSetpoint, GetSetpoint());
    m_table->PutBoolean(kEnabled, IsEnabled());
    m_table->AddTableListener(this, false);
  }
}

std::shared_ptr<ITable> PIDController::GetTable() const { return m_table; }

void PIDController::ValueChanged(std::shared_ptr<ITable> source, const std::string &key,
                                 EntryValue value, bool isNew) {
  if (key == kP || key == kI || key == kD || key == kF) {
    if (m_P != m_table->GetNumber(kP) || m_I != m_table->GetNumber(kI) ||
        m_D != m_table->GetNumber(kD) || m_F != m_table->GetNumber(kF)) {
      SetPID(m_table->GetNumber(kP, 0.0), m_table->GetNumber(kI, 0.0),
             m_table->GetNumber(kD, 0.0), m_table->GetNumber(kF, 0.0));
    }
  } else if (key == kSetpoint && m_setpoint != value.f) {
    SetSetpoint(value.f);
  } else if (key == kEnabled && m_enabled != value.b) {
    if (value.b) {
      Enable();
    } else {
      Disable();
    }
  }
}

void PIDController::UpdateTable() {}

void PIDController::StartLiveWindowMode() { Disable(); }

void PIDController::StopLiveWindowMode() {}
