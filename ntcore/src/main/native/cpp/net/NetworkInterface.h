// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <string_view>

#include "ntcore_cpp.h"

namespace wpi {
class json;
}  // namespace wpi

namespace nt {
class PubSubOptions;
class Value;
}  // namespace nt

namespace nt::net {

class LocalInterface {
 public:
  virtual ~LocalInterface() = default;

  virtual NT_Topic NetworkAnnounce(std::string_view name,
                                   std::string_view typeStr,
                                   const wpi::json& properties,
                                   NT_Publisher pubHandle) = 0;
  virtual void NetworkUnannounce(std::string_view name) = 0;
  virtual void NetworkPropertiesUpdate(std::string_view name,
                                       const wpi::json& update, bool ack) = 0;
  virtual void NetworkSetValue(NT_Topic topicHandle, const Value& value) = 0;
};

class NetworkStartupInterface {
 public:
  virtual ~NetworkStartupInterface() = default;

  virtual void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                       std::string_view name, std::string_view typeStr,
                       const wpi::json& properties,
                       const PubSubOptions& options) = 0;
  virtual void Subscribe(NT_Subscriber subHandle,
                         std::span<const std::string> topicNames,
                         const PubSubOptions& options) = 0;
  virtual void SetValue(NT_Publisher pubHandle, const Value& value) = 0;
};

class NetworkInterface : public NetworkStartupInterface {
 public:
  virtual void Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle) = 0;
  virtual void SetProperties(NT_Topic topicHandle, std::string_view name,
                             const wpi::json& update) = 0;
  virtual void Unsubscribe(NT_Subscriber subHandle) = 0;
};

class ILocalStorage : public LocalInterface {
 public:
  virtual void StartNetwork(NetworkStartupInterface& startup,
                            NetworkInterface* network) = 0;
  virtual void ClearNetwork() = 0;
};

}  // namespace nt::net
