#pragma once

#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "MockTelemetryBackendFunctions.h"

namespace pybind11::detail {

template <>
struct type_caster<
    wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue> {
  using Type = wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue;

  PYBIND11_TYPE_CASTER(
      Type,
      const_name("telemetry.mock_backend.KeepDuplicatesValue"));

  bool load(handle src, bool) {
    if (!src) {
      return false;
    }

    try {
      object source = reinterpret_borrow<object>(src);
      object pyValue = source.attr("value");
      value.value = pybind11::cast<bool>(pyValue);
      return true;
    } catch (error_already_set& error) {
      if (!error.matches(PyExc_AttributeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      return false;
    } catch (const cast_error&) {
      return false;
    }
  }

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::KEEP_DUPLICATES);
    return cls(src.value).release();
  }
};

template <>
struct type_caster<wpi::telemetry::MockTelemetryBackend::SetPropertyValue> {
  using Type = wpi::telemetry::MockTelemetryBackend::SetPropertyValue;

  PYBIND11_TYPE_CASTER(
      Type, const_name("telemetry.mock_backend.SetPropertyValue"));

  bool load(handle src, bool) {
    if (!src) {
      return false;
    }

    try {
      object source = reinterpret_borrow<object>(src);
      object key = source.attr("key");
      object pyValue = source.attr("value");
      value.key = pybind11::cast<std::string>(key);
      value.value = pybind11::cast<std::string>(pyValue);
      return true;
    } catch (error_already_set& error) {
      if (!error.matches(PyExc_AttributeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      return false;
    } catch (const cast_error&) {
      return false;
    }
  }

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::SET_PROPERTY);
    return cls(src.key, src.value).release();
  }
};

template <>
struct type_caster<wpi::telemetry::MockTelemetryBackend::LogStringValue> {
  using Type = wpi::telemetry::MockTelemetryBackend::LogStringValue;

  PYBIND11_TYPE_CASTER(
      Type, const_name("telemetry.mock_backend.LogStringValue"));

  bool load(handle src, bool) {
    if (!src) {
      return false;
    }

    try {
      object source = reinterpret_borrow<object>(src);
      object pyValue = source.attr("value");
      object typeString = source.attr("type_string");
      value.value = pybind11::cast<std::string>(pyValue);
      value.typeString = pybind11::cast<std::string>(typeString);
      return true;
    } catch (error_already_set& error) {
      if (!error.matches(PyExc_AttributeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      return false;
    } catch (const cast_error&) {
      return false;
    }
  }

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::LOG_STRING);
    return cls(src.value, src.typeString).release();
  }
};

template <>
struct type_caster<
    wpi::telemetry::MockTelemetryBackend::LogBooleanArrayValue> {
  using Type = wpi::telemetry::MockTelemetryBackend::LogBooleanArrayValue;

  PYBIND11_TYPE_CASTER(
      Type,
      const_name("telemetry.mock_backend.LogBooleanArrayValue"));

  bool load(handle src, bool) {
    if (!src) {
      return false;
    }

    try {
      object source = reinterpret_borrow<object>(src);
      object pyValue = source.attr("value");
      auto booleans = pybind11::cast<std::vector<bool>>(pyValue);
      value.value.clear();
      value.value.reserve(booleans.size());
      for (bool item : booleans) {
        value.value.push_back(item ? 1 : 0);
      }
      return true;
    } catch (error_already_set& error) {
      if (!error.matches(PyExc_AttributeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      return false;
    } catch (const cast_error&) {
      return false;
    }
  }

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::LOG_BOOLEAN_ARRAY);
    list pyValue;
    for (int item : src.value) {
      pyValue.append(item != 0);
    }
    return cls(std::move(pyValue)).release();
  }
};

template <>
struct type_caster<wpi::telemetry::MockTelemetryBackend::LogRawValue> {
  using Type = wpi::telemetry::MockTelemetryBackend::LogRawValue;

  PYBIND11_TYPE_CASTER(
      Type, const_name("telemetry.mock_backend.LogRawValue"));

  bool load(handle src, bool) {
    if (!src) {
      return false;
    }

    try {
      object source = reinterpret_borrow<object>(src);
      object pyValue = source.attr("value");
      object typeString = source.attr("type_string");
      if (!PyBytes_Check(pyValue.ptr())) {
        return false;
      }
      std::string raw = pybind11::cast<std::string>(pyValue);
      value.value.assign(raw.begin(), raw.end());
      value.typeString = pybind11::cast<std::string>(typeString);
      return true;
    } catch (error_already_set& error) {
      if (!error.matches(PyExc_AttributeError)) {
        throw;
      }
      error.restore();
      PyErr_Clear();
      return false;
    } catch (const cast_error&) {
      return false;
    }
  }

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::LOG_RAW);
    bytes pyValue{reinterpret_cast<const char*>(src.value.data()),
                  src.value.size()};
    return cls(std::move(pyValue), src.typeString).release();
  }
};

template <>
struct type_caster<wpi::telemetry::MockTelemetryBackend::Action> {
  using Type = wpi::telemetry::MockTelemetryBackend::Action;

  static constexpr auto name =
      const_name("telemetry.mock_backend.Action");

  static handle cast(const Type& src, return_value_policy, handle) {
    object cls = wpi::telemetry::python::GetMockBackendValueType(
        wpi::telemetry::python::MockBackendValueType::ACTION);
    object pyValue = std::visit(
        [](const auto& item) { return pybind11::cast(item); }, src.value);
    return cls(src.path, std::move(pyValue), src.timestamp).release();
  }

  static handle cast(Type&& src, return_value_policy policy, handle parent) {
    return cast(static_cast<const Type&>(src), policy, parent);
  }

  static handle cast(const Type* src, return_value_policy policy,
                     handle parent) {
    if (!src) {
      return none().release();
    }
    return cast(*src, policy, parent);
  }
};

}  // namespace pybind11::detail
