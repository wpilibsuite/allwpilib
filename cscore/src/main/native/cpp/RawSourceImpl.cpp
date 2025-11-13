// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RawSourceImpl.hpp"

#include <memory>

#include "Instance.hpp"
#include "Notifier.hpp"
#include "wpi/cs/cscore_raw.h"
#include "wpi/util/timestamp.h"

using namespace wpi::cs;

RawSourceImpl::RawSourceImpl(std::string_view name, wpi::util::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             const VideoMode& mode)
    : ConfigurableSourceImpl{name, logger, notifier, telemetry, mode} {}

RawSourceImpl::~RawSourceImpl() = default;

void RawSourceImpl::PutFrame(const WPI_RawFrame& image) {
  auto currentTime = wpi::util::Now();
  std::string_view data_view{reinterpret_cast<char*>(image.data), image.size};
  SourceImpl::PutFrame(static_cast<VideoMode::PixelFormat>(image.pixelFormat),
                       image.width, image.height, data_view, currentTime);
}

namespace wpi::cs {
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

}  // namespace wpi::cs

extern "C" {
CS_Source CS_CreateRawSource(const struct WPI_String* name, CS_Bool isCv,
                             const CS_VideoMode* mode, CS_Status* status) {
  return wpi::cs::CreateRawSource(wpi::util::to_string_view(name), isCv,
                                  static_cast<const wpi::cs::VideoMode&>(*mode),
                                  status);
}

void CS_PutRawSourceFrame(CS_Source source, const struct WPI_RawFrame* image,
                          CS_Status* status) {
  return wpi::cs::PutSourceFrame(source, *image, status);
}

}  // extern "C"
