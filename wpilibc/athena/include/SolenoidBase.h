/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "Resource.h"
#include "SensorBase.h"

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public SensorBase {
 public:
  virtual ~SolenoidBase() = default;
  uint8_t GetAll(int32_t module = 0) const;

  uint8_t GetPCMSolenoidBlackList(int32_t module) const;
  bool GetPCMSolenoidVoltageStickyFault(int32_t module) const;
  bool GetPCMSolenoidVoltageFault(int32_t module) const;
  void ClearAllPCMStickyFaults(int32_t module);

 protected:
  explicit SolenoidBase(uint8_t pcmID);
  void Set(uint8_t value, uint8_t mask, int32_t module);
  const static int32_t m_maxModules = 63;
  const static int32_t m_maxPorts = 8;
  static void* m_ports[m_maxModules][m_maxPorts];
  uint32_t m_moduleNumber;  ///< Slot number where the module is plugged into
                            /// the chassis.
  static std::unique_ptr<Resource> m_allocated;
};
