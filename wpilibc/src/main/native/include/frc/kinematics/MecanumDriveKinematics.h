/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>
#include <Eigen/QR>
#include <hal/FRCUsageReporting.h>

#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/MecanumDriveWheelSpeeds.h"

namespace frc {

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual wheel speeds.
 *
 * The inverse kinematics (converting from a desired chassis velocity to
 * individual wheel speeds) uses the relative locations of the wheels with
 * respect to the center of rotation. The center of rotation for inverse
 * kinematics is also variable. This means that you can set your set your center
 * of rotation in a corner of the robot to perform special evasion manuevers.
 *
 * Forward kinematics (converting an array of wheel speeds into the overall
 * chassis motion) is performs the exact opposite of what inverse kinematics
 * does. Since this is an overdetermined system (more equations than variables),
 * we use a least-squares approximation.
 *
 * The inverse kinematics: [wheelSpeeds] = [wheelLocations] * [chassisSpeeds]
 * We take the Moore-Penrose pseudoinverse of [wheelLocations] and then
 * multiply by [wheelSpeeds] to get our chassis speeds.
 *
 * Forward kinematics is also used for odometry -- determining the position of
 * the robot on the field using encoders and a gyro.
 */
class MecanumDriveKinematics {
 public:
  /**
   * Constructs a mecanum drive kinematics object.
   *
   * @param frontLeftWheel The location of the front-left wheel relative to the
   *                       physical center of the robot.
   * @param frontRightWheel The location of the front-right wheel relative to
   *                        the physical center of the robot.
   * @param rearLeftWheel The location of the rear-left wheel relative to the
   *                      physical center of the robot.
   * @param rearRightWheel The location of the rear-right wheel relative to the
   *                       physical center of the robot.
   */
  explicit MecanumDriveKinematics(Translation2d frontLeftWheel,
                                  Translation2d frontRightWheel,
                                  Translation2d rearLeftWheel,
                                  Translation2d rearRightWheel)
      : m_frontLeftWheel{frontLeftWheel},
        m_frontRightWheel{frontRightWheel},
        m_rearLeftWheel{rearLeftWheel},
        m_rearRightWheel{rearRightWheel} {
    SetInverseKinematics(frontLeftWheel, frontRightWheel, rearLeftWheel,
                         rearRightWheel);
    m_forwardKinematics = m_inverseKinematics.householderQr();
    HAL_Report(HALUsageReporting::kResourceType_Kinematics,
               HALUsageReporting::kKinematics_MecanumDrive);
  }

  MecanumDriveKinematics(const MecanumDriveKinematics&) = default;

  /**
   * Performs inverse kinematics to return the wheel speeds from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * wheel speeds.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * manuevers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @param centerOfRotation The center of rotation. For example, if you set the
   *                         center of rotation at one corner of the robot and
   *                         provide a chassis speed that only has a dtheta
   *                         component, the robot will rotate around that
   *                         corner.
   *
   * @return The wheel speeds. Use caution because they are not normalized.
   *         Sometimes, a user input may cause one of the wheel speeds to go
   *         above the attainable max velocity. Use the
   *         MecanumDriveWheelSpeeds::Normalize() function to rectify this
   *         issue. In addition, you can leverage the power of C++17 to directly
   *         assign the wheel speeds to variables:
   *
   * @code{.cpp}
   * auto [fl, fr, bl, br] = kinematics.ToWheelSpeeds(chassisSpeeds);
   * @endcode
   */
  MecanumDriveWheelSpeeds ToWheelSpeeds(
      const ChassisSpeeds& chassisSpeeds,
      const Translation2d& centerOfRotation = Translation2d());

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given wheel speeds. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world speed of
   * each wheel on the robot.
   *
   * @param wheelSpeeds The current mecanum drive wheel speeds.
   *
   * @return The resulting chassis speed.
   */
  ChassisSpeeds ToChassisSpeeds(const MecanumDriveWheelSpeeds& wheelSpeeds);

 private:
  Eigen::Matrix<double, 4, 3> m_inverseKinematics;
  Eigen::HouseholderQR<Eigen::Matrix<double, 4, 3>> m_forwardKinematics;
  Translation2d m_frontLeftWheel;
  Translation2d m_frontRightWheel;
  Translation2d m_rearLeftWheel;
  Translation2d m_rearRightWheel;

  Translation2d m_previousCoR;

  /**
   * Construct inverse kinematics matrix from wheel locations.
   *
   * @param fl The location of the front-left wheel relative to the physical
   *           center of the robot.
   * @param fr The location of the front-right wheel relative to the physical
   *           center of the robot.
   * @param rl The location of the rear-left wheel relative to the physical
   *           center of the robot.
   * @param rr The location of the rear-right wheel relative to the physical
   *           center of the robot.
   */
  void SetInverseKinematics(Translation2d fl, Translation2d fr,
                            Translation2d rl, Translation2d rr);
};

}  // namespace frc
