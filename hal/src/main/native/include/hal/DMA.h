// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_dma DMA Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The DMA Read Status.
 */
HAL_ENUM(HAL_DMAReadStatus) {
  HAL_DMA_OK = 1,
  HAL_DMA_TIMEOUT = 2,
  HAL_DMA_ERROR = 3,
};

/**
 * Buffer for containing all DMA data for a specific sample.
 */
struct HAL_DMASample {
  uint32_t readBuffer[74];
  int32_t channelOffsets[22];
  uint64_t timeStamp;
  uint32_t captureSize;
  uint8_t triggerChannels;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an object for performing DMA transfers.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the created dma handle
 */
HAL_DMAHandle HAL_InitializeDMA(int32_t* status);

/**
 * Frees a DMA object.
 *
 * @param handle the dma handle
 */
void HAL_FreeDMA(HAL_DMAHandle handle);

/**
 * Pauses or unpauses a DMA transfer.
 *
 * This can only be called while DMA is running.
 *
 * @param[in] handle  the dma handle
 * @param[in] pause   true to pause transfers, false to resume.
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetDMAPause(HAL_DMAHandle handle, HAL_Bool pause, int32_t* status);

/**
 * Sets DMA transfers to occur at a specific timed interval.
 *
 * This will remove any external triggers. Only timed or external is supported.
 *
 * Only 1 timed period is supported.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle        the dma handle
 * @param[in] periodSeconds the period to trigger in seconds
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetDMATimedTrigger(HAL_DMAHandle handle, double periodSeconds,
                            int32_t* status);

/**
 * Sets DMA transfers to occur at a specific timed interval in FPGA cycles.
 *
 * This will remove any external triggers. Only timed or external is supported.
 *
 * Only 1 timed period is supported
 *
 * The FPGA currently runs at 40 MHz, but this can change.
 * HAL_GetSystemClockTicksPerMicrosecond can be used to get a computable value
 * for this.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] cycles the period to trigger in FPGA cycles
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetDMATimedTriggerCycles(HAL_DMAHandle handle, uint32_t cycles,
                                  int32_t* status);

/**
 * Adds position data for an encoder to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] encoderHandle the encoder to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMAEncoder(HAL_DMAHandle handle, HAL_EncoderHandle encoderHandle,
                       int32_t* status);

/**
 * Adds timer data for an encoder to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] encoderHandle the encoder to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMAEncoderPeriod(HAL_DMAHandle handle,
                             HAL_EncoderHandle encoderHandle, int32_t* status);

/**
 * Adds position data for an counter to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] counterHandle the counter to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMACounter(HAL_DMAHandle handle, HAL_CounterHandle counterHandle,
                       int32_t* status);

/**
 * Adds timer data for an counter to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] counterHandle the counter to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMACounterPeriod(HAL_DMAHandle handle,
                             HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Adds a digital source to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] digitalSourceHandle the digital source to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMADigitalSource(HAL_DMAHandle handle,
                             HAL_Handle digitalSourceHandle, int32_t* status);

/**
 * Adds an analog input to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] aInHandle the analog input to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMAAnalogInput(HAL_DMAHandle handle,
                           HAL_AnalogInputHandle aInHandle, int32_t* status);

/**
 * Adds averaged data of an analog input to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] aInHandle the analog input to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMAAveragedAnalogInput(HAL_DMAHandle handle,
                                   HAL_AnalogInputHandle aInHandle,
                                   int32_t* status);

/**
 * Adds accumulator data of an analog input to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] aInHandle the analog input to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMAAnalogAccumulator(HAL_DMAHandle handle,
                                 HAL_AnalogInputHandle aInHandle,
                                 int32_t* status);

/**
 * Adds a duty cycle input to be collected by DMA.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[in] dutyCycleHandle the duty cycle input to add
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_AddDMADutyCycle(HAL_DMAHandle handle,
                         HAL_DutyCycleHandle dutyCycleHandle, int32_t* status);

/**
 * Sets DMA transfers to occur on an external trigger.
 *
 * This will remove any timed trigger set. Only timed or external is supported.
 *
 * Up to 8 external triggers are currently supported.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle              the dma handle
 * @param[in] digitalSourceHandle the digital source handle (either a
 *                                HAL_AnalogTriggerHandle or a
 *                                HAL_DigitalHandle)
 * @param[in] analogTriggerType   the analog trigger type if the source is an
 *                                analog trigger
 * @param[in] rising              true to trigger on rising edge
 * @param[in] falling             true to trigger on falling edge
 * @param[out] status             Error status variable. 0 on success.
 * @return the index of the trigger
 */
int32_t HAL_SetDMAExternalTrigger(HAL_DMAHandle handle,
                                  HAL_Handle digitalSourceHandle,
                                  HAL_AnalogTriggerType analogTriggerType,
                                  HAL_Bool rising, HAL_Bool falling,
                                  int32_t* status);

/**
 * Clear all sensors from the DMA collection list.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ClearDMASensors(HAL_DMAHandle handle, int32_t* status);

/**
 * Clear all external triggers from the DMA trigger list.
 *
 * This can only be called if DMA is not started.
 *
 * @param[in] handle the dma handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ClearDMAExternalTriggers(HAL_DMAHandle handle, int32_t* status);

/**
 * Starts DMA Collection.
 *
 * @param[in] handle the dma handle
 * @param[in] queueDepth the number of objects to be able to queue
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_StartDMA(HAL_DMAHandle handle, int32_t queueDepth, int32_t* status);

/**
 * Stops DMA Collection.
 *
 * @param[in] handle the dma handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_StopDMA(HAL_DMAHandle handle, int32_t* status);

/**
 * Gets the direct pointer to the DMA object.
 *
 * This is only to be used if absolute maximum performance is required. This
 * will only be valid until the handle is freed.
 *
 * @param handle the dma handle
 */
void* HAL_GetDMADirectPointer(HAL_DMAHandle handle);

/**
 * Reads a DMA sample using a direct DMA pointer.
 *
 * See HAL_ReadDMA for full documentation.
 *
 * @param[in] dmaPointer     direct DMA pointer
 * @param[in] dmaSample      the sample object to place data into
 * @param[in] timeoutSeconds the time to wait for data to be queued before
 *                           timing out
 * @param[in] remainingOut   the number of samples remaining in the queue
 * @param[out] status        Error status variable. 0 on success.
 */
enum HAL_DMAReadStatus HAL_ReadDMADirect(void* dmaPointer,
                                         HAL_DMASample* dmaSample,
                                         double timeoutSeconds,
                                         int32_t* remainingOut,
                                         int32_t* status);

/**
 * Reads a DMA sample from the queue.
 *
 *
 * @param[in] handle         the dma handle
 * @param[in] dmaSample      the sample object to place data into
 * @param[in] timeoutSeconds the time to wait for data to be queued before
 *                           timing out
 * @param[in] remainingOut   the number of samples remaining in the queue
 * @param[out] status        Error status variable. 0 on success.
 * @return the success result of the sample read
 */
enum HAL_DMAReadStatus HAL_ReadDMA(HAL_DMAHandle handle,
                                   HAL_DMASample* dmaSample,
                                   double timeoutSeconds, int32_t* remainingOut,
                                   int32_t* status);

// The following are helper functions for reading data from samples

/**
 * Returns the timestamp of the sample.
 * This is in the same time domain as HAL_GetFPGATime().
 *
 * @param[in] dmaSample the sample to read from
 * @param[out] status Error status variable. 0 on success.
 * @return timestamp in microseconds since FPGA Initialization
 */
uint64_t HAL_GetDMASampleTime(const HAL_DMASample* dmaSample, int32_t* status);

/**
 * Returns the raw distance data for an encoder from the sample.
 *
 * This can be scaled with DistancePerPulse and DecodingScaleFactor to match the
 * result of GetDistance()
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] encoderHandle the encoder handle
 * @param[out] status Error status variable. 0 on success.
 * @return raw encoder ticks
 */
int32_t HAL_GetDMASampleEncoderRaw(const HAL_DMASample* dmaSample,
                                   HAL_EncoderHandle encoderHandle,
                                   int32_t* status);

/**
 * Returns the distance data for an counter from the sample.
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] counterHandle the counter handle
 * @param[out] status Error status variable. 0 on success.
 * @return counter ticks
 */
int32_t HAL_GetDMASampleCounter(const HAL_DMASample* dmaSample,
                                HAL_CounterHandle counterHandle,
                                int32_t* status);

/**
 * Returns the raw period data for an encoder from the sample.
 *
 * This can be scaled with DistancePerPulse and DecodingScaleFactor to match the
 * result of GetRate()
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] encoderHandle the encoder handle
 * @param[out] status Error status variable. 0 on success.
 * @return raw encoder period
 */
int32_t HAL_GetDMASampleEncoderPeriodRaw(const HAL_DMASample* dmaSample,
                                         HAL_EncoderHandle encoderHandle,
                                         int32_t* status);

/**
 * Returns the period data for an counter from the sample.
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] counterHandle the counter handle
 * @param[out] status Error status variable. 0 on success.
 * @return counter period
 */
int32_t HAL_GetDMASampleCounterPeriod(const HAL_DMASample* dmaSample,
                                      HAL_CounterHandle counterHandle,
                                      int32_t* status);

/**
 * Returns the state of a digital source from the sample.
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] dSourceHandle the digital source handle
 * @param[out] status Error status variable. 0 on success.
 * @return digital source state
 */
HAL_Bool HAL_GetDMASampleDigitalSource(const HAL_DMASample* dmaSample,
                                       HAL_Handle dSourceHandle,
                                       int32_t* status);

/**
 * Returns the raw analog data for an analog input from the sample.
 *
 * This can be scaled with HAL_GetAnalogValueToVolts to match GetVoltage().
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] aInHandle the analog input handle
 * @param[out] status Error status variable. 0 on success.
 * @return raw analog data
 */
int32_t HAL_GetDMASampleAnalogInputRaw(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int32_t* status);

/**
 * Returns the raw averaged analog data for an analog input from the sample.
 *
 * This can be scaled with HAL_GetAnalogValueToVolts to match
 * GetAveragedVoltage().
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] aInHandle the analog input handle
 * @param[out] status Error status variable. 0 on success.
 * @return raw averaged analog data
 */
int32_t HAL_GetDMASampleAveragedAnalogInputRaw(const HAL_DMASample* dmaSample,
                                               HAL_AnalogInputHandle aInHandle,
                                               int32_t* status);

/**
 * Returns the analog accumulator data for an analog input from the sample.
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] aInHandle the analog input handle
 * @param[in] count the accumulator count
 * @param[in] value the accumulator value
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetDMASampleAnalogAccumulator(const HAL_DMASample* dmaSample,
                                       HAL_AnalogInputHandle aInHandle,
                                       int64_t* count, int64_t* value,
                                       int32_t* status);

/**
 * Returns the raw duty cycle input ratio data from the sample.
 *
 * Use HAL_GetDutyCycleOutputScaleFactor to scale this to a percentage.
 *
 * @param[in] dmaSample the sample to read from
 * @param[in] dutyCycleHandle the duty cycle handle
 * @param[out] status Error status variable. 0 on success.
 * @return raw duty cycle input data
 */
int32_t HAL_GetDMASampleDutyCycleOutputRaw(const HAL_DMASample* dmaSample,
                                           HAL_DutyCycleHandle dutyCycleHandle,
                                           int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
