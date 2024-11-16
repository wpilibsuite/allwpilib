// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <memory>
#include <utility>

#include <frc/Timer.h>
#include <frc/controller/HolonomicDriveController.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/ChassisSpeeds.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <frc/trajectory/Trajectory.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/Requirements.h"

#pragma once

namespace frc2 {

/**
 * A command that uses two PID controllers (PIDController) and a profiled PID
 * controller (ProfiledPIDController) to follow a trajectory (Trajectory) with a
 * swerve drive.
 *
 * <p>The command handles trajectory-following, Velocity PID calculations, and
 * feedforwards internally. This is intended to be a more-or-less "complete
 * solution" that can be used by teams without a great deal of controls
 * expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to
 * use the onboard PID functionality of a "smart" motor controller) may use the
 * secondary constructor that omits the PID and feedforward functionality,
 * returning only the raw module states from the position PID controllers.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the
 * trajectory.
 *
 * This class is provided by the NewCommands VendorDep
 */
template <size_t NumModules>
class SwerveControllerCommand
    : public CommandHelper<Command, SwerveControllerCommand<NumModules>> {
  using voltsecondspermeter =
      units::compound_unit<units::voltage::volt, units::second,
                           units::inverse<units::meter>>;
  using voltsecondssquaredpermeter =
      units::compound_unit<units::voltage::volt, units::squared<units::second>,
                           units::inverse<units::meter>>;

 public:
  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the
   * provided trajectory. This command will not return output voltages but
   * rather raw module states from the position controllers which need to be put
   * into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path- this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose,
   *                        provided by the odometry class.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param xController     The Trajectory Tracker PID controller
   *                        for the robot's x position.
   * @param yController     The Trajectory Tracker PID controller
   *                        for the robot's y position.
   * @param thetaController The Trajectory Tracker PID controller
   *                        for angle for the robot.
   * @param desiredRotation The angle that the drivetrain should be
   *                        facing. This is sampled at each time step.
   * @param output          The raw output module states from the
   *                        position controllers.
   * @param requirements    The subsystems to require.
   */
  SwerveControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SwerveDriveKinematics<NumModules> kinematics,
      frc::PIDController xController, frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      std::function<frc::Rotation2d()> desiredRotation,
      std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
          output,
      Requirements requirements = {})
      : m_trajectory(std::move(trajectory)),
        m_pose(std::move(pose)),
        m_kinematics(kinematics),
        m_controller(xController, yController, thetaController),
        m_desiredRotation(std::move(desiredRotation)),
        m_outputStates(output) {
    this->AddRequirements(requirements);
  }

  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the
   * provided trajectory. This command will not return output voltages but
   * rather raw module states from the position controllers which need to be put
   * into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path- this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * <p>Note 2: The final rotation of the robot will be set to the rotation of
   * the final pose in the trajectory. The robot will not follow the rotations
   * from the poses at each timestep. If alternate rotation behavior is desired,
   * the other constructor with a supplier for rotation should be used.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose,
   *                        provided by the odometry class.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param xController     The Trajectory Tracker PID controller
   *                        for the robot's x position.
   * @param yController     The Trajectory Tracker PID controller
   *                        for the robot's y position.
   * @param thetaController The Trajectory Tracker PID controller
   *                        for angle for the robot.
   * @param output          The raw output module states from the
   *                        position controllers.
   * @param requirements    The subsystems to require.
   */
  SwerveControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SwerveDriveKinematics<NumModules> kinematics,
      frc::PIDController xController, frc::PIDController yController,
      frc::ProfiledPIDController<units::radians> thetaController,
      std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
          output,
      Requirements requirements = {})
      : m_trajectory(std::move(trajectory)),
        m_pose(std::move(pose)),
        m_kinematics(kinematics),
        m_controller(xController, yController, thetaController),
        m_outputStates(output) {
    this->AddRequirements(requirements);
  }

  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the
   * provided trajectory. This command will not return output voltages but
   * rather raw module states from the position controllers which need to be put
   * into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path- this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose,
   *                        provided by the odometry class.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param controller      The HolonomicDriveController for the drivetrain.
   * @param desiredRotation The angle that the drivetrain should be
   *                        facing. This is sampled at each time step.
   * @param output          The raw output module states from the
   *                        position controllers.
   * @param requirements    The subsystems to require.
   */
  SwerveControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SwerveDriveKinematics<NumModules> kinematics,
      frc::HolonomicDriveController controller,
      std::function<frc::Rotation2d()> desiredRotation,
      std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
          output,
      Requirements requirements = {})
      : m_trajectory(std::move(trajectory)),
        m_pose(std::move(pose)),
        m_kinematics(kinematics),
        m_controller(std::move(controller)),
        m_desiredRotation(std::move(desiredRotation)),
        m_outputStates(output) {
    this->AddRequirements(requirements);
  }

  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the
   * provided trajectory. This command will not return output voltages but
   * rather raw module states from the position controllers which need to be put
   * into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon
   * completion of the path- this is left to the user, since it is not
   * appropriate for paths with nonstationary endstates.
   *
   * <p>Note 2: The final rotation of the robot will be set to the rotation of
   * the final pose in the trajectory. The robot will not follow the rotations
   * from the poses at each timestep. If alternate rotation behavior is desired,
   * the other constructor with a supplier for rotation should be used.
   *
   * @param trajectory      The trajectory to follow.
   * @param pose            A function that supplies the robot pose,
   *                        provided by the odometry class.
   * @param kinematics      The kinematics for the robot drivetrain.
   * @param controller      The HolonomicDriveController for the drivetrain.
   * @param output          The raw output module states from the
   *                        position controllers.
   * @param requirements    The subsystems to require.
   */
  SwerveControllerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SwerveDriveKinematics<NumModules> kinematics,
      frc::HolonomicDriveController controller,
      std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
          output,
      Requirements requirements = {})
      : m_trajectory(std::move(trajectory)),
        m_pose(std::move(pose)),
        m_kinematics(kinematics),
        m_controller(std::move(controller)),
        m_outputStates(output) {
    this->AddRequirements(requirements);
  }

  void Initialize() override {
    if (m_desiredRotation == nullptr) {
      m_desiredRotation = [&] {
        return m_trajectory.States().back().pose.Rotation();
      };
    }
    m_timer.Restart();
  }

  void Execute() override {
    auto curTime = m_timer.Get();
    auto m_desiredState = m_trajectory.Sample(curTime);

    auto targetChassisSpeeds =
        m_controller.Calculate(m_pose(), m_desiredState, m_desiredRotation());
    auto targetModuleStates =
        m_kinematics.ToSwerveModuleStates(targetChassisSpeeds);

    m_outputStates(targetModuleStates);
  }

  void End(bool interrupted) override { m_timer.Stop(); }

  bool IsFinished() override {
    return m_timer.HasElapsed(m_trajectory.TotalTime());
  }

 private:
  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::SwerveDriveKinematics<NumModules> m_kinematics;
  frc::HolonomicDriveController m_controller;
  std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
      m_outputStates;

  std::function<frc::Rotation2d()> m_desiredRotation;

  frc::Timer m_timer;
  units::second_t m_prevTime;
  frc::Rotation2d m_finalRotation;
};

}  // namespace frc2
