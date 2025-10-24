cls_NetworkTableEntry
    .def_property_readonly("value", [](const nt::NetworkTableEntry &self) {
        nt::Value v;
        {
            py::gil_scoped_release release;
            v = self.GetValue();
        }
        return pyntcore::ntvalue2py(v);
    })

    // double overload must come before boolean version
    .def("setValue", [](nt::NetworkTableEntry *self, double value) {
        return self->SetValue(nt::Value::MakeDouble(value));
    }, py::arg("value"), release_gil())
    .def("setValue", [](nt::NetworkTableEntry *self, bool value) {
        return self->SetValue(nt::Value::MakeBoolean(value));
    }, py::arg("value"), release_gil())
    .def("setValue", [](nt::NetworkTableEntry *self, py::bytes value) {
        auto v = nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->SetValue(v);
    }, py::arg("value"))
    .def("setValue", [](nt::NetworkTableEntry *self, std::string value) {
        return self->SetValue(nt::Value::MakeString(value));
    }, py::arg("value"), release_gil())
    .def("setValue", [](nt::NetworkTableEntry *self, py::sequence value) {
        return self->SetValue(pyntcore::py2ntvalue(value));
    }, py::arg("value"))

    // double overload must come before boolean version
    .def("setDefaultValue", [](nt::NetworkTableEntry *self, double value) {
        return self->SetDefaultValue(nt::Value::MakeDouble(value));
    }, py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTableEntry *self, bool value) {
        return self->SetDefaultValue(nt::Value::MakeBoolean(value));
    }, py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTableEntry *self, py::bytes value) {
        auto v = nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->SetDefaultValue(v);
    }, py::arg("value"))
    .def("setDefaultValue", [](nt::NetworkTableEntry *self, std::string value) {
        return self->SetDefaultValue(nt::Value::MakeString(value));
    }, py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTableEntry *self, py::sequence value) {
        return self->SetDefaultValue(pyntcore::py2ntvalue(value));
    }, py::arg("value"))
;
