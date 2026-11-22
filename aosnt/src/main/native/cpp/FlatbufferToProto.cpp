// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/FlatbufferToProto.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <upb/base/string_view.h>
#include <upb/mem/arena.h>
#include <upb/reflection/stage0/google/protobuf/descriptor.upb.h>

#include "pb.h"
#include "pb_encode.h"

#ifdef PB_WITHOUT_64BIT
#error "FlatbufferToProto encodes 64-bit fields, which nanopb is built without"
#endif

namespace wpi::aosnt {
namespace {

bool IsSigned(reflection::BaseType type) {
  return type == reflection::BaseType::Byte ||
         type == reflection::BaseType::Short ||
         type == reflection::BaseType::Int ||
         type == reflection::BaseType::Long;
}

std::string_view GetProtoTypeAttribute(const reflection::Field& field) {
  if (!field.attributes()) {
    return {};
  }
  const reflection::KeyValue* attribute =
      field.attributes()->LookupByKey("proto_type");
  if (!attribute || !attribute->value()) {
    return {};
  }
  return attribute->value()->string_view();
}

// The protobuf type a field encodes as. Everything else about the field's
// encoding, and its descriptor, is derived from this.
bool IsRepeated(const reflection::Field& field) {
  return field.type()->base_type() == reflection::BaseType::Vector ||
         field.type()->base_type() == reflection::BaseType::Array;
}

// Refuses an enum protobuf cannot represent as a protobuf enum, rather than
// describing the field as an integer or changing the enum to fit.
void CheckEnum(const reflection::Field& field, const reflection::Enum& enumDef,
               std::string_view attribute) {
  const std::string_view fieldName = field.name()->string_view();
  const std::string_view enumName = enumDef.name()->string_view();
  if (!attribute.empty()) {
    throw std::invalid_argument(std::format(
        "field {} has proto_type \"{}\", but it is the enum {}, which is a "
        "protobuf enum, so there is nothing for the attribute to pick",
        fieldName, attribute, enumName));
  }
  // Only a schema compiled with --bfbs-builtins records bit_flags. Its values
  // are single bits, so without the attribute it is refused for having no 0.
  if (enumDef.attributes() && enumDef.attributes()->LookupByKey("bit_flags")) {
    throw std::invalid_argument(std::format(
        "field {} is the bit_flags enum {}, which holds combinations of its "
        "values, and a protobuf enum holds one value",
        fieldName, enumName));
  }
  const reflection::BaseType underlying =
      enumDef.underlying_type()->base_type();
  switch (underlying) {
    case reflection::BaseType::Byte:
    case reflection::BaseType::UByte:
    case reflection::BaseType::Short:
    case reflection::BaseType::UShort:
    case reflection::BaseType::Int:
      break;
    default:
      throw std::invalid_argument(std::format(
          "field {} is the enum {}, which is a {}, and a protobuf enum is an "
          "int32",
          fieldName, enumName, reflection::EnumNameBaseType(underlying)));
  }
  const bool hasZero = std::any_of(
      enumDef.values()->begin(), enumDef.values()->end(),
      [](const reflection::EnumVal* value) { return value->value() == 0; });
  if (!hasZero) {
    throw std::invalid_argument(std::format(
        "field {} is the enum {}, which has no value of 0, and a proto3 enum's "
        "first value has to be 0",
        fieldName, enumName));
  }
}

upb_FieldType GetProtoType(const reflection::Schema& schema,
                           const reflection::Field& field) {
  // A vector and a struct's fixed-length array are both a repeated field of
  // their element type.
  const reflection::BaseType type =
      IsRepeated(field) ? field.type()->element() : field.type()->base_type();
  const std::string_view attribute = GetProtoTypeAttribute(field);

  if (type == reflection::BaseType::Union ||
      type == reflection::BaseType::UType) {
    throw std::invalid_argument(
        std::format("field {} is part of a union, which has no protobuf "
                    "equivalent",
                    field.name()->string_view()));
  }

  switch (type) {
    case reflection::BaseType::String:
    case reflection::BaseType::Obj:
    case reflection::BaseType::Bool:
    case reflection::BaseType::Float:
    case reflection::BaseType::Double:
      if (!attribute.empty()) {
        throw std::invalid_argument(std::format(
            "field {} has proto_type \"{}\", but its type has only one "
            "protobuf type, so there is nothing for the attribute to pick",
            field.name()->string_view(), attribute));
      }
      break;
    default:
      break;
  }

  switch (type) {
    case reflection::BaseType::String:
      return kUpb_FieldType_String;
    case reflection::BaseType::Obj:
      return kUpb_FieldType_Message;
    case reflection::BaseType::Bool:
      return kUpb_FieldType_Bool;
    case reflection::BaseType::Float:
      return kUpb_FieldType_Float;
    case reflection::BaseType::Double:
      return kUpb_FieldType_Double;
    default:
      break;
  }

  if (!flatbuffers::IsInteger(type)) {
    throw std::invalid_argument(
        std::format("field {} has a type with no protobuf equivalent",
                    field.name()->string_view()));
  }

  // An integer that refers to an enum is that enum.
  if (field.type()->index() >= 0) {
    CheckEnum(field, *schema.enums()->Get(field.type()->index()), attribute);
    return kUpb_FieldType_Enum;
  }

  if (!attribute.empty()) {
    // The attribute has to name exactly the protobuf type the flatbuffer type
    // is, so that nothing is widened, truncated or read with the other
    // signedness without the schema saying so.
    const bool isLong = flatbuffers::IsLong(type);
    const bool isSigned = IsSigned(type);
    std::string_view expected;
    upb_FieldType protoType;
    if (attribute == "sint32" || attribute == "sint64") {
      if (!isSigned) {
        throw std::invalid_argument(std::format(
            "field {} has proto_type \"{}\", but zigzag encoding is for signed "
            "integers and the field is a {}",
            field.name()->string_view(), attribute,
            reflection::EnumNameBaseType(type)));
      }
      expected = isLong ? "sint64" : "sint32";
      protoType = isLong ? kUpb_FieldType_SInt64 : kUpb_FieldType_SInt32;
    } else if (attribute == "fixed32" || attribute == "sfixed32" ||
               attribute == "fixed64" || attribute == "sfixed64") {
      if (isSigned) {
        expected = isLong ? "sfixed64" : "sfixed32";
        protoType = isLong ? kUpb_FieldType_SFixed64 : kUpb_FieldType_SFixed32;
      } else {
        expected = isLong ? "fixed64" : "fixed32";
        protoType = isLong ? kUpb_FieldType_Fixed64 : kUpb_FieldType_Fixed32;
      }
    } else {
      throw std::invalid_argument(std::format(
          "field {} has proto_type \"{}\", which is not one of sint32, sint64, "
          "fixed32, fixed64, sfixed32 or sfixed64",
          field.name()->string_view(), attribute));
    }
    if (attribute != expected) {
      throw std::invalid_argument(std::format(
          "field {} has proto_type \"{}\", but its {} type calls for \"{}\"",
          field.name()->string_view(), attribute,
          reflection::EnumNameBaseType(type), expected));
    }
    return protoType;
  }

  if (flatbuffers::IsLong(type)) {
    return IsSigned(type) ? kUpb_FieldType_Int64 : kUpb_FieldType_UInt64;
  }
  return IsSigned(type) ? kUpb_FieldType_Int32 : kUpb_FieldType_UInt32;
}

pb_wire_type_t GetWireType(upb_FieldType type) {
  switch (type) {
    case kUpb_FieldType_Double:
    case kUpb_FieldType_Fixed64:
    case kUpb_FieldType_SFixed64:
      return PB_WT_64BIT;
    case kUpb_FieldType_Float:
    case kUpb_FieldType_Fixed32:
    case kUpb_FieldType_SFixed32:
      return PB_WT_32BIT;
    case kUpb_FieldType_String:
    case kUpb_FieldType_Bytes:
    case kUpb_FieldType_Message:
      return PB_WT_STRING;
    default:
      return PB_WT_VARINT;
  }
}

uint32_t GetFieldNumber(const reflection::Field& field) {
  return field.id() + 1;
}

bool WriteScalar(pb_ostream_t* stream, upb_FieldType type, int64_t integer,
                 double real) {
  switch (type) {
    case kUpb_FieldType_SInt32:
    case kUpb_FieldType_SInt64:
      return pb_encode_svarint(stream, integer);
    case kUpb_FieldType_Float: {
      const float value = static_cast<float>(real);
      uint32_t bits;
      std::memcpy(&bits, &value, sizeof(bits));
      return pb_encode_fixed32(stream, &bits);
    }
    case kUpb_FieldType_Double: {
      uint64_t bits;
      std::memcpy(&bits, &real, sizeof(bits));
      return pb_encode_fixed64(stream, &bits);
    }
    case kUpb_FieldType_Fixed32:
    case kUpb_FieldType_SFixed32: {
      const uint32_t value = static_cast<uint32_t>(integer);
      return pb_encode_fixed32(stream, &value);
    }
    case kUpb_FieldType_Fixed64:
    case kUpb_FieldType_SFixed64: {
      const uint64_t value = static_cast<uint64_t>(integer);
      return pb_encode_fixed64(stream, &value);
    }
    default:
      // A negative int32 is sign-extended to ten bytes, as protobuf does.
      return pb_encode_varint(stream, static_cast<uint64_t>(integer));
  }
}

}  // namespace

// Walks a message twice per nesting level: once with a sizing stream to learn
// a submessage's length prefix, then again to write it.
struct FlatbufferToProto::Walker {
  const FlatbufferToProto& translator;

