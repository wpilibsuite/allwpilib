// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 Analog Devices Inc. All Rights Reserved.           */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*                                                                            */
/* Juan Chong - frcsupport@analog.com                                         */
/*----------------------------------------------------------------------------*/

#include "frc/ADIS16470_IMU.h"

#include <cmath>
#include <numbers>
#include <utility>

#include <fmt/format.h>
#include <hal/HAL.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/DigitalInput.h"
#include "frc/Errors.h"
#include "frc/MathUtil.h"
#include "frc/Timer.h"

/* Helpful conversion functions */
static inline int32_t ToInt(const uint32_t* buf) {
  return static_cast<int32_t>((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) |
                              buf[3]);
}

static inline int16_t BuffToShort(const uint32_t* buf) {
  return (static_cast<int16_t>(buf[0]) << 8) | buf[1];
}

using namespace frc;

namespace {
template <typename S, typename... Args>
inline void ADISReportError(int32_t status, const char* file, int line,
                            const char* function, const S& format,
                            Args&&... args) {
  frc::ReportErrorV(status, file, line, function, format,
                    fmt::make_format_args(args...));
}
}  // namespace

#define REPORT_WARNING(msg) \
  ADISReportError(warn::Warning, __FILE__, __LINE__, __FUNCTION__, msg);
#define REPORT_ERROR(msg) \
  ADISReportError(err::Error, __FILE__, __LINE__, __FUNCTION__, msg)

ADIS16470_IMU::ADIS16470_IMU()
    : ADIS16470_IMU(kZ, kY, kX, SPI::Port::kOnboardCS0, CalibrationTime::_1s) {}

ADIS16470_IMU::ADIS16470_IMU(IMUAxis yaw_axis, IMUAxis pitch_axis,
                             IMUAxis roll_axis)
    : ADIS16470_IMU(yaw_axis, pitch_axis, roll_axis, SPI::Port::kOnboardCS0,
                    CalibrationTime::_1s) {}

