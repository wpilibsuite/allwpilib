// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

namespace glass {

class NetworkTablesHelper {
 public:
  explicit NetworkTablesHelper(NT_Inst inst);
  ~NetworkTablesHelper();

  NetworkTablesHelper(const NetworkTablesHelper&) = delete;
  NetworkTablesHelper& operator=(const NetworkTablesHelper&) = delete;

  NT_Inst GetInstance() const { return m_inst; }
  NT_EntryListenerPoller GetPoller() const { return m_poller; }

  NT_Entry GetEntry(const wpi::Twine& name) const {
    return nt::GetEntry(m_inst, name);
  }

  static constexpr int kDefaultListenerFlags =
      NT_NOTIFY_LOCAL | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE | NT_NOTIFY_IMMEDIATE;

  NT_EntryListener AddListener(NT_Entry entry,
                               unsigned int flags = kDefaultListenerFlags) {
    return nt::AddPolledEntryListener(m_poller, entry, flags);
  }

  NT_EntryListener AddListener(const wpi::Twine& prefix,
                               unsigned int flags = kDefaultListenerFlags) {
    return nt::AddPolledEntryListener(m_poller, prefix, flags);
  }

  std::vector<nt::EntryNotification> PollListener() {
    bool timedOut = false;
    return nt::PollEntryListener(m_poller, 0, &timedOut);
  }

  bool IsConnected() const;

 private:
  NT_Inst m_inst;
  NT_EntryListenerPoller m_poller;
};

}  // namespace glass
