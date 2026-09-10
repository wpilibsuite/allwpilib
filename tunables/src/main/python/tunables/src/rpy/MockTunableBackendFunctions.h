#pragma once

#include <stdint.h>

#include <optional>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <pybind11/typing.h>

#include "wpi/tunables/MockTunableBackend.hpp"

namespace wpi::tunables::python {

using RawValue = pybind11::typing::Union<
    pybind11::bytes, pybind11::bytearray, pybind11::memoryview,
    pybind11::typing::Iterable<int>>;
using StructIterable = pybind11::typing::Iterable<pybind11::object>;

void SetRaw(wpi::tunables::MockTunableBackend& self, std::string_view path,
            RawValue value);
void SetStruct(wpi::tunables::MockTunableBackend& self, std::string_view path,
               pybind11::handle value);
void SetStructVector(wpi::tunables::MockTunableBackend& self,
                     std::string_view path, StructIterable value);
void SetBoolVector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<bool> value);
void SetInt32Vector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<int32_t> value);
void SetInt64Vector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<int64_t> value);
void SetFloatVector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<float> value);
void SetDoubleVector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<double> value);
void SetStringVector(
    wpi::tunables::MockTunableBackend& self, std::string_view path,
    pybind11::typing::Iterable<std::string> value);

std::optional<uint32_t> GetUid(
    const wpi::tunables::MockTunableBackend& self, std::string_view path);

pybind11::object GetTunableValue(
    const wpi::tunables::MockTunableBackend& self, std::string_view path);

}  // namespace wpi::tunables::python
