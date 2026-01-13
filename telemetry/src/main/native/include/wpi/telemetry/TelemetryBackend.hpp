// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string_view>

namespace wpi {

class TelemetryEntry;

/** Interface for telemetry backends. */
class TelemetryBackend {
 public:
  TelemetryBackend() = default;
  virtual ~TelemetryBackend() = default;
  TelemetryBackend(const TelemetryBackend&) = delete;
  TelemetryBackend& operator=(const TelemetryBackend&) = delete;

  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  virtual TelemetryEntry& GetEntry(std::string_view path) = 0;

  /**
   * Returns whether there is a data schema already registered with the given
   * name. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @return True if schema already registered
   */
  virtual bool HasSchema(std::string_view schemaName) const = 0;

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
  virtual void AddSchema(std::string_view schemaName, std::string_view type,
                         std::span<const uint8_t> schema) = 0;

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
  virtual void AddSchema(std::string_view schemaName, std::string_view type,
                         std::string_view schema) = 0;
};

}  // namespace wpi