  const std::vector<Field>& GetFields(const reflection::Object& object) const {
    return translator.m_fields.find(&object)->second;
  }

  const reflection::Object& GetObject(const reflection::Field& field) const {
    return *translator.m_schema->objects()->Get(field.type()->index());
  }

  bool WriteSubmessage(pb_ostream_t* stream, uint32_t number,
                       const reflection::Object& object,
                       const flatbuffers::Table& table) const {
    pb_ostream_t sizing = PB_OSTREAM_SIZING;
    return WriteTable(&sizing, object, table) &&
           pb_encode_tag(stream, PB_WT_STRING, number) &&
           pb_encode_varint(stream, sizing.bytes_written) &&
           WriteTable(stream, object, table);
  }

  bool WriteSubmessage(pb_ostream_t* stream, uint32_t number,
                       const reflection::Object& object,
                       const flatbuffers::Struct& value) const {
    pb_ostream_t sizing = PB_OSTREAM_SIZING;
    return WriteStruct(&sizing, object, value) &&
           pb_encode_tag(stream, PB_WT_STRING, number) &&
           pb_encode_varint(stream, sizing.bytes_written) &&
           WriteStruct(stream, object, value);
  }

  static bool WriteString(pb_ostream_t* stream, uint32_t number,
                          const flatbuffers::String& value) {
    return pb_encode_tag(stream, PB_WT_STRING, number) &&
           pb_encode_string(stream,
                            reinterpret_cast<const pb_byte_t*>(value.data()),
                            value.size());
  }

