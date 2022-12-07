// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/mutex.h>

#include "net/NetworkInterface.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {

class IListenerStorage;

class LocalStorage final : public net::ILocalStorage {
 public:
  LocalStorage(int inst, IListenerStorage& listenerStorage,
               wpi::Logger& logger);
  LocalStorage(const LocalStorage&) = delete;
  LocalStorage& operator=(const LocalStorage&) = delete;
  ~LocalStorage() final;

  // network interface functions
  NT_Topic NetworkAnnounce(std::string_view name, std::string_view typeStr,
                           const wpi::json& properties,
                           NT_Publisher pubHandle) final;
  void NetworkUnannounce(std::string_view name) final;
  void NetworkPropertiesUpdate(std::string_view name, const wpi::json& update,
                               bool ack) final;
  void NetworkSetValue(NT_Topic topicHandle, const Value& value) final;

  void StartNetwork(net::NetworkStartupInterface& startup,
                    net::NetworkInterface* network) final;
  void ClearNetwork() final;

  // User functions.  These are the actual implementations of the corresponding
  // user API functions in ntcore_cpp.

  std::vector<NT_Topic> GetTopics(std::string_view prefix, unsigned int types);
  std::vector<NT_Topic> GetTopics(std::string_view prefix,
                                  std::span<const std::string_view> types);

  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      unsigned int types);
  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      std::span<const std::string_view> types);

  NT_Topic GetTopic(std::string_view name);

  std::string GetTopicName(NT_Topic topic);

  NT_Type GetTopicType(NT_Topic topic);

  std::string GetTopicTypeString(NT_Topic topic);

  void SetTopicPersistent(NT_Topic topic, bool value);

  bool GetTopicPersistent(NT_Topic topic);

  void SetTopicRetained(NT_Topic topic, bool value);

  bool GetTopicRetained(NT_Topic topic);

  bool GetTopicExists(NT_Handle handle);

  wpi::json GetTopicProperty(NT_Topic topic, std::string_view name);

  void SetTopicProperty(NT_Topic topic, std::string_view name,
                        const wpi::json& value);

  void DeleteTopicProperty(NT_Topic topic, std::string_view name);

  wpi::json GetTopicProperties(NT_Topic topic);

  bool SetTopicProperties(NT_Topic topic, const wpi::json& update);

  TopicInfo GetTopicInfo(NT_Topic topic);

  NT_Subscriber Subscribe(NT_Topic topic, NT_Type type,
                          std::string_view typeStr,
                          std::span<const PubSubOption> options);

  void Unsubscribe(NT_Subscriber sub);

  NT_MultiSubscriber SubscribeMultiple(
      std::span<const std::string_view> prefixes,
      std::span<const PubSubOption> options);

  void UnsubscribeMultiple(NT_MultiSubscriber subHandle);

  NT_Publisher Publish(NT_Topic topic, NT_Type type, std::string_view typeStr,
                       const wpi::json& properties,
                       std::span<const PubSubOption> options);

  void Unpublish(NT_Handle pubentry);

  NT_Entry GetEntry(NT_Topic topic, NT_Type type, std::string_view typeStr,
                    std::span<const PubSubOption> options);

  void ReleaseEntry(NT_Entry entry);

  void Release(NT_Handle pubsubentry);

  NT_Topic GetTopicFromHandle(NT_Handle pubsubentry);

  bool SetEntryValue(NT_Handle pubentry, const Value& value);

  bool SetDefaultEntryValue(NT_Handle pubsubentry, const Value& value);

  TimestampedBoolean GetAtomicBoolean(NT_Handle subentry, bool defaultValue);
  TimestampedInteger GetAtomicInteger(NT_Handle subentry, int64_t defaultValue);
  TimestampedFloat GetAtomicFloat(NT_Handle subentry, float defaultValue);
  TimestampedDouble GetAtomicDouble(NT_Handle subentry, double defaultValue);
  TimestampedString GetAtomicString(NT_Handle subentry,
                                    std::string_view defaultValue);
  TimestampedRaw GetAtomicRaw(NT_Handle subentry,
                              std::span<const uint8_t> defaultValue);
  TimestampedBooleanArray GetAtomicBooleanArray(
      NT_Handle subentry, std::span<const int> defaultValue);
  TimestampedIntegerArray GetAtomicIntegerArray(
      NT_Handle subentry, std::span<const int64_t> defaultValue);
  TimestampedFloatArray GetAtomicFloatArray(
      NT_Handle subentry, std::span<const float> defaultValue);
  TimestampedDoubleArray GetAtomicDoubleArray(
      NT_Handle subentry, std::span<const double> defaultValue);
  TimestampedStringArray GetAtomicStringArray(
      NT_Handle subentry, std::span<const std::string> defaultValue);

  TimestampedStringView GetAtomicString(NT_Handle subentry,
                                        wpi::SmallVectorImpl<char>& buf,
                                        std::string_view defaultValue);
  TimestampedRawView GetAtomicRaw(NT_Handle subentry,
                                  wpi::SmallVectorImpl<uint8_t>& buf,
                                  std::span<const uint8_t> defaultValue);
  TimestampedBooleanArrayView GetAtomicBooleanArray(
      NT_Handle subentry, wpi::SmallVectorImpl<int>& buf,
      std::span<const int> defaultValue);
  TimestampedIntegerArrayView GetAtomicIntegerArray(
      NT_Handle subentry, wpi::SmallVectorImpl<int64_t>& buf,
      std::span<const int64_t> defaultValue);
  TimestampedFloatArrayView GetAtomicFloatArray(
      NT_Handle subentry, wpi::SmallVectorImpl<float>& buf,
      std::span<const float> defaultValue);
  TimestampedDoubleArrayView GetAtomicDoubleArray(
      NT_Handle subentry, wpi::SmallVectorImpl<double>& buf,
      std::span<const double> defaultValue);

  std::vector<Value> ReadQueueValue(NT_Handle subentry);

  std::vector<TimestampedBoolean> ReadQueueBoolean(NT_Handle subentry);
  std::vector<TimestampedInteger> ReadQueueInteger(NT_Handle subentry);
  std::vector<TimestampedFloat> ReadQueueFloat(NT_Handle subentry);
  std::vector<TimestampedDouble> ReadQueueDouble(NT_Handle subentry);
  std::vector<TimestampedString> ReadQueueString(NT_Handle subentry);
  std::vector<TimestampedRaw> ReadQueueRaw(NT_Handle subentry);
  std::vector<TimestampedBooleanArray> ReadQueueBooleanArray(
      NT_Handle subentry);
  std::vector<TimestampedIntegerArray> ReadQueueIntegerArray(
      NT_Handle subentry);
  std::vector<TimestampedFloatArray> ReadQueueFloatArray(NT_Handle subentry);
  std::vector<TimestampedDoubleArray> ReadQueueDoubleArray(NT_Handle subentry);
  std::vector<TimestampedStringArray> ReadQueueStringArray(NT_Handle subentry);

  //
  // Backwards compatible user functions
  //

  Value GetEntryValue(NT_Handle subentry);
  void SetEntryFlags(NT_Entry entry, unsigned int flags);
  unsigned int GetEntryFlags(NT_Entry entry);

  // Index-only
  NT_Entry GetEntry(std::string_view name);

  std::string GetEntryName(NT_Entry entry);
  NT_Type GetEntryType(NT_Entry entry);
  int64_t GetEntryLastChange(NT_Entry entry);

  //
  // Listener functions
  //

  void AddListener(NT_Listener listener,
                   std::span<const std::string_view> prefixes,
                   unsigned int mask);
  void AddListener(NT_Listener listener, NT_Handle handle, unsigned int mask);

  void RemoveListener(NT_Listener listener, unsigned int mask);

  //
  // Data log functions
  //
  NT_DataLogger StartDataLog(wpi::log::DataLog& log, std::string_view prefix,
                             std::string_view logPrefix);
  void StopDataLog(NT_DataLogger logger);

  void Reset();

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;

  wpi::mutex m_mutex;
};

}  // namespace nt
