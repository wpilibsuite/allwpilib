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

template <UnitType Unit>
class TunableUnit {
  using BaseUnits =
      unit<conversion_factor<std::ratio<1>,
                             wpi::units::traits::dimension_of_t<Unit>>>;
  using UnderlyingType = typename traits::unit_traits<Unit>::underlying_type;

 public:
  TunableUnit() : m_tunable{MakeUnitTunableConfig<Unit>()} {}
  explicit TunableUnit(Unit val)
      : m_tunable{BaseUnits(val).template to<UnderlyingType>(),
                  MakeUnitTunableConfig<Unit>()} {}
  explicit TunableUnit(const wpi::tunables::TunableConfig& config)
      : m_tunable{MakeUnitTunableConfig<Unit>(config)} {}
  TunableUnit(Unit val, const wpi::tunables::TunableConfig& config)
      : m_tunable{BaseUnits(val).template to<UnderlyingType>(),
                  MakeUnitTunableConfig<Unit>(config)} {}

  BaseUnits Get() const { return BaseUnits(m_tunable.Get()); }
  void Set(Unit value) {
    m_tunable = BaseUnits(value).template to<UnderlyingType>();
  }

  wpi::tunables::Tunable<UnderlyingType>& GetInnerTunable() {
    return m_tunable;
  }

  template <typename U>
    requires wpi::units::same_dimension<U, Unit>
  operator U() const {  // NOLINT(google-explicit-constructor)
    return Get();
  }

 private:
  wpi::tunables::Tunable<UnderlyingType> m_tunable;
};

template <UnitType Unit>
class TunableMemberValue
    : public wpi::tunables::detail::TunableMemberValueBase<double> {
  using BaseUnits =
      unit<conversion_factor<std::ratio<1>,
                             wpi::units::traits::dimension_of_t<Unit>>>;
  using UnderlyingType = typename traits::unit_traits<Unit>::underlying_type;

 public:
  template <std::derived_from<wpi::tunables::ComplexTunable> Class>
  explicit TunableMemberValue(Unit Class::* member)
      : TunableMemberValueBase<double>{MakeUnitTunableConfig<Unit>()},
        m_ptr{member} {}

  template <std::derived_from<wpi::tunables::ComplexTunable> Class>
  explicit TunableMemberValue(Unit Class::* member,
                              const wpi::tunables::TunableConfig& config)
      : TunableMemberValueBase<double>{MakeUnitTunableConfig<Unit>(config)},
        m_ptr{member} {}

  const double& Get(const wpi::tunables::ComplexTunable* obj) const override {
    m_value = BaseUnits(m_ptr.Get(obj)).template to<UnderlyingType>();
    return m_value;
  }

  void Set(wpi::tunables::ComplexTunable* obj, double value) override {
    m_ptr.Get(obj) = BaseUnits(value);
    this->SetTunableChanged();
  }

 private:
  mutable double m_value;
  wpi::tunables::detail::TunableMemberPointer<Unit> m_ptr;
};
}  // namespace detail

template <UnitType Unit>
inline detail::TunableUnit<Unit> GetCustomTunable(const Unit& value) {
  return {value};
}

template <UnitType Unit, typename Class, typename... Args>
inline std::unique_ptr<wpi::tunables::detail::TunableMemberBase>
MakeTunableMember(Unit Class::* member, Args&&... args) {
  return std::make_unique<detail::TunableMemberValue<Unit>>(
      member, std::forward<Args>(args)...);
}

}  // namespace wpi::units
