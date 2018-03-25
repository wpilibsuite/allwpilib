/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

#include "PIDOutput.h"

namespace frc {

class PIDOutputBuffer : public PIDOutput {
 public:
  void PIDWrite(double output) override;

  double GetOutput() const;

 private:
  std::atomic<double> m_output{0.0};
};

}  // namespace frc
