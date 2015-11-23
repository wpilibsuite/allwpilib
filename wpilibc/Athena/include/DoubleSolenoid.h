/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SolenoidBase.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

#include <memory>

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output
 * (PCM).
 *
 * The DoubleSolenoid class is typically used for pneumatics solenoids that
 * have two positions controlled by two separate channels.
 */
class DoubleSolenoid : public SolenoidBase,
                       public LiveWindowSendable,
                       public ITableListener {
 public:
  enum Value { kOff, kForward, kReverse };

  explicit DoubleSolenoid(uint32_t forwardChannel, uint32_t reverseChannel);
  DoubleSolenoid(uint8_t moduleNumber, uint32_t forwardChannel,
                 uint32_t reverseChannel);
  virtual ~DoubleSolenoid();
  virtual void Set(Value value);
  virtual Value Get() const;
  bool IsFwdSolenoidBlackListed() const;
  bool IsRevSolenoidBlackListed() const;

  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew);
  void UpdateTable();
  void StartLiveWindowMode();
  void StopLiveWindowMode();
  std::string GetSmartDashboardType() const;
  void InitTable(std::shared_ptr<ITable> subTable);
  std::shared_ptr<ITable> GetTable() const;

 private:
  uint32_t m_forwardChannel;  ///< The forward channel on the module to control.
  uint32_t m_reverseChannel;  ///< The reverse channel on the module to control.
  uint8_t m_forwardMask;      ///< The mask for the forward channel.
  uint8_t m_reverseMask;      ///< The mask for the reverse channel.

  std::shared_ptr<ITable> m_table;
};
