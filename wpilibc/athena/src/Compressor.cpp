/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Compressor.h"
#include "HAL/Compressor.h"

#include "HAL/HAL.h"
#include "HAL/Ports.h"
#include "HAL/Solenoid.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor.
 *
 * @param module The PCM ID to use (0-62)
 */
Compressor::Compressor(int pcmID) : m_module(pcmID) {
  int32_t status = 0;
  m_compressorHandle = HAL_InitializeCompressor(m_module, &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumPCMModules(), pcmID,
                                 HAL_GetErrorMessage(status));
    return;
  }
  SetClosedLoopControl(true);
}

/**
 * Starts closed-loop control. Note that closed loop control is enabled by
 * default.
 */
void Compressor::Start() {
  if (StatusIsFatal()) return;
  SetClosedLoopControl(true);
}

/**
 * Stops closed-loop control. Note that closed loop control is enabled by
 * default.
 */
void Compressor::Stop() {
  if (StatusIsFatal()) return;
  SetClosedLoopControl(false);
}

/**
 * Check if compressor output is active.
 *
 * @return true if the compressor is on
 */
bool Compressor::Enabled() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressor(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Check if the pressure switch is triggered.
 *
 * @return true if pressure is low
 */
bool Compressor::GetPressureSwitchValue() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorPressureSwitch(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query how much current the compressor is drawing.
 *
 * @return The current through the compressor, in amps
 */
double Compressor::GetCompressorCurrent() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  double value;

  value = HAL_GetCompressorCurrent(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Enables or disables automatically turning the compressor on when the
 * pressure is low.
 *
 * @param on Set to true to enable closed loop control of the compressor. False
 *           to disable.
 */
void Compressor::SetClosedLoopControl(bool on) {
  if (StatusIsFatal()) return;
  int32_t status = 0;

  HAL_SetCompressorClosedLoopControl(m_compressorHandle, on, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }
}

/**
 * Returns true if the compressor will automatically turn on when the
 * pressure is low.
 *
 * @return True if closed loop control of the compressor is enabled. False if
 *         disabled.
 */
bool Compressor::GetClosedLoopControl() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorClosedLoopControl(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output has been disabled due to high current draw.
 *
 * @return true if PCM is in fault state : Compressor Drive is
 *         disabled due to compressor current being too high.
 */
bool Compressor::GetCompressorCurrentTooHighFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorCurrentTooHighFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output has been disabled due to high current draw
 * (sticky).
 *
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 *
 * @return true if PCM sticky fault is set : Compressor Drive is
 *         disabled due to compressor current being too high.
 */
bool Compressor::GetCompressorCurrentTooHighStickyFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value =
      HAL_GetCompressorCurrentTooHighStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output has been disabled due to a short circuit
 * (sticky).
 *
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 *
 * @return true if PCM sticky fault is set : Compressor output
 *         appears to be shorted.
 */
bool Compressor::GetCompressorShortedStickyFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorShortedStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output has been disabled due to a short circuit.
 *
 * @return true if PCM is in fault state : Compressor output
 *         appears to be shorted.
 */
bool Compressor::GetCompressorShortedFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorShortedFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output does not appear to be wired (sticky).
 *
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 *
 * @return true if PCM sticky fault is set : Compressor does not
 *         appear to be wired, i.e. compressor is not drawing enough current.
 */
bool Compressor::GetCompressorNotConnectedStickyFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorNotConnectedStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output does not appear to be wired.
 *
 * @return true if PCM is in fault state : Compressor does not
 *         appear to be wired, i.e. compressor is not drawing enough current.
 */
bool Compressor::GetCompressorNotConnectedFault() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorNotConnectedFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

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
void Compressor::ClearAllPCMStickyFaults() {
  if (StatusIsFatal()) return;
  int32_t status = 0;

  HAL_ClearAllPCMStickyFaults(m_module, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }
}

void Compressor::UpdateTable() {
  if (m_table) {
    m_table->PutBoolean("Enabled", Enabled());
    m_table->PutBoolean("Pressure switch", GetPressureSwitchValue());
  }
}

void Compressor::StartLiveWindowMode() {}

void Compressor::StopLiveWindowMode() {}

std::string Compressor::GetSmartDashboardType() const { return "Compressor"; }

void Compressor::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Compressor::GetTable() const { return m_table; }

void Compressor::ValueChanged(ITable* source, llvm::StringRef key,
                              std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsBoolean()) return;
  if (value->GetBoolean())
    Start();
  else
    Stop();
}
