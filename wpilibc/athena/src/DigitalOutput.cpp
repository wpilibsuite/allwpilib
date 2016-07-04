/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalOutput.h"

#include "HAL/HAL.h"
#include "WPIErrors.h"

#include <limits>
#include <sstream>

/**
 * Create an instance of a digital output.
 *
 * Create a digital output given a channel.
 *
 * @param channel The digital channel 0-9 are on-board, 10-25 are on the MXP
 *                port
 */
DigitalOutput::DigitalOutput(uint32_t channel) {
  std::stringstream buf;

  m_pwmGenerator = HAL_INVALID_HANDLE;
  if (!CheckDigitalChannel(channel)) {
    buf << "Digital Channel " << channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    m_channel = std::numeric_limits<uint32_t>::max();
    return;
  }
  m_channel = channel;

  int32_t status = 0;
  m_handle = initializeDIOPort(getPort(channel), false, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    m_channel = std::numeric_limits<uint32_t>::max();
    m_handle = HAL_INVALID_HANDLE;
    return;
  }

  HALReport(HALUsageReporting::kResourceType_DigitalOutput, channel);
}

/**
 * Free the resources associated with a digital output.
 */
DigitalOutput::~DigitalOutput() {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
  if (StatusIsFatal()) return;
  // Disable the PWM in case it was running.
  DisablePWM();

  freeDIOPort(m_handle);
}

/**
 * Set the value of a digital output.
 *
 * Set the value of a digital output to either one (true) or zero (false).
 *
 * @param value 1 (true) for high, 0 (false) for disabled
 */
void DigitalOutput::Set(uint32_t value) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  setDIO(m_handle, value, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * @return The GPIO channel number that this object represents.
 */
uint32_t DigitalOutput::GetChannel() const { return m_channel; }

/**
 * Output a single pulse on the digital output line.
 *
 * Send a single pulse on the digital output line where the pulse duration is
 * specified in seconds. Maximum pulse length is 0.0016 seconds.
 *
 * @param length The pulselength in seconds
 */
void DigitalOutput::Pulse(float length) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  pulse(m_handle, length, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Determine if the pulse is still going.
 *
 * Determine if a previously started pulse is still going.
 */
bool DigitalOutput::IsPulsing() const {
  if (StatusIsFatal()) return false;

  int32_t status = 0;
  bool value = isPulsing(m_handle, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * Change the PWM frequency of the PWM output on a Digital Output line.
 *
 * The valid range is from 0.6 Hz to 19 kHz.  The frequency resolution is
 * logarithmic.
 *
 * There is only one PWM frequency for all digital channels.
 *
 * @param rate The frequency to output all digital output PWM signals.
 */
void DigitalOutput::SetPWMRate(float rate) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  setDigitalPWMRate(rate, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Enable a PWM Output on this line.
 *
 * Allocate one of the 6 DO PWM generator resources from this module.
 *
 * Supply the initial duty-cycle to output so as to avoid a glitch when first
 * starting.
 *
 * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less)
 * but is reduced the higher the frequency of the PWM signal is.
 *
 * @param initialDutyCycle The duty-cycle to start generating. [0..1]
 */
void DigitalOutput::EnablePWM(float initialDutyCycle) {
  if (m_pwmGenerator != HAL_INVALID_HANDLE) return;

  int32_t status = 0;

  if (StatusIsFatal()) return;
  m_pwmGenerator = allocateDigitalPWM(&status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  if (StatusIsFatal()) return;
  setDigitalPWMDutyCycle(m_pwmGenerator, initialDutyCycle, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  if (StatusIsFatal()) return;
  setDigitalPWMOutputChannel(m_pwmGenerator, m_channel, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Change this line from a PWM output back to a static Digital Output line.
 *
 * Free up one of the 6 DO PWM generator resources that were in use.
 */
void DigitalOutput::DisablePWM() {
  if (StatusIsFatal()) return;
  if (m_pwmGenerator == HAL_INVALID_HANDLE) return;

  int32_t status = 0;

  // Disable the output by routing to a dead bit.
  setDigitalPWMOutputChannel(m_pwmGenerator, kDigitalChannels, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  freeDigitalPWM(m_pwmGenerator, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  m_pwmGenerator = HAL_INVALID_HANDLE;
}

/**
 * Change the duty-cycle that is being generated on the line.
 *
 * The resolution of the duty cycle is 8-bit for low frequencies (1kHz or less)
 * but is reduced the higher the frequency of the PWM signal is.
 *
 * @param dutyCycle The duty-cycle to change to. [0..1]
 */
void DigitalOutput::UpdateDutyCycle(float dutyCycle) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  setDigitalPWMDutyCycle(m_pwmGenerator, dutyCycle, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * @return The value to be written to the channel field of a routing mux.
 */
uint32_t DigitalOutput::GetChannelForRouting() const { return GetChannel(); }

/**
 * @return The value to be written to the module field of a routing mux.
 */
uint32_t DigitalOutput::GetModuleForRouting() const { return 0; }

/**
 * @return The value to be written to the analog trigger field of a routing mux.
 */
bool DigitalOutput::GetAnalogTriggerForRouting() const { return false; }

/**
 * @return The HAL Handle to the specified source.
 */
HalHandle DigitalOutput::GetPortHandle() const { return m_handle; }

void DigitalOutput::ValueChanged(ITable* source, llvm::StringRef key,
                                 std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsBoolean()) return;
  Set(value->GetBoolean());
}

void DigitalOutput::UpdateTable() {}

void DigitalOutput::StartLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->AddTableListener("Value", this, true);
  }
}

void DigitalOutput::StopLiveWindowMode() {
  if (m_table != nullptr) {
    m_table->RemoveTableListener(this);
  }
}

std::string DigitalOutput::GetSmartDashboardType() const {
  return "Digital Output";
}

void DigitalOutput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> DigitalOutput::GetTable() const { return m_table; }
