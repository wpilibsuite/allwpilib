// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 Analog Devices Inc. All Rights Reserved.           */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*                                                                            */
/* Modified by Juan Chong - frcsupport@analog.com                             */
/*----------------------------------------------------------------------------*/

#include "frc/ADIS16448_IMU.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>

#include <fmt/format.h>
#include <hal/HAL.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/Errors.h"
#include "frc/MathUtil.h"
#include "frc/SPI.h"
#include "frc/Timer.h"

/* Helpful conversion functions */
static inline uint16_t BuffToUShort(const uint32_t* buf) {
  return (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
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

ADIS16448_IMU::ADIS16448_IMU()
    : ADIS16448_IMU(kZ, SPI::Port::kMXP, CalibrationTime::_512ms) {}

ADIS16448_IMU::ADIS16448_IMU(IMUAxis yaw_axis, SPI::Port port,
                             CalibrationTime cal_time)
    : m_yaw_axis(yaw_axis),
      m_spi_port(port),
      m_simDevice("Gyro:ADIS16448", port) {
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
    DigitalOutput* reset_out = new DigitalOutput(18);  // Drive MXP DIO8 low
    Wait(10_ms);
    delete reset_out;
    m_reset_in = new DigitalInput(18);  // Set MXP DIO8 high
    Wait(500_ms);                       // Wait for reset to complete

    ConfigCalTime(cal_time);

    if (!SwitchToStandardSPI()) {
      return;
    }
    bool needsFlash = false;
    // Set IMU internal decimation to 1 (output data rate of 819.2 SPS / (1 + 1)
    // = 409.6Hz), output bandwidth = 204.8Hz
    if (ReadRegister(SMPL_PRD) != 0x0001) {
      WriteRegister(SMPL_PRD, 0x0001);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: SMPL_PRD register configuration inconsistent! Scheduling "
          "flash update.");
    }

    // Set data ready polarity (LOW = Good Data) on DIO1 (PWM0 on MXP)
    if (ReadRegister(MSC_CTRL) != 0x0016) {
      WriteRegister(MSC_CTRL, 0x0016);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: MSC_CTRL register configuration inconsistent! Scheduling "
          "flash update.");
    }

    // Disable IMU internal Bartlett filter (204Hz bandwidth is sufficient) and
    // set IMU scale factor (range)
    if (ReadRegister(SENS_AVG) != 0x0400) {
      WriteRegister(SENS_AVG, 0x0400);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: SENS_AVG register configuration inconsistent! Scheduling "
          "flash update.");
    }
    // Clear offset registers
    if (ReadRegister(XGYRO_OFF) != 0x0000) {
      WriteRegister(XGYRO_OFF, 0x0000);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: XGYRO_OFF register configuration inconsistent! "
          "Scheduling flash update.");
    }

    if (ReadRegister(YGYRO_OFF) != 0x0000) {
      WriteRegister(YGYRO_OFF, 0x0000);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: YGYRO_OFF register configuration inconsistent! "
          "Scheduling flash update.");
    }

    if (ReadRegister(ZGYRO_OFF) != 0x0000) {
      WriteRegister(ZGYRO_OFF, 0x0000);
      needsFlash = true;
      REPORT_WARNING(
          "ADIS16448: ZGYRO_OFF register configuration inconsistent! "
          "Scheduling flash update.");
    }

    // If any registers on the IMU don't match the config, trigger a flash
    // update
    if (needsFlash) {
      REPORT_WARNING(
          "ADIS16448: Register configuration changed! Starting IMU flash "
          "update.");
      WriteRegister(GLOB_CMD, 0x0008);
      // Wait long enough for the flash update to finish (75ms minimum as per
      // the datasheet)
      Wait(0.5_s);
      REPORT_WARNING("ADIS16448: Flash update finished!");
    } else {
      REPORT_WARNING(
          "ADIS16448: Flash and RAM configuration consistent. No flash update "
          "required!");
    }

    if (!SwitchToAutoSPI()) {
      return;
    }
    // Notify DS that IMU calibration delay is active
    REPORT_WARNING("ADIS16448: Starting initial calibration delay.");
    // Wait for whatever time the user set as the start-up delay
    Wait(static_cast<double>(m_calibration_time) * 1.2_s);
    // Execute calibration routine
    Calibrate();
    // Reset accumulated offsets
    Reset();
    // Tell the acquire loop that we're done starting up
    m_start_up_mode = false;

    // Let the user know the IMU was initialized successfully
    REPORT_WARNING("ADIS16448 IMU Successfully Initialized!");

    // TODO: Find what the proper pin is to turn this LED
    // Drive MXP PWM5 (IMU ready LED) low (active low)
    m_status_led = new DigitalOutput(19);
  }

  // Report usage and post data to DS
  HAL_Report(HALUsageReporting::kResourceType_ADIS16448, 0);

  wpi::SendableRegistry::AddLW(this, "ADIS16448", port);
  m_connected = true;
}

