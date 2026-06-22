// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "server/ServerImpl.hpp"

#include <stddef.h>
#include <stdint.h>

#include <concepts>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../MockAssertions.hpp"
#include "../MockLogger.hpp"
#include "../TestPrinters.hpp"
#include "../net/MockClientMessageQueue.hpp"
#include "../net/MockMessageHandler.hpp"
#include "../net/MockWireConnection.hpp"
#include "Handle.hpp"
#include "net/Message.hpp"
#include "net/WireEncoder.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"

namespace wpi::nt {
namespace {

class SetPeriodicRecorder {
 public:
  std::function<void(uint32_t)> AsStdFunction() {
    return [this](uint32_t repeatMs) { calls.emplace_back(repeatMs); };
  }

  std::vector<uint32_t> calls;
};

struct WireConnectionCounts {
  size_t sendPing = 0;
  size_t ready = 0;
  size_t writeText = 0;
  size_t writeBinary = 0;
  size_t flush = 0;
  size_t lastReceivedTime = 0;
  size_t sendText = 0;
  size_t sendBinary = 0;
  size_t disconnect = 0;
};

void CheckWireConnectionCounts(const net::MockWireConnection& wire,
                               const WireConnectionCounts& expected = {}) {
  REQUIRE(wire.sendPingCalls.size() == expected.sendPing);
  REQUIRE(static_cast<size_t>(wire.readyCalls) == expected.ready);
  REQUIRE(wire.writeTextCalls.size() == expected.writeText);
  REQUIRE(wire.writeBinaryCalls.size() == expected.writeBinary);
  REQUIRE(static_cast<size_t>(wire.flushCalls) == expected.flush);
  REQUIRE(static_cast<size_t>(wire.lastReceivedTimeCalls) ==
          expected.lastReceivedTime);
  REQUIRE(wire.sendTextCalls.size() == expected.sendText);
  REQUIRE(wire.sendBinaryCalls.size() == expected.sendBinary);
  REQUIRE(wire.disconnectCalls.size() == expected.disconnect);
}

template <typename... Expected, typename... Calls>
void CheckCallOrder(const std::vector<std::variant<Calls...>>& calls) {
  REQUIRE(calls.size() == sizeof...(Expected));
  size_t index = 0;
  (
      [&] {
        CHECK(std::holds_alternative<Expected>(calls[index]));
        ++index;
      }(),
      ...);
}

}  // namespace

class ServerImplTest {
 public:
  net::MockServerMessageHandler local;
  net::MockClientMessageQueue queue;
  wpi::MockLogger logger;
  server::ServerImpl server{logger};
};

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest AddClient",
                 "[ntcore][server]") {
  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());
  CHECK(name == "test@1");
  CHECK(id != -1);
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest AddDuplicateClient",
                 "[ntcore][server]") {
  net::MockWireConnection wire1;
  net::MockWireConnection wire2;
  SetPeriodicRecorder setPeriodic1;
  SetPeriodicRecorder setPeriodic2;

  auto [name1, id1] = server.AddClient("test", "connInfo", false, wire1,
                                       setPeriodic1.AsStdFunction());
  auto [name2, id2] = server.AddClient("test", "connInfo2", false, wire2,
                                       setPeriodic2.AsStdFunction());
  CHECK(name1 == "test@1");
  CHECK(id1 != -1);
  CHECK(name2 == "test@2");
  CHECK(id1 != id2);
  CHECK(id2 != -1);
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest AddClient3",
                 "[ntcore][server]") {}

template <typename T>
static std::string EncodeText1(const T& msg) {
  std::string data;
  wpi::util::raw_string_ostream os{data};
  net::WireEncodeText(os, msg);
  return data;
}

template <typename T>
static std::string EncodeText(const T& msgs) {
  std::string data;
  wpi::util::raw_string_ostream os{data};
  bool first = true;
  for (auto&& msg : msgs) {
    if (first) {
      os << '[';
      first = false;
    } else {
      os << ',';
    }
    net::WireEncodeText(os, msg);
  }
  os << ']';
  return data;
}

