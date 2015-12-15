/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "AnalogInput.h"
#include "Resource.h"
#include "Timer.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"
#include "HAL/Port.h"

#include <sstream>

static std::unique_ptr<Resource> inputs;

const uint8_t AnalogInput::kAccumulatorModuleNumber;
const uint32_t AnalogInput::kAccumulatorNumChannels;
const uint32_t AnalogInput::kAccumulatorChannels[] = {0, 1};

/**
 * Construct an analog input.
 *
 * @param channel The channel number on the roboRIO to represent. 0-3 are
 * on-board 4-7 are on the MXP port.
 */
AnalogInput::AnalogInput(uint32_t channel) {
  std::stringstream buf;
  buf << "Analog Input " << channel;
  Resource::CreateResourceObject(inputs, kAnalogInputs);

  if (!checkAnalogInputChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    return;
  }

  if (inputs->Allocate(channel, buf.str()) ==
      std::numeric_limits<uint32_t>::max()) {
    CloneError(*inputs);
    return;
  }

  m_channel = channel;

  void* port = getPort(channel);
  int32_t status = 0;
  m_port = initializeAnalogInputPort(port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  freePort(port);

  LiveWindow::GetInstance()->AddSensor("AnalogInput", channel, this);
  HALReport(HALUsageReporting::kResourceType_AnalogChannel, channel);
}

/**
 * Channel destructor.
 */
AnalogInput::~AnalogInput() {
  freeAnalogInputPort(m_port);
  inputs->Free(m_channel);
}

/**
 * Get a sample straight from this channel.
 * The sample is a 12-bit value representing the 0V to 5V range of the A/D
 * converter in the module.
 * The units are in A/D converter codes.  Use GetVoltage() to get the analog
 * value in calibrated units.
 * @return A sample straight from this channel.
 */
int16_t AnalogInput::GetValue() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int16_t value = getAnalogValue(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * Get a sample from the output of the oversample and average engine for this
 * channel.
 * The sample is 12-bit + the bits configured in SetOversampleBits().
 * The value configured in SetAverageBits() will cause this value to be averaged
 * 2**bits number of samples.
 * This is not a sliding window.  The sample will not change until
 * 2**(OversampleBits + AverageBits) samples
 * have been acquired from the module on this channel.
 * Use GetAverageVoltage() to get the analog value in calibrated units.
 * @return A sample from the oversample and average engine for this channel.
 */
int32_t AnalogInput::GetAverageValue() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int32_t value = getAnalogAverageValue(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * Get a scaled sample straight from this channel.
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 * @return A scaled sample straight from this channel.
 */
float AnalogInput::GetVoltage() const {
  if (StatusIsFatal()) return 0.0f;
  int32_t status = 0;
  float voltage = getAnalogVoltage(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return voltage;
}

/**
 * Get a scaled sample from the output of the oversample and average engine for
 * this channel.
 * The value is scaled to units of Volts using the calibrated scaling data from
 * GetLSBWeight() and GetOffset().
 * Using oversampling will cause this value to be higher resolution, but it will
 * update more slowly.
 * Using averaging will cause this value to be more stable, but it will update
 * more slowly.
 * @return A scaled sample from the output of the oversample and average engine
 * for this channel.
 */
float AnalogInput::GetAverageVoltage() const {
  if (StatusIsFatal()) return 0.0f;
  int32_t status = 0;
  float voltage = getAnalogAverageVoltage(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return voltage;
}

/**
 * Get the factory scaling least significant bit weight constant.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @return Least significant bit weight.
 */
uint32_t AnalogInput::GetLSBWeight() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int32_t lsbWeight = getAnalogLSBWeight(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return lsbWeight;
}

/**
 * Get the factory scaling offset constant.
 *
 * Volts = ((LSB_Weight * 1e-9) * raw) - (Offset * 1e-9)
 *
 * @return Offset constant.
 */
int32_t AnalogInput::GetOffset() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int32_t offset = getAnalogOffset(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return offset;
}

/**
 * Get the channel number.
 * @return The channel number.
 */
uint32_t AnalogInput::GetChannel() const {
  if (StatusIsFatal()) return 0;
  return m_channel;
}

/**
 * Set the number of averaging bits.
 * This sets the number of averaging bits. The actual number of averaged samples
 * is 2^bits.
 * Use averaging to improve the stability of your measurement at the expense of
 * sampling rate.
 * The averaging is done automatically in the FPGA.
 *
 * @param bits Number of bits of averaging.
 */
void AnalogInput::SetAverageBits(uint32_t bits) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  setAnalogAverageBits(m_port, bits, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of averaging bits previously configured.
 * This gets the number of averaging bits from the FPGA. The actual number of
 * averaged samples is 2^bits.
 * The averaging is done automatically in the FPGA.
 *
 * @return Number of bits of averaging previously configured.
 */
uint32_t AnalogInput::GetAverageBits() const {
  int32_t status = 0;
  int32_t averageBits = getAnalogAverageBits(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return averageBits;
}

/**
 * Set the number of oversample bits.
 * This sets the number of oversample bits. The actual number of oversampled
 * values is 2^bits.
 * Use oversampling to improve the resolution of your measurements at the
 * expense of sampling rate.
 * The oversampling is done automatically in the FPGA.
 *
 * @param bits Number of bits of oversampling.
 */
void AnalogInput::SetOversampleBits(uint32_t bits) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  setAnalogOversampleBits(m_port, bits, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the number of oversample bits previously configured.
 * This gets the number of oversample bits from the FPGA. The actual number of
 * oversampled values is
 * 2^bits. The oversampling is done automatically in the FPGA.
 *
 * @return Number of bits of oversampling previously configured.
 */
uint32_t AnalogInput::GetOversampleBits() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int32_t oversampleBits = getAnalogOversampleBits(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return oversampleBits;
}

/**
 * Is the channel attached to an accumulator.
 *
 * @return The analog input is attached to an accumulator.
 */
bool AnalogInput::IsAccumulatorChannel() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool isAccum = isAccumulatorChannel(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return isAccum;
}

/**
 * Initialize the accumulator.
 */
void AnalogInput::InitAccumulator() {
  if (StatusIsFatal()) return;
  m_accumulatorOffset = 0;
  int32_t status = 0;
  initAccumulator(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set an initial value for the accumulator.
 *
 * This will be added to all values returned to the user.
 * @param initialValue The value that the accumulator should start from when
 * reset.
 */
void AnalogInput::SetAccumulatorInitialValue(int64_t initialValue) {
  if (StatusIsFatal()) return;
  m_accumulatorOffset = initialValue;
}

/**
 * Resets the accumulator to the initial value.
 */
void AnalogInput::ResetAccumulator() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  resetAccumulator(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  if (!StatusIsFatal()) {
    // Wait until the next sample, so the next call to GetAccumulator*()
    // won't have old values.
    const float sampleTime = 1.0f / GetSampleRate();
    const float overSamples = 1 << GetOversampleBits();
    const float averageSamples = 1 << GetAverageBits();
    Wait(sampleTime * overSamples * averageSamples);
  }
}

/**
 * Set the center value of the accumulator.
 *
 * The center value is subtracted from each A/D value before it is added to the
 * accumulator. This
 * is used for the center value of devices like gyros and accelerometers to
 * take the device offset into account when integrating.
 *
 * This center value is based on the output of the oversampled and averaged
 * source from the accumulator
 * channel. Because of this, any non-zero oversample bits will affect the size
 * of the value for this field.
 */
void AnalogInput::SetAccumulatorCenter(int32_t center) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  setAccumulatorCenter(m_port, center, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the accumulator's deadband.
 * @param
 */
void AnalogInput::SetAccumulatorDeadband(int32_t deadband) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  setAccumulatorDeadband(m_port, deadband, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Read the accumulated value.
 *
 * Read the value that has been accumulating.
 * The accumulator is attached after the oversample and average engine.
 *
 * @return The 64-bit value accumulated since the last Reset().
 */
int64_t AnalogInput::GetAccumulatorValue() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int64_t value = getAccumulatorValue(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value + m_accumulatorOffset;
}

/**
 * Read the number of accumulated values.
 *
 * Read the count of the accumulated values since the accumulator was last
 * Reset().
 *
 * @return The number of times samples from the channel were accumulated.
 */
uint32_t AnalogInput::GetAccumulatorCount() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  uint32_t count = getAccumulatorCount(m_port, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return count;
}

/**
 * Read the accumulated value and the number of accumulated values atomically.
 *
 * This function reads the value and count from the FPGA atomically.
 * This can be used for averaging.
 *
 * @param value Reference to the 64-bit accumulated output.
 * @param count Reference to the number of accumulation cycles.
 */
void AnalogInput::GetAccumulatorOutput(int64_t &value, uint32_t &count) const {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  getAccumulatorOutput(m_port, &value, &count, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  value += m_accumulatorOffset;
}

/**
 * Set the sample rate per channel for all analog channels.
 * The maximum rate is 500kS/s divided by the number of channels in use.
 * This is 62500 samples/s per channel.
 * @param samplesPerSecond The number of samples per second.
 */
void AnalogInput::SetSampleRate(float samplesPerSecond) {
  int32_t status = 0;
  setAnalogSampleRate(samplesPerSecond, &status);
  wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the current sample rate for all channels
 *
 * @return Sample rate.
 */
float AnalogInput::GetSampleRate() {
  int32_t status = 0;
  float sampleRate = getAnalogSampleRate(&status);
  wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
  return sampleRate;
}

/**
 * Get the Average value for the PID Source base object.
 *
 * @return The average voltage.
 */
double AnalogInput::PIDGet() {
  if (StatusIsFatal()) return 0.0;
  return GetAverageVoltage();
}

void AnalogInput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", GetAverageVoltage());
  }
}

void AnalogInput::StartLiveWindowMode() {}

void AnalogInput::StopLiveWindowMode() {}

std::string AnalogInput::GetSmartDashboardType() const {
  return "Analog Input";
}

void AnalogInput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> AnalogInput::GetTable() const { return m_table; }