ADIS16448_IMU::ADIS16448_IMU(ADIS16448_IMU&& other)
    : m_reset_in{std::move(other.m_reset_in)},
      m_status_led{std::move(other.m_status_led)},
      m_yaw_axis{std::move(other.m_yaw_axis)},
      m_gyro_rate_x{std::move(other.m_gyro_rate_x)},
      m_gyro_rate_y{std::move(other.m_gyro_rate_y)},
      m_gyro_rate_z{std::move(other.m_gyro_rate_z)},
      m_accel_x{std::move(other.m_accel_x)},
      m_accel_y{std::move(other.m_accel_y)},
      m_accel_z{std::move(other.m_accel_z)},
      m_mag_x{std::move(other.m_mag_x)},
      m_mag_y{std::move(other.m_mag_y)},
      m_mag_z{std::move(other.m_mag_z)},
      m_baro{std::move(other.m_baro)},
      m_temp{std::move(other.m_temp)},
      m_dt{std::move(other.m_dt)},
      m_alpha{std::move(other.m_alpha)},
      m_compAngleX{std::move(other.m_compAngleX)},
      m_compAngleY{std::move(other.m_compAngleY)},
      m_accelAngleX{std::move(other.m_accelAngleX)},
      m_accelAngleY{std::move(other.m_accelAngleY)},
      m_offset_buffer{other.m_offset_buffer},
      m_gyro_rate_offset_x{std::move(other.m_gyro_rate_offset_x)},
      m_gyro_rate_offset_y{std::move(other.m_gyro_rate_offset_y)},
      m_gyro_rate_offset_z{std::move(other.m_gyro_rate_offset_z)},
      m_avg_size{std::move(other.m_avg_size)},
      m_accum_count{std::move(other.m_accum_count)},
      m_integ_gyro_angle_x{std::move(other.m_integ_gyro_angle_x)},
      m_integ_gyro_angle_y{std::move(other.m_integ_gyro_angle_y)},
      m_integ_gyro_angle_z{std::move(other.m_integ_gyro_angle_z)},
      m_thread_active{other.m_thread_active.load()},
      m_first_run{other.m_first_run.load()},
      m_thread_idle{other.m_thread_idle.load()},
      m_start_up_mode{other.m_start_up_mode.load()},
      m_auto_configured{std::move(other.m_auto_configured)},
      m_spi_port{std::move(other.m_spi_port)},
      m_calibration_time{std::move(other.m_calibration_time)},
      m_spi{std::move(other.m_spi)},
      m_auto_interrupt{std::move(other.m_auto_interrupt)},
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

