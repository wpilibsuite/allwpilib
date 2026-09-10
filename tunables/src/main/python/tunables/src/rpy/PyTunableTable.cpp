#include "PyTunableTable.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "PyComplexTunableAdapter.h"
#include "TunableStorage.h"
#include "TunableValuePython.h"
#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/TunableRegistry.hpp"

namespace py = pybind11;

namespace wpi::tunables::python::table {
namespace {

struct ContextEntry {
  std::shared_ptr<TunableTableOwnerContext> ownerContext;
  std::uint64_t generation;
  py::weakref table;
};

std::unordered_map<wpi::tunables::TunableTable*, ContextEntry>& GetContexts() {
  static std::unordered_map<wpi::tunables::TunableTable*, ContextEntry>
      contexts;
  return contexts;
}

std::uint64_t NextGeneration() {
  static std::uint64_t generation;
  return ++generation;
}

std::unordered_map<std::string, std::uint64_t>& GetPendingPublications() {
  static std::unordered_map<std::string, std::uint64_t> publications;
  return publications;
}

auto LockForComplexPublication() {
  std::unique_lock lock{wpi::tunables::TunableRegistry::GetUpdateMutex(),
                        std::defer_lock};
  {
    py::gil_scoped_release release;
    lock.lock();
  }
  return lock;
}

class PendingPublication {
 public:
  explicit PendingPublication(std::string path)
      : m_path{std::move(path)}, m_generation{NextGeneration()} {
    auto& publications = GetPendingPublications();
    auto [it, inserted] = publications.try_emplace(m_path, m_generation);
    if (!inserted) {
      m_previousGeneration = it->second;
      it->second = m_generation;
    }
  }

  ~PendingPublication() { Resolve(false); }

  bool CanRetain(bool published) {
    bool isCurrent = Resolve(published);
    return published && isCurrent;
  }

 private:
  bool Resolve(bool published) {
    auto& publications = GetPendingPublications();
    auto it = publications.find(m_path);
    bool isCurrent = it != publications.end() && it->second == m_generation;
    if (isCurrent) {
      if (!published && m_previousGeneration) {
        it->second = *m_previousGeneration;
      } else {
        publications.erase(it);
      }
    }
    m_generation = 0;
    return isCurrent;
  }

