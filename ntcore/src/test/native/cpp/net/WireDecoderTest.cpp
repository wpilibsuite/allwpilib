// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/WireDecoder.hpp"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "../MockAssertions.hpp"
#include "../MockLogger.hpp"
#include "../TestPrinters.hpp"
#include "MockMessageHandler.hpp"
#include "PubSubOptions.hpp"
#include "net/MessageHandler.hpp"
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

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest EmptyArray",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[]", handler, logger);
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorEmpty",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("", handler, logger);
  CHECK(logger.HasMessage("could not decode JSON message: absent_value"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorBadJson1",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[", handler, logger);
  CHECK(logger.HasMessage("could not decode JSON message: unexpected_eof"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorBadJson2",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{", handler, logger);
  CHECK(logger.HasMessage("could not decode JSON message: unexpected_eof"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNotArray",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("{}", handler, logger);
  CHECK(logger.HasMessage("expected JSON array at top level"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorMessageNotObject",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[5]", handler, logger);
  CHECK(logger.HasMessage("0: expected message to be an object"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNoMethodKey",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{}]", handler, logger);
  CHECK(logger.HasMessage("0: no method key"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorMethodNotString",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":5}]", handler, logger);
  CHECK(logger.HasMessage("0: method must be a string"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorNoParamsKey",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\"}]", handler, logger);
  CHECK(logger.HasMessage("0: no params key"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorParamsNotObject",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\",\"params\":5}]", handler, logger);
  CHECK(logger.HasMessage("0: params must be an object"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest ErrorUnknownMethod",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"a\",\"params\":{}}]", handler, logger);
  CHECK(logger.HasMessage("0: unrecognized method 'a'"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishPropsEmpty",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  REQUIRE(handler.publishCalls.size() == 1u);
  CheckPublish(handler.publishCalls.back(), 5, "test", "double",
               wpi::util::json::object());

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  REQUIRE(handler.publishCalls.size() == 2u);
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
  REQUIRE(handler.publishCalls.size() == 1u);
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
  CHECK(logger.HasMessage("0: properties must be an object"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest PublishError",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
  CHECK(logger.HasMessage("0: no name key"sv));
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5}}]",
      handler, logger);
  CHECK(logger.HasMessage("0: no type key"sv));
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"type\":\"double\"}}]",
      handler, logger);
  CHECK(logger.HasMessage("0: no pubuid key"sv));
  CheckNoClientCalls(handler);
}

TEST_CASE_METHOD(WireDecodeTextClientTest, "WireDecodeTextClientTest Unpublish",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}]",
                      handler, logger);
  REQUIRE(handler.unpublishCalls.size() == 1u);
  CHECK(handler.unpublishCalls[0] == 5);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest UnpublishMultiple",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}},{\"method\":"
      "\"unpublish\",\"params\":{\"pubuid\":6}}]",
      handler, logger);
  REQUIRE(handler.unpublishCalls.size() == 2u);
  CHECK(handler.unpublishCalls[0] == 5);
  CHECK(handler.unpublishCalls[1] == 6);
}

TEST_CASE_METHOD(WireDecodeTextClientTest,
                 "WireDecodeTextClientTest UnpublishError",
                 "[ntcore][wire][decoder]") {
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{}}]", handler,
                      logger);
  CHECK(logger.HasMessage("0: no pubuid key"sv));
  CheckNoClientCalls(handler);

  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":\"5\"}}]", handler,
      logger);
  CHECK(logger.HasMessage("0: pubuid must be a number"sv));
  CheckNoClientCalls(handler);
}

}  // namespace wpi::nt