ADIS16470_IMU::ADIS16470_IMU(IMUAxis yaw_axis, IMUAxis pitch_axis,
                             IMUAxis roll_axis, SPI::Port port,
                             CalibrationTime cal_time)
    : m_yaw_axis(yaw_axis),
      m_pitch_axis(pitch_axis),
      m_roll_axis(roll_axis),
      m_spi_port(port),
      m_calibration_time(static_cast<uint16_t>(cal_time)),
      m_simDevice("Gyro:ADIS16470", port) {
  if (yaw_axis == kYaw || yaw_axis == kPitch || yaw_axis == kRoll ||
      pitch_axis == kYaw || pitch_axis == kPitch || pitch_axis == kRoll ||
      roll_axis == kYaw || roll_axis == kPitch || roll_axis == kRoll) {
    REPORT_ERROR(
        "ADIS16470 constructor only allows IMUAxis.kX, IMUAxis.kY or "
        "IMUAxis.kZ as arguments.");
    REPORT_ERROR(
        "Constructing ADIS with default axes. (IMUAxis.kZ is defined as Yaw)");
    yaw_axis = kZ;
    pitch_axis = kY;
    roll_axis = kX;
  }

  if (m_simDevice) {
    m_connected =
        m_simDevice.CreateBoolean("connected", hal::SimDevice::kInput, true);
    m_simGyroAngleX =
        m_simDevice.CreateDouble("gyro_angle_x", hal::SimDevice::kInput, 0.0);
    m_simGyroAngleY =
        m_simDevice.CreateDouble("gyro_angle_y", hal::SimDevice::kInput, 0.0);
    m_simGyroAngleZ =
        m_simDevice.CreateDouble("gyro_angle_z", hal::SimDevice::kInput, 0.0);
    m_simGyroRateX =
        m_simDevice.CreateDouble("gyro_rate_x", hal::SimDevice::kInput, 0.0);
    m_simGyroRateY =
        m_simDevice.CreateDouble("gyro_rate_y", hal::SimDevice::kInput, 0.0);
    m_simGyroRateZ =
        m_simDevice.CreateDouble("gyro_rate_z", hal::SimDevice::kInput, 0.0);
    m_simAccelX =
        m_simDevice.CreateDouble("accel_x", hal::SimDevice::kInput, 0.0);
    m_simAccelY =
        m_simDevice.CreateDouble("accel_y", hal::SimDevice::kInput, 0.0);
    m_simAccelZ =
        m_simDevice.CreateDouble("accel_z", hal::SimDevice::kInput, 0.0);
  }

  if (!m_simDevice) {
    // Force the IMU reset pin to toggle on startup (doesn't require DS enable)
    // Relies on the RIO hardware by default configuring an output as low
    // and configuring an input as high Z. The 10k pull-up resistor internal to
    // the IMU then forces the reset line high for normal operation.
    DigitalOutput* reset_out =
        new DigitalOutput(27);  // Drive SPI CS2 (IMU RST) low
    Wait(10_ms);
    delete reset_out;
    m_reset_in = new DigitalInput(27);  // Set SPI CS2 (IMU RST) high
    Wait(500_ms);                       // Wait for reset to complete

    // Configure standard SPI
    if (!SwitchToStandardSPI()) {
      return;
    }
    bool needsFlash = false;
    // Set IMU internal decimation to 4 (output data rate of 2000 SPS / (4 + 1)
    // = 400Hz)
    if (ReadRegister(DEC_RATE) != 0x0004) {
      WriteRegister(DEC_RATE, 0x0004);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16470: DEC_RATE register configuration inconsistent! Scheduling "
          "flash update.");
    }

    // Set data ready polarity (HIGH = Good Data), Disable gSense Compensation
    // and PoP
    if (ReadRegister(MSC_CTRL) != 0x0001) {
      WriteRegister(MSC_CTRL, 0x0001);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16470: MSC_CTRL register configuration inconsistent! Scheduling "
          "flash update.");
    }

    // Disable IMU internal Bartlett filter (200Hz bandwidth is sufficient)
    if (ReadRegister(FILT_CTRL) != 0x0000) {
      WriteRegister(FILT_CTRL, 0x0000);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16470: FILT_CTRL register configuration inconsistent! "
          "Scheduling flash update.");
    }

    // If any registers on the IMU don't match the config, trigger a flash
    // update
    if (needsFlash) {
      REPORT_WARNING(
          "ADIS16470: Register configuration changed! Starting IMU flash "
          "update.");
      WriteRegister(GLOB_CMD, 0x0008);
      // Wait long enough for the flash update to finish (72ms minimum as per
      // the datasheet)
      Wait(0.3_s);
      REPORT_WARNING("ADIS16470: Flash update finished!");
    } else {
      REPORT_WARNING(
          "ADIS16470: Flash and RAM configuration consistent. No flash update "
          "required!");
    }

    // Configure continuous bias calibration time based on user setting
    WriteRegister(NULL_CNFG, m_calibration_time | 0x700);

    // Notify DS that IMU calibration delay is active
    REPORT_WARNING("ADIS16470: Starting initial calibration delay.");

    // Wait for samples to accumulate internal to the IMU (110% of user-defined
    // time)
    Wait(units::second_t{pow(2, m_calibration_time) / 2000 * 64 * 1.1});

    // Write offset calibration command to IMU
    WriteRegister(GLOB_CMD, 0x0001);

    // Configure and enable auto SPI
    if (!SwitchToAutoSPI()) {
      return;
    }

    // Let the user know the IMU was initialized successfully
    REPORT_WARNING("ADIS16470 IMU Successfully Initialized!");

    // Drive SPI CS3 (IMU ready LED) low (active low)
    m_status_led = new DigitalOutput(28);
  }

  // Report usage and post data to DS
  HAL_Report(HALUsageReporting::kResourceType_ADIS16470, 0);

  wpi::SendableRegistry::AddLW(this, "ADIS16470", port);
  m_connected = true;
}

