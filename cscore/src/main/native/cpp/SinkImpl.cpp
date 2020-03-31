/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SinkImpl.h"

#include <wpi/json.h>

#include "Log.h"
#include "SourceImpl.h"

using namespace cs;

SinkImpl::SinkImpl(const wpi::Twine& name, wpi::Logger& logger)
    : m_logger(logger), m_name{name.str()} {}

SinkImpl::~SinkImpl() {
  if (m_source) {
    if (m_enabledCount > 0) m_source->DisableSink();
    m_source->RemoveSink();
  }
}

void SinkImpl::SetDescription(const wpi::Twine& description) {
  std::scoped_lock lock(m_mutex);
  m_description = description.str();
}

wpi::StringRef SinkImpl::GetDescription(wpi::SmallVectorImpl<char>& buf) const {
  std::scoped_lock lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return wpi::StringRef{buf.data(), buf.size()};
}

void SinkImpl::Enable() {
  std::scoped_lock lock(m_mutex);
  ++m_enabledCount;
  if (m_enabledCount == 1) {
    if (m_source) m_source->EnableSink();
    enabled();
  }
}

void SinkImpl::Disable() {
  std::scoped_lock lock(m_mutex);
  --m_enabledCount;
  if (m_enabledCount == 0) {
    if (m_source) m_source->DisableSink();
    disabled();
  }
}

void SinkImpl::SetEnabled(bool enable) {
  std::scoped_lock lock(m_mutex);
  if (enable && m_enabledCount == 0) {
    if (m_source) m_source->EnableSink();
    m_enabledCount = 1;
    enabled();
  } else if (!enable && m_enabledCount > 0) {
    if (m_source) m_source->DisableSink();
    m_enabledCount = 0;
    disabled();
  }
}

void SinkImpl::SetSource(std::shared_ptr<SourceImpl> source) {
  {
    std::scoped_lock lock(m_mutex);
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
  sourceChanged(source.get());
}

std::string SinkImpl::GetError() const {
  std::scoped_lock lock(m_mutex);
  if (!m_source) return "no source connected";
  return m_source->GetCurFrame().GetError();
}

wpi::StringRef SinkImpl::GetError(wpi::SmallVectorImpl<char>& buf) const {
  std::scoped_lock lock(m_mutex);
  if (!m_source) return "no source connected";
  // Make a copy as it's shared data
  wpi::StringRef error = m_source->GetCurFrame().GetError();
  buf.clear();
  buf.append(error.data(), error.data() + error.size());
  return wpi::StringRef{buf.data(), buf.size()};
}

bool SinkImpl::SetConfigJson(wpi::StringRef config, CS_Status* status) {
  wpi::json j;
  try {
    j = wpi::json::parse(config);
  } catch (const wpi::json::parse_error& e) {
    SWARNING("SetConfigJson: parse error at byte " << e.byte << ": "
                                                   << e.what());
    *status = CS_PROPERTY_WRITE_FAILED;
    return false;
  }
  return SetConfigJson(j, status);
}

bool SinkImpl::SetConfigJson(const wpi::json& config, CS_Status* status) {
  if (config.count("properties") != 0)
    SetPropertiesJson(config.at("properties"), m_logger, GetName(), status);

  return true;
}

std::string SinkImpl::GetConfigJson(CS_Status* status) {
  std::string rv;
  wpi::raw_string_ostream os(rv);
  GetConfigJsonObject(status).dump(os, 4);
  os.flush();
  return rv;
}

wpi::json SinkImpl::GetConfigJsonObject(CS_Status* status) {
  wpi::json j;

  wpi::json props = GetPropertiesJsonObject(status);
  if (props.is_array()) j.emplace("properties", props);

  return j;
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
  if (m_properties_cached) propertyValueUpdated(property, *prop);
}

void SinkImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {}
