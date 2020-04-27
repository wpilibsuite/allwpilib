/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/length.h>
#include <units/time.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/system/LinearSystem.h"
#include "frc/trajectory/Trajectory.h"

namespace frc {

template <int N>
using Vector = Eigen::Matrix<double, N, 1>;

/**
 * A Linear Time-Varying Differential Drive Controller for differential drive
 * robots. This class takes in a LinearSystem of a differential drive,
 * which can be created from known linear and angular
 * Kv, and Ka terms with LinearSystemId::IdentifyDrivetrainSystem.
 * This is then used to calculate the model dynamics #Dynamics.
 *
 * This controller is advantageous over the LTVUnicycleController
 * due to the fact that it is easier to specify the relative weighting of
 * state vs. input (ex. being able to have the X, Y, Theta controller
 * have more jurisdiction over the input than the left and right velocity
 * controller.)
 *
 * The current state estimate for the controller can be determined by using
 * a DifferentialDriveStateEstimator.
 *
 * Our state-space system is:
 *
 * <strong> x = [[x, y, theta, vel_l, vel_r]]^T </strong> in the field
 * coordinate system.
 *
 * <strong> u = [[voltage_l, voltage_r]]^T </strong> the control input.
 */
class LTVDiffDriveController {
 public:
  /**
   * Construct a LTV Unicycle Controller.
   *
   * @param plant       A LinearSystem representing a differential
   * drivetrain.
   * @param controllerQ The maximum desired error tolerance for the robot's
   * state, in the form [X, Y, Heading, leftVelocity, right Velocity]^T. Units
   * are meters and radians for the translation and heading. 1 is a good
   * starting value.
   * @param controllerR The maximum desired control effort by the feedback
   * controller, in the form [voltsLeft, voltsRight]^T. should apply on top of
   * the trajectory feedforward.
   * @param kinematics  A DifferentialDriveKinematics object
   * representing the differential drivetrain's kinematics.
   * @param dtSeconds   The nominal dt of this controller. With command based
   * this is 0.020.
   */
  LTVDiffDriveController(const LinearSystem<2, 2, 2>& plant,
                         const std::array<double, 5>& controllerQ,
                         const std::array<double, 2>& controllerR,
                         const DifferentialDriveKinematics& kinematics,
                         units::second_t dt);

  /**
   * Construct a LTV Unicycle Controller.
   *
   * @param plant       A LinearSystem representing a differential
   * drivetrain.
   * @param controllerQ The maximum desired error tolerance for the robot's
   * state, in the form [X, Y, Heading, leftVelocity, right Velocity]^T. Units
   * are meters and radians for the translation and heading.
   * @param rho         A weighting factor that balances control effort and
   * state excursion. Greater values penalize state excursion more heavily. 1 is
   * a good starting value.
   * @param controllerR The maximum desired control effort by the feedback
   * controller, in the form [voltsLeft, voltsRight]^T. should apply on top of
   * the trajectory feedforward.
   * @param kinematics  A DifferentialDriveKinematics object
   * representing the differential drivetrain's kinematics.
   * @param dtSeconds   The nominal dt of this controller. With command based
   * this is 0.020.
   */
  LTVDiffDriveController(const LinearSystem<2, 2, 2>& plant,
                         const std::array<double, 5>& controllerQ, double rho,
                         const std::array<double, 2>& controllerR,
                         const DifferentialDriveKinematics& kinematics,
                         units::second_t dt);

  /**
   * Returns if the controller is at the reference pose on the trajectory.
   * Note that this is different than if the robot has traversed the entire
   * trajectory. The tolerance is set by the #SetTolerance method.
   *
   * @return If the robot is within the specified tolerances.
   */
  bool AtReference() const;

  /**
   * Set the tolerance for if the robot is #AtReference or not.
   *
   * @param poseTolerance The new pose tolerance.
   * @param velocityTolerance The velocity tolerance.
   */
  void SetTolerance(const Pose2d& poseTolerance,
                    units::meters_per_second_t velocityTolerance);

  /**
   * Returns the current controller reference in the form
   * [X, Y, Heading, LeftVelocity, RightVelocity, LeftPosition].
   *
   * @return Matrix [5, 1] The reference.
   */
  const Vector<5>& GetReferences() const;

  /**
   * Returns the state error as a Vector.
   */
  const Vector<5>& StateError() const;

  /**
   * Returns the inputs of the controller in the form [LeftVoltage,
   * RightVoltage].
   *
   * @return Matrix[2, 1] The inputs.
   */
  const Vector<2>& GetInputs() const;

  /**
   * Returns the uncapped control input after updating the controller with the
   * given reference and current states.
   *
   * @param currentState  The current state of the robot as a vector.
   * @param stateRef      The reference state vector.
   * @return The control input as a matrix with motor voltages [left, right].
   */
  const Vector<2>& Calculate(const Vector<5>& currentState,
                             const Vector<5>& stateRef);

  /**
   * Returns the next output of the controller.
   *
   * <p>The desired state should come from a Trajectory.
   *
   * @param currentState  The current state of the robot as a vector.
   * @param desiredState  The desired pose, linear velocity, and angular
   * velocity from a trajectory.
   * @return The control input as a matrix with motor voltages [left, right].
   */
  const Vector<2>& Calculate(const Vector<5>& currentState,
                             const Trajectory::State& desiredState);

  /**
   * Resets the internal state of the controller.
   */
  void Reset();

  Vector<2> Controller(const Vector<5>& x, const Vector<5>& r);

  Vector<10> Dynamics(const Vector<10>& x, const Vector<2>& u);

 private:
  LinearSystem<2, 2, 2> m_plant;
  units::meter_t m_rb;

  Vector<5> m_nextR;
  Vector<2> m_uncappedU;

  Eigen::Matrix<double, 5, 2> m_B;
  Eigen::Matrix<double, 2, 5> m_K0;
  Eigen::Matrix<double, 2, 5> m_K1;

  Vector<5> m_stateError;

  Pose2d m_poseTolerance;
  units::meters_per_second_t m_velocityTolerance;

  DifferentialDriveKinematics m_kinematics;

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

  class Input {
   public:
    static constexpr int kLeftVoltage = 0;
    static constexpr int kRightVoltage = 1;
  };
};

}  // namespace frc
