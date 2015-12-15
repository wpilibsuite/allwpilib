/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "DigitalInput.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common initialization code for Encoders.
 * This code allocates resources for Encoders and is common to all constructors.
 *
 * The counter will start counting immediately.
 *
 * @param reverseDirection If true, counts down instead of up (this is all
 * relative)
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 * decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be
 * 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are
 * selected then
 * a counter object will be used and the returned value will either exactly
 * match the spec'd count
 * or be double (2x) the spec'd count.
 */
void Encoder::InitEncoder(bool reverseDirection, EncodingType encodingType) {
  m_encodingType = encodingType;
  switch (encodingType) {
    case k4X: {
      m_encodingScale = 4;
      if (m_aSource->StatusIsFatal()) {
        CloneError(*m_aSource);
        return;
      }
      if (m_bSource->StatusIsFatal()) {
        CloneError(*m_bSource);
        return;
      }
      int32_t status = 0;
      m_encoder = initializeEncoder(
          m_aSource->GetModuleForRouting(), m_aSource->GetChannelForRouting(),
          m_aSource->GetAnalogTriggerForRouting(),
          m_bSource->GetModuleForRouting(), m_bSource->GetChannelForRouting(),
          m_bSource->GetAnalogTriggerForRouting(), reverseDirection, &m_index,
          &status);
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
      m_counter = nullptr;
      SetMaxPeriod(.5);
      break;
    }
    case k1X:
    case k2X: {
      m_encodingScale = encodingType == k1X ? 1 : 2;
      m_counter = std::make_unique<Counter>(m_encodingType, m_aSource,
                                              m_bSource, reverseDirection);
      m_index = m_counter->GetFPGAIndex();
      break;
    }
    default:
      wpi_setErrorWithContext(-1, "Invalid encodingType argument");
      break;
  }

  HALReport(HALUsageReporting::kResourceType_Encoder, m_index, encodingType);
  LiveWindow::GetInstance()->AddSensor("Encoder",
                                       m_aSource->GetChannelForRouting(), this);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels.
 *
 * The counter will start counting immediately.
 *
 * @param aChannel The a channel DIO channel. 0-9 are on-board, 10-25 are on the
 * MXP port
 * @param bChannel The b channel DIO channel. 0-9 are on-board, 10-25 are on the
 * MXP port
 * @param reverseDirection represents the orientation of the encoder and inverts
 * the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 * decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be
 * 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are
 * selected then
 * a counter object will be used and the returned value will either exactly
 * match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(uint32_t aChannel, uint32_t bChannel, bool reverseDirection,
                 EncodingType encodingType) {
  m_aSource = std::make_shared<DigitalInput>(aChannel);
  m_bSource = std::make_shared<DigitalInput>(bChannel);
  InitEncoder(reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in
 * the case where the digital inputs are shared. The Encoder class will not
 * allocate the digital inputs and assume that they already are counted.
 * The counter will start counting immediately.
 *
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts
 * the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 * decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be
 * 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are
 * selected then
 * a counter object will be used and the returned value will either exactly
 * match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource *aSource, DigitalSource *bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(aSource, NullDeleter<DigitalSource>()),
      m_bSource(bSource, NullDeleter<DigitalSource>()) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(aSource), m_bSource(bSource) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 * Construct a Encoder given a and b channels as digital inputs. This is used in
 * the case
 * where the digital inputs are shared. The Encoder class will not allocate the
 * digital inputs
 * and assume that they already are counted.
 *
 * The counter will start counting immediately.
 *
 * @param aSource The source that should be used for the a channel.
 * @param bSource the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and inverts
 * the output values
 * if necessary so forward represents positive values.
 * @param encodingType either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 * decoding. If 4X is
 * selected, then an encoder FPGA object is used and the returned counts will be
 * 4x the encoder
 * spec'd value since all rising and falling edges are counted. If 1X or 2X are
 * selected then
 * a counter object will be used and the returned value will either exactly
 * match the spec'd count
 * or be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource &aSource, DigitalSource &bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(&aSource, NullDeleter<DigitalSource>()),
      m_bSource(&bSource, NullDeleter<DigitalSource>())
{
  InitEncoder(reverseDirection, encodingType);
}

/**
 * Free the resources for an Encoder.
 * Frees the FPGA resources associated with an Encoder.
 */
Encoder::~Encoder() {
  if (!m_counter) {
    int32_t status = 0;
    freeEncoder(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * The encoding scale factor 1x, 2x, or 4x, per the requested encodingType.
 * Used to divide raw edge counts down to spec'd counts.
 */
int32_t Encoder::GetEncodingScale() const { return m_encodingScale; }

/**
 * Gets the raw value from the encoder.
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current raw count from the encoder
 */
int32_t Encoder::GetRaw() const {
  if (StatusIsFatal()) return 0;
  int32_t value;
  if (m_counter)
    value = m_counter->Get();
  else {
    int32_t status = 0;
    value = getEncoder(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
  return value;
}

/**
 * Gets the current count.
 * Returns the current count on the Encoder.
 * This method compensates for the decoding type.
 *
 * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale
 * factor.
 */
int32_t Encoder::Get() const {
  if (StatusIsFatal()) return 0;
  return (int32_t)(GetRaw() * DecodingScaleFactor());
}

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void Encoder::Reset() {
  if (StatusIsFatal()) return;
  if (m_counter)
    m_counter->Reset();
  else {
    int32_t status = 0;
    resetEncoder(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compensates for the decoding type.
 *
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled
 * periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double Encoder::GetPeriod() const {
  if (StatusIsFatal()) return 0.0;
  if (m_counter) {
    return m_counter->GetPeriod() / DecodingScaleFactor();
  } else {
    int32_t status = 0;
    double period = getEncoderPeriod(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    return period;
  }
}

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it
 * will assume
 * that the attached device is stopped. This timeout allows users to determine
 * if the wheels or
 * other shaft has stopped rotating.
 * This method compensates for the decoding type.
 *
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled
 * periods and SetMinRate() scales using value from SetDistancePerPulse().
 *
 * @param maxPeriod The maximum time between rising and falling edges before the
 * FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void Encoder::SetMaxPeriod(double maxPeriod) {
  if (StatusIsFatal()) return;
  if (m_counter) {
    m_counter->SetMaxPeriod(maxPeriod * DecodingScaleFactor());
  } else {
    int32_t status = 0;
    setEncoderMaxPeriod(m_encoder, maxPeriod, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder
 * is considered
 * stopped and false if it is still moving. A stopped encoder is one where the
 * most recent pulse
 * width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
bool Encoder::GetStopped() const {
  if (StatusIsFatal()) return true;
  if (m_counter) {
    return m_counter->GetStopped();
  } else {
    int32_t status = 0;
    bool value = getEncoderStopped(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    return value;
  }
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool Encoder::GetDirection() const {
  if (StatusIsFatal()) return false;
  if (m_counter) {
    return m_counter->GetDirection();
  } else {
    int32_t status = 0;
    bool value = getEncoderDirection(m_encoder, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    return value;
  }
}

/**
 * The scale needed to convert a raw counter value into a number of encoder
 * pulses.
 */
double Encoder::DecodingScaleFactor() const {
  if (StatusIsFatal()) return 0.0;
  switch (m_encodingType) {
    case k1X:
      return 1.0;
    case k2X:
      return 0.5;
    case k4X:
      return 0.25;
    default:
      return 0.0;
  }
}

/**
 * Get the distance the robot has driven since the last reset.
 *
 * @return The distance driven since the last reset as scaled by the value from
 * SetDistancePerPulse().
 */
double Encoder::GetDistance() const {
  if (StatusIsFatal()) return 0.0;
  return GetRaw() * DecodingScaleFactor() * m_distancePerPulse;
}

/**
 * Get the current rate of the encoder.
 * Units are distance per second as scaled by the value from
 * SetDistancePerPulse().
 *
 * @return The current rate of the encoder.
 */
double Encoder::GetRate() const {
  if (StatusIsFatal()) return 0.0;
  return (m_distancePerPulse / GetPeriod());
}

/**
 * Set the minimum rate of the device before the hardware reports it stopped.
 *
 * @param minRate The minimum rate.  The units are in distance per second as
 * scaled by the value from SetDistancePerPulse().
 */
void Encoder::SetMinRate(double minRate) {
  if (StatusIsFatal()) return;
  SetMaxPeriod(m_distancePerPulse / minRate);
}

/**
 * Set the distance per pulse for this encoder.
 * This sets the multiplier used to determine the distance driven based on the
 * count value
 * from the encoder.
 * Do not include the decoding type in this scale.  The library already
 * compensates for the decoding type.
 * Set this value based on the encoder's rated Pulses per Revolution and
 * factor in gearing reductions following the encoder shaft.
 * This distance can be in any units you like, linear or angular.
 *
 * @param distancePerPulse The scale factor that will be used to convert pulses
 * to useful units.
 */
void Encoder::SetDistancePerPulse(double distancePerPulse) {
  if (StatusIsFatal()) return;
  m_distancePerPulse = distancePerPulse;
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the
 * correct
 * software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void Encoder::SetReverseDirection(bool reverseDirection) {
  if (StatusIsFatal()) return;
  if (m_counter) {
    m_counter->SetReverseDirection(reverseDirection);
  } else {
    int32_t status = 0;
    setEncoderReverseDirection(m_encoder, reverseDirection, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
  }
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to
 * average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void Encoder::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Average counter values must be between 1 and 127");
  }
  int32_t status = 0;
  switch (m_encodingType) {
    case k4X:
      setEncoderSamplesToAverage(m_encoder, samplesToAverage, &status);
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
      break;
    case k1X:
    case k2X:
      m_counter->SetSamplesToAverage(samplesToAverage);
      break;
  }
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to
 * average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int Encoder::GetSamplesToAverage() const {
  int result = 1;
  int32_t status = 0;
  switch (m_encodingType) {
    case k4X:
      result = getEncoderSamplesToAverage(m_encoder, &status);
      wpi_setErrorWithContext(status, getHALErrorMessage(status));
      break;
    case k1X:
    case k2X:
      result = m_counter->GetSamplesToAverage();
      break;
  }
  return result;
}

/**
 * Implement the PIDSource interface.
 *
 * @return The current value of the selected source parameter.
 */
double Encoder::PIDGet() {
  if (StatusIsFatal()) return 0.0;
  switch (GetPIDSourceType()) {
    case PIDSourceType::kDisplacement:
      return GetDistance();
    case PIDSourceType::kRate:
      return GetRate();
    default:
      return 0.0;
  }
}

/**
 * Set the index source for the encoder.  When this source is activated, the
 * encoder count automatically resets.
 *
 * @param channel A DIO channel to set as the encoder index
 * @param type The state that will cause the encoder to reset
 */
void Encoder::SetIndexSource(uint32_t channel, Encoder::IndexingType type) {
  int32_t status = 0;
  bool activeHigh = (type == kResetWhileHigh) || (type == kResetOnRisingEdge);
  bool edgeSensitive =
      (type == kResetOnFallingEdge) || (type == kResetOnRisingEdge);

  setEncoderIndexSource(m_encoder, channel, false, activeHigh, edgeSensitive,
                        &status);
  wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Set the index source for the encoder.  When this source is activated, the
 * encoder count automatically resets.
 *
 * @param channel A digital source to set as the encoder index
 * @param type The state that will cause the encoder to reset
 */
DEPRECATED("Use pass-by-reference instead.")
void Encoder::SetIndexSource(DigitalSource *source,
                             Encoder::IndexingType type) {
  SetIndexSource(*source, type);
}

/**
 * Set the index source for the encoder.  When this source is activated, the
 * encoder count automatically resets.
 *
 * @param channel A digital source to set as the encoder index
 * @param type The state that will cause the encoder to reset
 */
void Encoder::SetIndexSource(const DigitalSource &source,
                             Encoder::IndexingType type) {
  int32_t status = 0;
  bool activeHigh = (type == kResetWhileHigh) || (type == kResetOnRisingEdge);
  bool edgeSensitive =
      (type == kResetOnFallingEdge) || (type == kResetOnRisingEdge);

  setEncoderIndexSource(m_encoder, source.GetChannelForRouting(),
                        source.GetAnalogTriggerForRouting(), activeHigh,
                        edgeSensitive, &status);
  wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
}

void Encoder::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Speed", GetRate());
    m_table->PutNumber("Distance", GetDistance());
    m_table->PutNumber("Distance per Tick", m_distancePerPulse);
  }
}

void Encoder::StartLiveWindowMode() {}

void Encoder::StopLiveWindowMode() {}

std::string Encoder::GetSmartDashboardType() const {
  if (m_encodingType == k4X)
    return "Quadrature Encoder";
  else
    return "Encoder";
}

void Encoder::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Encoder::GetTable() const { return m_table; }
