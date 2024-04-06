// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/telemetry/Telemetry.h"

#include <wpi/json.h>
#include <wpi/sendable2/SendableTable.h>

using namespace frc;

wpi2::SendableTable& Telemetry::GetTableHolder() {
  static wpi2::SendableTable instance;
  return instance;
}

wpi2::SendableTable Telemetry::SetTable(wpi2::SendableTable table) {
  auto& inst = GetTableHolder();
  auto rv = std::move(inst);
  inst = std::move(table);
  return rv;
}

wpi2::SendableTable Telemetry::GetTable() {
  return GetTableHolder();
}

void Telemetry::SetBoolean(std::string_view name, bool value) {
  GetTableHolder().SetBoolean(name, value);
}

void Telemetry::SetInteger(std::string_view name, int64_t value) {
  GetTableHolder().SetInteger(name, value);
}

void Telemetry::SetFloat(std::string_view name, float value) {
  GetTableHolder().SetFloat(name, value);
}

void Telemetry::SetDouble(std::string_view name, double value) {
  GetTableHolder().SetDouble(name, value);
}

void Telemetry::SetString(std::string_view name, std::string_view value) {
  GetTableHolder().SetString(name, value);
}

void Telemetry::SetRaw(std::string_view name, std::string_view typeString,
                       std::span<const uint8_t> value) {
  GetTableHolder().SetRaw(name, typeString, value);
}

void Telemetry::PublishBoolean(std::string_view name,
                               std::function<bool()> supplier) {
  GetTableHolder().PublishBoolean(name, std::move(supplier));
}

void Telemetry::PublishInteger(std::string_view name,
                               std::function<int64_t()> supplier) {
  GetTableHolder().PublishInteger(name, std::move(supplier));
}

void Telemetry::PublishFloat(std::string_view name,
                             std::function<float()> supplier) {
  GetTableHolder().PublishFloat(name, std::move(supplier));
}

void Telemetry::PublishDouble(std::string_view name,
                              std::function<double()> supplier) {
  GetTableHolder().PublishDouble(name, std::move(supplier));
}

void Telemetry::PublishString(std::string_view name,
                              std::function<std::string()> supplier) {
  GetTableHolder().PublishString(name, std::move(supplier));
}

void Telemetry::PublishRaw(std::string_view name, std::string_view typeString,
                           std::function<std::vector<uint8_t>()> supplier) {
  GetTableHolder().PublishRaw(name, typeString, std::move(supplier));
}

void Telemetry::PublishRawSmall(
    std::string_view name, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        supplier) {
  GetTableHolder().PublishRawSmall(name, typeString, std::move(supplier));
}

std::function<void(bool)> Telemetry::AddBooleanPublisher(
    std::string_view name) {
  return GetTableHolder().AddBooleanPublisher(name);
}

std::function<void(int64_t)> Telemetry::AddIntegerPublisher(
    std::string_view name) {
  return GetTableHolder().AddIntegerPublisher(name);
}

std::function<void(float)> Telemetry::AddFloatPublisher(std::string_view name) {
  return GetTableHolder().AddFloatPublisher(name);
}

std::function<void(double)> Telemetry::AddDoublePublisher(
    std::string_view name) {
  return GetTableHolder().AddDoublePublisher(name);
}

std::function<void(std::string_view)> Telemetry::AddStringPublisher(
    std::string_view name) {
  return GetTableHolder().AddStringPublisher(name);
}

std::function<void(std::span<const uint8_t>)> Telemetry::AddRawPublisher(
    std::string_view name, std::string_view typeString) {
  return GetTableHolder().AddRawPublisher(name, typeString);
}

void Telemetry::SubscribeBoolean(std::string_view name,
                                 std::function<void(bool)> consumer) {
  GetTableHolder().SubscribeBoolean(name, std::move(consumer));
}

void Telemetry::SubscribeInteger(std::string_view name,
                                 std::function<void(int64_t)> consumer) {
  GetTableHolder().SubscribeInteger(name, std::move(consumer));
}

void Telemetry::SubscribeFloat(std::string_view name,
                               std::function<void(float)> consumer) {
  GetTableHolder().SubscribeFloat(name, std::move(consumer));
}

void Telemetry::SubscribeDouble(std::string_view name,
                                std::function<void(double)> consumer) {
  GetTableHolder().SubscribeDouble(name, std::move(consumer));
}

void Telemetry::SubscribeString(
    std::string_view name, std::function<void(std::string_view)> consumer) {
  GetTableHolder().SubscribeString(name, std::move(consumer));
}

void Telemetry::SubscribeRaw(
    std::string_view name, std::string_view typeString,
    std::function<void(std::span<const uint8_t>)> consumer) {
  GetTableHolder().SubscribeRaw(name, typeString, std::move(consumer));
}

wpi2::SendableTable Telemetry::GetChild(std::string_view name) {
  return GetTableHolder().GetChild(name);
}

void Telemetry::SetPublishOptions(const wpi2::SendableOptions& options) {
  GetTableHolder().SetPublishOptions(options);
}

void Telemetry::SetSubscribeOptions(const wpi2::SendableOptions& options) {
  GetTableHolder().SetSubscribeOptions(options);
}

wpi::json Telemetry::GetProperty(std::string_view name,
                                 std::string_view propName) {
  return GetTableHolder().GetProperty(name, propName);
}

void Telemetry::SetProperty(std::string_view name, std::string_view propName,
                            const wpi::json& value) {
  GetTableHolder().SetProperty(name, propName, value);
}

void Telemetry::DeleteProperty(std::string_view name,
                               std::string_view propName) {
  GetTableHolder().DeleteProperty(name, propName);
}

wpi::json Telemetry::GetProperties(std::string_view name) {
  return GetTableHolder().GetProperties(name);
}

bool Telemetry::SetProperties(std::string_view name,
                              const wpi::json& properties) {
  return GetTableHolder().SetProperties(name, properties);
}

void Telemetry::Remove(std::string_view name) {
  GetTableHolder().Remove(name);
}

void Telemetry::Clear() {
  GetTableHolder().Clear();
}
