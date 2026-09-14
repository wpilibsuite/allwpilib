// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/aosnt/FlatbufferToProto.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <format>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
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
upb_FieldType GetProtoType(const reflection::Field& field) {
  const reflection::BaseType type =
      field.type()->base_type() == reflection::BaseType::Vector
          ? field.type()->element()
          : field.type()->base_type();
  const std::string_view attribute = GetProtoTypeAttribute(field);

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

  // The width comes from the flatbuffer type rather than from the attribute,
  // so that sint32 on a long is a sint64 rather than a type the value does not
  // fit in.
  if (attribute == "sint32" || attribute == "sint64") {
    return flatbuffers::IsLong(type) ? kUpb_FieldType_SInt64
                                     : kUpb_FieldType_SInt32;
  }
  if (attribute == "fixed32" || attribute == "sfixed32") {
    return IsSigned(type) ? kUpb_FieldType_SFixed32 : kUpb_FieldType_Fixed32;
  }
  if (attribute == "fixed64" || attribute == "sfixed64") {
    return IsSigned(type) ? kUpb_FieldType_SFixed64 : kUpb_FieldType_Fixed64;
  }
  if (!attribute.empty()) {
    throw std::invalid_argument(std::format(
        "field {} has proto_type \"{}\", which is not one of sint32, sint64, "
        "fixed32, fixed64, sfixed32 or sfixed64",
        field.name()->string_view(), attribute));
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

  bool WriteStruct(pb_ostream_t* stream, const reflection::Object& object,
                   const flatbuffers::Struct& value) const {
    for (const Field& field : GetFields(object)) {
      const uint32_t number = GetFieldNumber(*field.field);
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

    // Scalars are packed: one length-delimited run of the elements.
    pb_ostream_t sizing = PB_OSTREAM_SIZING;
    for (size_t i = 0; i < vector->size(); ++i) {
      WriteScalar(&sizing, field.type,
                  flatbuffers::GetAnyVectorElemI(vector, element, i),
                  flatbuffers::GetAnyVectorElemF(vector, element, i));
    }
    if (!pb_encode_tag(stream, PB_WT_STRING, number) ||
        !pb_encode_varint(stream, sizing.bytes_written)) {
      return false;
    }
    for (size_t i = 0; i < vector->size(); ++i) {
      if (!WriteScalar(stream, field.type,
                       flatbuffers::GetAnyVectorElemI(vector, element, i),
                       flatbuffers::GetAnyVectorElemF(vector, element, i))) {
        return false;
      }
    }
    return true;
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
          // proto3 omits a scalar equal to its default.
          const bool isDefault =
              flatbuffers::IsFloat(field.field->type()->base_type())
                  ? real == field.field->default_real()
                  : integer == field.field->default_integer();
          if (!isDefault &&
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
      if (field->type()->base_type() == reflection::BaseType::Union) {
        throw std::invalid_argument(std::format(
            "field {} of {} is a union, which has no protobuf equivalent",
            field->name()->string_view(), object->name()->string_view()));
      }
      fields.push_back({field, GetProtoType(*field)});
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

std::vector<uint8_t> BuildFileDescriptorProto(const reflection::Schema* schema,
                                              std::string_view fileName) {
  const std::string_view package = GetNamespace(GetProtoMessageName(schema));
  for (const reflection::Object* object : *schema->objects()) {
    // Spanning namespaces would take one file per namespace, with dependency
    // edges between them. Encoding is unaffected, since the wire format has no
    // notion of a package.
    if (GetNamespace(object->name()->string_view()) != package) {
      throw std::invalid_argument(
          std::format("{} is not in the root table's namespace ({}), and a "
                      "FileDescriptorProto has only one package",
                      object->name()->string_view(), package));
    }
  }

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

  for (const reflection::Object* object : *schema->objects()) {
    google_protobuf_DescriptorProto* message =
        CheckAllocated(google_protobuf_FileDescriptorProto_add_message_type(
            file, arena.get()));
    google_protobuf_DescriptorProto_set_name(
        message,
        MakeStringView(GetLastComponent(object->name()->string_view())));

    std::vector<const reflection::Field*> fields(object->fields()->begin(),
                                                 object->fields()->end());
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
      const upb_FieldType type = GetProtoType(*field);
      google_protobuf_FieldDescriptorProto* out = CheckAllocated(
          google_protobuf_DescriptorProto_add_field(message, arena.get()));
      google_protobuf_FieldDescriptorProto_set_name(
          out, MakeStringView(field->name()->string_view()));
      google_protobuf_FieldDescriptorProto_set_number(
          out, static_cast<int32_t>(GetFieldNumber(*field)));
      google_protobuf_FieldDescriptorProto_set_label(
          out, field->type()->base_type() == reflection::BaseType::Vector
                   ? kUpb_Label_Repeated
                   : kUpb_Label_Optional);
      google_protobuf_FieldDescriptorProto_set_type(out, type);
      if (type == kUpb_FieldType_Message) {
        google_protobuf_FieldDescriptorProto_set_type_name(
            out,
            CopyToArena(arena.get(),
                        std::format(".{}", schema->objects()
                                               ->Get(field->type()->index())
                                               ->name()
                                               ->string_view())));
      }
    }
  }

  size_t size = 0;
  const char* serialized = CheckAllocated(
      google_protobuf_FileDescriptorProto_serialize(file, arena.get(), &size));
  return std::vector<uint8_t>(serialized, serialized + size);
}

}  // namespace wpi::aosnt
