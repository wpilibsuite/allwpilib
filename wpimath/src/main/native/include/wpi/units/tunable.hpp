// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <utility>

#include <wpi/tunables/Tunable.hpp>
#include <wpi/tunables/TunableConfig.hpp>
#include <wpi/util/ct_string.hpp>

#include "wpi/units/core.hpp"

namespace wpi::units {

namespace detail {
template <class Units>
wpi::tunables::TunableConfig MakeUnitTunableConfig() {
  wpi::tunables::TunableConfig config;
  config.properties["unit"] = std::string{ComplexAbbrev<Units>()};
  return config;
}

template <class Units>
wpi::tunables::TunableConfig MakeUnitTunableConfig(
    const wpi::tunables::TunableConfig& config) {
  wpi::tunables::TunableConfig unitConfig = config;
  unitConfig.properties["unit"] = std::string{ComplexAbbrev<Units>()};
  return unitConfig;
}

template <class Units, typename T, template <typename> class NonLinearScale>
class TunableUnit {
  using BaseUnits =
      unit<std::ratio<1>, typename traits::unit_traits<Units>::base_unit_type>;

 public:
  using ValueType = unit_t<Units, T, NonLinearScale>;

  TunableUnit() : m_tunable{MakeUnitTunableConfig<Units>()} {}
  explicit TunableUnit(ValueType val)
      : m_tunable{convert<Units, BaseUnits>(val).template to<T>(),
                  MakeUnitTunableConfig<Units>()} {}
  explicit TunableUnit(const wpi::tunables::TunableConfig& config)
      : m_tunable{MakeUnitTunableConfig<Units>(config)} {}
  TunableUnit(ValueType val, const wpi::tunables::TunableConfig& config)
      : m_tunable{convert<Units, BaseUnits>(val).template to<T>(),
                  MakeUnitTunableConfig<Units>(config)} {}

  ValueType Get() const {
    return unit_t<BaseUnits, T, NonLinearScale>{m_tunable.Get()};
  }
  void Set(ValueType value) {
    m_tunable = convert<Units, BaseUnits>(value).template to<T>();
  }

  wpi::tunables::Tunable<T>& GetInnerTunable() { return m_tunable; }

  template <typename U>
    requires traits::is_convertible_unit_t<ValueType, U>::value
  operator U() const {  // NOLINT(google-explicit-constructor)
    return Get();
  }

 private:
  wpi::tunables::Tunable<T> m_tunable;
};

template <class Units, typename T, template <typename> class NonLinearScale>
class TunableMemberValue
    : public wpi::tunables::detail::TunableMemberValueBase<double> {
  using BaseUnits =
      unit<std::ratio<1>, typename traits::unit_traits<Units>::base_unit_type>;

 public:
  using ValueType = unit_t<Units, T, NonLinearScale>;

  template <std::derived_from<wpi::tunables::ComplexTunable> Class>
  explicit TunableMemberValue(ValueType Class::* member)
      : TunableMemberValueBase<double>{MakeUnitTunableConfig<Units>()},
        m_ptr{member} {}

  template <std::derived_from<wpi::tunables::ComplexTunable> Class>
  explicit TunableMemberValue(ValueType Class::* member,
                              const wpi::tunables::TunableConfig& config)
      : TunableMemberValueBase<double>{MakeUnitTunableConfig<Units>(config)},
        m_ptr{member} {}

  const double& Get(const wpi::tunables::ComplexTunable* obj) const override {
    m_value = convert<Units, BaseUnits>(m_ptr.Get(obj)).template to<T>();
    return m_value;
  }

  void Set(wpi::tunables::ComplexTunable* obj, double value) override {
    m_ptr.Get(obj) = unit_t<BaseUnits, T, NonLinearScale>{value};
    this->SetTunableChanged();
  }

 private:
  mutable double m_value;
  wpi::tunables::detail::TunableMemberPointer<ValueType> m_ptr;
};
}  // namespace detail

template <class Units, typename T, template <typename> class NonLinearScale>
inline detail::TunableUnit<Units, T, NonLinearScale> GetCustomTunable(
    const unit_t<Units, T, NonLinearScale>& value) {
  return {value};
}

template <class Units, typename T, template <typename> class NonLinearScale,
          typename Class, typename... Args>
inline std::unique_ptr<wpi::tunables::detail::TunableMemberBase>
MakeTunableMember(unit_t<Units, T, NonLinearScale> Class::* member,
                  Args&&... args) {
  return std::make_unique<detail::TunableMemberValue<Units, T, NonLinearScale>>(
      member, std::forward<Args>(args)...);
}

}  // namespace wpi::units
