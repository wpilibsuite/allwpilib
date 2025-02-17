// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string_view>

#include "wpi/protobuf/Protobuf.h"
#include "wpi/struct/Struct.h"

namespace wpi {

class TelemetryBackend;
class TelemetryEntry;
class TelemetryTable;

/**
 * Global registry for telemetry handlers (type handlers and telemetry
 * backends).
 */
class TelemetryRegistry final {
 public:
  TelemetryRegistry() = delete;

  /**
   * Registers a backend for creating telemetry entries. When calling
   * GetEntry(), the longest prefix match is used.
   *
   * @param prefix prefix for entries covered by this backend
   * @param backend backend
   */
  static void RegisterBackend(std::string_view prefix,
                              std::shared_ptr<TelemetryBackend> backend);

  /**
   * Gets the backend for logging an object. Should generally only be used
   * internally or by custom backends.
   *
   * @param path full name
   * @return telemetry backend, or null if no match
   */
  static std::shared_ptr<TelemetryBackend> GetBackend(std::string_view path);

  /**
   * Get a telemetry entry for a particular name.
   *
   * @param path full name
   * @return telemetry entry
   */
  static TelemetryEntry& GetEntry(std::string_view path);

  /**
   * Get a telemetry table for a particular name.
   *
   * @param path full name
   * @return telemetry table
   */
  static TelemetryTable& GetTable(std::string_view path);

  /**
   * Clear all registered types and backends and closes all entries. Should
   * typically only be used by unit test code.
   */
  static void Reset();

  /**
   * Returns whether there is a data schema already registered with the given
   * name. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @return True if schema already registered
   */
  static bool HasSchema(TelemetryBackend& backend, std::string_view schemaName);

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc). In NetworkTables, schemas are
   * published just like normal topics, with the name being generated from the
   * provided name: "/.schema/<name>".  Duplicate calls to this function with
   * the same name are silently ignored.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  static void AddSchema(TelemetryBackend& backend, std::string_view schemaName,
                        std::string_view type, std::span<const uint8_t> schema);

  /**
   * Registers a data schema.  Data schemas provide information for how a
   * certain data type string can be decoded.  The type string of a data schema
   * indicates the type of the schema itself (e.g. "protobuf" for protobuf
   * schemas, "struct" for struct schemas, etc). In NetworkTables, schemas are
   * published just like normal topics, with the name being generated from the
   * provided name: "/.schema/<name>".  Duplicate calls to this function with
   * the same name are silently ignored.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  static void AddSchema(TelemetryBackend& backend, std::string_view schemaName,
                        std::string_view type, std::string_view schema);

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T protobuf serializable type
   * @param msg protobuf message
   */
  template <wpi::ProtobufSerializable T>
  static void AddProtobufSchema(TelemetryBackend& backend,
                                wpi::ProtobufMessage<T>& msg) {
    msg.ForEachProtobufDescriptor(
        [&](auto typeString) { return HasSchema(backend, typeString); },
        [&](auto typeString, auto schema) {
          AddSchema(backend, typeString, "proto:FileDescriptorProto", schema);
        });
  }

  /**
   * Registers a struct schema. Duplicate calls to this function with the same
   * name are silently ignored.
   *
   * @tparam T struct serializable type
   * @param info optional struct type info
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  static void AddStructSchema(TelemetryBackend& backend, const I&... info) {
    wpi::ForEachStructSchema<T>(
        [&](auto typeString, auto schema) {
          AddSchema(backend, typeString, "structschema", schema);
        },
        info...);
  }
};

}  // namespace wpi
