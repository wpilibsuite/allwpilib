// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/system/LinearSystem.h>
#include <frc/system/plant/DCMotor.h>

#include <Eigen/Core>
#include <units/length.h>
#include <units/moment_of_inertia.h>
#include <units/time.h>
#include <units/voltage.h>

namespace frc::sim {

class DifferentialDrivetrainSim {
 public:
  /**
   * Create a SimDrivetrain.
   *
   * @param drivetrainPlant   The LinearSystem representing the robot's
   * drivetrain. This system can be created with
   * LinearSystemId#createDrivetrainVelocitySystem or
   * LinearSystemId#identifyDrivetrainSystem.
   * @param trackWidth        The robot's track width.
   * @param driveMotor        A {@link DCMotor} representing the left side of
   * the drivetrain.
   * @param gearingRatio      The gearingRatio ratio of the left side, as output
   * over input. This must be the same ratio as the ratio used to identify or
   * create the drivetrainPlant.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain, in
   * meters.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]^T. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  DifferentialDrivetrainSim(
      LinearSystem<2, 2, 2> plant, units::meter_t trackWidth,
      DCMotor driveMotor, double gearingRatio, units::meter_t wheelRadius,
      const std::array<double, 7>& measurementStdDevs = {});

  /**
   * Create a SimDrivetrain.
   *
   * @param driveMotor  A {@link DCMotor} representing the left side of the
   * drivetrain.
   * @param gearing     The gearing on the drive between motor and wheel, as
   * output over input. This must be the same ratio as the ratio used to
   * identify or create the drivetrainPlant.
   * @param J           The moment of inertia of the drivetrain about its
   * center.
   * @param mass        The mass of the drivebase.
   * @param wheelRadius The radius of the wheels on the drivetrain.
   * @param trackWidth  The robot's track width, or distance between left and
   * right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]^T. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  DifferentialDrivetrainSim(
      frc::DCMotor driveMotor, double gearing, units::kilogram_square_meter_t J,
      units::kilogram_t mass, units::meter_t wheelRadius,
      units::meter_t trackWidth,
      const std::array<double, 7>& measurementStdDevs = {});

  /**
   * Clamp the input vector such that no element exceeds the given voltage. If
   * any does, the relative magnitudes of the input will be maintained.
   *
   * @param u          The input vector.
   * @param maxVoltage The maximum voltage.
   * @return The normalized input.
   */
  Eigen::Matrix<double, 2, 1> ClampInput(Eigen::Matrix<double, 2, 1> u);

  /**
   * Sets the applied voltage to the drivetrain. Note that positive voltage must
   * make that side of the drivetrain travel forward (+X).
   *
   * @param leftVoltage  The left voltage.
   * @param rightVoltage The right voltage.
   */
  void SetInputs(units::volt_t leftVoltage, units::volt_t rightVoltage);

  /**
   * Sets the gearing reduction on the drivetrain. This is commonly used for
   * shifting drivetrains.
   *
   * @param newGearing The new gear ratio, as output over input.
   */
  void SetGearing(double newGearing);

  /**
   * Updates the simulation.
   *
   * @param dt The time that's passed since the last {@link #update(double)}
   * call.
   */
  void Update(units::second_t dt);

  /**
   * Returns the current gearing reduction of the drivetrain, as output over
   * input.
   */
  double GetGearing() const;

  /**
   * Returns the direction the robot is pointing.
   *
   * Note that this angle is counterclockwise-positive, while most gyros are
   * clockwise positive.
   */
  Rotation2d GetHeading() const;

  /**
   * Returns the current pose.
   */
  Pose2d GetPose() const;

  /**
   * Get the right encoder position in meters.
   * @return The encoder position.
   */
  units::meter_t GetRightPosition() const {
    return units::meter_t{GetOutput(State::kRightPosition)};
  }

  /**
   * Get the right encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  units::meters_per_second_t GetRightVelocity() const {
    return units::meters_per_second_t{GetOutput(State::kRightVelocity)};
  }

  /**
   * Get the left encoder position in meters.
   * @return The encoder position.
   */
  units::meter_t GetLeftPosition() const {
    return units::meter_t{GetOutput(State::kLeftPosition)};
  }

  /**
   * Get the left encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  units::meters_per_second_t GetLeftVelocity() const {
    return units::meters_per_second_t{GetOutput(State::kLeftVelocity)};
  }

  /**
   * Returns the currently drawn current for the right side.
   */
  units::ampere_t GetRightCurrentDraw() const;

  /**
   * Returns the currently drawn current for the left side.
   */
  units::ampere_t GetLeftCurrentDraw() const;

