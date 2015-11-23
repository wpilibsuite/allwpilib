/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "DigitalSource.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

#include <memory>

/**
 * Class to write to digital outputs.
 * Write values to the digital output channels. Other devices implemented
 * elsewhere will allocate
 * channels automatically so for those devices it shouldn't be done here.
 */
class DigitalOutput : public DigitalSource,
                      public ITableListener,
                      public LiveWindowSendable {
 public:
  explicit DigitalOutput(uint32_t channel);
  virtual ~DigitalOutput();
  void Set(uint32_t value);
  uint32_t GetChannel() const;
  void Pulse(float length);
  bool IsPulsing() const;
  void SetPWMRate(float rate);
  void EnablePWM(float initialDutyCycle);
  void DisablePWM();
  void UpdateDutyCycle(float dutyCycle);

  // Digital Source Interface
  virtual uint32_t GetChannelForRouting() const;
  virtual uint32_t GetModuleForRouting() const;
  virtual bool GetAnalogTriggerForRouting() const;

  virtual void ValueChanged(ITable* source, llvm::StringRef key,
                            std::shared_ptr<nt::Value> value, bool isNew);
  void UpdateTable();
  void StartLiveWindowMode();
  void StopLiveWindowMode();
  std::string GetSmartDashboardType() const;
  void InitTable(std::shared_ptr<ITable> subTable);
  std::shared_ptr<ITable> GetTable() const;

 private:
  uint32_t m_channel;
  void *m_pwmGenerator;

  std::shared_ptr<ITable> m_table;
};