ADIS16448_IMU& ADIS16448_IMU::operator=(ADIS16448_IMU&& other) {
  if (this == &other) {
    return *this;
  }

  std::swap(this->m_reset_in, other.m_reset_in);
  std::swap(this->m_status_led, other.m_status_led);
  std::swap(this->m_yaw_axis, other.m_yaw_axis);
  std::swap(this->m_gyro_rate_x, other.m_gyro_rate_x);
  std::swap(this->m_gyro_rate_y, other.m_gyro_rate_y);
  std::swap(this->m_gyro_rate_z, other.m_gyro_rate_z);
  std::swap(this->m_accel_x, other.m_accel_x);
  std::swap(this->m_accel_y, other.m_accel_y);
  std::swap(this->m_accel_z, other.m_accel_z);
  std::swap(this->m_mag_x, other.m_mag_x);
  std::swap(this->m_mag_y, other.m_mag_y);
  std::swap(this->m_mag_z, other.m_mag_z);
  std::swap(this->m_baro, other.m_baro);
  std::swap(this->m_temp, other.m_temp);
  std::swap(this->m_dt, other.m_dt);
  std::swap(this->m_alpha, other.m_alpha);
  std::swap(this->m_compAngleX, other.m_compAngleX);
  std::swap(this->m_compAngleY, other.m_compAngleY);
  std::swap(this->m_accelAngleX, other.m_accelAngleX);
  std::swap(this->m_accelAngleY, other.m_accelAngleY);
  std::swap(this->m_offset_buffer, other.m_offset_buffer);
  std::swap(this->m_gyro_rate_offset_x, other.m_gyro_rate_offset_x);
  std::swap(this->m_gyro_rate_offset_y, other.m_gyro_rate_offset_y);
  std::swap(this->m_gyro_rate_offset_z, other.m_gyro_rate_offset_z);
  std::swap(this->m_avg_size, other.m_avg_size);
  std::swap(this->m_accum_count, other.m_accum_count);
  std::swap(this->m_integ_gyro_angle_x, other.m_integ_gyro_angle_x);
  std::swap(this->m_integ_gyro_angle_y, other.m_integ_gyro_angle_y);
  std::swap(this->m_integ_gyro_angle_z, other.m_integ_gyro_angle_z);
  this->m_thread_active = other.m_thread_active.load();
  this->m_first_run = other.m_first_run.load();
  this->m_thread_idle = other.m_thread_idle.load();
  this->m_start_up_mode = other.m_start_up_mode.load();
  std::swap(this->m_auto_configured, other.m_auto_configured);
  std::swap(this->m_spi_port, other.m_spi_port);
  std::swap(this->m_calibration_time, other.m_calibration_time);
  std::swap(this->m_spi, other.m_spi);
  std::swap(this->m_auto_interrupt, other.m_auto_interrupt);
  std::swap(this->m_connected, other.m_connected);
  std::swap(this->m_acquire_task, other.m_acquire_task);
  std::swap(this->m_simDevice, other.m_simDevice);
  std::swap(this->m_simConnected, other.m_simConnected);
  std::swap(this->m_simGyroAngleX, other.m_simGyroAngleX);
  std::swap(this->m_simGyroAngleY, other.m_simGyroAngleY);
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

bool ADIS16448_IMU::IsConnected() const {
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
bool ADIS16448_IMU::SwitchToStandardSPI() {
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
      // Sometimes data magically reappears, so we have to check
      // the buffer size a couple of times to be sure we got it all. Yuck.
      uint32_t trashBuffer[200];
      Wait(100_ms);
      int data_count = m_spi->ReadAutoReceivedData(trashBuffer, 0, 0_s);
      while (data_count > 0) {
        // Dequeue 200 at a time, or the remainder of the buffer if less than
        // 200
        m_spi->ReadAutoReceivedData(trashBuffer, (std::min)(200, data_count),
                                    0_s);
        // Update remaining buffer count
        data_count = m_spi->ReadAutoReceivedData(trashBuffer, 0, 0_s);
      }
    }
  }
  if (m_spi == nullptr) {
    m_spi = new SPI(m_spi_port);
    m_spi->SetClockRate(1000000);
    m_spi->SetMode(frc::SPI::Mode::kMode3);
    m_spi->SetChipSelectActiveLow();
  }
  ReadRegister(PROD_ID);  // Dummy read
  // Validate the product ID
  uint16_t prod_id = ReadRegister(PROD_ID);
  if (prod_id != 16448) {
    REPORT_ERROR(
        fmt::format("Could not find ADIS16448; got product ID {}", prod_id));
    Close();
    return false;
  }
  return true;
}

