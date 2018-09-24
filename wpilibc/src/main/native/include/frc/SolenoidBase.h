/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * SolenoidBase class is the common base class for the Solenoid and
 * DoubleSolenoid classes.
 */
class SolenoidBase : public ErrorBase, public SendableBase {
 public:
  /**
   * Read all 8 solenoids as a single byte
   *
   * @param module the module to read from
   * @return The current value of all 8 solenoids on the module.
   */
  static int GetAll(int module);

  /**
   * Read all 8 solenoids as a single byte
   *
   * @return The current value of all 8 solenoids on the module.
   */
  int GetAll() const;

  /**
   * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
   *
   * If a solenoid is shorted, it is added to the blacklist and
   * disabled until power cycle, or until faults are cleared.
   * @see ClearAllPCMStickyFaults()
   *
   * @param module the module to read from
   * @return The solenoid blacklist of all 8 solenoids on the module.
   */
  static int GetPCMSolenoidBlackList(int module);

  /**
   * Reads complete solenoid blacklist for all 8 solenoids as a single byte.
   *
   * If a solenoid is shorted, it is added to the blacklist and
   * disabled until power cycle, or until faults are cleared.
   * @see ClearAllPCMStickyFaults()
   *
   * @return The solenoid blacklist of all 8 solenoids on the module.
   */
  int GetPCMSolenoidBlackList() const;

  /**
   * @param module the module to read from
   * @return true if PCM sticky fault is set : The common highside solenoid
   *         voltage rail is too low, most likely a solenoid channel is shorted.
   */
  static bool GetPCMSolenoidVoltageStickyFault(int module);

  /**
   * @return true if PCM sticky fault is set : The common highside solenoid
   *         voltage rail is too low, most likely a solenoid channel is shorted.
   */
  bool GetPCMSolenoidVoltageStickyFault() const;

  /**
   * @param module the module to read from
   * @return true if PCM is in fault state : The common highside solenoid
   *         voltage rail is too low, most likely a solenoid channel is shorted.
   */
  static bool GetPCMSolenoidVoltageFault(int module);

  /**
   * @return true if PCM is in fault state : The common highside solenoid
   *         voltage rail is too low, most likely a solenoid channel is shorted.
   */
  bool GetPCMSolenoidVoltageFault() const;

  /**
   * Clear ALL sticky faults inside PCM that Compressor is wired to.
   *
   * If a sticky fault is set, then it will be persistently cleared.  Compressor
   * drive maybe momentarily disable while flags are being cleared. Care should
   * be taken to not call this too frequently, otherwise normal compressor
   * functionality may be prevented.
   *
   * If no sticky faults are set then this call will have no effect.
   *
   * @param module the module to read from
   */
  static void ClearAllPCMStickyFaults(int module);

  /**
   * Clear ALL sticky faults inside PCM that Compressor is wired to.
   *
   * If a sticky fault is set, then it will be persistently cleared.  Compressor
   * drive maybe momentarily disable while flags are being cleared. Care should
   * be taken to not call this too frequently, otherwise normal compressor
   * functionality may be prevented.
   *
   * If no sticky faults are set then this call will have no effect.
   */
  void ClearAllPCMStickyFaults();

 protected:
  /**
   * Constructor.
   *
   * @param moduleNumber The CAN PCM ID.
   */
  explicit SolenoidBase(int pcmID);

  SolenoidBase(SolenoidBase&&) = default;
  SolenoidBase& operator=(SolenoidBase&&) = default;

  static constexpr int m_maxModules = 63;
  static constexpr int m_maxPorts = 8;

  int m_moduleNumber;  // PCM module number
};

}  // namespace frc
