/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CvSinkImpl.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <wpi/SmallString.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "Notifier.h"
#include "c_util.h"
#include "cscore_cpp.h"

using namespace cs;

CvSinkImpl::CvSinkImpl(const wpi::Twine& name, wpi::Logger& logger,
                       Notifier& notifier, Telemetry& telemetry)
    : SinkImpl{name, logger, notifier, telemetry} {
  m_active = true;
  // m_thread = std::thread(&CvSinkImpl::ThreadMain, this);
}

CvSinkImpl::CvSinkImpl(const wpi::Twine& name, wpi::Logger& logger,
                       Notifier& notifier, Telemetry& telemetry,
                       std::function<void(uint64_t time)> processFrame)
    : SinkImpl{name, logger, notifier, telemetry} {}

CvSinkImpl::~CvSinkImpl() { Stop(); }

void CvSinkImpl::Stop() {
  m_active = false;

  // wake up any waiters by forcing an empty frame to be sent
  if (auto source = GetSource()) source->Wakeup();

  // join thread
  if (m_thread.joinable()) m_thread.join();
}

uint64_t CvSinkImpl::GrabFrame(cv::Mat& image) {
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

  if (!frame.GetCv(image)) {
    // Shouldn't happen, but just in case...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;
  }

  return frame.GetTime();
}

uint64_t CvSinkImpl::GrabFrame(cv::Mat& image, double timeout) {
  SetEnabled(true);

  auto source = GetSource();
  if (!source) {
    // Source disconnected; sleep for one second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
  }

  auto frame = source->GetNextFrame(timeout);  // blocks
  if (!frame) {
    // Bad frame; sleep for 20 ms so we don't consume all processor time.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;  // signal error
  }

  if (!frame.GetCv(image)) {
    // Shouldn't happen, but just in case...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;
  }

  return frame.GetTime();
}

// Send HTTP response and a stream of JPG-frames
void CvSinkImpl::ThreadMain() {
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
    if (!m_active) break;
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

CS_Sink CreateCvSink(const wpi::Twine& name, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(
      CS_SINK_CV, std::make_shared<CvSinkImpl>(name, inst.logger, inst.notifier,
                                               inst.telemetry));
}

CS_Sink CreateCvSinkCallback(const wpi::Twine& name,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(
      CS_SINK_CV, std::make_shared<CvSinkImpl>(name, inst.logger, inst.notifier,
                                               inst.telemetry, processFrame));
}

void SetSinkDescription(CS_Sink sink, const wpi::Twine& description,
                        CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSinkImpl&>(*data->sink).SetDescription(description);
}

uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<CvSinkImpl&>(*data->sink).GrabFrame(image);
}

uint64_t GrabSinkFrameTimeout(CS_Sink sink, cv::Mat& image, double timeout,
                              CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<CvSinkImpl&>(*data->sink).GrabFrame(image, timeout);
}

std::string GetSinkError(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<CvSinkImpl&>(*data->sink).GetError();
}

wpi::StringRef GetSinkError(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                            CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return static_cast<CvSinkImpl&>(*data->sink).GetError(buf);
}

void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSinkImpl&>(*data->sink).SetEnabled(enabled);
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateCvSink(const char* name, CS_Status* status) {
  return cs::CreateCvSink(name, status);
}

CS_Sink CS_CreateCvSinkCallback(const char* name, void* data,
                                void (*processFrame)(void* data, uint64_t time),
                                CS_Status* status) {
  return cs::CreateCvSinkCallback(
      name, [=](uint64_t time) { processFrame(data, time); }, status);
}

void CS_SetSinkDescription(CS_Sink sink, const char* description,
                           CS_Status* status) {
  return cs::SetSinkDescription(sink, description, status);
}

uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image,
                          CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::GrabSinkFrame(sink, mat, status);
}

uint64_t CS_GrabSinkFrameTimeout(CS_Sink sink, struct CvMat* image,
                                 double timeout, CS_Status* status) {
  auto mat = cv::cvarrToMat(image);
  return cs::GrabSinkFrameTimeout(sink, mat, timeout, status);
}

uint64_t CS_GrabSinkFrameCpp(CS_Sink sink, cv::Mat* image, CS_Status* status) {
  return cs::GrabSinkFrame(sink, *image, status);
}

uint64_t CS_GrabSinkFrameTimeoutCpp(CS_Sink sink, cv::Mat* image,
                                    double timeout, CS_Status* status) {
  return cs::GrabSinkFrameTimeout(sink, *image, timeout, status);
}

char* CS_GetSinkError(CS_Sink sink, CS_Status* status) {
  wpi::SmallString<128> buf;
  auto str = cs::GetSinkError(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

void CS_SetSinkEnabled(CS_Sink sink, CS_Bool enabled, CS_Status* status) {
  return cs::SetSinkEnabled(sink, enabled, status);
}

}  // extern "C"
