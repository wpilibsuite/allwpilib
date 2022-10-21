// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <utility>
#include <vector>

#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"

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

inline void NetworkTableInstance::Destroy(NetworkTableInstance inst) {
  if (inst.m_handle != 0) {
    DestroyInstance(inst.m_handle);
  }
}

inline NT_Inst NetworkTableInstance::GetHandle() const {
  return m_handle;
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

inline void NetworkTableInstance::RemoveConnectionListener(
    NT_ConnectionListener conn_listener) {
  ::nt::RemoveConnectionListener(conn_listener);
}

inline void NetworkTableInstance::SetNetworkIdentity(std::string_view name) {
  ::nt::SetNetworkIdentity(m_handle, name);
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

inline void NetworkTableInstance::StartClient3() {
  ::nt::StartClient3(m_handle);
}

inline void NetworkTableInstance::StartClient4() {
  ::nt::StartClient4(m_handle);
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

inline NT_Logger NetworkTableInstance::AddLogger(
    std::function<void(const LogMessage& msg)> func, unsigned int min_level,
    unsigned int max_level) {
  return ::nt::AddLogger(m_handle, func, min_level, max_level);
}

inline void NetworkTableInstance::RemoveLogger(NT_Logger logger) {
  ::nt::RemoveLogger(logger);
}

}  // namespace nt
