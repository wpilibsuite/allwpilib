// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/XboxController.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/estimator/KalmanFilter.hpp"
#include "wpi/math/system/LinearSystemLoop.hpp"
#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/velocity.hpp"

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control an elevator.
 */
class Robot : public wpi::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  static constexpr wpi::units::meter_t kRaisedPosition = 2_ft;
  static constexpr wpi::units::meter_t kLoweredPosition = 0_ft;

  static constexpr wpi::units::meter_t kDrumRadius = 0.75_in;
  static constexpr wpi::units::kilogram_t kCarriageMass = 4.5_kg;
  static constexpr double kGearRatio = 6.0;

  // The plant holds a state-space model of our elevator. This system has the
  // following properties:
  //
  // States: [position, velocity], in meters and meters per second.
  // Inputs (what we can "put in"): [voltage], in volts.
  // Outputs (what we can measure): [position], in meters.
  wpi::math::LinearSystem<2, 1, 1> m_elevatorPlant =
      wpi::math::LinearSystemId::ElevatorSystem(
          wpi::math::DCMotor::NEO(2), kCarriageMass, kDrumRadius, kGearRatio)
          .Slice(0);

  // The observer fuses our encoder data and voltage inputs to reject noise.
  wpi::math::KalmanFilter<2, 1, 1> m_observer{
      m_elevatorPlant,
      {wpi::units::meter_t{2_in}.value(),
       wpi::units::meters_per_second_t{40_in / 1_s}
           .value()},  // How accurate we think our model is
      {0.001},         // How accurate we think our encoder position
      // data is. In this case we very highly trust our encoder position
      // reading.
      20_ms};

  // A LQR uses feedback to create voltage commands.
  wpi::math::LinearQuadraticRegulator<2, 1> m_controller{
      m_elevatorPlant,
      // qelms. State error tolerance, in meters and meters per second.
      // Decrease this to more heavily penalize state excursion, or make the
      // controller behave more aggressively.
      {wpi::units::meter_t{1_in}.value(),
       wpi::units::meters_per_second_t{10_in / 1_s}.value()},
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
  wpi::math::LinearSystemLoop<2, 1, 1> m_loop{m_elevatorPlant, m_controller,
                                              m_observer, 12_V, 20_ms};

  // An encoder set up to measure elevator height in meters.
  wpi::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};

  wpi::PWMSparkMax m_motor{kMotorPort};
  wpi::XboxController m_joystick{kJoystickPort};

  wpi::math::TrapezoidProfile<wpi::units::meters> m_profile{{3_fps, 6_fps_sq}};

  wpi::math::TrapezoidProfile<wpi::units::meters>::State
      m_lastProfiledReference;

 public:
  Robot() {
    // Circumference = pi * d, so distance per click = pi * d / counts
    m_encoder.SetDistancePerPulse(2.0 * std::numbers::pi * kDrumRadius.value() /
                                  4096.0);
  }

  void TeleopInit() override {
    // Reset our loop to make sure it's in a known state.
    m_loop.Reset(
        wpi::math::Vectord<2>{m_encoder.GetDistance(), m_encoder.GetRate()});

    m_lastProfiledReference = {
        wpi::units::meter_t{m_encoder.GetDistance()},
        wpi::units::meters_per_second_t{m_encoder.GetRate()}};
  }

  void TeleopPeriodic() override {
    // Sets the target height of our elevator. This is similar to setting the
    // setpoint of a PID controller.
    wpi::math::TrapezoidProfile<wpi::units::meters>::State goal;
    if (m_joystick.GetRightBumperButton()) {
      // We pressed the bumper, so let's set our next reference
      goal = {kRaisedPosition, 0_fps};
    } else {
      // We released the bumper, so let's spin down
      goal = {kLoweredPosition, 0_fps};
    }
    m_lastProfiledReference =
        m_profile.Calculate(20_ms, m_lastProfiledReference, goal);

    m_loop.SetNextR(
        wpi::math::Vectord<2>{m_lastProfiledReference.position.value(),
                              m_lastProfiledReference.velocity.value()});

    // Correct our Kalman filter's state vector estimate with encoder data.
    m_loop.Correct(wpi::math::Vectord<1>{m_encoder.GetDistance()});

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
