// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>

#include <frc/Timer.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/RamseteController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/trajectory/Trajectory.h>
#include <units/length.h>
#include <units/voltage.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/Requirements.h"

namespace frc2 {
/**
 * A command that uses a RAMSETE controller  to follow a trajectory
 * with a differential drive.
 *
 * <p>The command handles trajectory-following, PID calculations, and
 * feedforwards internally.  This is intended to be a more-or-less "complete
 * solution" that can be used by teams without a great deal of controls
 * expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to
 * use the onboard PID functionality of a "smart" motor controller) may use the
 * secondary constructor that omits the PID and feedforward functionality,
 * returning only the raw wheel speeds from the RAMSETE controller.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see RamseteController
 * @see Trajectory
 */
class RamseteCommand : public CommandHelper<Command, RamseteCommand> {
 public:
  /**
   * Constructs a new RamseteCommand that, when executed, will follow the
   * provided trajectory. PID control and feedforward are handled internally,
   * and outputs are scaled -12 to 12 representing units of volts.
   *
   * <p>Note: The controller will *not* set the outputVolts to zero upon
   * completion of the path - this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose - use one of
   * the odometry classes to provide this.
   * @param controller      The RAMSETE controller used to follow the
   * trajectory.
   * @param feedforward     A component for calculating the feedforward for the
   * drive.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param wheelSpeeds     A function that supplies the speeds of the left
   * and right sides of the robot drive.
   * @param leftController  The PIDController for the left side of the robot
   * drive.
   * @param rightController The PIDController for the right side of the robot
   * drive.
   * @param output          A function that consumes the computed left and right
   * outputs (in volts) for the robot drive.
   * @param requirements    The subsystems to require.
   * @deprecated Use LTVUnicycleController instead.
   */
  [[deprecated("Use LTVUnicycleController instead.")]]
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::SimpleMotorFeedforward<units::meters> feedforward,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<frc::DifferentialDriveWheelSpeeds()> wheelSpeeds,
                 frc::PIDController leftController,
                 frc::PIDController rightController,
                 std::function<void(units::volt_t, units::volt_t)> output,
                 Requirements requirements = {});

  /**
   * Constructs a new RamseteCommand that, when executed, will follow the
   * provided trajectory. Performs no PID control and calculates no
   * feedforwards; outputs are the raw wheel speeds from the RAMSETE controller,
   * and will need to be converted into a usable form by the user.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose - use one of
   * the odometry classes to provide this.
   * @param controller      The RAMSETE controller used to follow the
   * trajectory.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param output          A function that consumes the computed left and right
   * wheel speeds.
   * @param requirements    The subsystems to require.
   * @deprecated Use LTVUnicycleController instead.
   */
  [[deprecated("Use LTVUnicycleController instead.")]]
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<void(units::meters_per_second_t,
                                    units::meters_per_second_t)> output,
                 Requirements requirements = {});

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::RamseteController m_controller;
  frc::SimpleMotorFeedforward<units::meters> m_feedforward;
  frc::DifferentialDriveKinematics m_kinematics;
  std::function<frc::DifferentialDriveWheelSpeeds()> m_speeds;
  std::unique_ptr<frc::PIDController> m_leftController;
  std::unique_ptr<frc::PIDController> m_rightController;
  std::function<void(units::volt_t, units::volt_t)> m_outputVolts;
  std::function<void(units::meters_per_second_t, units::meters_per_second_t)>
      m_outputVel;

  frc::Timer m_timer;
  units::second_t m_prevTime;
  frc::DifferentialDriveWheelSpeeds m_prevSpeeds;
  bool m_usePID;
};
}  // namespace frc2
