/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ThreeAxisAccelerometerData.h"
#include "lowfisim/SimpleAccelerometerSim.h"

namespace frc {
namespace sim {
namespace lowfi {

class ADXLThreeAxisAccelerometerSim {
 public:
  ADXLThreeAxisAccelerometerSim(
      hal::ThreeAxisAccelerometerData& accelerometerWrapper);

  AccelerometerSim& GetXWrapper();
  AccelerometerSim& GetYWrapper();
  AccelerometerSim& GetZWrapper();

 protected:
  hal::ThreeAxisAccelerometerData& m_accelerometerWrapper;
  SimpleAccelerometerSim m_xWrapper;
  SimpleAccelerometerSim m_yWrapper;
  SimpleAccelerometerSim m_zWrapper;
};

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
