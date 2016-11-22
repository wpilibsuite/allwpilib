/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include "SpeedController.h"

namespace frc {

class SpeedControllerGroup : public SpeedController {
 public:
  SpeedControllerGroup() = default;
  SpeedControllerGroup(
      std::initializer_list<SpeedController*> speedControllers);

  void Add(SpeedController* speedController);
  void Remove(SpeedController* speedController);

  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  void Disable() override;
  bool GetInverted() const override;
  void StopMotor() override;
  void PIDWrite(double output) override;

 private:
  bool m_isInverted = false;
  std::vector<SpeedController*> m_speedControllers;
};

}  // namespace frc