  // Scalars are packed: one length-delimited run of the elements. get(i)
  // returns element i as an integer and as a real, and only the one the type
  // uses matters.
  template <typename Get>
  static bool WritePacked(pb_ostream_t* stream, uint32_t number,
                          upb_FieldType type, size_t count, Get get) {
    pb_ostream_t sizing = PB_OSTREAM_SIZING;
    for (size_t i = 0; i < count; ++i) {
      const auto [integer, real] = get(i);
      WriteScalar(&sizing, type, integer, real);
    }
    if (!pb_encode_tag(stream, PB_WT_STRING, number) ||
        !pb_encode_varint(stream, sizing.bytes_written)) {
      return false;
    }
    for (size_t i = 0; i < count; ++i) {
      const auto [integer, real] = get(i);
      if (!WriteScalar(stream, type, integer, real)) {
        return false;
      }
    }
    return true;
  }

  // A fixed-length array lives inline in its struct, so every element is
  // present, zeros included.
  bool WriteArray(pb_ostream_t* stream, const Field& field,
                  const flatbuffers::Struct& value) const {
    const reflection::Type& type = *field.field->type();
    const uint32_t number = GetFieldNumber(*field.field);
    const uint8_t* data = value.GetAddressOf(field.field->offset());
    const size_t length = type.fixed_length();

    if (type.element() == reflection::BaseType::Obj) {
      const reflection::Object& object = GetObject(*field.field);
      for (size_t i = 0; i < length; ++i) {
        if (!WriteSubmessage(stream, number, object,
                             *reinterpret_cast<const flatbuffers::Struct*>(
                                 data + i * object.bytesize()))) {
          return false;
        }
      }
      return true;
    }

    const size_t size = flatbuffers::GetTypeSize(type.element());
    return WritePacked(stream, number, field.type, length, [&](size_t i) {
      return std::pair{
          flatbuffers::GetAnyValueI(type.element(), data + i * size),
          flatbuffers::GetAnyValueF(type.element(), data + i * size)};
    });
  }

