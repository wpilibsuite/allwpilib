// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <wpi/json.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "InstanceImpl.h"
#include "Log.h"
#include "Types_internal.h"
#include "ntcore.h"

static std::atomic_bool gNowSet{false};
static std::atomic<int64_t> gNowTime;

namespace nt {

wpi::json TopicInfo::GetProperties() const {
  try {
    return wpi::json::parse(properties);
  } catch (wpi::json::parse_error&) {
    return wpi::json::object();
  }
}

/*
 * Instance Functions
 */

NT_Inst GetDefaultInstance() {
  return Handle{InstanceImpl::GetDefaultIndex(), 0, Handle::kInstance};
}

NT_Inst CreateInstance() {
  return Handle{InstanceImpl::Alloc(), 0, Handle::kInstance};
}

void DestroyInstance(NT_Inst inst) {
  int i = Handle{inst}.GetTypedInst(Handle::kInstance);
  if (i < 0) {
    return;
  }
  InstanceImpl::Destroy(i);
}

NT_Inst GetInstanceFromHandle(NT_Handle handle) {
  Handle h{handle};
  auto type = h.GetType();
  if (type >= Handle::kConnectionListener && type < Handle::kTypeMax) {
    return Handle(h.GetInst(), 0, Handle::kInstance);
  }

  return 0;
}

/*
 * Table Functions
 */

NT_Entry GetEntry(NT_Inst inst, std::string_view name) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetEntry(name);
  } else {
    return {};
  }
}

std::string GetEntryName(NT_Entry entry) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    return ii->localStorage.GetEntryName(entry);
  } else {
    return {};
  }
}

NT_Type GetEntryType(NT_Entry entry) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    return ii->localStorage.GetEntryType(entry);
  } else {
    return {};
  }
}

int64_t GetEntryLastChange(NT_Handle subentry) {
  if (auto ii = InstanceImpl::GetHandle(subentry)) {
    return ii->localStorage.GetEntryLastChange(subentry);
  } else {
    return {};
  }
}

Value GetEntryValue(NT_Handle subentry) {
  if (auto ii = InstanceImpl::GetHandle(subentry)) {
    return ii->localStorage.GetEntryValue(subentry);
  } else {
    return {};
  }
}

bool SetDefaultEntryValue(NT_Entry entry, const Value& value) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    return ii->localStorage.SetDefaultEntryValue(entry, value);
  } else {
    return {};
  }
}

bool SetEntryValue(NT_Entry entry, const Value& value) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    return ii->localStorage.SetEntryValue(entry, value);
  } else {
    return {};
  }
}

void SetEntryFlags(NT_Entry entry, unsigned int flags) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    ii->localStorage.SetEntryFlags(entry, flags);
  }
}

unsigned int GetEntryFlags(NT_Entry entry) {
  if (auto ii = InstanceImpl::GetHandle(entry)) {
    return ii->localStorage.GetEntryFlags(entry);
  } else {
    return {};
  }
}

std::vector<Value> ReadQueueValue(NT_Handle subentry) {
  if (auto ii = InstanceImpl::GetHandle(subentry)) {
    return ii->localStorage.ReadQueueValue(subentry);
  } else {
    return {};
  }
}

/*
 * Topic Functions
 */

std::vector<NT_Topic> GetTopics(NT_Inst inst, std::string_view prefix,
                                unsigned int types) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetTopics(prefix, types);
  } else {
    return {};
  }
}

std::vector<NT_Topic> GetTopics(NT_Inst inst, std::string_view prefix,
                                std::span<const std::string_view> types) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetTopics(prefix, types);
  } else {
    return {};
  }
}

std::vector<TopicInfo> GetTopicInfo(NT_Inst inst, std::string_view prefix,
                                    unsigned int types) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetTopicInfo(prefix, types);
  } else {
    return {};
  }
}

std::vector<TopicInfo> GetTopicInfo(NT_Inst inst, std::string_view prefix,
                                    std::span<const std::string_view> types) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetTopicInfo(prefix, types);
  } else {
    return {};
  }
}

TopicInfo GetTopicInfo(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicInfo(topic);
  } else {
    return {};
  }
}

NT_Topic GetTopic(NT_Inst inst, std::string_view name) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.GetTopic(name);
  } else {
    return {};
  }
}

