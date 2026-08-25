// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/LinearQuadraticRegulator.hpp"
#include "wpi/math/estimator/KalmanFilter.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystemLoop.hpp"
#include "wpi/math/system/Models.hpp"
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
  static constexpr int MOTOR_PORT = 0;
  static constexpr int ENCODER_A_CHANNEL = 0;
  static constexpr int ENCODER_B_CHANNEL = 1;
  static constexpr int JOYSTICK_PORT = 0;

  static constexpr wpi::units::meters<> RAISED_POSITION = 2_ft;
  static constexpr wpi::units::meters<> LOWERED_POSITION = 0_ft;

  static constexpr wpi::units::meters<> DRUM_RADIUS = 0.75_in;
  static constexpr wpi::units::kilograms<> CARRIAGE_MASS = 4.5_kg;
  static constexpr double GEAR_RATIO = 6.0;

  // The plant holds a state-space model of our elevator. This system has the
  // following properties:
  //
  // States: [position, velocity], in meters and meters per second.
  // Inputs (what we can "put in"): [voltage], in volts.
  // Outputs (what we can measure): [position], in meters.
  wpi::math::LinearSystem<2, 1, 1> elevatorPlant =
      wpi::math::Models::ElevatorFromPhysicalConstants(
          wpi::math::DCMotor::NEO(2), CARRIAGE_MASS, DRUM_RADIUS, GEAR_RATIO)
          .Slice(0);

  // The observer fuses our encoder data and voltage inputs to reject noise.
  wpi::math::KalmanFilter<2, 1, 1> observer{
      elevatorPlant,
      {wpi::units::meters<>{2_in}.value(),
       wpi::units::meters_per_second<>{40_in / 1_s}
           .value()},  // How accurate we think our model is
      {0.001},         // How accurate we think our encoder position
      // data is. In this case we very highly trust our encoder position
      // reading.
      20_ms};

  // A LQR uses feedback to create voltage commands.
  wpi::math::LinearQuadraticRegulator<2, 1> controller{
      elevatorPlant,
      // qelms. State error tolerance, in meters and meters per second.
      // Decrease this to more heavily penalize state excursion, or make the
      // controller behave more aggressively.
      {wpi::units::meters<>{1_in}.value(),
       wpi::units::meters_per_second<>{10_in / 1_s}.value()},
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
  wpi::math::LinearSystemLoop<2, 1, 1> loop{elevatorPlant, controller, observer,
                                            12_V, 20_ms};

  // An encoder set up to measure elevator height in meters.
  wpi::Encoder encoder{ENCODER_A_CHANNEL, ENCODER_B_CHANNEL};

  wpi::PWMSparkMax motor{MOTOR_PORT};
  wpi::Gamepad joystick{JOYSTICK_PORT};

  wpi::math::TrapezoidProfile<wpi::units::meters_> profile{{3_fps, 6_fps2}};

  wpi::math::TrapezoidProfile<wpi::units::meters_>::State lastProfiledReference;

 public:
  Robot() {
    // Circumference = pi * d, so distance per click = pi * d / counts
    encoder.SetDistancePerPulse(2.0 * std::numbers::pi * DRUM_RADIUS.value() /
                                4096.0);
  }

  void TeleopInit() override {
    // Reset our loop to make sure it's in a known state.
    loop.Reset(wpi::math::Vectord<2>{encoder.GetDistance(), encoder.GetRate()});

    lastProfiledReference = {
        wpi::units::meters<>{encoder.GetDistance()},
        wpi::units::meters_per_second<>{encoder.GetRate()}};
  }

  void TeleopPeriodic() override {
    // Sets the target height of our elevator. This is similar to setting the
    // setpoint of a PID controller.
    wpi::math::TrapezoidProfile<wpi::units::meters_>::State goal;
    if (joystick.GetRightBumperButton()) {
      // We pressed the bumper, so let's set our next reference
      goal = {RAISED_POSITION, 0_fps};
    } else {
      // We released the bumper, so let's spin down
      goal = {LOWERED_POSITION, 0_fps};
    }
    lastProfiledReference =
        profile.Calculate(20_ms, lastProfiledReference, goal);

    loop.SetNextR(
        wpi::math::Vectord<2>{lastProfiledReference.position.value(),
                              lastProfiledReference.velocity.value()});

    // Correct our Kalman filter's state vector estimate with encoder data.
    loop.Correct(wpi::math::Vectord<1>{encoder.GetDistance()});

    // Update our LQR to generate new voltage commands and use the voltages to
    // predict the next state with out Kalman filter.
    loop.Predict(20_ms);

    // Send the new calculated voltage to the motors.
    // voltage = duty cycle * battery voltage, so
    // duty cycle = voltage / battery voltage
    motor.SetVoltage(wpi::units::volts<>{loop.U(0)});
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
