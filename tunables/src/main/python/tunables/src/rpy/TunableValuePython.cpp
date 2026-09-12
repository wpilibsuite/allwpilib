#include "TunableValuePython.h"

#include <string>
#include <vector>

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

bool IsWpiStruct(py::handle value) {
  return py::hasattr(py::type::of(value), "WPIStruct");
}

bool IsBytesLike(py::handle value) {
  return PyBytes_Check(value.ptr()) || PyByteArray_Check(value.ptr()) ||
         PyMemoryView_Check(value.ptr());
}

std::string BytesLikeToString(py::handle value) {
  py::object bytes =
      py::reinterpret_steal<py::object>(PyBytes_FromObject(value.ptr()));
  if (!bytes) {
    throw py::error_already_set{};
  }
  return bytes.cast<std::string>();
}

}  // namespace

py::object BuiltinType(const char* name) {
  return py::module_::import("builtins").attr(name);
}

std::optional<py::object> GetOptionalAttr(py::handle value, const char* name) {
#if PY_VERSION_HEX >= 0x030D0000
  PyObject* attr = nullptr;
  int result = PyObject_GetOptionalAttrString(value.ptr(), name, &attr);
  if (result < 0) {
    throw py::error_already_set{};
  }
  if (result == 0) {
    return std::nullopt;
  }
  return py::reinterpret_steal<py::object>(attr);
#else
  PyObject* attr = PyObject_GetAttrString(value.ptr(), name);
  if (attr) {
    return py::reinterpret_steal<py::object>(attr);
  }
  if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
    PyErr_Clear();
    return std::nullopt;
  }
  throw py::error_already_set{};
#endif
}

std::vector<uint8_t> ToRawVector(py::handle value) {
  if (IsBytesLike(value)) {
    auto raw = BytesLikeToString(value);
    return {raw.begin(), raw.end()};
  }
  auto sequence = py::reinterpret_borrow<py::sequence>(value);
  std::vector<uint8_t> data;
  const size_t size = py::len(sequence);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    int item = sequence[static_cast<py::ssize_t>(i)].cast<int>();
    if (item < 0 || item > 255) {
      throw py::value_error("raw tunable values must be in range 0-255");
    }
    data.emplace_back(static_cast<uint8_t>(item));
  }
  return data;
}

py::type GetStructSequenceType(const py::sequence& value) {
  if (py::len(value) == 0) {
    throw py::value_error("struct tunable arrays require at least one value");
  }

  py::handle first = value[0];
  if (!IsWpiStruct(first)) {
    throw py::type_error("struct tunable arrays require WPIStruct values");
  }

  py::type type = py::type::of(first);
  const size_t size = py::len(value);
  for (size_t i = 1; i < size; ++i) {
    py::handle item = value[static_cast<py::ssize_t>(i)];
    if (!py::type::of(item).is(type)) {
      throw py::type_error("struct tunable arrays require one WPIStruct type");
    }
  }
  return type;
}

void ValidateStructSequenceType(const py::sequence& value,
                                const py::type& type) {
  const size_t size = py::len(value);
  for (size_t i = 0; i < size; ++i) {
    py::handle item = value[static_cast<py::ssize_t>(i)];
    int isInstance = PyObject_IsInstance(item.ptr(), type.ptr());
    if (isInstance < 0) {
      throw py::error_already_set{};
    }
    if (isInstance == 0) {
      throw py::type_error(
          "struct tunable arrays require values of the specified WPIStruct "
          "type");
    }
  }
}

std::vector<WPyStruct> ToStructVector(const py::sequence& value,
                                      bool allowEmpty) {
  if (allowEmpty && py::len(value) == 0) {
    return {};
  }

  GetStructSequenceType(value);

  std::vector<WPyStruct> data;
  const size_t size = py::len(value);
  data.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    data.emplace_back(
        py::reinterpret_borrow<py::object>(value[static_cast<py::ssize_t>(i)]));
  }
  return data;
}

wpi::util::json ToJson(py::handle value) {
  if (value.is_none()) {
    return nullptr;
  }
  if (py::isinstance<py::bool_>(value)) {
    return value.cast<bool>();
  }
  if (py::isinstance<py::int_>(value)) {
    return value.cast<int64_t>();
  }
  if (py::isinstance<py::float_>(value)) {
    return value.cast<double>();
  }
  if (py::isinstance<py::str>(value)) {
    return value.cast<std::string>();
  }
  if (py::isinstance<py::dict>(value)) {
    wpi::util::json obj = wpi::util::json::object();
    auto dict = py::reinterpret_borrow<py::dict>(value);
    for (auto&& item : dict) {
      obj[item.first.cast<std::string>()] = ToJson(item.second);
    }
    return obj;
  }
  if (PySequence_Check(value.ptr()) && !IsBytesLike(value)) {
    wpi::util::json arr = wpi::util::json::array();
    auto sequence = py::reinterpret_borrow<py::sequence>(value);
    const size_t size = py::len(sequence);
    for (size_t i = 0; i < size; ++i) {
      arr.emplace_back(ToJson(sequence[static_cast<py::ssize_t>(i)]));
    }
    return arr;
  }
  return py::str(value).cast<std::string>();
}

}  // namespace wpi::tunables::python
