/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/Types.h>

#include "ErrorBase.h"
#include "SensorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

/**
 * Class for operating a compressor connected to a %PCM (Pneumatic Control
 * Module).
 *
 * The PCM will automatically run in closed loop mode by default whenever a
 * Solenoid object is created. For most cases, a Compressor object does not need
 * to be instantiated or used in a robot program. This class is only required in
 * cases where the robot program needs a more detailed status of the compressor
 * or to enable/disable closed loop control.
 *
 * Note: you cannot operate the compressor directly from this class as doing so
 * would circumvent the safety provided by using the pressure switch and closed
 * loop control. You can only turn off closed loop control, thereby stopping
 * the compressor from operating.
 */
class Compressor : public ErrorBase, public SendableBase {
 public:
  // Default PCM ID is 0
  explicit Compressor(int pcmID = SensorBase::GetDefaultSolenoidModule());
  ~Compressor() override = default;

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

  void InitSendable(SendableBuilder& builder) override;

 protected:
  HAL_CompressorHandle m_compressorHandle;

 private:
  void SetCompressor(bool on);
  int m_module;
};

}  // namespace frc
