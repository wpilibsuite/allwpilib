// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/SendableBuilderImpl.h"

#include <memory>
#include <string>
#include <utility>

#include <networktables/BooleanArrayTopic.h>
#include <networktables/BooleanTopic.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/FloatArrayTopic.h>
#include <networktables/FloatTopic.h>
#include <networktables/IntegerArrayTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/RawTopic.h>
#include <networktables/StringArrayTopic.h>
#include <ntcore_cpp.h>
#include <wpi/SmallVector.h>
#include <wpi/json.h>

using namespace frc;

template <typename Topic>
void SendableBuilderImpl::PropertyImpl<Topic>::Update(bool controllable,
                                                      int64_t time) {
  if (controllable && sub && updateLocal) {
    updateLocal(sub);
  }
  if (pub && updateNetwork) {
    updateNetwork(pub, time);
  }
}

void SendableBuilderImpl::SetTable(std::shared_ptr<nt::NetworkTable> table) {
  m_table = table;
  m_controllablePublisher = table->GetBooleanTopic(".controllable").Publish();
  m_controllablePublisher.SetDefault(false);
}

std::shared_ptr<nt::NetworkTable> SendableBuilderImpl::GetTable() {
  return m_table;
}

bool SendableBuilderImpl::IsPublished() const {
  return m_table != nullptr;
}

bool SendableBuilderImpl::IsActuator() const {
  return m_actuator;
}

void SendableBuilderImpl::Update() {
  uint64_t time = nt::Now();
  for (auto& property : m_properties) {
    property->Update(m_controllable, time);
  }
  for (auto& updateTable : m_updateTables) {
    updateTable();
  }
}

void SendableBuilderImpl::StartListeners() {
  m_controllable = true;
  if (m_controllablePublisher) {
    m_controllablePublisher.Set(true);
  }
}

void SendableBuilderImpl::StopListeners() {
  m_controllable = false;
  if (m_controllablePublisher) {
    m_controllablePublisher.Set(false);
  }
}

void SendableBuilderImpl::StartLiveWindowMode() {
  if (m_safeState) {
    m_safeState();
  }
  StartListeners();
}

void SendableBuilderImpl::StopLiveWindowMode() {
  StopListeners();
  if (m_safeState) {
    m_safeState();
  }
}

void SendableBuilderImpl::ClearProperties() {
  m_properties.clear();
}

void SendableBuilderImpl::SetSmartDashboardType(std::string_view type) {
  if (!m_typePublisher) {
    m_typePublisher = m_table->GetStringTopic(".type").PublishEx(
        nt::StringTopic::kTypeString, {{"SmartDashboard", type}});
  }
  m_typePublisher.Set(type);
}

void SendableBuilderImpl::SetActuator(bool value) {
  if (!m_actuatorPublisher) {
    m_actuatorPublisher = m_table->GetBooleanTopic(".actuator").Publish();
  }
  m_actuatorPublisher.Set(value);
  m_actuator = value;
}

void SendableBuilderImpl::SetSafeState(std::function<void()> func) {
  m_safeState = func;
}

void SendableBuilderImpl::SetUpdateTable(wpi::unique_function<void()> func) {
  m_updateTables.emplace_back(std::move(func));
}

nt::Topic SendableBuilderImpl::GetTopic(std::string_view key) {
  return m_table->GetTopic(key);
}

template <typename Topic, typename Getter, typename Setter>
void SendableBuilderImpl::AddPropertyImpl(Topic topic, Getter getter,
                                          Setter setter) {
  auto prop = std::make_unique<PropertyImpl<Topic>>();
  if (getter) {
    prop->pub = topic.Publish();
    prop->updateNetwork = [=](auto& pub, int64_t time) {
      pub.Set(getter(), time);
    };
  }
  if (setter) {
    prop->sub =
        topic.Subscribe({}, {.excludePublisher = prop->pub.GetHandle()});
    prop->updateLocal = [=](auto& sub) {
      for (auto&& val : sub.ReadQueue()) {
        setter(val.value);
      }
    };
  }
  m_properties.emplace_back(std::move(prop));
}

template <typename Topic, typename Value>
void SendableBuilderImpl::PublishConstImpl(Topic topic, Value value) {
  auto prop = std::make_unique<PropertyImpl<Topic>>();
  prop->pub = topic.Publish();
  prop->pub.Set(value);
  m_properties.emplace_back(std::move(prop));
}

