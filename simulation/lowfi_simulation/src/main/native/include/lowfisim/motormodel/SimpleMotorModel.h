/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "lowfisim/motormodel/MotorModel.h"

namespace frc {
namespace sim {
namespace lowfi {

class SimpleMotorModel : public MotorModel {
 public:
  explicit SimpleMotorModel(double maxSpeed);

  void Reset() override;
  void SetVoltage(double voltage) override;
  void Update(double elapsedTime) override;

  double GetPosition() const override;
  double GetVelocity() const override;
  double GetAcceleration() const override;
  double GetCurrent() const override;

 protected:
  double m_maxSpeed;
  double m_voltagePercentage{0};
  double m_position{0};
  double m_velocity{0};

  static constexpr double kMaxExpectedVoltage = 12;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
