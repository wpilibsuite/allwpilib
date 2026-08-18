// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "IListenerStorage.hpp"

namespace wpi::nt {

class MockListenerStorage : public IListenerStorage {
 public:
  struct ActivateCall {
    NT_Listener listenerHandle;
    unsigned int mask;
    FinishEventFunc finishEvent;
  };

  struct ConnectionNotifyCall {
    std::vector<NT_Listener> handles;
    unsigned int flags;
    std::vector<ConnectionInfo> infos;
  };

  struct TopicNotifyCall {
    std::vector<NT_Listener> handles;
    unsigned int flags;
    std::vector<TopicInfo> infos;
  };

  struct ValueNotifyCall {
    std::vector<NT_Listener> handles;
    unsigned int flags;
    NT_Topic topic;
    NT_Handle subentry;
    Value value;
  };

  struct LogNotifyCall {
    unsigned int flags;
    unsigned int level;
    std::string filename;
    unsigned int line;
    std::string message;
  };

  struct TimeSyncNotifyCall {
    std::vector<NT_Listener> handles;
    unsigned int flags;
    int64_t serverTimeOffset;
    int64_t rtt2;
    bool valid;
  };

  void Activate(NT_Listener listenerHandle, unsigned int mask,
                FinishEventFunc finishEvent = {}) override {
    activateCalls.emplace_back(listenerHandle, mask, std::move(finishEvent));
  }

  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              std::span<ConnectionInfo const* const> infos) override {
    auto& call = connectionNotifyCalls.emplace_back();
    call.handles.assign(handles.begin(), handles.end());
    call.flags = flags;
    for (auto info : infos) {
      call.infos.emplace_back(*info);
    }
  }

  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              std::span<const TopicInfo> infos) override {
    topicNotifyCalls.emplace_back(
        std::vector<NT_Listener>{handles.begin(), handles.end()}, flags,
        std::vector<TopicInfo>{infos.begin(), infos.end()});
  }

  void Notify(std::span<const NT_Listener> handles, unsigned int flags,
              NT_Topic topic, NT_Handle subentry, const Value& value) override {
    valueNotifyCalls.emplace_back(
        std::vector<NT_Listener>{handles.begin(), handles.end()}, flags, topic,
        subentry, value);
  }

  void Notify(unsigned int flags, unsigned int level, std::string_view filename,
              unsigned int line, std::string_view message) override {
    logNotifyCalls.emplace_back(flags, level, std::string{filename}, line,
                                std::string{message});
  }

  void NotifyTimeSync(std::span<const NT_Listener> handles, unsigned int flags,
                      int64_t serverTimeOffset, int64_t rtt2,
                      bool valid) override {
    timeSyncNotifyCalls.emplace_back(
        std::vector<NT_Listener>{handles.begin(), handles.end()}, flags,
        serverTimeOffset, rtt2, valid);
  }

  std::vector<ActivateCall> activateCalls;
  std::vector<ConnectionNotifyCall> connectionNotifyCalls;
  std::vector<TopicNotifyCall> topicNotifyCalls;
  std::vector<ValueNotifyCall> valueNotifyCalls;
  std::vector<LogNotifyCall> logNotifyCalls;
  std::vector<TimeSyncNotifyCall> timeSyncNotifyCalls;
};

}  // namespace wpi::nt