  std::string m_path;
  std::uint64_t m_generation;
  std::optional<std::uint64_t> m_previousGeneration;
};

template <typename T>
Getter EraseGetter(const TypedGetter<T>& getter) {
  return py::reinterpret_borrow<Getter>(getter);
}

template <typename T>
Setter EraseSetter(const TypedSetter<T>& setter) {
  return py::reinterpret_borrow<Setter>(setter);
}

template <typename T>
std::optional<TuneCallback> EraseTuneCallback(
    const std::optional<TypedTuneCallback<T>>& callback) {
  if (!callback) {
    return std::nullopt;
  }
  return py::reinterpret_borrow<TuneCallback>(*callback);
}

std::shared_ptr<TunableTableOwnerContext> GetOwnerContext(
    wpi::tunables::TunableTable& table) {
  auto& contexts = GetContexts();
  auto it = contexts.find(&table);
  if (it == contexts.end()) {
    return nullptr;
  }
  if (it->second.table().is_none()) {
    contexts.erase(it);
    return nullptr;
  }
  return it->second.ownerContext;
}

std::shared_ptr<PyComplexTunableAdapter> GetOwner(
    wpi::tunables::TunableTable& table) {
  auto ownerContext = GetOwnerContext(table);
  if (!ownerContext) {
    return nullptr;
  }
  auto owner = ownerContext->owner.lock();
  if (!owner) {
    throw std::runtime_error("callback TunableTable owner is no longer valid");
  }
  return owner;
}

void StoreValue(wpi::tunables::TunableTable& table, std::string_view name,
                std::shared_ptr<PyTunable> tunable,
                const std::shared_ptr<PyComplexTunableAdapter>& owner) {
  std::string path = NormalizeTablePath(table, name);
  if (owner) {
    owner->AddValue(path, tunable);
  } else {
    detail::StoreValue(path, tunable);
  }
  StoreRefreshValue(path, tunable);
}

}  // namespace

py::object MakePythonTable(
    wpi::tunables::TunableTable table,
    std::shared_ptr<TunableTableOwnerContext> ownerContext) {
  py::object pythonTable = py::cast(std::move(table));
  if (!ownerContext) {
    return pythonTable;
  }

  auto* tablePtr = pythonTable.cast<wpi::tunables::TunableTable*>();
  std::uint64_t generation = NextGeneration();
  py::cpp_function cleanup{[tablePtr, generation](py::handle) {
    auto& contexts = GetContexts();
    auto it = contexts.find(tablePtr);
    if (it != contexts.end() && it->second.generation == generation) {
      contexts.erase(it);
    }
  }};
  GetContexts().insert_or_assign(
      tablePtr, ContextEntry{std::move(ownerContext), generation,
                             py::weakref{pythonTable, cleanup}});
  return pythonTable;
}

py::object GetTable(wpi::tunables::TunableTable& table, std::string_view name) {
  auto ownerContext = GetOwnerContext(table);
  return MakePythonTable(table.GetTable(name), std::move(ownerContext));
}

bool Publish(wpi::tunables::TunableTable& table, std::string_view name,
             py::object value) {
  auto owner = GetOwner(table);
  bool published;
  if (py::isinstance<PyTunable>(value)) {
    auto tunable = value.cast<std::shared_ptr<PyTunable>>();
    {
      std::string nameString{name};
      py::gil_scoped_release release;
      published = table.Publish(nameString, tunable->GetBase());
    }
    if (published) {
      StoreValue(table, name, std::move(tunable), owner);
    }
  } else if (py::isinstance<wpi::tunables::ComplexTunable>(value)) {
    auto& tunable = value.cast<wpi::tunables::ComplexTunable&>();
    std::string path = NormalizeTablePath(table, name);
    auto updateLock = LockForComplexPublication();
    PendingPublication pending{path};
    published = table.Publish(name, tunable);
    if (pending.CanRetain(published)) {
      if (owner) {
        owner->AddNativeComplex(std::move(path), std::move(value));
      } else {
        detail::StoreNativeComplexValue(std::move(path), std::move(value));
      }
    }
  } else if (auto publishTunable = GetOptionalAttr(value, "publish_tunables")) {
    auto tunable = std::make_shared<PyComplexTunableAdapter>(
        std::move(value), std::move(*publishTunable));
    std::string path = NormalizeTablePath(table, name);
    auto updateLock = LockForComplexPublication();
    PendingPublication pending{path};
    published = table.Publish(name, *tunable);
    if (pending.CanRetain(published)) {
      if (owner) {
        owner->AddComplex(std::move(path), std::move(tunable));
      } else {
        detail::StoreComplex(std::move(path), std::move(tunable));
      }
    }
  } else {
    throw py::type_error(
        "value must be a wpi::tunables::Tunable or "
        "wpi::tunables::ComplexTunable");
  }
  return published;
}

namespace {

std::shared_ptr<PyTunable> AddImpl(wpi::tunables::TunableTable& table,
                                   std::string_view name, py::object value,
                                   py::typing::Optional<PythonType> valueType,
                                   py::typing::Optional<PythonType> elementType,
                                   bool robust, bool isMutable,
                                   std::optional<TuneCallback> onTune,
                                   std::optional<Properties> properties,
                                   std::string typeString, bool narrowScalar) {
  auto owner = GetOwner(table);
  auto tunable = std::make_shared<PyTunable>(
      std::move(value), std::nullopt, std::nullopt, std::move(onTune), robust,
      isMutable, std::move(valueType), std::move(elementType),
      std::move(properties), std::move(typeString), false, narrowScalar);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = table.Publish(nameString, tunable->GetBase());
  }
  if (published) {
    StoreValue(table, name, tunable, owner);
  }
  return tunable;
}

}  // namespace

std::shared_ptr<PyTunable> Add(wpi::tunables::TunableTable& table,
                               std::string_view name, py::object value,
                               py::typing::Optional<PythonType> valueType,
                               py::typing::Optional<PythonType> elementType,
                               bool robust, bool isMutable,
                               std::optional<TuneCallback> onTune,
                               std::optional<Properties> properties,
                               std::string typeString) {
  return AddImpl(table, name, std::move(value), std::move(valueType),
                 std::move(elementType), robust, isMutable, std::move(onTune),
                 std::move(properties), std::move(typeString), false);
}

