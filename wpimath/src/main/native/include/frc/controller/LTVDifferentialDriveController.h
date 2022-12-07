// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/array.h>
#include <wpi/interpolating_map.h>

#include "frc/EigenCore.h"
#include "frc/controller/DifferentialDriveWheelVoltages.h"
#include "frc/geometry/Pose2d.h"
#include "frc/system/LinearSystem.h"
#include "frc/trajectory/Trajectory.h"
#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"
#include "units/voltage.h"

namespace frc {

/**
 * The linear time-varying differential drive controller has a similar form to
 * the LQR, but the model used to compute the controller gain is the nonlinear
 * model linearized around the drivetrain's current state. We precomputed gains
 * for important places in our state-space, then interpolated between them with
 * a LUT to save computational resources.
 *
 * See section 8.7 in Controls Engineering in FRC for a derivation of the
 * control law we used shown in theorem 8.7.4.
 */
class WPILIB_DLLEXPORT LTVDifferentialDriveController {
 public:
  /**
   * Constructs a linear time-varying differential drive controller.
   *
   * @param plant      The differential drive velocity plant.
   * @param trackwidth The distance between the differential drive's left and
   *                   right wheels.
   * @param Qelems     The maximum desired error tolerance for each state.
   * @param Relems     The maximum desired control effort for each input.
   * @param dt         Discretization timestep.
   */
  LTVDifferentialDriveController(const frc::LinearSystem<2, 2, 2>& plant,
                                 units::meter_t trackwidth,
                                 const wpi::array<double, 5>& Qelems,
                                 const wpi::array<double, 2>& Relems,
                                 units::second_t dt);

  /**
   * Move constructor.
   */
  LTVDifferentialDriveController(LTVDifferentialDriveController&&) = default;

  /**
   * Move assignment operator.
   */
  LTVDifferentialDriveController& operator=(LTVDifferentialDriveController&&) =
      default;

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  bool AtReference() const;

  /**
   * Sets the pose error which is considered tolerable for use with
   * AtReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   * @param leftVelocityTolerance Left velocity error which is tolerable.
   * @param rightVelocityTolerance Right velocity error which is tolerable.
   */
  void SetTolerance(const Pose2d& poseTolerance,
                    units::meters_per_second_t leftVelocityTolerance,
                    units::meters_per_second_t rightVelocityTolerance);

  /**
   * Returns the left and right output voltages of the LTV controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose      The current pose.
   * @param leftVelocity     The current left velocity.
   * @param rightVelocity    The current right velocity.
   * @param poseRef          The desired pose.
   * @param leftVelocityRef  The desired left velocity.
   * @param rightVelocityRef The desired right velocity.
   */
  DifferentialDriveWheelVoltages Calculate(
      const Pose2d& currentPose, units::meters_per_second_t leftVelocity,
      units::meters_per_second_t rightVelocity, const Pose2d& poseRef,
      units::meters_per_second_t leftVelocityRef,
      units::meters_per_second_t rightVelocityRef);

  /**
   * Returns the left and right output voltages of the LTV controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose  The current pose.
   * @param leftVelocity The left velocity.
   * @param rightVelocity The right velocity.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   */
  DifferentialDriveWheelVoltages Calculate(
      const Pose2d& currentPose, units::meters_per_second_t leftVelocity,
      units::meters_per_second_t rightVelocity,
      const Trajectory::State& desiredState);

 private:
  units::meter_t m_trackwidth;

  // LUT from drivetrain linear velocity to LQR gain
  wpi::interpolating_map<units::meters_per_second_t, Matrixd<2, 5>> m_table;

  Vectord<5> m_error;
  Vectord<5> m_tolerance;
};

}  // namespace frc
