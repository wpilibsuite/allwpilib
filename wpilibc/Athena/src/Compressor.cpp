/*
 * Compressor.cpp
 */

#include "Compressor.h"
#include "WPIErrors.h"

/**
 * Constructor
 *
 * @param module The PCM ID to use (0-62)
 */
Compressor::Compressor(uint8_t pcmID) {
  m_pcm_pointer = initializeCompressor(pcmID);
  SetClosedLoopControl(true);
}

/**
 *  Starts closed-loop control. Note that closed loop control is enabled by
 * default.
 */
void Compressor::Start() { SetClosedLoopControl(true); }

/**
 *  Stops closed-loop control. Note that closed loop control is enabled by
 * default.
 */
void Compressor::Stop() { SetClosedLoopControl(false); }

/**
 * Check if compressor output is active
 * @return true if the compressor is on
 */
bool Compressor::Enabled() const {
  int32_t status = 0;
  bool value;

  value = getCompressor(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Check if the pressure switch is triggered
 * @return true if pressure is low
 */
bool Compressor::GetPressureSwitchValue() const {
  int32_t status = 0;
  bool value;

  value = getPressureSwitch(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query how much current the compressor is drawing
 * @return The current through the compressor, in amps
 */
float Compressor::GetCompressorCurrent() const {
  int32_t status = 0;
  float value;

  value = getCompressorCurrent(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Enables or disables automatically turning the compressor on when the
 * pressure is low.
 * @param on Set to true to enable closed loop control of the compressor. False
 * to disable.
 */
void Compressor::SetClosedLoopControl(bool on) {
  int32_t status = 0;

  setClosedLoopControl(m_pcm_pointer, on, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }
}

/**
 * Returns true if the compressor will automatically turn on when the
 * pressure is low.
 * @return True if closed loop control of the compressor is enabled. False if
 * disabled.
 */
bool Compressor::GetClosedLoopControl() const {
  int32_t status = 0;
  bool value;

  value = getClosedLoopControl(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

/**
 * Query if the compressor output has been disabled due to high current draw.
 * @return true if PCM is in fault state : Compressor Drive is
 * 			disabled due to compressor current being too high.
 */
bool Compressor::GetCompressorCurrentTooHighFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorCurrentTooHighFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Query if the compressor output has been disabled due to high current draw
 * (sticky).
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 * @return true if PCM sticky fault is set : Compressor Drive is
 * 			disabled due to compressor current being too high.
 */
bool Compressor::GetCompressorCurrentTooHighStickyFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorCurrentTooHighStickyFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Query if the compressor output has been disabled due to a short circuit
 * (sticky).
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 * @return true if PCM sticky fault is set : Compressor output
 * 			appears to be shorted.
 */
bool Compressor::GetCompressorShortedStickyFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorShortedStickyFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Query if the compressor output has been disabled due to a short circuit.
 * @return true if PCM is in fault state : Compressor output
 * 			appears to be shorted.
 */
bool Compressor::GetCompressorShortedFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorShortedFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Query if the compressor output does not appear to be wired (sticky).
 * A sticky fault will not clear on device reboot, it must be cleared through
 * code or the webdash.
 * @return true if PCM sticky fault is set : Compressor does not
 * 			appear to be wired, i.e. compressor is
 * 			not drawing enough current.
 */
bool Compressor::GetCompressorNotConnectedStickyFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorNotConnectedStickyFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Query if the compressor output does not appear to be wired.
 * @return true if PCM is in fault state : Compressor does not
 * 			appear to be wired, i.e. compressor is
 * 			not drawing enough current.
 */
bool Compressor::GetCompressorNotConnectedFault() const {
  int32_t status = 0;
  bool value;

  value = getCompressorNotConnectedFault(m_pcm_pointer, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}
/**
 * Clear ALL sticky faults inside PCM that Compressor is wired to.
 *
 * If a sticky fault is set, then it will be persistently cleared.  Compressor
 * drive
 * 		maybe momentarily disable while flags are being cleared. Care
 * should be
 * 		taken to not call this too frequently, otherwise normal
 * compressor
 * 		functionality may be prevented.
 *
 * If no sticky faults are set then this call will have no effect.
 */
void Compressor::ClearAllPCMStickyFaults() {
  int32_t status = 0;

  clearAllPCMStickyFaults(m_pcm_pointer, &status);

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
