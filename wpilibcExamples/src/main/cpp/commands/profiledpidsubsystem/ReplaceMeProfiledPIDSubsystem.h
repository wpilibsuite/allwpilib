#pragma once

#include <frc2/command/ProfiledPIDSubsystem.h>

class ReplaceMeProfiledPIDSubsystem : public frc2::ProfiledPIDSubsystem {
 public:
  ReplaceMeProfiledPIDSubsystem();

  void UseOutput(double output, frc::TrapezoidProfile::State setpoint) override;

  frc::TrapezoidProfile::State GetGoal() override;

  units::meter_t GetMeasurement() override;
};