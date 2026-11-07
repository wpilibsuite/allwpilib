#pragma once

#include <stdint.h>

#include <optional>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>
#include <wpystruct.h>

namespace wpi::tunables::python {

using PythonType = pybind11::typing::Type<pybind11::object>;

PythonType BuiltinType(PyTypeObject* type);
std::optional<pybind11::object> GetOptionalAttr(pybind11::handle value,
                                                const char* name);
std::vector<uint8_t> ToRawVector(pybind11::handle value);
pybind11::type GetStructSequenceType(const pybind11::sequence& value);
void ValidateStructSequenceType(const pybind11::sequence& value,
                                const pybind11::type& type);
std::vector<WPyStruct> ToStructVector(const pybind11::sequence& value,
                                      bool allowEmpty = false);

}  // namespace wpi::tunables::python
