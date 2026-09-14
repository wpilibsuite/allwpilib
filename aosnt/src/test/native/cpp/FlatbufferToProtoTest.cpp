// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/FlatbufferToProto.hpp"

#include <array>
#include <cstring>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>
#include <upb/base/status.h>
#include <upb/mem/arena.h>
#include <upb/message/array.h>
#include <upb/message/message.h>
#include <upb/reflection/def.h>
#include <upb/reflection/message.h>
#include <upb/reflection/stage0/google/protobuf/descriptor.upb.h>
#include <upb/wire/decode.h>

#include "aos/flatbuffer_merge.h"
#include "aos/flatbuffers/builder.h"
#include "aos/realtime.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_bad_attribute_test_schema.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_defaults_test_generated.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_defaults_test_schema.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_scalars_test_generated.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_scalars_test_schema.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_spanning_test_generated.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_spanning_test_schema.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_test_generated.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_test_schema.h"
#include "aosnt/src/test/fbs/flatbuffer_to_proto_test_static.h"
#include "pb.h"
#include "pb_decode.h"

namespace wpi::aosnt::testing {
namespace {

const reflection::Schema* GetSchemaFromSpan(
    flatbuffers::span<const uint8_t> span) {
  return reflection::GetSchema(span.data());
}

const reflection::Schema* GetControlDataSchema() {
  return GetSchemaFromSpan(ControlDataSchema());
}

const reflection::Schema* GetScalarsSchema() {
  return GetSchemaFromSpan(ScalarsSchema());
}

const reflection::Schema* GetSpanningSchema() {
  return GetSchemaFromSpan(spanning::SpanningSchema());
}

const reflection::Schema* GetBadAttributeSchema() {
  return GetSchemaFromSpan(BadAttributeSchema());
}

const reflection::Schema* GetDefaultsSchema() {
  return GetSchemaFromSpan(DefaultsSchema());
}

// One field as it is on the wire, decoded without reference to any
// descriptor, so these tests say what the encoder wrote rather than whether
// the descriptor agrees with it.
struct WireField {
  uint32_t number = 0;
  pb_wire_type_t wireType = PB_WT_VARINT;
  uint64_t value = 0;
  std::vector<uint8_t> bytes;
};

std::vector<WireField> Decode(std::span<const uint8_t> message) {
  std::vector<WireField> result;
  pb_istream_t stream = pb_istream_from_buffer(message.data(), message.size());
  while (stream.bytes_left > 0) {
    WireField field;
    bool eof = false;
    REQUIRE(pb_decode_tag(&stream, &field.wireType, &field.number, &eof));
    switch (field.wireType) {
      case PB_WT_VARINT:
        REQUIRE(pb_decode_varint(&stream, &field.value));
        break;
      case PB_WT_64BIT:
        REQUIRE(pb_decode_fixed64(&stream, &field.value));
        break;
      case PB_WT_32BIT: {
        uint32_t value = 0;
        REQUIRE(pb_decode_fixed32(&stream, &value));
        field.value = value;
        break;
      }
      case PB_WT_STRING: {
        uint32_t size = 0;
        REQUIRE(pb_decode_varint32(&stream, &size));
        field.bytes.resize(size);
        REQUIRE(pb_read(&stream, field.bytes.data(), size));
        break;
      }
      default:
        FAIL("unknown wire type " << field.wireType);
    }
    result.push_back(std::move(field));
  }
  return result;
}

// The elements of a packed repeated field's payload.
std::vector<uint64_t> DecodePackedVarints(std::span<const uint8_t> payload) {
  std::vector<uint64_t> result;
  pb_istream_t stream = pb_istream_from_buffer(payload.data(), payload.size());
  while (stream.bytes_left > 0) {
    uint64_t value = 0;
    REQUIRE(pb_decode_varint(&stream, &value));
    result.push_back(value);
  }
  return result;
}

std::vector<uint32_t> DecodePackedFixed32(std::span<const uint8_t> payload) {
  std::vector<uint32_t> result;
  pb_istream_t stream = pb_istream_from_buffer(payload.data(), payload.size());
  while (stream.bytes_left > 0) {
    uint32_t value = 0;
    REQUIRE(pb_decode_fixed32(&stream, &value));
    result.push_back(value);
  }
  return result;
}

std::vector<uint64_t> DecodePackedFixed64(std::span<const uint8_t> payload) {
  std::vector<uint64_t> result;
  pb_istream_t stream = pb_istream_from_buffer(payload.data(), payload.size());
  while (stream.bytes_left > 0) {
    uint64_t value = 0;
    REQUIRE(pb_decode_fixed64(&stream, &value));
    result.push_back(value);
  }
  return result;
}

const WireField* Find(const std::vector<WireField>& fields, uint32_t number) {
  for (const WireField& field : fields) {
    if (field.number == number) {
      return &field;
    }
  }
  return nullptr;
}

// The result points into fields, so a temporary would dangle.
const WireField* Find(std::vector<WireField>&& fields,
                      uint32_t number) = delete;

std::vector<uint8_t> Encode(const FlatbufferToProto& translator,
                            const uint8_t* flatbuffer) {
  std::vector<uint8_t> out(translator.GetEncodedSize(flatbuffer));
  REQUIRE(translator.Encode(flatbuffer, out) == out.size());
  return out;
}

uint32_t FloatBits(float value) {
  uint32_t bits;
  std::memcpy(&bits, &value, sizeof(bits));
  return bits;
}

uint64_t DoubleBits(double value) {
  uint64_t bits;
  std::memcpy(&bits, &value, sizeof(bits));
  return bits;
}

// Loads a descriptor into upb and decodes messages with it, the way DataLog
// and Glass read protobuf topics.
class UpbDecoder {
 public:
  explicit UpbDecoder(std::span<const uint8_t> descriptor) {
    google_protobuf_FileDescriptorProto* file =
        google_protobuf_FileDescriptorProto_parse(
            reinterpret_cast<const char*>(descriptor.data()), descriptor.size(),
            m_arena.get());
    INFO("the descriptor is not valid protobuf");
    REQUIRE(file != nullptr);
    upb_Status status;
    upb_Status_Clear(&status);
    const bool added = upb_DefPool_AddFile(m_pool.get(), file, &status);
    INFO("upb rejected the descriptor: " << upb_Status_ErrorMessage(&status));
    REQUIRE(added);
  }