std::shared_ptr<PyTunable> AddBoolean(
    wpi::tunables::TunableTable& table, std::string_view name, bool value,
    bool robust, bool isMutable,
    std::optional<TypedTuneCallback<BoolCallbackValue>> onTune,
    std::optional<Properties> properties, std::string typeString) {
  return Add(table, name, py::cast(value), BuiltinType(&PyBool_Type),
             py::none(), robust, isMutable, EraseTuneCallback(onTune),
             std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> AddInt(
    wpi::tunables::TunableTable& table, std::string_view name,
    std::int32_t value, bool robust, bool isMutable,
    std::optional<TypedTuneCallback<IntCallbackValue>> onTune,
    std::optional<Properties> properties, std::string typeString) {
  return AddImpl(table, name, py::cast(value), BuiltinType(&PyLong_Type),
                 py::none(), robust, isMutable, EraseTuneCallback(onTune),
                 std::move(properties), std::move(typeString), true);
}

std::shared_ptr<PyTunable> AddLong(
    wpi::tunables::TunableTable& table, std::string_view name,
    std::int64_t value, bool robust, bool isMutable,
    std::optional<TypedTuneCallback<IntCallbackValue>> onTune,
    std::optional<Properties> properties, std::string typeString) {
  return Add(table, name, py::cast(value), BuiltinType(&PyLong_Type),
             py::none(), robust, isMutable, EraseTuneCallback(onTune),
             std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> AddFloat(
    wpi::tunables::TunableTable& table, std::string_view name, float value,
    bool robust, bool isMutable,
    std::optional<TypedTuneCallback<FloatCallbackValue>> onTune,
    std::optional<Properties> properties, std::string typeString) {
  return AddImpl(table, name, py::cast(value), BuiltinType(&PyFloat_Type),
                 py::none(), robust, isMutable, EraseTuneCallback(onTune),
                 std::move(properties), std::move(typeString), true);
}

std::shared_ptr<PyTunable> AddDouble(
    wpi::tunables::TunableTable& table, std::string_view name, double value,
    bool robust, bool isMutable,
    std::optional<TypedTuneCallback<FloatCallbackValue>> onTune,
    std::optional<Properties> properties, std::string typeString) {
  return Add(table, name, py::cast(value), BuiltinType(&PyFloat_Type),
             py::none(), robust, isMutable, EraseTuneCallback(onTune),
             std::move(properties), std::move(typeString));
}

namespace {

std::shared_ptr<PyTunable> PublishValueImpl(
    wpi::tunables::TunableTable& table, std::string_view name, Getter getter,
    Setter setter, py::typing::Optional<PythonType> valueType,
    py::typing::Optional<PythonType> elementType, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString,
    bool narrowScalar) {
  auto owner = GetOwner(table);
  py::object initialValue = getter();
  auto value = std::make_shared<PyTunable>(
      std::move(initialValue), std::move(getter), std::move(setter),
      std::nullopt, robust, isMutable, std::move(valueType),
      std::move(elementType), std::move(properties), std::move(typeString),
      false, narrowScalar);
  bool published;
  {
    std::string nameString{name};
    py::gil_scoped_release release;
    published = table.Publish(nameString, value->GetBase());
  }
  if (published) {
    StoreValue(table, name, value, owner);
  }
  return value;
}

}  // namespace

std::shared_ptr<PyTunable> PublishValue(
    wpi::tunables::TunableTable& table, std::string_view name, Getter getter,
    Setter setter, py::typing::Optional<PythonType> valueType,
    py::typing::Optional<PythonType> elementType, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString) {
  return PublishValueImpl(table, name, std::move(getter), std::move(setter),
                          std::move(valueType), std::move(elementType), robust,
                          isMutable, std::move(properties),
                          std::move(typeString), false);
}

std::shared_ptr<PyTunable> PublishBoolean(wpi::tunables::TunableTable& table,
                                          std::string_view name,
                                          TypedGetter<BoolCallbackValue> getter,
                                          TypedSetter<BoolCallbackValue> setter,
                                          bool robust, bool isMutable,
                                          std::optional<Properties> properties,
                                          std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      BuiltinType(&PyBool_Type), py::none(), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishInt(wpi::tunables::TunableTable& table,
                                      std::string_view name,
                                      TypedGetter<IntCallbackValue> getter,
                                      TypedSetter<IntCallbackValue> setter,
                                      bool robust, bool isMutable,
                                      std::optional<Properties> properties,
                                      std::string typeString) {
  return PublishValueImpl(table, name, EraseGetter(getter), EraseSetter(setter),
                          BuiltinType(&PyLong_Type), py::none(), robust,
                          isMutable, std::move(properties),
                          std::move(typeString), true);
}

std::shared_ptr<PyTunable> PublishLong(wpi::tunables::TunableTable& table,
                                       std::string_view name,
                                       TypedGetter<IntCallbackValue> getter,
                                       TypedSetter<IntCallbackValue> setter,
                                       bool robust, bool isMutable,
                                       std::optional<Properties> properties,
                                       std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      BuiltinType(&PyLong_Type), py::none(), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishFloat(wpi::tunables::TunableTable& table,
                                        std::string_view name,
                                        TypedGetter<FloatCallbackValue> getter,
                                        TypedSetter<FloatCallbackValue> setter,
                                        bool robust, bool isMutable,
                                        std::optional<Properties> properties,
                                        std::string typeString) {
  return PublishValueImpl(table, name, EraseGetter(getter), EraseSetter(setter),
                          BuiltinType(&PyFloat_Type), py::none(), robust,
                          isMutable, std::move(properties),
                          std::move(typeString), true);
}

std::shared_ptr<PyTunable> PublishDouble(wpi::tunables::TunableTable& table,
                                         std::string_view name,
                                         TypedGetter<FloatCallbackValue> getter,
                                         TypedSetter<FloatCallbackValue> setter,
                                         bool robust, bool isMutable,
                                         std::optional<Properties> properties,
                                         std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      BuiltinType(&PyFloat_Type), py::none(), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishString(
    wpi::tunables::TunableTable& table, std::string_view name,
    TypedGetter<StringCallbackValue> getter,
    TypedSetter<StringCallbackValue> setter, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      BuiltinType(&PyUnicode_Type), py::none(), robust,
                      isMutable, std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishBooleanArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<BoolCallbackValue> getter,
    ArraySetter<BoolCallbackValue> setter, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      py::none(), BuiltinType(&PyBool_Type), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishIntegerArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<IntCallbackValue> getter, ArraySetter<IntCallbackValue> setter,
    bool robust, bool isMutable, std::optional<Properties> properties,
    std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      py::none(), BuiltinType(&PyLong_Type), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishDoubleArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<FloatCallbackValue> getter,
    ArraySetter<FloatCallbackValue> setter, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      py::none(), BuiltinType(&PyFloat_Type), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishStringArray(
    wpi::tunables::TunableTable& table, std::string_view name,
    ArrayGetter<StringCallbackValue> getter,
    ArraySetter<StringCallbackValue> setter, bool robust, bool isMutable,
    std::optional<Properties> properties, std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      py::none(), BuiltinType(&PyUnicode_Type), robust,
                      isMutable, std::move(properties), std::move(typeString));
}

std::shared_ptr<PyTunable> PublishRaw(wpi::tunables::TunableTable& table,
                                      std::string_view name,
                                      TypedGetter<BytesCallbackValue> getter,
                                      TypedSetter<BytesCallbackValue> setter,
                                      bool robust, bool isMutable,
                                      std::optional<Properties> properties,
                                      std::string typeString) {
  return PublishValue(table, name, EraseGetter(getter), EraseSetter(setter),
                      BuiltinType(&PyBytes_Type), py::none(), robust, isMutable,
                      std::move(properties), std::move(typeString));
}

void Remove(wpi::tunables::TunableTable& table, std::string_view name) {
  std::string path = NormalizeTablePath(table, name);
  if (auto owner = GetOwner(table)) {
    owner->RemovePath(path);
  } else {
    RemovePath(path);
  }
}

void InvalidatePendingPublications(std::string_view path) {
  std::string childPrefix{path};
  if (childPrefix.empty() || childPrefix.back() != '/') {
    childPrefix.push_back('/');
  }
  std::erase_if(GetPendingPublications(), [&](const auto& publication) {
    return publication.first == path ||
           publication.first.starts_with(childPrefix);
  });
}

void ClearContexts() {
  GetContexts().clear();
  GetPendingPublications().clear();
}

}  // namespace wpi::tunables::python::table
