// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

#include <wpi/json_fwd.h>

#include "ntcore_cpp.h"

namespace nt {
class PubSubOptionsImpl;
class Value;
}  // namespace nt

namespace nt::net {

class LocalInterface {
 public:
  virtual ~LocalInterface() = default;

  virtual NT_Topic NetworkAnnounce(std::string_view name,
                                   std::string_view typeStr,
                                   const wpi::json& properties,
                                   std::optional<int> pubuid) = 0;
  virtual void NetworkUnannounce(std::string_view name) = 0;
  virtual void NetworkPropertiesUpdate(std::string_view name,
                                       const wpi::json& update, bool ack) = 0;
  virtual void NetworkSetValue(NT_Topic topicHandle, const Value& value) = 0;
};

class NetworkInterface {
 public:
  virtual ~NetworkInterface() = default;

  virtual void Publish(int pubuid, std::string_view name,
                       std::string_view typeStr, const wpi::json& properties,
                       const PubSubOptionsImpl& options) = 0;
  virtual void Unpublish(int pubuid) = 0;
  virtual void SetProperties(std::string_view name,
                             const wpi::json& update) = 0;
  virtual void Subscribe(int subuid, std::span<const std::string> topicNames,
                         const PubSubOptionsImpl& options) = 0;
  virtual void Unsubscribe(int subuid) = 0;
  virtual void SetValue(int pubuid, const Value& value) = 0;
};

class ILocalStorage : public LocalInterface {
 public:
  virtual void StartNetwork(NetworkInterface* network) = 0;
  virtual void ClearNetwork() = 0;
};

}  // namespace nt::net
