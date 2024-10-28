// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Encoder.h>
#include <frc/Joystick.h>
#include <frc/TimedRobot.h>
#include <frc/Ultrasonic.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/event/BooleanEvent.h>
#include <frc/event/EventLoop.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

static const auto SHOT_VELOCITY = 200_rpm;
static const auto TOLERANCE = 8_rpm;
static const auto KICKER_THRESHOLD = 15_mm;

class Robot : public frc::TimedRobot {
 public:
  Robot() {
    m_controller.SetTolerance(TOLERANCE.value());

    frc::BooleanEvent isBallAtKicker{&m_loop, [&kickerSensor = m_kickerSensor] {
                                       return kickerSensor.GetRange() <
                                              KICKER_THRESHOLD;
                                     }};
    frc::BooleanEvent intakeButton{
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

    frc::BooleanEvent shootTrigger{
        &m_loop, [&joystick = m_joystick] { return joystick.GetTrigger(); }};

    // if the trigger is held
    shootTrigger
        // accelerate the shooter wheel
        .IfHigh([&shooter = m_shooter, &controller = m_controller, &ff = m_ff,
                 &encoder = m_shooterEncoder] {
          shooter.SetVoltage(
              units::volt_t{controller.Calculate(encoder.GetRate(),
                                                 SHOT_VELOCITY.value())} +
              ff.Calculate(units::radians_per_second_t{SHOT_VELOCITY}));
        });
    // if not, stop
    (!shootTrigger).IfHigh([&shooter = m_shooter] { shooter.Set(0.0); });

    frc::BooleanEvent atTargetVelocity =
        frc::BooleanEvent(
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
  frc::PWMSparkMax m_shooter{0};
  frc::Encoder m_shooterEncoder{0, 1};
  frc::PIDController m_controller{0.3, 0, 0};
  frc::SimpleMotorFeedforward<units::radians> m_ff{0.1_V, 0.065_V / 1_rpm};

  frc::PWMSparkMax m_kicker{1};
  frc::Ultrasonic m_kickerSensor{2, 3};

  frc::PWMSparkMax m_intake{3};

  frc::EventLoop m_loop{};
  frc::Joystick m_joystick{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
