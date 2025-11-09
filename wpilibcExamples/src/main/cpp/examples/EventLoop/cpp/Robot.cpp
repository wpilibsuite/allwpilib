// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/event/EventLoop.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"

static const auto SHOT_VELOCITY = 200_rpm;
static const auto TOLERANCE = 8_rpm;

class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    m_controller.SetTolerance(TOLERANCE.value());

    wpi::BooleanEvent isBallAtKicker{&m_loop, [] {
                                       return false;
                                       //    return kickerSensor.GetRange() <
                                       //           KICKER_THRESHOLD;
                                     }};
    wpi::BooleanEvent intakeButton{
        &m_loop, [&joystick = m_joystick] { return joystick.GetRawButton(2); }};

    // if the thumb button is held
    (intakeButton
     // and there is not a ball at the kicker
     && !isBallAtKicker)
        // activate the intake
        .IfHigh([&intake = m_intake] { intake.Set(0.5); });

    // if the thumb button is not held
    (!intakeButton
     // or there is a ball in the kicker
     || isBallAtKicker)
        // stop the intake
        .IfHigh([&intake = m_intake] { intake.Set(0.0); });

    wpi::BooleanEvent shootTrigger{
        &m_loop, [&joystick = m_joystick] { return joystick.GetTrigger(); }};

    // if the trigger is held
    shootTrigger
        // accelerate the shooter wheel
        .IfHigh([&shooter = m_shooter, &controller = m_controller, &ff = m_ff,
                 &encoder = m_shooterEncoder] {
          shooter.SetVoltage(
              wpi::units::volt_t{controller.Calculate(encoder.GetRate(),
                                                      SHOT_VELOCITY.value())} +
              ff.Calculate(wpi::units::radians_per_second_t{SHOT_VELOCITY}));
        });
    // if not, stop
    (!shootTrigger).IfHigh([&shooter = m_shooter] { shooter.Set(0.0); });

    wpi::BooleanEvent atTargetVelocity =
        wpi::BooleanEvent(
            &m_loop,
            [&controller = m_controller] { return controller.AtSetpoint(); })
            // debounce for more stability
            .Debounce(0.2_s);

    // if we're at the target velocity, kick the ball into the shooter wheel
    atTargetVelocity.IfHigh([&kicker = m_kicker] { kicker.Set(0.7); });

    // when we stop being at the target velocity, it means the ball was shot
    atTargetVelocity
        .Falling()
        // so stop the kicker
        .IfHigh([&kicker = m_kicker] { kicker.Set(0.0); });
  }

  void RobotPeriodic() override { m_loop.Poll(); }

 private:
  wpi::PWMSparkMax m_shooter{0};
  wpi::Encoder m_shooterEncoder{0, 1};
  wpi::math::PIDController m_controller{0.3, 0, 0};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> m_ff{0.1_V,
                                                              0.065_V / 1_rpm};

  wpi::PWMSparkMax m_kicker{1};

  wpi::PWMSparkMax m_intake{3};

  wpi::EventLoop m_loop{};
  wpi::Joystick m_joystick{0};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
