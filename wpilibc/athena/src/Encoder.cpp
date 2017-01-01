/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"

#include "DigitalInput.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Common initialization code for Encoders.
 *
 * This code allocates resources for Encoders and is common to all constructors.
 *
 * The counter will start counting immediately.
 *
 * @param reverseDirection If true, counts down instead of up (this is all
 *                         relative)
 * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 *                         decoding. If 4X is selected, then an encoder FPGA
 *                         object is used and the returned counts will be 4x
 *                         the encoder spec'd value since all rising and
 *                         falling edges are counted. If 1X or 2X are selected
 *                         then a counter object will be used and the returned
 *                         value will either exactly match the spec'd count or
 *                         be double (2x) the spec'd count.
 */
void Encoder::InitEncoder(bool reverseDirection, EncodingType encodingType) {
  int32_t status = 0;
  m_encoder = HAL_InitializeEncoder(
      m_aSource->GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)m_aSource->GetAnalogTriggerTypeForRouting(),
      m_bSource->GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)m_bSource->GetAnalogTriggerTypeForRouting(),
      reverseDirection, (HAL_EncoderEncodingType)encodingType, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  HAL_Report(HALUsageReporting::kResourceType_Encoder, GetFPGAIndex(),
             encodingType);
  LiveWindow::GetInstance()->AddSensor("Encoder", m_aSource->GetChannel(),
                                       this);
}

/**
 * Encoder constructor.
 *
 * Construct a Encoder given a and b channels.
 *
 * The counter will start counting immediately.
 *
 * @param aChannel         The a channel DIO channel. 0-9 are on-board, 10-25
 *                         are on the MXP port
 * @param bChannel         The b channel DIO channel. 0-9 are on-board, 10-25
 *                         are on the MXP port
 * @param reverseDirection represents the orientation of the encoder and
 *                         inverts the output values if necessary so forward
 *                         represents positive values.
 * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 *                         decoding. If 4X is selected, then an encoder FPGA
 *                         object is used and the returned counts will be 4x
 *                         the encoder spec'd value since all rising and
 *                         falling edges are counted. If 1X or 2X are selected
 *                         then a counter object will be used and the returned
 *                         value will either exactly match the spec'd count or
 *                         be double (2x) the spec'd count.
 */
