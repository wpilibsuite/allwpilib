/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CSCORE_RAW_H_
#define CSCORE_CSCORE_RAW_H_

#include "cscore_c.h"

#ifdef __cplusplus
#include "cscore_cpp.h"
#endif

/**
 * Raw Frame
 */
typedef struct CS_RawFrame {
  char* data;
  int dataLength;
  int pixelFormat;
  int width;
  int height;
  int totalData;
} CS_RawFrame;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cscore_raw_cfunc Raw Image Functions
 * @{
 */
void CS_AllocateRawFrameData(CS_RawFrame* frame, int requestedSize);
void CS_FreeRawFrameData(CS_RawFrame* frame);

uint64_t CS_GrabRawSinkFrame(CS_Sink sink, struct CS_RawFrame* rawImage,
                             CS_Status* status);
uint64_t CS_GrabRawSinkFrameTimeout(CS_Sink sink, struct CS_RawFrame* rawImage,
                                    double timeout, CS_Status* status);

CS_Sink CS_CreateRawSink(const char* name, CS_Status* status);

CS_Sink CS_CreateRawSinkCallback(const char* name, void* data,
                                 void (*processFrame)(void* data,
                                                      uint64_t time),
                                 CS_Status* status);

void CS_PutRawSourceFrame(CS_Source source, const struct CS_RawFrame* image,
                          CS_Status* status);

CS_Source CS_CreateRawSource(const char* name, const CS_VideoMode* mode,
                             CS_Status* status);
/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
namespace cs {

struct RawFrame : public CS_RawFrame {
  RawFrame() {
    data = nullptr;
    dataLength = 0;
    pixelFormat = CS_PIXFMT_UNKNOWN;
    width = 0;
    height = 0;
    totalData = 0;
  }

  ~RawFrame() { CS_FreeRawFrameData(this); }
};

/**
 * @defgroup cscore_raw_func Raw Image Functions
 * @{
 */

CS_Source CreateRawSource(const wpi::Twine& name, const VideoMode& mode,
                          CS_Status* status);

CS_Sink CreateRawSink(const wpi::Twine& name, CS_Status* status);
CS_Sink CreateRawSinkCallback(const wpi::Twine& name,
                              std::function<void(uint64_t time)> processFrame,
                              CS_Status* status);

void PutSourceFrame(CS_Source source, const CS_RawFrame& image,
                    CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, CS_RawFrame& image, CS_Status* status);
uint64_t GrabSinkFrameTimeout(CS_Sink sink, CS_RawFrame& image, double timeout,
                              CS_Status* status);

}  // namespace cs

/** @} */

#endif

#endif  // CSCORE_CSCORE_RAW_H_
