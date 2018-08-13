/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/HttpParser.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

namespace wpi {

TEST(HttpParserTest, UrlMethodHeadersComplete) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.url.connect([&](StringRef path) {
    ASSERT_EQ(path, "/foo/bar");
    ASSERT_EQ(p.GetUrl(), "/foo/bar");
    ++callbacks;
  });
  p.Execute("GET /foo");
  p.Execute("/bar");
  ASSERT_EQ(callbacks, 0);
  p.Execute(" HTTP/1.1\r\n\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_EQ(p.GetUrl(), "/foo/bar");
  ASSERT_EQ(p.GetMethod(), HTTP_GET);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, UrlMethodHeader) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.url.connect([&](StringRef path) {
    ASSERT_EQ(path, "/foo/bar");
    ASSERT_EQ(p.GetUrl(), "/foo/bar");
    ++callbacks;
  });
  p.Execute("GET /foo");
  p.Execute("/bar");
  ASSERT_EQ(callbacks, 0);
  p.Execute(" HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("F");
  ASSERT_EQ(callbacks, 1);
  ASSERT_EQ(p.GetUrl(), "/foo/bar");
  ASSERT_EQ(p.GetMethod(), HTTP_GET);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, StatusHeadersComplete) {
  HttpParser p{HttpParser::kResponse};
  int callbacks = 0;
  p.status.connect([&](StringRef status) {
    ASSERT_EQ(status, "OK");
    ASSERT_EQ(p.GetStatusCode(), 200u);
    ++callbacks;
  });
  p.Execute("HTTP/1.1 200");
  p.Execute(" OK");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_EQ(p.GetStatusCode(), 200u);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, StatusHeader) {
  HttpParser p{HttpParser::kResponse};
  int callbacks = 0;
  p.status.connect([&](StringRef status) {
    ASSERT_EQ(status, "OK");
    ASSERT_EQ(p.GetStatusCode(), 200u);
    ++callbacks;
  });
  p.Execute("HTTP/1.1 200");
  p.Execute(" OK\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("F");
  ASSERT_EQ(callbacks, 1);
  ASSERT_EQ(p.GetStatusCode(), 200u);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeaderFieldComplete) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.header.connect([&](StringRef name, StringRef value) {
    ASSERT_EQ(name, "Foo");
    ASSERT_EQ(value, "Bar");
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Fo");
  ASSERT_EQ(callbacks, 0);
  p.Execute("o: ");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Bar");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeaderFieldNext) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.header.connect([&](StringRef name, StringRef value) {
    ASSERT_EQ(name, "Foo");
    ASSERT_EQ(value, "Bar");
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Fo");
  ASSERT_EQ(callbacks, 0);
  p.Execute("o: ");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Bar");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("F");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeadersComplete) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    ASSERT_EQ(keepAlive, false);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeadersCompleteHTTP11) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    ASSERT_EQ(keepAlive, true);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeadersCompleteKeepAlive) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    ASSERT_EQ(keepAlive, true);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Connection: Keep-Alive\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, HeadersCompleteUpgrade) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.headersComplete.connect([&](bool) {
    ASSERT_TRUE(p.IsUpgrade());
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("Connection: Upgrade\r\n");
  p.Execute("Upgrade: websocket\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  ASSERT_FALSE(p.HasError());
}

TEST(HttpParserTest, Reset) {
  HttpParser p{HttpParser::kRequest};
  int callbacks = 0;
  p.headersComplete.connect([&](bool) { ++callbacks; });
  p.Execute("GET / HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 0);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 1);
  p.Reset(HttpParser::kRequest);
  p.Execute("GET / HTTP/1.1\r\n");
  ASSERT_EQ(callbacks, 1);
  p.Execute("\r\n");
  ASSERT_EQ(callbacks, 2);
  ASSERT_FALSE(p.HasError());
}

}  // namespace wpi