  const upb_MessageDef* FindMessage(std::string_view name) {
    const upb_MessageDef* message = upb_DefPool_FindMessageByNameWithSize(
        m_pool.get(), name.data(), name.size());
    INFO("no message " << name);
    REQUIRE(message != nullptr);
    return message;
  }

  const upb_Message* Decode(const upb_MessageDef* def,
                            std::span<const uint8_t> bytes) {
    const upb_MiniTable* table = upb_MessageDef_MiniTable(def);
    upb_Message* message = upb_Message_New(table, m_arena.get());
    REQUIRE(message != nullptr);
    INFO("upb could not decode what the encoder produced");
    REQUIRE(upb_Decode(reinterpret_cast<const char*>(bytes.data()),
                       bytes.size(), message, table, nullptr, 0,
                       m_arena.get()) == kUpb_DecodeStatus_Ok);
    return message;
  }

 private:
  struct ArenaDeleter {
    void operator()(upb_Arena* arena) const { upb_Arena_Free(arena); }
  };
  struct DefPoolDeleter {
    void operator()(upb_DefPool* pool) const { upb_DefPool_Free(pool); }
  };

  std::unique_ptr<upb_Arena, ArenaDeleter> m_arena{upb_Arena_New()};
  std::unique_ptr<upb_DefPool, DefPoolDeleter> m_pool{upb_DefPool_New()};
};

const upb_FieldDef* FindField(const upb_MessageDef* def, const char* name) {
  const upb_FieldDef* field = upb_MessageDef_FindFieldByName(def, name);
  INFO("no field " << name);
  REQUIRE(field != nullptr);
  return field;
}

upb_MessageValue GetField(const upb_Message* message, const upb_MessageDef* def,
                          const char* name) {
  return upb_Message_GetFieldByDef(message, FindField(def, name));
}

flatbuffers::DetachedBuffer MakeControlData() {
  flatbuffers::FlatBufferBuilder fbb;

  const std::array<int32_t, 3> axes{-1, 2, -32768};
  const auto axesOffset = fbb.CreateVector(axes.data(), axes.size());

  JoystickDataBuilder joystick{fbb};
  joystick.add_available_buttons(0xFFFF);
  joystick.add_buttons(0x555);
  joystick.add_available_axes(0x7);
  joystick.add_axes(axesOffset);
  joystick.add_pov_count(2);
  joystick.add_povs(0xF3);
  const auto joystickOffset = joystick.Finish();

  const auto joysticksOffset = fbb.CreateVector(&joystickOffset, 1);
  const auto gameDataOffset = fbb.CreateString("LRL");

  ControlDataBuilder builder{fbb};
  builder.add_match_time(135);
  builder.add_joysticks(joysticksOffset);
  builder.add_current_op_mode(0x0300000000000042ULL);
  builder.add_control_word(0x21);
  builder.add_game_data(gameDataOffset);
  fbb.Finish(builder.Finish());

  return fbb.Release();
}

// Every field set to a value that is wrong under a neighboring encoding:
// negatives, so zigzag and a plain varint disagree, and floats whose bits are
// not their integer values.
flatbuffers::DetachedBuffer MakeScalars() {
  flatbuffers::FlatBufferBuilder fbb;

  const auto stringOffset = fbb.CreateString("hi");
  const std::array<Inner, 2> inners{Inner{7, 0.5f}, Inner{-1, -0.5f}};
  const auto innersOffset = fbb.CreateVectorOfStructs(inners.data(), 2);
  const std::array<float, 2> floats{1.0f, 2.0f};
  const auto floatsOffset = fbb.CreateVector(floats.data(), floats.size());
  const std::array<uint8_t, 2> bools{1, 0};
  const auto boolsOffset = fbb.CreateVector(bools.data(), bools.size());
  const std::array<int8_t, 2> bytes{-2, 3};
  const auto bytesOffset = fbb.CreateVector(bytes.data(), bytes.size());
  const std::array<uint8_t, 2> ubytes{200, 1};
  const auto ubytesOffset = fbb.CreateVector(ubytes.data(), ubytes.size());
  const std::array<int16_t, 2> shorts{-300, 4};
  const auto shortsOffset = fbb.CreateVector(shorts.data(), shorts.size());
  const std::array<uint16_t, 2> ushorts{40000, 5};
  const auto ushortsOffset = fbb.CreateVector(ushorts.data(), ushorts.size());
  const std::array<int32_t, 2> ints{-70000, 6};
  const auto intsOffset = fbb.CreateVector(ints.data(), ints.size());
  const std::array<uint32_t, 2> uints{3000000000u, 7};
  const auto uintsOffset = fbb.CreateVector(uints.data(), uints.size());
  const std::array<int64_t, 2> longs{-5000000000LL, 8};
  const auto longsOffset = fbb.CreateVector(longs.data(), longs.size());
  const std::array<uint64_t, 2> ulongs{0xF000000000000001ULL, 9};
  const auto ulongsOffset = fbb.CreateVector(ulongs.data(), ulongs.size());
  const std::array<double, 2> doubles{-2.25, 4.5};
  const auto doublesOffset = fbb.CreateVector(doubles.data(), doubles.size());
  const std::vector<std::string> strings{"one", "two"};
  const auto stringsOffset = fbb.CreateVectorOfStrings(strings);
  const std::array<int32_t, 2> zigzagInts{-3, 4};
  const auto zigzagIntsOffset =
      fbb.CreateVector(zigzagInts.data(), zigzagInts.size());
  const std::array<int64_t, 2> zigzagLongs{-4, 5};
  const auto zigzagLongsOffset =
      fbb.CreateVector(zigzagLongs.data(), zigzagLongs.size());
  const std::array<uint32_t, 2> fixedUints{0xDEADBEEFu, 1};
  const auto fixedUintsOffset =
      fbb.CreateVector(fixedUints.data(), fixedUints.size());
  const std::array<int32_t, 2> fixedInts{-2, 3};
  const auto fixedIntsOffset =
      fbb.CreateVector(fixedInts.data(), fixedInts.size());
  const std::array<uint64_t, 2> fixedUlongs{0x0123456789ABCDEFULL, 2};
  const auto fixedUlongsOffset =
      fbb.CreateVector(fixedUlongs.data(), fixedUlongs.size());
  const std::array<int64_t, 2> fixedLongs{-2, 3};
  const auto fixedLongsOffset =
      fbb.CreateVector(fixedLongs.data(), fixedLongs.size());

  // Inner.a is 0, which a struct writes and a table would skip.
  const Outer outer{Inner{0, 2.5f}, -2.25};

  ScalarsBuilder builder{fbb};
  builder.add_the_bool(true);
  builder.add_the_byte(-2);
  builder.add_the_ubyte(200);
  builder.add_the_short(-300);
  builder.add_the_ushort(40000);
  builder.add_the_int(-70000);
  builder.add_the_uint(3000000000u);
  builder.add_the_long(-5000000000LL);
  builder.add_the_ulong(0xF000000000000001ULL);
  builder.add_the_float(1.5f);
  builder.add_the_double(-2.25);
  builder.add_the_string(stringOffset);
  builder.add_zigzag_int(-3);
  builder.add_zigzag_long(-4);
  builder.add_fixed_uint(0xDEADBEEFu);
  builder.add_fixed_int(-2);
  builder.add_fixed_ulong(0x0123456789ABCDEFULL);
  builder.add_fixed_long(-2);
  builder.add_outer(&outer);
  builder.add_inners(innersOffset);
  builder.add_bools(boolsOffset);
  builder.add_bytes_(bytesOffset);
  builder.add_ubytes(ubytesOffset);
  builder.add_shorts(shortsOffset);
  builder.add_ushorts(ushortsOffset);
  builder.add_ints(intsOffset);
  builder.add_uints(uintsOffset);
  builder.add_longs(longsOffset);
  builder.add_ulongs(ulongsOffset);
  builder.add_floats(floatsOffset);
  builder.add_doubles(doublesOffset);
  builder.add_strings(stringsOffset);
  builder.add_zigzag_ints(zigzagIntsOffset);
  builder.add_zigzag_longs(zigzagLongsOffset);
  builder.add_fixed_uints(fixedUintsOffset);
  builder.add_fixed_ints(fixedIntsOffset);
  builder.add_fixed_ulongs(fixedUlongsOffset);
  builder.add_fixed_longs(fixedLongsOffset);
  fbb.Finish(builder.Finish());

  return fbb.Release();
}

std::vector<uint8_t> EncodeScalars(const flatbuffers::DetachedBuffer& buffer) {
  const FlatbufferToProto translator{GetScalarsSchema()};
  return Encode(translator, buffer.data());
}

}  // namespace

// MrcComm.proto numbers these MatchTime=2, Joysticks=3, CurrentOpMode=4,
// ControlWord=5, GameData=6, and the deprecated id 0 holds field 1's place.
TEST_CASE("FlatbufferToProtoTest FieldNumbersFollowTheIds",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  const std::vector<WireField> fields =
      Decode(Encode(translator, buffer.data()));

  REQUIRE(Find(fields, 2) != nullptr);
  REQUIRE(Find(fields, 3) != nullptr);
  REQUIRE(Find(fields, 4) != nullptr);
  REQUIRE(Find(fields, 5) != nullptr);
  REQUIRE(Find(fields, 6) != nullptr);

  CHECK(Find(fields, 2)->value == 135u);
  CHECK(Find(fields, 5)->value == 0x21u);
  CHECK(std::string(Find(fields, 6)->bytes.begin(),
                    Find(fields, 6)->bytes.end()) == "LRL");
}

// By name the fields are control_word, current_op_mode, game_data, joysticks,
// match_time, which is not their number order.
TEST_CASE("FlatbufferToProtoTest EmitsFieldsInNumberOrder",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};

