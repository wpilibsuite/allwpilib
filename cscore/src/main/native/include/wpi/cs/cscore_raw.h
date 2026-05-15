// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/cs/cscore_c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cscore_raw_cfunc Raw Image Functions
 * @{
 */
uint64_t CS_GrabRawSinkFrame(CS_Sink sink, struct WPI_RawFrame* rawImage,
                             CS_Status* status);
uint64_t CS_GrabRawSinkFrameTimeout(CS_Sink sink, struct WPI_RawFrame* rawImage,
                                    double timeout, CS_Status* status);
uint64_t CS_GrabRawSinkFrameTimeoutWithFrameTime(CS_Sink sink,
                                                 struct WPI_RawFrame* rawImage,
                                                 double timeout,
                                                 uint64_t lastFrameTime,
                                                 CS_Status* status);

CS_Sink CS_CreateRawSink(const struct WPI_String* name, CS_Bool isCv,
                         CS_Status* status);

CS_Sink CS_CreateRawSinkCallback(
    const struct WPI_String* name, CS_Bool isCv, void* data,
    void (*processFrame)(void* data, uint64_t time), CS_Status* status);

void CS_PutRawSourceFrame(CS_Source source, const struct WPI_RawFrame* image,
                          CS_Status* status);

CS_Source CS_CreateRawSource(const struct WPI_String* name, CS_Bool isCv,
                             const CS_VideoMode* mode, CS_Status* status);
/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif
