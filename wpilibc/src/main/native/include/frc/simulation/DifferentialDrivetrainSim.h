// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/EigenCore.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/system/LinearSystem.h>
#include <frc/system/plant/DCMotor.h>

#include <units/length.h>
#include <units/moment_of_inertia.h>
#include <units/time.h>
#include <units/voltage.h>

namespace frc::sim {

class DifferentialDrivetrainSim {
 public:
  /**
   * Creates a simulated differential drivetrain.
   *
   * @param plant The LinearSystem representing the robot's drivetrain. This
   *              system can be created with
   *              LinearSystemId::DrivetrainVelocitySystem() or
   *              LinearSystemId::IdentifyDrivetrainSystem().
   * @param trackwidth   The robot's trackwidth.
   * @param driveMotor   A DCMotor representing the left side of the drivetrain.
   * @param gearingRatio The gearingRatio ratio of the left side, as output over
   *                     input. This must be the same ratio as the ratio used to
   *                     identify or create the plant.
   * @param wheelRadius  The radius of the wheels on the drivetrain, in meters.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   *                           [x, y, heading, left velocity, right velocity,
   *                           left distance, right distance]ᵀ. Can be omitted
   *                           if no noise is desired. Gyro standard deviations
   *                           of 0.0001 radians, velocity standard deviations
   *                           of 0.05 m/s, and position measurement standard
   *                           deviations of 0.005 meters are a reasonable
   *                           starting point.
   */
  DifferentialDrivetrainSim(
      LinearSystem<2, 2, 2> plant, units::meter_t trackwidth,
      DCMotor driveMotor, double gearingRatio, units::meter_t wheelRadius,
      const std::array<double, 7>& measurementStdDevs = {});

  /**
   * Creates a simulated differential drivetrain.
   *
   * @param driveMotor  A DCMotor representing the left side of the drivetrain.
   * @param gearing     The gearing on the drive between motor and wheel, as
   *                    output over input. This must be the same ratio as the
   *                    ratio used to identify or create the plant.
   * @param J           The moment of inertia of the drivetrain about its
   *                    center.
   * @param mass        The mass of the drivebase.
   * @param wheelRadius The radius of the wheels on the drivetrain.
   * @param trackwidth  The robot's trackwidth, or distance between left and
   *                    right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   *                           [x, y, heading, left velocity, right velocity,
   *                           left distance, right distance]ᵀ. Can be omitted
   *                           if no noise is desired. Gyro standard deviations
   *                           of 0.0001 radians, velocity standard deviations
   *                           of 0.05 m/s, and position measurement standard
   *                           deviations of 0.005 meters are a reasonable
   *                           starting point.
   */
  DifferentialDrivetrainSim(
      frc::DCMotor driveMotor, double gearing, units::kilogram_square_meter_t J,
      units::kilogram_t mass, units::meter_t wheelRadius,
      units::meter_t trackwidth,
      const std::array<double, 7>& measurementStdDevs = {});

  /**
   * Clamp the input vector such that no element exceeds the battery voltage.
   * If any does, the relative magnitudes of the input will be maintained.
   *
   * @param u The input vector.
   * @return The normalized input.
   */
  Eigen::Vector2d ClampInput(const Eigen::Vector2d& u);

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
   * @param dt The time that's passed since the last Update(units::second_t)
   *           call.
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
    return units::meter_t{GetOutput(State::RIGHT_POSITION)};
  }

  /**
   * Get the right encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  units::meters_per_second_t GetRightVelocity() const {
    return units::meters_per_second_t{GetOutput(State::RIGHT_VELOCITY)};
  }

  /**
   * Get the left encoder position in meters.
   * @return The encoder position.
   */
  units::meter_t GetLeftPosition() const {
    return units::meter_t{GetOutput(State::LEFT_POSITION)};
  }

  /**
   * Get the left encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  units::meters_per_second_t GetLeftVelocity() const {
    return units::meters_per_second_t{GetOutput(State::LEFT_VELOCITY)};
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
  void SetState(const Vectord<7>& state);

  /**
   * Sets the system pose.
   *
   * @param pose The pose.
   */
  void SetPose(const frc::Pose2d& pose);

  /**
   * The differential drive dynamics function.
   *
   * @param x The state.
   * @param u The input.
   * @return The state derivative with respect to time.
   */
  Vectord<7> Dynamics(const Vectord<7>& x, const Eigen::Vector2d& u);

