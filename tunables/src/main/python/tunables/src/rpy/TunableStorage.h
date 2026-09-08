#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <wpi/tunables/TunableTable.hpp>

namespace wpi::tunables::python {

class PyComplexTunableAdapter;
class PyTunable;

namespace detail {

void StoreValue(std::string path, std::shared_ptr<PyTunable> tunable);
void StoreComplexValue(std::string path,
                       std::shared_ptr<PyComplexTunableAdapter> tunable);
void StoreNativeComplexValue(std::string path, pybind11::object tunable);

}  // namespace detail

void StoreRefreshValue(std::string_view path,
                       const std::shared_ptr<PyTunable>& tunable);
void RemoveRefreshPath(std::string_view path);
void ClearValues();
void RegisterPreUpdateCallback();
void CleanupPythonStorage();
std::string NormalizePath(std::string_view path);
std::string NormalizeTablePath(const wpi::tunables::TunableTable& table,
                               std::string_view name);
void RemoveRetainedPath(std::string_view path);
void RemovePath(std::string_view path);
void RemoveValue(pybind11::handle value);
void RemoveRootValue(std::string_view name);

}  // namespace wpi::tunables::python
