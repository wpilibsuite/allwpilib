// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <stdint.h>

#include <concepts>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>

#include "../MockLogger.h"
#include "../PubSubOptionsMatcher.h"
#include "../TestPrinters.h"
#include "../ValueMatcher.h"
#include "Handle.h"
#include "MockClientMessageQueue.h"
#include "MockMessageHandler.h"
#include "MockWireConnection.h"
#include "gmock/gmock.h"
#include "net/Message.h"
#include "net/ServerImpl.h"
#include "net/WireEncoder.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;

using MockSetPeriodicFunc = ::testing::MockFunction<void(uint32_t repeatMs)>;
using MockConnected3Func =
    ::testing::MockFunction<void(std::string_view name, uint16_t proto)>;

namespace nt {

class ServerImplTest : public ::testing::Test {
 public:
  ::testing::StrictMock<net::MockServerMessageHandler> local;
  ::testing::StrictMock<net::MockClientMessageQueue> queue;
  wpi::MockLogger logger;
  net::ServerImpl server{logger};
};

TEST_F(ServerImplTest, AddClient) {
  ::testing::StrictMock<net::MockWireConnection> wire;
  // EXPECT_CALL(wire, Flush());
  MockSetPeriodicFunc setPeriodic;
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());
  EXPECT_EQ(name, "test@1");
  EXPECT_NE(id, -1);
}

TEST_F(ServerImplTest, AddDuplicateClient) {
  ::testing::StrictMock<net::MockWireConnection> wire1;
  ::testing::StrictMock<net::MockWireConnection> wire2;
  MockSetPeriodicFunc setPeriodic1;
  MockSetPeriodicFunc setPeriodic2;
  // EXPECT_CALL(wire1, Flush());
  // EXPECT_CALL(wire2, Flush());

  auto [name1, id1] = server.AddClient("test", "connInfo", false, wire1,
                                       setPeriodic1.AsStdFunction());
  auto [name2, id2] = server.AddClient("test", "connInfo2", false, wire2,
                                       setPeriodic2.AsStdFunction());
  EXPECT_EQ(name1, "test@1");
  EXPECT_NE(id1, -1);
  EXPECT_EQ(name2, "test@2");
  EXPECT_NE(id1, id2);
  EXPECT_NE(id2, -1);
}

TEST_F(ServerImplTest, AddClient3) {}

template <typename T>
static std::string EncodeText1(const T& msg) {
  std::string data;
  wpi::raw_string_ostream os{data};
  net::WireEncodeText(os, msg);
  return data;
}

