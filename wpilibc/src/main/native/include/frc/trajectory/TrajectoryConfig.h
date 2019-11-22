/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <units/units.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h"
#include "frc/trajectory/constraint/MecanumDriveKinematicsConstraint.h"
#include "frc/trajectory/constraint/SwerveDriveKinematicsConstraint.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
/**
 * Represents the configuration for generating a trajectory. This class stores
 * the start velocity, end velocity, max velocity, max acceleration, custom
 * constraints, and the reversed flag.
 *
 * The class must be constructed with a max velocity and max acceleration.
 * The other parameters (start velocity, end velocity, constraints, reversed)
 * have been defaulted to reasonable values (0, 0, {}, false). These values can
 * be changed via the SetXXX methods.
 */
class TrajectoryConfig {
 public:
  /**
   * Constructs a config object.
   * @param maxVelocity The max velocity of the trajectory.
   * @param maxAcceleration The max acceleration of the trajectory.
   */
  TrajectoryConfig(units::meters_per_second_t maxVelocity,
                   units::meters_per_second_squared_t maxAcceleration)
      : m_maxVelocity(maxVelocity), m_maxAcceleration(maxAcceleration) {}

  TrajectoryConfig(const TrajectoryConfig&) = delete;
  TrajectoryConfig& operator=(const TrajectoryConfig&) = delete;

  TrajectoryConfig(TrajectoryConfig&&) = default;
  TrajectoryConfig& operator=(TrajectoryConfig&&) = default;

  /**
   * Sets the start velocity of the trajectory.
   * @param startVelocity The start velocity of the trajectory.
   */
  void SetStartVelocity(units::meters_per_second_t startVelocity) {
    m_startVelocity = startVelocity;
  }

  /**
   * Sets the end velocity of the trajectory.
   * @param endVelocity The end velocity of the trajectory.
   */
  void SetEndVelocity(units::meters_per_second_t endVelocity) {
    m_endVelocity = endVelocity;
  }

  /**
   * Sets the reversed flag of the trajectory.
   * @param reversed Whether the trajectory should be reversed or not.
   */
  void SetReversed(bool reversed) { m_reversed = reversed; }

  /**
   * Adds a user-defined constraint to the trajectory.
   * @param constraint The user-defined constraint.
   */
  template <typename Constraint, typename = std::enable_if_t<std::is_base_of_v<
                                     TrajectoryConstraint, Constraint>>>
  void AddConstraint(Constraint constraint) {
    m_constraints.emplace_back(std::make_unique<Constraint>(constraint));
  }

  /**
   * Adds a differential drive kinematics constraint to ensure that
   * no wheel velocity of a differential drive goes above the max velocity.
   *
   * @param kinematics The differential drive kinematics.
   */
  void SetKinematics(const DifferentialDriveKinematics& kinematics) {
    AddConstraint(
        DifferentialDriveKinematicsConstraint(kinematics, m_maxVelocity));
  }

  /**
   * Adds a mecanum drive kinematics constraint to ensure that
   * no wheel velocity of a mecanum drive goes above the max velocity.
   *
   * @param kinematics The mecanum drive kinematics.
   */
  void SetKinematics(MecanumDriveKinematics kinematics) {
    AddConstraint(MecanumDriveKinematicsConstraint(kinematics, m_maxVelocity));
  }

  /**
   * Adds a swerve drive kinematics constraint to ensure that
   * no wheel velocity of a swerve drive goes above the max velocity.
   *
   * @param kinematics The swerve drive kinematics.
   */
  template <size_t NumModules>
  void SetKinematics(SwerveDriveKinematics<NumModules>& kinematics) {
    AddConstraint(SwerveDriveKinematicsConstraint(kinematics, m_maxVelocity));
  }

  /**
   * Returns the starting velocity of the trajectory.
   * @return The starting velocity of the trajectory.
   */
  units::meters_per_second_t StartVelocity() const { return m_startVelocity; }

  /**
   * Returns the ending velocity of the trajectory.
   * @return The ending velocity of the trajectory.
   */
  units::meters_per_second_t EndVelocity() const { return m_endVelocity; }

  /**
   * Returns the maximum velocity of the trajectory.
   * @return The maximum velocity of the trajectory.
   */
  units::meters_per_second_t MaxVelocity() const { return m_maxVelocity; }

  /**
   * Returns the maximum acceleration of the trajectory.
   * @return The maximum acceleration of the trajectory.
   */
  units::meters_per_second_squared_t MaxAcceleration() const {
    return m_maxAcceleration;
  }

  /**
   * Returns the user-defined constraints of the trajectory.
   * @return The user-defined constraints of the trajectory.
   */
  const std::vector<std::unique_ptr<TrajectoryConstraint>>& Constraints()
      const {
    return m_constraints;
  }

  /**
   * Returns whether the trajectory is reversed or not.
   * @return whether the trajectory is reversed or not.
   */
  bool IsReversed() const { return m_reversed; }

 private:
  units::meters_per_second_t m_startVelocity = 0_mps;
  units::meters_per_second_t m_endVelocity = 0_mps;
  units::meters_per_second_t m_maxVelocity;
  units::meters_per_second_squared_t m_maxAcceleration;
  std::vector<std::unique_ptr<TrajectoryConstraint>> m_constraints;
  bool m_reversed = false;
};
}  // namespace frc