  bool WriteStruct(pb_ostream_t* stream, const reflection::Object& object,
                   const flatbuffers::Struct& value) const {
    for (const Field& field : GetFields(object)) {
      const uint32_t number = GetFieldNumber(*field.field);
      if (field.field->type()->base_type() == reflection::BaseType::Array) {
        if (!WriteArray(stream, field, value)) {
          return false;
        }
        continue;
      }
      if (field.type == kUpb_FieldType_Message) {
        if (!WriteSubmessage(
                stream, number, GetObject(*field.field),
                *flatbuffers::GetFieldStruct(value, *field.field))) {
          return false;
        }
        continue;
      }
      // A struct has no vtable, so a zero here is a real zero rather than a
      // default to skip.
      if (!pb_encode_tag(stream, GetWireType(field.type), number) ||
          !WriteScalar(stream, field.type,
                       flatbuffers::GetAnyFieldI(value, *field.field),
                       flatbuffers::GetAnyFieldF(value, *field.field))) {
        return false;
      }
    }
    return true;
  }

  bool WriteVector(pb_ostream_t* stream, const Field& field,
                   const flatbuffers::Table& table) const {
    const flatbuffers::VectorOfAny* vector =
        flatbuffers::GetFieldAnyV(table, *field.field);
    if (!vector || vector->size() == 0) {
      return true;
    }
    const uint32_t number = GetFieldNumber(*field.field);
    const reflection::BaseType element = field.field->type()->element();

    if (element == reflection::BaseType::String) {
      const auto* strings = reinterpret_cast<
          const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>*>(
          vector);
      for (const flatbuffers::String* value : *strings) {
        if (!WriteString(stream, number, *value)) {
          return false;
        }
      }
      return true;
    }

    if (element == reflection::BaseType::Obj) {
      const reflection::Object& object = GetObject(*field.field);
      for (size_t i = 0; i < vector->size(); ++i) {
        const bool ok =
            object.is_struct()
                ? WriteSubmessage(stream, number, object,
                                  *flatbuffers::GetAnyVectorElemAddressOf<
                                      const flatbuffers::Struct>(
                                      vector, i, object.bytesize()))
                : WriteSubmessage(stream, number, object,
                                  *flatbuffers::GetAnyVectorElemPointer<
                                      const flatbuffers::Table>(vector, i));
        if (!ok) {
          return false;
        }
      }
      return true;
    }

    return WritePacked(
        stream, number, field.type, vector->size(), [&](size_t i) {
          return std::pair{flatbuffers::GetAnyVectorElemI(vector, element, i),
                           flatbuffers::GetAnyVectorElemF(vector, element, i)};
        });
  }

