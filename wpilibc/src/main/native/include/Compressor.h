/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <HAL/Types.h>

#include "LiveWindow/LiveWindowSendable.h"
#include "SensorBase.h"
#include "tables/ITableListener.h"

namespace frc {

/**
 * Class for operating a compressor connected to a %PCM (Pneumatic Control
 * Module). The %PCM will automatically run in closed loop mode by default
 * whenever a Solenoid object is created. For most cases, a Compressor object
 * does not need to be instantiated or used in a robot program. This class is
 * only required in cases where the robot program needs a more detailed status
 * of the compressor or to enable/disable closed loop control.
 *
 * Note: you cannot operate the compressor directly from this class as doing so
 * would circumvent the safety provided by using the pressure switch and closed
 * loop control. You can only turn off closed loop control, thereby stopping
 * the compressor from operating.
 */
class Compressor : public SensorBase,
                   public LiveWindowSendable,
                   public ITableListener {
 public:
  // Default PCM ID is 0
  explicit Compressor(int pcmID = GetDefaultSolenoidModule());
  virtual ~Compressor() = default;

  void Start();
  void Stop();
  bool Enabled() const;

  bool GetPressureSwitchValue() const;

  double GetCompressorCurrent() const;

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
  HAL_CompressorHandle m_compressorHandle;

 private:
  void SetCompressor(bool on);
  int m_module;

  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
