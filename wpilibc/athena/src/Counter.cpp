/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Counter.h"

#include "AnalogTrigger.h"
#include "DigitalInput.h"
#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Create an instance of a counter where no sources are selected.
 *
 * They all must be selected by calling functions to specify the upsource and
 * the downsource independently.
 *
 * This creates a ChipObject counter and initializes status variables
 * appropriately.
 *
 * The counter will start counting immediately.
 *
 * @param mode The counter mode
 */
Counter::Counter(Mode mode) {
  int32_t status = 0;
  m_counter = HAL_InitializeCounter((HAL_Counter_Mode)mode, &m_index, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  SetMaxPeriod(.5);

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index, mode);
}

/**
 * Create an instance of a counter from a Digital Source (such as a Digital
 * Input).
 *
 * This is used if an existing digital input is to be shared by multiple other
 * objects such as encoders or if the Digital Source is not a Digital Input
 * channel (such as an Analog Trigger).
 *
 * The counter will start counting immediately.
 * @param source A pointer to the existing DigitalSource object. It will be set
 *               as the Up Source.
 */
Counter::Counter(DigitalSource* source) : Counter(kTwoPulse) {
  SetUpSource(source);
  ClearDownSource();
}

/**
 * Create an instance of a counter from a Digital Source (such as a Digital
 * Input).
 *
 * This is used if an existing digital input is to be shared by multiple other
 * objects such as encoders or if the Digital Source is not a Digital Input
 * channel (such as an Analog Trigger).
 *
 * The counter will start counting immediately.
 *
 * @param source A pointer to the existing DigitalSource object. It will be
 *               set as the Up Source.
 */
Counter::Counter(std::shared_ptr<DigitalSource> source) : Counter(kTwoPulse) {
  SetUpSource(source);
  ClearDownSource();
}

/**
 * Create an instance of a Counter object.
 *
 * Create an up-Counter instance given a channel.
 *
 * The counter will start counting immediately.
 *
 * @param channel The DIO channel to use as the up source. 0-9 are on-board,
 *                10-25 are on the MXP
 */
Counter::Counter(int channel) : Counter(kTwoPulse) {
  SetUpSource(channel);
  ClearDownSource();
}

/**
 * Create an instance of a Counter object.
 *
 * Create an instance of a simple up-Counter given an analog trigger.
 * Use the trigger state output from the analog trigger.
 *
 * The counter will start counting immediately.
 *
 * @param trigger The pointer to the existing AnalogTrigger object.
 */
WPI_DEPRECATED("Use pass-by-reference instead.")
Counter::Counter(AnalogTrigger* trigger) : Counter(kTwoPulse) {
  SetUpSource(trigger->CreateOutput(AnalogTriggerType::kState));
  ClearDownSource();
}

/**
 * Create an instance of a Counter object.
 *
 * Create an instance of a simple up-Counter given an analog trigger.
 * Use the trigger state output from the analog trigger.
 *
 * The counter will start counting immediately.
 *
 * @param trigger The reference to the existing AnalogTrigger object.
 */
Counter::Counter(const AnalogTrigger& trigger) : Counter(kTwoPulse) {
  SetUpSource(trigger.CreateOutput(AnalogTriggerType::kState));
  ClearDownSource();
}

/**
 * Create an instance of a Counter object.
 *
 * Creates a full up-down counter given two Digital Sources.
 *
 * @param encodingType The quadrature decoding mode (1x or 2x)
 * @param upSource     The pointer to the DigitalSource to set as the up source
 * @param downSource   The pointer to the DigitalSource to set as the down
 *                     source
 * @param inverted     True to invert the output (reverse the direction)
 */
Counter::Counter(EncodingType encodingType, DigitalSource* upSource,
                 DigitalSource* downSource, bool inverted)
    : Counter(encodingType, std::shared_ptr<DigitalSource>(
                                upSource, NullDeleter<DigitalSource>()),
              std::shared_ptr<DigitalSource>(downSource,
                                             NullDeleter<DigitalSource>()),
              inverted) {}