  std::vector<uint32_t> numbers;
  for (const WireField& field : Decode(Encode(translator, buffer.data()))) {
    numbers.push_back(field.number);
  }
  CHECK(numbers == std::vector<uint32_t>{2, 3, 4, 5, 6});
}

TEST_CASE("FlatbufferToProtoTest HonorsProtoTypeAttribute",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  const std::vector<WireField> fields =
      Decode(Encode(translator, buffer.data()));

  // A ulong is a varint on its own. The attribute says fixed64.
  const WireField* opMode = Find(fields, 4);
  REQUIRE(opMode != nullptr);
  CHECK(opMode->wireType == PB_WT_64BIT);
  CHECK(opMode->value == 0x0300000000000042ULL);
}

TEST_CASE("FlatbufferToProtoTest EncodesZigzagPackedRepeated",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  const std::vector<WireField> fields =
      Decode(Encode(translator, buffer.data()));

  const WireField* joysticksField = Find(fields, 3);
  REQUIRE(joysticksField != nullptr);
  const std::vector<WireField> joysticks = Decode(joysticksField->bytes);
  const WireField* axes = Find(joysticks, 4);
  REQUIRE(axes != nullptr);
  REQUIRE(axes->wireType == PB_WT_STRING);

  // -1 -> 1, 2 -> 4, -32768 -> 65535
  CHECK(DecodePackedVarints(axes->bytes) ==
        std::vector<uint64_t>{1u, 4u, 65535u});
}

