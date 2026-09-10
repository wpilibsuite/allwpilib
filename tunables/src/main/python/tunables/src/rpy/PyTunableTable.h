#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "PyTunable.h"
#include "wpi/tunables/TunableTable.hpp"

namespace wpi::tunables::python {

class PyComplexTunableAdapter;

struct TunableTableOwnerContext {
  std::weak_ptr<PyComplexTunableAdapter> owner;
};

namespace table {

using Getter = PyTunable::Getter;
using Setter = PyTunable::Setter;
using TuneCallback = PyTunable::TuneCallback;
using PythonType = PyTunable::PythonType;
using Properties = PyTunable::Properties;

using BoolCallbackValue = pybind11::bool_;
using IntCallbackValue = pybind11::int_;
using FloatCallbackValue = pybind11::float_;
using StringCallbackValue = pybind11::str;
using BytesCallbackValue = pybind11::bytes;

template <typename T>
using TypedGetter = pybind11::typing::Callable<T()>;
template <typename T>
using TypedSetter = pybind11::typing::Callable<void(T)>;
template <typename T>
using TypedTuneCallback = pybind11::typing::Callable<void(T)>;
template <typename T>
using ArrayGetter = TypedGetter<pybind11::typing::List<T>>;
template <typename T>
using ArraySetter = TypedSetter<pybind11::typing::List<T>>;

pybind11::object MakePythonTable(
    wpi::tunables::TunableTable table,
    std::shared_ptr<TunableTableOwnerContext> ownerContext = nullptr);
pybind11::object GetTable(wpi::tunables::TunableTable& table,
                          std::string_view name);
bool Publish(wpi::tunables::TunableTable& table, std::string_view name,
             pybind11::object value);
std::shared_ptr<PyTunable> Add(
    wpi::tunables::TunableTable& table, std::string_view name,
    pybind11::object value,
    pybind11::typing::Optional<PythonType> valueType = pybind11::none(),
    pybind11::typing::Optional<PythonType> elementType = pybind11::none(), bool robust = false,
    bool isMutable = true,
    std::optional<TuneCallback> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> AddBoolean(
    wpi::tunables::TunableTable& table, std::string_view name, bool value,
    bool robust = false, bool isMutable = true,
    std::optional<TypedTuneCallback<BoolCallbackValue>> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> AddInt(
    wpi::tunables::TunableTable& table, std::string_view name,
    std::int32_t value, bool robust = false, bool isMutable = true,
    std::optional<TypedTuneCallback<IntCallbackValue>> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> AddLong(
    wpi::tunables::TunableTable& table, std::string_view name,
    std::int64_t value, bool robust = false, bool isMutable = true,
    std::optional<TypedTuneCallback<IntCallbackValue>> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> AddFloat(
    wpi::tunables::TunableTable& table, std::string_view name, float value,
    bool robust = false, bool isMutable = true,
    std::optional<TypedTuneCallback<FloatCallbackValue>> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> AddDouble(
    wpi::tunables::TunableTable& table, std::string_view name, double value,
    bool robust = false, bool isMutable = true,
    std::optional<TypedTuneCallback<FloatCallbackValue>> onTune = std::nullopt,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishValue(
    wpi::tunables::TunableTable& table, std::string_view name, Getter getter,
    Setter setter, pybind11::typing::Optional<PythonType> valueType = pybind11::none(),
    pybind11::typing::Optional<PythonType> elementType = pybind11::none(), bool robust = false,
    bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishBoolean(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<BoolCallbackValue> getter,
    TypedSetter<BoolCallbackValue> setter, bool robust = false,
    bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishInt(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<IntCallbackValue> getter, TypedSetter<IntCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishLong(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<IntCallbackValue> getter, TypedSetter<IntCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishFloat(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<FloatCallbackValue> getter,
    TypedSetter<FloatCallbackValue> setter, bool robust = false,
    bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishDouble(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<FloatCallbackValue> getter,
    TypedSetter<FloatCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishString(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<StringCallbackValue> getter,
    TypedSetter<StringCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishBooleanArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<BoolCallbackValue> getter,
    ArraySetter<BoolCallbackValue> setter, bool robust = false,
    bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishIntegerArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<IntCallbackValue> getter, ArraySetter<IntCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishDoubleArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<FloatCallbackValue> getter,
    ArraySetter<FloatCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishStringArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<StringCallbackValue> getter,
    ArraySetter<StringCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
std::shared_ptr<PyTunable> PublishRaw(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<BytesCallbackValue> getter,
    TypedSetter<BytesCallbackValue> setter,
    bool robust = false, bool isMutable = true,
    std::optional<Properties> properties = std::nullopt,
    std::string typeString = "");
void Remove(wpi::tunables::TunableTable& table, std::string_view name);
void InvalidatePendingPublications(std::string_view path);
void ClearContexts();

}  // namespace table
}  // namespace wpi::tunables::python
