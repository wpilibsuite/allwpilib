/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Accelerometer.h"

#include <stdint.h>

#include <cassert>
#include <cstdio>
#include <memory>

#include "HALInitializer.h"
#include "hal/ChipObject.h"
#include "hal/HAL.h"

using namespace hal;

// The 7-bit I2C address with a 0 "send" bit
static constexpr uint8_t kSendAddress = (0x1c << 1) | 0;

// The 7-bit I2C address with a 1 "receive" bit
static constexpr uint8_t kReceiveAddress = (0x1c << 1) | 1;

static constexpr uint8_t kControlTxRx = 1;
static constexpr uint8_t kControlStart = 2;
static constexpr uint8_t kControlStop = 4;

static std::unique_ptr<tAccel> accel;
static HAL_AccelerometerRange accelerometerRange;

// Register addresses
enum Register {
  kReg_Status = 0x00,
  kReg_OutXMSB = 0x01,
  kReg_OutXLSB = 0x02,
  kReg_OutYMSB = 0x03,
  kReg_OutYLSB = 0x04,
  kReg_OutZMSB = 0x05,
  kReg_OutZLSB = 0x06,
  kReg_Sysmod = 0x0B,
  kReg_IntSource = 0x0C,
  kReg_WhoAmI = 0x0D,
  kReg_XYZDataCfg = 0x0E,
  kReg_HPFilterCutoff = 0x0F,
  kReg_PLStatus = 0x10,
  kReg_PLCfg = 0x11,
  kReg_PLCount = 0x12,
  kReg_PLBfZcomp = 0x13,
  kReg_PLThsReg = 0x14,
  kReg_FFMtCfg = 0x15,
  kReg_FFMtSrc = 0x16,
  kReg_FFMtThs = 0x17,
  kReg_FFMtCount = 0x18,
  kReg_TransientCfg = 0x1D,
  kReg_TransientSrc = 0x1E,
  kReg_TransientThs = 0x1F,
  kReg_TransientCount = 0x20,
  kReg_PulseCfg = 0x21,
  kReg_PulseSrc = 0x22,
  kReg_PulseThsx = 0x23,
  kReg_PulseThsy = 0x24,
  kReg_PulseThsz = 0x25,
  kReg_PulseTmlt = 0x26,
  kReg_PulseLtcy = 0x27,
  kReg_PulseWind = 0x28,
  kReg_ASlpCount = 0x29,
  kReg_CtrlReg1 = 0x2A,
  kReg_CtrlReg2 = 0x2B,
  kReg_CtrlReg3 = 0x2C,
  kReg_CtrlReg4 = 0x2D,
  kReg_CtrlReg5 = 0x2E,
  kReg_OffX = 0x2F,
  kReg_OffY = 0x30,
  kReg_OffZ = 0x31
};

namespace hal {
namespace init {
void InitializeAccelerometer() {}
}  // namespace init
}  // namespace hal

