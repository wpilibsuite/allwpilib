#include "MockTunableBackendFunctions.h"

#include <stdint.h>

#include <concepts>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <pybind11/stl.h>
#include <wpybuffer.h>

#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/detail/TunableDetail.hpp"
#include "wpystruct.h"

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

bool IsWpiStruct(py::handle value) {
  return py::hasattr(py::type::of(value), "WPIStruct");
}

std::vector<uint8_t> ToRawVector(py::handle value) {
  std::vector<uint8_t> data;
  for (py::handle item : py::reinterpret_borrow<py::iterable>(value)) {
    int integer = item.cast<int>();
    if (integer < 0 || integer > 255) {
      throw py::value_error("raw tunable values must be in range 0-255");
    }
    data.emplace_back(static_cast<uint8_t>(integer));
  }
  return data;
}

template <typename T, typename Iterable>
std::vector<T> ToVector(const Iterable& value) {
  std::vector<T> data;
  for (py::handle item : value) {
    data.emplace_back(item.cast<T>());
  }
  return data;
}

std::vector<WPyStruct> ToStructVector(StructIterable value,
                                      WPyStructInfo& info) {
  std::vector<py::object> values;
  for (py::handle item : value) {
    values.emplace_back(py::reinterpret_borrow<py::object>(item));
  }
  if (values.empty()) {
    throw py::value_error("struct tunable arrays require at least one value");
  }
  if (!IsWpiStruct(values.front())) {
    throw py::type_error("struct tunable arrays require WPIStruct values");
  }

  py::type type = py::type::of(values.front());
  std::vector<WPyStruct> data;
  data.reserve(values.size());
  for (auto&& item : values) {
    if (!py::type::of(item).is(type)) {
      throw py::type_error("struct tunable arrays require one WPIStruct type");
    }
    data.emplace_back(std::move(item));
  }
  info = WPyStructInfo{std::move(type)};
  return data;
}

template <typename T>
py::object ValueToPython(const T& value) {
  return py::cast(value);
}

py::bytes ValueToPython(const std::vector<uint8_t>& value) {
  return py::bytes{reinterpret_cast<const char*>(value.data()), value.size()};
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

py::bytes ReadStructValue(
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
            RawValue value) {
  if (wpi::util::python::IsBytesLike(value)) {
    auto raw = wpi::util::python::RequestContiguousBuffer(
        py::reinterpret_borrow<py::buffer>(value));
    self.SetRaw(path,
                std::span<const uint8_t>{static_cast<const uint8_t*>(raw.ptr),
                                         static_cast<size_t>(raw.view()->len)});
  } else {
    auto raw = ToRawVector(value);
    self.SetRaw(path, std::span<const uint8_t>{raw});
  }
}

void SetStruct(wpi::tunables::MockTunableBackend& self, std::string_view path,
               py::handle value) {
  WPyStructInfo info{py::type::of(value)};
  self.SetStruct<WPyStruct, WPyStructInfo>(
      path, WPyStruct{py::reinterpret_borrow<py::object>(value)},
      std::move(info));
}

void SetStructVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, StructIterable value) {
  WPyStructInfo info;
  auto data = ToStructVector(std::move(value), info);
  self.SetStructVector<WPyStruct, WPyStructInfo>(
      path, std::span<const WPyStruct>{data}, std::move(info));
}

void SetBoolVector(wpi::tunables::MockTunableBackend& self,
                   std::string_view path, py::typing::Iterable<bool> value) {
  self.SetBoolVector(path, ToVector<bool>(value));
}

void SetInt32Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path,
                    py::typing::Iterable<int32_t> value) {
  auto data = ToVector<int32_t>(value);
  self.SetInt32Vector(path, std::span<const int32_t>{data});
}

void SetInt64Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path,
                    py::typing::Iterable<int64_t> value) {
  auto data = ToVector<int64_t>(value);
  self.SetInt64Vector(path, std::span<const int64_t>{data});
}

void SetFloatVector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, py::typing::Iterable<float> value) {
  auto data = ToVector<float>(value);
  self.SetFloatVector(path, std::span<const float>{data});
}

void SetDoubleVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path,
                     py::typing::Iterable<double> value) {
  auto data = ToVector<double>(value);
  self.SetDoubleVector(path, std::span<const double>{data});
}

void SetStringVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path,
                     py::typing::Iterable<std::string> value) {
  auto data = ToVector<std::string>(value);
  self.SetStringVector(path, std::span<const std::string>{data});
}

std::optional<uint32_t> GetUid(const wpi::tunables::MockTunableBackend& self,
                               std::string_view path) {
  return self.GetUid(path);
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
