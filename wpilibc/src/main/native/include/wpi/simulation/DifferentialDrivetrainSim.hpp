// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/DCMotor.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/moment_of_inertia.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

namespace wpi::sim {

class DifferentialDrivetrainSim {
 public:
  /**
   * Creates a simulated differential drivetrain.
   *
   * @param plant The wpi::math::LinearSystem representing the robot's
   *     drivetrain. This system can be created with
   *     wpi::math::Models::DifferentialDriveFromPhysicalConstants() or
   *     wpi::math::Models::DifferentialDriveFromSysId().
   * @param trackwidth The robot's trackwidth.
   * @param driveMotor A wpi::math::DCMotor representing the left side of the
   *     drivetrain.
   * @param gearingRatio The gearingRatio ratio of the left side, as output over
   *     input. This must be the same ratio as the ratio used to identify or
   *     create the plant.
   * @param wheelRadius  The radius of the wheels on the drivetrain, in meters.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   *     [x, y, heading, left velocity, right velocity, left distance, right
   *     distance]ᵀ. Can be omitted if no noise is desired. Gyro standard
   *     deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s,
   *     and position measurement standard deviations of 0.005 meters are a
   *     reasonable starting point.
   */
  DifferentialDrivetrainSim(
      wpi::math::LinearSystem<2, 2, 2> plant, wpi::units::meter_t trackwidth,
      wpi::math::DCMotor driveMotor, double gearingRatio,
      wpi::units::meter_t wheelRadius,
      const std::array<double, 7>& measurementStdDevs = {});

  /**
   * Creates a simulated differential drivetrain.
   *
   * @param driveMotor A wpi::math::DCMotor representing the left side of the
   *     drivetrain.
   * @param gearing The gearing on the drive between motor and wheel, as output
   *     over input. This must be the same ratio as the ratio used to identify
   *     or create the plant.
   * @param J The moment of inertia of the drivetrain about its center.
   * @param mass The mass of the drivebase.
   * @param wheelRadius The radius of the wheels on the drivetrain.
   * @param trackwidth The robot's trackwidth, or distance between left and
   *     right wheels.
   * @param measurementStdDevs Standard deviations for measurements, in the form
   *     [x, y, heading, left velocity, right velocity, left distance, right
   *     distance]ᵀ. Can be omitted if no noise is desired. Gyro standard
   *     deviations of 0.0001 radians, velocity standard deviations of 0.05 m/s,
   *     and position measurement standard deviations of 0.005 meters are a
   *     reasonable starting point.
   */
  DifferentialDrivetrainSim(
      wpi::math::DCMotor driveMotor, double gearing,
      wpi::units::kilogram_square_meter_t J, wpi::units::kilogram_t mass,
      wpi::units::meter_t wheelRadius, wpi::units::meter_t trackwidth,
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
  void SetInputs(wpi::units::volt_t leftVoltage,
                 wpi::units::volt_t rightVoltage);

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
   * @param dt The time that's passed since the last
   * Update(wpi::units::second_t) call.
   */
  void Update(wpi::units::second_t dt);

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
  wpi::math::Rotation2d GetHeading() const;

  /**
   * Returns the current pose.
   */
  wpi::math::Pose2d GetPose() const;

  /**
   * Get the right encoder position in meters.
   * @return The encoder position.
   */
  wpi::units::meter_t GetRightPosition() const {
    return wpi::units::meter_t{GetOutput(State::kRightPosition)};
  }

  /**
   * Get the right encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  wpi::units::meters_per_second_t GetRightVelocity() const {
    return wpi::units::meters_per_second_t{GetOutput(State::kRightVelocity)};
  }

  /**
   * Get the left encoder position in meters.
   * @return The encoder position.
   */
  wpi::units::meter_t GetLeftPosition() const {
    return wpi::units::meter_t{GetOutput(State::kLeftPosition)};
  }

  /**
   * Get the left encoder velocity in meters per second.
   * @return The encoder velocity.
   */
  wpi::units::meters_per_second_t GetLeftVelocity() const {
    return wpi::units::meters_per_second_t{GetOutput(State::kLeftVelocity)};
  }

  /**
   * Returns the currently drawn current for the right side.
   */
  wpi::units::ampere_t GetRightCurrentDraw() const;

  /**
   * Returns the currently drawn current for the left side.
   */
  wpi::units::ampere_t GetLeftCurrentDraw() const;

  /**
   * Returns the currently drawn current.
   */
  wpi::units::ampere_t GetCurrentDraw() const;

  /**
   * Sets the system state.
   *
   * @param state The state.
   */
  void SetState(const wpi::math::Vectord<7>& state);

  /**
   * Sets the system pose.
   *
   * @param pose The pose.
   */
  void SetPose(const wpi::math::Pose2d& pose);

  /**
   * The differential drive dynamics function.
   *
   * @param x The state.
   * @param u The input.
   * @return The state derivative with respect to time.
   */
  wpi::math::Vectord<7> Dynamics(const wpi::math::Vectord<7>& x,
                                 const Eigen::Vector2d& u);

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
    /// Gear ratio of 12.75:1.
    static constexpr double k12p75 = 12.75;
    /// Gear ratio of 10.71:1.
    static constexpr double k10p71 = 10.71;
    /// Gear ratio of 8.45:1.
    static constexpr double k8p45 = 8.45;
    /// Gear ratio of 7.31:1.
    static constexpr double k7p31 = 7.31;
    /// Gear ratio of 5.95:1.
    static constexpr double k5p95 = 5.95;
  };

