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

RawSourceImpl::RawSourceImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             const VideoMode& mode)
    : ConfigurableSourceImpl{name, logger, notifier, telemetry, mode} {}

RawSourceImpl::~RawSourceImpl() = default;

void RawSourceImpl::PutFrame(const WPI_RawFrame& image) {
  int type;
  switch (image.pixelFormat) {
    case VideoMode::kYUYV:
    case VideoMode::kRGB565:
    case VideoMode::kY16:
    case VideoMode::kUYVY:
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
  cv::Mat finalImage{image.height, image.width, type, image.data,
                     static_cast<size_t>(image.stride)};
  std::unique_ptr<Image> dest =
      AllocImage(static_cast<VideoMode::PixelFormat>(image.pixelFormat),
                 image.width, image.height, image.size);
  finalImage.copyTo(dest->AsMat());

  SourceImpl::PutFrame(std::move(dest), wpi::Now());
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

void NotifySourceError(CS_Source source, std::string_view msg,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<RawSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<RawSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, std::string_view description,
                          CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<RawSourceImpl&>(*data->source).SetDescription(description);
}

CS_Property CreateSourceProperty(CS_Source source, std::string_view name,
                                 CS_PropertyKind kind, int minimum, int maximum,
                                 int step, int defaultValue, int value,
                                 CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<RawSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value);
  return Handle{source, property, Handle::kProperty};
}

CS_Property CreateSourcePropertyCallback(
    CS_Source source, std::string_view name, CS_PropertyKind kind, int minimum,
    int maximum, int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return -1;
  }
  int property = static_cast<RawSourceImpl&>(*data->source)
                     .CreateProperty(name, kind, minimum, maximum, step,
                                     defaultValue, value, onChange);
  return Handle{source, property, Handle::kProperty};
}

void SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                  std::span<const std::string> choices,
                                  CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }

  // Get property index; also validate the source owns this property
  Handle handle{property};
  int i = handle.GetParentIndex();
  if (i < 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  auto data2 = Instance::GetInstance().GetSource(Handle{i, Handle::kSource});
  if (!data2 || data->source.get() != data2->source.get()) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  int propertyIndex = handle.GetIndex();
  static_cast<RawSourceImpl&>(*data->source)
      .SetEnumPropertyChoices(propertyIndex, choices, status);
}

}  // namespace cs

extern "C" {
CS_Source CS_CreateRawSource(const char* name, CS_Bool isCv,
                             const CS_VideoMode* mode, CS_Status* status) {
  return cs::CreateRawSource(name, isCv,
                             static_cast<const cs::VideoMode&>(*mode), status);
}

void CS_PutRawSourceFrame(CS_Source source, const struct WPI_RawFrame* image,
                          CS_Status* status) {
  return cs::PutSourceFrame(source, *image, status);
}

void CS_NotifySourceError(CS_Source source, const char* msg,
                          CS_Status* status) {
  return cs::NotifySourceError(source, msg, status);
}

void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status) {
  return cs::SetSourceConnected(source, connected, status);
}

void CS_SetSourceDescription(CS_Source source, const char* description,
                             CS_Status* status) {
  return cs::SetSourceDescription(source, description, status);
}

CS_Property CS_CreateSourceProperty(CS_Source source, const char* name,
                                    enum CS_PropertyKind kind, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value, CS_Status* status) {
  return cs::CreateSourceProperty(source, name, kind, minimum, maximum, step,
                                  defaultValue, value, status);
}

CS_Property CS_CreateSourcePropertyCallback(
    CS_Source source, const char* name, enum CS_PropertyKind kind, int minimum,
    int maximum, int step, int defaultValue, int value, void* data,
    void (*onChange)(void* data, CS_Property property), CS_Status* status) {
  return cs::CreateSourcePropertyCallback(
      source, name, kind, minimum, maximum, step, defaultValue, value,
      [=](CS_Property property) { onChange(data, property); }, status);
}

void CS_SetSourceEnumPropertyChoices(CS_Source source, CS_Property property,
                                     const char** choices, int count,
                                     CS_Status* status) {
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec.push_back(choices[i]);
  }
  return cs::SetSourceEnumPropertyChoices(source, property, vec, status);
}

}  // extern "C"
