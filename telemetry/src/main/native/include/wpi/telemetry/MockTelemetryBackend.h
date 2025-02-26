// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <variant>
#include <vector>

#include <wpi/mutex.h>
#include <wpi/StringMap.h>

#include "wpi/telemetry/TelemetryBackend.h"

namespace wpi {

/**
 * A telemetry backend that saves logged data to an array for unit testing
 * purposes.
 */
class MockTelemetryBackend : public TelemetryBackend {
 public:
  /** Value for KeepDuplicate() action. */
  struct KeepDuplicatesValue {
    bool value = true;
  };

  /** Value for SetProperty() action. */
  struct SetPropertyValue {
    std::string key;
    std::string value;
  };

  /** Value for LogString() action. */
  struct LogStringValue {
    std::string value;
    std::string typeString;
  };

  /** Value for LogBooleanArray() action. */
  struct LogBooleanArrayValue {
    std::vector<int> value;
  };

  /** Value for LogRaw() action. */
  struct LogRawValue {
    std::vector<uint8_t> value;
    std::string typeString;
  };

  /** A logged action. */
  struct Action {
    template <typename T>
    Action(std::string_view path_, T value_)
        : path{path_}, value{std::move(value_)} {}

    std::string path;
    std::variant<KeepDuplicatesValue, SetPropertyValue, bool, int16_t, int32_t,
                 int64_t, float, double, LogStringValue, LogBooleanArrayValue,
                 std::vector<int16_t>, std::vector<int32_t>,
                 std::vector<int64_t>, std::vector<float>, std::vector<double>,
                 std::vector<std::string>, LogRawValue>
        value;
  };

  struct Schema {
    std::string type;
    std::vector<uint8_t> schemaBytes;
    std::string schemaString;
  };

  MockTelemetryBackend();

  ~MockTelemetryBackend() override;

  /**
   * Get logged actions.
   *
   * @return Logged actions
   */
  const std::vector<Action>& GetActions() const { return m_actions; }

  /**
   * Clear logged actions.
   */
  void ClearActions() { m_actions.clear(); }

  /**
   * Get schema.
   *
   * @param schemaName schema name
   * @return Schema, or nullptr if no match
   */
  const Schema* GetSchema(std::string_view schemaName) {
    std::scoped_lock lock{m_mutex};
    auto it = m_schemas.find(schemaName);
    if (it == m_schemas.end()) {
      return nullptr;
    }
    return &it->second;
  }

  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  TelemetryEntry& GetEntry(std::string_view path) override;

  /**
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function
   */
  void SetReportWarning(
      std::function<void(std::string_view msg)> func) override;

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
  wpi::mutex m_mutex;
  wpi::StringMap<Entry> m_entries;
  wpi::StringMap<Schema> m_schemas;
  std::vector<Action> m_actions;
};

}  // namespace wpi
