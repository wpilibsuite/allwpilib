/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogGyro.h"
#include "HAL/AnalogGyro.h"

#include <climits>

#include "AnalogInput.h"
#include "HAL/Errors.h"
#include "HAL/HAL.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"
#include "WPIErrors.h"

using namespace frc;

const int AnalogGyro::kOversampleBits;
const int AnalogGyro::kAverageBits;
constexpr double AnalogGyro::kSamplesPerSecond;
constexpr double AnalogGyro::kCalibrationSampleTime;
constexpr double AnalogGyro::kDefaultVoltsPerDegreePerSecond;

/**
 * Gyro constructor using the Analog Input channel number.
 *
 * @param channel The analog channel the gyro is connected to. Gyros can only
 *                be used on on-board Analog Inputs 0-1.
 */
AnalogGyro::AnalogGyro(int channel)
    : AnalogGyro(std::make_shared<AnalogInput>(channel)) {}

/**
 * Gyro constructor with a precreated AnalogInput object.
 *
 * Use this constructor when the analog channel needs to be shared.
 * This object will not clean up the AnalogInput object when using this
 * constructor.
 *
 * Gyros can only be used on on-board channels 0-1.
 *
 * @param channel A pointer to the AnalogInput object that the gyro is
 *                connected to.
 */
AnalogGyro::AnalogGyro(AnalogInput* channel)
    : AnalogGyro(
          std::shared_ptr<AnalogInput>(channel, NullDeleter<AnalogInput>())) {}

/**
 * Gyro constructor with a precreated AnalogInput object.
 *
 * Use this constructor when the analog channel needs to be shared.
 * This object will not clean up the AnalogInput object when using this
 * constructor.
 *
 * @param channel A pointer to the AnalogInput object that the gyro is
 *                connected to.
 */
AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel)
    : m_analog(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitGyro();
    Calibrate();
  }
}

/**
 * Gyro constructor using the Analog Input channel number with parameters for
 * presetting the center and offset values. Bypasses calibration.
 *
 * @param channel The analog channel the gyro is connected to. Gyros can only
 *                be used on on-board Analog Inputs 0-1.
 * @param center  Preset uncalibrated value to use as the accumulator center
 *                value.
 * @param offset  Preset uncalibrated value to use as the gyro offset.
 */
AnalogGyro::AnalogGyro(int channel, int center, double offset) {
  m_analog = std::make_shared<AnalogInput>(channel);
  InitGyro();
  int32_t status = 0;
  HAL_SetAnalogGyroParameters(m_gyroHandle, kDefaultVoltsPerDegreePerSecond,
                              offset, center, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_gyroHandle = HAL_kInvalidHandle;
    return;
  }
  Reset();
}

/**
 * Gyro constructor with a precreated AnalogInput object and calibrated
 * parameters.
 *
 * Use this constructor when the analog channel needs to be shared.
 * This object will not clean up the AnalogInput object when using this
 * constructor.
 *
 * @param channel A pointer to the AnalogInput object that the gyro is
 *                connected to.
 */
AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel, int center,
                       double offset)
    : m_analog(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitGyro();
    int32_t status = 0;
    HAL_SetAnalogGyroParameters(m_gyroHandle, kDefaultVoltsPerDegreePerSecond,
                                offset, center, &status);
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
    Reset();
  }
}

/**
 * AnalogGyro Destructor
 *
 */
AnalogGyro::~AnalogGyro() { HAL_FreeAnalogGyro(m_gyroHandle); }

/**
 * Reset the gyro.
 *
 * Resets the gyro to a heading of zero. This can be used if there is
 * significant drift in the gyro and it needs to be recalibrated after it has
 * been running.
 */
void AnalogGyro::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetAnalogGyro(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Initialize the gyro.  Calibration is handled by Calibrate().
 */
void AnalogGyro::InitGyro() {
  if (StatusIsFatal()) return;
  if (m_gyroHandle == HAL_kInvalidHandle) {
    int32_t status = 0;
    m_gyroHandle = HAL_InitializeAnalogGyro(m_analog->m_port, &status);
    if (status == PARAMETER_OUT_OF_RANGE) {
      wpi_setWPIErrorWithContext(ParameterOutOfRange,
                                 " channel (must be accumulator channel)");
      m_analog = nullptr;
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_analog = nullptr;
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
  }

  int32_t status = 0;
  HAL_SetupAnalogGyro(m_gyroHandle, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_analog = nullptr;
    m_gyroHandle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_Gyro, m_analog->GetChannel());
  LiveWindow::GetInstance()->AddSensor("AnalogGyro", m_analog->GetChannel(),
                                       this);
}

void AnalogGyro::Calibrate() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_CalibrateAnalogGyro(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the
 * oversampling rate, the gyro type and the A/D calibration values.
 * The angle is continuous, that is it will continue from 360->361 degrees. This
 * allows algorithms that wouldn't want to see a discontinuity in the gyro
 * output as it sweeps from 360 to 0 on the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on
 *         integration of the returned rate from the gyro.
 */
double AnalogGyro::GetAngle() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroAngle(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double AnalogGyro::GetRate() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroRate(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Return the gyro offset value. If run after calibration,
 * the offset value can be used as a preset later.
 *
 * @return the current offset value
 */
double AnalogGyro::GetOffset() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroOffset(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Return the gyro center value. If run after calibration,
 * the center value can be used as a preset later.
 *
 * @return the current center value
 */
int AnalogGyro::GetCenter() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetAnalogGyroCenter(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * Set the gyro sensitivity.
 *
 * This takes the number of volts/degree/second sensitivity of the gyro and uses
 * it in subsequent calculations to allow the code to work with multiple gyros.
 * This value is typically found in the gyro datasheet.
 *
 * @param voltsPerDegreePerSecond The sensitivity in Volts/degree/second
 */
void AnalogGyro::SetSensitivity(double voltsPerDegreePerSecond) {
  int32_t status = 0;
  HAL_SetAnalogGyroVoltsPerDegreePerSecond(m_gyroHandle,
                                           voltsPerDegreePerSecond, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the size of the neutral zone.
 *
 * Any voltage from the gyro less than this amount from the center is
 * considered stationary.  Setting a deadband will decrease the amount of drift
 * when the gyro isn't rotating, but will make it less accurate.
 *
 * @param volts The size of the deadband in volts
 */
void AnalogGyro::SetDeadband(double volts) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogGyroDeadband(m_gyroHandle, volts, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}
