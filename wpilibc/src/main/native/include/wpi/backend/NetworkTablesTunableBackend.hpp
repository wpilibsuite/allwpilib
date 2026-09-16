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
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::backend {

class NetworkTablesTunableBackendTestAccess;

/** A tunable backend that publishes tunables to NetworkTables. */
class NetworkTablesTunableBackend : public wpi::tunables::TunableBackend {
  friend class NetworkTablesTunableBackendTestAccess;

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

  /**
   * Retires this backend and releases NetworkTables publishers,
   * subscribers, listeners, and poller resources.
   */
  void Retire() override;

  bool Publish(std::string_view path, uint32_t uid,
               wpi::tunables::detail::TunableBase& tunable,
               const wpi::tunables::TunableConfig* config,
               wpi::tunables::detail::TunableTypeValue type) override;

  void MarkDirty(uint32_t uid) override;

  void Remove(std::string_view path) override;

  std::vector<PublishedTunable> RemovePrefix(std::string_view prefix) override;

  void UnregisterTunable(uint32_t uid) override;

  void Update() override;

 private:
  struct PendingErase {
    std::string path;
    std::shared_ptr<Entry> entry;
  };

  void TrackEntry(const std::shared_ptr<Entry>& entry,
                  const wpi::tunables::TunableConfig* config,
                  wpi::tunables::detail::TunableTypeValue type);
  void UntrackEntry(const std::shared_ptr<Entry>& entry);
  void EnqueueDirtyEntry(const std::shared_ptr<Entry>& entry);
  void UntrackEntryNow(const std::shared_ptr<Entry>& entry);
  void RetireEntry(const std::shared_ptr<Entry>& entry);
  void RemovePublishedEntry(std::string path,
                            const std::shared_ptr<Entry>& entry);
  void EraseEntry(std::string path, const std::shared_ptr<Entry>& entry);
  void ClearTrackedEntries();
  void ApplyDeferredErases();

  wpi::nt::NetworkTableInstance m_inst;
  std::string m_prefix;
  wpi::nt::NetworkTableListenerPoller m_poller;
  wpi::util::mutex m_mutex;
  wpi::util::StringMap<std::shared_ptr<Entry>> m_entries;
  wpi::util::DenseMap<uint32_t, std::vector<std::string>> m_uids;
  wpi::util::DenseMap<NT_Handle, std::shared_ptr<ValueEntry>> m_subscribers;
  std::vector<std::shared_ptr<Entry>> m_dirtyEntries;
  std::vector<std::shared_ptr<Entry>> m_alwaysGetEntries;
  std::vector<std::shared_ptr<Entry>> m_complexEntries;
  std::vector<PendingErase> m_deferredErases;
  std::vector<std::shared_ptr<Entry>> m_deferredUntracks;
  int m_updateDepth = 0;
  uint64_t m_nextInsertEpoch = 0;
  bool m_clearTrackedEntries = false;
  bool m_retired = false;
};

}  // namespace wpi::backend
