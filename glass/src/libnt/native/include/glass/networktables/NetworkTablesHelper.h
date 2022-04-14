// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <vector>

#include <ntcore_cpp.h>

namespace glass {

class NetworkTablesHelper {
 public:
  explicit NetworkTablesHelper(NT_Inst inst);
  ~NetworkTablesHelper();

  NetworkTablesHelper(const NetworkTablesHelper&) = delete;
  NetworkTablesHelper& operator=(const NetworkTablesHelper&) = delete;

  NT_Inst GetInstance() const { return m_inst; }
  NT_EntryListenerPoller GetPoller() const { return m_poller; }

  NT_Entry GetEntry(std::string_view name) const {
    return nt::GetEntry(m_inst, name);
  }

  static constexpr int kDefaultListenerFlags =
      NT_NOTIFY_LOCAL | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE | NT_NOTIFY_DELETE |
      NT_NOTIFY_IMMEDIATE;

  NT_EntryListener AddListener(NT_Entry entry,
                               unsigned int flags = kDefaultListenerFlags) {
    return nt::AddPolledEntryListener(m_poller, entry, flags);
  }

  NT_EntryListener AddListener(std::string_view prefix,
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
