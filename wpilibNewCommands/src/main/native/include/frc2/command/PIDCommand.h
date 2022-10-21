// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <span>

#include <frc/controller/PIDController.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that controls an output with a PIDController.  Runs forever by
 * default - to add exit conditions and/or other behavior, subclass this class.
 * The controller calculation and output are performed synchronously in the
 * command's execute() method.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see PIDController
 */
class PIDCommand : public CommandHelper<CommandBase, PIDCommand> {
 public:
  /**
   * Creates a new PIDCommand, which controls the given output with a
   * PIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpointSource   the controller's reference (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  PIDCommand(PIDController controller,
             std::function<double()> measurementSource,
             std::function<double()> setpointSource,
             std::function<void(double)> useOutput,
             std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * PIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpointSource   the controller's reference (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  PIDCommand(PIDController controller,
             std::function<double()> measurementSource,
             std::function<double()> setpointSource,
             std::function<void(double)> useOutput,
             std::span<Subsystem* const> requirements = {});

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * PIDController with a constant setpoint.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpoint         the controller's setpoint (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  PIDCommand(PIDController controller,
             std::function<double()> measurementSource, double setpoint,
             std::function<void(double)> useOutput,
             std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * PIDController with a constant setpoint.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param setpoint         the controller's setpoint (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  PIDCommand(PIDController controller,
             std::function<double()> measurementSource, double setpoint,
             std::function<void(double)> useOutput,
             std::span<Subsystem* const> requirements = {});

  PIDCommand(PIDCommand&& other) = default;

  PIDCommand(const PIDCommand& other) = default;

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  /**
   * Returns the PIDController used by the command.
   *
   * @return The PIDController
   */
  PIDController& GetController();

 protected:
  PIDController m_controller;
  std::function<double()> m_measurement;
  std::function<double()> m_setpoint;
  std::function<void(double)> m_useOutput;
};
}  // namespace frc2
