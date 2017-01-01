/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "LiveWindow/LiveWindowSendable.h"
#include "simulation/SimContinuousOutput.h"
#include "tables/ITableListener.h"

namespace frc {

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output
 * (PCM).
 *
 * The DoubleSolenoid class is typically used for pneumatics solenoids that
 * have two positions controlled by two separate channels.
 */
class DoubleSolenoid : public LiveWindowSendable, public ITableListener {
 public:
  enum Value { kOff, kForward, kReverse };

  explicit DoubleSolenoid(int forwardChannel, int reverseChannel);
  DoubleSolenoid(int moduleNumber, int forwardChannel, int reverseChannel);
  virtual ~DoubleSolenoid();
  virtual void Set(Value value);
  virtual Value Get() const;

  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;

 private:
  SimContinuousOutput* m_impl;
  Value m_value;
  bool m_reversed;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
