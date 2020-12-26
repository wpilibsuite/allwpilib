// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