std::string GetTopicName(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicName(topic);
  } else {
    return {};
  }
}

NT_Type GetTopicType(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicType(topic);
  } else {
    return {};
  }
}

std::string GetTopicTypeString(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicTypeString(topic);
  } else {
    return {};
  }
}

void SetTopicPersistent(NT_Topic topic, bool value) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    ii->localStorage.SetTopicPersistent(topic, value);
  } else {
    return;
  }
}

bool GetTopicPersistent(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicPersistent(topic);
  } else {
    return {};
  }
}

void SetTopicRetained(NT_Topic topic, bool value) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    ii->localStorage.SetTopicRetained(topic, value);
  } else {
    return;
  }
}

bool GetTopicRetained(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicRetained(topic);
  } else {
    return {};
  }
}

bool GetTopicExists(NT_Handle handle) {
  if (auto ii = InstanceImpl::GetHandle(handle)) {
    return ii->localStorage.GetTopicExists(handle);
  }
  return false;
}

wpi::json GetTopicProperty(NT_Topic topic, std::string_view name) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicProperty(topic, name);
  } else {
    return {};
  }
}

void SetTopicProperty(NT_Topic topic, std::string_view name,
                      const wpi::json& value) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    ii->localStorage.SetTopicProperty(topic, name, value);
  } else {
    return;
  }
}

void DeleteTopicProperty(NT_Topic topic, std::string_view name) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    ii->localStorage.DeleteTopicProperty(topic, name);
  }
}

wpi::json GetTopicProperties(NT_Topic topic) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetTopicProperties(topic);
  } else {
    return {};
  }
}

bool SetTopicProperties(NT_Topic topic, const wpi::json& properties) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.SetTopicProperties(topic, properties);
  } else {
    return {};
  }
}

NT_Subscriber Subscribe(NT_Topic topic, NT_Type type, std::string_view typeStr,
                        std::span<const PubSubOption> options) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.Subscribe(topic, type, typeStr, options);
  } else {
    return {};
  }
}

void Unsubscribe(NT_Subscriber sub) {
  if (auto ii = InstanceImpl::GetTyped(sub, Handle::kSubscriber)) {
    ii->localStorage.Unsubscribe(sub);
  }
}

NT_Publisher Publish(NT_Topic topic, NT_Type type, std::string_view typeStr,
                     std::span<const PubSubOption> options) {
  return PublishEx(topic, type, typeStr, wpi::json::object(), options);
}

NT_Publisher PublishEx(NT_Topic topic, NT_Type type, std::string_view typeStr,
                       const wpi::json& properties,
                       std::span<const PubSubOption> options) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.Publish(topic, type, typeStr, properties, options);
  } else {
    return {};
  }
}

void Unpublish(NT_Handle pubentry) {
  if (auto ii = InstanceImpl::GetHandle(pubentry)) {
    ii->localStorage.Unpublish(pubentry);
  }
}

NT_Entry GetEntry(NT_Topic topic, NT_Type type, std::string_view typeStr,
                  std::span<const PubSubOption> options) {
  if (auto ii = InstanceImpl::GetTyped(topic, Handle::kTopic)) {
    return ii->localStorage.GetEntry(topic, type, typeStr, options);
  } else {
    return {};
  }
}

void ReleaseEntry(NT_Entry entry) {
  if (auto ii = InstanceImpl::GetTyped(entry, Handle::kEntry)) {
    ii->localStorage.ReleaseEntry(entry);
  }
}

void Release(NT_Handle pubsubentry) {
  if (auto ii = InstanceImpl::GetHandle(pubsubentry)) {
    ii->localStorage.Release(pubsubentry);
  }
}

NT_Topic GetTopicFromHandle(NT_Handle pubsubentry) {
  if (auto ii = InstanceImpl::GetHandle(pubsubentry)) {
    return ii->localStorage.GetTopicFromHandle(pubsubentry);
  } else {
    return {};
  }
}

NT_MultiSubscriber SubscribeMultiple(NT_Inst inst,
                                     std::span<const std::string_view> prefixes,
                                     std::span<const PubSubOption> options) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.SubscribeMultiple(prefixes, options);
  } else {
    return {};
  }
}

