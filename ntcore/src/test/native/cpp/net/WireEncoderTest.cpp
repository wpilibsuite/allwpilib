// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/WireEncoder.hpp"

#include <stdint.h>

#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../TestPrinters.hpp"
#include "Handle.hpp"
#include "PubSubOptions.hpp"
#include "net/Message.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace std::string_view_literals;

namespace wpi::nt {
namespace {

std::span<const uint8_t> operator""_us(const char* str, size_t len) {
  return {reinterpret_cast<const uint8_t*>(str), len};
}

bool SpanEquals(std::span<const uint8_t> actual,
                std::span<const uint8_t> expected) {
  return actual.size() == expected.size() &&
         std::equal(actual.begin(), actual.end(), expected.begin());
}

}  // namespace

class WireEncoderTextTest {
 protected:
  std::string out;
  wpi::util::raw_string_ostream os{out};
  wpi::util::json GetJson() {
    return wpi::util::json::parse(os.str()).value_or(wpi::util::json::object());
  }
};

class WireEncoderBinaryTest {
 protected:
  std::vector<uint8_t> out;
  wpi::util::raw_uvector_ostream os{out};
};

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest PublishPropsEmpty",
                 "[ntcore][wire][encoder]") {
  net::WireEncodePublish(os, 5, "test", "double", wpi::util::json::object());
  REQUIRE(os.str() ==
          "{\"method\":\"publish\",\"params\":{"
          "\"name\":\"test\",\"properties\":{},\"pubuid\":5,"
          "\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest PublishProps",
                 "[ntcore][wire][encoder]") {
  net::WireEncodePublish(os, 5, "test", "double",
                         wpi::util::json::object("k", 6));
  REQUIRE(os.str() ==
          "{\"method\":\"publish\",\"params\":{"
          "\"name\":\"test\",\"properties\":{\"k\":6},"
          "\"pubuid\":5,\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest Unpublish",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeUnpublish(os, 5);
  REQUIRE(os.str() == "{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest SetProperties",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeSetProperties(os, "test", wpi::util::json::object("k", 6));
  REQUIRE(os.str() ==
          "{\"method\":\"setproperties\",\"params\":{"
          "\"name\":\"test\",\"update\":{\"k\":6}}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest Subscribe",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           PubSubOptions{});
  REQUIRE(os.str() ==
          "{\"method\":\"subscribe\",\"params\":{"
          "\"options\":{},\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest SubscribeSendAll",
                 "[ntcore][wire][encoder]") {
  PubSubOptionsImpl options;
  options.sendAll = true;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  REQUIRE(os.str() ==
          "{\"method\":\"subscribe\",\"params\":{"
          "\"options\":{\"all\":true},\"topics\":[\"a\",\"b\"],"
          "\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest SubscribePeriodic",
                 "[ntcore][wire][encoder]") {
  PubSubOptionsImpl options;
  options.periodicMs = 500u;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  REQUIRE(os.str() ==
          "{\"method\":\"subscribe\",\"params\":{"
          "\"options\":{\"periodic\":0.5},\"topics\":[\"a\",\"b\"],"
          "\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest SubscribeAllOptions",
                 "[ntcore][wire][encoder]") {
  PubSubOptionsImpl options;
  options.sendAll = true;
  options.periodicMs = 500u;
  net::WireEncodeSubscribe(os, 5, std::span<const std::string_view>{{"a", "b"}},
                           options);
  REQUIRE(os.str() ==
          "{\"method\":\"subscribe\",\"params\":{"
          "\"options\":{\"all\":true,\"periodic\":0.5},"
          "\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest Unsubscribe",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeUnsubscribe(os, 5);
  REQUIRE(os.str() == "{\"method\":\"unsubscribe\",\"params\":{\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest Announce",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeAnnounce(os, "test", 5, "double", wpi::util::json::object(),
                          std::nullopt);
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{},\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest AnnounceProperties",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeAnnounce(os, "test", 5, "double",
                          wpi::util::json::object("k", 6), std::nullopt);
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{\"k\":6},\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest AnnouncePubuid",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeAnnounce(os, "test", 5, "double", wpi::util::json::object(),
                          6);
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{},\"pubuid\":6,\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest Unannounce",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeUnannounce(os, "test", 5);
  REQUIRE(
      os.str() ==
      "{\"method\":\"unannounce\",\"params\":{\"id\":5,\"name\":\"test\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessagePublish",
                 "[ntcore][wire][encoder]") {
  net::ClientMessage msg{net::PublishMsg{
      5, "test", "double", wpi::util::json::object("k", 6), {}}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"publish\",\"params\":{"
          "\"name\":\"test\",\"properties\":{\"k\":6},"
          "\"pubuid\":5,\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessageUnpublish",
                 "[ntcore][wire][encoder]") {
  net::ClientMessage msg{net::UnpublishMsg{5}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() == "{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest,
                 "WireEncoderTextTest MessageSetProperties",
                 "[ntcore][wire][encoder]") {
  net::ClientMessage msg{
      net::SetPropertiesMsg{"test", wpi::util::json::object("k", 6)}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"setproperties\",\"params\":{"
          "\"name\":\"test\",\"update\":{\"k\":6}}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessageSubscribe",
                 "[ntcore][wire][encoder]") {
  net::ClientMessage msg{net::SubscribeMsg{5, {"a", "b"}, {}}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"subscribe\",\"params\":{"
          "\"options\":{},\"topics\":[\"a\",\"b\"],\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessageUnsubscribe",
                 "[ntcore][wire][encoder]") {
  net::ClientMessage msg{net::UnsubscribeMsg{5}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() == "{\"method\":\"unsubscribe\",\"params\":{\"subuid\":5}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessageAnnounce",
                 "[ntcore][wire][encoder]") {
  net::ServerMessage msg{net::AnnounceMsg{"test", 5, "double", std::nullopt,
                                          wpi::util::json::object()}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{},\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest,
                 "WireEncoderTextTest MessageAnnounceProperties",
                 "[ntcore][wire][encoder]") {
  net::ServerMessage msg{net::AnnounceMsg{"test", 5, "double", std::nullopt,
                                          wpi::util::json::object("k", 6)}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{\"k\":6},\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest,
                 "WireEncoderTextTest MessageAnnouncePubuid",
                 "[ntcore][wire][encoder]") {
  net::ServerMessage msg{
      net::AnnounceMsg{"test", 5, "double", 6, wpi::util::json::object()}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(os.str() ==
          "{\"method\":\"announce\",\"params\":{\"id\":5,\"name\":\"test\","
          "\"properties\":{},\"pubuid\":6,\"type\":\"double\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest MessageUnannounce",
                 "[ntcore][wire][encoder]") {
  net::ServerMessage msg{net::UnannounceMsg{"test", 5}};
  REQUIRE(net::WireEncodeText(os, msg));
  REQUIRE(
      os.str() ==
      "{\"method\":\"unannounce\",\"params\":{\"id\":5,\"name\":\"test\"}}");
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest ServerMessageEmpty",
                 "[ntcore][wire][encoder]") {
  REQUIRE_FALSE(net::WireEncodeText(os, net::ServerMessage{}));
}

TEST_CASE_METHOD(WireEncoderTextTest, "WireEncoderTextTest ServerMessageValue",
                 "[ntcore][wire][encoder]") {
  net::ServerMessage msg{net::ServerValueMsg{}};
  REQUIRE_FALSE(net::WireEncodeText(os, msg));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest Boolean",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeBoolean(true));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x00\xc3"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest Integer",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeInteger(7));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x02\x07"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest Float",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeFloat(2.5));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x03\xca\x40\x20\x00\x00"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest Double",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeDouble(2.5));
  REQUIRE(SpanEquals(
      out, "\x94\x05\x06\x01\xcb\x40\x04\x00\x00\x00\x00\x00\x00"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest String",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeString("hello"));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x04\xa5hello"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest Raw",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeRaw("hello"_us));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x05\xc4\x05hello"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest BooleanArray",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeBooleanArray({true, false, true}));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x10\x93\xc3\xc2\xc3"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest IntegerArray",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeIntegerArray({1, 2, 4}));
  REQUIRE(SpanEquals(out, "\x94\x05\x06\x12\x93\x01\x02\x04"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest FloatArray",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeFloatArray({1, 2, 3}));
  REQUIRE(SpanEquals(out,
                     "\x94\x05\x06\x13\x93"
                     "\xca\x3f\x80\x00\x00"
                     "\xca\x40\x00\x00\x00"
                     "\xca\x40\x40\x00\x00"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest DoubleArray",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeDoubleArray({1, 2, 3}));
  REQUIRE(SpanEquals(out,
                     "\x94\x05\x06\x11\x93"
                     "\xcb\x3f\xf0\x00\x00\x00\x00\x00\x00"
                     "\xcb\x40\x00\x00\x00\x00\x00\x00\x00"
                     "\xcb\x40\x08\x00\x00\x00\x00\x00\x00"_us));
}

TEST_CASE_METHOD(WireEncoderBinaryTest, "WireEncoderBinaryTest StringArray",
                 "[ntcore][wire][encoder]") {
  net::WireEncodeBinary(os, 5, 6, Value::MakeStringArray({"hello", "bye"}));
  REQUIRE(SpanEquals(out,
                     "\x94\x05\x06\x14\x92\xa5hello\xa3"
                     "bye"_us));
}

}  // namespace wpi::nt
