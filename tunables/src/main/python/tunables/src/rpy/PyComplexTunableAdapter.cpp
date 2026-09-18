#include "PyComplexTunableAdapter.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "PyTunableTable.h"
#include "TunableStorage.h"
#include "TunableValuePython.h"
#include "wpi/tunables/TunableRegistry.hpp"

namespace py = pybind11;

namespace wpi::tunables::python {
namespace {

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

}  // namespace

PyComplexTunableAdapter::PyComplexTunableAdapter(
    py::object value, py::object initialPublishTunable)
    : m_tableOwnerContext{std::make_shared<TunableTableOwnerContext>()},
      m_value{std::move(value)},
      m_initialPublishTunable{std::move(initialPublishTunable)} {
  if (auto getTunableType = GetOptionalAttr(m_value, "get_tunable_type")) {
    py::object typeObj = (*getTunableType)();
    if (!typeObj.is_none()) {
      m_type = typeObj.cast<std::string>();
    }
  }
}

std::string_view PyComplexTunableAdapter::GetTunableType() const {
  return m_type;
}

bool PyComplexTunableAdapter::IsValue(py::handle value) const {
  return m_value.is(value);
}

void PyComplexTunableAdapter::PublishTunable(
    wpi::tunables::TunableTable& table) {
  py::gil_scoped_acquire gil;
  py::object publishTunable;
  if (m_initialPublishTunable) {
    publishTunable = std::move(*m_initialPublishTunable);
    m_initialPublishTunable.reset();
  } else {
    publishTunable = m_value.attr("publish_tunables");
  }
  m_tableOwnerContext->owner = shared_from_this();
  publishTunable(table::MakePythonTable(wpi::tunables::TunableTable{table},
                                        m_tableOwnerContext));
}

void PyComplexTunableAdapter::UpdateTunable() const {
  py::gil_scoped_acquire gil;
  py::object updateTunable =
      py::getattr(m_value, "update_tunables", py::none());
  if (!updateTunable.is_none()) {
    updateTunable();
  }
}

void PyComplexTunableAdapter::AddValue(std::string path,
                                       std::shared_ptr<PyTunable> value) {
  for (auto&& child : m_values) {
    if (child.first == path) {
      child.second = std::move(value);
      return;
    }
  }
  m_values.emplace_back(std::move(path), std::move(value));
}

void PyComplexTunableAdapter::AddComplex(
    std::string path, std::shared_ptr<PyComplexTunableAdapter> value) {
  for (auto&& child : m_complex) {
    if (child.first == path) {
      child.second = std::move(value);
      return;
    }
  }
  m_complex.emplace_back(std::move(path), std::move(value));
}

void PyComplexTunableAdapter::AddNativeComplex(std::string path,
                                               py::object value) {
  for (auto&& child : m_nativeComplex) {
    if (child.first == path) {
      child.second = std::move(value);
      return;
    }
  }
  m_nativeComplex.emplace_back(std::move(path), std::move(value));
}

void PyComplexTunableAdapter::RemovePath(std::string_view path) {
  table::InvalidatePendingPublications(path);
  {
    py::gil_scoped_release release;
    wpi::tunables::TunableRegistry::Remove(path);
  }
  RemoveRetainedPath(path);
}

void PyComplexTunableAdapter::RemoveRetainedPath(std::string_view path) {
  RemoveRefreshPath(path);
  std::string childPrefix = MakeChildPrefix(path);
  std::erase_if(m_values, [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
  for (auto it = m_complex.begin(); it != m_complex.end();) {
    if (IsPathOrDescendant(it->first, path, childPrefix)) {
      it = m_complex.erase(it);
    } else if (IsPathOrDescendant(path, it->first)) {
      it->second->RemoveRetainedPath(path);
      ++it;
    } else {
      ++it;
    }
  }
  std::erase_if(m_nativeComplex, [&](auto&& child) {
    return IsPathOrDescendant(child.first, path, childPrefix);
  });
}

}  // namespace wpi::tunables::python
