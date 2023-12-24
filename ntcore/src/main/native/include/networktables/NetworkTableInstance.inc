// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <utility>
#include <vector>

#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "ntcore_c.h"

namespace nt {

inline NetworkTableInstance::NetworkTableInstance() noexcept {}

inline NetworkTableInstance::NetworkTableInstance(NT_Inst handle) noexcept
    : m_handle{handle} {}

inline NetworkTableInstance NetworkTableInstance::GetDefault() {
  return NetworkTableInstance{GetDefaultInstance()};
}

inline NetworkTableInstance NetworkTableInstance::Create() {
  return NetworkTableInstance{CreateInstance()};
}

inline void NetworkTableInstance::Destroy(NetworkTableInstance& inst) {
  if (inst.m_handle != 0) {
    DestroyInstance(inst.m_handle);
    inst.m_handle = 0;
  }
}

inline NT_Inst NetworkTableInstance::GetHandle() const {
  return m_handle;
}

template <wpi::ProtobufSerializable T>
inline ProtobufTopic<T> NetworkTableInstance::GetProtobufTopic(
    std::string_view name) const {
  return ProtobufTopic<T>{GetTopic(name)};
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline StructTopic<T, I...> NetworkTableInstance::GetStructTopic(
    std::string_view name, I... info) const {
  return StructTopic<T, I...>{GetTopic(name), std::move(info)...};
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
inline StructArrayTopic<T, I...> NetworkTableInstance::GetStructArrayTopic(
    std::string_view name, I... info) const {
  return StructArrayTopic<T, I...>{GetTopic(name), std::move(info)...};
}

inline std::vector<Topic> NetworkTableInstance::GetTopics() {
  auto handles = ::nt::GetTopics(m_handle, "", 0);
  return {handles.begin(), handles.end()};
}

inline std::vector<Topic> NetworkTableInstance::GetTopics(
    std::string_view prefix) {
  auto handles = ::nt::GetTopics(m_handle, prefix, 0);
  return {handles.begin(), handles.end()};
}

inline std::vector<Topic> NetworkTableInstance::GetTopics(
    std::string_view prefix, unsigned int types) {
  auto handles = ::nt::GetTopics(m_handle, prefix, types);
  return {handles.begin(), handles.end()};
}

inline std::vector<Topic> NetworkTableInstance::GetTopics(
    std::string_view prefix, std::span<std::string_view> types) {
  auto handles = ::nt::GetTopics(m_handle, prefix, types);
  return {handles.begin(), handles.end()};
}

inline std::vector<TopicInfo> NetworkTableInstance::GetTopicInfo() {
  return ::nt::GetTopicInfo(m_handle, "", 0);
}

inline std::vector<TopicInfo> NetworkTableInstance::GetTopicInfo(
    std::string_view prefix) {
  return ::nt::GetTopicInfo(m_handle, prefix, 0);
}

inline std::vector<TopicInfo> NetworkTableInstance::GetTopicInfo(
    std::string_view prefix, unsigned int types) {
  return ::nt::GetTopicInfo(m_handle, prefix, types);
}

inline std::vector<TopicInfo> NetworkTableInstance::GetTopicInfo(
    std::string_view prefix, std::span<std::string_view> types) {
  return ::nt::GetTopicInfo(m_handle, prefix, types);
}

inline NetworkTableEntry NetworkTableInstance::GetEntry(std::string_view name) {
  return NetworkTableEntry{::nt::GetEntry(m_handle, name)};
}

inline bool NetworkTableInstance::WaitForListenerQueue(double timeout) {
  return ::nt::WaitForListenerQueue(m_handle, timeout);
}

inline void NetworkTableInstance::RemoveListener(NT_Listener listener) {
  ::nt::RemoveListener(listener);
}

inline NT_Listener NetworkTableInstance::AddConnectionListener(
    bool immediate_notify, ListenerCallback callback) const {
  return ::nt::AddListener(
      m_handle,
      NT_EVENT_CONNECTION | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
      std::move(callback));
}

inline NT_Listener NetworkTableInstance::AddTimeSyncListener(
    bool immediate_notify, ListenerCallback callback) const {
  return ::nt::AddListener(
      m_handle, NT_EVENT_TIMESYNC | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
      std::move(callback));
}

inline NT_Listener NetworkTableInstance::AddListener(
    std::span<const std::string_view> prefixes, int eventMask,
    ListenerCallback listener) {
  return ::nt::AddListener(m_handle, prefixes, eventMask, std::move(listener));
}

inline unsigned int NetworkTableInstance::GetNetworkMode() const {
  return ::nt::GetNetworkMode(m_handle);
}

inline void NetworkTableInstance::StartLocal() {
  ::nt::StartLocal(m_handle);
}

inline void NetworkTableInstance::StopLocal() {
  ::nt::StopLocal(m_handle);
}

inline void NetworkTableInstance::StartServer(std::string_view persist_filename,
                                              const char* listen_address,
                                              unsigned int port3,
                                              unsigned int port4) {
  ::nt::StartServer(m_handle, persist_filename, listen_address, port3, port4);
}

inline void NetworkTableInstance::StopServer() {
  ::nt::StopServer(m_handle);
}

inline void NetworkTableInstance::StartClient3(std::string_view identity) {
  ::nt::StartClient3(m_handle, identity);
}

inline void NetworkTableInstance::StartClient4(std::string_view identity) {
  ::nt::StartClient4(m_handle, identity);
}

inline void NetworkTableInstance::StopClient() {
  ::nt::StopClient(m_handle);
}

inline void NetworkTableInstance::SetServer(const char* server_name,
                                            unsigned int port) {
  ::nt::SetServer(m_handle, server_name, port);
}

inline void NetworkTableInstance::SetServer(
    std::span<const std::pair<std::string_view, unsigned int>> servers) {
  ::nt::SetServer(m_handle, servers);
}

inline void NetworkTableInstance::SetServerTeam(unsigned int team,
                                                unsigned int port) {
  ::nt::SetServerTeam(m_handle, team, port);
}

inline void NetworkTableInstance::Disconnect() {
  ::nt::Disconnect(m_handle);
}

inline void NetworkTableInstance::StartDSClient(unsigned int port) {
  ::nt::StartDSClient(m_handle, port);
}

inline void NetworkTableInstance::StopDSClient() {
  ::nt::StopDSClient(m_handle);
}

inline void NetworkTableInstance::FlushLocal() const {
  ::nt::FlushLocal(m_handle);
}

inline void NetworkTableInstance::Flush() const {
  ::nt::Flush(m_handle);
}

inline std::vector<ConnectionInfo> NetworkTableInstance::GetConnections()
    const {
  return ::nt::GetConnections(m_handle);
}

inline bool NetworkTableInstance::IsConnected() const {
  return ::nt::IsConnected(m_handle);
}

inline std::optional<int64_t> NetworkTableInstance::GetServerTimeOffset()
    const {
  return ::nt::GetServerTimeOffset(m_handle);
}

inline NT_DataLogger NetworkTableInstance::StartEntryDataLog(
    wpi::log::DataLog& log, std::string_view prefix,
    std::string_view logPrefix) {
  return ::nt::StartEntryDataLog(m_handle, log, prefix, logPrefix);
}

inline void NetworkTableInstance::StopEntryDataLog(NT_DataLogger logger) {
  ::nt::StopEntryDataLog(logger);
}

inline NT_ConnectionDataLogger NetworkTableInstance::StartConnectionDataLog(
    wpi::log::DataLog& log, std::string_view name) {
  return ::nt::StartConnectionDataLog(m_handle, log, name);
}

inline void NetworkTableInstance::StopConnectionDataLog(
    NT_ConnectionDataLogger logger) {
  ::nt::StopConnectionDataLog(logger);
}

inline NT_Listener NetworkTableInstance::AddLogger(unsigned int min_level,
                                                   unsigned int max_level,
                                                   ListenerCallback func) {
  return ::nt::AddLogger(m_handle, min_level, max_level, std::move(func));
}

inline bool NetworkTableInstance::HasSchema(std::string_view name) const {
  return ::nt::HasSchema(m_handle, name);
}

inline void NetworkTableInstance::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::span<const uint8_t> schema) {
  ::nt::AddSchema(m_handle, name, type, schema);
}

inline void NetworkTableInstance::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::string_view schema) {
  ::nt::AddSchema(m_handle, name, type, schema);
}

// Suppress unused-lambda-capture warning on AddSchema() call
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"
#endif

template <wpi::ProtobufSerializable T>
void NetworkTableInstance::AddProtobufSchema(wpi::ProtobufMessage<T>& msg) {
  msg.ForEachProtobufDescriptor(
      [this](auto typeString) { return HasSchema(typeString); },
      [this](auto typeString, auto schema) {
        AddSchema(typeString, "proto:FileDescriptorProto", schema);
      });
}

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
void NetworkTableInstance::AddStructSchema(const I&... info) {
  wpi::ForEachStructSchema<T>(
      [this](auto typeString, auto schema) {
        AddSchema(typeString, "structschema", schema);
      },
      info...);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

}  // namespace nt
