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
 * Solenoid class for running high voltage Digital Output (PCM).
 *
 * The Solenoid class is typically used for pneumatics solenoids, but could be
 * used
 * for any device within the current spec of the PCM.
 */
class Solenoid : public SolenoidBase,
                 public LiveWindowSendable,
                 public ITableListener {
 public:
  explicit Solenoid(uint32_t channel);
  Solenoid(uint8_t moduleNumber, uint32_t channel);
  virtual ~Solenoid();
  virtual void Set(bool on);
  virtual bool Get() const;
  bool IsBlackListed() const;

  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew);
  void UpdateTable();
  void StartLiveWindowMode();
  void StopLiveWindowMode();
  std::string GetSmartDashboardType() const;
  void InitTable(std::shared_ptr<ITable> subTable);
  std::shared_ptr<ITable> GetTable() const;

 private:
  uint32_t m_channel;  ///< The channel on the module to control.
  std::shared_ptr<ITable> m_table;
};
