#pragma once

#include <frc2/command/PIDSubsystem.h>

class ReplaceMePIDSubsystem : public frc2::PIDSubsystem {
 public:
  ReplaceMePIDSubsystem();

  void UseOutput(double output) override;

  double GetSetpoint() override;

  double GetMeasurement() override;
};