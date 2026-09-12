#include "TelemetryPython.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "rpy/MockTelemetryBackendFunctions.h"
#include "rpy/PyTelemetryTable.h"
#include "wpi/telemetry/DiscardTelemetryBackend.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/MultiTelemetryBackend.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

namespace py = pybind11;

namespace {

constexpr const char* LOG_DOC = R"doc(Logs a telemetry value.

Sequences must pass an explicit element_type. Use bool, int, float, or str for
primitive arrays, object to log a string array using str() for each element, or
a WPIStruct class for struct arrays. type_string is only used as custom type
metadata for scalar str and bytes-like values.)doc";

}  // namespace

void wpi::InitTelemetryPython(py::module_& m) {
  using telemetry::python::ActionValueToPython;
  using telemetry::python::PyTelemetryTable;
  using telemetry::python::SchemaToPython;

  py::class_<wpi::telemetry::TelemetryTable>(m, "_NativeTelemetryTable")
      .def_property_readonly("path",
                             [](const wpi::telemetry::TelemetryTable& self) {
                               return std::string{self.GetPath()};
                             })
      .def("set_type", &wpi::telemetry::TelemetryTable::SetType)
      .def("get_type", &wpi::telemetry::TelemetryTable::GetType)
      .def("has_type", &wpi::telemetry::TelemetryTable::HasType)
      .def("get_table", &wpi::telemetry::TelemetryTable::GetTable,
           py::return_value_policy::reference)
      .def("keep_duplicates", &wpi::telemetry::TelemetryTable::KeepDuplicates)
      .def("set_property", &wpi::telemetry::TelemetryTable::SetProperty)
      .def(
          "log",
          [](wpi::telemetry::TelemetryTable& self, std::string_view name,
             py::object value, py::object elementType,
             std::string_view typeString) {
            PyTelemetryTable{self}.Log(name, value, std::move(elementType),
                                       typeString);
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("element_type") = py::none(), py::arg("type_string") = "",
          LOG_DOC);

  py::class_<PyTelemetryTable>(m, "TelemetryTable")
      .def_property_readonly("path", &PyTelemetryTable::GetPath)
      .def("set_type", &PyTelemetryTable::SetType)
      .def("get_type", &PyTelemetryTable::GetType)
      .def("has_type", &PyTelemetryTable::HasType)
      .def("get_table", &PyTelemetryTable::GetTable)
      .def("keep_duplicates", &PyTelemetryTable::KeepDuplicates)
      .def("set_property", &PyTelemetryTable::SetProperty)
      .def("log", &PyTelemetryTable::Log, py::arg("name"), py::arg("value"),
           py::kw_only(), py::arg("element_type") = py::none(),
           py::arg("type_string") = "", LOG_DOC);

  m.def(
      "get_table",
      [](std::string_view name) {
        auto& root = wpi::telemetry::GetTable();
        return PyTelemetryTable{name.empty() ? root : root.GetTable(name)};
      },
      py::arg("name") = "");
  m.def(
      "log",
      [](std::string_view name, py::object value, py::object elementType,
         std::string_view typeString) {
        PyTelemetryTable{wpi::telemetry::GetTable()}.Log(
            name, value, std::move(elementType), typeString);
      },
      py::arg("name"), py::arg("value"), py::kw_only(),
      py::arg("element_type") = py::none(), py::arg("type_string") = "",
      LOG_DOC);
  m.def("keep_duplicates", &wpi::telemetry::KeepDuplicates);
  m.def("set_property", &wpi::telemetry::SetProperty);

  py::class_<wpi::telemetry::TelemetryRegistry>(m, "TelemetryRegistry")
      .def_static(
          "set_report_warning",
          [](py::object func) {
            if (func.is_none()) {
              wpi::telemetry::TelemetryRegistry::SetReportWarning(nullptr);
            } else {
              auto callback = std::shared_ptr<py::object>{
                  new py::object{std::move(func)}, [](py::object* object) {
                    py::gil_scoped_acquire gil;
                    delete object;
                  }};
              wpi::telemetry::TelemetryRegistry::SetReportWarning(
                  [callback](std::string_view path, std::string_view msg) {
                    py::gil_scoped_acquire gil;
                    (*callback)(std::string{path}, std::string{msg});
                  });
            }
          })
      .def_static("report_warning",
                  &wpi::telemetry::TelemetryRegistry::ReportWarning)
      .def_static(
          "register_backend",
          [](std::string_view prefix,
             std::shared_ptr<wpi::telemetry::TelemetryBackend> backend) {
            wpi::telemetry::TelemetryRegistry::RegisterBackend(
                prefix, std::move(backend));
          })
      .def_static("get_backend", &wpi::telemetry::TelemetryRegistry::GetBackend)
      .def_static("get_entry", &wpi::telemetry::TelemetryRegistry::GetEntry)
      .def_static(
          "get_table",
          [](std::string_view path) {
            return PyTelemetryTable{
                wpi::telemetry::TelemetryRegistry::GetTable(path)};
          },
          py::arg("path"))
      .def_static("reset", &wpi::telemetry::TelemetryRegistry::Reset);

  py::class_<wpi::telemetry::DiscardTelemetryBackend, py::smart_holder,
             wpi::telemetry::TelemetryBackend>(m, "DiscardTelemetryBackend")
      .def(py::init<>());

  py::class_<wpi::telemetry::MultiTelemetryBackend, py::smart_holder,
             wpi::telemetry::TelemetryBackend>(m, "MultiTelemetryBackend")
      .def(py::init<>())
      .def(
          py::init<
              std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>>(),
          py::arg("backends"));

  py::class_<wpi::telemetry::MockTelemetryBackend, py::smart_holder,
             wpi::telemetry::TelemetryBackend>(m, "MockTelemetryBackend")
      .def(py::init<>())
      .def("clear", &wpi::telemetry::MockTelemetryBackend::Clear)
      .def("get_actions",
           [](const wpi::telemetry::MockTelemetryBackend& self) {
             py::list actions;
             for (const auto& action : self.GetActions()) {
               actions.append(ActionValueToPython(action));
             }
             return actions;
           })
      .def("get_last_action",
           [](const wpi::telemetry::MockTelemetryBackend& self,
              std::string_view path) {
             auto* action = self.GetLastAction(path);
             if (!action) {
               return py::object{py::none{}};
             }
             return ActionValueToPython(*action);
           })
      .def("get_last_value",
           [](const wpi::telemetry::MockTelemetryBackend& self,
              std::string_view path) {
             auto* action = self.GetLastAction(path);
             if (!action) {
               return py::object{py::none{}};
             }
             py::dict result =
                 py::reinterpret_borrow<py::dict>(ActionValueToPython(*action));
             return py::object{result["value"]};
           })
      .def("get_schema", [](wpi::telemetry::MockTelemetryBackend& self,
                            std::string_view schemaName) {
        return SchemaToPython(self.GetSchema(schemaName));
      });
}
