/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CvSinkImpl.h"

#include "llvm/SmallString.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "cscore_cpp.h"
#include "c_util.h"
#include "Handle.h"
#include "Log.h"

using namespace cs;

CvSinkImpl::CvSinkImpl(llvm::StringRef name) : SinkImpl{name} {
  m_active = true;
  // m_thread = std::thread(&CvSinkImpl::ThreadMain, this);
}

CvSinkImpl::CvSinkImpl(llvm::StringRef name,
                       std::function<void(uint64_t time)> processFrame)
    : SinkImpl{name} {}

CvSinkImpl::~CvSinkImpl() { Stop(); }

void CvSinkImpl::Stop() {
  m_active = false;

  // wake up any waiters by forcing an empty frame to be sent
  if (auto source = GetSource())
    source->Wakeup();

  // join thread
  if (m_thread.joinable()) m_thread.join();
}

uint64_t CvSinkImpl::GrabFrame(cv::Mat& image) {
  SetEnabled(true);
  auto source = GetSource();
  if (!source) return 0;
  auto frame = source->GetNextFrame();  // blocks
  if (!frame) return 0;  // signal error
  switch (frame.GetPixelFormat()) {
    case VideoMode::kMJPEG:
      cv::imdecode(cv::InputArray{frame.data(), static_cast<int>(frame.size())},
                   cv::IMREAD_COLOR, &image);
      // Check to see if we successfully decoded
      if (image.cols != frame.width() || image.rows != frame.height()) return 0;
      break;
    case VideoMode::kYUYV:
      cv::cvtColor(cv::Mat{frame.height(), frame.width(), CV_8UC2,
                           frame.data()},
                   image, cv::COLOR_YUV2BGR_YUYV);
      break;
    case VideoMode::kRGB565:
      cv::cvtColor(cv::Mat{frame.height(), frame.width(), CV_8UC2,
                           frame.data()},
                   image, cv::COLOR_BGR5652RGB);
      break;
    default:
      return 0;
  }
  return frame.time();
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
    DEBUG4("Cv: waiting for frame");
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

CS_Sink CreateCvSink(llvm::StringRef name, CS_Status* status) {
  auto sink = std::make_shared<CvSinkImpl>(name);
  return Sinks::GetInstance().Allocate(CS_SINK_CV, sink);
}

CS_Sink CreateCvSinkCallback(llvm::StringRef name,
                             std::function<void(uint64_t time)> processFrame,
                             CS_Status* status) {
  auto sink = std::make_shared<CvSinkImpl>(name, processFrame);
  return Sinks::GetInstance().Allocate(CS_SINK_CV, sink);
}

void SetSinkDescription(CS_Sink sink, llvm::StringRef description,
                        CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<CvSinkImpl&>(*data->sink).SetDescription(description);
}

uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<CvSinkImpl&>(*data->sink).GrabFrame(image);
}

std::string GetSinkError(CS_Sink sink, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<CvSinkImpl&>(*data->sink).GetError();
}

llvm::StringRef GetSinkError(CS_Sink sink, llvm::SmallVectorImpl<char>& buf,
                             CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
  if (!data || data->kind != CS_SINK_CV) {
    *status = CS_INVALID_HANDLE;
    return llvm::StringRef{};
  }
  return static_cast<CvSinkImpl&>(*data->sink).GetError(buf);
}

void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status) {
  auto data = Sinks::GetInstance().Get(sink);
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

char* CS_GetSinkError(CS_Sink sink, CS_Status* status) {
  llvm::SmallString<128> buf;
  auto str = cs::GetSinkError(sink, buf, status);
  if (*status != 0) return nullptr;
  return cs::ConvertToC(str);
}

void CS_SetSinkEnabled(CS_Sink sink, CS_Bool enabled, CS_Status* status) {
  return cs::SetSinkEnabled(sink, enabled, status);
}

}  // extern "C"