void UnsubscribeMultiple(NT_MultiSubscriber sub) {
  if (auto ii = InstanceImpl::GetTyped(sub, Handle::kMultiSubscriber)) {
    ii->localStorage.UnsubscribeMultiple(sub);
  }
}

/*
 * Callback Creation Functions
 */

NT_TopicListener AddTopicListener(
    NT_Inst inst, std::span<const std::string_view> prefixes, unsigned int mask,
    std::function<void(const TopicNotification&)> callback) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.AddTopicListener(prefixes, mask,
                                             std::move(callback));
  } else {
    return {};
  }
}

NT_TopicListener AddTopicListener(
    NT_Handle handle, unsigned int mask,
    std::function<void(const TopicNotification&)> callback) {
  if (auto ii = InstanceImpl::GetTyped(handle, Handle::kTopic)) {
    return ii->localStorage.AddTopicListener(handle, mask, std::move(callback));
  } else {
    return {};
  }
}

bool WaitForTopicListenerQueue(NT_Handle handle, double timeout) {
  if (auto ii = InstanceImpl::GetHandle(handle)) {
    return ii->localStorage.WaitForTopicListenerQueue(timeout);
  } else {
    return {};
  }
}

NT_TopicListenerPoller CreateTopicListenerPoller(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.CreateTopicListenerPoller();
  } else {
    return {};
  }
}

void DestroyTopicListenerPoller(NT_TopicListenerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kTopicListenerPoller)) {
    ii->localStorage.DestroyTopicListenerPoller(poller);
  }
}

NT_TopicListener AddPolledTopicListener(
    NT_TopicListenerPoller poller, std::span<const std::string_view> prefixes,
    unsigned int mask) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kTopicListenerPoller)) {
    return ii->localStorage.AddPolledTopicListener(poller, prefixes, mask);
  } else {
    return {};
  }
}

NT_TopicListener AddPolledTopicListener(NT_TopicListenerPoller poller,
                                        NT_Handle handle, unsigned int mask) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kTopicListenerPoller)) {
    return ii->localStorage.AddPolledTopicListener(poller, handle, mask);
  } else {
    return {};
  }
}

std::vector<TopicNotification> ReadTopicListenerQueue(
    NT_TopicListenerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kTopicListenerPoller)) {
    return ii->localStorage.ReadTopicListenerQueue(poller);
  } else {
    return {};
  }
}

void RemoveTopicListener(NT_TopicListener listener) {
  if (auto ii = InstanceImpl::GetTyped(listener, Handle::kTopicListener)) {
    return ii->localStorage.RemoveTopicListener(listener);
  }
}

NT_ValueListener AddValueListener(
    NT_Handle subentry, unsigned int mask,
    std::function<void(const ValueNotification&)> callback) {
  if (auto ii = InstanceImpl::GetHandle(subentry)) {
    return ii->localStorage.AddValueListener(subentry, mask,
                                             std::move(callback));
  } else {
    return {};
  }
}

bool WaitForValueListenerQueue(NT_Handle handle, double timeout) {
  if (auto ii = InstanceImpl::GetHandle(handle)) {
    return ii->localStorage.WaitForValueListenerQueue(timeout);
  } else {
    return {};
  }
}

NT_ValueListenerPoller CreateValueListenerPoller(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.CreateValueListenerPoller();
  } else {
    return {};
  }
}

void DestroyValueListenerPoller(NT_ValueListenerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kValueListenerPoller)) {
    ii->localStorage.DestroyValueListenerPoller(poller);
  }
}

NT_ValueListener AddPolledValueListener(NT_ValueListenerPoller poller,
                                        NT_Handle subentry, unsigned int mask) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kValueListenerPoller)) {
    return ii->localStorage.AddPolledValueListener(poller, subentry, mask);
  } else {
    return {};
  }
}

std::vector<ValueNotification> ReadValueListenerQueue(
    NT_ValueListenerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kValueListenerPoller)) {
    return ii->localStorage.ReadValueListenerQueue(poller);
  } else {
    return {};
  }
}

void RemoveValueListener(NT_ValueListener listener) {
  if (auto ii = InstanceImpl::GetTyped(listener, Handle::kValueListener)) {
    return ii->localStorage.RemoveValueListener(listener);
  }
}