// A channel schema in a flattened AOS config has been through a
// reflection-level copy, so its root_table() is not one of its objects().
TEST_CASE("FlatbufferToProtoTest EncodesFromACopiedSchema",
          "[aosnt][flatbuffer-to-proto]") {
  const aos::FlatbufferDetachedBuffer<reflection::Schema> copied =
      aos::RecursiveCopyFlatBuffer(GetControlDataSchema());
  REQUIRE(copied.message().root_table() != nullptr);

  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto copiedTranslator{&copied.message()};
  const FlatbufferToProto originalTranslator{GetControlDataSchema()};

  const std::vector<uint8_t> fromCopy = Encode(copiedTranslator, buffer.data());
  REQUIRE_FALSE(fromCopy.empty());
  CHECK(fromCopy == Encode(originalTranslator, buffer.data()));
}

// The API an AOS caller builds messages with.
TEST_CASE("FlatbufferToProtoTest EncodesFromTheStaticApi",
          "[aosnt][flatbuffer-to-proto]") {
  aos::fbs::Builder<ControlDataStatic> builder;
  ControlDataStatic* controlData = builder.get();
  controlData->set_match_time(135);
  controlData->set_current_op_mode(0x0300000000000042ULL);
  controlData->set_control_word(0x21);
  aos::fbs::SetStringOrDie(controlData->add_game_data(), "LRL");

  auto* joysticks = controlData->add_joysticks();
  REQUIRE(joysticks->reserve(1));
  auto* joystick = joysticks->emplace_back();
  REQUIRE(joystick != nullptr);
  joystick->set_available_buttons(0xFFFF);
  joystick->set_buttons(0x555);
  joystick->set_available_axes(0x7);
  joystick->set_pov_count(2);
  joystick->set_povs(0xF3);
  auto* axes = joystick->add_axes();
  REQUIRE(axes->reserve(3));
  for (const int32_t value : {-1, 2, -32768}) {
    REQUIRE(axes->emplace_back(value));
  }
  REQUIRE(builder.Verify());

  const auto flatbuffer = builder.AsFlatbufferSpan();
  const FlatbufferToProto translator{GetControlDataSchema()};
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  CHECK(Encode(translator, flatbuffer.span().data()) ==
        Encode(translator, buffer.data()));
}

// The wire format has no notion of a package, so only the descriptor refuses.
TEST_CASE("FlatbufferToProtoTest RefusesToDescribeASchemaSpanningNamespaces",
          "[aosnt][flatbuffer-to-proto]") {
  const reflection::Schema* schema = GetSpanningSchema();

  CHECK_THROWS_WITH(BuildFileDescriptorProto(schema, "spanning.proto"),
                    Catch::Matchers::ContainsSubstring(
                        "is not in the root table's namespace"));

  flatbuffers::FlatBufferBuilder fbb;
  spanning::SpanningBuilder builder{fbb};
  fbb.Finish(builder.Finish());
  const flatbuffers::DetachedBuffer buffer = fbb.Release();

  const FlatbufferToProto translator{schema};
  std::vector<uint8_t> out(translator.GetEncodedSize(buffer.data()));
  CHECK(translator.Encode(buffer.data(), out) == out.size());
}

TEST_CASE("FlatbufferToProtoTest RejectsAProtoTypeWithNothingToPick",
          "[aosnt][flatbuffer-to-proto]") {
  const reflection::Schema* schema = GetBadAttributeSchema();
  CHECK_THROWS_WITH(
      FlatbufferToProto{schema},
      Catch::Matchers::ContainsSubstring("nothing for the attribute to pick"));
  CHECK_THROWS_AS(BuildFileDescriptorProto(schema, "bad.proto"),
                  std::invalid_argument);
}

TEST_CASE("FlatbufferToProtoTest OmitsDefaults",
          "[aosnt][flatbuffer-to-proto]") {
  flatbuffers::FlatBufferBuilder fbb;
  ControlDataBuilder builder{fbb};
  builder.add_match_time(0);
  builder.add_control_word(7);
  fbb.Finish(builder.Finish());
  const flatbuffers::DetachedBuffer buffer = fbb.Release();

  const FlatbufferToProto translator{GetControlDataSchema()};
  const std::vector<WireField> fields =
      Decode(Encode(translator, buffer.data()));

  CHECK(Find(fields, 2) == nullptr);
  REQUIRE(Find(fields, 5) != nullptr);
  CHECK(Find(fields, 5)->value == 7u);
}

// Compiles a one-field schema the way flatc would, for checking what a
// proto_type is accepted on.
std::vector<uint8_t> CompileSchemaWithField(std::string_view field) {
  flatbuffers::Parser parser;
  const std::string text = std::format(
      "attribute \"proto_type\"; table T {{ {} }} root_type T;", field);
  INFO(parser.error_);
  REQUIRE(parser.Parse(text.c_str()));
  parser.Serialize();
  return std::vector<uint8_t>(
      parser.builder_.GetBufferPointer(),
      parser.builder_.GetBufferPointer() + parser.builder_.GetSize());
}

// A proto_type names exactly the protobuf type its field is. Anything else
// would widen, truncate, or read the value with the other signedness.
TEST_CASE("FlatbufferToProtoTest AcceptsAProtoTypeMatchingItsField",
          "[aosnt][flatbuffer-to-proto]") {
  for (std::string_view field :
       {R"(f:byte (id: 0, proto_type: "sint32");)",
        R"(f:int (id: 0, proto_type: "sint32");)",
        R"(f:long (id: 0, proto_type: "sint64");)",
        R"(f:ushort (id: 0, proto_type: "fixed32");)",
        R"(f:uint (id: 0, proto_type: "fixed32");)",
        R"(f:int (id: 0, proto_type: "sfixed32");)",
        R"(f:ulong (id: 0, proto_type: "fixed64");)",
        R"(f:long (id: 0, proto_type: "sfixed64");)",
        R"(f:[short] (id: 0, proto_type: "sint32");)"}) {
    INFO(field);
    const std::vector<uint8_t> schema = CompileSchemaWithField(field);
    CHECK_NOTHROW(FlatbufferToProto{reflection::GetSchema(schema.data())});
    CHECK_NOTHROW(BuildFileDescriptorProto(reflection::GetSchema(schema.data()),
                                           "t.proto"));
  }
}

