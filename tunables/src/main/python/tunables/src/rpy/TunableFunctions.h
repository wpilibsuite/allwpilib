#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>

#include "PyTunableTable.h"

namespace wpi::tunables::python {

/**
 * Gets a tunable table relative to the root table.
 *
 * @param name child table name; an empty name returns the root table
 * @return tunable table
 */
wpi::tunables::TunableTable GetTable(std::string_view name = "");

/**
 * Publishes an existing Tunable or complex tunable in the root table.
 * Successful publications retain the value until it is removed.
 *
 * @param name name relative to the root table
 * @param value tunable to publish
 * @return true if the tunable was published, false otherwise
 */
bool Publish(std::string_view name, pybind11::object value);

/**
 * Creates and publishes a tunable in the root table. The tunable type is
 * inferred from value unless explicit type selectors are provided. on_tune runs
 * after an accepted remote update.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param valueType explicit value type, or None to infer it from value
 * @param elementType explicit sequence element type, or None to infer it
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> Add(
    std::string_view name, pybind11::object value,
    pybind11::typing::Optional<table::PythonType> valueType = pybind11::none(),
    pybind11::typing::Optional<table::PythonType> elementType =
        pybind11::none(),
    bool robust = false, bool isMutable = true,
    std::optional<table::TuneCallback> onTune = std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Creates and publishes a boolean tunable in the root table. on_tune receives
 * the boolean value after an accepted remote update.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> AddBoolean(
    std::string_view name, bool value, bool robust = false,
    bool isMutable = true,
    std::optional<table::TypedTuneCallback<table::BoolCallbackValue>> onTune =
        std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Creates and publishes a signed 32-bit integer tunable in the root table.
 * Values outside the native range are rejected.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> AddInt(
    std::string_view name, std::int32_t value, bool robust = false,
    bool isMutable = true,
    std::optional<table::TypedTuneCallback<table::IntCallbackValue>> onTune =
        std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Creates and publishes a signed 64-bit integer tunable in the root table.
 * Values outside the native range are rejected.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> AddLong(
    std::string_view name, std::int64_t value, bool robust = false,
    bool isMutable = true,
    std::optional<table::TypedTuneCallback<table::IntCallbackValue>> onTune =
        std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Creates and publishes a 32-bit floating-point tunable in the root table.
 * on_tune receives the stored float value after an accepted remote update.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> AddFloat(
    std::string_view name, float value, bool robust = false,
    bool isMutable = true,
    std::optional<table::TypedTuneCallback<table::FloatCallbackValue>> onTune =
        std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Creates and publishes a 64-bit floating-point tunable in the root table.
 * on_tune receives the stored float value after an accepted remote update.
 *
 * @param name name relative to the root table
 * @param value initial value
 * @param robust whether to separately echo a remotely set value
 * @param isMutable whether remote updates may change the tunable
 * @param onTune callback that receives the value after a remote update
 * @param properties additional tunable properties
 * @param typeString custom tunable type string
 * @return created tunable
 */
std::shared_ptr<PyTunable> AddDouble(
    std::string_view name, double value, bool robust = false,
    bool isMutable = true,
    std::optional<table::TypedTuneCallback<table::FloatCallbackValue>> onTune =
        std::nullopt,
    std::optional<table::Properties> properties = std::nullopt,
    std::string typeString = "");

/**
 * Removes the named tunable and its descendants from the root table, releasing
 * retained Python values and callbacks.
 *
 * @param name name relative to the root table
 */
void Remove(std::string_view name);

}  // namespace wpi::tunables::python
