/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef Compressor_H_
#define Compressor_H_

#include "HAL/HAL.hpp"
#include "SensorBase.h"
#include "tables/ITableListener.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

/**
 * PCM compressor
 */
class Compressor : public SensorBase,
                   public LiveWindowSendable,
                   public ITableListener {
 public:
  // Default PCM ID is 0
  explicit Compressor(uint8_t pcmID = GetDefaultSolenoidModule());
  virtual ~Compressor() = default;

  void Start();
  void Stop();
  bool Enabled() const;

  bool GetPressureSwitchValue() const;

  float GetCompressorCurrent() const;

  void SetClosedLoopControl(bool on);
  bool GetClosedLoopControl() const;

  bool GetCompressorCurrentTooHighFault() const;
  bool GetCompressorCurrentTooHighStickyFault() const;
  bool GetCompressorShortedStickyFault() const;
  bool GetCompressorShortedFault() const;
  bool GetCompressorNotConnectedStickyFault() const;
  bool GetCompressorNotConnectedFault() const;
  void ClearAllPCMStickyFaults();

  void UpdateTable() override;
  void StartLiveWindowMode() override;
  void StopLiveWindowMode() override;
  std::string GetSmartDashboardType() const override;
  void InitTable(std::shared_ptr<ITable> subTable) override;
  std::shared_ptr<ITable> GetTable() const override;
  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;

 protected:
  void *m_pcm_pointer;

 private:
  void SetCompressor(bool on);

  std::shared_ptr<ITable> m_table;
};

#endif /* Compressor_H_ */
