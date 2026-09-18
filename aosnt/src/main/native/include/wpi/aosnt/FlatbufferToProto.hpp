// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <flatbuffers/reflection.h>
#include <upb/base/descriptor_constants.h>

#include "wpi/util/DenseMap.hpp"

namespace wpi::aosnt {

/**
 * Encodes flatbuffers as protobuf wire format, driven entirely by the
 * flatbuffer's reflection schema. There is no generated code and nothing to
 * write per message type.
 *
 * Encoding allocates nothing. Everything that needs memory happens in the
 * constructor, and Encode() writes into a buffer the caller owns.
 *
 * The mapping from flatbuffer to protobuf is mechanical, so a schema produces
 * the protobuf it describes. To match an existing .proto, the .fbs has to be
 * written to mirror it:
 *
 * - A field's protobuf number is its flatbuffer id plus one. Where the proto
 *   has a gap, the .fbs reserves the matching id with a deprecated field:
 *
 *       reserved_0:byte (id: 0, deprecated);  // proto field 1, deleted
 *
 * - Anything protobuf packs that flatbuffers does not, such as a bitfield word,
 *   has to be a single scalar field on the flatbuffer side too.
 *
 * - A `proto_type` attribute picks a wire type flatbuffers has no notion of:
 *
 *       axes:[short] (id: 4, proto_type: "sint32");
 *
 *   Accepted values are sint32, sint64, fixed32, fixed64, sfixed32 and
 *   sfixed64, and the value has to match the field's type exactly: the 64-bit
 *   ones on a long or ulong and the 32-bit ones on anything narrower, sint and
 *   sfixed on a signed integer and fixed on an unsigned one. Without it,
 *   integers are plain varints, floats are fixed32, and doubles are fixed64.
 *
 * A vector, or a fixed-length array in a struct, is a repeated field.
 *
 * An enum is a protobuf enum, so it has to meet protobuf's rules: an underlying
 * type no wider than int, a value of 0, and not bit_flags. Protobuf scopes an
 * enum's values to its package rather than to the enum, so two enums in one
 * namespace cannot share a value name, and a value cannot share a table's name.
 *
 * Scalars that are zero are omitted and repeated scalars are packed, matching
 * proto3. A flatbuffer default is not a protobuf default, so a table field left
 * unset at a nonzero default is written with that default.
 */
class FlatbufferToProto {
 public:
  /**
   * Constructs a translator for a schema.
   *
   * @param schema The schema to translate. Must outlive this.
   * @throws std::invalid_argument if the schema is null or has no root table,
   *         or contains something with no protobuf equivalent: a union, an
   *         enum protobuf cannot represent, or a proto_type attribute that is
   *         unknown, on a field with only one protobuf type, or does not match
   *         its field's width and signedness.
   */
  explicit FlatbufferToProto(const reflection::Schema* schema);

  /**
   * Gets the schema being translated.
   *
   * @return schema
   */
  const reflection::Schema* GetSchema() const { return m_schema; }

  /**
   * Gets the number of bytes Encode() would write. Allocates nothing.
   *
   * @param flatbuffer A flatbuffer of the schema's root type.
   * @return encoded size in bytes
   */
  size_t GetEncodedSize(const uint8_t* flatbuffer) const;

  /**
   * Encodes a flatbuffer as protobuf. Allocates nothing.
   *
   * @param flatbuffer A flatbuffer of the schema's root type.
   * @param buffer Where to write the message. Must be at least
   *               GetEncodedSize() bytes.
   * @return number of bytes written, or 0 if the buffer was too small
   */
  size_t Encode(const uint8_t* flatbuffer, std::span<uint8_t> buffer) const;

 private:
  struct Field {
    const reflection::Field* field;
    upb_FieldType type;
  };

  struct Walker;

  const reflection::Schema* m_schema;
  // Each object's fields in protobuf field number order. reflection lists them
  // by name.
  wpi::util::DenseMap<const reflection::Object*, std::vector<Field>> m_fields;
};

/**
 * Gets the protobuf full name of a schema's root table, which is its flatbuffer
 * name unchanged. This is what follows "proto:" in a NetworkTables topic's type
 * string.
 *
 * @param schema schema
 * @return message full name
 * @throws std::invalid_argument if the schema has no root table
 */
std::string_view GetProtoMessageName(const reflection::Schema* schema);

/**
 * Gets the name of the FileDescriptorProto a message or enum is described in:
 * its full name with the package spelled as directories, the way protobuf files
 * are, and ".proto" on the end.
 *
 * @param messageName message or enum full name
 * @return file name
 */
std::string GetProtoFileName(std::string_view messageName);

/** A serialized FileDescriptorProto and the file name recorded in it. */
struct ProtoFile {
  /** The file name, as GetProtoFileName() gives it. */
  std::string name;
  /** The serialized FileDescriptorProto. */
  std::vector<uint8_t> descriptor;
};

/**
 * Builds the serialized FileDescriptorProtos describing a schema's root table
 * and everything it refers to, for NetworkTables' and DataLog's schema
 * registries.
 *
 * Each message and enum is described in a file of its own, named after it,
 * which depends on the files of the messages and enums it refers to. Two
 * schemas that share a message therefore describe it in the same file, which a
 * registry loads once, rather than each defining it again. Tables that refer to
 * each other in a cycle share the file of the first of them by name, since
 * protobuf files cannot depend on each other in a cycle.
 *
 * @param schema schema
 * @return the files, each after every file it depends on
 * @throws std::invalid_argument if the schema has no root table, contains
 *         something with no protobuf equivalent, has tables in different
 *         namespaces that refer to each other in a cycle, or has an enum value
 *         sharing its protobuf name with another value or a type
 */
std::vector<ProtoFile> BuildFileDescriptorProtos(
    const reflection::Schema* schema);

}  // namespace wpi::aosnt