TEST_CASE("FlatbufferToProtoTest RejectsAProtoTypeNotMatchingItsField",
          "[aosnt][flatbuffer-to-proto]") {
  for (const auto& [field, message] :
       std::vector<std::pair<std::string_view, std::string_view>>{
           // Truncating.
           {R"(f:ulong (id: 0, proto_type: "fixed32");)",
            R"(its ULong type calls for "fixed64")"},
           {R"(f:long (id: 0, proto_type: "sfixed32");)",
            R"(its Long type calls for "sfixed64")"},
           {R"(f:[ulong] (id: 0, proto_type: "fixed32");)",
            R"(its ULong type calls for "fixed64")"},
           // Widening.
           {R"(f:long (id: 0, proto_type: "sint32");)",
            R"(its Long type calls for "sint64")"},
           {R"(f:int (id: 0, proto_type: "sint64");)",
            R"(its Int type calls for "sint32")"},
           {R"(f:uint (id: 0, proto_type: "fixed64");)",
            R"(its UInt type calls for "fixed32")"},
           // The other signedness.
           {R"(f:int (id: 0, proto_type: "fixed32");)",
            R"(its Int type calls for "sfixed32")"},
           {R"(f:ulong (id: 0, proto_type: "sfixed64");)",
            R"(its ULong type calls for "fixed64")"},
           {R"(f:uint (id: 0, proto_type: "sint32");)",
            "zigzag encoding is for signed integers"}}) {
    INFO(field);
    const std::vector<uint8_t> schema = CompileSchemaWithField(field);
    CHECK_THROWS_WITH(FlatbufferToProto{reflection::GetSchema(schema.data())},
                      Catch::Matchers::ContainsSubstring(std::string{message}));
    CHECK_THROWS_AS(BuildFileDescriptorProto(
                        reflection::GetSchema(schema.data()), "t.proto"),
                    std::invalid_argument);
  }
}

// proto3 reads a missing scalar as zero, so a flatbuffer default of anything
// else has to be written even when the field was never set.
TEST_CASE("FlatbufferToProtoDefaultsTest WritesAnUnsetNonzeroDefault",
          "[aosnt][flatbuffer-to-proto]") {
  flatbuffers::FlatBufferBuilder fbb;
  DefaultsBuilder builder{fbb};
  fbb.Finish(builder.Finish());
  const flatbuffers::DetachedBuffer buffer = fbb.Release();

  const FlatbufferToProto translator{GetDefaultsSchema()};
  const std::vector<uint8_t> encoded = Encode(translator, buffer.data());
  const std::vector<WireField> fields = Decode(encoded);

  REQUIRE(Find(fields, 1) != nullptr);
  CHECK(Find(fields, 1)->value == 5u);
  REQUIRE(Find(fields, 2) != nullptr);
  CHECK(Find(fields, 2)->value == DoubleBits(1.5));
  REQUIRE(Find(fields, 3) != nullptr);
  CHECK(Find(fields, 3)->value == 1u);
  CHECK(Find(fields, 4) == nullptr);

  UpbDecoder decoder{BuildFileDescriptorProto(
      GetDefaultsSchema(), "flatbuffer_to_proto_defaults_test.proto")};
  const upb_MessageDef* defaults =
      decoder.FindMessage(GetProtoMessageName(GetDefaultsSchema()));
  const upb_Message* message = decoder.Decode(defaults, encoded);
  CHECK(GetField(message, defaults, "mode").int32_val == 5);
  CHECK(GetField(message, defaults, "gain").double_val == 1.5);
  CHECK(GetField(message, defaults, "enabled").bool_val);
}

// Zero is what a receiver assumes, so it is omitted even where the flatbuffer
// default is something else.
TEST_CASE("FlatbufferToProtoDefaultsTest OmitsAZeroThatOverridesADefault",
          "[aosnt][flatbuffer-to-proto]") {
  flatbuffers::FlatBufferBuilder fbb;
  DefaultsBuilder builder{fbb};
  builder.add_mode(0);
  builder.add_gain(0.0);
  builder.add_enabled(false);
  fbb.Finish(builder.Finish());
  const flatbuffers::DetachedBuffer buffer = fbb.Release();

  const FlatbufferToProto translator{GetDefaultsSchema()};
  const std::vector<uint8_t> encoded = Encode(translator, buffer.data());
  CHECK(Decode(encoded).empty());

  UpbDecoder decoder{BuildFileDescriptorProto(
      GetDefaultsSchema(), "flatbuffer_to_proto_defaults_test.proto")};
  const upb_MessageDef* defaults =
      decoder.FindMessage(GetProtoMessageName(GetDefaultsSchema()));
  const upb_Message* message = decoder.Decode(defaults, encoded);
  CHECK(GetField(message, defaults, "mode").int32_val == 0);
  CHECK(GetField(message, defaults, "gain").double_val == 0.0);
  CHECK_FALSE(GetField(message, defaults, "enabled").bool_val);
}

