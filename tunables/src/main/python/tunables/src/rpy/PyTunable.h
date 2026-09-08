#pragma once

#include <stdint.h>

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <pybind11/pybind11.h>
#include <wpi/tunables/Tunable.hpp>
#include <wpi/tunables/TunableConfig.hpp>
#include <wpi/tunables/detail/TunableBase.hpp>
#include <wpystruct.h>

namespace wpi::tunables::python {

using TunableVariant =
    std::variant<wpi::tunables::TunableBool, wpi::tunables::TunableInt64,
                 wpi::tunables::TunableDouble, wpi::tunables::TunableString,
                 wpi::tunables::TunableRaw, wpi::tunables::TunableBoolVector,
                 wpi::tunables::TunableInt64Vector,
                 wpi::tunables::TunableDoubleVector,
                 wpi::tunables::TunableStringVector,
                 wpi::tunables::Tunable<WPyStruct, WPyStructInfo>,
                 wpi::tunables::Tunable<std::vector<WPyStruct>, WPyStructInfo>>;

class PyTunable : public std::enable_shared_from_this<PyTunable> {
 public:
  PyTunable(pybind11::object value, pybind11::object getter,
            pybind11::object setter, pybind11::object onTune, bool robust,
            bool isMutable, pybind11::object valueType,
            pybind11::object elementType, pybind11::object properties,
            std::string typeString, bool alwaysGet);

  wpi::tunables::detail::TunableBase& GetBase();
  pybind11::object Get() const;
  void Set(pybind11::handle value);
  pybind11::object Mutate();
  void Refresh();
  bool NeedsRefresh() const;

 private:
  friend class PyMutationList;

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
  pybind11::object MutateCached();
  void SetCached(pybind11::handle value);
  void SetCachedIfChanged(pybind11::handle value);
  std::optional<std::vector<uint8_t>> PackCachedStructData() const;
  wpi::tunables::TunableConfig MakeConfig(bool robust, bool isMutable,
                                          pybind11::handle properties,
                                          std::string typeString,
                                          bool alwaysGet);
  TunableVariant MakeValue(pybind11::handle value, bool robust, bool isMutable,
                           pybind11::object valueType,
                           pybind11::object elementType,
                           pybind11::object properties, std::string typeString,
                           bool alwaysGet);

  pybind11::object m_getter;
  pybind11::object m_setter;
  pybind11::object m_onTune;
  TunableVariant m_value;
  std::optional<std::vector<uint8_t>> m_lastStructData;
};

}  // namespace wpi::tunables::python
