/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_INSTANCE_INL_
#define NT_INSTANCE_INL_

namespace nt {

inline NetworkTableInstance::NetworkTableInstance() noexcept : m_handle{0} {}

inline NetworkTableInstance::NetworkTableInstance(NT_Inst handle) noexcept
    : m_handle{handle} {}

inline NetworkTableInstance NetworkTableInstance::GetDefault() {
  return NetworkTableInstance{GetDefaultInstance()};
}

inline NetworkTableInstance NetworkTableInstance::Create() {
  return NetworkTableInstance{CreateInstance()};
}

inline void NetworkTableInstance::Destroy(NetworkTableInstance inst) {
  if (inst.m_handle != 0) DestroyInstance(inst.m_handle);
}

inline NT_Inst NetworkTableInstance::GetHandle() const { return m_handle; }

inline NetworkTableEntry NetworkTableInstance::GetEntry(const Twine& name) {
  return NetworkTableEntry{::nt::GetEntry(m_handle, name)};
}

inline std::vector<NetworkTableEntry> NetworkTableInstance::GetEntries(
    const Twine& prefix, unsigned int types) {
  std::vector<NetworkTableEntry> entries;
  for (auto entry : ::nt::GetEntries(m_handle, prefix, types))
    entries.emplace_back(entry);
  return entries;
}

inline std::vector<EntryInfo> NetworkTableInstance::GetEntryInfo(
    const Twine& prefix, unsigned int types) const {
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

inline void NetworkTableInstance::SetNetworkIdentity(const Twine& name) {
  ::nt::SetNetworkIdentity(m_handle, name);
}

inline unsigned int NetworkTableInstance::GetNetworkMode() const {
  return ::nt::GetNetworkMode(m_handle);
}

inline void NetworkTableInstance::StartServer(const Twine& persist_filename,
                                              const char* listen_address,
                                              unsigned int port) {
  ::nt::StartServer(m_handle, persist_filename, listen_address, port);
}

inline void NetworkTableInstance::StopServer() { ::nt::StopServer(m_handle); }

inline void NetworkTableInstance::StartClient() { ::nt::StartClient(m_handle); }

inline void NetworkTableInstance::StartClient(const char* server_name,
                                              unsigned int port) {
  ::nt::StartClient(m_handle, server_name, port);
}

inline void NetworkTableInstance::StartClient(
    ArrayRef<std::pair<StringRef, unsigned int>> servers) {
  ::nt::StartClient(m_handle, servers);
}

inline void NetworkTableInstance::StartClientTeam(unsigned int team,
                                                  unsigned int port) {
  ::nt::StartClientTeam(m_handle, team, port);
}

inline void NetworkTableInstance::StopClient() { ::nt::StopClient(m_handle); }

inline void NetworkTableInstance::SetServer(const char* server_name,
                                            unsigned int port) {
  ::nt::SetServer(m_handle, server_name, port);
}

inline void NetworkTableInstance::SetServer(
    ArrayRef<std::pair<StringRef, unsigned int>> servers) {
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

inline void NetworkTableInstance::Flush() const { ::nt::Flush(m_handle); }

inline std::vector<ConnectionInfo> NetworkTableInstance::GetConnections()
    const {
  return ::nt::GetConnections(m_handle);
}

inline bool NetworkTableInstance::IsConnected() const {
  return ::nt::IsConnected(m_handle);
}

inline const char* NetworkTableInstance::SavePersistent(
    const Twine& filename) const {
  return ::nt::SavePersistent(m_handle, filename);
}

inline const char* NetworkTableInstance::LoadPersistent(
    const Twine& filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return ::nt::LoadPersistent(m_handle, filename, warn);
}

inline const char* NetworkTableInstance::SaveEntries(
    const Twine& filename, const Twine& prefix) const {
  return ::nt::SaveEntries(m_handle, filename, prefix);
}

inline const char* NetworkTableInstance::LoadEntries(
    const Twine& filename, const Twine& prefix,
    std::function<void(size_t line, const char* msg)> warn) {
  return ::nt::LoadEntries(m_handle, filename, prefix, warn);
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

#endif  // NT_INSTANCE_INL_
