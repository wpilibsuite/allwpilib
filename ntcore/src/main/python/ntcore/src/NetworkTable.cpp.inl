#pragma once

#include <string>
#include <utility>

// clang-format off
cls_NetworkTable
    .def("get_value", [](const NetworkTable &self, std::string_view key, py::object defaultValue) -> py::object {
        wpi::nt::NetworkTableEntry entry;
        {
            py::gil_scoped_release release;
            entry = self.GetEntry(key);
        }
        return pyntcore::GetValueEntry(entry, defaultValue);
    }, py::arg("key"), py::arg("value"))

    // double overload must come before boolean version
    .def("put_value", [](wpi::nt::NetworkTable *self, std::string_view key, double value) {
        return self->PutValue(key, wpi::nt::Value::MakeDouble(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("put_value", [](wpi::nt::NetworkTable *self, std::string_view key, bool value) {
        return self->PutValue(key, wpi::nt::Value::MakeBoolean(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("put_value", [](wpi::nt::NetworkTable *self, std::string_view key, py::bytes value) {
        auto v = wpi::nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->PutValue(key, v);
    }, py::arg("key"), py::arg("value"))
    .def("put_value", [](wpi::nt::NetworkTable *self, std::string_view key, std::string value) {
        return self->PutValue(key, wpi::nt::Value::MakeString(std::move(value)));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("put_value", [](wpi::nt::NetworkTable *self, std::string_view key, py::sequence value) {
        auto v = pyntcore::py2ntvalue(value);
        py::gil_scoped_release release;
        return self->PutValue(key, v);
    }, py::arg("key"), py::arg("value"))

    // double overload must come before boolean version
    .def("set_default_value", [](wpi::nt::NetworkTable *self, std::string_view key, double value) {
        return self->SetDefaultValue(key, wpi::nt::Value::MakeDouble(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("set_default_value", [](wpi::nt::NetworkTable *self, std::string_view key, bool value) {
        return self->SetDefaultValue(key, wpi::nt::Value::MakeBoolean(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("set_default_value", [](wpi::nt::NetworkTable *self, std::string_view key, py::bytes value) {
        auto v = wpi::nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->SetDefaultValue(key, v);
    }, py::arg("key"), py::arg("value"))
    .def("set_default_value", [](wpi::nt::NetworkTable *self, std::string_view key, std::string value) {
        return self->SetDefaultValue(key, wpi::nt::Value::MakeString(std::move(value)));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("set_default_value", [](wpi::nt::NetworkTable *self, std::string_view key, py::sequence value) {
        auto v = pyntcore::py2ntvalue(value);
        py::gil_scoped_release release;
        return self->SetDefaultValue(key, v);
    }, py::arg("key"), py::arg("value"))

    .def("__contains__", [](const wpi::nt::NetworkTable &self, std::string_view key) -> bool {
        return self.ContainsKey(key);
    }, release_gil())
;
// clang-format on