ADIS16470_IMU::ADIS16470_IMU(ADIS16470_IMU&& other)
    : m_yaw_axis{std::move(other.m_yaw_axis)},
      m_pitch_axis{std::move(other.m_pitch_axis)},
      m_roll_axis{std::move(other.m_roll_axis)},
      m_reset_in{std::move(other.m_reset_in)},
      m_status_led{std::move(other.m_status_led)},
      m_integ_angle_x{std::move(other.m_integ_angle_x)},
      m_integ_angle_y{std::move(other.m_integ_angle_y)},
      m_integ_angle_z{std::move(other.m_integ_angle_z)},
      m_gyro_rate_x{std::move(other.m_gyro_rate_x)},
      m_gyro_rate_y{std::move(other.m_gyro_rate_y)},
      m_gyro_rate_z{std::move(other.m_gyro_rate_z)},
      m_accel_x{std::move(other.m_accel_x)},
      m_accel_y{std::move(other.m_accel_y)},
      m_accel_z{std::move(other.m_accel_z)},
      m_dt{std::move(other.m_dt)},
      m_alpha{std::move(other.m_alpha)},
      m_compAngleX{std::move(other.m_compAngleX)},
      m_compAngleY{std::move(other.m_compAngleY)},
      m_accelAngleX{std::move(other.m_accelAngleX)},
      m_accelAngleY{std::move(other.m_accelAngleY)},
      m_thread_active{other.m_thread_active.load()},
      m_first_run{other.m_first_run.load()},
      m_thread_idle{other.m_thread_idle.load()},
      m_auto_configured{std::move(other.m_auto_configured)},
      m_spi_port{std::move(other.m_spi_port)},
      m_calibration_time{std::move(other.m_calibration_time)},
      m_spi{std::move(other.m_spi)},
      m_auto_interrupt{std::move(other.m_auto_interrupt)},
      m_scaled_sample_rate{std::move(other.m_scaled_sample_rate)},
      m_connected{std::move(other.m_connected)},
      m_acquire_task{std::move(other.m_acquire_task)},
      m_simDevice{std::move(other.m_simDevice)},
      m_simConnected{std::move(other.m_simConnected)},
      m_simGyroAngleX{std::move(other.m_simGyroAngleX)},
      m_simGyroAngleY{std::move(other.m_simGyroAngleZ)},
      m_simGyroAngleZ{std::move(other.m_simGyroAngleZ)},
      m_simGyroRateX{std::move(other.m_simGyroRateX)},
      m_simGyroRateY{std::move(other.m_simGyroRateY)},
      m_simGyroRateZ{std::move(other.m_simGyroRateZ)},
      m_simAccelX{std::move(other.m_simAccelX)},
      m_simAccelY{std::move(other.m_simAccelY)},
      m_simAccelZ{std::move(other.m_simAccelZ)},
      m_mutex{std::move(other.m_mutex)} {}

ADIS16470_IMU& ADIS16470_IMU::operator=(ADIS16470_IMU&& other) {
  if (this == &other) {
    return *this;
  }

  std::swap(this->m_yaw_axis, other.m_yaw_axis);
  std::swap(this->m_pitch_axis, other.m_pitch_axis);
  std::swap(this->m_roll_axis, other.m_roll_axis);
  std::swap(this->m_reset_in, other.m_reset_in);
  std::swap(this->m_status_led, other.m_status_led);
  std::swap(this->m_integ_angle_x, other.m_integ_angle_x);
  std::swap(this->m_integ_angle_y, other.m_integ_angle_y);
  std::swap(this->m_integ_angle_z, other.m_integ_angle_z);
  std::swap(this->m_gyro_rate_x, other.m_gyro_rate_x);
  std::swap(this->m_gyro_rate_y, other.m_gyro_rate_y);
  std::swap(this->m_gyro_rate_z, other.m_gyro_rate_z);
  std::swap(this->m_accel_x, other.m_accel_x);
  std::swap(this->m_accel_y, other.m_accel_y);
  std::swap(this->m_accel_z, other.m_accel_z);
  std::swap(this->m_dt, other.m_dt);
  std::swap(this->m_alpha, other.m_alpha);
  std::swap(this->m_compAngleX, other.m_compAngleX);
  std::swap(this->m_compAngleY, other.m_compAngleY);
  std::swap(this->m_accelAngleX, other.m_accelAngleX);
  std::swap(this->m_accelAngleY, other.m_accelAngleY);
  this->m_thread_active = other.m_thread_active.load();
  this->m_first_run = other.m_first_run.load();
  this->m_thread_idle = other.m_thread_idle.load();
  std::swap(this->m_auto_configured, other.m_auto_configured);
  std::swap(this->m_spi_port, other.m_spi_port);
  std::swap(this->m_calibration_time, other.m_calibration_time);
  std::swap(this->m_spi, other.m_spi);
  std::swap(this->m_auto_interrupt, other.m_auto_interrupt);
  std::swap(this->m_scaled_sample_rate, other.m_scaled_sample_rate);
  std::swap(this->m_connected, other.m_connected);
  std::swap(this->m_acquire_task, other.m_acquire_task);
  std::swap(this->m_simDevice, other.m_simDevice);
  std::swap(this->m_simConnected, other.m_simConnected);
  std::swap(this->m_simGyroAngleX, other.m_simGyroAngleX);
  std::swap(this->m_simGyroAngleY, other.m_simGyroAngleZ);
  std::swap(this->m_simGyroAngleZ, other.m_simGyroAngleZ);
  std::swap(this->m_simGyroRateX, other.m_simGyroRateX);
  std::swap(this->m_simGyroRateY, other.m_simGyroRateY);
  std::swap(this->m_simGyroRateZ, other.m_simGyroRateZ);
  std::swap(this->m_simAccelX, other.m_simAccelX);
  std::swap(this->m_simAccelY, other.m_simAccelY);
  std::swap(this->m_simAccelZ, other.m_simAccelZ);
  std::swap(this->m_mutex, other.m_mutex);
  return *this;
}

