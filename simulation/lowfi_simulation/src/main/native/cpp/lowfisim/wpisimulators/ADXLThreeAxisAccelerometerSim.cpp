/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/ADXLThreeAxisAccelerometerSim.h"
#pragma warning(disable: 4244 4267 4146)

namespace frc {
namespace sim {
namespace lowfi {

ADXLThreeAxisAccelerometerSim::ADXLThreeAxisAccelerometerSim(
    hal::ThreeAxisAccelerometerData& accelerometerWrapper)
    : m_accelerometerWrapper(accelerometerWrapper),
      m_xWrapper(
          [data = &m_accelerometerWrapper] { return data->GetInitialized(); },
          [data = &m_accelerometerWrapper](double x) { data->x = x; },
          [data = &m_accelerometerWrapper] { return data->GetX(); }),

      m_yWrapper(
          [data = &m_accelerometerWrapper] { return data->GetInitialized(); },
          [data = &m_accelerometerWrapper](double y) { data->y = y; },
          [data = &m_accelerometerWrapper] { return data->GetY(); }),

      m_zWrapper(
          [data = &m_accelerometerWrapper] { return data->GetInitialized(); },
          [data = &m_accelerometerWrapper](double z) { data->z = z; },
          [data = &m_accelerometerWrapper] { return data->GetZ(); }) {}

AccelerometerSim& ADXLThreeAxisAccelerometerSim::GetXWrapper() {
  return m_xWrapper;
}

AccelerometerSim& ADXLThreeAxisAccelerometerSim::GetYWrapper() {
  return m_yWrapper;
}

AccelerometerSim& ADXLThreeAxisAccelerometerSim::GetZWrapper() {
  return m_zWrapper;
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
