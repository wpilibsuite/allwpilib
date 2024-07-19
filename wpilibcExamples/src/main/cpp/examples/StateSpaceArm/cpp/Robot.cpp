// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <frc/Encoder.h>
#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <frc/controller/LinearQuadraticRegulator.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/estimator/KalmanFilter.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc/system/LinearSystemLoop.h>
#include <frc/system/plant/DCMotor.h>
#include <frc/system/plant/LinearSystemId.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <units/angle.h>
#include <units/moment_of_inertia.h>

/**
 * This is a sample program to demonstrate how to use a state-space controller
 * to control an arm.
 */
class Robot : public frc::TimedRobot {
  static constexpr int kMotorPort = 0;
  static constexpr int kEncoderAChannel = 0;
  static constexpr int kEncoderBChannel = 1;
  static constexpr int kJoystickPort = 0;

  static constexpr units::radian_t kRaisedPosition = 90_deg;
  static constexpr units::radian_t kLoweredPosition = 0_deg;

  // Moment of inertia of the arm. Can be estimated with CAD. If finding this
  // constant is difficult, LinearSystem.identifyPositionSystem may be better.
  static constexpr units::kilogram_square_meter_t kArmMOI = 1.2_kg_sq_m;

  // Reduction between motors and encoder, as output over input. If the arm
  // spins slower than the motors, this number should be greater than one.
  static constexpr double kArmGearing = 10.0;

  // The plant holds a state-space model of our arm. This system has the
  // following properties:
  //
  // States: [position, velocity], in radians and radians per second.
  // Inputs (what we can "put in"): [voltage], in volts.
  // Outputs (what we can measure): [position], in radians.
  frc::LinearSystem<2, 1, 1> m_armPlant =
      frc::LinearSystemId::SingleJointedArmSystem(frc::DCMotor::NEO(2), kArmMOI,
                                                  kArmGearing)
          .Slice(0);

  // The observer fuses our encoder data and voltage inputs to reject noise.
  frc::KalmanFilter<2, 1, 1> m_observer{
      m_armPlant,
      {0.015, 0.17},  // How accurate we think our model is
      {0.01},         // How accurate we think our encoder position
      // data is. In this case we very highly trust our encoder position
      // reading.
      20_ms};

  // A LQR uses feedback to create voltage commands.
  frc::LinearQuadraticRegulator<2, 1> m_controller{
      m_armPlant,
      // qelms. Velocity error tolerance, in radians and radians per second.
      // Decrease this to more heavily penalize state excursion, or make the
      // controller behave more aggressively.
      {1.0 * 2.0 * std::numbers::pi / 360.0,
       10.0 * 2.0 * std::numbers::pi / 360.0},
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
  frc::LinearSystemLoop<2, 1, 1> m_loop{m_armPlant, m_controller, m_observer,
                                        12_V, 20_ms};

  // An encoder set up to measure arm position in radians per second.
  frc::Encoder m_encoder{kEncoderAChannel, kEncoderBChannel};

  frc::PWMSparkMax m_motor{kMotorPort};
  frc::XboxController m_joystick{kJoystickPort};

  frc::TrapezoidProfile<units::radians> m_profile{
      {45_deg_per_s, 90_deg_per_s / 1_s}};

  frc::TrapezoidProfile<units::radians>::State m_lastProfiledReference;

 public:
  Robot() {
    // We go 2 pi radians per 4096 clicks.
    m_encoder.SetDistancePerPulse(2.0 * std::numbers::pi / 4096.0);
  }

  void TeleopInit() override {
    m_loop.Reset(frc::Vectord<2>{m_encoder.GetDistance(), m_encoder.GetRate()});

    m_lastProfiledReference = {
        units::radian_t{m_encoder.GetDistance()},
        units::radians_per_second_t{m_encoder.GetRate()}};
  }

  void TeleopPeriodic() override {
    // Sets the target position of our arm. This is similar to setting the
    // setpoint of a PID controller.
    frc::TrapezoidProfile<units::radians>::State goal;
    if (m_joystick.GetRightBumperButton()) {
      // We pressed the bumper, so let's set our next reference
      goal = {kRaisedPosition, 0_rad_per_s};
    } else {
      // We released the bumper, so let's spin down
      goal = {kLoweredPosition, 0_rad_per_s};
    }
    m_lastProfiledReference =
        m_profile.Calculate(20_ms, m_lastProfiledReference, goal);

    m_loop.SetNextR(frc::Vectord<2>{m_lastProfiledReference.position.value(),
                                    m_lastProfiledReference.velocity.value()});

    // Correct our Kalman filter's state vector estimate with encoder data.
    m_loop.Correct(frc::Vectord<1>{m_encoder.GetDistance()});

    // Update our LQR to generate new voltage commands and use the voltages to
    // predict the next state with out Kalman filter.
    m_loop.Predict(20_ms);

    // Send the new calculated voltage to the motors.
    // voltage = duty cycle * battery voltage, so
    // duty cycle = voltage / battery voltage
    m_motor.SetVoltage(units::volt_t{m_loop.U(0)});
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
