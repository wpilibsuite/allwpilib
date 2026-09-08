#include "TunablePython.h"

#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>

#include "pybind11/functional.h"
#include "pybind11/stl.h"
#include "rpy/MockTunableBackendFunctions.h"
#include "rpy/PyMutationList.h"
#include "rpy/PyTunable.h"
#include "rpy/PyTunableTable.h"
#include "rpy/TunableStorage.h"
#include "rpy/TunableValuePython.h"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"

namespace py = pybind11;

void wpi::InitTunablePython(py::module_& m) {
  using tunables::python::BuiltinType;
  using tunables::python::CleanupPythonStorage;
  using tunables::python::ClearValues;
  using tunables::python::GetTunableValue;
  using tunables::python::GetUid;
  using tunables::python::PyMutationList;
  using tunables::python::PyTunable;
  using tunables::python::PyTunableTable;
  using tunables::python::RegisterPreUpdateCallback;
  using tunables::python::RemovePath;
  using tunables::python::RemoveRootValue;
  using tunables::python::RemoveValue;
  using tunables::python::SetBoolVector;
  using tunables::python::SetDoubleVector;
  using tunables::python::SetFloatVector;
  using tunables::python::SetInt32Vector;
  using tunables::python::SetInt64Vector;
  using tunables::python::SetRaw;
  using tunables::python::SetStringVector;
  using tunables::python::SetStruct;
  using tunables::python::SetStructVector;

  RegisterPreUpdateCallback();

  static int unused;
  py::capsule cleanup(&unused, [](void*) {
    py::gil_scoped_acquire gil;
    CleanupPythonStorage();
  });
  m.add_object("_tunable_cleanup", cleanup);

  py::class_<PyMutationList>(m, "_MutationList")
      .def("__len__", &PyMutationList::Size)
      .def("__iter__", &PyMutationList::Iter, py::keep_alive<0, 1>())
      .def("__getitem__", &PyMutationList::GetItem)
      .def("__setitem__", &PyMutationList::SetItem)
      .def("__delitem__", &PyMutationList::DelItem)
      .def("__contains__", &PyMutationList::Contains)
      .def("__eq__", &PyMutationList::Equal)
      .def("__repr__", &PyMutationList::Repr)
      .def("__iadd__", &PyMutationList::IAdd,
           py::return_value_policy::reference_internal)
      .def("copy", &PyMutationList::Copy)
      .def("append", &PyMutationList::Append)
      .def("extend", &PyMutationList::Extend)
      .def("insert", &PyMutationList::Insert)
      .def("pop", &PyMutationList::Pop)
      .def("remove", &PyMutationList::Remove)
      .def("clear", &PyMutationList::Clear)
      .def("reverse", &PyMutationList::Reverse)
      .def("sort", &PyMutationList::Sort);

  py::class_<PyTunable, std::shared_ptr<PyTunable>>(m, "Tunable")
      .def(py::init<py::object, py::object, py::object, py::object, bool, bool,
                    py::object, py::object, py::object, std::string, bool>(),
           py::arg("value"), py::kw_only(), py::arg("getter") = py::none(),
           py::arg("setter") = py::none(), py::arg("on_tune") = py::none(),
           py::arg("robust") = false, py::arg("mutable") = true,
           py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "",
           py::arg("always_get") = false)
      .def("get", &PyTunable::Get)
      .def("set", &PyTunable::Set)
      .def("mutate", &PyTunable::Mutate);

  py::class_<wpi::tunables::TunableTable>(m, "_NativeTunableTable")
      .def_property_readonly("path", &wpi::tunables::TunableTable::GetPath)
      .def("get_table", &wpi::tunables::TunableTable::GetTable)
      .def("remove", &wpi::tunables::TunableTable::Remove);

  py::class_<PyTunableTable>(m, "TunableTable")
      .def_property_readonly("path", &PyTunableTable::GetPath)
      .def("get_table", &PyTunableTable::GetTable)
      .def("publish", &PyTunableTable::Publish)
      .def("add", &PyTunableTable::Add, py::arg("name"), py::arg("value"),
           py::kw_only(), py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(), py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("on_tune") = py::none(),
           py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "add_boolean",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("bool"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_int",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("int"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_long",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("int"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_float",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("float"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def(
          "add_double",
          [](PyTunableTable& self, std::string_view name, py::object value,
             bool robust, bool isMutable, py::object onTune,
             py::object properties, std::string typeString) {
            return self.Add(name, std::move(value), BuiltinType("float"),
                            py::none(), robust, isMutable, std::move(onTune),
                            std::move(properties), std::move(typeString));
          },
          py::arg("name"), py::arg("value"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
          py::arg("type_string") = "")
      .def("publish_value", &PyTunableTable::PublishValue, py::arg("name"),
           py::arg("getter"), py::arg("setter"), py::kw_only(),
           py::arg("value_type") = py::none(),
           py::arg("element_type") = py::none(), py::arg("robust") = false,
           py::arg("mutable") = true, py::arg("properties") = py::none(),
           py::arg("type_string") = "")
      .def(
          "publish_boolean",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("bool"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_int",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("int"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_long",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("int"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_float",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("float"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_double",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("float"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_string",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("str"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_boolean_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("bool"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_integer_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("int"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_double_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("float"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_string_array",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     py::none(), BuiltinType("str"), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def(
          "publish_raw",
          [](PyTunableTable& self, std::string_view name, py::object getter,
             py::object setter, bool robust, bool isMutable,
             py::object properties, std::string typeString) {
            return self.PublishValue(name, std::move(getter), std::move(setter),
                                     BuiltinType("bytes"), py::none(), robust,
                                     isMutable, std::move(properties),
                                     std::move(typeString));
          },
          py::arg("name"), py::arg("getter"), py::arg("setter"), py::kw_only(),
          py::arg("robust") = false, py::arg("mutable") = true,
          py::arg("properties") = py::none(), py::arg("type_string") = "")
      .def("remove", &PyTunableTable::Remove);

  m.def(
      "get_table",
      [](std::string_view name) {
        return PyTunableTable{name.empty() ? wpi::tunables::GetTable()
                                           : wpi::tunables::GetTable(name),
                              nullptr};
      },
      py::arg("name") = "");
  m.def("publish", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Publish(name,
                                                                      value);
  });
  m.def(
      "add",
      [](std::string_view name, py::object value, py::object valueType,
         py::object elementType, bool robust, bool isMutable, py::object onTune,
         py::object properties, std::string typeString) {
        return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
            name, std::move(value), std::move(valueType),
            std::move(elementType), robust, isMutable, std::move(onTune),
            std::move(properties), std::move(typeString));
      },
      py::arg("name"), py::arg("value"), py::kw_only(),
      py::arg("value_type") = py::none(), py::arg("element_type") = py::none(),
      py::arg("robust") = false, py::arg("mutable") = true,
      py::arg("on_tune") = py::none(), py::arg("properties") = py::none(),
      py::arg("type_string") = "");
  m.def("add_boolean", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("bool"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_int", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("int"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_long", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("int"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_float", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("float"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("add_double", [](std::string_view name, py::object value) {
    return PyTunableTable{wpi::tunables::GetTable(), nullptr}.Add(
        name, std::move(value), BuiltinType("float"), py::none(), false, true,
        py::none(), py::none(), "");
  });
  m.def("remove", &RemoveRootValue);

  py::class_<wpi::tunables::TunableRegistry>(m, "TunableRegistry")
      .def_static("set_report_warning",
                  [](py::object func) {
                    if (func.is_none()) {
                      wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
                    } else {
                      auto callback = std::shared_ptr<py::object>{
                          new py::object{std::move(func)},
                          [](py::object* object) {
                            py::gil_scoped_acquire gil;
                            delete object;
                          }};
                      wpi::tunables::TunableRegistry::SetReportWarning(
                          [callback](std::string_view msg) {
                            py::gil_scoped_acquire gil;
                            (*callback)(std::string{msg});
                          });
                    }
                  })
      .def_static("report_warning",
                  &wpi::tunables::TunableRegistry::ReportWarning)
      .def_static("register_backend",
                  [](std::string_view prefix,
                     std::shared_ptr<wpi::tunables::TunableBackend> backend) {
                    std::string prefixString{prefix};
                    py::gil_scoped_release release;
                    wpi::tunables::TunableRegistry::RegisterBackend(
                        prefixString, std::move(backend));
                  })
      .def_static("get_backend", &wpi::tunables::TunableRegistry::GetBackend)
      .def_static(
          "get_table",
          [](std::string_view path) {
            return PyTunableTable{wpi::tunables::GetTable(path), nullptr};
          },
          py::arg("path"))
      .def_static(
          "normalize_name",
          [](std::string_view path) {
            std::string buf;
            return std::string{wpi::tunables::detail::NormalizeName(path, buf)};
          })
      .def_static(
          "remove",
          [](py::object value) {
            if (py::isinstance<py::str>(value)) {
              RemovePath(value.cast<std::string>());
            } else {
              RemoveValue(value);
            }
          },
          py::arg("value"))
      .def_static("update",
                  [] {
                    py::gil_scoped_release release;
                    wpi::tunables::TunableRegistry::Update();
                  })
      .def_static(
          "with_update_mutex",
          [](py::function func) {
            std::unique_lock lock{
                wpi::tunables::TunableRegistry::GetUpdateMutex(),
                std::defer_lock};
            {
              py::gil_scoped_release release;
              lock.lock();
            }
            func();
          },
          py::arg("func"),
          "Run a short function while holding the registry update "
          "mutex; every competing tunable access must participate.")
      .def_static("reset", [] {
        ClearValues();
        {
          py::gil_scoped_release release;
          wpi::tunables::TunableRegistry::Reset();
        }
        RegisterPreUpdateCallback();
      });

  py::class_<wpi::tunables::MockTunableBackend, py::smart_holder,
             wpi::tunables::TunableBackend>(m, "MockTunableBackend")
      .def(py::init<>())
      .def("set_bool", &wpi::tunables::MockTunableBackend::SetBool)
      .def("set_int32", &wpi::tunables::MockTunableBackend::SetInt32)
      .def("set_int64", &wpi::tunables::MockTunableBackend::SetInt64)
      .def("set_float", &wpi::tunables::MockTunableBackend::SetFloat)
      .def("set_double", &wpi::tunables::MockTunableBackend::SetDouble)
      .def("set_string", &wpi::tunables::MockTunableBackend::SetString)
      .def("set_raw", &SetRaw)
      .def("set_struct", &SetStruct)
      .def("set_struct_vector", &SetStructVector)
      .def("set_struct_array", &SetStructVector)
      .def("set_bool_vector", &SetBoolVector)
      .def("set_int32_vector", &SetInt32Vector)
      .def("set_int64_vector", &SetInt64Vector)
      .def("set_float_vector", &SetFloatVector)
      .def("set_double_vector", &SetDoubleVector)
      .def("set_string_vector", &SetStringVector)
      .def("get_uid", &GetUid)
      .def("get_value", &GetTunableValue);
}
