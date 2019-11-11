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
#include "frc2/Timer.h"
#include "frc/controller/PIDController.h"
#include "frc/controller/ProfiledPIDController.h"
#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveWheelSpeeds.h"
#include "frc/trajectory/Trajectory.h"

#pragma once

namespace frc2 {
/**
 * A command that uses two PID controllers ({@link PIDController}) and a
 * ProfiledPIDController ({@link ProfiledPIDController}) to follow a trajectory
 * {@link Trajectory} with a mecanum drive.
 *
 * <p>The command handles trajectory-following,
 * Velocity PID calculations, and feedforwards internally. This
 * is intended to be a more-or-less "complete solution" that can be used by teams without a great
 * deal of controls expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to use the onboard
 * PID functionality of a "smart" motor controller) may use the secondary constructor that omits
 * the PID and feedforward functionality, returning only the raw wheel speeds from the PID
 * controllers.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the trajectory.
 */

class MecanumFollowerCommand
    : public CommandHelper<CommandBase, MecanumFollowerCommand> {
  using voltsecondspermeter =
      units::compound_unit<units::voltage::volt, units::second,
                           units::inverse<units::meter>>;
  using voltsecondssquaredpermeter =
      units::compound_unit<units::voltage::volt, units::squared<units::second>,
                           units::inverse<units::meter>>;

 public:
  /**
   * Constructs a new MecanumFollowerCommand that when executed will follow the provided trajectory.
   * PID control and feedforward are handled internally,
   * outputs are scaled from -12 to 12 as a voltage output to the motor.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path
   * this is left to the user, since it is not appropriate for paths with nonstationary endstates.
   *
   * <p>Note2: The rotation controller will calculate the rotation based on the final pose in the
   * trajectory, not the poses at each time step.
   *
   * @param trajectory                        The trajectory to follow.
   * @param pose                              A function that supplies the robot pose - use one of
   *                                          the odometry classes to provide this.
   * @param ksVolts                           Constant feedforward term for the robot drive.
   * @param kvVoltSecondsPerMeter             Velocity-proportional feedforward term for the robot
   *                                          drive.
   * @param kaVoltSecondsSquaredPerMeter      Acceleration-proportional feedforward term
   *                                          for the robot drive.
   * @param kinematics                        The kinematics for the robot drivetrain.
   * @param xController                      The Trajectory Tracker PID controller
   *                                          for the robot's x position.
   * @param yController                      The Trajectory Tracker PID controller
   *                                          for the robot's y position.
   * @param thetaController                   The Trajectory Tracker PID controller
   *                                          for angle for the robot.
   * @param maxWheelVelocityMetersPerSecond   The maximum velocity of a drivetrain wheel.
   * @param currentWheelSpeeds                A MecanumDriveWheelSpeeds object containing
   * @param frontLeftController               The front left wheel velocity PID.
   * @param rearLeftController                The rear left wheel velocity PID.
   * @param frontRightController              The front right wheel velocity PID.
   * @param rearRightController               The rear right wheel velocity PID.
   *                                          the current wheel speeds.
   * @param output                            The output of the velocity PIDs in volts.
   * @param requirements                      The subsystems to require.
   */

  MecanumFollowerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      units::voltage::volt_t ks, units::unit_t<voltsecondspermeter> kv,
      units::unit_t<voltsecondssquaredpermeter> ka,
      frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
      frc2::PIDController yController,
      frc::ProfiledPIDController thetaController,
      units::meters_per_second_t MaxWheelVelocityMetersPerSecond,
      std::function<frc::MecanumDriveWheelSpeeds()> currentWheelSpeeds,
      frc2::PIDController frontLeftController,
      frc2::PIDController rearLeftController,
      frc2::PIDController frontRightController,
      frc2::PIDController rearRightController,
      std::function<void(units::voltage::volt_t, units::voltage::volt_t,
                         units::voltage::volt_t, units::voltage::volt_t)>
          output,
      std::initializer_list<Subsystem*> requirements);

  /**
   * Constructs a new MecanumFollowerCommand that when executed will follow the provided trajectory.
   * The user should implement a velocity PID on the desired output wheel velocities.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path -
   * this
   * is left to the user, since it is not appropriate for paths with non-stationary end-states.
   *
   * <p>Note2: The rotation controller will calculate the rotation based on the final pose
   * in the trajectory, not the poses at each time step.
   *
   * @param trajectory                        The trajectory to follow.
   * @param pose                              A function that supplies the robot pose - use one of
   *                                          the odometry classes to provide this.
   * @param kinematics                        The kinematics for the robot drivetrain.
   * @param xController                      The Trajectory Tracker PID controller
   *                                          for the robot's x position.
   * @param yController                      The Trajectory Tracker PID controller
   *                                          for the robot's y position.
   * @param thetaController                   The Trajectory Tracker PID controller
   *                                          for angle for the robot.
   * @param maxWheelVelocityMetersPerSecond   The maximum velocity of a drivetrain wheel.
   * @param output                            The output of the velocity PID's in volts.
   * @param requirements                      The subsystems to require.
   */
  MecanumFollowerCommand(
      frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
      frc::MecanumDriveKinematics kinematics, frc2::PIDController xController,
      frc2::PIDController yController,
      frc::ProfiledPIDController thetaController,
      units::meters_per_second_t MaxWheelVelocityMetersPerSecond,
      std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                         units::meters_per_second_t,
                         units::meters_per_second_t)>
          output,
      std::initializer_list<Subsystem*> requirements);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::Timer m_timer;
  frc::MecanumDriveWheelSpeeds m_prevSpeeds;
  units::second_t m_prevTime;
  frc::Pose2d m_finalPose;
  bool m_usePID;

  frc::Trajectory m_trajectory;
  std::function<frc::Pose2d()> m_pose;
  const units::voltage::volt_t m_ks;
  const units::unit_t<voltsecondspermeter> m_kv;
  const units::unit_t<voltsecondssquaredpermeter> m_ka;
  frc::MecanumDriveKinematics m_kinematics;
  std::unique_ptr<frc2::PIDController> m_xController;
  std::unique_ptr<frc2::PIDController> m_yController;
  std::unique_ptr<frc::ProfiledPIDController> m_thetaController;
  const units::meters_per_second_t m_maxWheelVelocityMetersPerSecond;
  std::unique_ptr<frc2::PIDController> m_frontLeftController;
  std::unique_ptr<frc2::PIDController> m_rearLeftController;
  std::unique_ptr<frc2::PIDController> m_frontRightController;
  std::unique_ptr<frc2::PIDController> m_rearRightController;
  std::function<frc::MecanumDriveWheelSpeeds()> m_currentWheelSpeeds;
  std::function<void(units::meters_per_second_t, units::meters_per_second_t,
                     units::meters_per_second_t, units::meters_per_second_t)>
      m_outputVel;
  std::function<void(units::voltage::volt_t, units::voltage::volt_t,
                     units::voltage::volt_t, units::voltage::volt_t)>
      m_outputVolts;
};
}  // namespace frc2