  bool WriteTable(pb_ostream_t* stream, const reflection::Object& object,
                  const flatbuffers::Table& table) const {
    for (const Field& field : GetFields(object)) {
      if (field.field->deprecated()) {
        continue;
      }
      const uint32_t number = GetFieldNumber(*field.field);

      switch (field.field->type()->base_type()) {
        case reflection::BaseType::String: {
          const flatbuffers::String* value =
              flatbuffers::GetFieldS(table, *field.field);
          // proto3 omits an empty string.
          if (value && value->size() != 0 &&
              !WriteString(stream, number, *value)) {
            return false;
          }
          break;
        }
        case reflection::BaseType::Obj: {
          const reflection::Object& nested = GetObject(*field.field);
          if (nested.is_struct()) {
            const flatbuffers::Struct* value =
                flatbuffers::GetFieldStruct(table, *field.field);
            if (value && !WriteSubmessage(stream, number, nested, *value)) {
              return false;
            }
          } else {
            const flatbuffers::Table* value =
                flatbuffers::GetFieldT(table, *field.field);
            if (value && !WriteSubmessage(stream, number, nested, *value)) {
              return false;
            }
          }
          break;
        }
        case reflection::BaseType::Vector:
          if (!WriteVector(stream, field, table)) {
            return false;
          }
          break;
        default: {
          const int64_t integer =
              flatbuffers::GetAnyFieldI(table, *field.field);
          const double real = flatbuffers::GetAnyFieldF(table, *field.field);
          // proto3 omits a scalar that is zero, and a receiver reads a missing
          // one as zero. The flatbuffer default is not the protobuf default,
          // so an unset field with a nonzero default is written. -0.0 is not
          // zero here, as it is not to protobuf.
          const bool isZero =
              flatbuffers::IsFloat(field.field->type()->base_type())
                  ? real == 0.0 && !std::signbit(real)
                  : integer == 0;
          if (!isZero &&
              (!pb_encode_tag(stream, GetWireType(field.type), number) ||
               !WriteScalar(stream, field.type, integer, real))) {
            return false;
          }
          break;
        }
      }
    }
    return true;
  }
};

FlatbufferToProto::FlatbufferToProto(const reflection::Schema* schema)
    : m_schema{schema} {
  if (!m_schema) {
    throw std::invalid_argument("schema is null");
  }
  if (!m_schema->root_table()) {
    throw std::invalid_argument("schema has no root table");
  }

  // A schema that has been through a reflection-level copy, as every channel
  // schema in a flattened AOS config has, can carry a root_table() that is not
  // one of its objects().
  std::vector<const reflection::Object*> objects(m_schema->objects()->begin(),
                                                 m_schema->objects()->end());
  objects.push_back(m_schema->root_table());

  for (const reflection::Object* object : objects) {
    // Usually root_table() is one of objects(), so it is seen twice.
    if (m_fields.contains(object)) {
      continue;
    }
    std::vector<Field>& fields = m_fields[object];
    for (const reflection::Field* field : *object->fields()) {
      if (field->deprecated()) {
        fields.push_back({field, kUpb_FieldType_Int32});
        continue;
      }
      fields.push_back({field, GetProtoType(*m_schema, *field)});
    }
    std::sort(fields.begin(), fields.end(), [](const Field& a, const Field& b) {
      return a.field->id() < b.field->id();
    });
  }
}

size_t FlatbufferToProto::GetEncodedSize(const uint8_t* flatbuffer) const {
  pb_ostream_t sizing = PB_OSTREAM_SIZING;
  Walker{*this}.WriteTable(&sizing, *m_schema->root_table(),
                           *flatbuffers::GetAnyRoot(flatbuffer));
  return sizing.bytes_written;
}

size_t FlatbufferToProto::Encode(const uint8_t* flatbuffer,
                                 std::span<uint8_t> buffer) const {
  const size_t needed = GetEncodedSize(flatbuffer);
  if (needed > buffer.size()) {
    return 0;
  }
  pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());
  if (!Walker{*this}.WriteTable(&stream, *m_schema->root_table(),
                                *flatbuffers::GetAnyRoot(flatbuffer))) {
    return 0;
  }
  assert(stream.bytes_written == needed);
  return stream.bytes_written;
}

namespace {

std::string_view GetLastComponent(std::string_view name) {
  const size_t dot = name.rfind('.');
  return dot == std::string_view::npos ? name : name.substr(dot + 1);
}

std::string_view GetNamespace(std::string_view name) {
  const size_t dot = name.rfind('.');
  return dot == std::string_view::npos ? std::string_view{}
                                       : name.substr(0, dot);
}

template <typename T>
T* CheckAllocated(T* ptr) {
  if (!ptr) {
    throw std::bad_alloc{};
  }
  return ptr;
}

upb_StringView MakeStringView(std::string_view str) {
  return upb_StringView_FromDataAndSize(str.data(), str.size());
}

// A upb_StringView does not own its bytes, so a string built here has to live
// in the arena until the descriptor is serialized.
upb_StringView CopyToArena(upb_Arena* arena, std::string_view str) {
  char* data =
      static_cast<char*>(CheckAllocated(upb_Arena_Malloc(arena, str.size())));
  std::memcpy(data, str.data(), str.size());
  return upb_StringView_FromDataAndSize(data, str.size());
}

struct ArenaDeleter {
  void operator()(upb_Arena* arena) const { upb_Arena_Free(arena); }
};

}  // namespace

std::string_view GetProtoMessageName(const reflection::Schema* schema) {
  if (!schema->root_table()) {
    throw std::invalid_argument("schema has no root table");
  }
  return schema->root_table()->name()->string_view();
}

std::string GetProtoFileName(std::string_view messageName) {
  std::string fileName{messageName};
  std::replace(fileName.begin(), fileName.end(), '.', '/');
  fileName += ".proto";
  return fileName;
}