void SendableBuilderImpl::AddBooleanProperty(std::string_view key,
                                             std::function<bool()> getter,
                                             std::function<void(bool)> setter) {
  AddPropertyImpl(m_table->GetBooleanTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstBoolean(std::string_view key,
                                              bool value) {
  PublishConstImpl(m_table->GetBooleanTopic(key), value);
}

void SendableBuilderImpl::AddIntegerProperty(
    std::string_view key, std::function<int64_t()> getter,
    std::function<void(int64_t)> setter) {
  AddPropertyImpl(m_table->GetIntegerTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstInteger(std::string_view key,
                                              int64_t value) {
  PublishConstImpl(m_table->GetIntegerTopic(key), value);
}

void SendableBuilderImpl::AddFloatProperty(std::string_view key,
                                           std::function<float()> getter,
                                           std::function<void(float)> setter) {
  AddPropertyImpl(m_table->GetFloatTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstFloat(std::string_view key, float value) {
  PublishConstImpl(m_table->GetFloatTopic(key), value);
}

void SendableBuilderImpl::AddDoubleProperty(
    std::string_view key, std::function<double()> getter,
    std::function<void(double)> setter) {
  AddPropertyImpl(m_table->GetDoubleTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstDouble(std::string_view key,
                                             double value) {
  PublishConstImpl(m_table->GetDoubleTopic(key), value);
}

void SendableBuilderImpl::AddStringProperty(
    std::string_view key, std::function<std::string()> getter,
    std::function<void(std::string_view)> setter) {
  AddPropertyImpl(m_table->GetStringTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstString(std::string_view key,
                                             std::string_view value) {
  PublishConstImpl(m_table->GetStringTopic(key), value);
}

void SendableBuilderImpl::AddBooleanArrayProperty(
    std::string_view key, std::function<std::vector<int>()> getter,
    std::function<void(std::span<const int>)> setter) {
  AddPropertyImpl(m_table->GetBooleanArrayTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstBooleanArray(std::string_view key,
                                                   std::span<const int> value) {
  PublishConstImpl(m_table->GetBooleanArrayTopic(key), value);
}

void SendableBuilderImpl::AddIntegerArrayProperty(
    std::string_view key, std::function<std::vector<int64_t>()> getter,
    std::function<void(std::span<const int64_t>)> setter) {
  AddPropertyImpl(m_table->GetIntegerArrayTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstIntegerArray(
    std::string_view key, std::span<const int64_t> value) {
  PublishConstImpl(m_table->GetIntegerArrayTopic(key), value);
}

void SendableBuilderImpl::AddFloatArrayProperty(
    std::string_view key, std::function<std::vector<float>()> getter,
    std::function<void(std::span<const float>)> setter) {
  AddPropertyImpl(m_table->GetFloatArrayTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstFloatArray(std::string_view key,
                                                 std::span<const float> value) {
  PublishConstImpl(m_table->GetFloatArrayTopic(key), value);
}

void SendableBuilderImpl::AddDoubleArrayProperty(
    std::string_view key, std::function<std::vector<double>()> getter,
    std::function<void(std::span<const double>)> setter) {
  AddPropertyImpl(m_table->GetDoubleArrayTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstDoubleArray(
    std::string_view key, std::span<const double> value) {
  PublishConstImpl(m_table->GetDoubleArrayTopic(key), value);
}

void SendableBuilderImpl::AddStringArrayProperty(
    std::string_view key, std::function<std::vector<std::string>()> getter,
    std::function<void(std::span<const std::string>)> setter) {
  AddPropertyImpl(m_table->GetStringArrayTopic(key), std::move(getter),
                  std::move(setter));
}

void SendableBuilderImpl::PublishConstStringArray(
    std::string_view key, std::span<const std::string> value) {
  PublishConstImpl(m_table->GetStringArrayTopic(key), value);
}

void SendableBuilderImpl::AddRawProperty(
    std::string_view key, std::string_view typeString,
    std::function<std::vector<uint8_t>()> getter,
    std::function<void(std::span<const uint8_t>)> setter) {
  auto topic = m_table->GetRawTopic(key);
  auto prop = std::make_unique<PropertyImpl<nt::RawTopic>>();
  if (getter) {
    prop->pub = topic.Publish(typeString);
    prop->updateNetwork = [=](auto& pub, int64_t time) {
      pub.Set(getter(), time);
    };
  }
  if (setter) {
    prop->sub = topic.Subscribe(typeString, {},
                                {.excludePublisher = prop->pub.GetHandle()});
    prop->updateLocal = [=](auto& sub) {
      for (auto&& val : sub.ReadQueue()) {
        setter(val.value);
      }
    };
  }
  m_properties.emplace_back(std::move(prop));
}

void SendableBuilderImpl::PublishConstRaw(std::string_view key,
                                          std::string_view typeString,
                                          std::span<const uint8_t> value) {
  auto topic = m_table->GetRawTopic(key);
  auto prop = std::make_unique<PropertyImpl<nt::RawTopic>>();
  prop->pub = topic.Publish(typeString);
  prop->pub.Set(value);
  m_properties.emplace_back(std::move(prop));
}

template <typename T, size_t Size, typename Topic, typename Getter,
          typename Setter>
void SendableBuilderImpl::AddSmallPropertyImpl(Topic topic, Getter getter,
                                               Setter setter) {
  auto prop = std::make_unique<PropertyImpl<Topic>>();
  if (getter) {
    prop->pub = topic.Publish();
    prop->updateNetwork = [=](auto& pub, int64_t time) {
      wpi::SmallVector<T, Size> buf;
      pub.Set(getter(buf), time);
    };
  }
  if (setter) {
    prop->sub =
        topic.Subscribe({}, {.excludePublisher = prop->pub.GetHandle()});
    prop->updateLocal = [=](auto& sub) {
      for (auto&& val : sub.ReadQueue()) {
        setter(val.value);
      }
    };
  }
  m_properties.emplace_back(std::move(prop));
}

void SendableBuilderImpl::AddSmallStringProperty(
    std::string_view key,
    std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(std::string_view)> setter) {
  AddSmallPropertyImpl<char, 128>(m_table->GetStringTopic(key),
                                  std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallBooleanArrayProperty(
    std::string_view key,
    std::function<std::span<const int>(wpi::SmallVectorImpl<int>& buf)> getter,
    std::function<void(std::span<const int>)> setter) {
  AddSmallPropertyImpl<int, 16>(m_table->GetBooleanArrayTopic(key),
                                std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallIntegerArrayProperty(
    std::string_view key,
    std::function<std::span<const int64_t>(wpi::SmallVectorImpl<int64_t>& buf)>
        getter,
    std::function<void(std::span<const int64_t>)> setter) {
  AddSmallPropertyImpl<int64_t, 16>(m_table->GetIntegerArrayTopic(key),
                                    std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallFloatArrayProperty(
    std::string_view key,
    std::function<std::span<const float>(wpi::SmallVectorImpl<float>& buf)>
        getter,
    std::function<void(std::span<const float>)> setter) {
  AddSmallPropertyImpl<float, 16>(m_table->GetFloatArrayTopic(key),
                                  std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallDoubleArrayProperty(
    std::string_view key,
    std::function<std::span<const double>(wpi::SmallVectorImpl<double>& buf)>
        getter,
    std::function<void(std::span<const double>)> setter) {
  AddSmallPropertyImpl<double, 16>(m_table->GetDoubleArrayTopic(key),
                                   std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallStringArrayProperty(
    std::string_view key,
    std::function<
        std::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
        getter,
    std::function<void(std::span<const std::string>)> setter) {
  AddSmallPropertyImpl<std::string, 16>(m_table->GetStringArrayTopic(key),
                                        std::move(getter), std::move(setter));
}

void SendableBuilderImpl::AddSmallRawProperty(
    std::string_view key, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        getter,
    std::function<void(std::span<const uint8_t>)> setter) {
  auto topic = m_table->GetRawTopic(key);
  auto prop = std::make_unique<PropertyImpl<nt::RawTopic>>();
  if (getter) {
    prop->pub = topic.Publish(typeString);
    prop->updateNetwork = [=](auto& pub, int64_t time) {
      wpi::SmallVector<uint8_t, 128> buf;
      pub.Set(getter(buf), time);
    };
  }
  if (setter) {
    prop->sub = topic.Subscribe(typeString, {},
                                {.excludePublisher = prop->pub.GetHandle()});
    prop->updateLocal = [=](auto& sub) {
      for (auto&& val : sub.ReadQueue()) {
        setter(val.value);
      }
    };
  }
  m_properties.emplace_back(std::move(prop));
}
