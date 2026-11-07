// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/HttpUtil.hpp"

#include <catch2/catch_test_macros.hpp>

namespace wpi::net {

TEST_CASE("HttpMultipartScannerTest ExecuteExact", "[http][multipart]") {
  HttpMultipartScanner scanner("foo");
  CHECK(scanner.Execute("abcdefg---\r\n--foo\r\n").empty());
  CHECK(scanner.IsDone());
  CHECK(scanner.GetSkipped().empty());
}

TEST_CASE("HttpMultipartScannerTest ExecutePartial", "[http][multipart]") {
  HttpMultipartScanner scanner("foo");
  CHECK(scanner.Execute("abcdefg--").empty());
  CHECK_FALSE(scanner.IsDone());
  CHECK(scanner.Execute("-\r\n").empty());
  CHECK_FALSE(scanner.IsDone());
  CHECK(scanner.Execute("--foo\r").empty());
  CHECK_FALSE(scanner.IsDone());
  CHECK(scanner.Execute("\n").empty());
  CHECK(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest ExecuteTrailing", "[http][multipart]") {
  HttpMultipartScanner scanner("foo");
  CHECK(scanner.Execute("abcdefg---\r\n--foo\r\nxyz") == "xyz");
}

TEST_CASE("HttpMultipartScannerTest ExecutePadding", "[http][multipart]") {
  HttpMultipartScanner scanner("foo");
  CHECK(scanner.Execute("abcdefg---\r\n--foo    \r\nxyz") == "xyz");
  CHECK(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest SaveSkipped", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);
  scanner.Execute("abcdefg---\r\n--foo\r\n");
  CHECK(scanner.GetSkipped() == "abcdefg---\r\n--foo\r\n");
}

TEST_CASE("HttpMultipartScannerTest Reset", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);

  scanner.Execute("abcdefg---\r\n--foo\r\n");
  CHECK(scanner.IsDone());
  CHECK(scanner.GetSkipped() == "abcdefg---\r\n--foo\r\n");

  scanner.Reset(true);
  CHECK_FALSE(scanner.IsDone());
  scanner.SetBoundary("bar");

  scanner.Execute("--foo\r\n--bar\r\n");
  CHECK(scanner.IsDone());
  CHECK(scanner.GetSkipped() == "--foo\r\n--bar\r\n");
}

TEST_CASE("HttpMultipartScannerTest WithoutDashes", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);

  CHECK(scanner.Execute("--\r\nfoo\r\n").empty());
  CHECK(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest SeqDashesDashes", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);
  CHECK(scanner.Execute("\r\n--foo\r\n").empty());
  CHECK(scanner.IsDone());
  CHECK(scanner.Execute("\r\n--foo\r\n").empty());
  CHECK(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest SeqDashesNoDashes", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);
  CHECK(scanner.Execute("\r\n--foo\r\n").empty());
  CHECK(scanner.IsDone());
  CHECK(scanner.Execute("\r\nfoo\r\n").empty());
  CHECK_FALSE(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest SeqNoDashesDashes", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);
  CHECK(scanner.Execute("\r\nfoo\r\n").empty());
  CHECK(scanner.IsDone());
  CHECK(scanner.Execute("\r\n--foo\r\n").empty());
  CHECK_FALSE(scanner.IsDone());
}

TEST_CASE("HttpMultipartScannerTest SeqNoDashesNoDashes", "[http][multipart]") {
  HttpMultipartScanner scanner("foo", true);
  CHECK(scanner.Execute("\r\nfoo\r\n").empty());
  CHECK(scanner.IsDone());
  CHECK(scanner.Execute("\r\nfoo\r\n").empty());
  CHECK(scanner.IsDone());
}

}  // namespace wpi::net
