// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_INC_
#define NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_INC_

#include <string_view>
#include <utility>
#include <vector>

#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

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

inline NetworkTableEntry NetworkTableInstance::GetEntry(std::string_view name) {
  return NetworkTableEntry{::nt::GetEntry(m_handle, name)};
}

inline std::vector<NetworkTableEntry> NetworkTableInstance::GetEntries(
    std::string_view prefix, unsigned int types) {
  std::vector<NetworkTableEntry> entries;
  for (auto entry : ::nt::GetEntries(m_handle, prefix, types)) {
    entries.emplace_back(entry);
  }
  return entries;
}

inline std::vector<EntryInfo> NetworkTableInstance::GetEntryInfo(
    std::string_view prefix, unsigned int types) const {
  return ::nt::GetEntryInfo(m_handle, prefix, types);
}

inline void NetworkTableInstance::DeleteAllEntries() {
  ::nt::DeleteAllEntries(m_handle);
}

inline void NetworkTableInstance::RemoveEntryListener(
    NT_EntryListener entry_listener) {
  ::nt::RemoveEntryListener(entry_listener);
}

inline bool NetworkTableInstance::WaitForEntryListenerQueue(double timeout) {
  return ::nt::WaitForEntryListenerQueue(m_handle, timeout);
}

inline void NetworkTableInstance::RemoveConnectionListener(
    NT_ConnectionListener conn_listener) {
  ::nt::RemoveConnectionListener(conn_listener);
}

inline bool NetworkTableInstance::WaitForConnectionListenerQueue(
    double timeout) {
  return ::nt::WaitForConnectionListenerQueue(m_handle, timeout);
}

inline bool NetworkTableInstance::WaitForRpcCallQueue(double timeout) {
  return ::nt::WaitForRpcCallQueue(m_handle, timeout);
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
                                              unsigned int port) {
  ::nt::StartServer(m_handle, persist_filename, listen_address, port);
}

inline void NetworkTableInstance::StopServer() {
  ::nt::StopServer(m_handle);
}

inline void NetworkTableInstance::StartClient() {
  ::nt::StartClient(m_handle);
}

inline void NetworkTableInstance::StartClient(const char* server_name,
                                              unsigned int port) {
  ::nt::StartClient(m_handle, server_name, port);
}

inline void NetworkTableInstance::StartClient(
    wpi::span<const std::pair<std::string_view, unsigned int>> servers) {
  ::nt::StartClient(m_handle, servers);
}

inline void NetworkTableInstance::StartClientTeam(unsigned int team,
                                                  unsigned int port) {
  ::nt::StartClientTeam(m_handle, team, port);
}

inline void NetworkTableInstance::StopClient() {
  ::nt::StopClient(m_handle);
}

inline void NetworkTableInstance::SetServer(const char* server_name,
                                            unsigned int port) {
  ::nt::SetServer(m_handle, server_name, port);
}

inline void NetworkTableInstance::SetServer(
    wpi::span<const std::pair<std::string_view, unsigned int>> servers) {
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

inline void NetworkTableInstance::SetUpdateRate(double interval) {
  ::nt::SetUpdateRate(m_handle, interval);
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

inline const char* NetworkTableInstance::SavePersistent(
    std::string_view filename) const {
  return ::nt::SavePersistent(m_handle, filename);
}

inline const char* NetworkTableInstance::LoadPersistent(
    std::string_view filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return ::nt::LoadPersistent(m_handle, filename, warn);
}

inline const char* NetworkTableInstance::SaveEntries(
    std::string_view filename, std::string_view prefix) const {
  return ::nt::SaveEntries(m_handle, filename, prefix);
}

inline const char* NetworkTableInstance::LoadEntries(
    std::string_view filename, std::string_view prefix,
    std::function<void(size_t line, const char* msg)> warn) {
  return ::nt::LoadEntries(m_handle, filename, prefix, warn);
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

inline bool NetworkTableInstance::WaitForLoggerQueue(double timeout) {
  return ::nt::WaitForLoggerQueue(m_handle, timeout);
}

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLEINSTANCE_INC_
