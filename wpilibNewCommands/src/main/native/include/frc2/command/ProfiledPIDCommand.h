/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <initializer_list>

#include <frc/controller/ProfiledPIDController.h>
#include <units/units.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that controls an output with a ProfiledPIDController.  Runs forever
 * by default - to add exit conditions and/or other behavior, subclass this
 * class. The controller calculation and output are performed synchronously in
 * the command's execute() method.
 *
 * @see ProfiledPIDController
 */
class ProfiledPIDCommand
    : public CommandHelper<CommandBase, ProfiledPIDCommand> {
  using State = frc::TrapezoidProfile::State;

 public:
  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goalSource   the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  ProfiledPIDCommand(frc::ProfiledPIDController controller,
                     std::function<units::meter_t()> measurementSource,
                     std::function<State()> goalSource,
                     std::function<void(double, State)> useOutput,
                     std::initializer_list<Subsystem*> requirements = {});

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goalSource   the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  ProfiledPIDCommand(frc::ProfiledPIDController controller,
                     std::function<units::meter_t()> measurementSource,
                     std::function<units::meter_t()> goalSource,
                     std::function<void(double, State)> useOutput,
                     std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController with a constant goal.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal         the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  ProfiledPIDCommand(frc::ProfiledPIDController controller,
                     std::function<units::meter_t()> measurementSource,
                     State goal, std::function<void(double, State)> useOutput,
                     std::initializer_list<Subsystem*> requirements);

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController with a constant goal.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal         the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  ProfiledPIDCommand(frc::ProfiledPIDController controller,
                     std::function<units::meter_t()> measurementSource,
                     units::meter_t goal,
                     std::function<void(double, State)> useOutput,
                     std::initializer_list<Subsystem*> requirements);

  ProfiledPIDCommand(ProfiledPIDCommand&& other) = default;

  ProfiledPIDCommand(const ProfiledPIDCommand& other) = default;

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  /**
   * Returns the ProfiledPIDController used by the command.
   *
   * @return The ProfiledPIDController
   */
  frc::ProfiledPIDController& GetController();

 protected:
  frc::ProfiledPIDController m_controller;
  std::function<units::meter_t()> m_measurement;
  std::function<State()> m_goal;
  std::function<void(double, State)> m_useOutput;
};
}  // namespace frc2
