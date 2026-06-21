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

struct ClientMessageCounts {
  size_t publish = 0;
  size_t unpublish = 0;
  size_t setProperties = 0;
  size_t subscribe = 0;
  size_t unsubscribe = 0;
  size_t setValue = 0;
};

inline void CheckClientMessageCounts(
    const net::MockClientMessageHandler& handler,
    const ClientMessageCounts& expected = {}) {
  REQUIRE(handler.publishCalls.size() == expected.publish);
  REQUIRE(handler.unpublishCalls.size() == expected.unpublish);
  REQUIRE(handler.setPropertiesCalls.size() == expected.setProperties);
  REQUIRE(handler.subscribeCalls.size() == expected.subscribe);
  REQUIRE(handler.unsubscribeCalls.size() == expected.unsubscribe);
  REQUIRE(handler.setValueCalls.size() == expected.setValue);
}

inline void CheckNoClientCalls(const net::MockClientMessageHandler& handler) {
  CheckClientMessageCounts(handler);
}

struct ServerMessageCounts {
  size_t announce = 0;
  size_t unannounce = 0;
  size_t propertiesUpdate = 0;
  size_t setValue = 0;
};

inline void CheckServerMessageCounts(
    const net::MockServerMessageHandler& handler,
    const ServerMessageCounts& expected = {}) {
  REQUIRE(handler.announceCalls.size() == expected.announce);
  REQUIRE(handler.unannounceCalls.size() == expected.unannounce);
  REQUIRE(handler.propertiesUpdateCalls.size() == expected.propertiesUpdate);
  REQUIRE(handler.setValueCalls.size() == expected.setValue);
}

inline void CheckNoServerCalls(const net::MockServerMessageHandler& handler) {
  CheckServerMessageCounts(handler);
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
  CheckClientMessageCounts(network, {
                                        .publish = publishCount,
                                        .unpublish = unpublishCount,
                                        .setProperties = setPropertiesCount,
                                        .subscribe = subscribeCount,
                                        .unsubscribe = unsubscribeCount,
                                        .setValue = setValueCount,
                                    });
}

}  // namespace wpi::nt