template <typename T>
static std::string EncodeText(const T& msgs) {
  std::string data;
  wpi::raw_string_ostream os{data};
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
  wpi::raw_uvector_ostream os{data};
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
  wpi::raw_uvector_ostream os{data};
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

TEST_F(ServerImplTest, PublishLocal) {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  constexpr int pubuid2 = 2;
  constexpr int pubuid3 = 3;
  {
    ::testing::InSequence seq;
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{pubuid}));
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test2"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{pubuid2}));
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test3"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{pubuid3}));
  }

  {
    queue.msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid, "test", "double", wpi::json::object(), {}}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  // client connect; it should get already-published topic as soon as it
  // subscribes
  ::testing::StrictMock<net::MockWireConnection> wire;
  MockSetPeriodicFunc setPeriodic;
  EXPECT_CALL(wire, GetVersion()).WillRepeatedly(Return(0x0401));
  {
    ::testing::InSequence seq;
    // EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // AddClient()
    EXPECT_CALL(setPeriodic, Call(100));  // ClientSubscribe()
    // EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // ClientSubscribe()
    EXPECT_CALL(wire, GetLastReceivedTime()).WillOnce(Return(0));
    EXPECT_CALL(wire, SendPing(100));
    EXPECT_CALL(wire, Ready()).WillOnce(Return(true));  // SendControl()
    EXPECT_CALL(
        wire, DoWriteText(StrEq(EncodeText1(net::ServerMessage{net::AnnounceMsg{
                  "test", 3, "double", std::nullopt, wpi::json::object()}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(
        wire, DoWriteText(StrEq(EncodeText1(net::ServerMessage{net::AnnounceMsg{
                  "test2", 8, "double", std::nullopt, wpi::json::object()}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // SendControl()
    EXPECT_CALL(wire, Ready()).WillOnce(Return(true));  // SendControl()
    EXPECT_CALL(
        wire, DoWriteText(StrEq(EncodeText1(net::ServerMessage{net::AnnounceMsg{
                  "test3", 11, "double", std::nullopt, wpi::json::object()}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(wire, Flush()).WillOnce(Return(0));  // SendControl()
  }
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
    queue.msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid2, "test2", "double", wpi::json::object(), {}}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  server.SendAllOutgoing(100, false);

  // publish after send control
  {
    queue.msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid3, "test3", "double", wpi::json::object(), {}}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  server.SendAllOutgoing(200, false);
}

TEST_F(ServerImplTest, ClientSubTopicOnlyThenValue) {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  EXPECT_CALL(
      local,
      ServerAnnounce(std::string_view{"test"}, 0, std::string_view{"double"},
                     wpi::json::object(), std::optional<int>{pubuid}));

  {
    queue.msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid, "test", "double", wpi::json::object(), {}}});
    queue.msgs.emplace_back(net::ClientMessage{
        net::ClientValueMsg{pubuid, Value::MakeDouble(1.0, 10)}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  ::testing::StrictMock<net::MockWireConnection> wire;
  EXPECT_CALL(wire, GetVersion()).WillRepeatedly(Return(0x0401));
  MockSetPeriodicFunc setPeriodic;
  {
    ::testing::InSequence seq;
    // EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // AddClient()
    EXPECT_CALL(setPeriodic, Call(100));  // ClientSubscribe()
    // EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // ClientSubscribe()
    EXPECT_CALL(wire, GetLastReceivedTime()).WillOnce(Return(0));
    EXPECT_CALL(wire, SendPing(100));
    EXPECT_CALL(wire, Ready()).WillOnce(Return(true));  // SendValues()
    EXPECT_CALL(
        wire, DoWriteText(StrEq(EncodeText1(net::ServerMessage{net::AnnounceMsg{
                  "test", 3, "double", std::nullopt, wpi::json::object()}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(wire, Flush()).WillOnce(Return(0));  // SendValues()
    EXPECT_CALL(setPeriodic, Call(100));             // ClientSubscribe()
    // EXPECT_CALL(wire, Flush()).WillOnce(Return(0));     // ClientSubscribe()
    EXPECT_CALL(wire, Ready()).WillOnce(Return(true));  // SendValues()
    EXPECT_CALL(
        wire, DoWriteBinary(wpi::SpanEq(EncodeServerBinary1(net::ServerMessage{
                  net::ServerValueMsg{3, Value::MakeDouble(1.0, 10)}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(wire, Flush());  // SendValues()
  }

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
}

TEST_F(ServerImplTest, ClientDisconnectUnpublish) {
  server.SetLocal(&local, &queue);
  constexpr int pubuidLocal = 1;
  constexpr int subuid = 1;
  {
    ::testing::InSequence seq;
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test2"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{pubuidLocal}));
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{}));
    EXPECT_CALL(local, ServerUnannounce(std::string_view{"test"}, 0));
  }

  {
    queue.msgs.emplace_back(net::ClientMessage{net::PublishMsg{
        pubuidLocal, "test2", "double", wpi::json::object(), {}}});
    queue.msgs.emplace_back(net::ClientMessage{
        net::ClientValueMsg{pubuidLocal, Value::MakeDouble(1.0, 10)}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  {
    queue.msgs.emplace_back(
        net::ClientMessage{net::SubscribeMsg{subuid, {"test"}, {}}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  ::testing::StrictMock<net::MockWireConnection> wire;
  EXPECT_CALL(wire, GetVersion()).WillRepeatedly(Return(0x0401));
  MockSetPeriodicFunc setPeriodic;
  {
    ::testing::InSequence seq;
    EXPECT_CALL(wire, GetLastReceivedTime()).WillOnce(Return(0));
    EXPECT_CALL(wire, SendPing(100));
    EXPECT_CALL(wire, Ready()).WillOnce(Return(true));  // SendValues()
    EXPECT_CALL(
        wire, DoWriteText(StrEq(EncodeText1(net::ServerMessage{net::AnnounceMsg{
                  "test", 8, "double", 1, wpi::json::object()}}))))
        .WillOnce(Return(0));
    EXPECT_CALL(wire, Flush());  // SendValues()
  }

  // connect client
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // publish topic
  {
    constexpr int pubuid = 1;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid, "test", "double", wpi::json::object(), {}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
  }

  server.SendOutgoing(id, 100);

  // disconnect client
  server.RemoveClient(id);
}

TEST_F(ServerImplTest, ZeroTimestampNegativeTime) {
  // publish before client connect
  server.SetLocal(&local, &queue);
  constexpr int pubuid = 1;
  NT_Topic topicHandle = nt::Handle{0, 1, nt::Handle::kTopic};
  constexpr int subuid = 1;
  Value defaultValue = Value::MakeDouble(1.0, 10);
  defaultValue.SetTime(0);
  defaultValue.SetServerTime(0);
  Value value = Value::MakeDouble(5, -10);
  {
    ::testing::InSequence seq;
    EXPECT_CALL(
        local,
        ServerAnnounce(std::string_view{"test"}, 0, std::string_view{"double"},
                       wpi::json::object(), std::optional<int>{pubuid}))
        .WillOnce(Return(topicHandle));
    EXPECT_CALL(local, ServerSetValue(topicHandle, defaultValue));
    EXPECT_CALL(local, ServerSetValue(topicHandle, value));
  }

  {
    queue.msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid, "test", "double", wpi::json::object(), {}}});
    queue.msgs.emplace_back(
        net::ClientMessage{net::ClientValueMsg{pubuid, defaultValue}});
    queue.msgs.emplace_back(
        net::ClientMessage{net::SubscribeMsg{subuid, {"test"}, {}}});
    EXPECT_FALSE(server.ProcessLocalMessages(UINT_MAX));
  }

  // client connect; it should get already-published topic as soon as it
  // subscribes
  ::testing::StrictMock<net::MockWireConnection> wire;
  MockSetPeriodicFunc setPeriodic;
  {
    ::testing::InSequence seq;
    // EXPECT_CALL(wire, Flush());  // AddClient()
  }
  auto [name, id] = server.AddClient("test", "connInfo", false, wire,
                                     setPeriodic.AsStdFunction());

  // publish and send non-default value with negative time offset
  {
    constexpr int pubuid2 = 2;
    std::vector<net::ClientMessage> msgs;
    msgs.emplace_back(net::ClientMessage{
        net::PublishMsg{pubuid2, "test", "double", wpi::json::object(), {}}});
    server.ProcessIncomingText(id, EncodeText(msgs));
    msgs.clear();
    msgs.emplace_back(net::ClientMessage{net::ClientValueMsg{pubuid2, value}});
    server.ProcessIncomingBinary(id, EncodeServerBinary(msgs));
  }
}

}  // namespace nt
