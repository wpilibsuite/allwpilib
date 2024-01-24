// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <memory>

#include <frc/Timer.h>
#include <frc/controller/HolonomicDriveController.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/ChassisSpeeds.h>
#include <frc/kinematics/MecanumDriveKinematics.h>
#include <frc/kinematics/MecanumDriveWheelSpeeds.h>
#include <frc/trajectory/Trajectory.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/velocity.h>
#include <units/voltage.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/Requirements.h"

#pragma once

namespace frc2 {
/**
 * A command that uses two PID controllers (PIDController) and a profiled PID
 * controller (ProfiledPIDController) to follow a trajectory (Trajectory) with a
 * mecanum drive.
 *
 * <p>The command handles trajectory-following,
 * Velocity PID calculations, and feedforwards internally. This
 * is intended to be a more-or-less "complete solution" that can be used by
 * teams without a great deal of controls expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to
 * use the onboard PID functionality of a "smart" motor controller) may use the
 * secondary constructor that omits the PID and feedforward functionality,
 * returning only the raw wheel speeds from the PID controllers.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the
 * trajectory.
 *
 * This class is provided by the NewCommands VendorDep
 */
class MecanumControllerCommand
    : public CommandHelper<Command, MecanumControllerCommand> {
 public:
  /**
   * Constructs a new MecanumControllerCommand that when executed will follow
   * the provided trajectory. PID control and feedforward are handled
   * internally. Outputs are scaled from -12 to 12 as a voltage output to the
   * motor.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * @param trajectory           The trajectory to follow.
   * @param pose                 A function that supplies the robot pose,
   *                             provided by the odometry class.
   * @param feedforward          The feedforward to use for the drivetrain.
   * @param kinematics           The kinematics for the robot drivetrain.
   * @param xController          The Trajectory Tracker PID controller
   *                             for the robot's x position.
   * @param yController          The Trajectory Tracker PID controller
   *                             for the robot's y position.
   * @param thetaController      The Trajectory Tracker PID controller
   *                             for angle for the robot.
   * @param desiredRotation      The angle that the robot should be facing.
   *                             This is sampled at each time step.
   * @param maxWheelVelocity     The maximum velocity of a drivetrain wheel.
   * @param frontLeftController  The front left wheel velocity PID.
   * @param rearLeftController   The rear left wheel velocity PID.
   * @param frontRightController The front right wheel velocity PID.
   * @param rearRightController  The rear right wheel velocity PID.
   * @param currentWheelSpeeds   A MecanumDriveWheelSpeeds object containing
   *                             the current wheel speeds.
   * @param output               The output of the velocity PIDs.
   * @param requirements         The subsystems to require.
   */
  MecanumControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SimpleMotorFeedforward<units::meters> feedforward,
      frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
      frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      std::function<frc::Rotation2d()> desiredRotation,
      units::meters_per_second_t maxWheelVelocity,
      std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
      frc::PIDController frontLeftController,
      frc::PIDController rearLeftController,
      frc::PIDController frontRightController,
      frc::PIDController rearRightController,
      std::function<void(units::volt_t, units::volt_t, units::volt_t,
                         units::volt_t)>
          output,
      Requirements requirements = {});