  /**
   * Returns the currently drawn current.
   */
  units::ampere_t GetCurrentDraw() const;

  /**
   * Sets the system state.
   *
   * @param state The state.
   */
  void SetState(const Eigen::Matrix<double, 7, 1>& state);

  /**
   * Sets the system pose.
   *
   * @param pose The pose.
   */
  void SetPose(const frc::Pose2d& pose);

  Eigen::Matrix<double, 7, 1> Dynamics(const Eigen::Matrix<double, 7, 1>& x,
                                       const Eigen::Matrix<double, 2, 1>& u);

  class State {
   public:
    static constexpr int kX = 0;
    static constexpr int kY = 1;
    static constexpr int kHeading = 2;
    static constexpr int kLeftVelocity = 3;
    static constexpr int kRightVelocity = 4;
    static constexpr int kLeftPosition = 5;
    static constexpr int kRightPosition = 6;
  };

  /**
   * Represents a gearing option of the Toughbox mini.
   * 12.75:1 -- 14:50 and 14:50
   * 10.71:1 -- 14:50 and 16:48
   * 8.45:1 -- 14:50 and 19:45
   * 7.31:1 -- 14:50 and 21:43
   * 5.95:1 -- 14:50 and 24:40
   */
  class KitbotGearing {
   public:
    static constexpr double k12p75 = 12.75;
    static constexpr double k10p71 = 10.71;
    static constexpr double k8p45 = 8.45;
    static constexpr double k7p31 = 7.31;
    static constexpr double k5p95 = 5.95;
  };

  class KitbotMotor {
   public:
    static constexpr frc::DCMotor SingleCIMPerSide = frc::DCMotor::CIM(1);
    static constexpr frc::DCMotor DualCIMPerSide = frc::DCMotor::CIM(2);
    static constexpr frc::DCMotor SingleMiniCIMPerSide =
        frc::DCMotor::MiniCIM(1);
    static constexpr frc::DCMotor DualMiniCIMPerSide = frc::DCMotor::MiniCIM(2);
  };

  class KitbotWheelSize {
   public:
    static constexpr units::meter_t kSixInch = 6_in;
    static constexpr units::meter_t kEightInch = 8_in;
    static constexpr units::meter_t kTenInch = 10_in;
  };

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor     The motors installed in the bot.
   * @param gearing   The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]^T. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  static DifferentialDrivetrainSim CreateKitbotSim(
      frc::DCMotor motor, double gearing, units::meter_t wheelSize,
      const std::array<double, 7>& measurementStdDevs = {}) {
    // MOI estimation -- note that I = m r^2 for point masses
    units::kilogram_square_meter_t batteryMoi = 12.5_lb * 10_in * 10_in;
    units::kilogram_square_meter_t gearboxMoi = (2.8_lb + 2.0_lb) *
                                                2  // CIM plus toughbox per side
                                                * (26_in / 2) * (26_in / 2);

    return DifferentialDrivetrainSim{
        motor,           gearing, batteryMoi + gearboxMoi, 25_kg,
        wheelSize / 2.0, 26_in,   measurementStdDevs};
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor     The motors installed in the bot.
   * @param gearing   The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param J         The moment of inertia of the drivebase. This can be
   * calculated using frc-characterization.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]^T. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  static DifferentialDrivetrainSim CreateKitbotSim(
      frc::DCMotor motor, double gearing, units::meter_t wheelSize,
      units::kilogram_square_meter_t J,
      const std::array<double, 7>& measurementStdDevs = {}) {
    return DifferentialDrivetrainSim{
        motor, gearing, J, 25_kg, wheelSize / 2.0, 26_in, measurementStdDevs};
  }

 private:
  /**
   * Returns an element of the state vector.
   *
   * @param output The row of the output vector.
   */
  double GetOutput(int output) const;

  /**
   * Returns the current output vector y.
   */
  Eigen::Matrix<double, 7, 1> GetOutput() const;

  /**
   * Returns an element of the state vector. Note that this will not include
   * noise!
   *
   * @param output The row of the output vector.
   */
  double GetState(int state) const;

  /**
   * Returns the current state vector x. Note that this will not include noise!
   */
  Eigen::Matrix<double, 7, 1> GetState() const;

  LinearSystem<2, 2, 2> m_plant;
  units::meter_t m_rb;
  units::meter_t m_wheelRadius;

  DCMotor m_motor;

  double m_originalGearing;
  double m_currentGearing;

  Eigen::Matrix<double, 7, 1> m_x;
  Eigen::Matrix<double, 2, 1> m_u;
  Eigen::Matrix<double, 7, 1> m_y;
  std::array<double, 7> m_measurementStdDevs;
};
}  // namespace frc::sim
