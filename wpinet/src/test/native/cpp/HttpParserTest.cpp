// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/HttpParser.hpp"

#include <catch2/catch_test_macros.hpp>

namespace wpi::net {

TEST_CASE("HttpParserTest UrlMethodHeadersComplete", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.url.connect([&](std::string_view path) {
    REQUIRE(path == "/foo/bar");
    REQUIRE(p.GetUrl() == "/foo/bar");
    ++callbacks;
  });
  p.Execute("GET /foo");
  p.Execute("/bar");
  REQUIRE(callbacks == 0);
  p.Execute(" HTTP/1.1\r\n\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE(p.GetUrl() == "/foo/bar");
  REQUIRE(p.GetMethod() == HTTP_GET);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest UrlMethodHeader", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.url.connect([&](std::string_view path) {
    REQUIRE(path == "/foo/bar");
    REQUIRE(p.GetUrl() == "/foo/bar");
    ++callbacks;
  });
  p.Execute("GET /foo");
  p.Execute("/bar");
  REQUIRE(callbacks == 0);
  p.Execute(" HTTP/1.1\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("F");
  REQUIRE(callbacks == 1);
  REQUIRE(p.GetUrl() == "/foo/bar");
  REQUIRE(p.GetMethod() == HTTP_GET);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest StatusHeadersComplete", "[http][parser]") {
  HttpParser p{HttpParser::Type::RESPONSE};
  int callbacks = 0;
  p.status.connect([&](std::string_view status) {
    REQUIRE(status == "OK");
    REQUIRE(p.GetStatusCode() == 200u);
    ++callbacks;
  });
  p.Execute("HTTP/1.1 200");
  p.Execute(" OK");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE(p.GetStatusCode() == 200u);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest StatusHeader", "[http][parser]") {
  HttpParser p{HttpParser::Type::RESPONSE};
  int callbacks = 0;
  p.status.connect([&](std::string_view status) {
    REQUIRE(status == "OK");
    REQUIRE(p.GetStatusCode() == 200u);
    ++callbacks;
  });
  p.Execute("HTTP/1.1 200");
  p.Execute(" OK\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("F");
  REQUIRE(callbacks == 1);
  REQUIRE(p.GetStatusCode() == 200u);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeaderFieldComplete", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.header.connect([&](std::string_view name, std::string_view value) {
    REQUIRE(name == "Foo");
    REQUIRE(value == "Bar");
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("Fo");
  REQUIRE(callbacks == 0);
  p.Execute("o: ");
  REQUIRE(callbacks == 0);
  p.Execute("Bar");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeaderFieldNext", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.header.connect([&](std::string_view name, std::string_view value) {
    REQUIRE(name == "Foo");
    REQUIRE(value == "Bar");
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("Fo");
  REQUIRE(callbacks == 0);
  p.Execute("o: ");
  REQUIRE(callbacks == 0);
  p.Execute("Bar");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("F");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeadersComplete", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    REQUIRE(keepAlive == false);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeadersCompleteHTTP11", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    REQUIRE(keepAlive == true);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.1\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeadersCompleteKeepAlive", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.headersComplete.connect([&](bool keepAlive) {
    REQUIRE(keepAlive == true);
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("Connection: Keep-Alive\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest HeadersCompleteUpgrade", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.headersComplete.connect([&](bool) {
    REQUIRE(p.IsUpgrade());
    ++callbacks;
  });
  p.Execute("GET / HTTP/1.0\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("Connection: Upgrade\r\n");
  p.Execute("Upgrade: websocket\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  REQUIRE_FALSE(p.HasError());
}

TEST_CASE("HttpParserTest Reset", "[http][parser]") {
  HttpParser p{HttpParser::Type::REQUEST};
  int callbacks = 0;
  p.headersComplete.connect([&](bool) { ++callbacks; });
  p.Execute("GET / HTTP/1.1\r\n");
  REQUIRE(callbacks == 0);
  p.Execute("\r\n");
  REQUIRE(callbacks == 1);
  p.Reset(HttpParser::Type::REQUEST);
  p.Execute("GET / HTTP/1.1\r\n");
  REQUIRE(callbacks == 1);
  p.Execute("\r\n");
  REQUIRE(callbacks == 2);
  REQUIRE_FALSE(p.HasError());
}

}  // namespace wpi::net
