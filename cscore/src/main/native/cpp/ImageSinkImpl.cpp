/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ImageSinkImpl.h"

#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <wpi/SmallString.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "c_util.h"
#include "cscore_cpp.h"
#include "cscore_raw.h"

using namespace cs;

ImageSinkImpl::ImageSinkImpl(const wpi::Twine& name, wpi::Logger& logger)
    : SinkImpl{name, logger} {}

ImageSinkImpl::~ImageSinkImpl() { Stop(); }

void ImageSinkImpl::Stop() {
  m_active = false;
  // wake up any waiters
  m_frameCv.notify_all();
}

uint64_t ImageSinkImpl::GrabFrame(cv::Mat& image) {
  return GrabFrameImpl(image, GetNextFrame(false, 0));
}

uint64_t ImageSinkImpl::GrabFrame(cv::Mat& image, double timeout) {
  return GrabFrameImpl(image, GetNextFrame(true, timeout));
}

uint64_t ImageSinkImpl::GrabFrame(CS_RawFrame& image) {
  return GrabFrameImpl(image, GetNextFrame(false, 0));
}

uint64_t ImageSinkImpl::GrabFrame(CS_RawFrame& image, double timeout) {
  return GrabFrameImpl(image, GetNextFrame(true, timeout));
}

Frame ImageSinkImpl::GetNextFrame(bool hasTimeout, double timeout) {
  SetEnabled(true);

  if (!HasSource()) {
    // Source disconnected; sleep for one second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (!HasSource()) return {};
  }

  Frame frame;
  if (hasTimeout) {
    std::unique_lock lock{m_mutex};
    // block until frame available or timeout
    if (m_frameCv.wait_for(
            lock, std::chrono::milliseconds(static_cast<int>(timeout * 1000)),
            [this] {
              return !m_active || m_frame.GetTime() != m_lastFrameTime;
            })) {
      if (m_active) {
        frame = m_frame;
        m_lastFrameTime = frame.GetTime();
      }
    }
  } else {
    std::unique_lock lock{m_mutex};
    // block until frame available
    m_frameCv.wait(lock, [this] {
      return !m_active || m_frame.GetTime() != m_lastFrameTime;
    });
    if (m_active) {
      frame = m_frame;
      m_lastFrameTime = frame.GetTime();
    }
  }

  if (m_active && !frame) {
    // Bad frame; sleep for 20 ms so we don't consume all processor time.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  return frame;
}

uint64_t ImageSinkImpl::GrabFrameImpl(cv::Mat& image, Frame&& frame) {
  if (!frame) return 0;

  if (!frame.GetCv(image)) {
    // Shouldn't happen, but just in case...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;
  }

  return frame.GetTime();
}

uint64_t ImageSinkImpl::GrabFrameImpl(CS_RawFrame& rawFrame, Frame&& frame) {
  if (!frame) return 0;

  Image* newImage = nullptr;

  if (rawFrame.pixelFormat == CS_PixelFormat::CS_PIXFMT_UNKNOWN) {
    // Always get incoming image directly on unknown
    newImage = frame.GetExistingImage(0);
  } else {
    // Format is known, ask for it
    auto width = rawFrame.width;
    auto height = rawFrame.height;
    auto pixelFormat =
        static_cast<VideoMode::PixelFormat>(rawFrame.pixelFormat);
    if (width <= 0 || height <= 0) {
      width = frame.GetOriginalWidth();
      height = frame.GetOriginalHeight();
    }
    newImage = frame.GetImage(width, height, pixelFormat);
  }

  if (!newImage) {
    // Shouldn't happen, but just in case...
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 0;
  }

  CS_AllocateRawFrameData(&rawFrame, newImage->size());
  rawFrame.height = newImage->height;
  rawFrame.width = newImage->width;
  rawFrame.pixelFormat = newImage->pixelFormat;
  rawFrame.totalData = newImage->size();
  std::copy(newImage->data(), newImage->data() + rawFrame.totalData,
            rawFrame.data);

  return frame.GetTime();
}

void ImageSinkImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {
  if (source) {
    m_onNewFrame = source->newFrame.connect_connection([this](Frame frame) {
      {
        std::lock_guard lock(m_mutex);
        m_frame = std::move(frame);
      }
      m_frameCv.notify_all();
    });
  } else {
    m_onNewFrame.disconnect();
  }
}

namespace cs {

CS_Sink CreateImageSink(const wpi::Twine& name, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSink(
      CS_SINK_IMAGE, std::make_shared<ImageSinkImpl>(name, inst.GetLogger()));
}

CS_Sink CreateCvSink(const wpi::Twine& name, CS_Status* status) {
  return CreateImageSink(name, status);
}

CS_Sink CreateRawSink(const wpi::Twine& name, CS_Status* status) {
  return CreateImageSink(name, status);
}

void SetSinkDescription(CS_Sink sink, const wpi::Twine& description,
                        CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & CS_SINK_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSinkImpl&>(*data->sink).SetDescription(description);
}

uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_IMAGE) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GrabFrame(image);
}

uint64_t GrabSinkFrameTimeout(CS_Sink sink, cv::Mat& image, double timeout,
                              CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_IMAGE) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GrabFrame(image, timeout);
}

uint64_t GrabSinkFrame(CS_Sink sink, CS_RawFrame& image, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_RAW) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GrabFrame(image);
}

uint64_t GrabSinkFrameTimeout(CS_Sink sink, CS_RawFrame& image, double timeout,
                              CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || data->kind != CS_SINK_RAW) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GrabFrame(image, timeout);
}

std::string GetSinkError(CS_Sink sink, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & CS_SINK_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GetError();
}

wpi::StringRef GetSinkError(CS_Sink sink, wpi::SmallVectorImpl<char>& buf,
                            CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & CS_SINK_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return wpi::StringRef{};
  }
  return static_cast<ImageSinkImpl&>(*data->sink).GetError(buf);
}

void SetSinkEnabled(CS_Sink sink, bool enabled, CS_Status* status) {
  auto data = Instance::GetInstance().GetSink(sink);
  if (!data || (data->kind & CS_SINK_IMAGE) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ImageSinkImpl&>(*data->sink).SetEnabled(enabled);
}

}  // namespace cs

extern "C" {

CS_Sink CS_CreateImageSink(const char* name, CS_Status* status) {
  return cs::CreateImageSink(name, status);
}

CS_Sink CS_CreateCvSink(const char* name, CS_Status* status) {
  return cs::CreateImageSink(name, status);
}

CS_Sink CS_CreateRawSink(const char* name, CS_Status* status) {
  return cs::CreateImageSink(name, status);
}

void CS_SetSinkDescription(CS_Sink sink, const char* description,
                           CS_Status* status) {
  return cs::SetSinkDescription(sink, description, status);
}

#if CV_VERSION_MAJOR < 4
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
#endif  // CV_VERSION_MAJOR < 4

uint64_t CS_GrabSinkFrameCpp(CS_Sink sink, cv::Mat* image, CS_Status* status) {
  return cs::GrabSinkFrame(sink, *image, status);
}

uint64_t CS_GrabSinkFrameTimeoutCpp(CS_Sink sink, cv::Mat* image,
                                    double timeout, CS_Status* status) {
  return cs::GrabSinkFrameTimeout(sink, *image, timeout, status);
}

uint64_t CS_GrabRawSinkFrame(CS_Sink sink, struct CS_RawFrame* image,
                             CS_Status* status) {
  return cs::GrabSinkFrame(sink, *image, status);
}

uint64_t CS_GrabRawSinkFrameTimeout(CS_Sink sink, struct CS_RawFrame* image,
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
