/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "FPGAEncoder.h"

#include "DigitalInternal.h"
#include "PortsInternal.h"
#include "handles/LimitedHandleResource.h"

using namespace hal;

namespace {
struct Encoder {
  tEncoder* encoder;
  uint32_t index;
};
}

static const double DECODING_SCALING_FACTOR = 0.25;

static LimitedHandleResource<HalFPGAEncoderHandle, Encoder, kNumEncoders,
                             HalHandleEnum::FPGAEncoder>
    fpgaEncoderHandles;

extern "C" {
HalFPGAEncoderHandle initializeFPGAEncoder(HalHandle digitalSourceHandleA,
                                           AnalogTriggerType analogTriggerTypeA,
                                           HalHandle digitalSourceHandleB,
                                           AnalogTriggerType analogTriggerTypeB,
                                           bool reverseDirection,
                                           int32_t* index, int32_t* status) {
  bool routingAnalogTriggerA = false;
  uint32_t routingPinA = 0;
  uint8_t routingModuleA = 0;
  bool successA =
      remapDigitalSource(digitalSourceHandleA, analogTriggerTypeA, routingPinA,
                         routingModuleA, routingAnalogTriggerA);
  bool routingAnalogTriggerB = false;
  uint32_t routingPinB = 0;
  uint8_t routingModuleB = 0;
  bool successB =
      remapDigitalSource(digitalSourceHandleB, analogTriggerTypeB, routingPinB,
                         routingModuleB, routingAnalogTriggerB);

  if (!successA || !successB) {
    *status = HAL_HANDLE_ERROR;
    return HAL_INVALID_HANDLE;
  }

  auto handle = fpgaEncoderHandles.Allocate();
  if (handle == HAL_INVALID_HANDLE) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
  }

  auto encoder = fpgaEncoderHandles.Get(handle);
  if (encoder == nullptr) {  // will only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_INVALID_HANDLE;
  }

  encoder->index = static_cast<uint32_t>(getHandleIndex(handle));

  *index = encoder->index;
  // TODO: if (index == ~0ul) { CloneError(quadEncoders); return; }
  encoder->encoder = tEncoder::create(encoder->index, status);
  encoder->encoder->writeConfig_ASource_Module(routingModuleA, status);
  encoder->encoder->writeConfig_ASource_Channel(routingPinA, status);
  encoder->encoder->writeConfig_ASource_AnalogTrigger(routingAnalogTriggerA,
                                                      status);
  encoder->encoder->writeConfig_BSource_Module(routingModuleB, status);
  encoder->encoder->writeConfig_BSource_Channel(routingPinB, status);
  encoder->encoder->writeConfig_BSource_AnalogTrigger(routingAnalogTriggerB,
                                                      status);
  encoder->encoder->strobeReset(status);
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
  encoder->encoder->writeTimerConfig_AverageSize(4, status);

  return handle;
}

void freeFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle,
                     int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  fpgaEncoderHandles.Free(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  delete encoder->encoder;
}

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void resetFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle,
                      int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->strobeReset(status);
}

/**
 * Gets the fpga value from the encoder.
 * The fpga value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current fpga count from the encoder
 */
int32_t getFPGAEncoder(HalFPGAEncoderHandle fpga_encoder_handle,
                       int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->encoder->readOutput_Value(status);
}

/**
 * Returns the period of the most recent pulse.
 * Returns the period of the most recent Encoder pulse in seconds.
 * This method compenstates for the decoding type.
 *
 * @deprecated Use GetRate() in favor of this method.  This returns unscaled
 * periods and GetRate() scales using value from SetDistancePerPulse().
 *
 * @return Period in seconds of the most recent pulse.
 */
double getFPGAEncoderPeriod(HalFPGAEncoderHandle fpga_encoder_handle,
                            int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }
  tEncoder::tTimerOutput output = encoder->encoder->readTimerOutput(status);
  double value;
  if (output.Stalled) {
    // Return infinity
    double zero = 0.0;
    value = 1.0 / zero;
  } else {
    // output.Period is a fixed point number that counts by 2 (24 bits, 25
    // integer bits)
    value = (double)(output.Period << 1) / (double)output.Count;
  }
  double measuredPeriod = value * 2.5e-8;
  return measuredPeriod / DECODING_SCALING_FACTOR;
}

/**
 * Sets the maximum period for stopped detection.
 * Sets the value that represents the maximum period of the Encoder before it
 * will assume that the attached device is stopped. This timeout allows users
 * to determine if the wheels or other shaft has stopped rotating.
 * This method compensates for the decoding type.
 *
 * @deprecated Use SetMinRate() in favor of this method.  This takes unscaled
 * periods and SetMinRate() scales using value from SetDistancePerPulse().
 *
 * @param maxPeriod The maximum time between rising and falling edges before the
 * FPGA will
 * report the device stopped. This is expressed in seconds.
 */
void setFPGAEncoderMaxPeriod(HalFPGAEncoderHandle fpga_encoder_handle,
                             double maxPeriod, int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->writeTimerConfig_StallPeriod(
      (uint32_t)(maxPeriod * 4.0e8 * DECODING_SCALING_FACTOR), status);
}

/**
 * Determine if the encoder is stopped.
 * Using the MaxPeriod value, a boolean is returned that is true if the encoder
 * is considered stopped and false if it is still moving. A stopped encoder is
 * one where the most recent pulse width exceeds the MaxPeriod.
 * @return True if the encoder is considered stopped.
 */
bool getFPGAEncoderStopped(HalFPGAEncoderHandle fpga_encoder_handle,
                           int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return encoder->encoder->readTimerOutput_Stalled(status) != 0;
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool getFPGAEncoderDirection(HalFPGAEncoderHandle fpga_encoder_handle,
                             int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return encoder->encoder->readOutput_Direction(status);
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the
 * correct software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void setFPGAEncoderReverseDirection(HalFPGAEncoderHandle fpga_encoder_handle,
                                    bool reverseDirection, int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void setFPGAEncoderSamplesToAverage(HalFPGAEncoderHandle fpga_encoder_handle,
                                    uint32_t samplesToAverage,
                                    int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (samplesToAverage < 1 || samplesToAverage > 127) {
    *status = PARAMETER_OUT_OF_RANGE;
  }
  encoder->encoder->writeTimerConfig_AverageSize(samplesToAverage, status);
}

/**
 * Get the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
uint32_t getFPGAEncoderSamplesToAverage(
    HalFPGAEncoderHandle fpga_encoder_handle, int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return encoder->encoder->readTimerConfig_AverageSize(status);
}

/**
 * Set an index source for an encoder, which is an input that resets the
 * encoder's count.
 */
void setFPGAEncoderIndexSource(HalFPGAEncoderHandle fpga_encoder_handle,
                               HalHandle digitalSourceHandle,
                               AnalogTriggerType analogTriggerType,
                               bool activeHigh, bool edgeSensitive,
                               int32_t* status) {
  auto encoder = fpgaEncoderHandles.Get(fpga_encoder_handle);
  if (encoder == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  bool routingAnalogTrigger = false;
  uint32_t routingPin = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, analogTriggerType, routingPin,
                         routingModule, routingAnalogTrigger);
  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  encoder->encoder->writeConfig_IndexSource_Channel(routingPin, status);
  encoder->encoder->writeConfig_IndexSource_Module(routingModule, status);
  encoder->encoder->writeConfig_IndexSource_AnalogTrigger(routingAnalogTrigger,
                                                          status);
  encoder->encoder->writeConfig_IndexActiveHigh(activeHigh, status);
  encoder->encoder->writeConfig_IndexEdgeSensitive(edgeSensitive, status);
}
}
