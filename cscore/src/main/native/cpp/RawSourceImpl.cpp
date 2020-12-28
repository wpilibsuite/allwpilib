// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RawSourceImpl.h"

#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "Notifier.h"
#include "cscore_raw.h"

using namespace cs;

RawSourceImpl::RawSourceImpl(const wpi::Twine& name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             const VideoMode& mode)
    : ConfigurableSourceImpl{name, logger, notifier, telemetry, mode} {}

RawSourceImpl::~RawSourceImpl() = default;

void RawSourceImpl::PutFrame(const CS_RawFrame& image) {
  int type;
  switch (image.pixelFormat) {
    case VideoMode::kYUYV:
    case VideoMode::kRGB565:
      type = CV_8UC2;
      break;
    case VideoMode::kBGR:
      type = CV_8UC3;
      break;
    case VideoMode::kGray:
    case VideoMode::kMJPEG:
    default:
      type = CV_8UC1;
      break;
  }
  cv::Mat finalImage{image.height, image.width, type, image.data};
  std::unique_ptr<Image> dest =
      AllocImage(static_cast<VideoMode::PixelFormat>(image.pixelFormat),
                 image.width, image.height, image.totalData);
  finalImage.copyTo(dest->AsMat());

  SourceImpl::PutFrame(std::move(dest), wpi::Now());
}

namespace cs {
CS_Source CreateRawSource(const wpi::Twine& name, const VideoMode& mode,
                          CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_RAW, std::make_shared<RawSourceImpl>(
                                              name, inst.logger, inst.notifier,
                                              inst.telemetry, mode));
}

void PutSourceFrame(CS_Source source, const CS_RawFrame& image,
                    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_RAW) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<RawSourceImpl&>(*data->source).PutFrame(image);
}
}  // namespace cs

extern "C" {
CS_Source CS_CreateRawSource(const char* name, const CS_VideoMode* mode,
                             CS_Status* status) {
  return cs::CreateRawSource(name, static_cast<const cs::VideoMode&>(*mode),
                             status);
}

void CS_PutRawSourceFrame(CS_Source source, const struct CS_RawFrame* image,
                          CS_Status* status) {
  return cs::PutSourceFrame(source, *image, status);
}
}  // extern "C"
