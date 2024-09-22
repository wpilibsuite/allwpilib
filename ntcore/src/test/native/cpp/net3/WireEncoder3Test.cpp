// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cfloat>
#include <climits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>
#include <wpi/raw_ostream.h>

#include "../TestPrinters.h"
#include "net3/Message3.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"

using namespace std::string_view_literals;

namespace nt {

class WireEncoder3Test : public ::testing::Test {
 protected:
  std::vector<uint8_t> out;
  wpi::raw_uvector_ostream os{out};
};

TEST_F(WireEncoder3Test, Unknown) {
  net3::WireEncode(os, net3::Message3{});
  ASSERT_TRUE(out.empty());
}

TEST_F(WireEncoder3Test, KeepAlive) {
  net3::WireEncode(os, net3::Message3::KeepAlive());
  ASSERT_THAT(out, wpi::SpanEq("\x00"_us));
}

TEST_F(WireEncoder3Test, ClientHello) {
  net3::WireEncode(os, net3::Message3::ClientHello("hello"));
  ASSERT_THAT(out, wpi::SpanEq("\x01\x03\x00\x05hello"_us));
}

TEST_F(WireEncoder3Test, ProtoUnsup) {
  net3::WireEncode(os, net3::Message3::ProtoUnsup());
  net3::WireEncode(os, net3::Message3::ProtoUnsup(0x0200u));
  ASSERT_THAT(out, wpi::SpanEq("\x02\x03\x00\x02\x02\x00"_us));
}

TEST_F(WireEncoder3Test, ServerHelloDone) {
  net3::WireEncode(os, net3::Message3::ServerHelloDone());
  ASSERT_THAT(out, wpi::SpanEq("\x03"_us));
}

TEST_F(WireEncoder3Test, ServerHello) {
  net3::WireEncode(os, net3::Message3::ServerHello(0x03, "hello"));
  ASSERT_THAT(out, wpi::SpanEq("\x04\x03\x05hello"_us));
}

TEST_F(WireEncoder3Test, ClientHelloDone) {
  net3::WireEncode(os, net3::Message3::ClientHelloDone());
  ASSERT_THAT(out, wpi::SpanEq("\x05"_us));
}

TEST_F(WireEncoder3Test, FlagsUpdate) {
  net3::WireEncode(os, net3::Message3::FlagsUpdate(0x5678, 0x03));
  ASSERT_THAT(out, wpi::SpanEq("\x12\x56\x78\x03"_us));
}

TEST_F(WireEncoder3Test, EntryDelete) {
  net3::WireEncode(os, net3::Message3::EntryDelete(0x5678));
  ASSERT_THAT(out, wpi::SpanEq("\x13\x56\x78"_us));
}

TEST_F(WireEncoder3Test, ClearEntries) {
  net3::WireEncode(os, net3::Message3::ClearEntries());
  ASSERT_THAT(out, wpi::SpanEq("\x14\xd0\x6c\xb2\x7a"_us));
}

TEST_F(WireEncoder3Test, ExecuteRpc) {
  net3::WireEncode(os, net3::Message3::ExecuteRpc(0x5678, 0x1234, "hello"_us));
  ASSERT_THAT(out, wpi::SpanEq("\x20\x56\x78\x12\x34\x05hello"_us));
}

TEST_F(WireEncoder3Test, RpcResponse) {
  net3::WireEncode(os, net3::Message3::RpcResponse(0x5678, 0x1234, "hello"_us));
  ASSERT_THAT(out, wpi::SpanEq("\x21\x56\x78\x12\x34\x05hello"_us));
}

TEST_F(WireEncoder3Test, EntryAssignBoolean) {
  net3::WireEncode(os,
                   net3::Message3::EntryAssign("test"sv, 0x5678, 0x1234,
                                               Value::MakeBoolean(true), 0x9a));
  ASSERT_THAT(out, wpi::SpanEq("\x10\x04test\x00\x56\x78\x12\x34\x9a\x01"_us));
}

TEST_F(WireEncoder3Test, EntryAssignDouble) {
  net3::WireEncode(os,
                   net3::Message3::EntryAssign("test"sv, 0x5678, 0x1234,
                                               Value::MakeDouble(2.3e5), 0x9a));
  ASSERT_THAT(out, wpi::SpanEq("\x10\x04test\x01\x56\x78\x12\x34"
                               "\x9a\x41\x0c\x13\x80\x00\x00\x00\x00"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateBoolean) {
  net3::WireEncode(os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                                   Value::MakeBoolean(true)));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x00\x01"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateDouble) {
  // values except min and max from
  // http://www.binaryconvert.com/result_double.html
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234, Value::MakeDouble(0.0)));
  ASSERT_THAT(
      out, wpi::SpanEq(
               "\x11\x56\x78\x12\x34\x01\x00\x00\x00\x00\x00\x00\x00\x00"_us));

