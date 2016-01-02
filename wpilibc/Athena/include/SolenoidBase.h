/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Resource.h"
#include "SensorBase.h"
#include "HAL/HAL.hpp"
#include "HAL/Port.h"

#include <memory>

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public SensorBase {
 public:
  virtual ~SolenoidBase() = default;
  uint8_t GetAll(int module = 0) const;

  uint8_t GetPCMSolenoidBlackList(int module) const;
  bool GetPCMSolenoidVoltageStickyFault(int module) const;
  bool GetPCMSolenoidVoltageFault(int module) const;
  void ClearAllPCMStickyFaults(int module);

 protected:
  explicit SolenoidBase(uint8_t pcmID);
  void Set(uint8_t value, uint8_t mask, int module);
  const static int m_maxModules = 63;
  const static int m_maxPorts = 8;
  static void* m_ports[m_maxModules][m_maxPorts];
  uint32_t m_moduleNumber;  ///< Slot number where the module is plugged into
                            ///the chassis.
  static std::unique_ptr<Resource> m_allocated;
};
