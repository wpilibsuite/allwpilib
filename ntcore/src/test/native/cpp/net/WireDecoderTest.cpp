// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/WireDecoder.hpp"

#include <limits>
#include <optional>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../MockAssertions.hpp"
#include "../MockLogger.hpp"
#include "../TestPrinters.hpp"
#include "MockMessageHandler.hpp"
#include "ProtocolVersions.hpp"
#include "PubSubOptions.hpp"
#include "net/MessageHandler.hpp"
#include "net/WireEncoder.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace std::string_view_literals;

namespace wpi::nt {

class WireDecodeTextClientTest {
 public:
  net::MockClientMessageHandler handler;
  wpi::MockLogger logger;
};

class WireDecodeTextServerTest {
 public:
  net::MockServerMessageHandler handler;
  wpi::MockLogger logger;
};

TEST_CASE("WireDecodeBinary rejects overflowing timestamp adjustments",
          "[ntcore][wire][decoder]") {
  auto check = [](int64_t timestamp, int64_t offset) {
    std::vector<uint8_t> encoded;
    wpi::util::raw_uvector_ostream os{encoded};
    net::WireEncodeBinary(os, 1, timestamp, Value::MakeInteger(1), NT_4_1);
    std::span<const uint8_t> input{encoded};
    int id;
    Value value;
    std::string error;

    CHECK_FALSE(
        net::WireDecodeBinary(&input, &id, &value, &error, offset, NT_4_1));
    CHECK(error == "timestamp out of range");
    CHECK(input.size() == encoded.size());
  };

  check((std::numeric_limits<int64_t>::max)(), 808);
  check((std::numeric_limits<int64_t>::min)(), -809);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest EmptyArray",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[]", handler, logger);
  logger.CheckMessages({});
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorEmpty",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING,
                      "could not decode JSON message: absent_value"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorBadJson1",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING,
                      "could not decode JSON message: unexpected_eof"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorBadJson2",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING,
                      "could not decode JSON message: unexpected_eof"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNotArray",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("{}", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "expected JSON array at top level"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorMessageNotObject",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[5]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: expected message to be an object"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNoMethodKey",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{}]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: no method key"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorMethodNotString",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":5}]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: method must be a string"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNoParamsKey",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\"}]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: no params key"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorParamsNotObject",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\",\"params\":5}]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: params must be an object"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorUnknownMethod",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\",\"params\":{}}]", handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: unrecognized method 'a'"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishPropsEmpty",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessages({});
  CheckClientMessageCounts(handler, {.publish = 1});
  CheckPublish(handler.publishCalls.back(), 5, "test", "double",
               wpi::util::json::object());

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessages({});
  CheckClientMessageCounts(handler, {.publish = 2});
  CheckPublish(handler.publishCalls.back(), 5, "test", "double",
               wpi::util::json::object());
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishProps",
                 "[ntcore][wire][decoder]") {
  auto props = wpi::util::json::object("k", 6);
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{\"k\":6},"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessages({});
  CheckClientMessageCounts(handler, {.publish = 1});
  CheckPublish(handler.publishCalls[0], 5, "test", "double", props);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishPropsError",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":[\"k\"],"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: properties must be an object"sv);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishError",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: no name key"sv);
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5}}]",
      handler, logger);
  logger.CheckMessages({{NT_LOG_WARNING, "0: no name key"sv},
                        {NT_LOG_WARNING, "0: no type key"sv}});
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"type\":\"double\"}}]",
      handler, logger);
  logger.CheckMessages({{NT_LOG_WARNING, "0: no name key"sv},
                        {NT_LOG_WARNING, "0: no type key"sv},
                        {NT_LOG_WARNING, "0: no pubuid key"sv}});
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest, "WireDecodeTextClientTest Unpublish",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}]",
                      handler, logger);
  logger.CheckMessages({});
  CheckClientMessageCounts(handler, {.unpublish = 1});
  CHECK(handler.unpublishCalls[0] == 5);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PeriodicOutOfRange",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"subscribe\",\"params\":{\"subuid\":1,"
      "\"topics\":[\"test\"],\"options\":{\"periodic\":-1}}}]",
      handler, logger);
  net::WireDecodeText(
      "[{\"method\":\"subscribe\",\"params\":{\"subuid\":2,"
      "\"topics\":[\"test\"],\"options\":{\"periodic\":1e100}}}]",
      handler, logger);

  logger.CheckMessages({{NT_LOG_WARNING, "0: periodic value out of range"sv},
                        {NT_LOG_WARNING, "0: periodic value out of range"sv}});
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest UnpublishMultiple",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}},{\"method\":"
      "\"unpublish\",\"params\":{\"pubuid\":6}}]",
      handler, logger);
  logger.CheckMessages({});
  CheckClientMessageCounts(handler, {.unpublish = 2});
  CHECK(handler.unpublishCalls[0] == 5);
  CHECK(handler.unpublishCalls[1] == 6);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest UnpublishError",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{}}]", handler,
                      logger);
  logger.CheckMessage(NT_LOG_WARNING, "0: no pubuid key"sv);
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":\"5\"}}]", handler,
      logger);
  logger.CheckMessages({{NT_LOG_WARNING, "0: no pubuid key"sv},
                        {NT_LOG_WARNING, "0: pubuid must be a number"sv}});
  CheckNoClientCalls(handler);
}

TEST_CASE("Wire timestamps decode negotiated units and check overflow",
          "[ntcore][wire][decoder]") {
  auto version = GENERATE(NT_4_0, NT_4_1, NT_4_2);
  auto [wireTime, offset, legacyTime, nanoTime] = GENERATE(
      Catch::Generators::table<int64_t, int64_t, std::optional<int64_t>,
                               std::optional<int64_t>>(
          {{0, 123, 0, 0},
           {1, -123, 877, -122},
           {-1, 123, -877, 122},
           {6001, 123, 6'001'123, 6124},
           {9'007'199'254'740'993, 0, 9'007'199'254'740'993'000,
            9'007'199'254'740'993},
           {(std::numeric_limits<int64_t>::max)(), 0, std::nullopt,
            (std::numeric_limits<int64_t>::max)()},
           {(std::numeric_limits<int64_t>::min)(), 0, std::nullopt,
            (std::numeric_limits<int64_t>::min)()},
           {(std::numeric_limits<int64_t>::max)(), 1, std::nullopt,
            std::nullopt},
           {(std::numeric_limits<int64_t>::min)(), -1, std::nullopt,
            std::nullopt}}));
  CAPTURE(version, wireTime, offset);
  // Build raw MessagePack independently of the NT encoder: [1, time, 2, 7].
  std::vector<uint8_t> encoded{0x94, 1, 0xd3};
  for (int shift = 56; shift >= 0; shift -= 8) {
    encoded.push_back(static_cast<uint64_t>(wireTime) >> shift);
  }
  encoded.push_back(2);
  encoded.push_back(7);
  std::span<const uint8_t> input{encoded};
  int id;
  Value value;
  std::string error;
  auto expectedTime = version == NT_4_2 ? nanoTime : legacyTime;
  bool decoded =
      net::WireDecodeBinary(&input, &id, &value, &error, offset, version);
  REQUIRE(decoded == expectedTime.has_value());
  if (!expectedTime) {
    CHECK(error == "timestamp out of range");
    CHECK(input.size() == encoded.size());
  } else {
    CHECK(input.empty());
    CHECK(id == 1);
    CHECK(value.GetInteger() == 7);
    CHECK(value.server_time() == (wireTime == 0 ? 0 : *expectedTime - offset));
    CHECK(value.time() == *expectedTime);
  }
}

}  // namespace wpi::nt
