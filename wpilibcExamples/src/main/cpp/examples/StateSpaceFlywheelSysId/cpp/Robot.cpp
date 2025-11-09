// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/XboxController.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/estimator/KalmanFilter.hpp"
#include "wpi/math/system/LinearSystemLoop.hpp"
#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control a flywheel.
 */
class Robot : public wpi::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;
  static constexpr wpi::units::radians_per_second_t kSpinup = 500_rpm;

  // Volts per (radian per second)
  static constexpr auto kFlywheelKv = 0.023_V / 1_rad_per_s;

  // Volts per (radian per second squared)
  static constexpr auto kFlywheelKa = 0.001_V / 1_rad_per_s_sq;

  // The plant holds a state-space model of our flywheel. This system has the
  // following properties:
  //
  // States: [velocity], in radians per second.
  // Inputs (what we can "put in"): [voltage], in volts.
  // Outputs (what we can measure): [velocity], in radians per second.
  //
  // The Kv and Ka constants are found using the FRC Characterization toolsuite.
  wpi::math::LinearSystem<1, 1, 1> m_flywheelPlant =
      wpi::math::LinearSystemId::IdentifyVelocitySystem<wpi::units::radian>(
          kFlywheelKv, kFlywheelKa);

  // The observer fuses our encoder data and voltage inputs to reject noise.
  wpi::math::KalmanFilter<1, 1, 1> m_observer{
      m_flywheelPlant,
      {3.0},   // How accurate we think our model is
      {0.01},  // How accurate we think our encoder data is
      20_ms};

  // A LQR uses feedback to create voltage commands.
  wpi::math::LinearQuadraticRegulator<1, 1> m_controller{
      m_flywheelPlant,
      // qelms. Velocity error tolerance, in radians per second. Decrease this
      // to more heavily penalize state excursion, or make the controller behave
      // more aggressively.
      {8.0},
      // relms. Control effort (voltage) tolerance. Decrease this to more
      // heavily penalize control effort, or make the controller less
      // aggressive. 12 is a good starting point because that is the
      // (approximate) maximum voltage of a battery.
      {12.0},
      // Nominal time between loops. 20ms for TimedRobot, but can be lower if
      // using notifiers.
      20_ms};

  // The state-space loop combines a controller, observer, feedforward and plant
  // for easy control.
  wpi::math::LinearSystemLoop<1, 1, 1> m_loop{m_flywheelPlant, m_controller,
                                              m_observer, 12_V, 20_ms};

  // An encoder set up to measure flywheel velocity in radians per second.
  wpi::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};

  wpi::PWMSparkMax m_motor{kMotorPort};
  wpi::XboxController m_joystick{kJoystickPort};

 public:
  Robot() {
    // We go 2 pi radians per 4096 clicks.
    m_encoder.SetDistancePerPulse(2.0 * std::numbers::pi / 4096.0);
  }

  void TeleopInit() override {
    m_loop.Reset(wpi::math::Vectord<1>{m_encoder.GetRate()});
  }

  void TeleopPeriodic() override {
    // Sets the target speed of our flywheel. This is similar to setting the
    // setpoint of a PID controller.
    if (m_joystick.GetRightBumperButton()) {
      // We pressed the bumper, so let's set our next reference
      m_loop.SetNextR(wpi::math::Vectord<1>{kSpinup.value()});
    } else {
      // We released the bumper, so let's spin down
      m_loop.SetNextR(wpi::math::Vectord<1>{0.0});
    }

    // Correct our Kalman filter's state vector estimate with encoder data.
    m_loop.Correct(wpi::math::Vectord<1>{m_encoder.GetRate()});

    // Update our LQR to generate new voltage commands and use the voltages to
    // predict the next state with out Kalman filter.
    m_loop.Predict(20_ms);

    // Send the new calculated voltage to the motors.
    // voltage = duty cycle * battery voltage, so
    // duty cycle = voltage / battery voltage
    m_motor.SetVoltage(wpi::units::volt_t{m_loop.U(0)});
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
