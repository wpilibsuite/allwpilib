#pragma once

#include <stdint.h>

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/typing.h>

#include "wpi/tunables/Tunable.hpp"
#include "wpystruct.h"

namespace wpi::tunables::python {

/**
 * A Python-backed tunable value.
 *
 * Tunables can be published with publish() or added and published in one step
 * with one of the add() functions.
 */
class PyTunable : public std::enable_shared_from_this<PyTunable> {
 public:
  using Getter = pybind11::typing::Callable<pybind11::object()>;
  using Setter = pybind11::typing::Callable<void(pybind11::object)>;
  using TuneCallback = pybind11::typing::Callable<void(pybind11::object)>;
  using PythonType = pybind11::typing::Type<pybind11::object>;
  using Properties = pybind11::typing::Dict<pybind11::str, pybind11::object>;

  /**
   * Creates an unpublished tunable. The tunable type is inferred from value
   * unless an explicit type selector is provided. Bytes, bytearray, and
   * memoryview values infer a raw tunable. Wrap other buffer exporters in
   * memoryview to explicitly use their raw bytes in C order.
   *
   * @param value initial value
   * @param getter optional function that supplies the current local value
   * @param setter optional function that receives values set locally or
   * remotely
   * @param onTune callback that receives the value after a remote update
   * @param robust whether to separately echo a remotely set value
   * @param isMutable whether remote updates may change the tunable
   * @param valueType explicit value type, or None to infer it from value
   * @param elementType explicit sequence element type, or None to infer it
   * @param properties additional tunable properties
   * @param typeString custom tunable type string
   * @param alwaysGet whether to call getter on every backend update
   */
  PyTunable(
      pybind11::object value, std::optional<Getter> getter = std::nullopt,
      std::optional<Setter> setter = std::nullopt,
      std::optional<TuneCallback> onTune = std::nullopt, bool robust = false,
      bool isMutable = true,
      pybind11::typing::Optional<PythonType> valueType = pybind11::none(),
      pybind11::typing::Optional<PythonType> elementType = pybind11::none(),
      std::optional<Properties> properties = std::nullopt,
      std::string typeString = "", bool alwaysGet = false,
      bool narrowScalar = false);

  wpi::tunables::detail::TunableBase& GetBase();

  /**
   * Gets the current local value. If a getter was provided, this calls it;
   * otherwise, it returns the stored value.
   *
   * @return current value
   */
  pybind11::object Get() const;

  /**
   * Sets the local value and marks the tunable as changed. If a setter was
   * provided, it is called with value. If a getter was also provided, its
   * result becomes the stored value.
   *
   * @param value new value
   */
  void Set(pybind11::object value);

  /**
   * Gets the current individual WPIStruct object for in-place mutation and
   * marks the tunable as changed. The object is returned without copying it.
   * If a getter was provided, it supplies the current object.
   *
   * Raises TypeError for all other types, including struct arrays. Use get(),
   * edit the value, and call set() for those types instead.
   * Call mutate() again before further edits after a backend update.
   *
   * @return struct object to mutate
   */
  pybind11::object Mutate();

  void Refresh();
  bool NeedsRefresh() const;

 private:
  class CallbackOwner;

  using TunableVariant = std::variant<
      wpi::tunables::TunableBool, wpi::tunables::TunableInt32,
      wpi::tunables::TunableInt64, wpi::tunables::TunableFloat,
      wpi::tunables::TunableDouble, wpi::tunables::TunableString,
      wpi::tunables::TunableRaw, wpi::tunables::TunableBoolVector,
      wpi::tunables::TunableInt64Vector, wpi::tunables::TunableDoubleVector,
      wpi::tunables::TunableStringVector,
      wpi::tunables::Tunable<WPyStruct, WPyStructInfo>,
      wpi::tunables::Tunable<std::vector<WPyStruct>, WPyStructInfo>>;

  template <typename T>
  static bool CachedValuesEqual(const T& lhs, const T& rhs);

  template <typename T>
  static constexpr bool IsStructCachedValue =
      std::same_as<T, WPyStruct> || std::same_as<T, std::vector<WPyStruct>>;

  static std::vector<uint8_t> PackStructValue(const WPyStruct& value,
                                              const WPyStructInfo& info);

  template <typename T>
  static std::optional<std::vector<uint8_t>> PackStructData(const T&);

  static std::optional<std::vector<uint8_t>> PackStructData(
      const WPyStruct& value);
  static std::optional<std::vector<uint8_t>> PackStructData(
      const std::vector<WPyStruct>& values);

  template <typename T>
  static T ToCachedValue(pybind11::handle value);

  pybind11::object GetCached() const;
  void SetCached(pybind11::handle value);
  void SetCachedIfChanged(pybind11::handle value);
  std::optional<std::vector<uint8_t>> PackCachedStructData() const;

  wpi::tunables::TunableConfig MakeConfig(
      bool robust, bool isMutable, const std::optional<Properties>& properties,
      std::string typeString, bool alwaysGet);
  TunableVariant MakeValue(
      pybind11::handle value, bool robust, bool isMutable,
      const pybind11::typing::Optional<PythonType>& valueType,
      const pybind11::typing::Optional<PythonType>& elementType,
      const std::optional<Properties>& properties, std::string typeString,
      bool alwaysGet, bool narrowScalar);

  std::optional<Getter> m_getter;
  std::optional<Setter> m_setter;
  std::optional<TuneCallback> m_onTune;
  std::shared_ptr<CallbackOwner> m_callbackOwner;
  TunableVariant m_value;
  std::optional<std::vector<uint8_t>> m_lastStructData;
};

}  // namespace wpi::tunables::python