namespace {

const reflection::Object& GetObjectAt(const reflection::Schema& schema,
                                      uint32_t index) {
  return *schema.objects()->Get(index);
}

// The objects a message refers to, as indices into the schema's objects().
std::vector<uint32_t> GetReferences(const reflection::Object& object) {
  std::vector<uint32_t> references;
  for (const reflection::Field* field : *object.fields()) {
    if (field->deprecated()) {
      continue;
    }
    const reflection::BaseType type = IsRepeated(*field)
                                          ? field->type()->element()
                                          : field->type()->base_type();
    if (type == reflection::BaseType::Obj) {
      references.push_back(static_cast<uint32_t>(field->type()->index()));
    }
  }
  return references;
}

// The enums a message's fields are, as indices into the schema's enums(). A
// union's type field refers to an enum too, but a union is refused.
std::vector<uint32_t> GetEnumReferences(const reflection::Object& object) {
  std::vector<uint32_t> references;
  for (const reflection::Field* field : *object.fields()) {
    if (field->deprecated()) {
      continue;
    }
    const reflection::BaseType type = IsRepeated(*field)
                                          ? field->type()->element()
                                          : field->type()->base_type();
    if (flatbuffers::IsInteger(type) && type != reflection::BaseType::UType &&
        field->type()->index() >= 0) {
      references.push_back(static_cast<uint32_t>(field->type()->index()));
    }
  }
  return references;
}

// Groups the objects reachable from one into the sets that refer to each other
// in a cycle, with Tarjan's algorithm. A set is finished only after every set
// it refers to, so the sets come out with dependencies first.
class ReferenceCycles {
 public:
  ReferenceCycles(const reflection::Schema& schema, uint32_t root)
      : m_schema{schema},
        m_index(schema.objects()->size(), -1),
        m_lowLink(schema.objects()->size(), 0),
        m_onStack(schema.objects()->size(), false) {
    Visit(root);
  }

  const std::vector<std::vector<uint32_t>>& GetSets() const { return m_sets; }

 private:
  void Visit(uint32_t object) {
    m_index[object] = m_lowLink[object] = m_nextIndex++;
    m_stack.push_back(object);
    m_onStack[object] = true;
    for (uint32_t reference : GetReferences(GetObjectAt(m_schema, object))) {
      if (m_index[reference] < 0) {
        Visit(reference);
        m_lowLink[object] = std::min(m_lowLink[object], m_lowLink[reference]);
      } else if (m_onStack[reference]) {
        m_lowLink[object] = std::min(m_lowLink[object], m_index[reference]);
      }
    }
    if (m_lowLink[object] != m_index[object]) {
      return;
    }
    std::vector<uint32_t>& set = m_sets.emplace_back();
    uint32_t member;
    do {
      member = m_stack.back();
      m_stack.pop_back();
      m_onStack[member] = false;
      set.push_back(member);
    } while (member != object);
  }