// -0.0 compares equal to 0.0, but its bits are not zero, and protobuf writes
// it.
TEST_CASE("FlatbufferToProtoDefaultsTest WritesNegativeZero",
          "[aosnt][flatbuffer-to-proto]") {
  flatbuffers::FlatBufferBuilder fbb;
  // The builder skips a value equal to the default, and -0.0f == 0.0f.
  fbb.ForceDefaults(true);
  DefaultsBuilder builder{fbb};
  builder.add_offset(-0.0f);
  fbb.Finish(builder.Finish());
  const flatbuffers::DetachedBuffer buffer = fbb.Release();

  const FlatbufferToProto translator{GetDefaultsSchema()};
  const std::vector<WireField> fields =
      Decode(Encode(translator, buffer.data()));
  REQUIRE(Find(fields, 4) != nullptr);
  CHECK(Find(fields, 4)->wireType == PB_WT_32BIT);
  CHECK(Find(fields, 4)->value == FloatBits(-0.0f));
}

// aos::ScopedRealtime makes any allocation inside Encode() fatal.
TEST_CASE("FlatbufferToProtoTest EncodingDoesNotAllocate",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  std::vector<uint8_t> out(translator.GetEncodedSize(buffer.data()));

  size_t written = 0;
  {
    aos::ScopedRealtime realtime;
    written = translator.Encode(buffer.data(), out);
  }
  CHECK(written == out.size());
}

TEST_CASE("FlatbufferToProtoTest RefusesASmallBuffer",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  std::vector<uint8_t> out(translator.GetEncodedSize(buffer.data()) - 1);
  CHECK(translator.Encode(buffer.data(), out) == 0u);
}

// The descriptor and the encoder have to agree on every field number, wire
// type and name, which only a real protobuf decoder can check.
TEST_CASE("FlatbufferToProtoTest UpbDecodesUsingTheBuiltDescriptor",
          "[aosnt][flatbuffer-to-proto]") {
  UpbDecoder decoder{BuildFileDescriptorProto(
      GetControlDataSchema(), "flatbuffer_to_proto_test.proto")};
  const upb_MessageDef* controlData =
      decoder.FindMessage(GetProtoMessageName(GetControlDataSchema()));

  const flatbuffers::DetachedBuffer buffer = MakeControlData();
  const FlatbufferToProto translator{GetControlDataSchema()};
  const std::vector<uint8_t> encoded = Encode(translator, buffer.data());
  const upb_Message* message = decoder.Decode(controlData, encoded);

  CHECK(GetField(message, controlData, "match_time").int32_val == 135);
  CHECK(GetField(message, controlData, "control_word").uint32_val == 0x21u);
  const upb_StringView gameData =
      GetField(message, controlData, "game_data").str_val;
  CHECK(std::string_view(gameData.data, gameData.size) == "LRL");
  CHECK(GetField(message, controlData, "current_op_mode").uint64_val ==
        0x0300000000000042ULL);

  const upb_FieldDef* joysticksField = FindField(controlData, "joysticks");
  const upb_Array* joysticks =
      upb_Message_GetFieldByDef(message, joysticksField).array_val;
  REQUIRE(joysticks != nullptr);
  REQUIRE(upb_Array_Size(joysticks) == 1u);
  const upb_Message* joystick = upb_Array_Get(joysticks, 0).msg_val;
  const upb_MessageDef* joystickDef =
      upb_FieldDef_MessageSubDef(joysticksField);

  const upb_FieldDef* axesField = FindField(joystickDef, "axes");
  REQUIRE(upb_FieldDef_Type(axesField) == kUpb_FieldType_SInt32);
  const upb_Array* axes =
      upb_Message_GetFieldByDef(joystick, axesField).array_val;
  REQUIRE(axes != nullptr);
  REQUIRE(upb_Array_Size(axes) == 3u);
  CHECK(upb_Array_Get(axes, 0).int32_val == -1);
  CHECK(upb_Array_Get(axes, 1).int32_val == 2);
  CHECK(upb_Array_Get(axes, 2).int32_val == -32768);
}

TEST_CASE("FlatbufferToProtoTest DeprecatedFieldsAreNotInTheDescriptor",
          "[aosnt][flatbuffer-to-proto]") {
  UpbDecoder decoder{BuildFileDescriptorProto(
      GetControlDataSchema(), "flatbuffer_to_proto_test.proto")};
  const upb_MessageDef* controlData =
      decoder.FindMessage(GetProtoMessageName(GetControlDataSchema()));

  CHECK(upb_MessageDef_FindFieldByName(controlData, "reserved_0") == nullptr);
  CHECK(upb_MessageDef_FindFieldByNumber(controlData, 1) == nullptr);
  CHECK(upb_FieldDef_Number(FindField(controlData, "match_time")) == 2u);
  CHECK(upb_FieldDef_Number(FindField(controlData, "game_data")) == 6u);
}

