// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include "Drivetrain.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/controller/LTVUnicycleController.hpp"
#include "wpi/math/filter/SlewRateLimiter.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/dimensionless.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot()
      // Replace this with a call to a trajectory generator
      : trajectory{wpi::math::HolonomicTrajectory{
            std::vector<wpi::math::HolonomicSample>{wpi::math::HolonomicSample{
                0_s, wpi::math::Pose2d{0_m, 0_m, 0_rad},
                wpi::math::ChassisVelocities{0_mps, 0_mps, 0_rad_per_s},
                wpi::math::ChassisAccelerations{0_mps_sq, 0_mps_sq,
                                                0_rad_per_s_sq}}}}} {}

  void RobotPeriodic() override { drive.Periodic(); }

  void AutonomousInit() override {
    timer.Restart();
    drive.ResetOdometry(trajectory.InitialPose());
  }

  void AutonomousPeriodic() override {
    auto elapsed = timer.Get();
    auto reference = trajectory.SampleAt(elapsed);
    auto velocities = feedback.Calculate(drive.GetPose(), reference);
    drive.Drive(velocities.vx, velocities.omega);
  }

  void TeleopPeriodic() override {
    // Get the x velocity. We are inverting this because Xbox controllers return
    // negative values when we push forward.
    const auto xVelocity = -velocityLimiter.Calculate(controller.GetLeftY()) *
                           Drivetrain::MAX_VELOCITY;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Xbox controllers return positive values when you pull to
    // the right by default.
    auto rot = -rotLimiter.Calculate(controller.GetRightX()) *
               Drivetrain::MAX_ANGULAR_VELOCITY;

    drive.Drive(xVelocity, rot);
  }

  void SimulationPeriodic() override { drive.SimulationPeriodic(); }

 private:
  wpi::Gamepad controller{0};

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  wpi::math::SlewRateLimiter<wpi::units::scalar> velocityLimiter{3 / 1_s};
  wpi::math::SlewRateLimiter<wpi::units::scalar> rotLimiter{3 / 1_s};

  Drivetrain drive;
  wpi::math::HolonomicTrajectory trajectory;
  wpi::math::LTVUnicycleController feedback{20_ms};
  wpi::Timer timer;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
