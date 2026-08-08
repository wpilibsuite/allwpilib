// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/tunable/TunableBackend.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::backend {

/** A tunable backend that publishes tunables to NetworkTables. */
class NetworkTablesTunableBackend : public wpi::TunableBackend {
 public:
  class Entry;
  class ValueEntry;

  /**
   * Construct.
   *
   * @param inst NetworkTables instance
   * @param prefix prefix to put in front of tunable paths in NT
   */
  NetworkTablesTunableBackend(wpi::nt::NetworkTableInstance inst,
                              std::string_view prefix);

  ~NetworkTablesTunableBackend() override;

  void Publish(std::string_view path, uint32_t uid,
               wpi::detail::TunableBase& tunable,
               const wpi::TunableConfig* config,
               wpi::detail::TunableTypeValue type) override;

  void Remove(std::string_view path) override;

  std::vector<PublishedTunable> RemovePrefix(std::string_view prefix) override;

  void UnregisterTunable(uint32_t uid) override;

  void Update() override;

 private:
  struct PendingErase {
    std::string path;
    std::shared_ptr<Entry> entry;
  };

  void CloseEntry(const std::shared_ptr<Entry>& entry);
  void EraseEntry(std::string path, const std::shared_ptr<Entry>& entry);
  void ApplyDeferredErases();

  wpi::nt::NetworkTableInstance m_inst;
  std::string m_prefix;
  wpi::nt::NetworkTableListenerPoller m_poller;
  wpi::util::mutex m_mutex;
  wpi::util::StringMap<std::shared_ptr<Entry>> m_entries;
  wpi::util::DenseMap<uint32_t, std::vector<std::string>> m_uids;
  wpi::util::DenseMap<NT_Handle, std::shared_ptr<ValueEntry>> m_subscribers;
  std::vector<PendingErase> m_deferredErases;
  int m_updateDepth = 0;
  uint64_t m_nextInsertEpoch = 0;
};

}  // namespace wpi::backend
