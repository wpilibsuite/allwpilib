/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTableInstance.h"

#include <wpi/SmallString.h>

using namespace nt;

std::shared_ptr<NetworkTable> NetworkTableInstance::GetTable(
    const Twine& key) const {
  StringRef simple;
  bool isSimple = key.isSingleStringRef();
  if (isSimple) simple = key.getSingleStringRef();
  if (isSimple && (simple.empty() || simple == "/")) {
    return std::make_shared<NetworkTable>(m_handle, "",
                                          NetworkTable::private_init{});
  } else if (isSimple && simple[0] == NetworkTable::PATH_SEPARATOR_CHAR) {
    return std::make_shared<NetworkTable>(m_handle, key,
                                          NetworkTable::private_init{});
  } else {
    return std::make_shared<NetworkTable>(
        m_handle, Twine(NetworkTable::PATH_SEPARATOR_CHAR) + key,
        NetworkTable::private_init{});
  }
}

void NetworkTableInstance::StartClient(ArrayRef<StringRef> servers,
                                       unsigned int port) {
  wpi::SmallVector<std::pair<StringRef, unsigned int>, 8> server_ports;
  for (const auto& server : servers)
    server_ports.emplace_back(std::make_pair(server, port));
  StartClient(server_ports);
}

void NetworkTableInstance::SetServer(ArrayRef<StringRef> servers,
                                     unsigned int port) {
  wpi::SmallVector<std::pair<StringRef, unsigned int>, 8> server_ports;
  for (const auto& server : servers)
    server_ports.emplace_back(std::make_pair(server, port));
  SetServer(server_ports);
}

NT_EntryListener NetworkTableInstance::AddEntryListener(
    const Twine& prefix,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) const {
  return ::nt::AddEntryListener(m_handle, prefix, callback, flags);
}

NT_ConnectionListener NetworkTableInstance::AddConnectionListener(
    std::function<void(const ConnectionNotification& event)> callback,
    bool immediate_notify) const {
  return ::nt::AddConnectionListener(m_handle, callback, immediate_notify);
}
