/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

/**
 * Constructs a new SwerveFollowerCommand that, when executed, will follow the
 * provided trajectory. This command will not return output voltages but rather
 * raw module states from the position controllers which need to be put into a
 * velocty PID.
 *
 * <p>Note: The controllers will *not* set the outputVolts to zero upon
 * completion of the path- this is left to the user, since it is not appropriate
 * for paths with nonstationary endstates.
 *
 * <p>Note2: The rotation controller will calculate the rotation based on the
 * final pose in the trajectory, not the poses at each time step.
 *
 * @param trajectory                        The trajectory to follow.
 * @param pose                              A function that supplies the robot
 * pose - use one of the odometry classes to provide this.
 * @param kinematics                        The kinematics for the robot
 * drivetrain.
 * @param xController                       The Trajectory Tracker PID
 * controller for the robot's x position.
 * @param yController                       The Trajectory Tracker PID
 * controller for the robot's y position.
 * @param thetaController                   The Trajectory Tracker PID
 * controller for angle for the robot.
 * @param outputModuleStates                The raw output module states from
 * the psoiton contollers.
 * @param requirements                      The subsystems to require.
 */

namespace frc2 {

template <int NumModules>
SwerveFollowerCommand<NumModules>::SwerveFollowerCommand(
    frc::Trajectory trajectory, std::function<frc::Pose2d()> pose,
    frc::SwerveDriveKinematics<NumModules> kinematics,
    frc2::PIDController xController, frc2::PIDController yController,
    frc::ProfiledPIDController thetaController,
    std::function<void(std::array<frc::SwerveModuleState, NumModules>)> output,
    std::initializer_list<Subsystem*> requirements)
    : m_trajectory(trajectory),
    m_pose(pose),
    m_kinematics(kinematics),
    m_xController(std::make_unique<frc2::PIDController>(xController)),
    m_yController(std::make_unique<frc2::PIDController>(yController)),
    m_thetaController(
        std::make_unique<frc::ProfiledPIDController>(thetaController)),
    m_outputStates(output) {
this->AddRequirements(requirements);
}

template <int NumModules>
void SwerveFollowerCommand<NumModules>::Initialize() {
auto m_finalPose = m_trajectory.Sample(m_trajectory.TotalTime()).pose;

m_timer.Reset();
m_timer.Start();
}

template <int NumModules>
void SwerveFollowerCommand<NumModules>::Execute() {
auto curTime = units::second_t(m_timer.Get());

auto m_desiredState = m_trajectory.Sample(curTime);
auto m_desiredPose = m_desiredState.pose;

auto m_poseError = m_desiredPose.RelativeTo(m_pose());

auto targetXVel = units::velocity::meters_per_second_t(
    m_xController->Calculate((m_pose().Translation().X().to<double>()),
                            (m_desiredPose.Translation().X().to<double>())));
auto targetYVel = units::velocity::meters_per_second_t(
    m_yController->Calculate((m_pose().Translation().Y().to<double>()),
                            (m_desiredPose.Translation().Y().to<double>())));
auto targetAngularVel =
    units::angular_velocity::radians_per_second_t(m_thetaController->Calculate(
        units::meter_t(m_pose().Rotation().Radians().to<double>()), units::meter_t(m_finalPose.Rotation().Radians().to<double>())));  
        // Profiled PID Controller only takes
        // meters as setpoint and measurement
// The robot will go to the desired rotation of the final pose in the
// trajectory, not following the poses at individual states.

auto vRef = m_desiredState.velocity;

targetXVel += vRef * std::sin(m_poseError.Rotation().Radians().to<double>());
targetYVel += vRef * std::cos(m_poseError.Rotation().Radians().to<double>());

auto targetChassisSpeeds =
    frc::ChassisSpeeds{targetXVel, targetYVel, targetAngularVel};

auto targetModuleStates =
    m_kinematics.ToSwerveModuleStates(targetChassisSpeeds);

m_outputStates(targetModuleStates);
}

template <int NumModules>
void SwerveFollowerCommand<NumModules>::End(bool interrupted) {
m_timer.Stop();
}

template <int NumModules>
bool SwerveFollowerCommand<NumModules>::IsFinished() {
return m_timer.HasPeriodPassed(m_trajectory.TotalTime().to<double>());
}

}
