/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/Twine.h>

namespace wpi::math {

enum class MathUsageId {
  kKinematics_DifferentialDrive,
  kKinematics_MecanumDrive,
  kKinematics_SwerveDrive,
  kTrajectory_TrapezoidProfile,
  kFilter_Linear,
  kOdometry_DifferentialDrive,
  kOdometry_SwerveDrive,
  kOdometry_MecanumDrive
};

class MathShared {
 public:
  virtual ~MathShared() = default;
  virtual void ReportError(const wpi::Twine& error) = 0;
  virtual void ReportUsage(MathUsageId id, int count) = 0;
};

class MathSharedStore {
 public:
  static MathShared& GetMathShared();

  static void SetMathShared(std::unique_ptr<MathShared> shared);

  static void ReportError(const wpi::Twine& error) {
    GetMathShared().ReportError(error);
  }

  static void ReportUsage(MathUsageId id, int count) {
    GetMathShared().ReportUsage(id, count);
  }
};

}  // namespace wpi::math
