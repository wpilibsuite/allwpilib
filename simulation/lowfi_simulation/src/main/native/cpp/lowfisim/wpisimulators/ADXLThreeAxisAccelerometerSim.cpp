/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/ADXLThreeAxisAccelerometerSim.h"

namespace frc {
namespace sim {
namespace lowfi {

ADXLThreeAxisAccelerometerSim::ADXLThreeAxisAccelerometerSim(
    hal::ThreeAxisAccelerometerData& accelerometerWrapper)
    : m_accelerometerWrapper(accelerometerWrapper),
      m_xWrapper(std::function<bool(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetInitialized,
                               &m_accelerometerWrapper)),
                 std::function<void(double)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::SetX,
                               &m_accelerometerWrapper, std::placeholders::_1)),
                 std::function<double(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetX,
                               &m_accelerometerWrapper))),

      m_yWrapper(std::function<bool(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetInitialized,
                               &m_accelerometerWrapper)),
                 std::function<void(double)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::SetY,
                               &m_accelerometerWrapper, std::placeholders::_1)),
                 std::function<double(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetY,
                               &m_accelerometerWrapper))),

      m_zWrapper(std::function<bool(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetInitialized,
                               &m_accelerometerWrapper)),
                 std::function<void(double)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::SetZ,
                               &m_accelerometerWrapper, std::placeholders::_1)),
                 std::function<double(void)>(
                     std::bind(&hal::ThreeAxisAccelerometerData::GetZ,
                               &m_accelerometerWrapper))) {}

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
