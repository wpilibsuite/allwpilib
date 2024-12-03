// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <utility>

#include <frc/controller/ProfiledPIDController.h>
#include <units/time.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/Requirements.h"

namespace frc2 {
/**
 * A command that controls an output with a ProfiledPIDController.  Runs forever
 * by default - to add exit conditions and/or other behavior, subclass this
 * class. The controller calculation and output are performed synchronously in
 * the command's execute() method.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see ProfiledPIDController<Distance>
 */
template <class Distance>
class ProfiledPIDCommand
    : public CommandHelper<Command, ProfiledPIDCommand<Distance>> {
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using State = typename frc::TrapezoidProfile<Distance>::State;

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
   * @deprecated Use a ProfiledPIDController instead
   */
  [[deprecated("Use a ProfiledPIDController instead")]]
  ProfiledPIDCommand(frc::ProfiledPIDController<Distance> controller,
                     std::function<Distance_t()> measurementSource,
                     std::function<State()> goalSource,
                     std::function<void(double, State)> useOutput,
                     Requirements requirements = {})
      : m_controller{controller},
        m_measurement{std::move(measurementSource)},
        m_goal{std::move(goalSource)},
        m_useOutput{std::move(useOutput)} {
    this->AddRequirements(requirements);
  }

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goalSource   the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   * @deprecated Use a ProfiledPIDController instead
   */
  [[deprecated("Use a ProfiledPIDController instead")]]
  ProfiledPIDCommand(frc::ProfiledPIDController<Distance> controller,
                     std::function<Distance_t()> measurementSource,
                     std::function<Distance_t()> goalSource,
                     std::function<void(double, State)> useOutput,
                     Requirements requirements = {})
      : ProfiledPIDCommand(
            controller, measurementSource,
            [goalSource = std::move(goalSource)]() {
              return State{goalSource(), Velocity_t{0}};
            },
            useOutput, requirements) {}

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController with a constant goal.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal         the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   * @deprecated Use a ProfiledPIDController instead
   */

  [[deprecated("Use a ProfiledPIDController instead")]]
  ProfiledPIDCommand(frc::ProfiledPIDController<Distance> controller,
                     std::function<Distance_t()> measurementSource, State goal,
                     std::function<void(double, State)> useOutput,
                     Requirements requirements = {})
      : ProfiledPIDCommand(
            controller, measurementSource, [goal] { return goal; }, useOutput,
            requirements) {}

  /**
   * Creates a new PIDCommand, which controls the given output with a
   * ProfiledPIDController with a constant goal.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param goal         the controller's goal
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   * @deprecated Use a ProfiledPIDController instead
   */
  [[deprecated("Use a ProfiledPIDController instead")]]
  ProfiledPIDCommand(frc::ProfiledPIDController<Distance> controller,
                     std::function<Distance_t()> measurementSource,
                     Distance_t goal,
                     std::function<void(double, State)> useOutput,
                     Requirements requirements = {})
      : ProfiledPIDCommand(
            controller, measurementSource, [goal] { return goal; }, useOutput,
            requirements) {}

  ProfiledPIDCommand(ProfiledPIDCommand&& other) = default;

  ProfiledPIDCommand(const ProfiledPIDCommand& other) = default;

  void Initialize() override { m_controller.Reset(m_measurement()); }

  void Execute() override {
    m_useOutput(m_controller.Calculate(m_measurement(), m_goal()),
                m_controller.GetSetpoint());
  }

  void End(bool interrupted) override {
    m_useOutput(0, State{Distance_t(0), Velocity_t(0)});
  }

  /**
   * Returns the ProfiledPIDController used by the command.
   *
   * @return The ProfiledPIDController
   */
  frc::ProfiledPIDController<Distance>& GetController() { return m_controller; }

 protected:
  /// Profiled PID controller.
  frc::ProfiledPIDController<Distance> m_controller;

  /// Measurement getter.
  std::function<Distance_t()> m_measurement;

  /// Goal getter.
  std::function<State()> m_goal;

  /// Profiled PID controller output consumer.
  std::function<void(double, State)> m_useOutput;
};
}  // namespace frc2
