// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "MockListenerStorage.hpp"
#include "PubSubOptionsMatcher.hpp"
#include "TestPrinters.hpp"
#include "net/MockMessageHandler.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/util/json.hpp"

namespace wpi::nt {

inline void CheckPublish(const net::MockClientMessageHandler::PublishCall& call,
                         std::string_view name, std::string_view typeStr,
                         const wpi::util::json& properties,
                         const PubSubOptionsImpl& options = {}) {
  CHECK(call.name == name);
  CHECK(call.typeStr == typeStr);
  CHECK(call.properties == properties);
  CHECK(call.options == options);
}

inline void CheckPublish(const net::MockClientMessageHandler::PublishCall& call,
                         int pubuid, std::string_view name,
                         std::string_view typeStr,
                         const wpi::util::json& properties,
                         const PubSubOptionsImpl& options = {}) {
  CHECK(call.pubuid == pubuid);
  CheckPublish(call, name, typeStr, properties, options);
}

inline void CheckSubscribe(
    const net::MockClientMessageHandler::SubscribeCall& call,
    std::initializer_list<std::string_view> prefixes,
    const PubSubOptionsImpl& options = {}) {
  REQUIRE(call.prefixes.size() == prefixes.size());
  auto prefixIt = prefixes.begin();
  for (const auto& prefix : call.prefixes) {
    CHECK(prefix == *prefixIt);
    ++prefixIt;
  }
  CHECK(call.options == options);
}

inline void CheckSetValue(
    const net::MockClientMessageHandler::SetValueCall& call, int pubuid,
    const Value& value) {
  CHECK(call.pubuid == pubuid);
  CHECK(call.value == value);
}

inline void CheckNoClientCalls(const net::MockClientMessageHandler& handler) {
  CHECK(handler.publishCalls.empty());
  CHECK(handler.unpublishCalls.empty());
  CHECK(handler.setPropertiesCalls.empty());
  CHECK(handler.subscribeCalls.empty());
  CHECK(handler.unsubscribeCalls.empty());
  CHECK(handler.setValueCalls.empty());
}

inline void CheckNoServerCalls(const net::MockServerMessageHandler& handler) {
  CHECK(handler.announceCalls.empty());
  CHECK(handler.unannounceCalls.empty());
  CHECK(handler.propertiesUpdateCalls.empty());
  CHECK(handler.setValueCalls.empty());
}

inline void CheckValueNotifyHandles(
    const MockListenerStorage::ValueNotifyCall& call,
    std::initializer_list<NT_Listener> expectedHandles) {
  REQUIRE(call.handles.size() == expectedHandles.size());
  CHECK(std::equal(call.handles.begin(), call.handles.end(),
                   expectedHandles.begin(), expectedHandles.end()));
}

inline void CheckNetworkCounts(const net::MockClientMessageHandler& network,
                               size_t publishCount, size_t subscribeCount,
                               size_t setValueCount, size_t unpublishCount = 0,
                               size_t setPropertiesCount = 0,
                               size_t unsubscribeCount = 0) {
  REQUIRE(network.publishCalls.size() == publishCount);
  REQUIRE(network.unpublishCalls.size() == unpublishCount);
  REQUIRE(network.setPropertiesCalls.size() == setPropertiesCount);
  REQUIRE(network.subscribeCalls.size() == subscribeCount);
  REQUIRE(network.unsubscribeCalls.size() == unsubscribeCount);
  REQUIRE(network.setValueCalls.size() == setValueCount);
}

}  // namespace wpi::nt