bool ADIS16470_IMU::IsConnected() const {
  if (m_simConnected) {
    return m_simConnected.Get();
  }
  return m_connected;
}

/**
 * @brief Switches to standard SPI operation. Primarily used when exiting auto
 *SPI mode.
 *
 * @return A boolean indicating the success or failure of setting up the SPI
 *peripheral in standard SPI mode.
 *
 * This function switches the active SPI port to standard SPI and is used
 *primarily when exiting auto SPI. Exiting auto SPI is required to read or write
 *using SPI since the auto SPI configuration, once active, locks the SPI message
 *being transacted. This function also verifies that the SPI port is operating
 *in standard SPI mode by reading back the IMU product ID.
 **/
bool ADIS16470_IMU::SwitchToStandardSPI() {
  // Check to see whether the acquire thread is active. If so, wait for it to
  // stop producing data.
  if (m_thread_active) {
    m_thread_active = false;
    while (!m_thread_idle) {
      Wait(10_ms);
    }
    // Maybe we're in auto SPI mode? If so, kill auto SPI, and then SPI.
    if (m_spi != nullptr && m_auto_configured) {
      m_spi->StopAuto();
      // We need to get rid of all the garbage left in the auto SPI buffer after
      // stopping it.
      // Sometimes data magically reappears, so we have to check the buffer size
      // a couple of times to be sure we got it all. Yuck.
      uint32_t trashBuffer[200];
      Wait(100_ms);
      int data_count = m_spi->ReadAutoReceivedData(trashBuffer, 0, 0_s);
      while (data_count > 0) {
        // Receive data, max of 200 words at a time (prevent potential segfault)
        m_spi->ReadAutoReceivedData(trashBuffer, (std::min)(data_count, 200),
                                    0_s);
        // Get the remaining data count
        data_count = m_spi->ReadAutoReceivedData(trashBuffer, 0, 0_s);
      }
    }
  }
  // There doesn't seem to be a SPI port active. Let's try to set one up
  if (m_spi == nullptr) {
    m_spi = new SPI(m_spi_port);
    m_spi->SetClockRate(2000000);
    m_spi->SetMode(frc::SPI::Mode::kMode3);
    m_spi->SetChipSelectActiveLow();
  }
  ReadRegister(PROD_ID);  // Dummy read
  // Validate the product ID
  uint16_t prod_id = ReadRegister(PROD_ID);
  if (prod_id != 16982 && prod_id != 16470) {
    REPORT_ERROR(
        fmt::format("Could not find ADIS16470; got product ID {}", prod_id));
    Close();
    return false;
  }
  return true;
}

/**
 * @brief Switches to auto SPI operation. Primarily used when exiting standard
 *SPI mode.
 *
 * @return A boolean indicating the success or failure of setting up the SPI
 *peripheral in auto SPI mode.
 *
 * This function switches the active SPI port to auto SPI and is used primarily
 *when exiting standard SPI. Auto SPI is required to asynchronously read data
 *over SPI as it utilizes special FPGA hardware to react to an external data
 *ready (GPIO) input. Data captured using auto SPI is buffered in the FPGA and
 *can be read by the CPU asynchronously. Standard SPI transactions are
 * impossible on the selected SPI port once auto SPI is enabled. The stall
 *settings, GPIO interrupt pin, and data packet settings used in this function
 *are hard-coded to work only with the ADIS16470 IMU.
 **/