  const reflection::Schema& m_schema;
  std::vector<int> m_index;
  std::vector<int> m_lowLink;
  std::vector<bool> m_onStack;
  std::vector<uint32_t> m_stack;
  int m_nextIndex = 0;
  std::vector<std::vector<uint32_t>> m_sets;
};

void AddMessage(google_protobuf_FileDescriptorProto* file, upb_Arena* arena,
                const reflection::Schema& schema,
                const reflection::Object& object) {
  google_protobuf_DescriptorProto* message = CheckAllocated(
      google_protobuf_FileDescriptorProto_add_message_type(file, arena));
  google_protobuf_DescriptorProto_set_name(
      message, MakeStringView(GetLastComponent(object.name()->string_view())));

  std::vector<const reflection::Field*> fields(object.fields()->begin(),
                                               object.fields()->end());
  std::sort(fields.begin(), fields.end(),
            [](const reflection::Field* a, const reflection::Field* b) {
              return a->id() < b->id();
            });

  for (const reflection::Field* field : fields) {
    // A deprecated field holds a place in the numbering and has no
    // counterpart in the proto.
    if (field->deprecated()) {
      continue;
    }
    const upb_FieldType type = GetProtoType(schema, *field);
    google_protobuf_FieldDescriptorProto* out = CheckAllocated(
        google_protobuf_DescriptorProto_add_field(message, arena));
    google_protobuf_FieldDescriptorProto_set_name(
        out, MakeStringView(field->name()->string_view()));
    google_protobuf_FieldDescriptorProto_set_number(
        out, static_cast<int32_t>(GetFieldNumber(*field)));
    google_protobuf_FieldDescriptorProto_set_label(
        out, IsRepeated(*field) ? kUpb_Label_Repeated : kUpb_Label_Optional);
    google_protobuf_FieldDescriptorProto_set_type(out, type);
    if (type == kUpb_FieldType_Message || type == kUpb_FieldType_Enum) {
      const std::string_view typeName =
          type == kUpb_FieldType_Message
              ? GetObjectAt(schema, field->type()->index())
                    .name()
                    ->string_view()
              : schema.enums()
                    ->Get(field->type()->index())
                    ->name()
                    ->string_view();
      google_protobuf_FieldDescriptorProto_set_type_name(
          out, CopyToArena(arena, std::format(".{}", typeName)));
    }
  }
}

void AddEnum(google_protobuf_FileDescriptorProto* file, upb_Arena* arena,
             const reflection::Enum& enumDef) {
  google_protobuf_EnumDescriptorProto* out = CheckAllocated(
      google_protobuf_FileDescriptorProto_add_enum_type(file, arena));
  google_protobuf_EnumDescriptorProto_set_name(
      out, MakeStringView(GetLastComponent(enumDef.name()->string_view())));
  // A proto3 enum's first value has to be 0. reflection lists the values in
  // order, so a negative one can come before it.
  std::vector<const reflection::EnumVal*> values(enumDef.values()->begin(),
                                                 enumDef.values()->end());
  std::stable_partition(
      values.begin(), values.end(),
      [](const reflection::EnumVal* value) { return value->value() == 0; });
  for (const reflection::EnumVal* value : values) {
    google_protobuf_EnumValueDescriptorProto* outValue = CheckAllocated(
        google_protobuf_EnumDescriptorProto_add_value(out, arena));
    google_protobuf_EnumValueDescriptorProto_set_name(
        outValue, MakeStringView(value->name()->string_view()));
    google_protobuf_EnumValueDescriptorProto_set_number(
        outValue, static_cast<int32_t>(value->value()));
  }
}

// Protobuf scopes an enum's values to the enum's package rather than to the
// enum, so a value shares names with the package's messages, its enums and the
// values of its other enums.
void CheckNamesAreUnique(const reflection::Schema& schema,
                         std::span<const uint32_t> objects,
                         std::span<const uint32_t> enums) {
  std::map<std::string, std::string, std::less<>> names;
  const auto add = [&names](std::string name, std::string what) {
    auto [it, inserted] = names.try_emplace(std::move(name), what);
    if (!inserted) {
      throw std::invalid_argument(std::format(
          "{} and {} are both named {} in protobuf, which scopes an enum's "
          "values to its package rather than to the enum",
          it->second, what, it->first));
    }
  };
  for (uint32_t index : objects) {
    const reflection::Object& object = GetObjectAt(schema, index);
    const std::string_view name = object.name()->string_view();
    add(std::string{name},
        std::format("{} {}", object.is_struct() ? "struct" : "table", name));
  }
  for (uint32_t index : enums) {
    const reflection::Enum& enumDef = *schema.enums()->Get(index);
    const std::string_view name = enumDef.name()->string_view();
    add(std::string{name}, std::format("enum {}", name));
    const std::string_view package = GetNamespace(name);
    for (const reflection::EnumVal* value : *enumDef.values()) {
      const std::string_view valueName = value->name()->string_view();
      add(package.empty() ? std::string{valueName}
                          : std::format("{}.{}", package, valueName),
          std::format("value {} of enum {}", valueName, name));
    }
  }
}

// Serializes a file holding the types add() puts in it.
template <typename Add>
ProtoFile MakeProtoFile(std::string fileName, std::string_view package,
                        std::span<const std::string_view> dependencies,
                        Add add) {
  std::unique_ptr<upb_Arena, ArenaDeleter> arena{
      CheckAllocated(upb_Arena_New())};
  google_protobuf_FileDescriptorProto* file =
      CheckAllocated(google_protobuf_FileDescriptorProto_new(arena.get()));
  google_protobuf_FileDescriptorProto_set_name(file, MakeStringView(fileName));
  if (!package.empty()) {
    google_protobuf_FileDescriptorProto_set_package(file,
                                                    MakeStringView(package));
  }
  google_protobuf_FileDescriptorProto_set_syntax(file,
                                                 MakeStringView("proto3"));
  for (std::string_view dependency : dependencies) {
    if (!google_protobuf_FileDescriptorProto_add_dependency(
            file, MakeStringView(dependency), arena.get())) {
      throw std::bad_alloc{};
    }
  }
  add(file, arena.get());

  size_t size = 0;
  const char* serialized = CheckAllocated(
      google_protobuf_FileDescriptorProto_serialize(file, arena.get(), &size));
  return {std::move(fileName),
          std::vector<uint8_t>(serialized, serialized + size)};
}

}  // namespace

