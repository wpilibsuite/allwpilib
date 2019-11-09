/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>
#include <functional>
#include <memory>

#include <units/units.h>

#include "CommandBase.h"
#include "CommandHelper.h"
#include "frc/Timer.h"
#include "frc/controller/PIDController.h"
#include "frc/controller/ProfiledPIDController.h"
#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveModuleState.h"
#include "frc/trajectory/Trajectory.h"

#pragma once

namespace frc2 {

/**
 * A command that uses two PID controllers ({@link PIDController}) and a
 * ProfiledPIDController ({@link ProfiledPIDController}) to follow a trajectory
 * {@link Trajectory} with a swerve drive.
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
 */

template <int NumModules>
class SwerveFollowerCommand
    : public CommandHelper<CommandBase, SwerveFollowerCommand<NumModules>> {
  using voltsecondspermeter =
      units::compound_unit<units::voltage::volt, units::second,
                           units::inverse<units::meter>>;
  using voltsecondssquaredpermeter =
      units::compound_unit<units::voltage::volt, units::squared<units::second>,
                           units::inverse<units::meter>>;

 public:

/**
 * Constructs a new SwerveFollowerCommand that when executed will follow the provided trajectory.
 * This command will not return output voltages but rather raw module states from
 * the position controllers which need to be put into a velocity PID.
 *
 * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path-
 * this is left to the user, since it is not appropriate for paths with nonstationary endstates.
 *
 * <p>Note2: The rotation controller will calculate the rotation based on the final pose
 * in the trajectory, not the poses at each time step.
 *
 * @param trajectory                        The trajectory to follow.
 * @param pose                              A function that supplies the robot pose - use one of
 *                                          the odometry classes to provide this.
 * @param kinematics                        The kinematics for the robot drivetrain.
 * @param xdController                      The Trajectory Tracker PID controller
 *                                          for the robot's x position.
 * @param ydController                      The Trajectory Tracker PID controller
 *                                          for the robot's y position.
 * @param thetaController                   The Trajectory Tracker PID controller
 *                                          for angle for the robot.
 * @param outputModuleStates                The raw output module states from the
 *                                          position controllers.
 * @param requirements                      The subsystems to require.
 */
  SwerveFollowerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::SwerveDriveKinematics<NumModules> kinematics,
      frc2::PIDController xController, frc2::PIDController yController,
      frc::ProfiledPIDController thetaController,
      std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
          output,
      std::initializer_list<Subsystem*> requirements);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Timer m_timer;
  units::second_t m_prevTime;
  frc::Pose2d m_finalPose;

  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  frc::SwerveDriveKinematics<NumModules> m_kinematics;
  std::unique_ptr<frc2::PIDController> m_xController;
  std::unique_ptr<frc2::PIDController> m_yController;
  std::unique_ptr<frc::ProfiledPIDController> m_thetaController;
  std::function<void(std::array<frc::SwerveModuleState, NumModules>)>
      m_outputStates;
};
}  // namespace frc2

#include "SwerveFollowerCommand.inc"