  class State {
   public:
    static constexpr int X = 0;
    static constexpr int Y = 1;
    static constexpr int HEADING = 2;
    static constexpr int LEFT_VELOCITY = 3;
    static constexpr int RIGHT_VELOCITY = 4;
    static constexpr int LEFT_POSITION = 5;
    static constexpr int RIGHT_POSITION = 6;
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
    /// Gear ratio of 12.75:1.
    static constexpr double RATIO_12_75 = 12.75;
    /// Gear ratio of 10.71:1.
    static constexpr double RATIO_10_71 = 10.71;
    /// Gear ratio of 8.45:1.
    static constexpr double RATIO_8_45 = 8.45;
    /// Gear ratio of 7.31:1.
    static constexpr double RATIO_7_31 = 7.31;
    /// Gear ratio of 5.95:1.
    static constexpr double RATIO_5_95 = 5.95;
  };

  /**
   * Represents common motor layouts of the kit drivetrain.
   */
  class KitbotMotor {
   public:
    /// One CIM motor per drive side.
    static constexpr frc::DCMotor SingleCIMPerSide = frc::DCMotor::CIM(1);
    /// Two CIM motors per drive side.
    static constexpr frc::DCMotor DualCIMPerSide = frc::DCMotor::CIM(2);
    /// One Mini CIM motor per drive side.
    static constexpr frc::DCMotor SingleMiniCIMPerSide =
        frc::DCMotor::MiniCIM(1);
    /// Two Mini CIM motors per drive side.
    static constexpr frc::DCMotor DualMiniCIMPerSide = frc::DCMotor::MiniCIM(2);
    /// One Falcon 500 motor per drive side.
    static constexpr frc::DCMotor SingleFalcon500PerSide =
        frc::DCMotor::Falcon500(1);
    /// Two Falcon 500 motors per drive side.
    static constexpr frc::DCMotor DualFalcon500PerSide =
        frc::DCMotor::Falcon500(2);
    /// One NEO motor per drive side.
    static constexpr frc::DCMotor SingleNEOPerSide = frc::DCMotor::NEO(1);
    /// Two NEO motors per drive side.
    static constexpr frc::DCMotor DualNEOPerSide = frc::DCMotor::NEO(2);
  };

  /**
   * Represents common wheel sizes of the kit drivetrain.
   */
  class KitbotWheelSize {
   public:
    /// Six inch diameter wheels.
    static constexpr units::meter_t SIX_INCH = 6_in;
    /// Eight inch diameter wheels.
    static constexpr units::meter_t EIGHT_INCH = 8_in;
    /// Ten inch diameter wheels.
    static constexpr units::meter_t TEN_INCH = 10_in;
  };

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor     The motors installed in the bot.
   * @param gearing   The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]ᵀ. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  static DifferentialDrivetrainSim CreateKitbotSim(
      frc::DCMotor motor, double gearing, units::meter_t wheelSize,
      const std::array<double, 7>& measurementStdDevs = {}) {
    // MOI estimation -- note that I = mr² for point masses
    units::kilogram_square_meter_t batteryMoi = 12.5_lb * 10_in * 10_in;
    units::kilogram_square_meter_t gearboxMoi = (2.8_lb + 2.0_lb) *
                                                2  // CIM plus toughbox per side
                                                * (26_in / 2) * (26_in / 2);

    return DifferentialDrivetrainSim{
        motor,           gearing, batteryMoi + gearboxMoi, 60_lb,
        wheelSize / 2.0, 26_in,   measurementStdDevs};
  }

  /**
   * Create a sim for the standard FRC kitbot.
   *
   * @param motor     The motors installed in the bot.
   * @param gearing   The gearing reduction used.
   * @param wheelSize The wheel size.
   * @param J         The moment of inertia of the drivebase. This can be
   * calculated using SysId.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   * [x, y, heading, left velocity, right velocity, left distance, right
   * distance]ᵀ. Can be omitted if no noise is desired. Gyro standard
   * deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s, and
   * position measurement standard deviations of 0.005 meters are a reasonable
   * starting point.
   */
  static DifferentialDrivetrainSim CreateKitbotSim(
      frc::DCMotor motor, double gearing, units::meter_t wheelSize,
      units::kilogram_square_meter_t J,
      const std::array<double, 7>& measurementStdDevs = {}) {
    return DifferentialDrivetrainSim{
        motor, gearing, J, 60_lb, wheelSize / 2.0, 26_in, measurementStdDevs};
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
  Vectord<7> GetOutput() const;

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
  Vectord<7> GetState() const;

  LinearSystem<2, 2, 2> m_plant;
  units::meter_t m_rb;
  units::meter_t m_wheelRadius;

  DCMotor m_motor;

  double m_originalGearing;
  double m_currentGearing;

  Vectord<7> m_x;
  Eigen::Vector2d m_u;
  Vectord<7> m_y;
  std::array<double, 7> m_measurementStdDevs;
};
}  // namespace frc::sim