  out.clear();
  net3::WireEncode(os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                                   Value::MakeDouble(2.3e5)));
  ASSERT_THAT(
      out, wpi::SpanEq(
               "\x11\x56\x78\x12\x34\x01\x41\x0c\x13\x80\x00\x00\x00\x00"_us));

  out.clear();
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(
              0x5678, 0x1234,
              Value::MakeDouble(std::numeric_limits<double>::infinity())));
  ASSERT_THAT(
      out, wpi::SpanEq(
               "\x11\x56\x78\x12\x34\x01\x7f\xf0\x00\x00\x00\x00\x00\x00"_us));

  out.clear();
  net3::WireEncode(os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                                   Value::MakeDouble(DBL_MIN)));
  ASSERT_THAT(
      out, wpi::SpanEq(
               "\x11\x56\x78\x12\x34\x01\x00\x10\x00\x00\x00\x00\x00\x00"_us));

  out.clear();
  net3::WireEncode(os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                                   Value::MakeDouble(DBL_MAX)));
  ASSERT_THAT(
      out, wpi::SpanEq(
               "\x11\x56\x78\x12\x34\x01\x7f\xef\xff\xff\xff\xff\xff\xff"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateString) {
  net3::WireEncode(os, net3::Message3::EntryUpdate(
                           0x5678, 0x1234, Value::MakeString("hello"sv)));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x02\x05hello"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateString2) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x7f};
  ex.insert(ex.end(), 127, '*');
  std::string in(127, '*');
  net3::WireEncode(os, net3::Message3::EntryUpdate(
                           0x5678, 0x1234, Value::MakeString(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateStringLarge) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x80, 0x01};
  ex.insert(ex.end(), 127, '*');
  ex.push_back('x');

  std::string in(127, '*');
  in.push_back('x');

  net3::WireEncode(os, net3::Message3::EntryUpdate(
                           0x5678, 0x1234, Value::MakeString(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateStringHuge) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x02, 0x81, 0x80, 0x04};
  ex.insert(ex.end(), 65534, '*');
  ex.insert(ex.end(), 3, 'x');

  std::string in(65534, '*');
  in.append(3, 'x');

  net3::WireEncode(os, net3::Message3::EntryUpdate(
                           0x5678, 0x1234, Value::MakeString(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateRaw) {
  net3::WireEncode(os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                                   Value::MakeRaw("hello"_us)));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x03\x05hello"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateBooleanArray) {
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(
              0x5678, 0x1234, Value::MakeBooleanArray({false, true, false})));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x10\x03\x00\x01\x00"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateBooleanArrayLarge) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x10, 0xff};
  ex.insert(ex.end(), 255, 0);
  std::vector<int> in(255, 0);
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeBooleanArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateBooleanArrayTrunc) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x10, 0xff};
  ex.insert(ex.end(), 255, 0);
  std::vector<int> in(256, 0);
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeBooleanArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateDoubleArray) {
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeDoubleArray({0.5, 0.25})));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x11\x02"
                               "\x3f\xe0\x00\x00\x00\x00\x00\x00"
                               "\x3f\xd0\x00\x00\x00\x00\x00\x00"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateDoubleArrayLarge) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x11, 0xff};
  ex.insert(ex.end(), 255 * 8, 0);
  std::vector<double> in(255, 0.0);
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeDoubleArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateDoubleArrayTrunc) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x11, 0xff};
  ex.insert(ex.end(), 255 * 8, 0);
  std::vector<double> in(256, 0.0);
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeDoubleArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateStringArray) {
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(
              0x5678, 0x1234, Value::MakeStringArray({"hello", "bye"})));
  ASSERT_THAT(out, wpi::SpanEq("\x11\x56\x78\x12\x34\x12\x02\x05hello\x03"
                               "bye"_us));
}

TEST_F(WireEncoder3Test, EntryUpdateStringArrayLarge) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x12, 0xff};
  ex.insert(ex.end(), 255, 0);
  std::vector<std::string> in(255, "");
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeStringArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

TEST_F(WireEncoder3Test, EntryUpdateStringArrayTrunc) {
  std::vector<uint8_t> ex{0x11, 0x56, 0x78, 0x12, 0x34, 0x12, 0xff};
  ex.insert(ex.end(), 255, 0);
  std::vector<std::string> in(256, "");
  net3::WireEncode(
      os, net3::Message3::EntryUpdate(0x5678, 0x1234,
                                      Value::MakeStringArray(std::move(in))));
  ASSERT_THAT(out, ex);
}

}  // namespace nt
