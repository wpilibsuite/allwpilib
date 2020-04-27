/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <units/force.h>
#include <units/length.h>
#include <units/mass.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/RungeKutta.h"
#include "frc/system/plant/DCMotor.h"

namespace frc {
namespace sim {
/**
 * Represents a simulated swerve drive module.
 */
class SimSwerveModule {
 public:
  /**
   * Creates a simulated swerve drive module.
   *
   * @param motor         The drive motor on the module.
   * @param gearing       The gearing of the drive motor (numbers greater than 1
   *                      represent reductions).
   * @param wheelRadius   The radius of the module wheel.
   * @param position      The position of the module on the robot.
   * @param azimuthSystem The linear system representing the module azimuth.
   */
  SimSwerveModule(const DCMotor& motor, double gearing,
                  units::meter_t wheelRadius, const Translation2d& position,
                  const LinearSystem<2, 1, 1>& azimuthSystem);

  /**
   * Returns the azimuth angle of the module.
   * @return The azimuth angle of the module.
   */
  Rotation2d GetAzimuthAngle() const;

  /**
   * Returns the module position on the robot.
   * @return The module position on the robot.
   */
  const Translation2d& GetModulePosition() const;

  /**
   * Returns an estimate of the force exerted by the module.
   *
   * @param wheelVelocity The velocity of the module wheel.
   * @param wheelVoltage  The voltage applied to the module drive motor.
   *
   * @return An estimate of the force exerted by the module.
   */
  units::newton_t EstimateModuleForce(units::meters_per_second_t wheelVelocity,
                                      units::volt_t wheelVoltage) const;

  /**
   * Updates the orientation of the module.
   *
   * @param azimuthVoltage The voltage applied to the azimuth motor.
   * @param dt             The time betwee updates.
   *
   * @return The new azimuth angle of the module.
   */
  Rotation2d Update(units::volt_t azimuthVoltage, units::second_t dt);

 private:
  DCMotor m_driveMotor;
  double m_gearing;
  units::meter_t m_wheelRadius;
  Translation2d m_position;

  LinearSystem<2, 1, 1> m_azimuthSystem;
  Eigen::Matrix<double, 2, 1> m_azimuthState;
};

/**
 * Represents a simulated swerve drive robot.
 */
template <int NumModules>
class SwerveDriveSim {
 public:
  /**
   * Creates a simulated swerve drive robot.
   *
   * @param mass              The mass of the chassis.
   * @param azimuthSystem     The linear system representing a module azimuth.
   * @param driveMotor        The motor used for driving on each module.
   * @param driveMotorGearing The gearing of the drive motor (numbers greater
   *                          than 1 represent reductions).
   * @param wheel             The location of the front left wheel.
   * @param wheels            The locations of the other modules on the robot.
   */
  template <typename... Wheels>
  SwerveDriveSim(units::kilogram_t mass,
                 const LinearSystem<2, 1, 1>& azimuthSystem,
                 const DCMotor& driveMotor, double driveMotorGearing,
                 units::meter_t wheelRadius, const Translation2d& wheel,
                 Wheels&&... wheels)
      : m_modules(GetModulesFromInfo(azimuthSystem, driveMotor,
                                     driveMotorGearing, wheelRadius, wheel,
                                     wheels...)),
        m_kinematics(GetKinematicsFromInfo(m_modules)),
        m_mass(mass) {}

  /**
   * Creates a simulated swerve drive robot.
   *
   * @param mass    The mass of the chassis.
   * @param module  The front-left module.
   * @param modules The other modules on the drivetrain.
   */
  template <typename... Modules>
  SwerveDriveSim(units::kilogram_t mass, const SimSwerveModule& module,
                 Modules&&... modules)
      : SwerveDriveSim{mass, {module, modules...}} {}

  /**
   * Creates a simulated swerve drive robot.
   *
   * @param mass    The mass of the chassis.
   * @param modules An array of sim swerve modules on the robot.
   */
  SwerveDriveSim(units::kilogram_t mass,
                 const std::array<SimSwerveModule, NumModules> modules)
      : m_modules(modules),
        m_kinematics(CreateKinematicsFromInfo(m_modules)),
        m_mass(mass) {}

  /**
   * Returns a const reference to the kinematics object that represents the
   * drivetrain.
   * @return A const reference to the kinematics object that represents the
   * drivetrain.
   */
  const SwerveDriveKinematics<NumModules>& GetKinematics() const {
    return m_kinematics;
  }

  /**
   * Returns the estimated speed of the chassis at this moment.
   * @return The estimated speed of the chassis at this moment.
   */
  const ChassisSpeeds& GetEstimatedSpeed() const { return m_estimatedSpeed; }

  /**
   * Returns the module states that represent the chassis' motion.
   * @return The module states that represent the chassis' motion.
   */
  std::array<SwerveModuleState, NumModules> GetEstimatedModuleStates() const {
    return m_kinematics.ToSwerveModuleStates(m_estimatedSpeed);
  }

  /**
   * Returns a const reference to the array of sim modules of this chassis.
   * @return A const reference to the array of sim modules of this chassis.
   */
  const std::array<SimSwerveModule, NumModules>& GetModules() const {
    return m_modules;
  }