// A signed integer narrower than 64 bits is sign-extended to ten bytes of
// varint, as protobuf writes a negative int32.
TEST_CASE("FlatbufferToProtoScalarsTest EncodesSignedIntegersAsVarints",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  for (const auto& [number, expected] :
       std::vector<std::pair<uint32_t, uint64_t>>{
           {2, static_cast<uint64_t>(int64_t{-2})},
           {4, static_cast<uint64_t>(int64_t{-300})},
           {6, static_cast<uint64_t>(int64_t{-70000})},
           {8, static_cast<uint64_t>(int64_t{-5000000000LL})}}) {
    INFO("field " << number);
    const WireField* field = Find(fields, number);
    REQUIRE(field != nullptr);
    CHECK(field->wireType == PB_WT_VARINT);
    CHECK(field->value == expected);
  }
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesUnsignedIntegersAsVarints",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  for (const auto& [number, expected] :
       std::vector<std::pair<uint32_t, uint64_t>>{{3, 200u},
                                                  {5, 40000u},
                                                  {7, 3000000000u},
                                                  {9, 0xF000000000000001ULL}}) {
    INFO("field " << number);
    const WireField* field = Find(fields, number);
    REQUIRE(field != nullptr);
    CHECK(field->wireType == PB_WT_VARINT);
    CHECK(field->value == expected);
  }
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesABoolAsAVarint",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));
  const WireField* field = Find(fields, 1);

  REQUIRE(field != nullptr);
  CHECK(field->wireType == PB_WT_VARINT);
  CHECK(field->value == 1u);
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesAFloatAsItsBits",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));
  const WireField* field = Find(fields, 10);

  REQUIRE(field != nullptr);
  CHECK(field->wireType == PB_WT_32BIT);
  CHECK(field->value == FloatBits(1.5f));
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesADoubleAsItsBits",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));
  const WireField* field = Find(fields, 11);

  REQUIRE(field != nullptr);
  CHECK(field->wireType == PB_WT_64BIT);
  CHECK(field->value == DoubleBits(-2.25));
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesAString",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));
  const WireField* field = Find(fields, 12);

  REQUIRE(field != nullptr);
  CHECK(field->wireType == PB_WT_STRING);
  CHECK(std::string(field->bytes.begin(), field->bytes.end()) == "hi");
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesZigzagScalars",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* zigzagInt = Find(fields, 13);
  REQUIRE(zigzagInt != nullptr);
  CHECK(zigzagInt->wireType == PB_WT_VARINT);
  CHECK(zigzagInt->value == 5u);

  const WireField* zigzagLong = Find(fields, 14);
  REQUIRE(zigzagLong != nullptr);
  CHECK(zigzagLong->wireType == PB_WT_VARINT);
  CHECK(zigzagLong->value == 7u);
}

// Fixed-width integers are truncated to their width, not sign-extended.
TEST_CASE("FlatbufferToProtoScalarsTest EncodesFixedWidthIntegers",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* fixedUint = Find(fields, 15);
  REQUIRE(fixedUint != nullptr);
  CHECK(fixedUint->wireType == PB_WT_32BIT);
  CHECK(fixedUint->value == 0xDEADBEEFu);

  const WireField* fixedInt = Find(fields, 16);
  REQUIRE(fixedInt != nullptr);
  CHECK(fixedInt->wireType == PB_WT_32BIT);
  CHECK(fixedInt->value == 0xFFFFFFFEu);

  const WireField* fixedUlong = Find(fields, 17);
  REQUIRE(fixedUlong != nullptr);
  CHECK(fixedUlong->wireType == PB_WT_64BIT);
  CHECK(fixedUlong->value == 0x0123456789ABCDEFULL);

  const WireField* fixedLong = Find(fields, 18);
  REQUIRE(fixedLong != nullptr);
  CHECK(fixedLong->wireType == PB_WT_64BIT);
  CHECK(fixedLong->value == 0xFFFFFFFFFFFFFFFEULL);
}

// Every field of a struct is on the wire, including Inner.a's zero.
TEST_CASE("FlatbufferToProtoScalarsTest EncodesANestedStructWithZeroes",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* outer = Find(fields, 19);
  REQUIRE(outer != nullptr);
  REQUIRE(outer->wireType == PB_WT_STRING);
  const std::vector<WireField> outerFields = Decode(outer->bytes);
  REQUIRE(outerFields.size() == 2u);

  const WireField* inner = Find(outerFields, 1);
  REQUIRE(inner != nullptr);
  REQUIRE(inner->wireType == PB_WT_STRING);

  const WireField* c = Find(outerFields, 2);
  REQUIRE(c != nullptr);
  CHECK(c->wireType == PB_WT_64BIT);
  CHECK(c->value == DoubleBits(-2.25));

  const std::vector<WireField> innerFields = Decode(inner->bytes);
  REQUIRE(innerFields.size() == 2u);

  const WireField* a = Find(innerFields, 1);
  REQUIRE(a != nullptr);
  CHECK(a->wireType == PB_WT_VARINT);
  CHECK(a->value == 0u);

  const WireField* b = Find(innerFields, 2);
  REQUIRE(b != nullptr);
  CHECK(b->wireType == PB_WT_32BIT);
  CHECK(b->value == FloatBits(2.5f));
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesAVectorOfStructs",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  std::vector<const WireField*> inners;
  for (const WireField& field : fields) {
    if (field.number == 20) {
      inners.push_back(&field);
    }
  }
  REQUIRE(inners.size() == 2u);

  const std::vector<WireField> first = Decode(inners[0]->bytes);
  REQUIRE(Find(first, 1) != nullptr);
  REQUIRE(Find(first, 2) != nullptr);
  CHECK(Find(first, 1)->value == 7u);
  CHECK(Find(first, 2)->value == FloatBits(0.5f));

  const std::vector<WireField> second = Decode(inners[1]->bytes);
  REQUIRE(Find(second, 1) != nullptr);
  REQUIRE(Find(second, 2) != nullptr);
  CHECK(Find(second, 1)->value == static_cast<uint64_t>(int64_t{-1}));
  CHECK(Find(second, 2)->value == FloatBits(-0.5f));
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesPackedVarintVectors",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const auto packed = [&fields](uint32_t number) {
    INFO("field " << number);
    const WireField* field = Find(fields, number);
    REQUIRE(field != nullptr);
    REQUIRE(field->wireType == PB_WT_STRING);
    return DecodePackedVarints(field->bytes);
  };

  CHECK(packed(21) == std::vector<uint64_t>{1, 0});
  CHECK(packed(22) ==
        std::vector<uint64_t>{static_cast<uint64_t>(int64_t{-2}), 3});
  CHECK(packed(23) == std::vector<uint64_t>{200, 1});
  CHECK(packed(24) ==
        std::vector<uint64_t>{static_cast<uint64_t>(int64_t{-300}), 4});
  CHECK(packed(25) == std::vector<uint64_t>{40000, 5});
  CHECK(packed(26) ==
        std::vector<uint64_t>{static_cast<uint64_t>(int64_t{-70000}), 6});
  CHECK(packed(27) == std::vector<uint64_t>{3000000000u, 7});
  CHECK(packed(28) == std::vector<uint64_t>{
                          static_cast<uint64_t>(int64_t{-5000000000LL}), 8});
  CHECK(packed(29) == std::vector<uint64_t>{0xF000000000000001ULL, 9});
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesPackedFloatAndDoubleVectors",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* floats = Find(fields, 30);
  REQUIRE(floats != nullptr);
  REQUIRE(floats->wireType == PB_WT_STRING);
  CHECK(DecodePackedFixed32(floats->bytes) ==
        std::vector<uint32_t>{FloatBits(1.0f), FloatBits(2.0f)});

  const WireField* doubles = Find(fields, 31);
  REQUIRE(doubles != nullptr);
  REQUIRE(doubles->wireType == PB_WT_STRING);
  CHECK(DecodePackedFixed64(doubles->bytes) ==
        std::vector<uint64_t>{DoubleBits(-2.25), DoubleBits(4.5)});
}

