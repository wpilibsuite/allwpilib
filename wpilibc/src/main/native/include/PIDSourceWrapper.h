/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>

#include "PIDSource.h"

namespace frc {

class PIDSourceWrapper : public PIDSource {
 public:
  PIDSourceWrapper(std::function<double()> func);  // NOLINT(runtime/explicit)

  double PIDGet() override;

 private:
  std::function<double()> m_func;
};

}  // namespace frc
