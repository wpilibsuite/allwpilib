// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>

#include <gtest/gtest.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "../MockLogger.h"
#include "../PubSubOptionsMatcher.h"
#include "../TestPrinters.h"
#include "MockMessageHandler.h"
#include "PubSubOptions.h"
#include "gmock/gmock.h"
#include "net/MessageHandler.h"
#include "net/WireDecoder.h"
#include "networktables/NetworkTableValue.h"

using namespace std::string_view_literals;
using testing::_;
using testing::MockFunction;
using testing::StrictMock;

namespace nt {

class WireDecodeTextClientTest : public ::testing::Test {
 public:
  StrictMock<net::MockClientMessageHandler> handler;
  StrictMock<wpi::MockLogger> logger;
};

class WireDecodeTextServerTest : public ::testing::Test {
 public:
  StrictMock<net::MockServerMessageHandler> handler;
  StrictMock<wpi::MockLogger> logger;
};

TEST_F(WireDecodeTextClientTest, EmptyArray) {
  net::WireDecodeText("[]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorEmpty) {
  EXPECT_CALL(
      logger,
      Call(_, _, _,
           "could not decode JSON message: [json.exception.parse_error.101] "
           "parse error at line 1, column 1: attempting to parse an empty "
           "input; check that your input string or stream contains the "
           "expected JSON"sv));
  net::WireDecodeText("", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorBadJson1) {
  EXPECT_CALL(
      logger,
      Call(_, _, _,
           "could not decode JSON message: [json.exception.parse_error.101] "
           "parse error at line 1, column 2: syntax error while parsing value "
           "- unexpected end of input; expected '[', '{', or a literal"sv));
  net::WireDecodeText("[", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorBadJson2) {
  EXPECT_CALL(
      logger,
      Call(_, _, _,
           "could not decode JSON message: [json.exception.parse_error.101] "
           "parse error at line 1, column 3: syntax error while parsing object "
           "key - unexpected end of input; expected string literal"sv));
  net::WireDecodeText("[{", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorNotArray) {
  EXPECT_CALL(logger, Call(_, _, _, "expected JSON array at top level"sv));
  net::WireDecodeText("{}", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorMessageNotObject) {
  EXPECT_CALL(logger, Call(_, _, _, "0: expected message to be an object"sv));
  net::WireDecodeText("[5]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorNoMethodKey) {
  EXPECT_CALL(logger, Call(_, _, _, "0: no method key"sv));
  net::WireDecodeText("[{}]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorMethodNotString) {
  EXPECT_CALL(logger, Call(_, _, _, "0: method must be a string"sv));
  net::WireDecodeText("[{\"method\":5}]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorNoParamsKey) {
  EXPECT_CALL(logger, Call(_, _, _, "0: no params key"sv));
  net::WireDecodeText("[{\"method\":\"a\"}]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorParamsNotObject) {
  EXPECT_CALL(logger, Call(_, _, _, "0: params must be an object"sv));
  net::WireDecodeText("[{\"method\":\"a\",\"params\":5}]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, ErrorUnknownMethod) {
  EXPECT_CALL(logger, Call(_, _, _, "0: unrecognized method 'a'"sv));
  net::WireDecodeText("[{\"method\":\"a\",\"params\":{}}]", handler, logger);
}

TEST_F(WireDecodeTextClientTest, PublishPropsEmpty) {
  EXPECT_CALL(handler, ClientPublish(5, std::string_view{"test"},
                                     std::string_view{"double"},
                                     wpi::json::object(), PubSubOptionsEq({})));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{},\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);

  EXPECT_CALL(handler, ClientPublish(5, std::string_view{"test"},
                                     std::string_view{"double"},
                                     wpi::json::object(), PubSubOptionsEq({})));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
}

TEST_F(WireDecodeTextClientTest, PublishProps) {
  wpi::json props = {{"k", 6}};
  EXPECT_CALL(handler, ClientPublish(5, std::string_view{"test"},
                                     std::string_view{"double"}, props,
                                     PubSubOptionsEq({})));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":{\"k\":6},"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
}

TEST_F(WireDecodeTextClientTest, PublishPropsError) {
  EXPECT_CALL(logger, Call(_, _, _, "0: properties must be an object"sv));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"properties\":[\"k\"],"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);
}

TEST_F(WireDecodeTextClientTest, PublishError) {
  EXPECT_CALL(logger, Call(_, _, _, "0: no name key"sv));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"pubuid\":5,\"type\":\"double\"}}]",
      handler, logger);

  EXPECT_CALL(logger, Call(_, _, _, "0: no type key"sv));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"pubuid\":5}}]",
      handler, logger);

  EXPECT_CALL(logger, Call(_, _, _, "0: no pubuid key"sv));
  net::WireDecodeText(
      "[{\"method\":\"publish\",\"params\":{"
      "\"name\":\"test\",\"type\":\"double\"}}]",
      handler, logger);
}

TEST_F(WireDecodeTextClientTest, Unpublish) {
  EXPECT_CALL(handler, ClientUnpublish(5));
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}}]",
                      handler, logger);
}

TEST_F(WireDecodeTextClientTest, UnpublishMultiple) {
  EXPECT_CALL(handler, ClientUnpublish(5));
  EXPECT_CALL(handler, ClientUnpublish(6));
  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":5}},{\"method\":"
      "\"unpublish\",\"params\":{\"pubuid\":6}}]",
      handler, logger);
}

TEST_F(WireDecodeTextClientTest, UnpublishError) {
  EXPECT_CALL(logger, Call(_, _, _, "0: no pubuid key"sv));
  net::WireDecodeText("[{\"method\":\"unpublish\",\"params\":{}}]", handler,
                      logger);

  EXPECT_CALL(logger, Call(_, _, _, "0: pubuid must be a number"sv));
  net::WireDecodeText(
      "[{\"method\":\"unpublish\",\"params\":{\"pubuid\":\"5\"}}]", handler,
      logger);
}

}  // namespace nt
