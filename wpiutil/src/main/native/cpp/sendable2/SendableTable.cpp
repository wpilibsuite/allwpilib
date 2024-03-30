// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable2/SendableTable.h"

#include "wpi/json.h"
#include "wpi/sendable2/SendableTableBackend.h"

using namespace wpi2;

void SendableTable::SetBoolean(std::string_view name, bool value,
                               const SendableOptions& options) {
  m_backend->SetBoolean(name, value, options);
}

void SendableTable::PublishBoolean(std::string_view name,
                                   std::function<bool()> supplier,
                                   const SendableOptions& options) {
  m_backend->PublishBoolean(name, std::move(supplier), options);
}

std::function<void(bool)> SendableTable::PublishBoolean(
    std::string_view name, const SendableOptions& options) {
  return m_backend->PublishBoolean(name, options);
}

void SendableTable::SubscribeBoolean(std::string_view name,
                                     std::function<void(bool)> consumer,
                                     const SendableOptions& options) {
  m_backend->SubscribeBoolean(name, std::move(consumer), options);
}

void SendableTable::SetInt(std::string_view name, int64_t value,
                           const SendableOptions& options) {
  m_backend->SetInt(name, value, options);
}

void SendableTable::PublishInt(std::string_view name,
                               std::function<int64_t()> supplier,
                               const SendableOptions& options) {
  m_backend->PublishInt(name, std::move(supplier), options);
}

std::function<void(int64_t)> SendableTable::PublishInt(
    std::string_view name, const SendableOptions& options) {
  return m_backend->PublishInt(name, options);
}

void SendableTable::SubscribeInt(std::string_view name,
                                 std::function<void(int64_t)> consumer,
                                 const SendableOptions& options) {
  m_backend->SubscribeInt(name, std::move(consumer), options);
}

void SendableTable::SetFloat(std::string_view name, float value,
                             const SendableOptions& options) {
  m_backend->SetFloat(name, value, options);
}

void SendableTable::PublishFloat(std::string_view name,
                                 std::function<float()> supplier,
                                 const SendableOptions& options) {
  m_backend->PublishFloat(name, std::move(supplier), options);
}

std::function<void(float)> SendableTable::PublishFloat(
    std::string_view name, const SendableOptions& options) {
  return m_backend->PublishFloat(name, options);
}

void SendableTable::SubscribeFloat(std::string_view name,
                                   std::function<void(float)> consumer,
                                   const SendableOptions& options) {
  m_backend->SubscribeFloat(name, std::move(consumer), options);
}

void SendableTable::SetDouble(std::string_view name, double value,
                              const SendableOptions& options) {
  m_backend->SetDouble(name, value, options);
}

void SendableTable::PublishDouble(std::string_view name,
                                  std::function<double()> supplier,
                                  const SendableOptions& options) {
  m_backend->PublishDouble(name, std::move(supplier), options);
}

std::function<void(double)> SendableTable::PublishDouble(
    std::string_view name, const SendableOptions& options) {
  return m_backend->PublishDouble(name, options);
}

void SendableTable::SubscribeDouble(std::string_view name,
                                    std::function<void(double)> consumer,
                                    const SendableOptions& options) {
  m_backend->SubscribeDouble(name, std::move(consumer), options);
}

void SendableTable::SetString(std::string_view name, std::string_view value,
                              const SendableOptions& options) {
  m_backend->SetString(name, value, options);
}

void SendableTable::PublishString(std::string_view name,
                                  std::function<std::string()> supplier,
                                  const SendableOptions& options) {
  m_backend->PublishString(name, std::move(supplier), options);
}

std::function<void(std::string_view)> SendableTable::PublishString(
    std::string_view name, const SendableOptions& options) {
  return m_backend->PublishString(name, options);
}

void SendableTable::SubscribeString(
    std::string_view name, std::function<void(std::string_view)> consumer,
    const SendableOptions& options) {
  m_backend->SubscribeString(name, std::move(consumer), options);
}

void SendableTable::SetRaw(std::string_view name, std::string_view typeString,
                           std::span<const uint8_t> value,
                           const SendableOptions& options) {
  m_backend->SetRaw(name, typeString, value, options);
}

void SendableTable::PublishRaw(std::string_view name,
                               std::string_view typeString,
                               std::function<std::vector<uint8_t>()> supplier,
                               const SendableOptions& options) {
  m_backend->PublishRaw(name, typeString, std::move(supplier), options);
}

void SendableTable::PublishRawSmall(
    std::string_view name, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        supplier,
    const SendableOptions& options) {
  m_backend->PublishRawSmall(name, typeString, std::move(supplier), options);
}

std::function<void(std::span<const uint8_t>)> SendableTable::PublishRaw(
    std::string_view name, std::string_view typeString,
    const SendableOptions& options) {
  return m_backend->PublishRaw(name, typeString, options);
}

void SendableTable::SubscribeRaw(
    std::string_view name, std::string_view typeString,
    std::function<void(std::span<const uint8_t>)> consumer,
    const SendableOptions& options) {
  m_backend->SubscribeRaw(name, typeString, std::move(consumer), options);
}

wpi::json SendableTable::GetProperty(std::string_view name,
                                     std::string_view propName) const {
  return m_backend->GetProperty(name, propName);
}

void SendableTable::SetProperty(std::string_view name,
                                std::string_view propName,
                                const wpi::json& value) {
  m_backend->SetProperty(name, propName, value);
}

void SendableTable::DeleteProperty(std::string_view name,
                                   std::string_view propName) {
  m_backend->DeleteProperty(name, propName);
}

wpi::json SendableTable::GetProperties(std::string_view name) const {
  return m_backend->GetProperties(name);
}

bool SendableTable::SetProperties(std::string_view name,
                                  const wpi::json& properties) {
  return m_backend->SetProperties(name, properties);
}

void SendableTable::Erase(std::string_view name) {
  return m_backend->Erase(name);
}

bool SendableTable::IsPublished() const {
  return m_backend->IsPublished();
}

void SendableTable::Update() {
  m_backend->Update();
}

void SendableTable::Clear() {
  m_backend->Clear();
}

bool SendableTable::HasSchema(std::string_view name) const {
  return m_backend->HasSchema(name);
}

void SendableTable::AddSchema(std::string_view name, std::string_view type,
                              std::span<const uint8_t> schema) {
  m_backend->AddSchema(name, type, schema);
}

void SendableTable::AddSchema(std::string_view name, std::string_view type,
                              std::string_view schema) {
  m_backend->AddSchema(name, type, schema);
}
