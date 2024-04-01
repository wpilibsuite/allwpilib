// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sendable2/SendableTable.h"

#include "wpi/json.h"
#include "wpi/sendable2/SendableTableBackend.h"
#include "wpi/sendable2/SendableWrapper.h"

using namespace wpi2;

void SendableTable::SetBoolean(std::string_view name, bool value) {
  m_backend->SetBoolean(name, value);
}

void SendableTable::PublishBoolean(std::string_view name,
                                   std::function<bool()> supplier) {
  m_backend->PublishBoolean(name, std::move(supplier));
}

std::function<void(bool)> SendableTable::PublishBoolean(std::string_view name) {
  return m_backend->PublishBoolean(name);
}

void SendableTable::SubscribeBoolean(std::string_view name,
                                     std::function<void(bool)> consumer) {
  m_backend->SubscribeBoolean(name, std::move(consumer));
}

void SendableTable::SetInt(std::string_view name, int64_t value) {
  m_backend->SetInt(name, value);
}

void SendableTable::PublishInt(std::string_view name,
                               std::function<int64_t()> supplier) {
  m_backend->PublishInt(name, std::move(supplier));
}

std::function<void(int64_t)> SendableTable::PublishInt(std::string_view name) {
  return m_backend->PublishInt(name);
}

void SendableTable::SubscribeInt(std::string_view name,
                                 std::function<void(int64_t)> consumer) {
  m_backend->SubscribeInt(name, std::move(consumer));
}

void SendableTable::SetFloat(std::string_view name, float value) {
  m_backend->SetFloat(name, value);
}

void SendableTable::PublishFloat(std::string_view name,
                                 std::function<float()> supplier) {
  m_backend->PublishFloat(name, std::move(supplier));
}

std::function<void(float)> SendableTable::PublishFloat(std::string_view name) {
  return m_backend->PublishFloat(name);
}

void SendableTable::SubscribeFloat(std::string_view name,
                                   std::function<void(float)> consumer) {
  m_backend->SubscribeFloat(name, std::move(consumer));
}

void SendableTable::SetDouble(std::string_view name, double value) {
  m_backend->SetDouble(name, value);
}

void SendableTable::PublishDouble(std::string_view name,
                                  std::function<double()> supplier) {
  m_backend->PublishDouble(name, std::move(supplier));
}

std::function<void(double)> SendableTable::PublishDouble(
    std::string_view name) {
  return m_backend->PublishDouble(name);
}

void SendableTable::SubscribeDouble(std::string_view name,
                                    std::function<void(double)> consumer) {
  m_backend->SubscribeDouble(name, std::move(consumer));
}

void SendableTable::SetString(std::string_view name, std::string_view value) {
  m_backend->SetString(name, value);
}

void SendableTable::PublishString(std::string_view name,
                                  std::function<std::string()> supplier) {
  m_backend->PublishString(name, std::move(supplier));
}

std::function<void(std::string_view)> SendableTable::PublishString(
    std::string_view name) {
  return m_backend->PublishString(name);
}

void SendableTable::SubscribeString(
    std::string_view name, std::function<void(std::string_view)> consumer) {
  m_backend->SubscribeString(name, std::move(consumer));
}

void SendableTable::SetRaw(std::string_view name, std::string_view typeString,
                           std::span<const uint8_t> value) {
  m_backend->SetRaw(name, typeString, value);
}

void SendableTable::PublishRaw(std::string_view name,
                               std::string_view typeString,
                               std::function<std::vector<uint8_t>()> supplier) {
  m_backend->PublishRaw(name, typeString, std::move(supplier));
}

void SendableTable::PublishRawSmall(
    std::string_view name, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        supplier) {
  m_backend->PublishRawSmall(name, typeString, std::move(supplier));
}

std::function<void(std::span<const uint8_t>)> SendableTable::PublishRaw(
    std::string_view name, std::string_view typeString) {
  return m_backend->PublishRaw(name, typeString);
}

void SendableTable::SubscribeRaw(
    std::string_view name, std::string_view typeString,
    std::function<void(std::span<const uint8_t>)> consumer) {
  m_backend->SubscribeRaw(name, typeString, std::move(consumer));
}

SendableTable SendableTable::AddChild(std::string_view name) {
  return SendableTable{m_backend->AddChild(name)};
}

void SendableTable::SetPublishOptions(const SendableOptions& options) {
  m_backend->SetPublishOptions(options);
}

void SendableTable::SetSubscribeOptions(const SendableOptions& options) {
  m_backend->SetSubscribeOptions(options);
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

SendableTable SendableTable::CreateSendable(
    std::string_view name, std::unique_ptr<SendableWrapper> sendable) {
  return SendableTable{m_backend->CreateSendable(name, std::move(sendable))};
}
