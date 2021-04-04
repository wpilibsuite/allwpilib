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
  NT_TopicListenerPoller GetTopicPoller() const { return m_topicPoller; }
  NT_ValueListenerPoller GetValuePoller() const { return m_valuePoller; }

  NT_Entry GetEntry(std::string_view name) const {
    return nt::GetEntry(m_inst, name);
  }

  static constexpr int kDefaultTopicListenerFlags = NT_TOPIC_NOTIFY_PUBLISH |
                                                    NT_TOPIC_NOTIFY_UNPUBLISH |
                                                    NT_TOPIC_NOTIFY_IMMEDIATE;
  static constexpr int kDefaultValueListenerFlags =
      NT_VALUE_NOTIFY_IMMEDIATE | NT_VALUE_NOTIFY_LOCAL;

  NT_TopicListener AddTopicListener(
      NT_Topic topic, unsigned int flags = kDefaultTopicListenerFlags) {
    return nt::AddPolledTopicListener(m_topicPoller, topic, flags);
  }

  NT_TopicListener AddTopicListener(
      std::string_view prefix,
      unsigned int flags = kDefaultTopicListenerFlags) {
    return nt::AddPolledTopicListener(m_topicPoller, {&prefix, 1}, flags);
  }

  std::vector<nt::TopicNotification> PollTopicListener() {
    return nt::ReadTopicListenerQueue(m_topicPoller);
  }

  NT_ValueListener AddValueListener(
      NT_Handle subentry, unsigned int mask = kDefaultValueListenerFlags) {
    return nt::AddPolledValueListener(m_valuePoller, subentry, mask);
  }

  std::vector<nt::ValueNotification> PollValueListener() {
    return nt::ReadValueListenerQueue(m_valuePoller);
  }

  bool IsConnected() const;

 private:
  NT_Inst m_inst;
  NT_TopicListenerPoller m_topicPoller;
  NT_ValueListenerPoller m_valuePoller;
};

}  // namespace glass
