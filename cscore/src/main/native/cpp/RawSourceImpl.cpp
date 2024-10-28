// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RawSourceImpl.h"

#include <memory>

#include <wpi/timestamp.h>

#include "Instance.h"
#include "Notifier.h"
#include "cscore_raw.h"

using namespace cs;

RawSourceImpl::RawSourceImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             const VideoMode& mode)
    : ConfigurableSourceImpl{name, logger, notifier, telemetry, mode} {}

RawSourceImpl::~RawSourceImpl() = default;

void RawSourceImpl::PutFrame(const WPI_RawFrame& image) {
  auto currentTime = wpi::Now();
  std::string_view data_view{reinterpret_cast<char*>(image.data), image.size};
  SourceImpl::PutFrame(static_cast<VideoMode::PixelFormat>(image.pixelFormat),
                       image.width, image.height, data_view, currentTime);
}

namespace cs {
static constexpr unsigned SourceMask = CS_SOURCE_CV | CS_SOURCE_RAW;

CS_Source CreateRawSource(std::string_view name, bool isCv,
                          const VideoMode& mode, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(
      isCv ? CS_SOURCE_CV : CS_SOURCE_RAW,
      std::make_shared<RawSourceImpl>(name, inst.logger, inst.notifier,
                                      inst.telemetry, mode));
}

void PutSourceFrame(CS_Source source, const WPI_RawFrame& image,
                    CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<RawSourceImpl&>(*data->source).PutFrame(image);
}

}  // namespace cs

extern "C" {
CS_Source CS_CreateRawSource(const struct WPI_String* name, CS_Bool isCv,
                             const CS_VideoMode* mode, CS_Status* status) {
  return cs::CreateRawSource(wpi::to_string_view(name), isCv,
                             static_cast<const cs::VideoMode&>(*mode), status);
}

void CS_PutRawSourceFrame(CS_Source source, const struct WPI_RawFrame* image,
                          CS_Status* status) {
  return cs::PutSourceFrame(source, *image, status);
}

}  // extern "C"
