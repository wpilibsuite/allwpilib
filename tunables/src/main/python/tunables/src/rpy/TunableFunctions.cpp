#include "TunableFunctions.h"

#include <memory>
#include <string>
#include <utility>

#include "wpi/tunables/Tunables.hpp"

namespace wpi::tunables::python {
namespace {

wpi::tunables::TunableTable RootTable() {
  return wpi::tunables::GetTable();
}

}  // namespace

wpi::tunables::TunableTable GetTable(std::string_view name) {
  auto root = RootTable();
  return name.empty() ? std::move(root) : root.GetTable(name);
}

bool Publish(std::string_view name, pybind11::object value) {
  auto root = RootTable();
  return table::Publish(root, name, std::move(value));
}

std::shared_ptr<PyTunable> Add(
    std::string_view name, pybind11::object value,
    pybind11::typing::Optional<table::PythonType> valueType,
    pybind11::typing::Optional<table::PythonType> elementType, bool robust,
    bool isMutable, std::optional<table::TuneCallback> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::Add(root, name, std::move(value), std::move(valueType),
                    std::move(elementType), robust, isMutable,
                    std::move(onTune), std::move(properties),
                    std::move(typeString));
}

std::shared_ptr<PyTunable> AddBoolean(
    std::string_view name, bool value, bool robust, bool isMutable,
    std::optional<table::TypedTuneCallback<table::BoolCallbackValue>> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::AddBoolean(root, name, value, robust, isMutable,
                           std::move(onTune), std::move(properties),
                           std::move(typeString));
}

std::shared_ptr<PyTunable> AddInt(
    std::string_view name, std::int32_t value, bool robust, bool isMutable,
    std::optional<table::TypedTuneCallback<table::IntCallbackValue>> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::AddInt(root, name, value, robust, isMutable, std::move(onTune),
                       std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> AddLong(
    std::string_view name, std::int64_t value, bool robust, bool isMutable,
    std::optional<table::TypedTuneCallback<table::IntCallbackValue>> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::AddLong(root, name, value, robust, isMutable, std::move(onTune),
                        std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> AddFloat(
    std::string_view name, float value, bool robust, bool isMutable,
    std::optional<table::TypedTuneCallback<table::FloatCallbackValue>> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::AddFloat(root, name, value, robust, isMutable,
                         std::move(onTune), std::move(properties),
                         std::move(typeString));
}

std::shared_ptr<PyTunable> AddDouble(
    std::string_view name, double value, bool robust, bool isMutable,
    std::optional<table::TypedTuneCallback<table::FloatCallbackValue>> onTune,
    std::optional<table::Properties> properties, std::string typeString) {
  auto root = RootTable();
  return table::AddDouble(root, name, value, robust, isMutable,
                          std::move(onTune), std::move(properties),
                          std::move(typeString));
}

void Remove(std::string_view name) {
  auto root = RootTable();
  table::Remove(root, name);
}

}  // namespace wpi::tunables::python