bool ADIS16470_IMU::SwitchToAutoSPI() {
  // No SPI port has been set up. Go set one up first.
  if (m_spi == nullptr && !SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to start/restart auto SPI");
    return false;
  }

  // Only set up the interrupt if needed.
  if (m_auto_interrupt == nullptr) {
    m_auto_interrupt = new DigitalInput(26);
  }
  // The auto SPI controller gets angry if you try to set up two instances on
  // one bus.
  if (!m_auto_configured) {
    m_spi->InitAuto(8200);
    m_auto_configured = true;
  }
  // Do we need to change auto SPI settings?
  m_spi->SetAutoTransmitData(m_autospi_allangle_packet, 2);
  // Configure auto stall time
  m_spi->ConfigureAutoStall(HAL_SPI_kOnboardCS0, 5, 1000, 1);
  // Kick off DMA SPI (Note: Device configuration impossible after SPI DMA is
  // activated)
  // DR High = Data good (data capture should be triggered on the rising edge)
  m_spi->StartAutoTrigger(*m_auto_interrupt, true, false);
  // Check to see if the acquire thread is running. If not, kick one off.
  if (!m_thread_idle) {
    m_first_run = true;
    m_thread_active = true;
    m_acquire_task = std::thread(&ADIS16470_IMU::Acquire, this);
  } else {
    m_first_run = true;
    m_thread_active = true;
  }
  // Looks like the thread didn't start for some reason. Abort.
  /*
  if(!m_thread_idle) {
    REPORT_ERROR("Failed to start/restart the acquire() thread.");
    Close();
    return false;
  }
  */
  return true;
}

/**
 * @brief Switches the active SPI port to standard SPI mode, writes a new value
 *to the NULL_CNFG register in the IMU, and re-enables auto SPI.
 *
 * @param new_cal_time Calibration time to be set.
 *
 * @return An int indicating the success or failure of writing the new NULL_CNFG
 *setting and returning to auto SPI mode. 0 = Success, 1 = No Change, 2 =
 *Failure
 *
 * This function enters standard SPI mode, writes a new NULL_CNFG setting to the
 *IMU, and re-enters auto SPI mode. This function does not include a blocking
 *sleep, so the user must keep track of the elapsed offset calibration time
 * themselves. After waiting the configured calibration time, the Calibrate()
 *function should be called to activate the new offset calibration.
 **/
int ADIS16470_IMU::ConfigCalTime(CalibrationTime new_cal_time) {
  if (m_calibration_time == static_cast<uint16_t>(new_cal_time)) {
    return 1;
  }
  if (!SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure standard SPI.");
    return 2;
  }
  m_calibration_time = static_cast<uint16_t>(new_cal_time);
  WriteRegister(NULL_CNFG, m_calibration_time | 0x700);
  if (!SwitchToAutoSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure auto SPI.");
    return 2;
  }
  return 0;
}

int ADIS16470_IMU::ConfigDecRate(uint16_t decimationRate) {
  // Switches the active SPI port to standard SPI mode, writes a new value to
  // the DECIMATE register in the IMU, adjusts the sample scale factor, and
  // re-enables auto SPI.
  if (!SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure standard SPI.");
    return 2;
  }
  if (decimationRate > 1999) {
    REPORT_ERROR("Attempted to write an invalid decimation value.");
    decimationRate = 1999;
  }
  m_scaled_sample_rate = (decimationRate + 1.0) / 2000.0 * 1000000.0;
  WriteRegister(DEC_RATE, decimationRate);
  if (!SwitchToAutoSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure auto SPI.");
    return 2;
  }
  return 0;
}

/**
 * @brief Switches the active SPI port to standard SPI mode, writes the command
 *to activate the new null configuration, and re-enables auto SPI.
 *
 * This function enters standard SPI mode, writes 0x0001 to the GLOB_CMD
 *register (thus making the new offset active in the IMU), and re-enters auto
 *SPI mode. This function does not include a blocking sleep, so the user must
 *keep track of the elapsed offset calibration time themselves.
 **/
void ADIS16470_IMU::Calibrate() {
  if (!SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure standard SPI.");
  }
  WriteRegister(GLOB_CMD, 0x0001);
  if (!SwitchToAutoSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure auto SPI.");
  }
}

