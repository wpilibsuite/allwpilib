/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/system/LinearSystem.h>
#include <frc/system/plant/DCMotor.h>

#include <Eigen/Core>
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
   * @param kinematics        A {@link DifferentialDriveKinematics} object
   * representing the differential drivetrain's kinematics.
   * @param driveMotor        A {@link DCMotor} representing the left side of
   * the drivetrain.
   * @param gearingRatio      The gearingRatio ratio of the left side, as output
   * over input. This must be the same ratio as the ratio used to identify or
   * create the drivetrainPlant.
   * @param wheelRadiusMeters The radius of the wheels on the drivetrain, in
   * meters.
   */
  DifferentialDrivetrainSim(LinearSystem<2, 2, 2>& plant,
                            DifferentialDriveKinematics& kinematics,
                            DCMotor driveMotor, double gearingRatio,
                            units::meter_t wheelRadius);

  /**
   * Set the applied voltage to the drivetrain. Note that positive voltage must
   * make that side of the drivetrain travel forward (+X).
   *
   * @param leftVoltage  The left voltage.
   * @param rightVoltage The right voltage.
   */
  void SetInputs(units::volt_t leftVoltage, units::volt_t rightVoltage);

  /**
   * Set the gearing reduction on the drivetrain. This is commonly used for
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
   * Returns an element of the state vector.
   *
   * @param state The row of the state vector.
   */
  double GetState(int state) const;

  /**
   * Get the current gearing reduction of the drivetrain, as output over input.
   */
  double GetGearing() const;

  /**
   * Returns the current state vector x.
   */
  Eigen::Matrix<double, 10, 1> GetState() const;

  /**
   * Get the estimated direction the robot is pointing. Note that this angle is
   * counterclockwise-positive, while most gyros are clockwise positive.
   */
  Rotation2d GetHeading() const;

  /**
   * Returns the current estimated position.
   */
  Pose2d GetEstimatedPosition() const;

  /**
   * Returns the currently drawn current.
   */
  units::ampere_t GetCurrentDraw() const;

  Eigen::Matrix<double, 10, 1> Dynamics(const Eigen::Matrix<double, 10, 1>& x,
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
    static constexpr int kLeftVoltageError = 7;
    static constexpr int kRightVoltageError = 8;
    static constexpr int kAngularVelocityError = 9;
  };

 private:
  LinearSystem<2, 2, 2> m_plant;
  units::meter_t m_rb;
  units::meter_t m_wheelRadius;

  DCMotor m_motor;

  double m_originalGearing;
  double m_currentGearing;

  Eigen::Matrix<double, 10, 1> m_x;
  Eigen::Matrix<double, 2, 1> m_u;
};
}  // namespace frc::sim