  /**
   * Returns an array of the azimuth angles of all modules.
   * @return An array of the azimuth angles of all modules.
   */
  std::array<Rotation2d, NumModules> GetEstimatedModuleAngles() const {
    std::array<Rotation2d, NumModules> angles;
    for (size_t i = 0; i < NumModules; i++) {
      angles[i] = m_modules[i].GetAzimuthAngle();
    }
    return angles;
  }

  /**
   * Sets the module drive voltages.
   *
   * @param voltage  The drive voltage of the front-left module.
   * @param voltages The drive voltages of all other modules.
   */
  template <typename... Voltages>
  void SetModuleDriveVoltages(units::volt_t voltage, Voltages&&... voltages) {
    static_assert(sizeof...(voltages) == (NumModules - 1));
    SetModuleDriveVoltages({voltage, voltages...});
  }

  /**
   * Sets the module drive voltages.
   *
   * @param voltages An array of all drive voltages.
   */
  void SetModuleDriveVoltages(
      const std::array<units::volt_t, NumModules>& voltages) {
    m_driveVoltages = voltages;
  }

  /**
   * Sets the azimuth drive voltages.
   *
   * @param voltage  The azimuth voltage of the front-left module.
   * @param voltages The azimuth voltages of all other modules.
   */
  template <typename... Voltages>
  void SetModuleAzimuthVoltages(units::volt_t voltage, Voltages&&... voltages) {
    static_assert(sizeof...(voltages) == (NumModules - 1));
    SetModuleAzimuthVoltages({voltage, voltages...});
  }

  /**
   * Sets the azimuth drive voltages.
   *
   * @param voltages An array of all azimuth voltages.
   */
  void SetModuleAzimuthVoltages(
      const std::array<units::volt_t, NumModules>& voltages) {
    m_azimuthVoltages = voltages;
  }

  /**
   * Updates the state of the chassis.
   *
   * @param dt The time between updates.
   */
  void Update(units::second_t dt) {
    // First, we determine the wheel speeds necessary to be at the current
    // chassis speeds Then, we use this speed to calculate the force each module
    // exerts on the chassis, Which we use to figure out the acceleration-ish of
    // each wheel. We then integrate this acceleration forward with RK4, and use
    // IK to go back to chassis speeds from our new wheel speeds.
    auto wheelSpeeds = m_kinematics.ToSwerveModuleStates(m_estimatedSpeed);
    std::array<SwerveModuleState, NumModules> newStates;

    for (size_t i = 0; i < NumModules; ++i) {
      auto newSpeed = RungeKutta(
          // F = ma  <==> a = F / m
          [&](double x, double u) {
            return m_modules[i]
                       .EstimateModuleForce(wheelSpeeds[i].speed,
                                            m_driveVoltages[i])
                       .template to<double>() /
                   m_mass.to<double>() * NumModules;
          },
          wheelSpeeds[i].speed.template to<double>(),
          m_driveVoltages[i].template to<double>(), dt);
      Rotation2d angle = m_modules[i].Update(m_azimuthVoltages[i], dt);
      newStates[i] =
          SwerveModuleState{units::meters_per_second_t(newSpeed), angle};
    }

    m_estimatedSpeed = m_kinematics.ToChassisSpeeds(newStates);

    // Integrate heading forward with RK4
    m_estimatedHeading = units::radian_t(RungeKutta(
        [&](double heading) {
          return m_estimatedSpeed.omega.template to<double>();
        },
        m_estimatedHeading.Radians().template to<double>(), dt));
  }

  /**
   * Resets the speed of the chassis.
   *
   * @param speeds The speed to reset to.
   */
  void ResetChassisSpeeds(const ChassisSpeeds& speeds) {
    m_estimatedSpeed = speeds;
  }

 private:
  template <typename... Wheels>
  static std::array<SimSwerveModule, NumModules> GetModulesFromInfo(
      const LinearSystem<2, 1, 1>& azimuthSystem, const DCMotor& driveMotor,
      double driveMotorGearing, units::meter_t wheelRadius,
      const Translation2d& wheel, Wheels&&... wheels) {
    std::array<Translation2d, NumModules> locations{wheel, wheels...};
    std::array<SimSwerveModule, NumModules> modules;

    for (size_t i = 0; i < NumModules; ++i) {
      modules[i] = SimSwerveModule(driveMotor, driveMotorGearing, wheelRadius,
                                   locations[i], azimuthSystem);
    }

    return modules;
  }

  static SwerveDriveKinematics<NumModules> CreateKinematicsFromInfo(
      const std::array<SimSwerveModule, NumModules>& modules) {
    std::array<Translation2d, NumModules> locations;
    for (size_t i = 0; i < NumModules; ++i) {
      locations[i] = modules[i].GetModulePosition();
    }
    return SwerveDriveKinematics<4>(locations);
  }

  std::array<SimSwerveModule, NumModules> m_modules;
  SwerveDriveKinematics<NumModules> m_kinematics;

  units::kilogram_t m_mass;

  std::array<units::volt_t, NumModules> m_driveVoltages;
  std::array<units::volt_t, NumModules> m_azimuthVoltages;

  ChassisSpeeds m_estimatedSpeed;
  Rotation2d m_estimatedHeading;
};  // namespace sim

}  // namespace sim
}  // namespace frc
