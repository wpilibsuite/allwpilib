// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ConfigurableSourceImpl.h"

#include <memory>
#include <string>

#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Notifier.h"

using namespace cs;

ConfigurableSourceImpl::ConfigurableSourceImpl(std::string_view name,
                                               wpi::Logger& logger,
                                               Notifier& notifier,
                                               Telemetry& telemetry,
                                               const VideoMode& mode)
    : SourceImpl{name, logger, notifier, telemetry} {
  m_mode = mode;
  m_videoModes.push_back(m_mode);
}

ConfigurableSourceImpl::~ConfigurableSourceImpl() = default;

void ConfigurableSourceImpl::Start() {
  m_notifier.NotifySource(*this, CS_SOURCE_CONNECTED);
  m_notifier.NotifySource(*this, CS_SOURCE_VIDEOMODES_UPDATED);
  m_notifier.NotifySourceVideoMode(*this, m_mode);
}

bool ConfigurableSourceImpl::SetVideoMode(const VideoMode& mode,
                                          CS_Status* status) {
  {
    std::scoped_lock lock(m_mutex);
    m_mode = mode;
    m_videoModes[0] = mode;
  }
  m_notifier.NotifySourceVideoMode(*this, mode);
  return true;
}

void ConfigurableSourceImpl::NumSinksChanged() {
  // ignore
}

void ConfigurableSourceImpl::NumSinksEnabledChanged() {
  // ignore
}

void ConfigurableSourceImpl::NotifyError(std::string_view msg) {
  PutError(msg, wpi::Now());
}

int ConfigurableSourceImpl::CreateProperty(std::string_view name,
                                           CS_PropertyKind kind, int minimum,
                                           int maximum, int step,
                                           int defaultValue, int value) {
  std::scoped_lock lock(m_mutex);
  int ndx = CreateOrUpdateProperty(
      name,
      [=] {
        return std::make_unique<PropertyImpl>(name, kind, minimum, maximum,
                                              step, defaultValue, value);
      },
      [&](PropertyImpl& prop) {
        // update all but value
        prop.propKind = kind;
        prop.minimum = minimum;
        prop.maximum = maximum;
        prop.step = step;
        prop.defaultValue = defaultValue;
        value = prop.value;
      });
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, name, ndx,
                                  kind, value, {});
  return ndx;
}

int ConfigurableSourceImpl::CreateProperty(
    std::string_view name, CS_PropertyKind kind, int minimum, int maximum,
    int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange) {
  // TODO
  return 0;
}

void ConfigurableSourceImpl::SetEnumPropertyChoices(
    int property, std::span<const std::string> choices, CS_Status* status) {
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }
  if (prop->propKind != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }
  prop->enumChoices.assign(choices.begin(), choices.end());
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                  prop->name, property, CS_PROP_ENUM,
                                  prop->value, {});
}

namespace cs {
static constexpr unsigned SourceMask = CS_SOURCE_CV | CS_SOURCE_RAW;

void NotifySourceError(CS_Source source, std::string_view msg,
                       CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ConfigurableSourceImpl&>(*data->source).NotifyError(msg);
}

void SetSourceConnected(CS_Source source, bool connected, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ConfigurableSourceImpl&>(*data->source).SetConnected(connected);
}

void SetSourceDescription(CS_Source source, std::string_view description,
                          CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || (data->kind & SourceMask) == 0) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<ConfigurableSourceImpl&>(*data->source)
      .SetDescription(description);
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
  int property = static_cast<ConfigurableSourceImpl&>(*data->source)
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
  int property = static_cast<ConfigurableSourceImpl&>(*data->source)
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
  static_cast<ConfigurableSourceImpl&>(*data->source)
      .SetEnumPropertyChoices(propertyIndex, choices, status);
}

}  // namespace cs

extern "C" {
void CS_NotifySourceError(CS_Source source, const struct WPI_String* msg,
                          CS_Status* status) {
  return cs::NotifySourceError(source, wpi::to_string_view(msg), status);
}

void CS_SetSourceConnected(CS_Source source, CS_Bool connected,
                           CS_Status* status) {
  return cs::SetSourceConnected(source, connected, status);
}

void CS_SetSourceDescription(CS_Source source,
                             const struct WPI_String* description,
                             CS_Status* status) {
  return cs::SetSourceDescription(source, wpi::to_string_view(description),
                                  status);
}

CS_Property CS_CreateSourceProperty(CS_Source source,
                                    const struct WPI_String* name,
                                    enum CS_PropertyKind kind, int minimum,
                                    int maximum, int step, int defaultValue,
                                    int value, CS_Status* status) {
  return cs::CreateSourceProperty(source, wpi::to_string_view(name), kind,
                                  minimum, maximum, step, defaultValue, value,
                                  status);
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
                                     const struct WPI_String* choices,
                                     int count, CS_Status* status) {
  wpi::SmallVector<std::string, 8> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec.emplace_back(wpi::to_string_view(&choices[i]));
  }
  return cs::SetSourceEnumPropertyChoices(source, property, vec, status);
}

}  // extern "C"
