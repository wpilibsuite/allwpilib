/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Encoder.h"

#include "DigitalInternal.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

extern "C" {
struct encoder_t {
  tEncoder* encoder;
  uint32_t index;
};
typedef struct encoder_t Encoder;

static const double DECODING_SCALING_FACTOR = 0.25;
static hal::Resource* quadEncoders = nullptr;

void* initializeEncoder(uint8_t port_a_module, uint32_t port_a_pin,
                        bool port_a_analog_trigger, uint8_t port_b_module,
                        uint32_t port_b_pin, bool port_b_analog_trigger,
                        bool reverseDirection, int32_t* index,
                        int32_t* status) {
  // Initialize encoder structure
  Encoder* encoder = new Encoder();

  remapDigitalSource(port_a_analog_trigger, port_a_pin, port_a_module);
  remapDigitalSource(port_b_analog_trigger, port_b_pin, port_b_module);

  hal::Resource::CreateResourceObject(&quadEncoders, tEncoder::kNumSystems);
  encoder->index = quadEncoders->Allocate("4X Encoder");
  *index = encoder->index;
  // TODO: if (index == ~0ul) { CloneError(quadEncoders); return; }
  encoder->encoder = tEncoder::create(encoder->index, status);
  encoder->encoder->writeConfig_ASource_Module(port_a_module, status);
  encoder->encoder->writeConfig_ASource_Channel(port_a_pin, status);
  encoder->encoder->writeConfig_ASource_AnalogTrigger(port_a_analog_trigger,
                                                      status);
  encoder->encoder->writeConfig_BSource_Module(port_b_module, status);
  encoder->encoder->writeConfig_BSource_Channel(port_b_pin, status);
  encoder->encoder->writeConfig_BSource_AnalogTrigger(port_b_analog_trigger,
                                                      status);
  encoder->encoder->strobeReset(status);
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
  encoder->encoder->writeTimerConfig_AverageSize(4, status);

  return encoder;
}

void freeEncoder(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  if (!encoder) return;
  quadEncoders->Free(encoder->index);
  delete encoder->encoder;
}

/**
 * Reset the Encoder distance to zero.
 * Resets the current count to zero on the encoder.
 */
void resetEncoder(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  encoder->encoder->strobeReset(status);
}

/**
 * Gets the raw value from the encoder.
 * The raw value is the actual count unscaled by the 1x, 2x, or 4x scale
 * factor.
 * @return Current raw count from the encoder
 */
int32_t getEncoder(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
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
double getEncoderPeriod(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
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
void setEncoderMaxPeriod(void* encoder_pointer, double maxPeriod,
                         int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
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
bool getEncoderStopped(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  return encoder->encoder->readTimerOutput_Stalled(status) != 0;
}

/**
 * The last direction the encoder value changed.
 * @return The last direction the encoder value changed.
 */
bool getEncoderDirection(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  return encoder->encoder->readOutput_Direction(status);
}

/**
 * Set the direction sensing for this encoder.
 * This sets the direction sensing on the encoder so that it could count in the
 * correct software direction regardless of the mounting.
 * @param reverseDirection true if the encoder direction should be reversed
 */
void setEncoderReverseDirection(void* encoder_pointer, bool reverseDirection,
                                int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  encoder->encoder->writeConfig_Reverse(reverseDirection, status);
}

/**
 * Set the Samples to Average which specifies the number of samples of the timer
 * to average when calculating the period. Perform averaging to account for
 * mechanical imperfections or as oversampling to increase resolution.
 * @param samplesToAverage The number of samples to average from 1 to 127.
 */
void setEncoderSamplesToAverage(void* encoder_pointer,
                                uint32_t samplesToAverage, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
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
uint32_t getEncoderSamplesToAverage(void* encoder_pointer, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  return encoder->encoder->readTimerConfig_AverageSize(status);
}

/**
 * Set an index source for an encoder, which is an input that resets the
 * encoder's count.
 */
void setEncoderIndexSource(void* encoder_pointer, uint32_t pin,
                           bool analogTrigger, bool activeHigh,
                           bool edgeSensitive, int32_t* status) {
  Encoder* encoder = (Encoder*)encoder_pointer;
  encoder->encoder->writeConfig_IndexSource_Channel((unsigned char)pin, status);
  encoder->encoder->writeConfig_IndexSource_Module((unsigned char)0, status);
  encoder->encoder->writeConfig_IndexSource_AnalogTrigger(analogTrigger,
                                                          status);
  encoder->encoder->writeConfig_IndexActiveHigh(activeHigh, status);
  encoder->encoder->writeConfig_IndexEdgeSensitive(edgeSensitive, status);
}
}