NT_ConnectionListener AddConnectionListener(
    NT_Inst inst, bool immediate_notify,
    std::function<void(const ConnectionNotification& event)> callback) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->connectionList.AddListener(immediate_notify,
                                          std::move(callback));
  } else {
    return {};
  }
}

bool WaitForConnectionListenerQueue(NT_Handle handle, double timeout) {
  if (auto ii = InstanceImpl::GetHandle(handle)) {
    return ii->connectionList.WaitForListenerQueue(timeout);
  } else {
    return {};
  }
}

NT_ConnectionListenerPoller CreateConnectionListenerPoller(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->connectionList.CreateListenerPoller();
  } else {
    return {};
  }
}

void DestroyConnectionListenerPoller(NT_ConnectionListenerPoller poller) {
  if (auto ii =
          InstanceImpl::GetTyped(poller, Handle::kConnectionListenerPoller)) {
    return ii->connectionList.DestroyListenerPoller(poller);
  }
}

NT_ConnectionListener AddPolledConnectionListener(
    NT_ConnectionListenerPoller poller, bool immediate_notify) {
  if (auto ii =
          InstanceImpl::GetTyped(poller, Handle::kConnectionListenerPoller)) {
    return ii->connectionList.AddPolledListener(poller, immediate_notify);
  } else {
    return {};
  }
}

std::vector<ConnectionNotification> ReadConnectionListenerQueue(
    NT_ConnectionListenerPoller poller) {
  if (auto ii =
          InstanceImpl::GetTyped(poller, Handle::kConnectionListenerPoller)) {
    return ii->connectionList.ReadListenerQueue(poller);
  } else {
    return {};
  }
}

void RemoveConnectionListener(NT_ConnectionListener listener) {
  if (auto ii = InstanceImpl::GetTyped(listener, Handle::kConnectionListener)) {
    return ii->connectionList.RemoveListener(listener);
  }
}

int64_t Now() {
  if (gNowSet) {
    return gNowTime;
  }
  return wpi::Now();
}

void SetNow(int64_t timestamp) {
  gNowTime = timestamp;
  gNowSet = true;
}

NT_Type GetTypeFromString(std::string_view typeString) {
  if (typeString.empty()) {
    return NT_UNASSIGNED;
  } else {
    return StringToType(typeString);
  }
}

std::string_view GetStringFromType(NT_Type type) {
  if (type == NT_UNASSIGNED) {
    return "";
  } else {
    return TypeToString(type);
  }
}

/*
 * Data Logger Functions
 */
NT_DataLogger StartEntryDataLog(NT_Inst inst, wpi::log::DataLog& log,
                                std::string_view prefix,
                                std::string_view logPrefix) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->localStorage.StartDataLog(log, prefix, logPrefix);
  } else {
    return 0;
  }
}

void StopEntryDataLog(NT_DataLogger logger) {
  if (auto ii = InstanceImpl::GetTyped(logger, Handle::kDataLogger)) {
    ii->localStorage.StopDataLog(logger);
  }
}

NT_ConnectionDataLogger StartConnectionDataLog(NT_Inst inst,
                                               wpi::log::DataLog& log,
                                               std::string_view name) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->connectionList.StartDataLog(log, name);
  } else {
    return 0;
  }
}

void StopConnectionDataLog(NT_ConnectionDataLogger logger) {
  if (auto ii = InstanceImpl::GetTyped(logger, Handle::kConnectionDataLogger)) {
    ii->connectionList.StopDataLog(logger);
  }
}

/*
 * Client/Server Functions
 */

unsigned int GetNetworkMode(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->networkMode;
  } else {
    return {};
  }
}

void StartLocal(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StartLocal();
  }
}

void StopLocal(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StopLocal();
  }
}

void StartServer(NT_Inst inst, std::string_view persist_filename,
                 const char* listen_address, unsigned int port3,
                 unsigned int port4) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StartServer(persist_filename, listen_address, port3, port4);
  }
}

void StopServer(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StopServer();
  }
}

void StartClient3(NT_Inst inst, std::string_view identity) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StartClient3(identity);
  }
}

void StartClient4(NT_Inst inst, std::string_view identity) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StartClient4(identity);
  }
}

