#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <wpi/tunables/TunableTable.hpp>

namespace wpi::tunables::python {

class PyComplexTunableAdapter;
class PyTunable;

class PyTunableTable {
 public:
  PyTunableTable(wpi::tunables::TunableTable table,
                 PyComplexTunableAdapter* owner);
  std::string GetPath() const;
  PyTunableTable GetTable(std::string_view name);
  bool Publish(std::string_view name, pybind11::object value);
  std::shared_ptr<PyTunable> Add(std::string_view name, pybind11::object value,
                                 pybind11::object valueType,
                                 pybind11::object elementType, bool robust,
                                 bool isMutable, pybind11::object onTune,
                                 pybind11::object properties,
                                 std::string typeString);
  std::shared_ptr<PyTunable> PublishValue(
      std::string_view name, pybind11::object getter, pybind11::object setter,
      pybind11::object valueType, pybind11::object elementType, bool robust,
      bool isMutable, pybind11::object properties, std::string typeString);
  void Remove(std::string_view name);

 private:
  void StoreValue(std::string_view name, std::shared_ptr<PyTunable> tunable);
  wpi::tunables::TunableTable m_table;
  PyComplexTunableAdapter* m_owner;
};

}  // namespace wpi::tunables::python
