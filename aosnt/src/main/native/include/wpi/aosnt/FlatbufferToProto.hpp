// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <span>
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
 *   sfixed64. Without it, integers are plain varints, floats are fixed32, and
 *   doubles are fixed64.
 *
 * Scalars equal to their default are omitted and repeated scalars are packed,
 * matching proto3.
 */
class FlatbufferToProto {
 public:
  /**
   * Constructs a translator for a schema.
   *
   * @param schema The schema to translate. Must outlive this.
   * @throws std::invalid_argument if the schema is null or has no root table,
   *         or contains something with no protobuf equivalent: a union, or a
   *         proto_type attribute that is unknown or on a field with only one
   *         protobuf type.
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
 * Builds the serialized FileDescriptorProto describing a schema, for
 * NetworkTables' and DataLog's schema registries.
 *
 * Every object in the schema has to share the root table's namespace, since a
 * FileDescriptorProto has exactly one package.
 *
 * @param schema schema
 * @param fileName The file name to record in the descriptor.
 * @return serialized FileDescriptorProto
 * @throws std::invalid_argument if the schema has no root table, spans
 *         namespaces, or contains something with no protobuf equivalent
 */
std::vector<uint8_t> BuildFileDescriptorProto(const reflection::Schema* schema,
                                              std::string_view fileName);

}  // namespace wpi::aosnt
