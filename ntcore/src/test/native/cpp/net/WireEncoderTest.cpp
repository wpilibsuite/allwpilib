// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>
#include <wpi/json.h>
#include <wpi/raw_ostream.h>

#include "../TestPrinters.h"
#include "Handle.h"
#include "PubSubOptions.h"
#include "gmock/gmock-matchers.h"
#include "net/Message.h"
#include "net/WireEncoder.h"
#include "networktables/NetworkTableValue.h"

using namespace std::string_view_literals;

namespace nt {

class WireEncoderTextTest : public ::testing::Test {
 protected:
  std::string out;
  wpi::raw_string_ostream os{out};
  wpi::json GetJson() { return wpi::json::parse(os.str()); }
};

class WireEncoderBinaryTest : public ::testing::Test {
 protected:
  std::vector<uint8_t> out;
  wpi::raw_uvector_ostream os{out};
};

TEST_F(WireEncoderTextTest, PublishPropsEmpty) {
  net::WireEncodePublish(os, 5, "test", "double", wpi::json::object());
  ASSERT_EQ(
      os.str(),
      "{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, PublishProps) {
  net::WireEncodePublish(os, 5, "test", "double", {{"k", 6}});
  ASSERT_EQ(os.str(),
            "{\"method\":\"publish\",\"params\":{"
            "\"name\":\"test\",\"properties\":{\"k\":6},"
            "\"pubuid\":5,\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, Unpublish) {
  net::WireEncodeUnpublish(os, 5);
  ASSERT_EQ(os.str(), "{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}");
}

TEST_F(WireEncoderTextTest, SetProperties) {
  net::WireEncodeSetProperties(os, "test", {{"k", 6}});
  ASSERT_EQ(os.str(),
            "{\"method\":\"setproperties\",\"params\":{"
            "\"name\":\"test\",\"update\":{\"k\":6}}}");
}

TEST_F(WireEncoderTextTest, Subscribe) {
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           PubSubOptions{});
  ASSERT_EQ(os.str(),
            "{\"method\":\"subscribe\",\"params\":{"
            "\"options\":{},\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, SubscribeSendAll) {
  PubSubOptionsImpl options;
  options.sendAll = true;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  ASSERT_EQ(os.str(),
            "{\"method\":\"subscribe\",\"params\":{"
            "\"options\":{\"all\":true},\"topics\":[\"a\",\"b\"],"
            "\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, SubscribePeriodic) {
  PubSubOptionsImpl options;
  options.periodicMs = 500u;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  ASSERT_EQ(os.str(),
            "{\"method\":\"subscribe\",\"params\":{"
            "\"options\":{\"periodic\":0.5},\"topics\":[\"a\",\"b\"],"
            "\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, SubscribeAllOptions) {
  PubSubOptionsImpl options;
  options.sendAll = true;
  options.periodicMs = 500u;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  ASSERT_EQ(os.str(),
            "{\"method\":\"subscribe\",\"params\":{"
            "\"options\":{\"all\":true,\"periodic\":0.5},"
            "\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, Unsubscribe) {
  net::WireEncodeUnsubscribe(os, 5);
  ASSERT_EQ(os.str(), "{\"method\":\"unsubscribe\",\"params\":{\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, Announce) {
  net::WireEncodeAnnounce(os, "test", 5, "double", wpi::json::object(),
                          std::nullopt);
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{},\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, AnnounceProperties) {
  net::WireEncodeAnnounce(os, "test", 5, "double", {{"k", 6}}, std::nullopt);
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{\"k\":6},\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, AnnouncePubuid) {
  net::WireEncodeAnnounce(os, "test", 5, "double", wpi::json::object(), 6);
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{},\"pubuid\":6,\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, Unannounce) {
  net::WireEncodeUnannounce(os, "test", 5);
  ASSERT_EQ(
      os.str(),
      "{\"method\":\"unannounce\",\"params\":{\"id\":5,\"name\":\"test\"}}");
}

TEST_F(WireEncoderTextTest, MessagePublish) {
  net::ClientMessage msg{net::PublishMsg{5, "test", "double", {{"k", 6}}, {}}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"publish\",\"params\":{"
            "\"name\":\"test\",\"properties\":{\"k\":6},"
            "\"pubuid\":5,\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, MessageUnpublish) {
  net::ClientMessage msg{net::UnpublishMsg{5}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(), "{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}");
}

TEST_F(WireEncoderTextTest, MessageSetProperties) {
  net::ClientMessage msg{net::SetPropertiesMsg{"test", {{"k", 6}}}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"setproperties\",\"params\":{"
            "\"name\":\"test\",\"update\":{\"k\":6}}}");
}

TEST_F(WireEncoderTextTest, MessageSubscribe) {
  net::ClientMessage msg{net::SubscribeMsg{5, {"a", "b"}, {}}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"subscribe\",\"params\":{"
            "\"options\":{},\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, MessageUnsubscribe) {
  net::ClientMessage msg{net::UnsubscribeMsg{5}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(), "{\"method\":\"unsubscribe\",\"params\":{\"subuid\":5}}");
}

TEST_F(WireEncoderTextTest, MessageAnnounce) {
  net::ServerMessage msg{
      net::AnnounceMsg{"test", 5, "double", std::nullopt, wpi::json::object()}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{},\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, MessageAnnounceProperties) {
  net::ServerMessage msg{
      net::AnnounceMsg{"test", 5, "double", std::nullopt, {{"k", 6}}}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{\"k\":6},\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, MessageAnnouncePubuid) {
  net::ServerMessage msg{
      net::AnnounceMsg{"test", 5, "double", 6, wpi::json::object()}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(os.str(),
            "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
            "\"properties\":{},\"pubuid\":6,\"type\":\"double\"}}");
}

TEST_F(WireEncoderTextTest, MessageUnannounce) {
  net::ServerMessage msg{net::UnannounceMsg{"test", 5}};
  ASSERT_TRUE(net::WireEncodeText(os, msg));
  ASSERT_EQ(
      os.str(),
      "{\"method\":\"unannounce\",\"params\":{\"id\":5,\"name\":\"test\"}}");
}

TEST_F(WireEncoderTextTest, ServerMessageEmpty) {
  ASSERT_FALSE(net::WireEncodeText(os, net::ServerMessage{}));
}

TEST_F(WireEncoderTextTest, ServerMessageValue) {
  net::ServerMessage msg{net::ServerValueMsg{}};
  ASSERT_FALSE(net::WireEncodeText(os, msg));
}

TEST_F(WireEncoderBinaryTest, Boolean) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeBoolean(true));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x00\xc3"_us));
}

TEST_F(WireEncoderBinaryTest, Integer) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeInteger(7));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x02\x07"_us));
}

TEST_F(WireEncoderBinaryTest, Float) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeFloat(2.5));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x03\xca\x40\x20\x00\x00"_us));
}

TEST_F(WireEncoderBinaryTest, Double) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeDouble(2.5));
  ASSERT_THAT(
      out,
      wpi::SpanEq("\x94\x05\x06\x01\xcb\x40\x04\x00\x00\x00\x00\x00\x00"_us));
}

TEST_F(WireEncoderBinaryTest, String) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeString("hello"));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x04\xa5hello"_us));
}

TEST_F(WireEncoderBinaryTest, Raw) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeRaw("hello"_us));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x05\xc4\x05hello"_us));
}

TEST_F(WireEncoderBinaryTest, BooleanArray) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeBooleanArray({true, false, true}));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x10\x93\xc3\xc2\xc3"_us));
}

TEST_F(WireEncoderBinaryTest, IntegerArray) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeIntegerArray({1, 2, 4}));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x12\x93\x01\x02\x04"_us));
}

TEST_F(WireEncoderBinaryTest, FloatArray) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeFloatArray({1, 2, 3}));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x13\x93"
                               "\xca\x3f\x80\x00\x00"
                               "\xca\x40\x00\x00\x00"
                               "\xca\x40\x40\x00\x00"_us));
}

TEST_F(WireEncoderBinaryTest, DoubleArray) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeDoubleArray({1, 2, 3}));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x11\x93"
                               "\xcb\x3f\xf0\x00\x00\x00\x00\x00\x00"
                               "\xcb\x40\x00\x00\x00\x00\x00\x00\x00"
                               "\xcb\x40\x08\x00\x00\x00\x00\x00\x00"_us));
}

TEST_F(WireEncoderBinaryTest, StringArray) {
  net::WireEncodeBinary(os, 5, 6, Value::MakeStringArray({"hello", "bye"}));
  ASSERT_THAT(out, wpi::SpanEq("\x94\x05\x06\x14\x92\xa5hello\xa3"
                               "bye"_us));
}

}  // namespace nt