  /**
   * Constructs a new MecanumControllerCommand that when executed will follow
   * the provided trajectory. PID control and feedforward are handled
   * internally. Outputs are scaled from -12 to 12 as a voltage output to the
   * motor.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * <p>Note 2: The final rotation of the robot will be set to the rotation of
   * the final pose in the trajectory. The robot will not follow the rotations
   * from the poses at each timestep. If alternate rotation behavior is desired,
   * the other constructor with a supplier for rotation should be used.
   *
   * @param trajectory           The trajectory to follow.
   * @param pose                 A function that supplies the robot pose,
   *                             provided by the odometry class.
   * @param feedforward          The feedforward to use for the drivetrain.
   * @param kinematics           The kinematics for the robot drivetrain.
   * @param xController          The Trajectory Tracker PID controller
   *                             for the robot's x position.
   * @param yController          The Trajectory Tracker PID controller
   *                             for the robot's y position.
   * @param thetaController      The Trajectory Tracker PID controller
   *                             for angle for the robot.
   * @param maxWheelVelocity     The maximum velocity of a drivetrain wheel.
   * @param frontLeftController  The front left wheel velocity PID.
   * @param rearLeftController   The rear left wheel velocity PID.
   * @param frontRightController The front right wheel velocity PID.
   * @param rearRightController  The rear right wheel velocity PID.
   * @param currentWheelSpeeds   A MecanumDriveWheelSpeeds object containing
   *                             the current wheel speeds.
   * @param output               The output of the velocity PIDs.
   * @param requirements         The subsystems to require.
   */
  MecanumControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SimpleMotorFeedforward<units::meters> feedforward,
      frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
      frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      units::meters_per_second_t maxWheelVelocity,
      std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
      frc::PIDController frontLeftController,
      frc::PIDController rearLeftController,
      frc::PIDController frontRightController,
      frc::PIDController rearRightController,
      std::function<void(units::volt_t, units::volt_t, units::volt_t,
                         units::volt_t)>
          output,
      Requirements requirements = {});

  /**
   * Constructs a new MecanumControllerCommand that when executed will follow
   * the provided trajectory. The user should implement a velocity PID on the
   * desired output wheel velocities.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path - this is left to the user, since it is not
   * appropriate for paths with nonstationary end-states.
   *
   * @param trajectory       The trajectory to follow.
   * @param pose             A function that supplies the robot pose - use one
   * of the odometry classes to provide this.
   * @param kinematics       The kinematics for the robot drivetrain.
   * @param xController      The Trajectory Tracker PID controller
   *                         for the robot's x position.
   * @param yController      The Trajectory Tracker PID controller
   *                         for the robot's y position.
   * @param thetaController  The Trajectory Tracker PID controller
   *                         for angle for the robot.
   * @param desiredRotation  The angle that the robot should be facing.
   *                         This is sampled at each time step.
   * @param maxWheelVelocity The maximum velocity of a drivetrain wheel.
   * @param output           The output of the position PIDs.
   * @param requirements     The subsystems to require.
   */
  MecanumControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
      frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      std::function<frc::Rotation2d()> desiredRotation,
      units::meters_per_second_t maxWheelVelocity,
      std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                         units::meters_per_second_t,
                         units::meters_per_second_t)>
          output,
      Requirements requirements);

  /**
   * Constructs a new MecanumControllerCommand that when executed will follow
   * the provided trajectory. The user should implement a velocity PID on the
   * desired output wheel velocities.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path - this is left to the user, since it is not
   * appropriate for paths with nonstationary end-states.
   *
   * <p>Note 2: The final rotation of the robot will be set to the rotation of
   * the final pose in the trajectory. The robot will not follow the rotations
   * from the poses at each timestep. If alternate rotation behavior is desired,
   * the other constructor with a supplier for rotation should be used.
   *
   * @param trajectory       The trajectory to follow.
   * @param pose             A function that supplies the robot pose - use one
   * of the odometry classes to provide this.
   * @param kinematics       The kinematics for the robot drivetrain.
   * @param xController      The Trajectory Tracker PID controller
   *                         for the robot's x position.
   * @param yController      The Trajectory Tracker PID controller
   *                         for the robot's y position.
   * @param thetaController  The Trajectory Tracker PID controller
   *                         for angle for the robot.
   * @param maxWheelVelocity The maximum velocity of a drivetrain wheel.
   * @param output           The output of the position PIDs.
   * @param requirements     The subsystems to require.
   */
  MecanumControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::MecanumDriveKinematics kinematics, frc::PIDController xController,
      frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      units::meters_per_second_t maxWheelVelocity,
      std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                         units::meters_per_second_t,
                         units::meters_per_second_t)>
          output,
      Requirements requirements = {});

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::SimpleMotorFeedforward<units::meters> m_feedforward;
  frc::MecanumDriveKinematics m_kinematics;
  frc::HolonomicDriveController m_controller;
  std::function<frc::Rotation2d()> m_desiredRotation;
  const units::meters_per_second_t m_maxWheelVelocity;
  std::unique_ptr<frc::PIDController> m_frontLeftController;
  std::unique_ptr<frc::PIDController> m_rearLeftController;
  std::unique_ptr<frc::PIDController> m_frontRightController;
  std::unique_ptr<frc::PIDController> m_rearRightController;
  std::function<frc::MecanumDriveWheelSpeeds()> m_currentWheelSpeeds;
  std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                     units::meters_per_second_t, units::meters_per_second_t)>
      m_outputVel;
  std::function<void(units::volt_t, units::volt_t, units::volt_t,
                     units::volt_t)>
      m_outputVolts;

  bool m_usePID;
  frc::Timer m_timer;
  frc::MecanumDriveWheelSpeeds m_prevSpeeds;
  units::second_t m_prevTime;
};
}  // namespace frc2
