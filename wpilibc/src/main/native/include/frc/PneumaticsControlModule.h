// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>
#include <wpi/mutex.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "PneumaticsBase.h"

namespace frc {
class PneumaticsControlModule
    : public PneumaticsBase,
      public wpi::Sendable,
      public wpi::SendableHelper<PneumaticsControlModule> {
 public:
  PneumaticsControlModule();
  explicit PneumaticsControlModule(int module);

  ~PneumaticsControlModule() override;

  bool GetCompressor();

  void SetClosedLoopControl(bool enabled);

  bool GetClosedLoopControl();

  bool GetPressureSwitch();

  double GetCompressorCurrent();

  bool GetCompressorCurrentTooHighFault();
  bool GetCompressorCurrentTooHighStickyFault();
  bool GetCompressorShortedFault();
  bool GetCompressorShortedStickyFault();
  bool GetCompressorNotConnectedFault();
  bool GetCompressorNotConnectedStickyFault();

  bool GetSolenoidVoltageFault();
  bool GetSolenoidVoltageStickyFault();

  void ClearAllStickyFaults();

  void SetSolenoids(int mask, int values) override;

  int GetSolenoids() const override;

  int GetModuleNumber() const override;

  int GetSolenoidDisabledList() const override;

  void FireOneShot(int index) override;

  void SetOneShotDuration(int index, units::second_t duration) override;

  bool CheckSolenoidChannel(int channel) const override;

  int CheckAndReserveSolenoids(int mask) override;

  void UnreserveSolenoids(int mask) override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  int m_module;
  hal::Handle<HAL_CTREPCMHandle> m_handle;
  uint32_t m_reservedMask;
  wpi::mutex m_reservedLock;
};
}  // namespace frc
