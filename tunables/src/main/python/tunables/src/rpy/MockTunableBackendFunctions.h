#pragma once

#include <string_view>

#include <pybind11/pybind11.h>
#include <wpi/tunables/MockTunableBackend.hpp>

namespace wpi::tunables::python {

void SetRaw(wpi::tunables::MockTunableBackend& self, std::string_view path,
            pybind11::handle value);
void SetStruct(wpi::tunables::MockTunableBackend& self, std::string_view path,
               pybind11::handle value);
void SetStructVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const pybind11::sequence& value);
void SetBoolVector(wpi::tunables::MockTunableBackend& self,
                   std::string_view path, const pybind11::sequence& value);
void SetInt32Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const pybind11::sequence& value);
void SetInt64Vector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const pybind11::sequence& value);
void SetFloatVector(wpi::tunables::MockTunableBackend& self,
                    std::string_view path, const pybind11::sequence& value);
void SetDoubleVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const pybind11::sequence& value);
void SetStringVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, const pybind11::sequence& value);
pybind11::object GetUid(const wpi::tunables::MockTunableBackend& self,
                        std::string_view path);
pybind11::object GetTunableValue(const wpi::tunables::MockTunableBackend& self,
                                 std::string_view path);

}  // namespace wpi::tunables::python
