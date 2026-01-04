// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string_view>

#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

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
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function; pass nullptr to use default
   */
  static void SetReportWarning(
      std::function<void(std::string_view path, std::string_view msg)> func);

  /**
   * Get function used for reporting warning messages.
   *
   * @return reporting function
   */
  static std::function<void(std::string_view path, std::string_view msg)>
  GetReportWarning();

  /**
   * Report a warning message (e.g. type mismatch).
   *
   * @param path entry path
   * @param msg warning message
   */
  static void ReportWarning(std::string_view path, std::string_view msg);

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
   * @param backend Backend
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
   * @param backend Backend
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
   * @param backend Backend
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
   * @param backend Backend
   * @param msg protobuf message
   */
  template <wpi::util::ProtobufSerializable T>
  static void AddProtobufSchema(TelemetryBackend& backend,
                                wpi::util::ProtobufMessage<T>& msg) {
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
   * @param backend Backend
   * @param info optional struct type info
   */
  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  static void AddStructSchema(TelemetryBackend& backend, const I&... info) {
    wpi::util::ForEachStructSchema<T>(
        [&](auto typeString, auto schema) {
          AddSchema(backend, typeString, "structschema", schema);
        },
        info...);
  }
};

}  // namespace wpi
