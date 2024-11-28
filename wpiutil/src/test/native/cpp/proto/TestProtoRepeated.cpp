// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "TestProtoInner.h"
#include "wpi/protobuf/ProtobufCallbacks.h"
#include "wpiutil.npb.h"

struct RepeatedTestProto {
  std::vector<double> double_msg;
  std::vector<float> float_msg;
  std::vector<int32_t> int32_msg;
  std::vector<int64_t> int64_msg;
  std::vector<uint32_t> uint32_msg;
  std::vector<uint64_t> uint64_msg;
  std::vector<int32_t> sint32_msg;
  std::vector<int64_t> sint64_msg;
  std::vector<uint32_t> fixed32_msg;
  std::vector<uint64_t> fixed64_msg;
  std::vector<int32_t> sfixed32_msg;
  std::vector<int64_t> sfixed64_msg;
  wpi::SmallVector<bool, 128> bool_msg;
  std::vector<std::string> string_msg;
  std::vector<std::vector<uint8_t>> bytes_msg;
  std::vector<TestProtoInner> TestProtoInner_msg;
};

template <>
struct wpi::Protobuf<RepeatedTestProto> {
  using MessageStruct = wpi_proto_RepeatedTestProto;
  using InputStream = wpi::ProtoInputStream<RepeatedTestProto>;
  using OutputStream = wpi::ProtoOutputStream<RepeatedTestProto>;
  static std::optional<RepeatedTestProto> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const RepeatedTestProto& value);
};

std::optional<RepeatedTestProto> wpi::Protobuf<RepeatedTestProto>::Unpack(
    InputStream& stream) {
  RepeatedTestProto toRet;

  wpi::DirectUnpackCallback<double, std::vector<double>> double_msg{
      toRet.double_msg};
  wpi::DirectUnpackCallback<float, std::vector<float>> float_msg{
      toRet.float_msg};
  wpi::DirectUnpackCallback<int32_t, std::vector<int32_t>> int32_msg{
      toRet.int32_msg};
  wpi::DirectUnpackCallback<int64_t, std::vector<int64_t>> int64_msg{
      toRet.int64_msg};
  wpi::DirectUnpackCallback<uint32_t, std::vector<uint32_t>> uint32_msg{
      toRet.uint32_msg};
  wpi::DirectUnpackCallback<uint64_t, std::vector<uint64_t>> uint64_msg{
      toRet.uint64_msg};
  wpi::DirectUnpackCallback<int32_t, std::vector<int32_t>> sint32_msg{
      toRet.sint32_msg};
  wpi::DirectUnpackCallback<int64_t, std::vector<int64_t>> sint64_msg{
      toRet.sint64_msg};
  wpi::DirectUnpackCallback<uint32_t, std::vector<uint32_t>> fixed32_msg{
      toRet.fixed32_msg};
  wpi::DirectUnpackCallback<uint64_t, std::vector<uint64_t>> fixed64_msg{
      toRet.fixed64_msg};
  wpi::DirectUnpackCallback<int32_t, std::vector<int32_t>> sfixed32_msg{
      toRet.sfixed32_msg};
  wpi::DirectUnpackCallback<int64_t, std::vector<int64_t>> sfixed64_msg{
      toRet.sfixed64_msg};
  wpi::DirectUnpackCallback<bool, wpi::SmallVector<bool, 128>> bool_msg{
      toRet.bool_msg};
  wpi::DirectUnpackCallback<std::string, std::vector<std::string>> string_msg{
      toRet.string_msg};
  wpi::DirectUnpackCallback<std::vector<uint8_t>,
                            std::vector<std::vector<uint8_t>>>
      bytes_msg{toRet.bytes_msg};
  wpi::DirectUnpackCallback<TestProtoInner, std::vector<TestProtoInner>>
      TestProtoInner_msg{toRet.TestProtoInner_msg};

  wpi_proto_RepeatedTestProto msg{
      .double_msg = double_msg.Callback(),
      .float_msg = float_msg.Callback(),
      .int32_msg = int32_msg.Callback(),
      .int64_msg = int64_msg.Callback(),
      .uint32_msg = uint32_msg.Callback(),
      .uint64_msg = uint64_msg.Callback(),
      .sint32_msg = sint32_msg.Callback(),
      .sint64_msg = sint64_msg.Callback(),
      .fixed32_msg = fixed32_msg.Callback(),
      .fixed64_msg = fixed64_msg.Callback(),
      .sfixed32_msg = sfixed32_msg.Callback(),
      .sfixed64_msg = sfixed64_msg.Callback(),
      .bool_msg = bool_msg.Callback(),
      .string_msg = string_msg.Callback(),
      .bytes_msg = bytes_msg.Callback(),
      .TestProtoInner_msg = TestProtoInner_msg.Callback(),
  };

  if (!stream.Decode(msg)) {
    return {};
  }

  return toRet;
}

