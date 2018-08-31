/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include <wpi/Twine.h>

#include "frc/commands/Command.h"
#include "frc/experimental/controller/ControllerOutput.h"
#include "frc/experimental/controller/ControllerRunner.h"
#include "frc/experimental/controller/MeasurementSource.h"
#include "frc/experimental/controller/PIDController.h"

namespace frc {
namespace experimental {

// TODO: remove when this class is moved into frc namespace
using frc::Command;
using frc::SendableBuilder;
using frc::Subsystem;

class PIDCommand : public Command,
                   public ControllerOutput,
                   public MeasurementSource {
 public:
  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name The name of the command.
   * @param Kp   The proportional value.
   * @param Ki   The integral value.
   * @param Kd   The derivative value.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name   The name of the command.
   * @param Kp     The proportional value.
   * @param Ki     The integral value.
   * @param Kd     The derivative value.
   * @param period The period between controller updates in seconds. The default
   *               is 5ms.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
             double period);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name of the command.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
             std::function<double()> feedforward, double period);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp The proportional value.
   * @param Ki The integral value.
   * @param Kd The derivative value.
   */
  PIDCommand(double Kp, double Ki, double Kd);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp     The proportional value.
   * @param Ki     The integral value.
   * @param Kd     The derivative value.
   * @param period The period between controller updates in seconds. The default
   *               is 5ms.
   */
  PIDCommand(double Kp, double Ki, double Kd, double period);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   */
  PIDCommand(double Kp, double Ki, double Kd,
             std::function<double()> feedforward, double period);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name      The name of the command.
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param subsystem The subsystem that this command requires.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
             Subsystem& subsystem);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name      The name of the command.
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param period    The period between controller updates in seconds. The
   *                  default is 5ms.
   * @param subsystem The subsystem that this command requires.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
             double period, Subsystem& subsystem);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param name        The name of the command.
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   * @param subsystem   The subsystem that this command requires.
   */
  PIDCommand(const wpi::Twine& name, double Kp, double Ki, double Kd,
             std::function<double()> feedforward, double period,
             Subsystem& subsystem);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param subsystem The subsystem that this command requires.
   */
  PIDCommand(double Kp, double Ki, double Kd, Subsystem& subsystem);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp        The proportional value.
   * @param Ki        The integral value.
   * @param Kd        The derivative value.
   * @param period    The period between controller updates in seconds. The
   *                  default is 5ms.
   * @param subsystem The subsystem that this command requires.
   */
  PIDCommand(double Kp, double Ki, double Kd, double period,
             Subsystem& subsystem);

  /**
   * Instantiates a PIDCommand that will use the given Kp, Ki, and Kd values.
   *
   * @param Kp          The proportional value.
   * @param Ki          The integral value.
   * @param Kd          The derivative value.
   * @param feedforward The arbitrary feedforward function.
   * @param period      The period between controller updates in seconds. The
   *                    default is 5ms.
   * @param subsystem   The subsystem that this command requires.
   */
  PIDCommand(double Kp, double Ki, double Kd,
             std::function<double()> feedforward, double period,
             Subsystem& subsystem);

  virtual ~PIDCommand() = default;

  PIDCommand(PIDCommand&&) = default;
  PIDCommand& operator=(PIDCommand&&) = default;

  /**
   * Adds the given value to the reference. If
   * PIDCommand::SetInputRange(double, double) was used, then the bounds will
   * still be honored by this method.
   *
   * @param deltaReference The change in the reference.
   */
  void SetReferenceRelative(double deltaReference);

  // ControllerOutput interface
  void SetOutput(double output) override = 0;

  // MeasurementSource interface
  double GetMeasurement() const override = 0;

  void InitSendable(SendableBuilder& builder) override;

 protected:
  const PIDController& GetPIDController() const;
  void _Initialize() override;
  void _Interrupted() override;
  void _End() override;

  /**
   * Set the reference for the PIDController.
   *
   * @param reference The desired reference.
   */
  void SetReference(double reference);

  /**
   * Returns the current reference of the PIDController.
   *
   * @return The current reference.
   */
  double GetReference() const;

 private:
  // The internal PIDController
  PIDController m_controller;
  ControllerRunner m_runner;
};

}  // namespace experimental
}  // namespace frc