template <typename T>
static std::vector<uint8_t> EncodeServerBinary1(const T& msg) {
  std::vector<uint8_t> data;
  wpi::util::raw_uvector_ostream os{data};
  if constexpr (std::same_as<T, net::ServerMessage>) {
    if (auto m = std::get_if<net::ServerValueMsg>(&msg.contents)) {
      net::WireEncodeBinary(os, m->topic, m->value.time(), m->value);
    }
  } else if constexpr (std::same_as<T, net::ClientMessage>) {
    if (auto m = std::get_if<net::ClientValueMsg>(&msg.contents)) {
      net::WireEncodeBinary(os, Handle{m->pubHandle}.GetIndex(),
                            m->value.time(), m->value);
    }
  }
  return data;
}

template <typename T>
static std::vector<uint8_t> EncodeServerBinary(const T& msgs) {
  std::vector<uint8_t> data;
  wpi::util::raw_uvector_ostream os{data};
  for (auto&& msg : msgs) {
    if constexpr (std::same_as<typename T::value_type, net::ServerMessage>) {
      if (auto m = std::get_if<net::ServerValueMsg>(&msg.contents)) {
        net::WireEncodeBinary(os, m->topic, m->value.time(), m->value);
      }
    } else if constexpr (std::same_as<typename T::value_type,
                                      net::ClientMessage>) {
      if (auto m = std::get_if<net::ClientValueMsg>(&msg.contents)) {
        net::WireEncodeBinary(os, m->pubuid, m->value.time(), m->value);
      }
    }
  }
  return data;
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest PublishLocal",
                 "[ntcore][server]") {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  constexpr int pubuid2 = 2;
  constexpr int pubuid3 = 3;

  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid, "test", "double", wpi::util::json::object(), {}}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  // client connect; it should get already-published topic as soon as it
  // subscribes
  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  {
    constexpr int subuid = 1;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{
        net::SubscribeMsg{subuid, {{""}}, PubSubOptions{.prefixMatch = true}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  // publish before send control
  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid2, "test2", "double", wpi::util::json::object(), {}}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  server.SendAllOutgoing(100, false);

  // publish after send control
  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid3, "test3", "double", wpi::util::json::object(), {}}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  server.SendAllOutgoing(200, false);

  CheckServerMessageCounts(local, {.announce = 3});
  CheckCallOrder<net::MockServerMessageHandler::AnnounceCall,
                 net::MockServerMessageHandler::AnnounceCall,
                 net::MockServerMessageHandler::AnnounceCall>(local.calls);
  CHECK(local.announceCalls[0].name == "test");
  CHECK(local.announceCalls[0].pubuid == std::optional<int>{pubuid});
  CHECK(local.announceCalls[1].name == "test2");
  CHECK(local.announceCalls[1].pubuid == std::optional<int>{pubuid2});
  CHECK(local.announceCalls[2].name == "test3");
  CHECK(local.announceCalls[2].pubuid == std::optional<int>{pubuid3});

  REQUIRE(setPeriodic.calls.size() == 1u);
  CHECK(setPeriodic.calls[0] == 100u);
  CheckWireConnectionCounts(wire, {.sendPing = 1,
                                   .ready = 2,
                                   .writeText = 3,
                                   .flush = 2,
                                   .lastReceivedTime = 1});
  CHECK(wire.sendPingCalls == std::vector<uint64_t>{100});
  CheckCallOrder<
      net::MockWireConnection::GetLastReceivedTimeCall,
      net::MockWireConnection::SendPingCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteTextCall,
      net::MockWireConnection::WriteTextCall,
      net::MockWireConnection::FlushCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteTextCall,
      net::MockWireConnection::FlushCall>(wire.calls);
  CHECK(wire.writeTextCalls[0] ==
        EncodeText1(net::ServerMessage{net::AnnounceMsg{
            "test", 3, "double", std::nullopt, wpi::util::json::object()}}));
  CHECK(wire.writeTextCalls[1] ==
        EncodeText1(net::ServerMessage{net::AnnounceMsg{
            "test2", 8, "double", std::nullopt, wpi::util::json::object()}}));
  CHECK(wire.writeTextCalls[2] ==
        EncodeText1(net::ServerMessage{net::AnnounceMsg{
            "test3", 11, "double", std::nullopt, wpi::util::json::object()}}));
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest ClientSubTopicOnlyThenValue",
                 "[ntcore][server]") {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;

  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid, "test", "double", wpi::util::json::object(), {}}});
    queue.msgs.emplace_back(net::ClientMessage{
        net::ClientValueMsg{pubuid, Value::MakeDouble(1.0, 10)}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;

  // connect client
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // subscribe topics only; will not send value
  {
    constexpr int subuid = 1;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{net::SubscribeMsg{
        subuid,
        {{""}},
        PubSubOptions{.topicsOnly = true, .prefixMatch = true}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  server.SendOutgoing(id, 100);

  // subscribe normal; will not resend announcement, but will send value
  {
    constexpr int subuid = 2;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{
        net::SubscribeMsg{subuid, {{"test"}}, PubSubOptions{}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  server.SendOutgoing(id, 200);

  CheckServerMessageCounts(local, {.announce = 1});
  CheckCallOrder<net::MockServerMessageHandler::AnnounceCall>(local.calls);
  CHECK(local.announceCalls[0].name == "test");
  CHECK(local.announceCalls[0].pubuid == std::optional<int>{pubuid});
  CHECK(setPeriodic.calls == std::vector<uint32_t>{100, 100});
  CheckWireConnectionCounts(wire, {.sendPing = 1,
                                   .ready = 2,
                                   .writeText = 1,
                                   .writeBinary = 1,
                                   .flush = 2,
                                   .lastReceivedTime = 1});
  CHECK(wire.sendPingCalls == std::vector<uint64_t>{100});
  CheckCallOrder<
      net::MockWireConnection::GetLastReceivedTimeCall,
      net::MockWireConnection::SendPingCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteTextCall,
      net::MockWireConnection::FlushCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteBinaryCall,
      net::MockWireConnection::FlushCall>(wire.calls);
  CHECK(wire.writeTextCalls[0] ==
        EncodeText1(net::ServerMessage{net::AnnounceMsg{
            "test", 3, "double", std::nullopt, wpi::util::json::object()}}));
  CHECK(wire.writeBinaryCalls[0] ==
        EncodeServerBinary1(net::ServerMessage{
            net::ServerValueMsg{3, Value::MakeDouble(1.0, 10)}}));
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest ClientDisconnectUnpublish",
                 "[ntcore][server]") {
  server.SetLocal(&local, &queue);
  constexpr int pubuidLocal = 1;
  constexpr int subuid = 1;

  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuidLocal, "test2", "double", wpi::util::json::object(), {}}});
    queue.msgs.emplace_back(net::ClientMessage{
        net::ClientValueMsg{pubuidLocal, Value::MakeDouble(1.0, 10)}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  {
    queue.msgs.emplace_back(
        net::ClientMessage{net::SubscribeMsg{subuid, {"test"}, {}}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;

  // connect client
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // publish topic
  {
    constexpr int pubuid = 1;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid, "test", "double", wpi::util::json::object(), {}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  server.SendOutgoing(id, 100);

  // disconnect client
  server.RemoveClient(id);

  CheckServerMessageCounts(local, {.announce = 2, .unannounce = 1});
  CheckCallOrder<net::MockServerMessageHandler::AnnounceCall,
                 net::MockServerMessageHandler::AnnounceCall,
                 net::MockServerMessageHandler::UnannounceCall>(local.calls);
  CHECK(local.announceCalls[0].name == "test2");
  CHECK(local.announceCalls[0].pubuid == std::optional<int>{pubuidLocal});
  CHECK(local.announceCalls[1].name == "test");
  CHECK(local.announceCalls[1].pubuid == std::optional<int>{});
  CHECK(local.unannounceCalls[0].name == "test");
  CHECK(local.unannounceCalls[0].id == 0);
  CheckWireConnectionCounts(wire, {.sendPing = 1,
                                   .ready = 1,
                                   .writeText = 1,
                                   .flush = 1,
                                   .lastReceivedTime = 1});
  CHECK(wire.sendPingCalls == std::vector<uint64_t>{100});
  CheckCallOrder<net::MockWireConnection::GetLastReceivedTimeCall,
                 net::MockWireConnection::SendPingCall,
                 net::MockWireConnection::ReadyCall,
                 net::MockWireConnection::WriteTextCall,
                 net::MockWireConnection::FlushCall>(wire.calls);
  CHECK(wire.writeTextCalls[0] ==
        EncodeText1(net::ServerMessage{net::AnnounceMsg{
            "test", 8, "double", 1, wpi::util::json::object()}}));
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest ZeroTimestampNegativeTime",
                 "[ntcore][server]") {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  NT_Topic topicHandle = wpi::nt::Handle{0, 1, wpi::nt::Handle::TOPIC};
  constexpr int subuid = 1;
  Value defaultValue = Value::MakeDouble(1.0, 10);
  defaultValue.SetTime(0);
  defaultValue.SetServerTime(0);
  Value value = Value::MakeDouble(5, -10);
  local.announceReturns.emplace_back(topicHandle);

  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid, "test", "double", wpi::util::json::object(), {}}});
    queue.msgs.emplace_back(
        net::ClientMessage{net::ClientValueMsg{pubuid, defaultValue}});
    queue.msgs.emplace_back(
        net::ClientMessage{net::SubscribeMsg{subuid, {"test"}, {}}});
    CHECK_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  // client connect; it should get already-published topic as soon as it
  // subscribes
  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // publish and send non-default value with negative time offset
  {
    constexpr int pubuid2 = 2;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuid2, "test", "double", wpi::util::json::object(), {}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
    msgs.clear();
    msgs.emplace_back(net::ClientMessage{net::ClientValueMsg{pubuid2, value}});
    server.ProcessIncomingBinary(id, EncodeServerBinary(msgs));
  }

  CheckServerMessageCounts(local, {.announce = 1, .setValue = 2});
  CheckCallOrder<net::MockServerMessageHandler::AnnounceCall,
                 net::MockServerMessageHandler::SetValueCall,
                 net::MockServerMessageHandler::SetValueCall>(local.calls);
  CHECK(local.announceCalls[0].name == "test");
  CHECK(local.announceCalls[0].pubuid == std::optional<int>{pubuid});
  CHECK(local.setValueCalls[0].topicuid == topicHandle);
  CHECK(local.setValueCalls[0].value == defaultValue);
  CHECK(local.setValueCalls[1].topicuid == topicHandle);
  CHECK(local.setValueCalls[1].value == value);
}

// When a client re-subscribes with a shorter period for a topic it already
// matched, UpdatePeriod must be called so the outgoing queue period is updated.
//
// Bug: the original code used `added ^ removed`. When a subscription update
// matches a topic that was already subscribed (added=true AND removed=true),
// XOR evaluates to false and UpdatePeriod is never called. The topic stays in
// the old outgoing queue (longer period) even though the client asked for a
// shorter one.
//
// Fix: change to `added || removed`. Now the period is recomputed whenever
// anything changed, including the both-true re-subscribe case.
//
// Test strategy: subscribe with period=200ms, drain the queue (nextSendMs now
// 200), then re-subscribe with period=100ms. The re-subscribe causes the last
// value to be re-queued (wasSubscribed is false after erasing the old sub).
// A send at t=150 is between the two periods:
//   With fix:  SetPeriod(100) called → 100ms queue (nextSendMs=0) fires →
//              DoWriteBinary called once.
//   With bug:  SetPeriod not called → value stays in 200ms queue
//              (nextSendMs=200) → DoWriteBinary NOT called at t=150.
TEST_CASE_METHOD(ServerImplTest,
                 "ServerImplTest ResubscribeShorterPeriodUpdatesTopicOutgoing",
                 "[ntcore][server]") {
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  constexpr int subuid = 1;

  // Publish topic and initial value so there is a lastValue when the client
  // subscribes.
  queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
      pubuid, "test", "double", wpi::util::json::object(), {}}});
  queue.msgs.emplace_back(net::ClientMessage{
      net::ClientValueMsg{pubuid, Value::MakeDouble(1.0, 50)}});
  REQUIRE_FALSE(server.ProcessLocalMessages(UINT_MAX));

  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;

  int binaryCallCount = 0;
  wire.onWriteBinary = [&](std::span<const uint8_t>) {
    ++binaryCallCount;
    return 0;
  };

  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // Subscribe with period=200ms; initial value 1.0 is queued in the 200ms
  // queue via the last-value send path.
  {
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{net::SubscribeMsg{
        subuid, {{""}}, PubSubOptions{.periodic = 0.2, .prefixMatch = true}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  // First send at t=100: drains the initial 200ms queue (nextSendMs=0 fires),
  // sends announce + initial value 1.0. After this nextSendMs[200ms]=200.
  server.SendOutgoing(id, 100);

  // Re-subscribe same subuid with period=100ms. Because the old subscriber is
  // erased first, wasSubscribed becomes false and the server re-queues the last
  // value (1.0). Where it lands depends on which queue the topic is mapped to:
  //   With fix: SetPeriod(topicId, 100) → topic maps to 100ms queue
  //             (nextSendMs=0) → value queued there.
  //   With bug: SetPeriod not called → topic still maps to 200ms queue
  //             (nextSendMs=200) → value queued there.
  {
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{net::SubscribeMsg{
        subuid, {{""}}, PubSubOptions{.periodic = 0.1, .prefixMatch = true}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  // At t=150 (between 100ms and 200ms):
  //   With fix:  100ms queue (nextSendMs=0) fires → 1 binary write.
  //   With bug:  200ms queue (nextSendMs=200) skips → 0 binary writes.
  binaryCallCount = 0;
  server.SendOutgoing(id, 150);

  UNSCOPED_INFO(
      "Re-subscribing with a shorter period must update the topic's outgoing "
      "queue period; the re-queued last value should be sent by t=150ms (XOR "
      "bug: UpdatePeriod is skipped when added && removed are both true, "
      "leaving the topic mapped to the 200ms queue)");
  CHECK(binaryCallCount == 1);
  CheckWireConnectionCounts(wire, {.sendPing = 1,
                                   .ready = 2,
                                   .writeText = 1,
                                   .writeBinary = 2,
                                   .flush = 2,
                                   .lastReceivedTime = 1});
  CheckCallOrder<
      net::MockWireConnection::GetLastReceivedTimeCall,
      net::MockWireConnection::SendPingCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteTextCall,
      net::MockWireConnection::WriteBinaryCall,
      net::MockWireConnection::FlushCall, net::MockWireConnection::ReadyCall,
      net::MockWireConnection::WriteBinaryCall,
      net::MockWireConnection::FlushCall>(wire.calls);
}

TEST_CASE_METHOD(ServerImplTest, "ServerImplTest InvalidPubUid",
                 "[ntcore][server]") {
  server.SetLocal(&local, &queue);

  // connect client
  net::MockWireConnection wire;
  SetPeriodicRecorder setPeriodic;
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  server.ProcessIncomingText(
      id,
      "[{\"method\":\"publish\",\"params\":{\"type\":\"string\",\"name\":"
      "\"myvalue\",\"pubuid\":2147483647,\"properties\":{}}}]");
  logger.CheckMessage(NT_LOG_WARNING, "0: pubuid out of range");
  CheckNoServerCalls(local);
}

}  // namespace wpi::nt