/**
 * Create an instance of a Counter object.
 *
 * Creates a full up-down counter given two Digital Sources.
 *
 * @param encodingType The quadrature decoding mode (1x or 2x)
 * @param upSource     The pointer to the DigitalSource to set as the up source
 * @param downSource   The pointer to the DigitalSource to set as the down
 *                     source
 * @param inverted     True to invert the output (reverse the direction)
 */
Counter::Counter(EncodingType encodingType,
                 std::shared_ptr<DigitalSource> upSource,
                 std::shared_ptr<DigitalSource> downSource, bool inverted)
    : Counter(kExternalDirection) {
  if (encodingType != k1X && encodingType != k2X) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Counter only supports 1X and 2X quadrature decoding.");
    return;
  }
  SetUpSource(upSource);
  SetDownSource(downSource);
  int32_t status = 0;

  if (encodingType == k1X) {
    SetUpSourceEdge(true, false);
    HAL_SetCounterAverageSize(m_counter, 1, &status);
  } else {
    SetUpSourceEdge(true, true);
    HAL_SetCounterAverageSize(m_counter, 2, &status);
  }

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  SetDownSourceEdge(inverted, true);
}

/**
 * Delete the Counter object.
 */
Counter::~Counter() {
  SetUpdateWhenEmpty(true);

  int32_t status = 0;
  HAL_FreeCounter(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  m_counter = HAL_kInvalidHandle;
}

/**
 * Set the upsource for the counter as a digital input channel.
 *
 * @param channel The DIO channel to use as the up source. 0-9 are on-board,
 *                10-25 are on the MXP
 */
void Counter::SetUpSource(int channel) {
  if (StatusIsFatal()) return;
  SetUpSource(std::make_shared<DigitalInput>(channel));
}

/**
 * Set the up counting source to be an analog trigger.
 *
 * @param analogTrigger The analog trigger object that is used for the Up Source
 * @param triggerType   The analog trigger output that will trigger the counter.
 */
void Counter::SetUpSource(AnalogTrigger* analogTrigger,
                          AnalogTriggerType triggerType) {
  SetUpSource(std::shared_ptr<AnalogTrigger>(analogTrigger,
                                             NullDeleter<AnalogTrigger>()),
              triggerType);
}

/**
 * Set the up counting source to be an analog trigger.
 *
 * @param analogTrigger The analog trigger object that is used for the Up Source
 * @param triggerType   The analog trigger output that will trigger the counter.
 */
void Counter::SetUpSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                          AnalogTriggerType triggerType) {
  if (StatusIsFatal()) return;
  SetUpSource(analogTrigger->CreateOutput(triggerType));
}

/**
 * Set the source object that causes the counter to count up.
 *
 * Set the up counting DigitalSource.
 *
 * @param source Pointer to the DigitalSource object to set as the up source
 */