bool wpi::Protobuf<RepeatedTestProto>::Pack(OutputStream& stream,
                                            const RepeatedTestProto& value) {
  wpi::PackCallback<double> double_msg{value.double_msg};
  wpi::PackCallback<float> float_msg{value.float_msg};
  wpi::PackCallback<int32_t> int32_msg{value.int32_msg};
  wpi::PackCallback<int64_t> int64_msg{value.int64_msg};
  wpi::PackCallback<uint32_t> uint32_msg{value.uint32_msg};
  wpi::PackCallback<uint64_t> uint64_msg{value.uint64_msg};
  wpi::PackCallback<int32_t> sint32_msg{value.sint32_msg};
  wpi::PackCallback<int64_t> sint64_msg{value.sint64_msg};
  wpi::PackCallback<uint32_t> fixed32_msg{value.fixed32_msg};
  wpi::PackCallback<uint64_t> fixed64_msg{value.fixed64_msg};
  wpi::PackCallback<int32_t> sfixed32_msg{value.sfixed32_msg};
  wpi::PackCallback<int64_t> sfixed64_msg{value.sfixed64_msg};
  wpi::PackCallback<bool> bool_msg{value.bool_msg};
  wpi::PackCallback<std::string> string_msg{value.string_msg};
  wpi::PackCallback<std::vector<uint8_t>> bytes_msg{value.bytes_msg};
  wpi::PackCallback<TestProtoInner> TestProtoInner_msg{
      value.TestProtoInner_msg};
  wpi_proto_RepeatedTestProto msg{
      .double_msg = double_msg.Callback(),
      .float_msg = float_msg.Callback(),
      .int32_msg = int32_msg.Callback(),
      .int64_msg = int64_msg.Callback(),
      .uint32_msg = uint32_msg.Callback(),
      .uint64_msg = uint64_msg.Callback(),
      .sint32_msg = sint32_msg.Callback(),
      .sint64_msg = sint64_msg.Callback(),
      .fixed32_msg = fixed32_msg.Callback(),
      .fixed64_msg = fixed64_msg.Callback(),
      .sfixed32_msg = sfixed32_msg.Callback(),
      .sfixed64_msg = sfixed64_msg.Callback(),
      .bool_msg = bool_msg.Callback(),
      .string_msg = string_msg.Callback(),
      .bytes_msg = bytes_msg.Callback(),
      .TestProtoInner_msg = TestProtoInner_msg.Callback(),
  };
  return stream.Encode(msg);
}

namespace {
using ProtoType = wpi::Protobuf<RepeatedTestProto>;
}  // namespace

TEST(RepeatedTestProtoTest, RoundtripNanopb) {
  RepeatedTestProto kExpectedData = RepeatedTestProto{};
  kExpectedData.bool_msg.emplace_back(true);
  kExpectedData.bool_msg.emplace_back(false);

  kExpectedData.double_msg.emplace_back(5.05);

  wpi::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  ASSERT_EQ(kExpectedData.double_msg.size(), unpacked_data->double_msg.size());
  ASSERT_EQ(kExpectedData.float_msg.size(), unpacked_data->float_msg.size());
  ASSERT_EQ(kExpectedData.int32_msg.size(), unpacked_data->int32_msg.size());
  ASSERT_EQ(kExpectedData.int64_msg.size(), unpacked_data->int64_msg.size());
  ASSERT_EQ(kExpectedData.uint32_msg.size(), unpacked_data->uint32_msg.size());
  ASSERT_EQ(kExpectedData.uint64_msg.size(), unpacked_data->uint64_msg.size());
  ASSERT_EQ(kExpectedData.sint32_msg.size(), unpacked_data->sint32_msg.size());
  ASSERT_EQ(kExpectedData.sint64_msg.size(), unpacked_data->sint64_msg.size());
  ASSERT_EQ(kExpectedData.fixed32_msg.size(),
            unpacked_data->fixed32_msg.size());
  ASSERT_EQ(kExpectedData.fixed64_msg.size(),
            unpacked_data->fixed64_msg.size());
  ASSERT_EQ(kExpectedData.sfixed32_msg.size(),
            unpacked_data->sfixed32_msg.size());
  ASSERT_EQ(kExpectedData.sfixed64_msg.size(),
            unpacked_data->sfixed64_msg.size());
  ASSERT_EQ(kExpectedData.bool_msg.size(), unpacked_data->bool_msg.size());
  ASSERT_EQ(kExpectedData.string_msg.size(), unpacked_data->string_msg.size());
  ASSERT_EQ(kExpectedData.bytes_msg.size(), unpacked_data->bytes_msg.size());
  ASSERT_EQ(kExpectedData.TestProtoInner_msg.size(),
            unpacked_data->TestProtoInner_msg.size());
}
