// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "PropertyContainer.h"

#include <memory>
#include <string>
#include <vector>

#include <wpi/Logger.h>
#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>
#include <wpi/json.h>

using namespace cs;

int PropertyContainer::GetPropertyIndex(std::string_view name) const {
  // We can't fail, so instead we create a new index if caching fails.
  CS_Status status = 0;
  if (!m_properties_cached) {
    CacheProperties(&status);
  }
  std::scoped_lock lock(m_mutex);
  int& ndx = m_properties[name];
  if (ndx == 0) {
    // create a new index
    ndx = m_propertyData.size() + 1;
    m_propertyData.emplace_back(CreateEmptyProperty(name));
  }
  return ndx;
}

std::span<int> PropertyContainer::EnumerateProperties(
    wpi::SmallVectorImpl<int>& vec, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  for (int i = 0; i < static_cast<int>(m_propertyData.size()); ++i) {
    if (m_propertyData[i]) {
      vec.push_back(i + 1);
    }
  }
  return vec;
}

CS_PropertyKind PropertyContainer::GetPropertyKind(int property) const {
  CS_Status status = 0;
  if (!m_properties_cached && !CacheProperties(&status)) {
    return CS_PROP_NONE;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    return CS_PROP_NONE;
  }
  return prop->propKind;
}

std::string_view PropertyContainer::GetPropertyName(
    int property, wpi::SmallVectorImpl<char>& buf, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return {};
  }
  // safe to not copy because we never modify it after caching
  return prop->name;
}

int PropertyContainer::GetProperty(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return 0;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  if ((prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return 0;
  }
  return prop->value;
}

void PropertyContainer::SetProperty(int property, int value,
                                    CS_Status* status) {
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }

  // Guess it's integer if we've set before get
  if (prop->propKind == CS_PROP_NONE) {
    prop->propKind = CS_PROP_INTEGER;
  }

  if ((prop->propKind & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }

  UpdatePropertyValue(property, false, value, {});
}

int PropertyContainer::GetPropertyMin(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return 0;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->minimum;
}

int PropertyContainer::GetPropertyMax(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return 0;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->maximum;
}

int PropertyContainer::GetPropertyStep(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return 0;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->step;
}

int PropertyContainer::GetPropertyDefault(int property,
                                          CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return 0;
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->defaultValue;
}

std::string_view PropertyContainer::GetStringProperty(
    int property, wpi::SmallVectorImpl<char>& buf, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return {};
  }
  if (prop->propKind != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return {};
  }
  buf.clear();
  buf.append(prop->valueStr.begin(), prop->valueStr.end());
  return {buf.data(), buf.size()};
}

void PropertyContainer::SetStringProperty(int property, std::string_view value,
                                          CS_Status* status) {
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return;
  }

  // Guess it's string if we've set before get
  if (prop->propKind == CS_PROP_NONE) {
    prop->propKind = CS_PROP_STRING;
  }

  if (prop->propKind != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return;
  }

  UpdatePropertyValue(property, true, 0, value);
}

std::vector<std::string> PropertyContainer::GetEnumPropertyChoices(
    int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) {
    return {};
  }
  std::scoped_lock lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return {};
  }
  if (prop->propKind != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return {};
  }
  return prop->enumChoices;
}

std::unique_ptr<PropertyImpl> PropertyContainer::CreateEmptyProperty(
    std::string_view name) const {
  return std::make_unique<PropertyImpl>(name);
}

bool PropertyContainer::CacheProperties(CS_Status* status) const {
  // Doesn't need to do anything.
  m_properties_cached = true;
  return true;
}

bool PropertyContainer::SetPropertiesJson(const wpi::json& config,
                                          wpi::Logger& logger,
                                          std::string_view logName,
                                          CS_Status* status) {
  for (auto&& prop : config) {
    std::string name;
    try {
      name = prop.at("name").get<std::string>();
    } catch (const wpi::json::exception& e) {
      WPI_WARNING(logger, "{}: SetConfigJson: could not read property name: {}",
                  logName, e.what());
      continue;
    }
    int n = GetPropertyIndex(name);
    try {
      auto& v = prop.at("value");
      if (v.is_string()) {
        std::string val = v.get<std::string>();
        WPI_INFO(logger, "{}: SetConfigJson: setting property '{}' to '{}'",
                 logName, name, val);
        SetStringProperty(n, val, status);
      } else if (v.is_boolean()) {
        bool val = v.get<bool>();
        WPI_INFO(logger, "{}: SetConfigJson: setting property '{}' to {}",
                 logName, name, val);
        SetProperty(n, val, status);
      } else {
        int val = v.get<int>();
        WPI_INFO(logger, "{}: SetConfigJson: setting property '{}' to {}",
                 logName, name, val);
        SetProperty(n, val, status);
      }
    } catch (const wpi::json::exception& e) {
      WPI_WARNING(logger,
                  "{}: SetConfigJson: could not read property value: {}",
                  logName, e.what());
      continue;
    }
  }

  return true;
}

wpi::json PropertyContainer::GetPropertiesJsonObject(CS_Status* status) {
  wpi::json j;
  wpi::SmallVector<int, 32> propVec;
  for (int p : EnumerateProperties(propVec, status)) {
    wpi::json prop;
    wpi::SmallString<128> strBuf;
    prop.emplace("name", GetPropertyName(p, strBuf, status));
    switch (GetPropertyKind(p)) {
      case CS_PROP_BOOLEAN:
        prop.emplace("value", static_cast<bool>(GetProperty(p, status)));
        break;
      case CS_PROP_INTEGER:
      case CS_PROP_ENUM:
        prop.emplace("value", GetProperty(p, status));
        break;
      case CS_PROP_STRING:
        prop.emplace("value", GetStringProperty(p, strBuf, status));
        break;
      default:
        continue;
    }
    j.emplace_back(prop);
  }

  return j;
}