namespace hal {

static void writeRegister(Register reg, uint8_t data);
static uint8_t readRegister(Register reg);

/**
 * Initialize the accelerometer.
 */
static void initializeAccelerometer() {
  hal::init::CheckInit();
  int32_t status;

  if (!accel) {
    accel.reset(tAccel::create(&status));

    accelerometerRange = HAL_AccelerometerRange::HAL_AccelerometerRange_k2G;

    // Enable I2C
    accel->writeCNFG(1, &status);

    // Set the counter to 100 kbps
    accel->writeCNTR(213, &status);

    // The device identification number should be 0x2a
    assert(readRegister(kReg_WhoAmI) == 0x2a);
  }
}

static void writeRegister(Register reg, uint8_t data) {
  int32_t status = 0;
  uint64_t initialTime;

  accel->writeADDR(kSendAddress, &status);

  // Send a start transmit/receive message with the register address
  accel->writeCNTL(kControlStart | kControlTxRx, &status);
  accel->writeDATO(reg, &status);
  accel->strobeGO(&status);

  // Execute and wait until it's done (up to a millisecond)
  initialTime = HAL_GetFPGATime(&status);
  while (accel->readSTAT(&status) & 1) {
    if (HAL_GetFPGATime(&status) > initialTime + 1000) break;
  }

  // Send a stop transmit/receive message with the data
  accel->writeCNTL(kControlStop | kControlTxRx, &status);
  accel->writeDATO(data, &status);
  accel->strobeGO(&status);

  // Execute and wait until it's done (up to a millisecond)
  initialTime = HAL_GetFPGATime(&status);
  while (accel->readSTAT(&status) & 1) {
    if (HAL_GetFPGATime(&status) > initialTime + 1000) break;
  }
}

static uint8_t readRegister(Register reg) {
  int32_t status = 0;
  uint64_t initialTime;

  // Send a start transmit/receive message with the register address
  accel->writeADDR(kSendAddress, &status);
  accel->writeCNTL(kControlStart | kControlTxRx, &status);
  accel->writeDATO(reg, &status);
  accel->strobeGO(&status);

  // Execute and wait until it's done (up to a millisecond)
  initialTime = HAL_GetFPGATime(&status);
  while (accel->readSTAT(&status) & 1) {
    if (HAL_GetFPGATime(&status) > initialTime + 1000) break;
  }

  // Receive a message with the data and stop
  accel->writeADDR(kReceiveAddress, &status);
  accel->writeCNTL(kControlStart | kControlStop | kControlTxRx, &status);
  accel->strobeGO(&status);

  // Execute and wait until it's done (up to a millisecond)
  initialTime = HAL_GetFPGATime(&status);
  while (accel->readSTAT(&status) & 1) {
    if (HAL_GetFPGATime(&status) > initialTime + 1000) break;
  }

  return accel->readDATI(&status);
}

/**
 * Convert a 12-bit raw acceleration value into a scaled double in units of
 * 1 g-force, taking into account the accelerometer range.
 */
static double unpackAxis(int16_t raw) {
  // The raw value is actually 12 bits, not 16, so we need to propogate the
  // 2's complement sign bit to the unused 4 bits for this to work with
  // negative numbers.
  raw <<= 4;
  raw >>= 4;

  switch (accelerometerRange) {
    case HAL_AccelerometerRange_k2G:
      return raw / 1024.0;
    case HAL_AccelerometerRange_k4G:
      return raw / 512.0;
    case HAL_AccelerometerRange_k8G:
      return raw / 256.0;
    default:
      return 0.0;
  }
}

}  // namespace hal

extern "C" {

void HAL_SetAccelerometerActive(HAL_Bool active) {
  initializeAccelerometer();

  uint8_t ctrlReg1 = readRegister(kReg_CtrlReg1);
  ctrlReg1 &= ~1;  // Clear the existing active bit
  writeRegister(kReg_CtrlReg1, ctrlReg1 | (active ? 1 : 0));
}

void HAL_SetAccelerometerRange(HAL_AccelerometerRange range) {
  initializeAccelerometer();

  accelerometerRange = range;

  uint8_t xyzDataCfg = readRegister(kReg_XYZDataCfg);
  xyzDataCfg &= ~3;  // Clear the existing two range bits
  writeRegister(kReg_XYZDataCfg, xyzDataCfg | range);
}

double HAL_GetAccelerometerX(void) {
  initializeAccelerometer();

  int32_t raw =
      (readRegister(kReg_OutXMSB) << 4) | (readRegister(kReg_OutXLSB) >> 4);
  return unpackAxis(raw);
}

double HAL_GetAccelerometerY(void) {
  initializeAccelerometer();

  int32_t raw =
      (readRegister(kReg_OutYMSB) << 4) | (readRegister(kReg_OutYLSB) >> 4);
  return unpackAxis(raw);
}

double HAL_GetAccelerometerZ(void) {
  initializeAccelerometer();

  int32_t raw =
      (readRegister(kReg_OutZMSB) << 4) | (readRegister(kReg_OutZLSB) >> 4);
  return unpackAxis(raw);
}

}  // extern "C"