void StopClient(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    ii->StopClient();
  }
}

void SetServer(NT_Inst inst, const char* server_name, unsigned int port) {
  SetServer(inst, {{{server_name, port}}});
}

void SetServer(
    NT_Inst inst,
    std::span<const std::pair<std::string_view, unsigned int>> servers) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    std::vector<std::pair<std::string, unsigned int>> serversCopy;
    serversCopy.reserve(servers.size());
    for (auto&& server : servers) {
      serversCopy.emplace_back(std::string{server.first}, server.second);
    }
    ii->SetServers(serversCopy);
  }
}

void SetServerTeam(NT_Inst inst, unsigned int team, unsigned int port) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    std::vector<std::pair<std::string, unsigned int>> servers;
    servers.reserve(5);

    // 10.te.am.2
    servers.emplace_back(fmt::format("10.{}.{}.2", static_cast<int>(team / 100),
                                     static_cast<int>(team % 100)),
                         port);

    // 172.22.11.2
    servers.emplace_back("172.22.11.2", port);

    // roboRIO-<team>-FRC.local
    servers.emplace_back(fmt::format("roboRIO-{}-FRC.local", team), port);

    // roboRIO-<team>-FRC.lan
    servers.emplace_back(fmt::format("roboRIO-{}-FRC.lan", team), port);

    // roboRIO-<team>-FRC.frc-field.local
    servers.emplace_back(fmt::format("roboRIO-{}-FRC.frc-field.local", team),
                         port);

    ii->SetServers(servers);
  }
}

void StartDSClient(NT_Inst inst, unsigned int port) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    if (auto client = ii->GetClient()) {
      client->StartDSClient(port);
    }
  }
}

void StopDSClient(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    if (auto client = ii->GetClient()) {
      client->StopDSClient();
    }
  }
}

void FlushLocal(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    if (auto client = ii->GetClient()) {
      client->FlushLocal();
    } else if (auto server = ii->GetServer()) {
      server->FlushLocal();
    }
  }
}

void Flush(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    if (auto client = ii->GetClient()) {
      client->Flush();
    } else if (auto server = ii->GetServer()) {
      server->Flush();
    }
  }
}

std::vector<ConnectionInfo> GetConnections(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->connectionList.GetConnections();
  } else {
    return {};
  }
}

bool IsConnected(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->networkMode == NT_NET_MODE_LOCAL ||
           ii->connectionList.IsConnected();
  } else {
    return false;
  }
}

NT_Logger AddLogger(NT_Inst inst,
                    std::function<void(const LogMessage& msg)> func,
                    unsigned int minLevel, unsigned int maxLevel) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    if (minLevel < ii->logger.min_level()) {
      ii->logger.set_min_level(minLevel);
    }
    return ii->logger_impl.Add(std::move(func), minLevel, maxLevel);
  } else {
    return {};
  }
}

NT_LoggerPoller CreateLoggerPoller(NT_Inst inst) {
  if (auto ii = InstanceImpl::GetTyped(inst, Handle::kInstance)) {
    return ii->logger_impl.CreatePoller();
  } else {
    return {};
  }
}

void DestroyLoggerPoller(NT_LoggerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kLoggerPoller)) {
    ii->logger_impl.DestroyPoller(poller);
  }
}

NT_Logger AddPolledLogger(NT_LoggerPoller poller, unsigned int min_level,
                          unsigned int max_level) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kLoggerPoller)) {
    if (min_level < ii->logger.min_level()) {
      ii->logger.set_min_level(min_level);
    }
    return ii->logger_impl.AddPolled(poller, min_level, max_level);
  } else {
    return {};
  }
}

std::vector<LogMessage> ReadLoggerQueue(NT_LoggerPoller poller) {
  if (auto ii = InstanceImpl::GetTyped(poller, Handle::kLoggerPoller)) {
    return ii->logger_impl.ReadQueue(poller);
  } else {
    return {};
  }
}

void RemoveLogger(NT_Logger logger) {
  if (auto ii = InstanceImpl::GetTyped(logger, Handle::kLogger)) {
    ii->logger_impl.Remove(logger);
    ii->logger.set_min_level(ii->logger_impl.GetMinLevel());
  }
}

}  // namespace nt
