// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/HttpUtil.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

namespace wpi {

TEST(HttpMultipartScannerTest, ExecuteExact) {
  HttpMultipartScanner scanner("foo");
  EXPECT_TRUE(scanner.Execute("abcdefg---\r\n--foo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_TRUE(scanner.GetSkipped().empty());
}

TEST(HttpMultipartScannerTest, ExecutePartial) {
  HttpMultipartScanner scanner("foo");
  EXPECT_TRUE(scanner.Execute("abcdefg--").empty());
  EXPECT_FALSE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("-\r\n").empty());
  EXPECT_FALSE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("--foo\r").empty());
  EXPECT_FALSE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("\n").empty());
  EXPECT_TRUE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, ExecuteTrailing) {
  HttpMultipartScanner scanner("foo");
  EXPECT_EQ(scanner.Execute("abcdefg---\r\n--foo\r\nxyz"), "xyz");
}

TEST(HttpMultipartScannerTest, ExecutePadding) {
  HttpMultipartScanner scanner("foo");
  EXPECT_EQ(scanner.Execute("abcdefg---\r\n--foo    \r\nxyz"), "xyz");
  EXPECT_TRUE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, SaveSkipped) {
  HttpMultipartScanner scanner("foo", true);
  scanner.Execute("abcdefg---\r\n--foo\r\n");
  EXPECT_EQ(scanner.GetSkipped(), "abcdefg---\r\n--foo\r\n");
}

TEST(HttpMultipartScannerTest, Reset) {
  HttpMultipartScanner scanner("foo", true);

  scanner.Execute("abcdefg---\r\n--foo\r\n");
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_EQ(scanner.GetSkipped(), "abcdefg---\r\n--foo\r\n");

  scanner.Reset(true);
  EXPECT_FALSE(scanner.IsDone());
  scanner.SetBoundary("bar");

  scanner.Execute("--foo\r\n--bar\r\n");
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_EQ(scanner.GetSkipped(), "--foo\r\n--bar\r\n");
}

TEST(HttpMultipartScannerTest, WithoutDashes) {
  HttpMultipartScanner scanner("foo", true);

  EXPECT_TRUE(scanner.Execute("--\r\nfoo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, SeqDashesDashes) {
  HttpMultipartScanner scanner("foo", true);
  EXPECT_TRUE(scanner.Execute("\r\n--foo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("\r\n--foo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, SeqDashesNoDashes) {
  HttpMultipartScanner scanner("foo", true);
  EXPECT_TRUE(scanner.Execute("\r\n--foo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("\r\nfoo\r\n").empty());
  EXPECT_FALSE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, SeqNoDashesDashes) {
  HttpMultipartScanner scanner("foo", true);
  EXPECT_TRUE(scanner.Execute("\r\nfoo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("\r\n--foo\r\n").empty());
  EXPECT_FALSE(scanner.IsDone());
}

TEST(HttpMultipartScannerTest, SeqNoDashesNoDashes) {
  HttpMultipartScanner scanner("foo", true);
  EXPECT_TRUE(scanner.Execute("\r\nfoo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
  EXPECT_TRUE(scanner.Execute("\r\nfoo\r\n").empty());
  EXPECT_TRUE(scanner.IsDone());
}

}  // namespace wpi
