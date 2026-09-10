#include "TunableStorage.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "PyComplexTunableAdapter.h"
#include "PyTunable.h"
#include "PyTunableTable.h"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"
#include "wpi/tunables/detail/TunableDetail.hpp"

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

template <typename T>
std::vector<std::unique_ptr<std::vector<std::shared_ptr<T>>>>&
GetSnapshotStorage() {
  static thread_local std::vector<
      std::unique_ptr<std::vector<std::shared_ptr<T>>>>
      storage;
  return storage;
}

template <typename T>
size_t& GetSnapshotDepth() {
  static thread_local size_t depth = 0;
  return depth;
}

template <typename T>
class RetainedValueSnapshot {
 public:
  template <typename Range>
  explicit RetainedValueSnapshot(const Range& values)
      : m_depth{GetSnapshotDepth<T>()},
        m_values{GetSnapshotValueStorage(m_depth)} {
    m_values.clear();
    m_values.reserve(values.size());
    for (const auto& entry : values) {
      m_values.emplace_back(entry.second);
    }
    ++GetSnapshotDepth<T>();
  }

  RetainedValueSnapshot(const RetainedValueSnapshot&) = delete;
  RetainedValueSnapshot& operator=(const RetainedValueSnapshot&) = delete;
  RetainedValueSnapshot(RetainedValueSnapshot&&) = delete;
  RetainedValueSnapshot& operator=(RetainedValueSnapshot&&) = delete;

  ~RetainedValueSnapshot() {
    --GetSnapshotDepth<T>();
    m_values.clear();
  }

  auto begin() const { return m_values.begin(); }
  auto end() const { return m_values.end(); }

 private:
  static std::vector<std::shared_ptr<T>>& GetSnapshotValueStorage(
      size_t depth) {
    auto& storage = GetSnapshotStorage<T>();
    while (storage.size() <= depth) {
      storage.emplace_back(std::make_unique<std::vector<std::shared_ptr<T>>>());
    }
    return *storage[depth];
  }

  size_t m_depth;
  std::vector<std::shared_ptr<T>>& m_values;
};

template <typename T, typename Range>
RetainedValueSnapshot<T> SnapshotRetainedValues(const Range& values) {
  return RetainedValueSnapshot<T>{values};
}

std::string MakeChildPrefix(std::string_view path) {
  std::string prefix{path};
  if (prefix.empty() || prefix.back() != '/') {
    prefix.push_back('/');
  }
  return prefix;
}

bool IsPathOrDescendant(std::string_view candidate, std::string_view path,
                        std::string_view childPrefix) {
  return candidate == path || candidate.starts_with(childPrefix);
}

bool IsPathOrDescendant(std::string_view candidate, std::string_view path) {
  return IsPathOrDescendant(candidate, path, MakeChildPrefix(path));
}

std::unordered_map<std::string, std::shared_ptr<PyTunable>>& GetValues() {
  static std::unordered_map<std::string, std::shared_ptr<PyTunable>> values;
  return values;
}

std::unordered_map<std::string, std::shared_ptr<PyTunable>>&
GetRefreshValues() {
  static std::unordered_map<std::string, std::shared_ptr<PyTunable>> values;
  return values;
}

std::unordered_map<std::string, std::shared_ptr<PyComplexTunableAdapter>>&
GetComplexValues() {
  static std::unordered_map<std::string,
                            std::shared_ptr<PyComplexTunableAdapter>>
      values;
  return values;
}

std::unordered_map<std::string, py::object>& GetNativeComplexValues() {
  static std::unordered_map<std::string, py::object> values;
  return values;
}

void RefreshValues() {
  py::gil_scoped_acquire gil;
  auto values = SnapshotRetainedValues<PyTunable>(GetRefreshValues());
  for (auto&& value : values) {
    value->Refresh();
  }
}

}  // namespace

namespace detail {

void StoreValue(std::string path, std::shared_ptr<PyTunable> value) {
  GetValues().insert_or_assign(std::move(path), std::move(value));
}

void StoreComplex(std::string path,
                  std::shared_ptr<PyComplexTunableAdapter> value) {
  GetComplexValues().insert_or_assign(std::move(path), std::move(value));
}

void StoreNativeComplexValue(std::string path, py::object value) {
  GetNativeComplexValues().insert_or_assign(std::move(path), std::move(value));
}

}  // namespace detail

void StoreRefreshValue(std::string_view path,
                       const std::shared_ptr<PyTunable>& tunable) {
  auto& values = GetRefreshValues();
  if (tunable->NeedsRefresh()) {
    values.insert_or_assign(std::string{path}, tunable);
  } else {
    values.erase(std::string{path});
  }
}

void RemoveRefreshPath(std::string_view path) {
  std::string childPrefix = MakeChildPrefix(path);
  std::erase_if(GetRefreshValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
}

void ClearValues() {
  GetValues().clear();
  GetRefreshValues().clear();
  GetComplexValues().clear();
  GetNativeComplexValues().clear();
}

void RegisterPreUpdateCallback() {
  wpi::tunables::detail::SetTunableRegistryPreUpdateCallback(
      [] { RefreshValues(); });
}

void CleanupPythonStorage() {
  wpi::tunables::detail::SetTunableRegistryPreUpdateCallback(nullptr);
  table::ClearContexts();
  ClearValues();
}

std::string NormalizePath(std::string_view path) {
  std::string buf;
  return std::string{wpi::tunables::detail::NormalizeName(path, buf)};
}

std::string NormalizeTablePath(const wpi::tunables::TunableTable& table,
                               std::string_view name) {
  return NormalizePath(table.GetPath() + std::string{name});
}

void RemoveRetainedPath(std::string_view path) {
  RemoveRefreshPath(path);
  std::string childPrefix = MakeChildPrefix(path);
  std::erase_if(GetValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
  auto& complexValues = GetComplexValues();
  for (auto it = complexValues.begin(); it != complexValues.end();) {
    if (IsPathOrDescendant(it->first, path, childPrefix)) {
      it = complexValues.erase(it);
    } else if (IsPathOrDescendant(path, it->first)) {
      it->second->RemoveRetainedPath(path);
      ++it;
    } else {
      ++it;
    }
  }
  std::erase_if(GetNativeComplexValues(), [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
}

void RemovePath(std::string_view path) {
  std::string normalized = NormalizePath(path);
  table::InvalidatePendingPublications(normalized);
  {
    py::gil_scoped_release release;
    wpi::tunables::TunableRegistry::Remove(normalized);
  }
  RemoveRetainedPath(normalized);
}

void RemoveValue(py::handle value) {
  std::vector<std::string> paths;

  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    for (auto&& [path, retained] : GetValues()) {
      if (retained == tunable) {
        paths.emplace_back(path);
      }
    }
  }

  for (auto&& [path, retained] : GetComplexValues()) {
    if (retained->IsValue(value)) {
      paths.emplace_back(path);
    }
  }

  for (auto&& [path, retained] : GetNativeComplexValues()) {
    if (retained.is(value)) {
      paths.emplace_back(path);
    }
  }

  for (auto&& path : paths) {
    RemovePath(path);
  }
}

void InitializeTunablePython(py::module_& module) {
  RegisterPreUpdateCallback();

  static int unused;
  py::capsule cleanup(&unused, [](void*) {
    py::gil_scoped_acquire gil;
    CleanupPythonStorage();
  });
  module.add_object("_tunable_cleanup", cleanup);
}

}  // namespace wpi::tunables::python
