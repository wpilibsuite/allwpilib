// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/tunable/Tunable.hpp>
#include <wpi/util/ct_string.hpp>

#include "wpi/units/base.hpp"

namespace wpi::units {

namespace detail {
template <class Units, typename T, template <typename> class NonLinearScale>
class TunableUnit {
  using BaseUnits =
      unit<std::ratio<1>, typename traits::unit_traits<Units>::base_unit_type>;

 public:
  using ValueType = unit_t<Units, T, NonLinearScale>;

  TunableUnit() = default;
  explicit TunableUnit(ValueType val)
      : m_tunable{convert<Units, BaseUnits>(val).template to<T>()} {}

  ValueType Get() const {
    return unit_t<BaseUnits, T, NonLinearScale>{m_tunable.Get()};
  }
  void Set(ValueType value) {
    m_tunable = convert<Units, BaseUnits>(value).template to<T>();
  }

  Tunable<T>& GetInnerTunable() { return m_tunable; }

  template <typename U>
    requires traits::is_convertible_unit_t<ValueType, U>::value
  operator U() const {  // NOLINT(google-explicit-constructor)
    return Get();
  }

 private:
  Tunable<T> m_tunable;
};

template <class Units, typename T, template <typename> class NonLinearScale>
class TunableMemberValue : public wpi::detail::TunableMemberValueBase<double> {
  using BaseUnits =
      unit<std::ratio<1>, typename traits::unit_traits<Units>::base_unit_type>;

 public:
  using ValueType = unit_t<Units, T, NonLinearScale>;

  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberValue(ValueType Class::*member) : m_ptr{member} {}

  template <std::derived_from<ComplexTunable> Class>
  explicit TunableMemberValue(ValueType Class::*member,
                              const TunableConfig& config)
      : TunableMemberValueBase<double>{config}, m_ptr{member} {}

  const double& Get(const ComplexTunable* obj) const override {
    m_value = convert<Units, BaseUnits>(m_ptr.Get(obj)).template to<T>();
    return m_value;
  }

  void Set(ComplexTunable* obj, double value) override {
    m_ptr.Get(obj) = unit_t<BaseUnits, T, NonLinearScale>{value};
  }

 private:
  mutable double m_value;
  wpi::detail::TunableMemberPointer<ValueType> m_ptr;
};
}  // namespace detail

template <class Units, typename T, template <typename> class NonLinearScale>
inline detail::TunableUnit<Units, T, NonLinearScale> GetCustomTunable(
    const unit_t<Units, T, NonLinearScale>& value) {
  return {value};
}

template <class Units, typename T, template <typename> class NonLinearScale,
          typename Class, typename... Args>
inline std::unique_ptr<wpi::detail::TunableMemberBase> MakeTunableMember(
    unit_t<Units, T, NonLinearScale> Class::*member, Args&&... args) {
  return std::make_unique<detail::TunableMemberValue<Units, T, NonLinearScale>>(
      member, std::forward<Args>(args)...);
}

}  // namespace wpi::units
