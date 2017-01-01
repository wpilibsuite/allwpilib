/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "SensorBase.h"

namespace frc {

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public SensorBase {
 public:
  virtual ~SolenoidBase() = default;
  int GetAll(int module = 0) const;

  int GetPCMSolenoidBlackList(int module) const;
  bool GetPCMSolenoidVoltageStickyFault(int module) const;
  bool GetPCMSolenoidVoltageFault(int module) const;
  void ClearAllPCMStickyFaults(int module);

 protected:
  explicit SolenoidBase(int pcmID);
  static const int m_maxModules = 63;
  static const int m_maxPorts = 8;
  // static void* m_ports[m_maxModules][m_maxPorts];
  int m_moduleNumber;  ///< Slot number where the module is plugged into
                       /// the chassis.
};

}  // namespace frc
