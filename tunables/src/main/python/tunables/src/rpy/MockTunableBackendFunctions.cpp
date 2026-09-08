#include "MockTunableBackendFunctions.h"

#include <stdint.h>

#include <concepts>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <wpi/tunables/TunableConfig.hpp>
#include <wpi/tunables/TunableRegistry.hpp>
#include <wpi/tunables/detail/TunableBase.hpp>
#include <wpi/tunables/detail/TunableDetail.hpp>
#include <wpystruct.h>

#include "TunableValuePython.h"

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

template <typename T, typename F>
void SetVector(wpi::tunables::MockTunableBackend& self, std::string_view path,
               const py::sequence& value, F setter) {
  std::vector<T> data;
  const size_t size = py::len(value);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    data.emplace_back(value[static_cast<py::ssize_t>(i)].cast<T>());
  }
  (self.*setter)(path, std::span<const T>{data.data(), data.size()});
}

template <typename T>
py::object ValueToPython(const T& value) {
  if constexpr (std::same_as<T, std::vector<uint8_t>>) {
    return py::bytes{reinterpret_cast<const char*>(value.data()), value.size()};
  } else {
    return py::cast(value);
  }
}

template <typename T>
py::object ReadTunableValue(
    const wpi::tunables::TunableRegistry::TunableInfo& info) {
  if (auto v = wpi::tunables::detail::CastTunable<T, false>(info.tunable,
                                                            info.type)) {
    return ValueToPython(v->Get());
  }
  if (auto v = wpi::tunables::detail::CastTunable<T, true>(info.tunable,
                                                           info.type)) {
    return ValueToPython(v->Get(info.config->parent));
  }
  throw py::type_error("tunable has unexpected type");
}

py::object ReadStructValue(
    const wpi::tunables::TunableRegistry::TunableInfo& info) {
  if (auto v = wpi::tunables::detail::CastTunable<
          wpi::tunables::detail::TunableStructTag, false>(info.tunable,
                                                          info.type)) {
    std::vector<uint8_t> data(v->GetStructSize());
    v->PackStruct(data);
    return py::bytes{reinterpret_cast<const char*>(data.data()), data.size()};
  }
  if (auto v = wpi::tunables::detail::CastTunable<
          wpi::tunables::detail::TunableStructTag, true>(info.tunable,
                                                         info.type)) {
    std::vector<uint8_t> data(v->GetStructSize(info.config->parent));
    v->PackStruct(info.config->parent, data);
    return py::bytes{reinterpret_cast<const char*>(data.data()), data.size()};
  }
  throw py::type_error("tunable has unexpected type");
}

}  // namespace

void SetRaw(wpi::tunables::MockTunableBackend& self, std::string_view path,
            py::handle value) {
  auto raw = ToRawVector(value);
  self.SetRaw(path, std::span<const uint8_t>{raw.data(), raw.size()});
}

void SetStruct(wpi::tunables::MockTunableBackend& self, std::string_view path,
               py::handle value) {
  WPyStructInfo info{py::type::of(value)};
  self.SetStruct<WPyStruct, WPyStructInfo>(
      path, WPyStruct{py::reinterpret_borrow<py::object>(value)},
      std::move(info));
}

void SetStructVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const py::sequence& value) {
  WPyStructInfo info{GetStructSequenceType(value)};
  auto data = ToStructVector(value);
  self.SetStructVector<WPyStruct, WPyStructInfo>(
      path, std::span<const WPyStruct>{data.data(), data.size()},
      std::move(info));
}

void SetBoolVector(wpi::tunables::MockTunableBackend& self,
                   std::string_view path, const py::sequence& value) {
  const size_t size = py::len(value);
  auto data = std::make_unique<bool[]>(size);
  for (size_t i = 0; i < size; ++i) {
    data[i] = value[static_cast<py::ssize_t>(i)].cast<bool>();
  }
  self.SetBoolVector(path, std::span<const bool>{data.get(), size});
}

void SetInt32Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const py::sequence& value) {
  SetVector<int32_t>(self, path, value,
                     &wpi::tunables::MockTunableBackend::SetInt32Vector);
}

void SetInt64Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const py::sequence& value) {
  SetVector<int64_t>(self, path, value,
                     &wpi::tunables::MockTunableBackend::SetInt64Vector);
}

void SetFloatVector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const py::sequence& value) {
  SetVector<float>(self, path, value,
                   &wpi::tunables::MockTunableBackend::SetFloatVector);
}

void SetDoubleVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const py::sequence& value) {
  SetVector<double>(self, path, value,
                    &wpi::tunables::MockTunableBackend::SetDoubleVector);
}

void SetStringVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const py::sequence& value) {
  SetVector<std::string>(self, path, value,
                         &wpi::tunables::MockTunableBackend::SetStringVector);
}

py::object GetUid(const wpi::tunables::MockTunableBackend& self,
                  std::string_view path) {
  if (auto uid = self.GetUid(path)) {
    return py::int_{*uid};
  }
  return py::none{};
}

py::object GetTunableValue(const wpi::tunables::MockTunableBackend& self,
                           std::string_view path) {
  auto uid = self.GetUid(path);
  if (!uid) {
    throw py::value_error("no tunable at path");
  }

  auto info = wpi::tunables::TunableRegistry::GetTunable(*uid);
  if (!info) {
    throw py::value_error("no registered tunable for path");
  }

  using Type = wpi::tunables::detail::TunableTypeValue;
  switch (info.type) {
    case Type::BOOLEAN:
    case Type::MEMBER_BOOLEAN:
      return ReadTunableValue<bool>(info);
    case Type::INT32:
    case Type::MEMBER_INT32:
      return ReadTunableValue<int32_t>(info);
    case Type::INT64:
    case Type::MEMBER_INT64:
      return ReadTunableValue<int64_t>(info);
    case Type::FLOAT:
    case Type::MEMBER_FLOAT:
      return ReadTunableValue<float>(info);
    case Type::DOUBLE:
    case Type::MEMBER_DOUBLE:
      return ReadTunableValue<double>(info);
    case Type::STRING:
    case Type::MEMBER_STRING:
      return ReadTunableValue<std::string>(info);
    case Type::RAW:
    case Type::MEMBER_RAW:
      return ReadTunableValue<std::vector<uint8_t>>(info);
    case Type::BOOLEAN_ARRAY:
    case Type::MEMBER_BOOLEAN_ARRAY:
      return ReadTunableValue<std::vector<bool>>(info);
    case Type::INT32_ARRAY:
    case Type::MEMBER_INT32_ARRAY:
      return ReadTunableValue<std::vector<int32_t>>(info);
    case Type::INT64_ARRAY:
    case Type::MEMBER_INT64_ARRAY:
      return ReadTunableValue<std::vector<int64_t>>(info);
    case Type::FLOAT_ARRAY:
    case Type::MEMBER_FLOAT_ARRAY:
      return ReadTunableValue<std::vector<float>>(info);
    case Type::DOUBLE_ARRAY:
    case Type::MEMBER_DOUBLE_ARRAY:
      return ReadTunableValue<std::vector<double>>(info);
    case Type::STRING_ARRAY:
    case Type::MEMBER_STRING_ARRAY:
      return ReadTunableValue<std::vector<std::string>>(info);
    case Type::STRUCT:
    case Type::MEMBER_STRUCT:
      return ReadStructValue(info);
    default:
      return py::none{};
  }
}

}  // namespace wpi::tunables::python
