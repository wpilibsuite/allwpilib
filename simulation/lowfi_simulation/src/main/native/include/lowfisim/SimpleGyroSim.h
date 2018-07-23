/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include "lowfisim/GyroSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class SimpleGyroSim : public GyroSim {
 public:
  SimpleGyroSim(std::function<void(double)>& setterFunction,
                std::function<double(void)>& getterFunction)
      : m_setAngleFunction(setterFunction),
        m_getAngleFunction(getterFunction) {}
  double GetAngle() override { return m_getAngleFunction(); }

  void SetAngle(double angle) override { m_setAngleFunction(angle); }

 private:
  std::function<void(double)> m_setAngleFunction;
  std::function<double(void)> m_getAngleFunction;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
