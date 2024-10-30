// // Copyright (c) FIRST and other WPILib contributors.
// // Open Source Software; you can modify and/or share it under the terms of
// // the WPILib BSD license file in the root directory of this project.

// #include "TestProtoInner.h"
// #include <gtest/gtest.h>

// #include <wpi/ProtoHelper.h>

// #include <vector>

// #include "wpiutil.npb.h"

// #include "wpi/protobuf/ProtobufCallbacks.h"

// struct RepeatedTestProto {
//     std::vector<double> double_msg;
//     std::vector<float> float_msg;
//     std::vector<int32_t> int32_msg;
//     std::vector<int64_t> int64_msg;
//     std::vector<uint32_t> uint32_msg;
//     std::vector<uint64_t> uint64_msg;
//     std::vector<int32_t> sint32_msg;
//     std::vector<int64_t> sint64_msg;
//     std::vector<uint32_t> fixed32_msg;
//     std::vector<uint64_t> fixed64_msg;
//     std::vector<int32_t> sfixed32_msg;
//     std::vector<int64_t> sfixed64_msg;
//     wpi::SmallVector<bool, 128> bool_msg;
//     std::vector<std::string> string_msg;
//     std::vector<std::vector<uint8_t>> bytes_msg;
//     std::vector<TestProtoInner> TestProtoInner_msg;
// };

// template <>
// struct wpi::Protobuf<RepeatedTestProto> {
//   static const pb_msgdesc_t* Message();
//   static std::optional<RepeatedTestProto> Unpack(wpi::ProtoInputStream& stream);
//   static bool Pack(wpi::ProtoOutputStream& stream, const RepeatedTestProto& value);
// };

// const pb_msgdesc_t* wpi::Protobuf<RepeatedTestProto>::Message() {
//   return get_wpi_proto_RepeatedTestProto_msg();
// }

// std::optional<RepeatedTestProto> wpi::Protobuf<RepeatedTestProto>::Unpack(
//     wpi::ProtoInputStream& stream) {
//   wpi::StringUnpackCallback str;
//   wpi::StdVectorBytesUnpackCallback bytes;
//   wpi::UnpackCallback<TestProtoInner> inner;
//   wpi_proto_RepeatedTestProto msg;
//   msg.string_msg = str.Callback();
//   msg.bytes_msg = bytes.Callback();
//   msg.TestProtoInner_msg = inner.Callback();
//   if (!stream.DecodeNoInit(msg)) {
//     return {};
//   }

//   auto istr = str.Items();
//   auto ibytes = bytes.Items();
//   auto iinner = inner.Items();

//   if (istr.empty() || ibytes.empty() || iinner.empty()) {
//     return {};
//   }

//   return RepeatedTestProto {
//     .double_msg = msg.double_msg,
//     .float_msg = msg.float_msg,
//     .int32_msg = msg.int32_msg,
//     .int64_msg = msg.int64_msg,
//     .uint32_msg = msg.uint32_msg,
//     .uint64_msg = msg.uint64_msg,
//     .sint32_msg = msg.sint32_msg,
//     .sint64_msg = msg.sint64_msg,
//     .fixed32_msg = msg.fixed32_msg,
//     .fixed64_msg = msg.fixed64_msg,
//     .sfixed32_msg = msg.sfixed32_msg,
//     .sfixed64_msg = msg.sfixed64_msg,
//     .bool_msg = msg.bool_msg,
//     .string_msg = std::move(istr[0]),
//     .bytes_msg = std::move(ibytes[0]),
//     .TestProtoInner_msg = std::move(iinner[0]),
//   };
// }

// bool wpi::Protobuf<RepeatedTestProto>::Pack(wpi::ProtoOutputStream& stream,
//                                          const RepeatedTestProto& value) {
//   wpi::PackCallback str{&value.string_msg};
//   wpi::PackCallback bytes{&value.bytes_msg};
//   wpi::PackCallback inner{&value.TestProtoInner_msg};
//   wpi_proto_RepeatedTestProto msg{
//     .double_msg = value.double_msg,
//     .float_msg = value.float_msg,
//     .int32_msg = value.int32_msg,
//     .int64_msg = value.int64_msg,
//     .uint32_msg = value.uint32_msg,
//     .uint64_msg = value.uint64_msg,
//     .sint32_msg = value.sint32_msg,
//     .sint64_msg = value.sint64_msg,
//     .fixed32_msg = value.fixed32_msg,
//     .fixed64_msg = value.fixed64_msg,
//     .sfixed32_msg = value.sfixed32_msg,
//     .sfixed64_msg = value.sfixed64_msg,
//     .bool_msg = value.bool_msg,
//     .string_msg = str.Callback(),
//     .bytes_msg = bytes.Callback(),
//     .TestProtoInner_msg = inner.Callback(),
//   };
//   return stream.Encode(msg);
// }

// namespace {
// using ProtoType = wpi::Protobuf<RepeatedTestProto>;
// }  // namespace

// TEST(RepeatedTestProto, RoundtripNanopb) {
//   const RepeatedTestProto kExpectedData = RepeatedTestProto{};

//   wpi::SmallVector<uint8_t, 64> buf;
//   wpi::ProtoOutputStream ostream{buf, ProtoType::Message()};

//   ASSERT_TRUE(ProtoType::Pack(ostream, kExpectedData));

//   wpi::ProtoInputStream istream{buf, ProtoType::Message()};
//   std::optional<RepeatedTestProto> unpacked_data = ProtoType::Unpack(istream);
//   ASSERT_TRUE(unpacked_data.has_value());
// }
