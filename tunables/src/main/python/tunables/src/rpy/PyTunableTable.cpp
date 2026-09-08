#include "PyTunableTable.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <pybind11/pybind11.h>
#include <wpi/tunables/ComplexTunable.hpp>

#include "PyComplexTunableAdapter.h"
#include "PyTunable.h"
#include "TunableStorage.h"
#include "TunableValuePython.h"

namespace py = pybind11;

namespace wpi::tunables::python {

PyTunableTable::PyTunableTable(wpi::tunables::TunableTable table,
                               PyComplexTunableAdapter* owner)
    : m_table{std::move(table)}, m_owner{owner} {}

std::string PyTunableTable::GetPath() const {
  return m_table.GetPath();
}

PyTunableTable PyTunableTable::GetTable(std::string_view name) {
  return PyTunableTable{m_table.GetTable(name), m_owner};
}

void PyTunableTable::StoreValue(std::string_view name,
                                std::shared_ptr<PyTunable> tunable) {
  std::string path = NormalizeTablePath(m_table, name);
  if (m_owner) {
    m_owner->AddValue(path, tunable);
  } else {
    detail::StoreValue(path, tunable);
  }
  StoreRefreshValue(path, tunable);
}

bool PyTunableTable::Publish(std::string_view name, py::object value) {
  bool published;
  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, tunable->GetBase());
    }
    if (published) {
      StoreValue(name, std::move(tunable));
    }
  } else if (py::isinstance<wpi::tunables::ComplexTunable>(value)) {
    auto& tunable = value.cast<wpi::tunables::ComplexTunable&>();
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, tunable);
    }
    if (published) {
      std::string path = NormalizeTablePath(m_table, name);
      if (m_owner) {
        m_owner->AddNativeComplex(std::move(path), std::move(value));
      } else {
        detail::StoreNativeComplexValue(std::move(path), std::move(value));
      }
    }
  } else if (auto publishTunable = GetOptionalAttr(value, "publish_tunables")) {
    auto tunable = std::make_shared<PyComplexTunableAdapter>(
        std::move(value), std::move(*publishTunable));
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = m_table.Publish(nameString, *tunable);
    }
    if (published) {
      std::string path = NormalizeTablePath(m_table, name);
      if (m_owner) {
        m_owner->AddComplex(std::move(path), std::move(tunable));
      } else {
        detail::StoreComplexValue(std::move(path), std::move(tunable));
      }
    }
  } else {
    throw py::type_error(
        "value must be a wpi::tunables::Tunable or "
        "wpi::tunables::ComplexTunable");
  }
  return published;
}

std::shared_ptr<PyTunable> PyTunableTable::Add(
    std::string_view name, py::object value, py::object valueType,
    py::object elementType, bool robust, bool isMutable, py::object onTune,
    py::object properties, std::string typeString) {
  auto tunable = std::make_shared<PyTunable>(
      value, py::none(), py::none(), std::move(onTune), robust, isMutable,
      std::move(valueType), std::move(elementType), std::move(properties),
      std::move(typeString), false);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = m_table.Publish(nameString, tunable->GetBase());
  }
  if (published) {
    StoreValue(name, tunable);
  }
  return tunable;
}

std::shared_ptr<PyTunable> PyTunableTable::PublishValue(
    std::string_view name, py::object getter, py::object setter,
    py::object valueType, py::object elementType, bool robust, bool isMutable,
    py::object properties, std::string typeString) {
  auto value = std::make_shared<PyTunable>(
      getter(), getter, setter, py::none(), robust, isMutable,
      std::move(valueType), std::move(elementType), std::move(properties),
      std::move(typeString), false);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = m_table.Publish(nameString, value->GetBase());
  }
  if (published) {
    StoreValue(name, value);
  }
  return value;
}

void PyTunableTable::Remove(std::string_view name) {
  std::string path = NormalizeTablePath(m_table, name);
  if (m_owner) {
    m_owner->RemovePath(path);
  } else {
    python::RemovePath(path);
  }
}

}  // namespace wpi::tunables::python