uint16_t ADIS16470_IMU::ReadRegister(uint8_t reg) {
  uint8_t buf[2];
  buf[0] = reg & 0x7f;
  buf[1] = 0;

  m_spi->Write(buf, 2);
  m_spi->Read(false, buf, 2);

  return (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
}

/**
 * @brief Writes an unsigned, 16-bit value to two adjacent, 8-bit register
 * locations over SPI.
 *
 * @param reg An unsigned, 8-bit register location.
 *
 * @param val An unsigned, 16-bit value to be written to the specified register
 * location.
 *
 * This function writes an unsigned, 16-bit value into adjacent 8-bit addresses
 * via SPI. The upper and lower bytes that make up the 16-bit value are split
 * into two unsigned, 8-bit values and written to the upper and lower addresses
 * of the specified register value. Only the lower (base) address must be
 * specified. This function assumes the controller is set to standard SPI mode.
 */
void ADIS16470_IMU::WriteRegister(uint8_t reg, uint16_t val) {
  uint8_t buf[2];
  buf[0] = 0x80 | reg;
  buf[1] = val & 0xff;
  m_spi->Write(buf, 2);
  buf[0] = 0x81 | reg;
  buf[1] = val >> 8;
  m_spi->Write(buf, 2);
}

void ADIS16470_IMU::Reset() {
  std::scoped_lock sync(m_mutex);
  m_integ_angle_x = 0.0;
  m_integ_angle_y = 0.0;
  m_integ_angle_z = 0.0;
}

void ADIS16470_IMU::Close() {
  if (m_reset_in != nullptr) {
    delete m_reset_in;
    m_reset_in = nullptr;
  }
  if (m_status_led != nullptr) {
    delete m_status_led;
    m_status_led = nullptr;
  }
  if (m_thread_active) {
    m_thread_active = false;
    if (m_acquire_task.joinable()) {
      m_acquire_task.join();
    }
  }
  if (m_spi != nullptr) {
    if (m_auto_configured) {
      m_spi->StopAuto();
    }
    delete m_spi;
    m_auto_configured = false;
    if (m_auto_interrupt != nullptr) {
      delete m_auto_interrupt;
      m_auto_interrupt = nullptr;
    }
    m_spi = nullptr;
  }
}

ADIS16470_IMU::~ADIS16470_IMU() {
  Close();
}

/**
 * @brief Main acquisition loop. Typically called asynchronously and free-wheels
 *while the robot code is active.
 *
 * This is the main acquisition loop for the IMU. During each iteration, data
 *read using auto SPI is extracted from the FPGA FIFO, split, scaled, and
 *integrated. Each X, Y, and Z value is 32-bits split across 4 indices (bytes)
 *in the buffer. Auto SPI puts one byte in each index location. Each index is
 *32-bits wide because the timestamp is an unsigned 32-bit int. The timestamp is
 *always located at the beginning of the frame. Two indices (request_1 and
 *request_2 below) are always invalid (garbage) and can be disregarded.
 *
 * Data order: [timestamp, request_1, request_2, x_1, x_2, x_3, x_4, y_1, y_2,
 *y_3, y_4, z_1, z_2, z_3, z_4] x = delta x (32-bit x_1 = highest bit) y = delta
 *y (32-bit y_1 = highest bit) z = delta z (32-bit z_1 = highest bit)
 *
 * Complementary filter code was borrowed from
 *https://github.com/tcleg/Six_Axis_Complementary_Filter
 **/
void ADIS16470_IMU::Acquire() {
  // Set data packet length
  const int dataset_len = 27;  // 26 data points + timestamp
  const int BUFFER_SIZE = 4000;

  // This buffer can contain many datasets
  uint32_t buffer[BUFFER_SIZE];
  uint32_t previous_timestamp = 0;
  double compAngleX = 0.0;
  double compAngleY = 0.0;
  while (true) {
    // Wait for data
    Wait(10_ms);

    if (m_thread_active) {
      m_thread_idle = false;

      // Read number of bytes currently stored in the buffer
      int data_count = m_spi->ReadAutoReceivedData(buffer, 0, 0_s);
      // Check if frame is incomplete
      int data_remainder = data_count % dataset_len;
      // Remove incomplete data from read count
      int data_to_read = data_count - data_remainder;
      // Want to cap the data to read in a single read at the buffer size
      if (data_to_read > BUFFER_SIZE) {
        REPORT_WARNING(
            "ADIS16470 data processing thread overrun has occurred!");
        data_to_read = BUFFER_SIZE - (BUFFER_SIZE % dataset_len);
      }
      // Read data from DMA buffer (only complete sets)
      m_spi->ReadAutoReceivedData(buffer, data_to_read, 0_s);

      // Could be multiple data sets in the buffer. Handle each one.
      for (int i = 0; i < data_to_read; i += dataset_len) {
        // Timestamp is at buffer[i]
        m_dt = (buffer[i] - previous_timestamp) / 1000000.0;
        // Get delta angle value for selected yaw axis and scale by the elapsed
        // time (based on timestamp)
        double elapsed_time =
            m_scaled_sample_rate / (buffer[i] - previous_timestamp);
        double delta_angle_x =
            ToInt(&buffer[i + 3]) * delta_angle_sf / elapsed_time;
        double delta_angle_y =
            ToInt(&buffer[i + 7]) * delta_angle_sf / elapsed_time;
        double delta_angle_z =
            ToInt(&buffer[i + 11]) * delta_angle_sf / elapsed_time;

        double gyro_rate_x = BuffToShort(&buffer[i + 15]) / 10.0;
        double gyro_rate_y = BuffToShort(&buffer[i + 17]) / 10.0;
        double gyro_rate_z = BuffToShort(&buffer[i + 19]) / 10.0;
        double accel_x = BuffToShort(&buffer[i + 21]) / 800.0;
        double accel_y = BuffToShort(&buffer[i + 23]) / 800.0;
        double accel_z = BuffToShort(&buffer[i + 25]) / 800.0;

        // Convert scaled sensor data to SI units
        double gyro_rate_x_si = gyro_rate_x * kDegToRad;
        double gyro_rate_y_si = gyro_rate_y * kDegToRad;
        // double gyro_rate_z_si = gyro_rate_z * kDegToRad;
        double accel_x_si = accel_x * kGrav;
        double accel_y_si = accel_y * kGrav;
        double accel_z_si = accel_z * kGrav;

        // Store timestamp for next iteration
        previous_timestamp = buffer[i];

        m_alpha = kTau / (kTau + m_dt);

        // Run inclinometer calculations
        double accelAngleX =
            atan2f(accel_x_si, std::hypotf(accel_y_si, accel_z_si));
        double accelAngleY =
            atan2f(accel_y_si, std::hypotf(accel_x_si, accel_z_si));
        if (m_first_run) {
          compAngleX = accelAngleX;
          compAngleY = accelAngleY;
        } else {
          accelAngleX = FormatAccelRange(accelAngleX, accel_z_si);
          accelAngleY = FormatAccelRange(accelAngleY, accel_z_si);
          compAngleX =
              CompFilterProcess(compAngleX, accelAngleX, -gyro_rate_y_si);
          compAngleY =
              CompFilterProcess(compAngleY, accelAngleY, gyro_rate_x_si);
        }

        {
          std::scoped_lock sync(m_mutex);
          // Push data to global variables
          if (m_first_run) {
            // Don't accumulate first run. previous_timestamp will be "very" old
            // and the integration will end up way off
            m_integ_angle_x = 0.0;
            m_integ_angle_y = 0.0;
            m_integ_angle_z = 0.0;
          } else {
            m_integ_angle_x += delta_angle_x;
            m_integ_angle_y += delta_angle_y;
            m_integ_angle_z += delta_angle_z;
          }
          m_gyro_rate_x = gyro_rate_x;
          m_gyro_rate_y = gyro_rate_y;
          m_gyro_rate_z = gyro_rate_z;
          m_accel_x = accel_x;
          m_accel_y = accel_y;
          m_accel_z = accel_z;
          m_compAngleX = compAngleX * kRadToDeg;
          m_compAngleY = compAngleY * kRadToDeg;
          m_accelAngleX = accelAngleX * kRadToDeg;
          m_accelAngleY = accelAngleY * kRadToDeg;
        }
        m_first_run = false;
      }
    } else {
      m_thread_idle = true;
      previous_timestamp = 0;
      compAngleX = 0.0;
      compAngleY = 0.0;
    }
  }
}

/* Complementary filter functions */
double ADIS16470_IMU::FormatFastConverge(double compAngle, double accAngle) {
  if (compAngle > accAngle + std::numbers::pi) {
    compAngle = compAngle - 2.0 * std::numbers::pi;
  } else if (accAngle > compAngle + std::numbers::pi) {
    compAngle = compAngle + 2.0 * std::numbers::pi;
  }
  return compAngle;
}

double ADIS16470_IMU::FormatAccelRange(double accelAngle, double accelZ) {
  if (accelZ < 0.0) {
    accelAngle = std::numbers::pi - accelAngle;
  } else if (accelZ > 0.0 && accelAngle < 0.0) {
    accelAngle = 2.0 * std::numbers::pi + accelAngle;
  }
  return accelAngle;
}

double ADIS16470_IMU::CompFilterProcess(double compAngle, double accelAngle,
                                        double omega) {
  compAngle = FormatFastConverge(compAngle, accelAngle);
  compAngle =
      m_alpha * (compAngle + omega * m_dt) + (1.0 - m_alpha) * accelAngle;
  return frc::InputModulus(compAngle, -std::numbers::pi, std::numbers::pi);
}

void ADIS16470_IMU::SetGyroAngle(IMUAxis axis, units::degree_t angle) {
  switch (axis) {
    case kYaw:
      axis = m_yaw_axis;
      break;
    case kPitch:
      axis = m_pitch_axis;
      break;
    case kRoll:
      axis = m_roll_axis;
      break;
    default:
      break;
  }

  switch (axis) {
    case kX:
      SetGyroAngleX(angle);
      break;
    case kY:
      SetGyroAngleY(angle);
      break;
    case kZ:
      SetGyroAngleZ(angle);
      break;
    default:
      break;
  }
}

void ADIS16470_IMU::SetGyroAngleX(units::degree_t angle) {
  std::scoped_lock sync(m_mutex);
  m_integ_angle_x = angle.value();
}

void ADIS16470_IMU::SetGyroAngleY(units::degree_t angle) {
  std::scoped_lock sync(m_mutex);
  m_integ_angle_y = angle.value();
}

void ADIS16470_IMU::SetGyroAngleZ(units::degree_t angle) {
  std::scoped_lock sync(m_mutex);
  m_integ_angle_z = angle.value();
}

units::degree_t ADIS16470_IMU::GetAngle(IMUAxis axis) const {
  switch (axis) {
    case kYaw:
      axis = m_yaw_axis;
      break;
    case kPitch:
      axis = m_pitch_axis;
      break;
    case kRoll:
      axis = m_roll_axis;
      break;
    default:
      break;
  }

  switch (axis) {
    case kX:
      if (m_simGyroAngleX) {
        return units::degree_t{m_simGyroAngleX.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degree_t{m_integ_angle_x};
      }
    case kY:
      if (m_simGyroAngleY) {
        return units::degree_t{m_simGyroAngleY.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degree_t{m_integ_angle_y};
      }
    case kZ:
      if (m_simGyroAngleZ) {
        return units::degree_t{m_simGyroAngleZ.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degree_t{m_integ_angle_z};
      }
    default:
      break;
  }

  return 0_deg;
}

units::degrees_per_second_t ADIS16470_IMU::GetRate(IMUAxis axis) const {
  switch (axis) {
    case kYaw:
      axis = m_yaw_axis;
      break;
    case kPitch:
      axis = m_pitch_axis;
      break;
    case kRoll:
      axis = m_roll_axis;
      break;
    default:
      break;
  }

  switch (axis) {
    case kX:
      if (m_simGyroRateX) {
        return units::degrees_per_second_t{m_simGyroRateX.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degrees_per_second_t{m_gyro_rate_x};
      }
    case kY:
      if (m_simGyroRateY) {
        return units::degrees_per_second_t{m_simGyroRateY.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degrees_per_second_t{m_gyro_rate_y};
      }
    case kZ:
      if (m_simGyroRateZ) {
        return units::degrees_per_second_t{m_simGyroRateZ.Get()};
      }
      {
        std::scoped_lock sync(m_mutex);
        return units::degrees_per_second_t{m_gyro_rate_z};
      }
    default:
      break;
  }

  return 0_deg_per_s;
}

units::meters_per_second_squared_t ADIS16470_IMU::GetAccelX() const {
  if (m_simAccelX) {
    return units::meters_per_second_squared_t{m_simAccelX.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::meters_per_second_squared_t{m_accel_x};
}

units::meters_per_second_squared_t ADIS16470_IMU::GetAccelY() const {
  if (m_simAccelY) {
    return units::meters_per_second_squared_t{m_simAccelY.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::meters_per_second_squared_t{m_accel_y};
}

units::meters_per_second_squared_t ADIS16470_IMU::GetAccelZ() const {
  if (m_simAccelZ) {
    return units::meters_per_second_squared_t{m_simAccelZ.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::meters_per_second_squared_t{m_accel_z};
}

units::degree_t ADIS16470_IMU::GetXComplementaryAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_compAngleX};
}

units::degree_t ADIS16470_IMU::GetYComplementaryAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_compAngleY};
}

units::degree_t ADIS16470_IMU::GetXFilteredAccelAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_accelAngleX};
}

units::degree_t ADIS16470_IMU::GetYFilteredAccelAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_accelAngleY};
}

ADIS16470_IMU::IMUAxis ADIS16470_IMU::GetYawAxis() const {
  return m_yaw_axis;
}

ADIS16470_IMU::IMUAxis ADIS16470_IMU::GetPitchAxis() const {
  return m_pitch_axis;
}

ADIS16470_IMU::IMUAxis ADIS16470_IMU::GetRollAxis() const {
  return m_roll_axis;
}

int ADIS16470_IMU::GetPort() const {
  return m_spi_port;
}

void ADIS16470_IMU::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("ADIS16470 IMU");
  builder.AddDoubleProperty(
      "Yaw Angle", [=, this] { return GetAngle(kYaw).value(); }, nullptr);
}
