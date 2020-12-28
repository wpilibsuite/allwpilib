// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ConfigurableSourceImpl.h"

#include <wpi/timestamp.h>

#include "Handle.h"
#include "Instance.h"
#include "Log.h"
#include "Notifier.h"

using namespace cs;

ConfigurableSourceImpl::ConfigurableSourceImpl(const wpi::Twine& name,
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

void ConfigurableSourceImpl::NotifyError(const wpi::Twine& msg) {
  PutError(msg, wpi::Now());
}

int ConfigurableSourceImpl::CreateProperty(const wpi::Twine& name,
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
                                  kind, value, wpi::Twine{});
  return ndx;
}

int ConfigurableSourceImpl::CreateProperty(
    const wpi::Twine& name, CS_PropertyKind kind, int minimum, int maximum,
    int step, int defaultValue, int value,
    std::function<void(CS_Property property)> onChange) {
  // TODO
  return 0;
}

void ConfigurableSourceImpl::SetEnumPropertyChoices(
    int property, wpi::ArrayRef<std::string> choices, CS_Status* status) {
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
  prop->enumChoices = choices;
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                  prop->name, property, CS_PROP_ENUM,
                                  prop->value, wpi::Twine{});
}
