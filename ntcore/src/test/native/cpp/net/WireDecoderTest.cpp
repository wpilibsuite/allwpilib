// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/WireDecoder.hpp"

#include <limits>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../MockAssertions.hpp"
#include "../MockLogger.hpp"
#include "../TestPrinters.hpp"
#include "MockMessageHandler.hpp"
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
    net::WireEncodeBinary(os, 1, timestamp, Value::MakeInteger(1));
    std::span<const uint8_t> input{encoded};
    int id;
    Value value;
    std::string error;

    CHECK_FALSE(net::WireDecodeBinary(&input, &id, &value, &error, offset));
    CHECK(error == "timestamp out of range");
    CHECK(input.size() == encoded.size());
  };

  check(std::numeric_limits<int64_t>::max(), 808);
  check(std::numeric_limits<int64_t>::min(), -809);
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

}  // namespace wpi::nt
