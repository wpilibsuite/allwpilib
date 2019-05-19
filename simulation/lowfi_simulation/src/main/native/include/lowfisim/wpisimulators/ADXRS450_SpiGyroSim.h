/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ADXRS450_SpiGyroWrapperData.h"
#include "lowfisim/GyroSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class ADXRS450_SpiGyroSim : public GyroSim {
 public:
  explicit ADXRS450_SpiGyroSim(int spiPort);

  bool IsWrapperInitialized() const override;

  void SetAngle(double angle) override;
  double GetAngle() override;

 protected:
  hal::ADXRS450_SpiGyroWrapper m_gyroWrapper;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