// Strings cannot be packed, so each is its own tagged field.
TEST_CASE("FlatbufferToProtoScalarsTest EncodesRepeatedStringsUnpacked",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  std::vector<std::string> strings;
  for (const WireField& field : fields) {
    if (field.number == 32) {
      CHECK(field.wireType == PB_WT_STRING);
      strings.emplace_back(field.bytes.begin(), field.bytes.end());
    }
  }
  CHECK(strings == std::vector<std::string>{"one", "two"});
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesPackedZigzagVectors",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* ints = Find(fields, 33);
  REQUIRE(ints != nullptr);
  // -3 -> 5, 4 -> 8
  CHECK(DecodePackedVarints(ints->bytes) == std::vector<uint64_t>{5, 8});

  const WireField* longs = Find(fields, 34);
  REQUIRE(longs != nullptr);
  // -4 -> 7, 5 -> 10
  CHECK(DecodePackedVarints(longs->bytes) == std::vector<uint64_t>{7, 10});
}

TEST_CASE("FlatbufferToProtoScalarsTest EncodesPackedFixedWidthVectors",
          "[aosnt][flatbuffer-to-proto]") {
  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<WireField> fields = Decode(EncodeScalars(buffer));

  const WireField* fixedUints = Find(fields, 35);
  REQUIRE(fixedUints != nullptr);
  CHECK(DecodePackedFixed32(fixedUints->bytes) ==
        std::vector<uint32_t>{0xDEADBEEFu, 1});

  const WireField* fixedInts = Find(fields, 36);
  REQUIRE(fixedInts != nullptr);
  CHECK(DecodePackedFixed32(fixedInts->bytes) ==
        std::vector<uint32_t>{0xFFFFFFFEu, 3});

  const WireField* fixedUlongs = Find(fields, 37);
  REQUIRE(fixedUlongs != nullptr);
  CHECK(DecodePackedFixed64(fixedUlongs->bytes) ==
        std::vector<uint64_t>{0x0123456789ABCDEFULL, 2});

  const WireField* fixedLongs = Find(fields, 38);
  REQUIRE(fixedLongs != nullptr);
  CHECK(DecodePackedFixed64(fixedLongs->bytes) ==
        std::vector<uint64_t>{0xFFFFFFFFFFFFFFFEULL, 3});
}

// Every encoding survives a round trip through upb, not just the ones
// ControlData uses.
TEST_CASE("FlatbufferToProtoScalarsTest UpbDecodesEveryEncoding",
          "[aosnt][flatbuffer-to-proto]") {
  UpbDecoder decoder{BuildFileDescriptorProto(
      GetScalarsSchema(), "flatbuffer_to_proto_scalars_test.proto")};
  const upb_MessageDef* scalars =
      decoder.FindMessage(GetProtoMessageName(GetScalarsSchema()));

  const flatbuffers::DetachedBuffer buffer = MakeScalars();
  const std::vector<uint8_t> encoded = EncodeScalars(buffer);
  const upb_Message* message = decoder.Decode(scalars, encoded);

  CHECK(GetField(message, scalars, "the_bool").bool_val);
  CHECK(GetField(message, scalars, "the_byte").int32_val == -2);
  CHECK(GetField(message, scalars, "the_ubyte").uint32_val == 200u);
  CHECK(GetField(message, scalars, "the_short").int32_val == -300);
  CHECK(GetField(message, scalars, "the_long").int64_val == -5000000000LL);
  CHECK(GetField(message, scalars, "the_ulong").uint64_val ==
        0xF000000000000001ULL);
  CHECK(GetField(message, scalars, "the_float").float_val == 1.5f);
  CHECK(GetField(message, scalars, "the_double").double_val == -2.25);
  CHECK(GetField(message, scalars, "zigzag_int").int32_val == -3);
  CHECK(GetField(message, scalars, "zigzag_long").int64_val == -4LL);
  CHECK(GetField(message, scalars, "fixed_uint").uint32_val == 0xDEADBEEFu);
  CHECK(GetField(message, scalars, "fixed_int").int32_val == -2);
  CHECK(GetField(message, scalars, "fixed_long").int64_val == -2LL);

  const upb_FieldDef* outerField = FindField(scalars, "outer");
  const upb_Message* outer = GetField(message, scalars, "outer").msg_val;
  REQUIRE(outer != nullptr);
  const upb_MessageDef* outerDef = upb_FieldDef_MessageSubDef(outerField);
  const upb_FieldDef* innerField = FindField(outerDef, "inner");
  const upb_Message* inner = GetField(outer, outerDef, "inner").msg_val;
  REQUIRE(inner != nullptr);
  const upb_MessageDef* innerDef = upb_FieldDef_MessageSubDef(innerField);
  CHECK(GetField(inner, innerDef, "a").int32_val == 0);
  CHECK(GetField(inner, innerDef, "b").float_val == 2.5f);
}

}  // namespace wpi::aosnt::testing