  /**
   * Represents common motor layouts of the kit drivetrain.
   */
  class KitbotMotor {
   public:
    /// One CIM motor per drive side.
    static constexpr wpi::math::DCMotor SingleCIMPerSide =
        wpi::math::DCMotor::CIM(1);
    /// Two CIM motors per drive side.
    static constexpr wpi::math::DCMotor DualCIMPerSide =
        wpi::math::DCMotor::CIM(2);
    /// One Mini CIM motor per drive side.
    static constexpr wpi::math::DCMotor SingleMiniCIMPerSide =
        wpi::math::DCMotor::MiniCIM(1);
    /// Two Mini CIM motors per drive side.
    static constexpr wpi::math::DCMotor DualMiniCIMPerSide =
        wpi::math::DCMotor::MiniCIM(2);
    /// One Falcon 500 motor per drive side.
    static constexpr wpi::math::DCMotor SingleFalcon500PerSide =
        wpi::math::DCMotor::Falcon500(1);
    /// Two Falcon 500 motors per drive side.
    static constexpr wpi::math::DCMotor DualFalcon500PerSide =
        wpi::math::DCMotor::Falcon500(2);
    /// One NEO motor per drive side.
    static constexpr wpi::math::DCMotor SingleNEOPerSide =
        wpi::math::DCMotor::NEO(1);
    /// Two NEO motors per drive side.
    static constexpr wpi::math::DCMotor DualNEOPerSide =
        wpi::math::DCMotor::NEO(2);
  };

  /**
   * Represents common wheel sizes of the kit drivetrain.
   */
  class KitbotWheelSize {
   public:
    /// Six inch diameter wheels.
    static constexpr wpi::units::meter_t kSixInch = 6_in;
    /// Eight inch diameter wheels.
    static constexpr wpi::units::meter_t kEightInch = 8_in;
    /// Ten inch diameter wheels.
    static constexpr wpi::units::meter_t kTenInch = 10_in;
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
      wpi::math::DCMotor motor, double gearing, wpi::units::meter_t wheelSize,
      const std::array<double, 7>& measurementStdDevs = {}) {
    // MOI estimation -- note that I = mr² for point masses
    wpi::units::kilogram_square_meter_t batteryMoi = 12.5_lb * 10_in * 10_in;
    wpi::units::kilogram_square_meter_t gearboxMoi =
        (2.8_lb + 2.0_lb) * 2  // CIM plus toughbox per side
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
      wpi::math::DCMotor motor, double gearing, wpi::units::meter_t wheelSize,
      wpi::units::kilogram_square_meter_t J,
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
  wpi::math::Vectord<7> GetOutput() const;

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
  wpi::math::Vectord<7> GetState() const;

  wpi::math::LinearSystem<2, 2, 2> m_plant;
  wpi::units::meter_t m_rb;
  wpi::units::meter_t m_wheelRadius;

  wpi::math::DCMotor m_motor;

  double m_originalGearing;
  double m_currentGearing;

  wpi::math::Vectord<7> m_x;
  Eigen::Vector2d m_u;
  wpi::math::Vectord<7> m_y;
  std::array<double, 7> m_measurementStdDevs;
};
}  // namespace wpi::sim