void Counter::SetUpSource(std::shared_ptr<DigitalSource> source) {
  if (StatusIsFatal()) return;
  m_upSource = source;
  if (m_upSource->StatusIsFatal()) {
    CloneError(*m_upSource);
  } else {
    int32_t status = 0;
    HAL_SetCounterUpSource(
        m_counter, source->GetPortHandleForRouting(),
        (HAL_AnalogTriggerType)source->GetAnalogTriggerTypeForRouting(),
        &status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  }
}

void Counter::SetUpSource(DigitalSource* source) {
  SetUpSource(
      std::shared_ptr<DigitalSource>(source, NullDeleter<DigitalSource>()));
}

/**
 * Set the source object that causes the counter to count up.
 *
 * Set the up counting DigitalSource.
 *
 * @param source Reference to the DigitalSource object to set as the up source
 */
void Counter::SetUpSource(DigitalSource& source) {
  SetUpSource(
      std::shared_ptr<DigitalSource>(&source, NullDeleter<DigitalSource>()));
}

/**
 * Set the edge sensitivity on an up counting source.
 *
 * Set the up source to either detect rising edges or falling edges or both.
 *
 * @param risingEdge  True to trigger on rising edges
 * @param fallingEdge True to trigger on falling edges
 */
void Counter::SetUpSourceEdge(bool risingEdge, bool fallingEdge) {
  if (StatusIsFatal()) return;
  if (m_upSource == nullptr) {
    wpi_setWPIErrorWithContext(
        NullParameter,
        "Must set non-nullptr UpSource before setting UpSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterUpSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Disable the up counting source to the counter.
 */
void Counter::ClearUpSource() {
  if (StatusIsFatal()) return;
  m_upSource.reset();
  int32_t status = 0;
  HAL_ClearCounterUpSource(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the down counting source to be a digital input channel.
 *
 * @param channel The DIO channel to use as the up source. 0-9 are on-board,
 *                10-25 are on the MXP
 */
void Counter::SetDownSource(int channel) {
  if (StatusIsFatal()) return;
  SetDownSource(std::make_shared<DigitalInput>(channel));
}

/**
 * Set the down counting source to be an analog trigger.
 *
 * @param analogTrigger The analog trigger object that is used for the Down
 *                      Source
 * @param triggerType   The analog trigger output that will trigger the counter.
 */
void Counter::SetDownSource(AnalogTrigger* analogTrigger,
                            AnalogTriggerType triggerType) {
  SetDownSource(std::shared_ptr<AnalogTrigger>(analogTrigger,
                                               NullDeleter<AnalogTrigger>()),
                triggerType);
}

/**
 * Set the down counting source to be an analog trigger.
 *
 * @param analogTrigger The analog trigger object that is used for the Down
 *                      Source
 * @param triggerType   The analog trigger output that will trigger the counter.
 */
void Counter::SetDownSource(std::shared_ptr<AnalogTrigger> analogTrigger,
                            AnalogTriggerType triggerType) {
  if (StatusIsFatal()) return;
  SetDownSource(analogTrigger->CreateOutput(triggerType));
}

/**
 * Set the source object that causes the counter to count down.
 *
 * Set the down counting DigitalSource.
 *
 * @param source Pointer to the DigitalSource object to set as the down source
 */
void Counter::SetDownSource(std::shared_ptr<DigitalSource> source) {
  if (StatusIsFatal()) return;
  m_downSource = source;
  if (m_downSource->StatusIsFatal()) {
    CloneError(*m_downSource);
  } else {
    int32_t status = 0;
    HAL_SetCounterDownSource(
        m_counter, source->GetPortHandleForRouting(),
        (HAL_AnalogTriggerType)source->GetAnalogTriggerTypeForRouting(),
        &status);
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  }
}

void Counter::SetDownSource(DigitalSource* source) {
  SetDownSource(
      std::shared_ptr<DigitalSource>(source, NullDeleter<DigitalSource>()));
}

/**
 * Set the source object that causes the counter to count down.
 *
 * Set the down counting DigitalSource.
 *
 * @param source Reference to the DigitalSource object to set as the down source
 */
void Counter::SetDownSource(DigitalSource& source) {
  SetDownSource(
      std::shared_ptr<DigitalSource>(&source, NullDeleter<DigitalSource>()));
}

/**
 * Set the edge sensitivity on a down counting source.
 *
 * Set the down source to either detect rising edges or falling edges.
 *
 * @param risingEdge  True to trigger on rising edges
 * @param fallingEdge True to trigger on falling edges
 */
void Counter::SetDownSourceEdge(bool risingEdge, bool fallingEdge) {
  if (StatusIsFatal()) return;
  if (m_downSource == nullptr) {
    wpi_setWPIErrorWithContext(
        NullParameter,
        "Must set non-nullptr DownSource before setting DownSourceEdge");
  }
  int32_t status = 0;
  HAL_SetCounterDownSourceEdge(m_counter, risingEdge, fallingEdge, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Disable the down counting source to the counter.
 */
void Counter::ClearDownSource() {
  if (StatusIsFatal()) return;
  m_downSource.reset();
  int32_t status = 0;
  HAL_ClearCounterDownSource(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set standard up / down counting mode on this counter.
 *
 * Up and down counts are sourced independently from two inputs.
 */
void Counter::SetUpDownCounterMode() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterUpDownMode(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set external direction mode on this counter.
 *
 * Counts are sourced on the Up counter input.
 * The Down counter input represents the direction to count.
 */
void Counter::SetExternalDirectionMode() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterExternalDirectionMode(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set Semi-period mode on this counter.
 *
 * Counts up on both rising and falling edges.
 */
void Counter::SetSemiPeriodMode(bool highSemiPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterSemiPeriodMode(m_counter, highSemiPeriod, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Configure the counter to count in up or down based on the length of the input
 * pulse.
 *
 * This mode is most useful for direction sensitive gear tooth sensors.
 *
 * @param threshold The pulse length beyond which the counter counts the
 *                  opposite direction.  Units are seconds.
 */
void Counter::SetPulseLengthMode(double threshold) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterPulseLengthMode(m_counter, threshold, &status);
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
int Counter::GetSamplesToAverage() const {
  int32_t status = 0;
  int samples = HAL_GetCounterSamplesToAverage(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return samples;
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 *
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void Counter::SetSamplesToAverage(int samplesToAverage) {
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    wpi_setWPIErrorWithContext(
        ParameterOutOfRange,
        "Average counter values must be between 1 and 127");
  }
  int32_t status = 0;
  HAL_SetCounterSamplesToAverage(m_counter, samplesToAverage, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Read the current counter value.
 *
 * Read the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 */
int Counter::Get() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetCounter(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Reset the Counter to zero.
 *
 * Set the counter value to zero. This doesn't effect the running state of the
 * counter, just sets the current value to zero.
 */
void Counter::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetCounter(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Get the Period of the most recent count.
 *
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 *
 * @returns The period between the last two pulses in units of seconds.
 */
double Counter::GetPeriod() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetCounterPeriod(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Set the maximum period where the device is still considered "moving".
 *
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the counter using the GetStopped
 * method.
 *
 * @param maxPeriod The maximum period where the counted device is considered
 *                  moving in seconds.
 */
void Counter::SetMaxPeriod(double maxPeriod) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterMaxPeriod(m_counter, maxPeriod, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Select whether you want to continue updating the event timer output when
 * there are no samples captured.
 *
 * The output of the event timer has a buffer of periods that are averaged and
 * posted to a register on the FPGA.  When the timer detects that the event
 * source has stopped (based on the MaxPeriod) the buffer of samples to be
 * averaged is emptied.  If you enable the update when empty, you will be
 * notified of the stopped source and the event time will report 0 samples.
 * If you disable update when empty, the most recent average will remain on
 * the output until a new sample is acquired.  You will never see 0 samples
 * output (except when there have been no events since an FPGA reset) and you
 * will likely not see the stopped bit become true (since it is updated at the
 * end of an average and there are no samples to average).
 *
 * @param enabled True to enable update when empty
 */
void Counter::SetUpdateWhenEmpty(bool enabled) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterUpdateWhenEmpty(m_counter, enabled, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Determine if the clock is stopped.
 *
 * Determine if the clocked input is stopped based on the MaxPeriod value set
 * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
 * device (and counter) are assumed to be stopped and it returns true.
 *
 * @return Returns true if the most recent counter period exceeds the MaxPeriod
 *         value set by SetMaxPeriod.
 */
bool Counter::GetStopped() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetCounterStopped(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * The last direction the counter value changed.
 *
 * @return The last direction the counter value changed.
 */
bool Counter::GetDirection() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetCounterDirection(m_counter, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Set the Counter to return reversed sensing on the direction.
 *
 * This allows counters to change the direction they are counting in the case of
 * 1X and 2X quadrature encoding only. Any other counter mode isn't supported.
 *
 * @param reverseDirection true if the value counted should be negated.
 */
void Counter::SetReverseDirection(bool reverseDirection) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetCounterReverseDirection(m_counter, reverseDirection, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void Counter::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutNumber("Value", Get());
  }
}

void Counter::StartLiveWindowMode() {}

void Counter::StopLiveWindowMode() {}

std::string Counter::GetSmartDashboardType() const { return "Counter"; }

void Counter::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> Counter::GetTable() const { return m_table; }