void ADIS16448_IMU::InitOffsetBuffer(int size) {
  // avoid exceptions in the case of bad arguments
  if (size < 1) {
    size = 1;
  }

  // set average size to size (correct bad values)
  m_avg_size = size;

  // resize vector
  if (m_offset_buffer != nullptr) {
    delete[] m_offset_buffer;
  }
  m_offset_buffer = new OffsetData[size];

  // set accumulate count to 0
  m_accum_count = 0;
}

/**
 * This function switches the active SPI port to auto SPI and is used primarily
 *when exiting standard SPI. Auto SPI is required to asynchronously read data
 *over SPI as it utilizes special FPGA hardware to react to an external data
 *ready (GPIO) input. Data captured using auto SPI is buffered in the FPGA and
 *can be read by the CPU asynchronously. Standard SPI transactions are
 * impossible on the selected SPI port once auto SPI is enabled. The stall
 *settings, GPIO interrupt pin, and data packet settings used in this function
 *are hard-coded to work only with the ADIS16448 IMU.
 **/
bool ADIS16448_IMU::SwitchToAutoSPI() {
  // No SPI port has been set up. Go set one up first.
  if (m_spi == nullptr && !SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to start/restart auto SPI");
    return false;
  }
  // Only set up the interrupt if needed.
  if (m_auto_interrupt == nullptr) {
    m_auto_interrupt = new DigitalInput(10);
  }
  // The auto SPI controller gets angry if you try to set up two instances on
  // one bus.
  if (!m_auto_configured) {
    m_spi->InitAuto(8200);
    m_auto_configured = true;
  }
  // Set auto SPI packet data and size
  m_spi->SetAutoTransmitData({{GLOB_CMD}}, 27);
  // Configure auto stall time
  m_spi->ConfigureAutoStall(HAL_SPI_kMXP, 100, 1000, 255);
  // Kick off DMA SPI (Note: Device configuration impossible after SPI DMA is
  // activated)
  m_spi->StartAutoTrigger(*m_auto_interrupt, true, false);
  // Check to see if the acquire thread is running. If not, kick one off.
  if (!m_thread_idle) {
    m_first_run = true;
    m_thread_active = true;
    // Set up circular buffer
    InitOffsetBuffer(m_avg_size);
    // Kick off acquire thread
    m_acquire_task = std::thread(&ADIS16448_IMU::Acquire, this);
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

int ADIS16448_IMU::ConfigCalTime(CalibrationTime new_cal_time) {
  if (m_calibration_time == new_cal_time) {
    return 1;
  } else {
    m_calibration_time = new_cal_time;
    m_avg_size = static_cast<uint16_t>(m_calibration_time) * 819;
    InitOffsetBuffer(m_avg_size);
    return 0;
  }
}

void ADIS16448_IMU::Calibrate() {
  std::scoped_lock sync(m_mutex);
  // Calculate the running average
  int gyroAverageSize = (std::min)(m_accum_count, m_avg_size);
  double accum_gyro_rate_x = 0.0;
  double accum_gyro_rate_y = 0.0;
  double accum_gyro_rate_z = 0.0;
  for (int i = 0; i < gyroAverageSize; i++) {
    accum_gyro_rate_x += m_offset_buffer[i].gyro_rate_x;
    accum_gyro_rate_y += m_offset_buffer[i].gyro_rate_y;
    accum_gyro_rate_z += m_offset_buffer[i].gyro_rate_z;
  }
  m_gyro_rate_offset_x = accum_gyro_rate_x / gyroAverageSize;
  m_gyro_rate_offset_y = accum_gyro_rate_y / gyroAverageSize;
  m_gyro_rate_offset_z = accum_gyro_rate_z / gyroAverageSize;
  m_integ_gyro_angle_x = 0.0;
  m_integ_gyro_angle_y = 0.0;
  m_integ_gyro_angle_z = 0.0;
}

uint16_t ADIS16448_IMU::ReadRegister(uint8_t reg) {
  uint8_t buf[2];
  buf[0] = reg & 0x7f;
  buf[1] = 0;

  m_spi->Write(buf, 2);
  m_spi->Read(false, buf, 2);

  return (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
}

/**
 * This function writes an unsigned, 16-bit value into adjacent 8-bit addresses
 * via SPI. The upper and lower bytes that make up the 16-bit value are split
 * into two unsigned, 8-bit values and written to the upper and lower addresses
 * of the specified register value. Only the lower (base) address must be
 * specified. This function assumes the controller is set to standard SPI mode.
 **/
void ADIS16448_IMU::WriteRegister(uint8_t reg, uint16_t val) {
  uint8_t buf[2];
  buf[0] = 0x80 | reg;
  buf[1] = val & 0xff;
  m_spi->Write(buf, 2);
  buf[0] = 0x81 | reg;
  buf[1] = val >> 8;
  m_spi->Write(buf, 2);
}

void ADIS16448_IMU::Reset() {
  std::scoped_lock sync(m_mutex);
  m_integ_gyro_angle_x = 0.0;
  m_integ_gyro_angle_y = 0.0;
  m_integ_gyro_angle_z = 0.0;
}

void ADIS16448_IMU::Close() {
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
  delete[] m_offset_buffer;
}

ADIS16448_IMU::~ADIS16448_IMU() {
  Close();
}

void ADIS16448_IMU::Acquire() {
  // Set data packet length
  const int dataset_len = 29;  // 18 data points + timestamp
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
      // Read number of bytes currently stored in the buffer
      int data_count = m_spi->ReadAutoReceivedData(buffer, 0, 0_s);
      // Check if frame is incomplete
      int data_remainder = data_count % dataset_len;
      // Remove incomplete data from read count
      int data_to_read = data_count - data_remainder;
      // Want to cap the data to read in a single read at the buffer size
      if (data_to_read > BUFFER_SIZE) {
        REPORT_WARNING(
            "ADIS16448 data processing thread overrun has occurred!");
        data_to_read = BUFFER_SIZE - (BUFFER_SIZE % dataset_len);
      }
      // Read data from DMA buffer
      m_spi->ReadAutoReceivedData(buffer, data_to_read, 0_s);

      // Could be multiple data sets in the buffer. Handle each one.
      for (int i = 0; i < data_to_read; i += dataset_len) {
        // Calculate CRC-16 on each data packet
        uint16_t calc_crc = 0xFFFF;  // Starting word
        // Cycle through XYZ GYRO, XYZ ACCEL, XYZ MAG, BARO, TEMP (Ignore Status
        // & CRC)
        for (int k = 5; k < 27; k += 2) {
          // Process LSB
          uint8_t byte = static_cast<uint8_t>(buffer[i + k + 1]);
          calc_crc = (calc_crc >> 8) ^ m_adiscrc[(calc_crc & 0xFF) ^ byte];
          // Process MSB
          byte = static_cast<uint8_t>(buffer[i + k]);
          calc_crc = (calc_crc >> 8) ^ m_adiscrc[(calc_crc & 0xFF) ^ byte];
        }
        // Complement
        calc_crc = ~calc_crc;
        // Flip LSB & MSB
        calc_crc = static_cast<uint16_t>((calc_crc << 8) | (calc_crc >> 8));
        // Extract DUT CRC from data buffer
        uint16_t imu_crc = BuffToUShort(&buffer[i + 27]);

        // Compare calculated vs read CRC. Don't update outputs or dt if CRC-16
        // is bad
        if (calc_crc == imu_crc) {
          // Timestamp is at buffer[i]
          m_dt = (buffer[i] - previous_timestamp) / 1000000.0;
          // Scale sensor data
          double gyro_rate_x = BuffToShort(&buffer[i + 5]) * 0.04;
          double gyro_rate_y = BuffToShort(&buffer[i + 7]) * 0.04;
          double gyro_rate_z = BuffToShort(&buffer[i + 9]) * 0.04;
          double accel_x = BuffToShort(&buffer[i + 11]) * 0.833;
          double accel_y = BuffToShort(&buffer[i + 13]) * 0.833;
          double accel_z = BuffToShort(&buffer[i + 15]) * 0.833;
          double mag_x = BuffToShort(&buffer[i + 17]) * 0.1429;
          double mag_y = BuffToShort(&buffer[i + 19]) * 0.1429;
          double mag_z = BuffToShort(&buffer[i + 21]) * 0.1429;
          double baro = BuffToShort(&buffer[i + 23]) * 0.02;
          double temp = BuffToShort(&buffer[i + 25]) * 0.07386 + 31.0;

          // Convert scaled sensor data to SI units
          double gyro_rate_x_si = gyro_rate_x * kDegToRad;
          double gyro_rate_y_si = gyro_rate_y * kDegToRad;
          // double gyro_rate_z_si = gyro_rate_z * kDegToRad;
          double accel_x_si = accel_x * kGrav;
          double accel_y_si = accel_y * kGrav;
          double accel_z_si = accel_z * kGrav;
          // Store timestamp for next iteration
          previous_timestamp = buffer[i];
          // Calculate alpha for use with the complementary filter
          m_alpha = kTau / (kTau + m_dt);
          // Run inclinometer calculations
          double accelAngleX =
              atan2f(-accel_x_si, std::hypotf(accel_y_si, -accel_z_si));
          double accelAngleY =
              atan2f(accel_y_si, std::hypotf(-accel_x_si, -accel_z_si));
          // Calculate complementary filter
          if (m_first_run) {
            compAngleX = accelAngleX;
            compAngleY = accelAngleY;
          } else {
            accelAngleX = FormatAccelRange(accelAngleX, -accel_z_si);
            accelAngleY = FormatAccelRange(accelAngleY, -accel_z_si);
            compAngleX =
                CompFilterProcess(compAngleX, accelAngleX, -gyro_rate_y_si);
            compAngleY =
                CompFilterProcess(compAngleY, accelAngleY, -gyro_rate_x_si);
          }

          // Update global variables and state
          {
            std::scoped_lock sync(m_mutex);
            // Ignore first, integrated sample
            if (m_first_run) {
              m_integ_gyro_angle_x = 0.0;
              m_integ_gyro_angle_y = 0.0;
              m_integ_gyro_angle_z = 0.0;
            } else {
              // Accumulate gyro for offset calibration
              // Add most recent sample data to buffer
              int bufferAvgIndex = m_accum_count % m_avg_size;
              m_offset_buffer[bufferAvgIndex] =
                  OffsetData{gyro_rate_x, gyro_rate_y, gyro_rate_z};
              // Increment counter
              m_accum_count++;
            }
            // Don't post accumulated data to the global variables until an
            // initial gyro offset has been calculated
            if (!m_start_up_mode) {
              m_gyro_rate_x = gyro_rate_x;
              m_gyro_rate_y = gyro_rate_y;
              m_gyro_rate_z = gyro_rate_z;
              m_accel_x = accel_x;
              m_accel_y = accel_y;
              m_accel_z = accel_z;
              m_mag_x = mag_x;
              m_mag_y = mag_y;
              m_mag_z = mag_z;
              m_baro = baro;
              m_temp = temp;
              m_compAngleX = compAngleX * kRadToDeg;
              m_compAngleY = compAngleY * kRadToDeg;
              m_accelAngleX = accelAngleX * kRadToDeg;
              m_accelAngleY = accelAngleY * kRadToDeg;
              // Accumulate gyro for angle integration and publish to global
              // variables
              m_integ_gyro_angle_x +=
                  (gyro_rate_x - m_gyro_rate_offset_x) * m_dt;
              m_integ_gyro_angle_y +=
                  (gyro_rate_y - m_gyro_rate_offset_y) * m_dt;
              m_integ_gyro_angle_z +=
                  (gyro_rate_z - m_gyro_rate_offset_z) * m_dt;
            }
          }
          m_first_run = false;
        }
      }
    } else {
      m_thread_idle = true;
      previous_timestamp = 0.0;
      compAngleX = 0.0;
      compAngleY = 0.0;
    }
  }
}

/* Complementary filter functions */
double ADIS16448_IMU::FormatFastConverge(double compAngle, double accAngle) {
  if (compAngle > accAngle + std::numbers::pi) {
    compAngle = compAngle - 2.0 * std::numbers::pi;
  } else if (accAngle > compAngle + std::numbers::pi) {
    compAngle = compAngle + 2.0 * std::numbers::pi;
  }
  return compAngle;
}

double ADIS16448_IMU::FormatAccelRange(double accelAngle, double accelZ) {
  if (accelZ < 0.0) {
    accelAngle = std::numbers::pi - accelAngle;
  } else if (accelZ > 0.0 && accelAngle < 0.0) {
    accelAngle = 2.0 * std::numbers::pi + accelAngle;
  }
  return accelAngle;
}

double ADIS16448_IMU::CompFilterProcess(double compAngle, double accelAngle,
                                        double omega) {
  compAngle = FormatFastConverge(compAngle, accelAngle);
  compAngle =
      m_alpha * (compAngle + omega * m_dt) + (1.0 - m_alpha) * accelAngle;
  return frc::InputModulus(compAngle, -std::numbers::pi, std::numbers::pi);
}

int ADIS16448_IMU::ConfigDecRate(uint16_t decimationRate) {
  // Switches the active SPI port to standard SPI mode, writes a new value to
  // the DECIMATE register in the IMU, adjusts the sample scale factor, and
  // re-enables auto SPI.
  if (!SwitchToStandardSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure standard SPI.");
    return 2;
  }

  // Check max
  if (decimationRate > 9) {
    REPORT_ERROR(
        "Attempted to write an invalid decimation value. Capping at 9");
    decimationRate = 9;
  }

  // Shift decimation setting to correct position and select internal sync
  uint16_t writeValue = (decimationRate << 8) | 0x1;

  // Apply to IMU
  WriteRegister(SMPL_PRD, writeValue);

  // Perform read back to verify write
  uint16_t readbackValue = ReadRegister(SMPL_PRD);

  // Throw error for invalid write
  if (readbackValue != writeValue) {
    REPORT_ERROR("ADIS16448 SMPL_PRD write failed.");
  }

  if (!SwitchToAutoSPI()) {
    REPORT_ERROR("Failed to configure/reconfigure auto SPI.");
    return 2;
  }
  return 0;
}

units::degree_t ADIS16448_IMU::GetAngle() const {
  switch (m_yaw_axis) {
    case kX:
      return GetGyroAngleX();
    case kY:
      return GetGyroAngleY();
    case kZ:
      return GetGyroAngleZ();
    default:
      return 0_deg;
  }
}

units::degrees_per_second_t ADIS16448_IMU::GetRate() const {
  switch (m_yaw_axis) {
    case kX:
      return GetGyroRateX();
    case kY:
      return GetGyroRateY();
    case kZ:
      return GetGyroRateZ();
    default:
      return 0_deg_per_s;
  }
}

units::degree_t ADIS16448_IMU::GetGyroAngleX() const {
  if (m_simGyroAngleX) {
    return units::degree_t{m_simGyroAngleX.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_integ_gyro_angle_x};
}

units::degree_t ADIS16448_IMU::GetGyroAngleY() const {
  if (m_simGyroAngleY) {
    return units::degree_t{m_simGyroAngleY.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_integ_gyro_angle_y};
}

units::degree_t ADIS16448_IMU::GetGyroAngleZ() const {
  if (m_simGyroAngleZ) {
    return units::degree_t{m_simGyroAngleZ.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_integ_gyro_angle_z};
}

units::degrees_per_second_t ADIS16448_IMU::GetGyroRateX() const {
  if (m_simGyroRateX) {
    return units::degrees_per_second_t{m_simGyroRateX.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degrees_per_second_t{m_gyro_rate_x};
}

units::degrees_per_second_t ADIS16448_IMU::GetGyroRateY() const {
  if (m_simGyroRateY) {
    return units::degrees_per_second_t{m_simGyroRateY.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degrees_per_second_t{m_gyro_rate_y};
}

units::degrees_per_second_t ADIS16448_IMU::GetGyroRateZ() const {
  if (m_simGyroRateZ) {
    return units::degrees_per_second_t{m_simGyroRateZ.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return units::degrees_per_second_t{m_gyro_rate_z};
}

units::meters_per_second_squared_t ADIS16448_IMU::GetAccelX() const {
  if (m_simAccelX) {
    return units::meters_per_second_squared_t{m_simAccelX.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return m_accel_x * 9.81_mps_sq;
}

units::meters_per_second_squared_t ADIS16448_IMU::GetAccelY() const {
  if (m_simAccelY) {
    return units::meters_per_second_squared_t{m_simAccelY.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return m_accel_y * 9.81_mps_sq;
}

units::meters_per_second_squared_t ADIS16448_IMU::GetAccelZ() const {
  if (m_simAccelZ) {
    return units::meters_per_second_squared_t{m_simAccelZ.Get()};
  }
  std::scoped_lock sync(m_mutex);
  return m_accel_z * 9.81_mps_sq;
}

units::tesla_t ADIS16448_IMU::GetMagneticFieldX() const {
  std::scoped_lock sync(m_mutex);
  return units::gauss_t{m_mag_x * 1e-3};
}

units::tesla_t ADIS16448_IMU::GetMagneticFieldY() const {
  std::scoped_lock sync(m_mutex);
  return units::gauss_t{m_mag_y * 1e-3};
}

units::tesla_t ADIS16448_IMU::GetMagneticFieldZ() const {
  std::scoped_lock sync(m_mutex);
  return units::gauss_t{m_mag_z * 1e-3};
}

units::degree_t ADIS16448_IMU::GetXComplementaryAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_compAngleX};
}

units::degree_t ADIS16448_IMU::GetYComplementaryAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_compAngleY};
}

units::degree_t ADIS16448_IMU::GetXFilteredAccelAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_accelAngleX};
}

units::degree_t ADIS16448_IMU::GetYFilteredAccelAngle() const {
  std::scoped_lock sync(m_mutex);
  return units::degree_t{m_accelAngleY};
}

units::pounds_per_square_inch_t ADIS16448_IMU::GetBarometricPressure() const {
  std::scoped_lock sync(m_mutex);
  return units::mbar_t{m_baro};
}

units::celsius_t ADIS16448_IMU::GetTemperature() const {
  std::scoped_lock sync(m_mutex);
  return units::celsius_t{m_temp};
}

ADIS16448_IMU::IMUAxis ADIS16448_IMU::GetYawAxis() const {
  return m_yaw_axis;
}

int ADIS16448_IMU::SetYawAxis(IMUAxis yaw_axis) {
  if (m_yaw_axis == yaw_axis) {
    return 1;
  } else {
    m_yaw_axis = yaw_axis;
    Reset();
    return 0;
  }
}

int ADIS16448_IMU::GetPort() const {
  return m_spi_port;
}

void ADIS16448_IMU::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("ADIS16448 IMU");
  builder.AddDoubleProperty(
      "Yaw Angle", [=, this] { return GetAngle().value(); }, nullptr);
}