Encoder::Encoder(int aChannel, int bChannel, bool reverseDirection,
                 EncodingType encodingType) {
  m_aSource = std::make_shared<DigitalInput>(aChannel);
  m_bSource = std::make_shared<DigitalInput>(bChannel);
  InitEncoder(reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 *
 * Construct a Encoder given a and b channels as digital inputs. This is used in
 * the case where the digital inputs are shared. The Encoder class will not
 * allocate the digital inputs and assume that they already are counted.
 *
 * The counter will start counting immediately.
 *
 * @param aSource          The source that should be used for the a channel.
 * @param bSource          the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and
 *                         inverts the output values if necessary so forward
 *                         represents positive values.
 * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 *                         decoding. If 4X is selected, then an encoder FPGA
 *                         object is used and the returned counts will be 4x
 *                         the encoder spec'd value since all rising and
 *                         falling edges are counted. If 1X or 2X are selected
 *                         then a counter object will be used and the returned
 *                         value will either exactly match the spec'd count or
 *                         be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource* aSource, DigitalSource* bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(aSource, NullDeleter<DigitalSource>()),
      m_bSource(bSource, NullDeleter<DigitalSource>()) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

Encoder::Encoder(std::shared_ptr<DigitalSource> aSource,
                 std::shared_ptr<DigitalSource> bSource, bool reverseDirection,
                 EncodingType encodingType)
    : m_aSource(aSource), m_bSource(bSource) {
  if (m_aSource == nullptr || m_bSource == nullptr)
    wpi_setWPIError(NullParameter);
  else
    InitEncoder(reverseDirection, encodingType);
}

/**
 * Encoder constructor.
 *
 * Construct a Encoder given a and b channels as digital inputs. This is used in
 * the case where the digital inputs are shared. The Encoder class will not
 * allocate the digital inputs and assume that they already are counted.
 *
 * The counter will start counting immediately.
 *
 * @param aSource          The source that should be used for the a channel.
 * @param bSource          the source that should be used for the b channel.
 * @param reverseDirection represents the orientation of the encoder and
 *                         inverts the output values if necessary so forward
 *                         represents positive values.
 * @param encodingType     either k1X, k2X, or k4X to indicate 1X, 2X or 4X
 *                         decoding. If 4X is selected, then an encoder FPGA
 *                         object is used and the returned counts will be 4x
 *                         the encoder spec'd value since all rising and
 *                         falling edges are counted. If 1X or 2X are selected
 *                         then a counter object will be used and the returned
 *                         value will either exactly match the spec'd count or
 *                         be double (2x) the spec'd count.
 */
Encoder::Encoder(DigitalSource& aSource, DigitalSource& bSource,
                 bool reverseDirection, EncodingType encodingType)
    : m_aSource(&aSource, NullDeleter<DigitalSource>()),
      m_bSource(&bSource, NullDeleter<DigitalSource>()) {
  InitEncoder(reverseDirection, encodingType);
}

/**
 * Free the resources for an Encoder.
 *
 * Frees the FPGA resources associated with an Encoder.
 */
Encoder::~Encoder() {
  int32_t status = 0;
  HAL_FreeEncoder(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * The encoding scale factor 1x, 2x, or 4x, per the requested encodingType.
 *
 * Used to divide raw edge counts down to spec'd counts.
 */
int Encoder::GetEncodingScale() const {
  int32_t status = 0;
  int val = HAL_GetEncoderEncodingScale(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

/**
 * Gets the raw value from the encoder.
 *
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 *
 * @return Current raw count from the encoder
 */
int Encoder::GetRaw() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetEncoderRaw(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Gets the current count.
 *
 * Returns the current count on the Encoder. This method compensates for the
 * decoding type.
 *
 * @return Current count from the Encoder adjusted for the 1x, 2x, or 4x scale
 *         factor.
 */
int Encoder::Get() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetEncoder(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Reset the Encoder distance to zero.
 *
 * Resets the current count to zero on the encoder.
 */
void Encoder::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetEncoder(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Returns the period of the most recent pulse.
 *
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compensates for the decoding type.
 *
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled
 *             periods and GetRate() scales using value from
 *             SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double Encoder::GetPeriod() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderPeriod(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Sets the maximum period for stopped detection.
 *
 * Sets the value that represents the maximum period of the Encoder before it
 * will assume that the attached device is stopped. This timeout allows users
 * to determine if the wheels or other shaft has stopped rotating.
 * This method compensates for the decoding type.
 *
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled
 *             periods and SetMinRate() scales using value from
 *             SetDistancePerPulse().
 *
 * @param maxPeriod The maximum time between rising and falling edges before
 *                  the FPGA will report the device stopped. This is expressed
 *                  in seconds.
 */
void Encoder::SetMaxPeriod(double maxPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderMaxPeriod(m_encoder, maxPeriod, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Determine if the encoder is stopped.
 *
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder
 * is considered stopped and false if it is still moving. A stopped encoder is
 * one where the most recent pulse width exceeds the MaxPeriod.
 *
 * @return True if the encoder is considered stopped.
 */
bool Encoder::GetStopped() const {
  if (StatusIsFatal()) return true;
  int32_t status = 0;
  bool value = HAL_GetEncoderStopped(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * The last direction the encoder value changed.
 *
 * @return The last direction the encoder value changed.
 */
bool Encoder::GetDirection() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetEncoderDirection(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * The scale needed to convert a raw counter value into a number of encoder
 * pulses.
 */
double Encoder::DecodingScaleFactor() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double val = HAL_GetEncoderDecodingScaleFactor(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

/**
 * Get the distance the robot has driven since the last reset.
 *
 * @return The distance driven since the last reset as scaled by the value from
 *         SetDistancePerPulse().
 */
double Encoder::GetDistance() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderDistance(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Get the current rate of the encoder.
 *
 * Units are distance per second as scaled by the value from
 * SetDistancePerPulse().
 *
 * @return The current rate of the encoder.
 */
double Encoder::GetRate() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetEncoderRate(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Set the minimum rate of the device before the hardware reports it stopped.
 *
 * @param minRate The minimum rate.  The units are in distance per second as
 *                scaled by the value from SetDistancePerPulse().
 */
void Encoder::SetMinRate(double minRate) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderMinRate(m_encoder, minRate, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the distance per pulse for this encoder.
 *
 * This sets the multiplier used to determine the distance driven based on the
 * count value from the encoder.
 *
 * Do not include the decoding type in this scale.  The library already
 * compensates for the decoding type.
 *
 * Set this value based on the encoder's rated Pulses per Revolution and
 * factor in gearing reductions following the encoder shaft.
 *
 * This distance can be in any units you like, linear or angular.
 *
 * @param distancePerPulse The scale factor that will be used to convert pulses
 *                         to useful units.
 */
void Encoder::SetDistancePerPulse(double distancePerPulse) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderDistancePerPulse(m_encoder, distancePerPulse, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the direction sensing for this encoder.
 *
 * This sets the direction sensing on the encoder so that it could count in the
 * correct software direction regardless of the mounting.
 *
 * @param reverseDirection true if the encoder direction should be reversed
 */
void Encoder::SetReverseDirection(bool reverseDirection) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetEncoderReverseDirection(m_encoder, reverseDirection, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period.
 *
 * Perform averaging to account for mechanical imperfections or as oversampling
 * to increase resolution.
 *
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void Encoder::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Average counter values must be between 1 and 127");
    return;
  }
  int32_t status = 0;
  HAL_SetEncoderSamplesToAverage(m_encoder, samplesToAverage, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period.
 *
 * Perform averaging to account for mechanical imperfections or as oversampling
 * to increase resolution.
 *
 * @return The number of samples being averaged (from 1 to 127)
 */
int Encoder::GetSamplesToAverage() const {
  int32_t status = 0;
  int result = HAL_GetEncoderSamplesToAverage(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
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
 * Set the index source for the encoder.
 *
 * When this source is activated, the encoder count automatically resets.
 *
 * @param channel A DIO channel to set as the encoder index
 * @param type    The state that will cause the encoder to reset
 */
void Encoder::SetIndexSource(int channel, Encoder::IndexingType type) {
  // Force digital input if just given an index
  m_indexSource = std::make_unique<DigitalInput>(channel);
  SetIndexSource(m_indexSource.get(), type);
}

/**
 * Set the index source for the encoder.
 *
 * When this source is activated, the encoder count automatically resets.
 *
 * @param channel A digital source to set as the encoder index
 * @param type    The state that will cause the encoder to reset
 */
WPI_DEPRECATED("Use pass-by-reference instead.")
void Encoder::SetIndexSource(DigitalSource* source,
                             Encoder::IndexingType type) {
  SetIndexSource(*source, type);
}

/**
 * Set the index source for the encoder.
 *
 * When this source is activated, the encoder count automatically resets.
 *
 * @param channel A digital source to set as the encoder index
 * @param type    The state that will cause the encoder to reset
 */
void Encoder::SetIndexSource(const DigitalSource& source,
                             Encoder::IndexingType type) {
  int32_t status = 0;
  HAL_SetEncoderIndexSource(
      m_encoder, source.GetPortHandleForRouting(),
      (HAL_AnalogTriggerType)source.GetAnalogTriggerTypeForRouting(),
      (HAL_EncoderIndexingType)type, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int Encoder::GetFPGAIndex() const {
  int32_t status = 0;
  int val = HAL_GetEncoderFPGAIndex(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return val;
}

void Encoder::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Speed", GetRate());
    m_table->PutNumber("Distance", GetDistance());
    int32_t status = 0;
    double distancePerPulse =
        HAL_GetEncoderDistancePerPulse(m_encoder, &status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_table->PutNumber("Distance per Tick", distancePerPulse);
  }
}

void Encoder::StartLiveWindowMode() {}

void Encoder::StopLiveWindowMode() {}

std::string Encoder::GetSmartDashboardType() const {
  int32_t status = 0;
  HAL_EncoderEncodingType type = HAL_GetEncoderEncodingType(m_encoder, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  if (type == HAL_EncoderEncodingType::HAL_Encoder_k4X)
    return "Quadrature Encoder";
  else
    return "Encoder";
}

void Encoder::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Encoder::GetTable() const { return m_table; }
