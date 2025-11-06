cls_NetworkTable
    .def("getValue", [](const NetworkTable &self, std::string_view key, py::object defaultValue) -> py::object {
        nt::NetworkTableEntry entry;
        {
            py::gil_scoped_release release;
            entry = self.GetEntry(key);
        }
        return pyntcore::GetValueEntry(entry, defaultValue);
    }, py::arg("key"), py::arg("value"))

    // double overload must come before boolean version
    .def("putValue", [](nt::NetworkTable *self, std::string_view key, double value) {
        return self->PutValue(key, nt::Value::MakeDouble(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("putValue", [](nt::NetworkTable *self, std::string_view key, bool value) {
        return self->PutValue(key, nt::Value::MakeBoolean(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("putValue", [](nt::NetworkTable *self, std::string_view key, py::bytes value) {
        auto v = nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->PutValue(key, v);
    }, py::arg("key"), py::arg("value"))
    .def("putValue", [](nt::NetworkTable *self, std::string_view key, std::string value) {
        return self->PutValue(key, nt::Value::MakeString(std::move(value)));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("putValue", [](nt::NetworkTable *self, std::string_view key, py::sequence value) {
        auto v = pyntcore::py2ntvalue(value);
        py::gil_scoped_release release;
        return self->PutValue(key, v);
    }, py::arg("key"), py::arg("value"))

    // double overload must come before boolean version
    .def("setDefaultValue", [](nt::NetworkTable *self, std::string_view key, double value) {
        return self->SetDefaultValue(key, nt::Value::MakeDouble(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTable *self, std::string_view key, bool value) {
        return self->SetDefaultValue(key, nt::Value::MakeBoolean(value));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTable *self, std::string_view key, py::bytes value) {
        auto v = nt::Value::MakeRaw(value.cast<std::span<const uint8_t>>());
        py::gil_scoped_release release;
        return self->SetDefaultValue(key, v);
    }, py::arg("key"), py::arg("value"))
    .def("setDefaultValue", [](nt::NetworkTable *self, std::string_view key, std::string value) {
        return self->SetDefaultValue(key, nt::Value::MakeString(std::move(value)));
    }, py::arg("key"), py::arg("value"), release_gil())
    .def("setDefaultValue", [](nt::NetworkTable *self, std::string_view key, py::sequence value) {
        auto v = pyntcore::py2ntvalue(value);
        py::gil_scoped_release release;
        return self->SetDefaultValue(key, v);
    }, py::arg("key"), py::arg("value"))

    .def("__contains__", [](const nt::NetworkTable &self, std::string_view key) -> bool {
        return self.ContainsKey(key);
    }, release_gil())
;
