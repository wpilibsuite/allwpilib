#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>

#include "wpi/tunables/ComplexTunable.hpp"

namespace wpi::tunables::python {

class PyTunable;
struct TunableTableOwnerContext;

class PyComplexTunableAdapter
    : public wpi::tunables::ComplexTunable,
      public std::enable_shared_from_this<PyComplexTunableAdapter> {
 public:
  PyComplexTunableAdapter(pybind11::object value,
                          pybind11::object initialPublishTunable);
  ~PyComplexTunableAdapter() override = default;

  std::string_view GetTunableType() const override;
  bool IsValue(pybind11::handle value) const;

  void PublishTunable(wpi::tunables::TunableTable& table) override;
  void UpdateTunable() const override;

  void AddValue(std::string path, std::shared_ptr<PyTunable> value);
  void AddComplex(std::string path,
                  std::shared_ptr<PyComplexTunableAdapter> value);
  void AddNativeComplex(std::string path, pybind11::object value);

  void RemovePath(std::string_view path);
  void RemoveRetainedPath(std::string_view path);

 private:
  std::shared_ptr<TunableTableOwnerContext> m_tableOwnerContext;
  pybind11::object m_value;
  std::optional<pybind11::object> m_initialPublishTunable;
  std::string m_type;
  mutable std::vector<std::pair<std::string, std::shared_ptr<PyTunable>>>
      m_values;
  std::vector<
      std::pair<std::string, std::shared_ptr<PyComplexTunableAdapter>>>
      m_complex;
  std::vector<std::pair<std::string, pybind11::object>> m_nativeComplex;
};

}  // namespace wpi::tunables::python
