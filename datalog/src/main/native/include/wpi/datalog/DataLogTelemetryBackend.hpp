// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/telemetry/TelemetryBackend.hpp"

namespace wpi::log {

class DataLog;

/** A telemetry backend that sends logged data to a DataLog. */
class DataLogTelemetryBackend : public wpi::TelemetryBackend {
 public:
  /**
   * Construct.
   *
   * @param log datalog
   * @param prefix prefix to put in front of logged path in data log
   */
  DataLogTelemetryBackend(DataLog& log, std::string_view prefix);

  ~DataLogTelemetryBackend() override;

  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  wpi::TelemetryEntry& GetEntry(std::string_view path) override;

  /**
   * Returns whether there is a data schema already registered with the given
   * name. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @return True if schema already registered
   */
  bool HasSchema(std::string_view schemaName) const override;

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
  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override;

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
  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override;

 private:
  class Entry;

  DataLog& m_log;
  std::string m_prefix;
  wpi::util::mutex m_mutex;
  wpi::util::StringMap<Entry> m_entries;
};

}  // namespace wpi::log
