// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RawSinkImpl.h"

#include <algorithm>
#include <memory>

#include "Instance.h"
#include "cscore_raw.h"

using namespace cs;

RawSinkImpl::RawSinkImpl(std::string_view name, wpi::Logger& logger,
                         Notifier& notifier, Telemetry& telemetry)
    : SinkImpl{name, logger, notifier, telemetry} {
  m_active = true;
  // m_thread = std::thread(&RawSinkImpl::ThreadMain, this);
}

RawSinkImpl::RawSinkImpl(std::string_view name, wpi::Logger& logger,
                         Notifier& notifier, Telemetry& telemetry,
                         std::function<void(uint64_t time)> processFrame)
    : SinkImpl{name, logger, notifier, telemetry} {}

RawSinkImpl::~RawSinkImpl() {
  Stop();
}

void RawSinkImpl::Stop() {
  m_active = false;

  // wake up any waiters by forcing an empty frame to be sent
  if (auto source = GetSource()) {
    source->Wakeup();
  }

  // join thread
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

uint64_t RawSinkImpl::GrabFrame(WPI_RawFrame& image) {
  SetEnabled(true);

  auto source = GetSource();
  if (!source) {
    // Source disconnected; sleep for one second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
  }

  auto frame = source->GetNextFrame();  // blocks
  if (!frame) {
    // Bad frame; sleep for 20 ms so we don't consume all processor time.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;  // signal error
  }

  return GrabFrameImpl(image, frame);
}

uint64_t RawSinkImpl::GrabFrame(WPI_RawFrame& image, double timeout) {
  return GrabFrame(image, timeout, 0);
}

uint64_t RawSinkImpl::GrabFrame(WPI_RawFrame& image, double timeout,
                                uint64_t lastFrameTime) {
  SetEnabled(true);

  auto source = GetSource();
  if (!source) {
    // Source disconnected; sleep for one second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
  }

  auto frame = source->GetNextFrame(timeout, lastFrameTime);  // blocks
  if (!frame) {
    // Bad frame; sleep for 20 ms so we don't consume all processor time.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;  // signal error
  }

  return GrabFrameImpl(image, frame);
}

uint64_t RawSinkImpl::GrabFrameImpl(WPI_RawFrame& rawFrame,
                                    Frame& incomingFrame) {
  Image* newImage = nullptr;

  if (rawFrame.pixelFormat == WPI_PixelFormat::WPI_PIXFMT_UNKNOWN) {
    // Always get incoming image directly on unknown
    newImage = incomingFrame.GetExistingImage(0);
  } else {
    // Format is known, ask for it
    auto width = rawFrame.width;
    auto height = rawFrame.height;
    auto pixelFormat =
        static_cast<VideoMode::PixelFormat>(rawFrame.pixelFormat);
    if (width <= 0 || height <= 0) {
      width = incomingFrame.GetOriginalWidth();
      height = incomingFrame.GetOriginalHeight();
    }
    newImage = incomingFrame.GetImage(width, height, pixelFormat);
  }

  if (!newImage) {
    // Shouldn't happen, but just in case...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;
  }

  WPI_AllocateRawFrameData(&rawFrame, newImage->size());
  rawFrame.height = newImage->height;
  rawFrame.width = newImage->width;
  rawFrame.stride = newImage->GetStride();
  rawFrame.pixelFormat = newImage->pixelFormat;
  rawFrame.size = newImage->size();
  std::copy(newImage->data(), newImage->data() + rawFrame.size, rawFrame.data);
  rawFrame.timestamp = incomingFrame.GetTime();
  rawFrame.timestampSrc = incomingFrame.GetTimeSource();

  return incomingFrame.GetTime();
}

// Send HTTP response and a stream of JPG-frames
void RawSinkImpl::ThreadMain() {
  Enable();
  while (m_active) {
    auto source = GetSource();
    if (!source) {
      // Source disconnected; sleep for one second
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    SDEBUG4("waiting for frame");
    Frame frame = source->GetNextFrame();  // blocks
    if (!m_active) {
      break;
    }
    if (!frame) {
      // Bad frame; sleep for 10 ms so we don't consume all processor time.
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }
    // TODO m_processFrame();
  }
  Disable();
}

namespace cs {
static constexpr unsigned SinkMask = CS_SINK_CV | CS_SINK_RAW;

CS_Sink CreateRawSink(std::string_view name, bool isCv, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(isCv ? CS_SINK_CV : CS_SINK_RAW,
                         std::make_shared<RawSinkImpl>(
                             name, inst.logger, inst.notifier, inst.telemetry));
}

CS_Sink CreateRawSinkCallback(std::string_view name, bool isCv,
                              std::function<void(uint64_t time)> processFrame,
                              CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(
      isCv ? CS_SINK_CV : CS_SINK_RAW,
      std::make_shared<RawSinkImpl>(name, inst.logger, inst.notifier,
                                    inst.telemetry, processFrame));
}

uint64_t GrabSinkFrame(CS_Sink sink, WPI_RawFrame& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & SinkMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<RawSinkImpl&>(*data->sink).GrabFrame(image);
}

uint64_t GrabSinkFrameTimeout(CS_Sink sink, WPI_RawFrame& image, double timeout,
                              CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & SinkMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<RawSinkImpl&>(*data->sink).GrabFrame(image, timeout);
}

uint64_t GrabSinkFrameTimeoutLastTime(CS_Sink sink, WPI_RawFrame& image,
                                      double timeout, uint64_t lastFrameTime,
                                      CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & SinkMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<RawSinkImpl&>(*data->sink)
      .GrabFrame(image, timeout, lastFrameTime);
}

}  // namespace cs

extern "C" {
CS_Sink CS_CreateRawSink(const struct WPI_String* name, CS_Bool isCv,
                         CS_Status* status) {
  return cs::CreateRawSink(wpi::to_string_view(name), isCv, status);
}

CS_Sink CS_CreateRawSinkCallback(
    const struct WPI_String* name, CS_Bool isCv, void* data,
    void (*processFrame)(void* data, uint64_t time), CS_Status* status) {
  return cs::CreateRawSinkCallback(
      wpi::to_string_view(name), isCv,
      [=](uint64_t time) { processFrame(data, time); }, status);
}

uint64_t CS_GrabRawSinkFrame(CS_Sink sink, struct WPI_RawFrame* image,
                             CS_Status* status) {
  return cs::GrabSinkFrame(sink, *image, status);
}

uint64_t CS_GrabRawSinkFrameTimeout(CS_Sink sink, struct WPI_RawFrame* image,
                                    double timeout, CS_Status* status) {
  return cs::GrabSinkFrameTimeout(sink, *image, timeout, status);
}

uint64_t CS_GrabRawSinkFrameTimeoutWithFrameTime(CS_Sink sink,
                                                 struct WPI_RawFrame* image,
                                                 double timeout,
                                                 uint64_t lastFrameTime,
                                                 CS_Status* status) {
  return cs::GrabSinkFrameTimeoutLastTime(sink, *image, timeout, lastFrameTime,
                                          status);
}

}  // extern "C"