std::vector<ProtoFile> BuildFileDescriptorProtos(
    const reflection::Schema* schema) {
  const std::string_view rootName = GetProtoMessageName(schema);
  // A copied schema's root_table() need not be one of its objects(), so find
  // the root among them by name.
  std::optional<uint32_t> root;
  for (uint32_t i = 0; i < schema->objects()->size(); ++i) {
    if (GetObjectAt(*schema, i).name()->string_view() == rootName) {
      root = i;
      break;
    }
  }
  if (!root) {
    throw std::invalid_argument(std::format(
        "the root table {} is not among the schema's objects", rootName));
  }

  const ReferenceCycles cycles{*schema, *root};

  // The tables, structs and enums reachable from the root, as indices into the
  // schema's objects() and enums().
  std::vector<uint32_t> objects;
  std::vector<uint32_t> enums;
  for (const std::vector<uint32_t>& set : cycles.GetSets()) {
    for (uint32_t member : set) {
      objects.push_back(member);
      for (uint32_t reference :
           GetEnumReferences(GetObjectAt(*schema, member))) {
        enums.push_back(reference);
      }
    }
  }
  std::sort(enums.begin(), enums.end());
  enums.erase(std::unique(enums.begin(), enums.end()), enums.end());
  CheckNamesAreUnique(*schema, objects, enums);

  std::vector<ProtoFile> files;

  // An enum refers to nothing, so each is a file of its own, ahead of the
  // messages that use it.
  std::vector<std::string> enumFileOf(schema->enums()->size());
  for (uint32_t index : enums) {
    const reflection::Enum& enumDef = *schema->enums()->Get(index);
    const std::string_view name = enumDef.name()->string_view();
    enumFileOf[index] = GetProtoFileName(name);
    files.push_back(MakeProtoFile(
        enumFileOf[index], GetNamespace(name), {},
        [&](google_protobuf_FileDescriptorProto* file, upb_Arena* arena) {
          AddEnum(file, arena, enumDef);
        }));
  }

  // Each message file is named after its first message by name, so every
  // schema that has the message names its file the same way.
  std::vector<std::string> fileOf(schema->objects()->size());
  for (std::vector<uint32_t> set : cycles.GetSets()) {
    std::sort(set.begin(), set.end(), [&](uint32_t a, uint32_t b) {
      return GetObjectAt(*schema, a).name()->string_view() <
             GetObjectAt(*schema, b).name()->string_view();
    });
    const std::string_view firstName =
        GetObjectAt(*schema, set.front()).name()->string_view();
    const std::string_view package = GetNamespace(firstName);
    for (uint32_t member : set) {
      const std::string_view name =
          GetObjectAt(*schema, member).name()->string_view();
      if (GetNamespace(name) != package) {
        throw std::invalid_argument(std::format(
            "{} and {} refer to each other, so they share a "
            "FileDescriptorProto, but are in different namespaces and a file "
            "has only one package",
            firstName, name));
      }
    }
    const std::string fileName = GetProtoFileName(firstName);
    for (uint32_t member : set) {
      fileOf[member] = fileName;
    }

    std::vector<std::string_view> dependencies;
    for (uint32_t member : set) {
      const reflection::Object& object = GetObjectAt(*schema, member);
      for (uint32_t reference : GetReferences(object)) {
        // Every set a member refers to came out earlier, so its file is known.
        if (fileOf[reference] != fileName) {
          dependencies.push_back(fileOf[reference]);
        }
      }
      for (uint32_t reference : GetEnumReferences(object)) {
        dependencies.push_back(enumFileOf[reference]);
      }
    }
    std::sort(dependencies.begin(), dependencies.end());
    dependencies.erase(std::unique(dependencies.begin(), dependencies.end()),
                       dependencies.end());

    files.push_back(MakeProtoFile(
        fileName, package, dependencies,
        [&](google_protobuf_FileDescriptorProto* file, upb_Arena* arena) {
          for (uint32_t member : set) {
            AddMessage(file, arena, *schema, GetObjectAt(*schema, member));
          }
        }));
  }
  return files;
}

}  // namespace wpi::aosnt
