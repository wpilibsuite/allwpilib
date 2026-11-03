// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <initializer_list>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::telemetry {

/**
 * A telemetry backend that forwards logged data to multiple child backends.
 */
class MultiTelemetryBackend : public TelemetryBackend {
 public:
  /** Constructs a telemetry backend with no child backends. */
  MultiTelemetryBackend();

  /**
   * Constructs a telemetry backend that forwards to multiple child backends.
   *
   * @param backends child backends
   */
  explicit MultiTelemetryBackend(
      std::span<const std::shared_ptr<TelemetryBackend>> backends);

  /**
   * Constructs a telemetry backend that forwards to multiple child backends.
   *
   * @param backends child backends
   */
  explicit MultiTelemetryBackend(
      std::vector<std::shared_ptr<TelemetryBackend>> backends);

  /**
   * Constructs a telemetry backend that forwards to multiple child backends.
   *
   * @param backends child backends
   */
  MultiTelemetryBackend(
      std::initializer_list<std::shared_ptr<TelemetryBackend>> backends);

  ~MultiTelemetryBackend() override;

  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  std::shared_ptr<TelemetryEntry> GetEntry(std::string_view path) override;

  /**
   * Removes an entry for the given path.
   *
   * @param path normalized full name
   */
  void RemoveEntry(std::string_view path) override;

  /**
   * Returns whether every child backend has a data schema already registered
   * with the given name.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @return True if every child backend already has the schema
   */
  bool HasSchema(std::string_view schemaName) const override;

  /**
   * Registers a data schema with every child backend.
   *
   * @param schemaName Name (the string passed as the data type for topics using
   * this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override;

  /**
   * Registers a data schema with every child backend.
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

  std::vector<std::shared_ptr<TelemetryBackend>> m_backends;
  wpi::util::mutex m_mutex;
  wpi::util::StringMap<std::shared_ptr<Entry>> m_entries;
};

}  // namespace wpi::telemetry
