/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/PIDSubsystem.h"

#include "PIDController.h"

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * @param name the name
 * @param p    the proportional value
 * @param i    the integral value
 * @param d    the derivative value
 */
frc::PIDSubsystem::PIDSubsystem(const std::string& name, double p, double i,
                                double d)
    : Subsystem(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * @param name the name
 * @param p    the proportional value
 * @param i    the integral value
 * @param d    the derivative value
 * @param f    the feedforward value
 */
frc::PIDSubsystem::PIDSubsystem(const std::string& name, double p, double i,
                                double d, double f)
    : Subsystem(name) {
  m_controller = std::make_shared<PIDController>(p, i, d, f, this, this);
}

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * It will also space the time between PID loop calculations to be equal to the
 * given period.
 *
 * @param name   the name
 * @param p      the proportional value
 * @param i      the integral value
 * @param d      the derivative value
 * @param f      the feedfoward value
 * @param period the time (in seconds) between calculations
 */
frc::PIDSubsystem::PIDSubsystem(const std::string& name, double p, double i,
                                double d, double f, double period)
    : Subsystem(name) {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * It will use the class name as its name.
 *
 * @param p the proportional value
 * @param i the integral value
 * @param d the derivative value
 */
frc::PIDSubsystem::PIDSubsystem(double p, double i, double d)
    : Subsystem("PIDSubsystem") {
  m_controller = std::make_shared<PIDController>(p, i, d, this, this);
}

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * It will use the class name as its name.
 *
 * @param p the proportional value
 * @param i the integral value
 * @param d the derivative value
 * @param f the feedforward value
 */
frc::PIDSubsystem::PIDSubsystem(double p, double i, double d, double f)
    : Subsystem("PIDSubsystem") {
  m_controller = std::make_shared<PIDController>(p, i, d, f, this, this);
}

/**
 * Instantiates a {@link PIDSubsystem} that will use the given p, i and d
 * values.
 *
 * It will use the class name as its name. It will also space the time
 * between PID loop calculations to be equal to the given period.
 *
 * @param p      the proportional value
 * @param i      the integral value
 * @param d      the derivative value
 * @param f      the feedforward value
 * @param period the time (in seconds) between calculations
 */
frc::PIDSubsystem::PIDSubsystem(double p, double i, double d, double f,
                                double period)
    : Subsystem("PIDSubsystem") {
  m_controller =
      std::make_shared<PIDController>(p, i, d, f, this, this, period);
}

/**
 * Enables the internal {@link PIDController}.
 */
void frc::PIDSubsystem::Enable() { m_controller->Enable(); }

/**
  * Disables the internal {@link PIDController}.
  */
void frc::PIDSubsystem::Disable() { m_controller->Disable(); }

/**
 * Returns the {@link PIDController} used by this {@link PIDSubsystem}.
 *
 * Use this if you would like to fine tune the pid loop.
 *
 * @return the {@link PIDController} used by this {@link PIDSubsystem}
 */
std::shared_ptr<frc::PIDController> frc::PIDSubsystem::GetPIDController() {
  return m_controller;
}

/**
 * Sets the setpoint to the given value.
 *
 * If {@link PIDCommand#SetRange(double, double) SetRange(...)} was called,
 * then the given setpoint will be trimmed to fit within the range.
 *
 * @param setpoint the new setpoint
 */
void frc::PIDSubsystem::SetSetpoint(double setpoint) {
  m_controller->SetSetpoint(setpoint);
}

/**
 * Adds the given value to the setpoint.
 *
 * If {@link PIDCommand#SetRange(double, double) SetRange(...)} was used,
 * then the bounds will still be honored by this method.
 *
 * @param deltaSetpoint the change in the setpoint
 */
void frc::PIDSubsystem::SetSetpointRelative(double deltaSetpoint) {
  SetSetpoint(GetSetpoint() + deltaSetpoint);
}

/**
 * Return the current setpoint.
 *
 * @return The current setpoint
 */
double frc::PIDSubsystem::GetSetpoint() { return m_controller->GetSetpoint(); }

/**
 * Sets the maximum and minimum values expected from the input.
 *
 * @param minimumInput the minimum value expected from the input
 * @param maximumInput the maximum value expected from the output
 */
void frc::PIDSubsystem::SetInputRange(double minimumInput,
                                      double maximumInput) {
  m_controller->SetInputRange(minimumInput, maximumInput);
}

/**
 * Sets the maximum and minimum values to write.
 *
 * @param minimumOutput the minimum value to write to the output
 * @param maximumOutput the maximum value to write to the output
 */
void frc::PIDSubsystem::SetOutputRange(double minimumOutput,
                                       double maximumOutput) {
  m_controller->SetOutputRange(minimumOutput, maximumOutput);
}

/**
 * Set the absolute error which is considered tolerable for use with
 * OnTarget.
 *
 * @param absValue absolute error which is tolerable
 */
void frc::PIDSubsystem::SetAbsoluteTolerance(double absValue) {
  m_controller->SetAbsoluteTolerance(absValue);
}

/**
 * Set the percentage error which is considered tolerable for use with OnTarget.
 *
 * @param percent percentage error which is tolerable
 */
void frc::PIDSubsystem::SetPercentTolerance(double percent) {
  m_controller->SetPercentTolerance(percent);
}

/**
 * Return true if the error is within the percentage of the total input range,
 * determined by SetTolerance.
 *
 * This asssumes that the maximum and minimum input were set using SetInput.
 * Use OnTarget() in the IsFinished() method of commands that use this
 * subsystem.
 *
 * Currently this just reports on target as the actual value passes through the
 * setpoint. Ideally it should be based on being within the tolerance for some
 * period of time.
 *
 * @return true if the error is within the percentage tolerance of the input
 *         range
 */
bool frc::PIDSubsystem::OnTarget() const { return m_controller->OnTarget(); }

/**
 * Returns the current position.
 *
 * @return the current position
 */
double frc::PIDSubsystem::GetPosition() { return ReturnPIDInput(); }

/**
 * Returns the current rate.
 *
 * @return the current rate
 */
double frc::PIDSubsystem::GetRate() { return ReturnPIDInput(); }

void frc::PIDSubsystem::PIDWrite(double output) { UsePIDOutput(output); }

double frc::PIDSubsystem::PIDGet() { return ReturnPIDInput(); }

std::string frc::PIDSubsystem::GetSmartDashboardType() const {
  return "PIDCommand";
}

void frc::PIDSubsystem::InitTable(std::shared_ptr<ITable> subtable) {
  m_controller->InitTable(subtable);
  Subsystem::InitTable(subtable);
}
