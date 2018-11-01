/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SinkImpl.h"

#include "Instance.h"
#include "Notifier.h"
#include "SourceImpl.h"

using namespace cs;

SinkImpl::SinkImpl(const wpi::Twine& name, wpi::Logger& logger,
                   Notifier& notifier, Telemetry& telemetry)
    : m_logger(logger),
      m_notifier(notifier),
      m_telemetry(telemetry),
      m_name{name.str()} {}

SinkImpl::~SinkImpl() {
  if (m_source) {
    if (m_enabledCount > 0) m_source->DisableSink();
    m_source->RemoveSink();
  }
}

void SinkImpl::SetDescription(const wpi::Twine& description) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_description = description.str();
}

wpi::StringRef SinkImpl::GetDescription(wpi::SmallVectorImpl<char>& buf) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return wpi::StringRef{buf.data(), buf.size()};
}

void SinkImpl::Enable() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  ++m_enabledCount;
  if (m_enabledCount == 1) {
    if (m_source) m_source->EnableSink();
    m_notifier.NotifySink(*this, CS_SINK_ENABLED);
  }
}

void SinkImpl::Disable() {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  --m_enabledCount;
  if (m_enabledCount == 0) {
    if (m_source) m_source->DisableSink();
    m_notifier.NotifySink(*this, CS_SINK_DISABLED);
  }
}

void SinkImpl::SetEnabled(bool enabled) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (enabled && m_enabledCount == 0) {
    if (m_source) m_source->EnableSink();
    m_enabledCount = 1;
    m_notifier.NotifySink(*this, CS_SINK_ENABLED);
  } else if (!enabled && m_enabledCount > 0) {
    if (m_source) m_source->DisableSink();
    m_enabledCount = 0;
    m_notifier.NotifySink(*this, CS_SINK_DISABLED);
  }
}

void SinkImpl::SetSource(std::shared_ptr<SourceImpl> source) {
  {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    if (m_source == source) return;
    if (m_source) {
      if (m_enabledCount > 0) m_source->DisableSink();
      m_source->RemoveSink();
    }
    m_source = source;
    if (m_source) {
      m_source->AddSink();
      if (m_enabledCount > 0) m_source->EnableSink();
    }
  }
  SetSourceImpl(source);
}

std::string SinkImpl::GetError() const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  return m_source->GetCurFrame().GetError();
}

wpi::StringRef SinkImpl::GetError(wpi::SmallVectorImpl<char>& buf) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  // Make a copy as it's shared data
  wpi::StringRef error = m_source->GetCurFrame().GetError();
  buf.clear();
  buf.append(error.data(), error.data() + error.size());
  return wpi::StringRef{buf.data(), buf.size()};
}

void SinkImpl::NotifyPropertyCreated(int propIndex, PropertyImpl& prop) {
  m_notifier.NotifySinkProperty(*this, CS_SINK_PROPERTY_CREATED, prop.name,
                                propIndex, prop.propKind, prop.value,
                                prop.valueStr);
  // also notify choices updated event for enum types
  if (prop.propKind == CS_PROP_ENUM)
    m_notifier.NotifySinkProperty(*this, CS_SINK_PROPERTY_CHOICES_UPDATED,
                                  prop.name, propIndex, prop.propKind,
                                  prop.value, wpi::Twine{});
}

void SinkImpl::UpdatePropertyValue(int property, bool setString, int value,
                                   const wpi::Twine& valueStr) {
  auto prop = GetProperty(property);
  if (!prop) return;

  if (setString)
    prop->SetValue(valueStr);
  else
    prop->SetValue(value);

  // Only notify updates after we've notified created
  if (m_properties_cached) {
    m_notifier.NotifySinkProperty(*this, CS_SINK_PROPERTY_VALUE_UPDATED,
                                  prop->name, property, prop->propKind,
                                  prop->value, prop->valueStr);
  }
}

void SinkImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {}
