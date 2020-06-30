/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>

#include <frc/controller/PIDController.h>
#include <frc/controller/RamseteController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/trajectory/Trajectory.h>
#include <units/length.h>
#include <units/voltage.h>
#include <wpi/ArrayRef.h>

#include "frc2/Timer.h"
#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

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
 * @see RamseteController
 * @see Trajectory
 */
class RamseteCommand : public CommandHelper<CommandBase, RamseteCommand> {
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
   */
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::SimpleMotorFeedforward<units::meters> feedforward,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<frc::DifferentialDriveWheelSpeeds()> wheelSpeeds,
                 frc2::PIDController leftController,
                 frc2::PIDController rightController,
                 std::function<void(units::volt_t, units::volt_t)> output,
                 std::initializer_list<Subsystem*> requirements);

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
   */
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::SimpleMotorFeedforward<units::meters> feedforward,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<frc::DifferentialDriveWheelSpeeds()> wheelSpeeds,
                 frc2::PIDController leftController,
                 frc2::PIDController rightController,
                 std::function<void(units::volt_t, units::volt_t)> output,
                 wpi::ArrayRef<Subsystem*> requirements = {});

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
   */
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<void(units::meters_per_second_t,
                                    units::meters_per_second_t)>
                     output,
                 std::initializer_list<Subsystem*> requirements);

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
   */
  RamseteCommand(frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
                 frc::RamseteController controller,
                 frc::DifferentialDriveKinematics kinematics,
                 std::function<void(units::meters_per_second_t,
                                    units::meters_per_second_t)>
                     output,
                 wpi::ArrayRef<Subsystem*> requirements = {});

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::RamseteController m_controller;
  frc::SimpleMotorFeedforward<units::meters> m_feedforward;
  frc::DifferentialDriveKinematics m_kinematics;
  std::function<frc::DifferentialDriveWheelSpeeds()> m_speeds;
  std::unique_ptr<frc2::PIDController> m_leftController;
  std::unique_ptr<frc2::PIDController> m_rightController;
  std::function<void(units::volt_t, units::volt_t)> m_outputVolts;
  std::function<void(units::meters_per_second_t, units::meters_per_second_t)>
      m_outputVel;

  Timer m_timer;
  units::second_t m_prevTime;
  frc::DifferentialDriveWheelSpeeds m_prevSpeeds;
  bool m_usePID;
};
}  // namespace frc2
