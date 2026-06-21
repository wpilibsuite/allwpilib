// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>
#include <chrono>
#include <format>
#include <iomanip>
#ifdef UNIT_LIB_ENABLE_IOSTREAM
#include <iostream>
#endif
#include <sstream>
#include <string>
#include <type_traits>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "wpi/math/TestAssertions.hpp"

#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_jerk.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/area.hpp"
#include "wpi/units/capacitance.hpp"
#include "wpi/units/charge.hpp"
#include "wpi/units/concentration.hpp"
#include "wpi/units/conductance.hpp"
#include "wpi/units/constants.hpp"
#include "wpi/units/current.hpp"
#include "wpi/units/data.hpp"
#include "wpi/units/data_transfer_rate.hpp"
#include "wpi/units/density.hpp"
#include "wpi/units/dimensionless.hpp"
#include "wpi/units/energy.hpp"
#include "wpi/units/force.hpp"
#include "wpi/units/frequency.hpp"
#include "wpi/units/illuminance.hpp"
#include "wpi/units/impedance.hpp"
#include "wpi/units/inductance.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/luminous_flux.hpp"
#include "wpi/units/luminous_intensity.hpp"
#include "wpi/units/magnetic_field_strength.hpp"
#include "wpi/units/magnetic_flux.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/power.hpp"
#include "wpi/units/pressure.hpp"
#include "wpi/units/radiation.hpp"
#include "wpi/units/solid_angle.hpp"
#include "wpi/units/substance.hpp"
#include "wpi/units/temperature.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/torque.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/units/volume.hpp"

using namespace wpi::units::acceleration;
using namespace wpi::units::angle;
using namespace wpi::units::angular_acceleration;
using namespace wpi::units::angular_jerk;
using namespace wpi::units::angular_velocity;
using namespace wpi::units::area;
using namespace wpi::units::capacitance;
using namespace wpi::units::charge;
using namespace wpi::units::concentration;
using namespace wpi::units::conductance;
using namespace wpi::units::data;
using namespace wpi::units::data_transfer_rate;
using namespace wpi::units::density;
using namespace wpi::units::dimensionless;
using namespace wpi::units::energy;
using namespace wpi::units::frequency;
using namespace wpi::units::illuminance;
using namespace wpi::units::impedance;
using namespace wpi::units::inductance;
using namespace wpi::units::length;
using namespace wpi::units::luminous_flux;
using namespace wpi::units::luminous_intensity;
using namespace wpi::units::magnetic_field_strength;
using namespace wpi::units::magnetic_flux;
using namespace wpi::units::mass;
using namespace wpi::units::math;
using namespace wpi::units::power;
using namespace wpi::units::pressure;
using namespace wpi::units::radiation;
using namespace wpi::units::solid_angle;
using namespace wpi::units::temperature;
using namespace wpi::units::time;
using namespace wpi::units::torque;
using namespace wpi::units::velocity;
using namespace wpi::units::voltage;
using namespace wpi::units::volume;
using namespace wpi::units;

#if !defined(_MSC_VER) || _MSC_VER > 1800
using namespace wpi::units::literals;
#endif

namespace {

class TypeTraits {
 protected:
  TypeTraits() = default;
};

class UnitManipulators {
 protected:
  UnitManipulators() = default;
};

class UnitContainer {
 protected:
  UnitContainer() = default;
};

class UnitConversion {
 protected:
  UnitConversion() = default;
};

class UnitMath {
 protected:
  UnitMath() = default;
};

class CompileTimeArithmetic {
 protected:
  CompileTimeArithmetic() = default;
};

class Constexpr {
 protected:
  Constexpr() = default;
};

class CaseStudies {
 protected:
  CaseStudies() = default;

  struct RightTriangle {
    using a = unit_value_t<meters, 3>;
    using b = unit_value_t<meters, 4>;
    using c = unit_value_sqrt<
        unit_value_add<unit_value_power<a, 2>, unit_value_power<b, 2>>>;
  };
};
}  // namespace

TEST_CASE_METHOD(TypeTraits, "TypeTraits isRatio", "[wpimath]") {
  CHECK(traits::is_ratio_v<std::ratio<1>>);
  CHECK_FALSE(traits::is_ratio_v<double>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits ratio sqrt", "[wpimath]") {
  using rt2 = ratio_sqrt<std::ratio<2>>;
  CHECK(std::abs(std::sqrt(2 / static_cast<double>(1)) -
                     rt2::num / static_cast<double>(rt2::den)) < 5e-9);

  using rt4 = ratio_sqrt<std::ratio<4>>;
  CHECK(std::abs(std::sqrt(4 / static_cast<double>(1)) -
                     rt4::num / static_cast<double>(rt4::den)) < 5e-9);

  using rt10 = ratio_sqrt<std::ratio<10>>;
  CHECK(std::abs(std::sqrt(10 / static_cast<double>(1)) -
                     rt10::num / static_cast<double>(rt10::den)) < 5e-9);

  using rt30 = ratio_sqrt<std::ratio<30>>;
  CHECK(std::abs(std::sqrt(30 / static_cast<double>(1)) -
                     rt30::num / static_cast<double>(rt30::den)) < 5e-9);

  using rt61 = ratio_sqrt<std::ratio<61>>;
  CHECK(std::abs(std::sqrt(61 / static_cast<double>(1)) -
                     rt61::num / static_cast<double>(rt61::den)) < 5e-9);

  using rt100 = ratio_sqrt<std::ratio<100>>;
  CHECK(std::abs(std::sqrt(100 / static_cast<double>(1)) -
                     rt100::num / static_cast<double>(rt100::den)) < 5e-9);

  using rt1000 = ratio_sqrt<std::ratio<1000>>;
  CHECK(std::abs(std::sqrt(1000 / static_cast<double>(1)) -
                     rt1000::num / static_cast<double>(rt1000::den)) < 5e-9);

  using rt10000 = ratio_sqrt<std::ratio<10000>>;
  CHECK(std::abs(std::sqrt(10000 / static_cast<double>(1)) -
                     rt10000::num / static_cast<double>(rt10000::den)) < 5e-9);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is unit v", "[wpimath]") {
  CHECK_FALSE(traits::is_unit_v<std::ratio<1>>);
  CHECK_FALSE(traits::is_unit_v<double>);
  CHECK(traits::is_unit_v<meters>);
  CHECK(traits::is_unit_v<feet>);
  CHECK(traits::is_unit_v<degrees_squared>);
  CHECK_FALSE(traits::is_unit_v<meter_t>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is unit t", "[wpimath]") {
  CHECK_FALSE(traits::is_unit_t_v<std::ratio<1>>);
  CHECK_FALSE(traits::is_unit_t_v<double>);
  CHECK_FALSE(traits::is_unit_t_v<meters>);
  CHECK_FALSE(traits::is_unit_t_v<feet>);
  CHECK_FALSE(traits::is_unit_t_v<degrees_squared>);
  CHECK(traits::is_unit_t_v<meter_t>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits unit traits", "[wpimath]") {
  CHECK((std::is_same_v<void,
                    traits::unit_traits<double>::conversion_ratio>));
  CHECK(!(std::is_same_v<void,
                    traits::unit_traits<meters>::conversion_ratio>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits unit t traits", "[wpimath]") {
  CHECK((std::is_same_v<void,
                    traits::unit_t_traits<double>::underlying_type>));
  CHECK((std::is_same_v<UNIT_LIB_DEFAULT_TYPE,
                    traits::unit_t_traits<meter_t>::underlying_type>));
  CHECK((std::is_same_v<void, traits::unit_t_traits<double>::value_type>));
  CHECK((std::is_same_v<UNIT_LIB_DEFAULT_TYPE,
                    traits::unit_t_traits<meter_t>::value_type>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits all true", "[wpimath]") {
  CHECK(all_true_t_v<true>);
  CHECK((all_true_t_v<true, true>));
  CHECK((all_true_t_v<true, true, true>));
  CHECK_FALSE(all_true_t_v<false>);
  CHECK_FALSE((all_true_t_v<true, false>));
  CHECK_FALSE((all_true_t_v<true, true, false>));
  CHECK_FALSE((all_true_t_v<false, false, false>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is convertible unit", "[wpimath]") {
  CHECK((traits::is_convertible_unit_v<meters, meters>));
  CHECK((traits::is_convertible_unit_v<meters, astronicalUnits>));
  CHECK((traits::is_convertible_unit_v<meters, parsecs>));

  CHECK((traits::is_convertible_unit_v<meters, meters>));
  CHECK((traits::is_convertible_unit_v<astronicalUnits, meters>));
  CHECK((traits::is_convertible_unit_v<parsecs, meters>));
  CHECK((traits::is_convertible_unit_v<years, weeks>));

  CHECK_FALSE((traits::is_convertible_unit_v<meters, seconds>));
  CHECK_FALSE((traits::is_convertible_unit_v<seconds, meters>));
  CHECK_FALSE((traits::is_convertible_unit_v<years, meters>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits inverse", "[wpimath]") {
  double test;

  using htz = inverse<seconds>;
  bool shouldBeTrue = std::is_same_v<htz, hertz>;
  CHECK(shouldBeTrue);

  test = convert<inverse<celsius>, inverse<fahrenheit>>(1.0);
  CHECK_NEAR(5.0 / 9.0, test, 5.0e-5);

  test = convert<inverse<kelvin>, inverse<fahrenheit>>(6.0);
  CHECK_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits base unit of", "[wpimath]") {
  using base = traits::base_unit_of<years>;
  bool shouldBeTrue = std::is_same_v<base, category::time_unit>;

  CHECK(shouldBeTrue);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits has linear scale", "[wpimath]") {
  CHECK(traits::has_linear_scale_v<scalar_t>);
  CHECK(traits::has_linear_scale_v<meter_t>);
  CHECK(traits::has_linear_scale_v<foot_t>);
  CHECK((traits::has_linear_scale_v<watt_t, scalar_t>));
  CHECK((traits::has_linear_scale_v<scalar_t, meter_t>));
  CHECK(traits::has_linear_scale_v<meters_per_second_t>);
  CHECK_FALSE(traits::has_linear_scale_v<dB_t>);
  CHECK_FALSE((traits::has_linear_scale_v<dB_t, meters_per_second_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits has decibel scale", "[wpimath]") {
  CHECK_FALSE(traits::has_decibel_scale_v<scalar_t>);
  CHECK_FALSE(traits::has_decibel_scale_v<meter_t>);
  CHECK_FALSE(traits::has_decibel_scale_v<foot_t>);
  CHECK(traits::has_decibel_scale_v<dB_t>);
  CHECK(traits::has_decibel_scale_v<dBW_t>);

  CHECK((traits::has_decibel_scale_v<dBW_t, dB_t>));
  CHECK((traits::has_decibel_scale_v<dBW_t, dBm_t>));
  CHECK((traits::has_decibel_scale_v<dB_t, dB_t>));
  CHECK((traits::has_decibel_scale_v<dB_t, dB_t, dB_t>));
  CHECK_FALSE((traits::has_decibel_scale_v<dB_t, dB_t, meter_t>));
  CHECK_FALSE((traits::has_decibel_scale_v<meter_t, dB_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is same scale", "[wpimath]") {
  CHECK((traits::is_same_scale_v<scalar_t, dimensionless_t>));
  CHECK((traits::is_same_scale_v<dB_t, dBW_t>));
  CHECK_FALSE((traits::is_same_scale_v<dB_t, scalar_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is dimensionless unit", "[wpimath]") {
  CHECK(traits::is_dimensionless_unit_v<scalar_t>);
  CHECK(traits::is_dimensionless_unit_v<const scalar_t>);
  CHECK(traits::is_dimensionless_unit_v<const scalar_t&>);
  CHECK(traits::is_dimensionless_unit_v<dimensionless_t>);
  CHECK(traits::is_dimensionless_unit_v<dB_t>);
  CHECK((traits::is_dimensionless_unit_v<dB_t, scalar_t>));
  CHECK(traits::is_dimensionless_unit_v<ppm_t>);
  CHECK_FALSE(traits::is_dimensionless_unit_v<meter_t>);
  CHECK_FALSE(traits::is_dimensionless_unit_v<dBW_t>);
  CHECK_FALSE((traits::is_dimensionless_unit_v<dBW_t, scalar_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is length unit", "[wpimath]") {
  CHECK(traits::is_length_unit_v<meter>);
  CHECK(traits::is_length_unit_v<cubit>);
  CHECK_FALSE(traits::is_length_unit_v<year>);
  CHECK_FALSE(traits::is_length_unit_v<double>);

  CHECK(traits::is_length_unit_v<meter_t>);
  CHECK(traits::is_length_unit_v<const meter_t>);
  CHECK(traits::is_length_unit_v<const meter_t&>);
  CHECK(traits::is_length_unit_v<cubit_t>);
  CHECK_FALSE(traits::is_length_unit_v<year_t>);
  CHECK((traits::is_length_unit_v<meter_t, cubit_t>));
  CHECK_FALSE((traits::is_length_unit_v<meter_t, year_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is mass unit", "[wpimath]") {
  CHECK(traits::is_mass_unit_v<kilogram>);
  CHECK(traits::is_mass_unit_v<stone>);
  CHECK_FALSE(traits::is_mass_unit_v<meter>);
  CHECK_FALSE(traits::is_mass_unit_v<double>);

  CHECK(traits::is_mass_unit_v<kilogram_t>);
  CHECK(traits::is_mass_unit_v<const kilogram_t>);
  CHECK(traits::is_mass_unit_v<const kilogram_t&>);
  CHECK(traits::is_mass_unit_v<stone_t>);
  CHECK_FALSE(traits::is_mass_unit_v<meter_t>);
  CHECK((traits::is_mass_unit_v<kilogram_t, stone_t>));
  CHECK_FALSE((traits::is_mass_unit_v<kilogram_t, meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is time unit", "[wpimath]") {
  CHECK(traits::is_time_unit_v<second>);
  CHECK(traits::is_time_unit_v<year>);
  CHECK_FALSE(traits::is_time_unit_v<meter>);
  CHECK_FALSE(traits::is_time_unit_v<double>);

  CHECK(traits::is_time_unit_v<second_t>);
  CHECK(traits::is_time_unit_v<const second_t>);
  CHECK(traits::is_time_unit_v<const second_t&>);
  CHECK(traits::is_time_unit_v<year_t>);
  CHECK_FALSE(traits::is_time_unit_v<meter_t>);
  CHECK((traits::is_time_unit_v<second_t, year_t>));
  CHECK_FALSE((traits::is_time_unit_v<second_t, meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is angle unit", "[wpimath]") {
  CHECK(traits::is_angle_unit_v<angle::radian>);
  CHECK(traits::is_angle_unit_v<angle::degree>);
  CHECK_FALSE(traits::is_angle_unit_v<watt>);
  CHECK_FALSE(traits::is_angle_unit_v<double>);

  CHECK(traits::is_angle_unit_v<angle::radian_t>);
  CHECK(traits::is_angle_unit_v<const angle::radian_t>);
  CHECK(traits::is_angle_unit_v<const angle::radian_t&>);
  CHECK(traits::is_angle_unit_v<angle::degree_t>);
  CHECK_FALSE(traits::is_angle_unit_v<watt_t>);
  CHECK((traits::is_angle_unit_v<angle::radian_t, angle::degree_t>));
  CHECK_FALSE((traits::is_angle_unit_v<angle::radian_t, watt_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is current unit", "[wpimath]") {
  CHECK(traits::is_current_unit_v<current::ampere>);
  CHECK_FALSE(traits::is_current_unit_v<volt>);
  CHECK_FALSE(traits::is_current_unit_v<double>);

  CHECK(traits::is_current_unit_v<current::ampere_t>);
  CHECK(traits::is_current_unit_v<const current::ampere_t>);
  CHECK(traits::is_current_unit_v<const current::ampere_t&>);
  CHECK_FALSE(traits::is_current_unit_v<volt_t>);
  CHECK((traits::is_current_unit_v<current::ampere_t,
                                       current::milliampere_t>));
  CHECK_FALSE((traits::is_current_unit_v<current::ampere_t, volt_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is temperature unit", "[wpimath]") {
  CHECK(traits::is_temperature_unit_v<fahrenheit>);
  CHECK(traits::is_temperature_unit_v<kelvin>);
  CHECK_FALSE(traits::is_temperature_unit_v<cubit>);
  CHECK_FALSE(traits::is_temperature_unit_v<double>);

  CHECK(traits::is_temperature_unit_v<fahrenheit_t>);
  CHECK(traits::is_temperature_unit_v<const fahrenheit_t>);
  CHECK(traits::is_temperature_unit_v<const fahrenheit_t&>);
  CHECK(traits::is_temperature_unit_v<kelvin_t>);
  CHECK_FALSE(traits::is_temperature_unit_v<cubit_t>);
  CHECK((traits::is_temperature_unit_v<fahrenheit_t, kelvin_t>));
  CHECK_FALSE((traits::is_temperature_unit_v<cubit_t, fahrenheit_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is substance unit", "[wpimath]") {
  CHECK(traits::is_substance_unit_v<substance::mol>);
  CHECK_FALSE(traits::is_substance_unit_v<year>);
  CHECK_FALSE(traits::is_substance_unit_v<double>);

  CHECK(traits::is_substance_unit_v<substance::mole_t>);
  CHECK(traits::is_substance_unit_v<const substance::mole_t>);
  CHECK(traits::is_substance_unit_v<const substance::mole_t&>);
  CHECK_FALSE(traits::is_substance_unit_v<year_t>);
  CHECK((traits::is_substance_unit_v<substance::mole_t, substance::mole_t>));
  CHECK_FALSE((traits::is_substance_unit_v<year_t, substance::mole_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is luminous intensity unit", "[wpimath]") {
  CHECK(traits::is_luminous_intensity_unit_v<candela>);
  CHECK_FALSE(traits::is_luminous_intensity_unit_v<wpi::units::radiation::rad>);
  CHECK_FALSE(traits::is_luminous_intensity_unit_v<double>);

  CHECK(traits::is_luminous_intensity_unit_v<candela_t>);
  CHECK(traits::is_luminous_intensity_unit_v<const candela_t>);
  CHECK(traits::is_luminous_intensity_unit_v<const candela_t&>);
  CHECK_FALSE(traits::is_luminous_intensity_unit_v<rad_t>);
  CHECK((traits::is_luminous_intensity_unit_v<candela_t, candela_t>));
  CHECK_FALSE((traits::is_luminous_intensity_unit_v<rad_t, candela_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is solid angle unit", "[wpimath]") {
  CHECK(traits::is_solid_angle_unit_v<steradian>);
  CHECK(traits::is_solid_angle_unit_v<degree_squared>);
  CHECK_FALSE(traits::is_solid_angle_unit_v<angle::degree>);
  CHECK_FALSE(traits::is_solid_angle_unit_v<double>);

  CHECK(traits::is_solid_angle_unit_v<steradian_t>);
  CHECK(traits::is_solid_angle_unit_v<const steradian_t>);
  CHECK(traits::is_solid_angle_unit_v<const degree_squared_t&>);
  CHECK_FALSE(traits::is_solid_angle_unit_v<angle::degree_t>);
  CHECK((traits::is_solid_angle_unit_v<degree_squared_t, steradian_t>));
  CHECK_FALSE((traits::is_solid_angle_unit_v<angle::degree_t, steradian_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is frequency unit", "[wpimath]") {
  CHECK(traits::is_frequency_unit_v<hertz>);
  CHECK_FALSE(traits::is_frequency_unit_v<second>);
  CHECK_FALSE(traits::is_frequency_unit_v<double>);

  CHECK(traits::is_frequency_unit_v<hertz_t>);
  CHECK(traits::is_frequency_unit_v<const hertz_t>);
  CHECK(traits::is_frequency_unit_v<const hertz_t&>);
  CHECK_FALSE(traits::is_frequency_unit_v<second_t>);
  CHECK((traits::is_frequency_unit_v<const hertz_t&, gigahertz_t>));
  CHECK_FALSE((traits::is_frequency_unit_v<second_t, hertz_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is velocity unit", "[wpimath]") {
  CHECK(traits::is_velocity_unit_v<meters_per_second>);
  CHECK(traits::is_velocity_unit_v<miles_per_hour>);
  CHECK_FALSE(traits::is_velocity_unit_v<meters_per_second_squared>);
  CHECK_FALSE(traits::is_velocity_unit_v<double>);

  CHECK(traits::is_velocity_unit_v<meters_per_second_t>);
  CHECK(traits::is_velocity_unit_v<const meters_per_second_t>);
  CHECK(traits::is_velocity_unit_v<const meters_per_second_t&>);
  CHECK(traits::is_velocity_unit_v<miles_per_hour_t>);
  CHECK_FALSE(traits::is_velocity_unit_v<meters_per_second_squared_t>);
  CHECK((traits::is_velocity_unit_v<miles_per_hour_t, meters_per_second_t>));
  CHECK_FALSE((traits::is_velocity_unit_v<meters_per_second_squared_t,
                                         meters_per_second_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is acceleration unit", "[wpimath]") {
  CHECK(traits::is_acceleration_unit_v<meters_per_second_squared>);
  CHECK(traits::is_acceleration_unit_v<acceleration::standard_gravity>);
  CHECK_FALSE(traits::is_acceleration_unit_v<inch>);
  CHECK_FALSE(traits::is_acceleration_unit_v<double>);

  CHECK(traits::is_acceleration_unit_v<meters_per_second_squared_t>);
  CHECK(traits::is_acceleration_unit_v<const meters_per_second_squared_t>);
  CHECK(traits::is_acceleration_unit_v<const meters_per_second_squared_t&>);
  CHECK(traits::is_acceleration_unit_v<standard_gravity_t>);
  CHECK_FALSE(traits::is_acceleration_unit_v<inch_t>);
  CHECK((traits::is_acceleration_unit_v<standard_gravity_t,
                                    meters_per_second_squared_t>));
  CHECK_FALSE((traits::is_acceleration_unit_v<inch_t,
                                    meters_per_second_squared_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is force unit", "[wpimath]") {
  CHECK(traits::is_force_unit_v<wpi::units::force::newton>);
  CHECK(traits::is_force_unit_v<wpi::units::force::dynes>);
  CHECK_FALSE(traits::is_force_unit_v<meter>);
  CHECK_FALSE(traits::is_force_unit_v<double>);

  CHECK(traits::is_force_unit_v<wpi::units::force::newton_t>);
  CHECK(traits::is_force_unit_v<const wpi::units::force::newton_t>);
  CHECK(traits::is_force_unit_v<const wpi::units::force::newton_t&>);
  CHECK(traits::is_force_unit_v<wpi::units::force::dyne_t>);
  CHECK_FALSE(traits::is_force_unit_v<watt_t>);
  CHECK((traits::is_force_unit_v<wpi::units::force::dyne_t,
                                     wpi::units::force::newton_t>));
  CHECK_FALSE((traits::is_force_unit_v<watt_t, wpi::units::force::newton_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is pressure unit", "[wpimath]") {
  CHECK(traits::is_pressure_unit_v<pressure::pascals>);
  CHECK(traits::is_pressure_unit_v<atmosphere>);
  CHECK_FALSE(traits::is_pressure_unit_v<year>);
  CHECK_FALSE(traits::is_pressure_unit_v<double>);

  CHECK(traits::is_pressure_unit_v<pascal_t>);
  CHECK(traits::is_pressure_unit_v<const pascal_t>);
  CHECK(traits::is_pressure_unit_v<const pascal_t&>);
  CHECK(traits::is_pressure_unit_v<atmosphere_t>);
  CHECK_FALSE(traits::is_pressure_unit_v<year_t>);
  CHECK((traits::is_pressure_unit_v<atmosphere_t, pressure::pascal_t>));
  CHECK_FALSE((traits::is_pressure_unit_v<year_t, pressure::pascal_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is charge unit", "[wpimath]") {
  CHECK(traits::is_charge_unit_v<coulomb>);
  CHECK_FALSE(traits::is_charge_unit_v<watt>);
  CHECK_FALSE(traits::is_charge_unit_v<double>);

  CHECK(traits::is_charge_unit_v<coulomb_t>);
  CHECK(traits::is_charge_unit_v<const coulomb_t>);
  CHECK(traits::is_charge_unit_v<const coulomb_t&>);
  CHECK_FALSE(traits::is_charge_unit_v<watt_t>);
  CHECK((traits::is_charge_unit_v<const coulomb_t&, coulomb_t>));
  CHECK_FALSE((traits::is_charge_unit_v<watt_t, coulomb_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is energy unit", "[wpimath]") {
  CHECK(traits::is_energy_unit_v<joule>);
  CHECK(traits::is_energy_unit_v<calorie>);
  CHECK_FALSE(traits::is_energy_unit_v<watt>);
  CHECK_FALSE(traits::is_energy_unit_v<double>);

  CHECK(traits::is_energy_unit_v<joule_t>);
  CHECK(traits::is_energy_unit_v<const joule_t>);
  CHECK(traits::is_energy_unit_v<const joule_t&>);
  CHECK(traits::is_energy_unit_v<calorie_t>);
  CHECK_FALSE(traits::is_energy_unit_v<watt_t>);
  CHECK((traits::is_energy_unit_v<calorie_t, joule_t>));
  CHECK_FALSE((traits::is_energy_unit_v<watt_t, joule_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is power unit", "[wpimath]") {
  CHECK(traits::is_power_unit_v<watt>);
  CHECK_FALSE(traits::is_power_unit_v<henry>);
  CHECK_FALSE(traits::is_power_unit_v<double>);

  CHECK(traits::is_power_unit_v<watt_t>);
  CHECK(traits::is_power_unit_v<const watt_t>);
  CHECK(traits::is_power_unit_v<const watt_t&>);
  CHECK_FALSE(traits::is_power_unit_v<henry_t>);
  CHECK((traits::is_power_unit_v<const watt_t&, watt_t>));
  CHECK_FALSE((traits::is_power_unit_v<henry_t, watt_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is voltage unit", "[wpimath]") {
  CHECK(traits::is_voltage_unit_v<volt>);
  CHECK_FALSE(traits::is_voltage_unit_v<henry>);
  CHECK_FALSE(traits::is_voltage_unit_v<double>);

  CHECK(traits::is_voltage_unit_v<volt_t>);
  CHECK(traits::is_voltage_unit_v<const volt_t>);
  CHECK(traits::is_voltage_unit_v<const volt_t&>);
  CHECK_FALSE(traits::is_voltage_unit_v<henry_t>);
  CHECK((traits::is_voltage_unit_v<const volt_t&, volt_t>));
  CHECK_FALSE((traits::is_voltage_unit_v<henry_t, volt_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is capacitance unit", "[wpimath]") {
  CHECK(traits::is_capacitance_unit_v<farad>);
  CHECK_FALSE(traits::is_capacitance_unit_v<ohm>);
  CHECK_FALSE(traits::is_capacitance_unit_v<double>);

  CHECK(traits::is_capacitance_unit_v<farad_t>);
  CHECK(traits::is_capacitance_unit_v<const farad_t>);
  CHECK(traits::is_capacitance_unit_v<const farad_t&>);
  CHECK_FALSE(traits::is_capacitance_unit_v<ohm_t>);
  CHECK((traits::is_capacitance_unit_v<const farad_t&, millifarad_t>));
  CHECK_FALSE((traits::is_capacitance_unit_v<ohm_t, farad_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is impedance unit", "[wpimath]") {
  CHECK(traits::is_impedance_unit_v<ohm>);
  CHECK_FALSE(traits::is_impedance_unit_v<farad>);
  CHECK_FALSE(traits::is_impedance_unit_v<double>);

  CHECK(traits::is_impedance_unit_v<ohm_t>);
  CHECK(traits::is_impedance_unit_v<const ohm_t>);
  CHECK(traits::is_impedance_unit_v<const ohm_t&>);
  CHECK_FALSE(traits::is_impedance_unit_v<farad_t>);
  CHECK((traits::is_impedance_unit_v<const ohm_t&, milliohm_t>));
  CHECK_FALSE((traits::is_impedance_unit_v<farad_t, ohm_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is conductance unit", "[wpimath]") {
  CHECK(traits::is_conductance_unit_v<siemens>);
  CHECK_FALSE(traits::is_conductance_unit_v<volt>);
  CHECK_FALSE(traits::is_conductance_unit_v<double>);

  CHECK(traits::is_conductance_unit_v<siemens_t>);
  CHECK(traits::is_conductance_unit_v<const siemens_t>);
  CHECK(traits::is_conductance_unit_v<const siemens_t&>);
  CHECK_FALSE(traits::is_conductance_unit_v<volt_t>);
  CHECK((traits::is_conductance_unit_v<const siemens_t&, millisiemens_t>));
  CHECK_FALSE((traits::is_conductance_unit_v<volt_t, siemens_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is magnetic flux unit", "[wpimath]") {
  CHECK(traits::is_magnetic_flux_unit_v<weber>);
  CHECK(traits::is_magnetic_flux_unit_v<maxwell>);
  CHECK_FALSE(traits::is_magnetic_flux_unit_v<inch>);
  CHECK_FALSE(traits::is_magnetic_flux_unit_v<double>);

  CHECK(traits::is_magnetic_flux_unit_v<weber_t>);
  CHECK(traits::is_magnetic_flux_unit_v<const weber_t>);
  CHECK(traits::is_magnetic_flux_unit_v<const weber_t&>);
  CHECK(traits::is_magnetic_flux_unit_v<maxwell_t>);
  CHECK_FALSE(traits::is_magnetic_flux_unit_v<inch_t>);
  CHECK((traits::is_magnetic_flux_unit_v<maxwell_t, weber_t>));
  CHECK_FALSE((traits::is_magnetic_flux_unit_v<inch_t, weber_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is magnetic field strength unit", "[wpimath]") {
  CHECK(traits::is_magnetic_field_strength_unit_v<
        wpi::units::magnetic_field_strength::tesla>);
  CHECK(traits::is_magnetic_field_strength_unit_v<gauss>);
  CHECK_FALSE(traits::is_magnetic_field_strength_unit_v<volt>);
  CHECK_FALSE(traits::is_magnetic_field_strength_unit_v<double>);

  CHECK(traits::is_magnetic_field_strength_unit_v<tesla_t>);
  CHECK(traits::is_magnetic_field_strength_unit_v<const tesla_t>);
  CHECK(traits::is_magnetic_field_strength_unit_v<const tesla_t&>);
  CHECK(traits::is_magnetic_field_strength_unit_v<gauss_t>);
  CHECK_FALSE(traits::is_magnetic_field_strength_unit_v<volt_t>);
  CHECK((traits::is_magnetic_field_strength_unit_v<gauss_t, tesla_t>));
  CHECK_FALSE((traits::is_magnetic_field_strength_unit_v<volt_t, tesla_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is inductance unit", "[wpimath]") {
  CHECK(traits::is_inductance_unit_v<henry>);
  CHECK_FALSE(traits::is_inductance_unit_v<farad>);
  CHECK_FALSE(traits::is_inductance_unit_v<double>);

  CHECK(traits::is_inductance_unit_v<henry_t>);
  CHECK(traits::is_inductance_unit_v<const henry_t>);
  CHECK(traits::is_inductance_unit_v<const henry_t&>);
  CHECK_FALSE(traits::is_inductance_unit_v<farad_t>);
  CHECK((traits::is_inductance_unit_v<const henry_t&, millihenry_t>));
  CHECK_FALSE((traits::is_inductance_unit_v<farad_t, henry_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is luminous flux unit", "[wpimath]") {
  CHECK(traits::is_luminous_flux_unit_v<lumen>);
  CHECK_FALSE(traits::is_luminous_flux_unit_v<pound>);
  CHECK_FALSE(traits::is_luminous_flux_unit_v<double>);

  CHECK(traits::is_luminous_flux_unit_v<lumen_t>);
  CHECK(traits::is_luminous_flux_unit_v<const lumen_t>);
  CHECK(traits::is_luminous_flux_unit_v<const lumen_t&>);
  CHECK_FALSE(traits::is_luminous_flux_unit_v<pound_t>);
  CHECK((traits::is_luminous_flux_unit_v<const lumen_t&, millilumen_t>));
  CHECK_FALSE((traits::is_luminous_flux_unit_v<pound_t, lumen_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is illuminance unit", "[wpimath]") {
  CHECK(traits::is_illuminance_unit_v<illuminance::footcandle>);
  CHECK(traits::is_illuminance_unit_v<illuminance::lux>);
  CHECK_FALSE(traits::is_illuminance_unit_v<meter>);
  CHECK_FALSE(traits::is_illuminance_unit_v<double>);

  CHECK(traits::is_illuminance_unit_v<footcandle_t>);
  CHECK(traits::is_illuminance_unit_v<const footcandle_t>);
  CHECK(traits::is_illuminance_unit_v<const footcandle_t&>);
  CHECK(traits::is_illuminance_unit_v<lux_t>);
  CHECK_FALSE(traits::is_illuminance_unit_v<meter_t>);
  CHECK((traits::is_illuminance_unit_v<lux_t, footcandle_t>));
  CHECK_FALSE((traits::is_illuminance_unit_v<meter_t, footcandle_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is radioactivity unit", "[wpimath]") {
  CHECK(traits::is_radioactivity_unit_v<becquerel>);
  CHECK_FALSE(traits::is_radioactivity_unit_v<year>);
  CHECK_FALSE(traits::is_radioactivity_unit_v<double>);

  CHECK(traits::is_radioactivity_unit_v<becquerel_t>);
  CHECK(traits::is_radioactivity_unit_v<const becquerel_t>);
  CHECK(traits::is_radioactivity_unit_v<const becquerel_t&>);
  CHECK_FALSE(traits::is_radioactivity_unit_v<year_t>);
  CHECK((traits::is_radioactivity_unit_v<const becquerel_t&,
                                             millibecquerel_t>));
  CHECK_FALSE((traits::is_radioactivity_unit_v<year_t, becquerel_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is torque unit", "[wpimath]") {
  CHECK(traits::is_torque_unit_v<torque::newton_meter>);
  CHECK(traits::is_torque_unit_v<torque::foot_pound>);
  CHECK_FALSE(traits::is_torque_unit_v<volume::cubic_meter>);
  CHECK_FALSE(traits::is_torque_unit_v<double>);

  CHECK(traits::is_torque_unit_v<torque::newton_meter_t>);
  CHECK(traits::is_torque_unit_v<const torque::newton_meter_t>);
  CHECK(traits::is_torque_unit_v<const torque::newton_meter_t&>);
  CHECK(traits::is_torque_unit_v<torque::foot_pound_t>);
  CHECK_FALSE(traits::is_torque_unit_v<volume::cubic_meter_t>);
  CHECK((traits::is_torque_unit_v<torque::foot_pound_t,
                                      torque::newton_meter_t>));
  CHECK_FALSE((traits::is_torque_unit_v<volume::cubic_meter_t,
                                       torque::newton_meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is area unit", "[wpimath]") {
  CHECK(traits::is_area_unit_v<square_meter>);
  CHECK(traits::is_area_unit_v<hectare>);
  CHECK_FALSE(traits::is_area_unit_v<astronicalUnit>);
  CHECK_FALSE(traits::is_area_unit_v<double>);

  CHECK(traits::is_area_unit_v<square_meter_t>);
  CHECK(traits::is_area_unit_v<const square_meter_t>);
  CHECK(traits::is_area_unit_v<const square_meter_t&>);
  CHECK(traits::is_area_unit_v<hectare_t>);
  CHECK_FALSE(traits::is_area_unit_v<astronicalUnit_t>);
  CHECK((traits::is_area_unit_v<hectare_t, square_meter_t>));
  CHECK_FALSE((traits::is_area_unit_v<astronicalUnit_t, square_meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is volume unit", "[wpimath]") {
  CHECK(traits::is_volume_unit_v<cubic_meter>);
  CHECK(traits::is_volume_unit_v<cubic_foot>);
  CHECK_FALSE(traits::is_volume_unit_v<square_feet>);
  CHECK_FALSE(traits::is_volume_unit_v<double>);

  CHECK(traits::is_volume_unit_v<cubic_meter_t>);
  CHECK(traits::is_volume_unit_v<const cubic_meter_t>);
  CHECK(traits::is_volume_unit_v<const cubic_meter_t&>);
  CHECK(traits::is_volume_unit_v<cubic_inch_t>);
  CHECK_FALSE(traits::is_volume_unit_v<foot_t>);
  CHECK((traits::is_volume_unit_v<cubic_inch_t, cubic_meter_t>));
  CHECK_FALSE((traits::is_volume_unit_v<foot_t, cubic_meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is density unit", "[wpimath]") {
  CHECK(traits::is_density_unit_v<kilograms_per_cubic_meter>);
  CHECK(traits::is_density_unit_v<ounces_per_cubic_foot>);
  CHECK_FALSE(traits::is_density_unit_v<year>);
  CHECK_FALSE(traits::is_density_unit_v<double>);

  CHECK(traits::is_density_unit_v<kilograms_per_cubic_meter_t>);
  CHECK(traits::is_density_unit_v<const kilograms_per_cubic_meter_t>);
  CHECK(traits::is_density_unit_v<const kilograms_per_cubic_meter_t&>);
  CHECK(traits::is_density_unit_v<ounces_per_cubic_foot_t>);
  CHECK_FALSE(traits::is_density_unit_v<year_t>);
  CHECK((traits::is_density_unit_v<ounces_per_cubic_foot_t,
                                       kilograms_per_cubic_meter_t>));
  CHECK_FALSE((traits::is_density_unit_v<year_t, kilograms_per_cubic_meter_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is data unit", "[wpimath]") {
  CHECK(traits::is_data_unit_v<bit>);
  CHECK(traits::is_data_unit_v<byte>);
  CHECK(traits::is_data_unit_v<exabit>);
  CHECK(traits::is_data_unit_v<exabyte>);
  CHECK_FALSE(traits::is_data_unit_v<year>);
  CHECK_FALSE(traits::is_data_unit_v<double>);

  CHECK(traits::is_data_unit_v<bit_t>);
  CHECK(traits::is_data_unit_v<const bit_t>);
  CHECK(traits::is_data_unit_v<const bit_t&>);
  CHECK(traits::is_data_unit_v<byte_t>);
  CHECK_FALSE(traits::is_data_unit_v<year_t>);
  CHECK((traits::is_data_unit_v<bit_t, byte_t>));
  CHECK_FALSE((traits::is_data_unit_v<year_t, byte_t>));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is data transfer rate unit", "[wpimath]") {
  CHECK(traits::is_data_transfer_rate_unit_v<Gbps>);
  CHECK(traits::is_data_transfer_rate_unit_v<GBps>);
  CHECK_FALSE(traits::is_data_transfer_rate_unit_v<year>);
  CHECK_FALSE(traits::is_data_transfer_rate_unit_v<double>);

  CHECK(traits::is_data_transfer_rate_unit_v<gigabits_per_second_t>);
  CHECK(traits::is_data_transfer_rate_unit_v<const gigabytes_per_second_t>);
  CHECK(
      traits::is_data_transfer_rate_unit_v<const gigabytes_per_second_t&>);
  CHECK(traits::is_data_transfer_rate_unit_v<gigabytes_per_second_t>);
  CHECK_FALSE(traits::is_data_transfer_rate_unit_v<year_t>);
  CHECK((traits::is_data_transfer_rate_unit_v<gigabits_per_second_t,
                                          gigabytes_per_second_t>));
  CHECK_FALSE((traits::is_data_transfer_rate_unit_v<year_t,
                                          gigabytes_per_second_t>));
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators squared", "[wpimath]") {
  double test;

  test = convert<squared<meters>, square_feet>(0.092903);
  CHECK_NEAR(0.99999956944, test, 5.0e-12);

  using scalar_2 = squared<scalar>;  // this is actually nonsensical, and should
                                     // also result in a scalar.
  bool isSame =
      std::is_same_v<typename std::decay_t<scalar_t>,
                   typename std::decay_t<unit_t<scalar_2>>>;
  CHECK(isSame);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators cubed", "[wpimath]") {
  double test;

  test = convert<cubed<meters>, cubic_feet>(0.0283168);
  CHECK_NEAR(0.999998354619, test, 5.0e-13);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators square root", "[wpimath]") {
  double test;

  test = convert<square_root<square_kilometer>, meter>(1.0);
  CHECK((traits::is_convertible_unit_v<
               typename std::decay_t<square_root<square_kilometer>>,
               kilometer>));
  CHECK_NEAR(1000.0, test, 5.0e-13);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators compound unit", "[wpimath]") {
  using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
  using acceleration2 =
      compound_unit<meters, inverse<seconds>, inverse<seconds>>;
  using acceleration3 =
      unit<std::ratio<1>,
           base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-2>>>;
  using acceleration4 = compound_unit<meters, inverse<squared<seconds>>>;
  using acceleration5 = compound_unit<meters, squared<inverse<seconds>>>;

  bool areSame12 = std::is_same_v<acceleration1, acceleration2>;
  bool areSame23 = std::is_same_v<acceleration2, acceleration3>;
  bool areSame34 = std::is_same_v<acceleration3, acceleration4>;
  bool areSame45 = std::is_same_v<acceleration4, acceleration5>;

  CHECK(areSame12);
  CHECK(areSame23);
  CHECK(areSame34);
  CHECK(areSame45);

  // test that thing with translations still compile
  using arbitrary1 = compound_unit<meters, inverse<celsius>>;
  using arbitrary2 = compound_unit<meters, celsius>;
  using arbitrary3 = compound_unit<arbitrary1, arbitrary2>;
  CHECK((std::is_same_v<square_meters, arbitrary3>));
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators dimensionalAnalysis", "[wpimath]") {
  // these look like 'compound units', but the dimensional analysis can be
  // REALLY handy if the unit types aren't know (i.e. they themselves are
  // template parameters), as you can get the resulting unit of the operation.

  using velocity = wpi::units::detail::unit_divide<meters, second>;
  bool shouldBeTrue = std::is_same_v<meters_per_second, velocity>;
  CHECK(shouldBeTrue);

  using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
  using acceleration2 = wpi::units::detail::unit_divide<
      meters, wpi::units::detail::unit_multiply<seconds, seconds>>;
  shouldBeTrue = std::is_same_v<acceleration1, acceleration2>;
  CHECK(shouldBeTrue);
}

#ifdef _MSC_VER
#if (_MSC_VER >= 1900)
TEST_CASE_METHOD(UnitContainer, "UnitContainer trivial", "[wpimath]") {
  CHECK(std::is_trivial_v<meter_t>);
  CHECK((std::is_trivially_assignable_v<meter_t, meter_t>));
  CHECK(std::is_trivially_constructible_v<meter_t>);
  CHECK(std::is_trivially_copy_assignable_v<meter_t>);
  CHECK(std::is_trivially_copy_constructible_v<meter_t>);
  CHECK(std::is_trivially_copyable_v<meter_t>);
  CHECK(std::is_trivially_default_constructible_v<meter_t>);
  CHECK(std::is_trivially_destructible_v<meter_t>);
  CHECK(std::is_trivially_move_assignable_v<meter_t>);
  CHECK(std::is_trivially_move_constructible_v<meter_t>);

  CHECK(std::is_trivial_v<dB_t>);
  CHECK((std::is_trivially_assignable_v<dB_t, dB_t>));
  CHECK(std::is_trivially_constructible_v<dB_t>);
  CHECK(std::is_trivially_copy_assignable_v<dB_t>);
  CHECK(std::is_trivially_copy_constructible_v<dB_t>);
  CHECK(std::is_trivially_copyable_v<dB_t>);
  CHECK(std::is_trivially_default_constructible_v<dB_t>);
  CHECK(std::is_trivially_destructible_v<dB_t>);
  CHECK(std::is_trivially_move_assignable_v<dB_t>);
  CHECK(std::is_trivially_move_constructible_v<dB_t>);
}
#endif
#endif

TEST_CASE_METHOD(UnitContainer, "UnitContainer has value member", "[wpimath]") {
  CHECK((traits::has_value_member_v<linear_scale<double>, double>));
  CHECK_FALSE((traits::has_value_member_v<meter, double>));
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer make unit", "[wpimath]") {
  auto dist = wpi::units::make_unit<meter_t>(5);
  CHECK(meter_t(5) == dist);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeAddition", "[wpimath]") {
  // units
  meter_t a_m(1.0), c_m;
  foot_t b_ft(3.28084);

  double d = convert<feet, meters>(b_ft());
  CHECK_NEAR(1.0, d, 5.0e-5);

  c_m = a_m + b_ft;
  CHECK_NEAR(2.0, c_m(), 5.0e-5);

  c_m = b_ft + meter_t(3);
  CHECK_NEAR(4.0, c_m(), 5.0e-5);

  auto e_ft = b_ft + meter_t(3);
  CHECK_NEAR(13.12336, e_ft(), 5.0e-6);

  // scalar
  scalar_t sresult = scalar_t(1.0) + scalar_t(1.0);
  CHECK_NEAR(2.0, sresult, 5.0e-6);

  sresult = scalar_t(1.0) + 1.0;
  CHECK_NEAR(2.0, sresult, 5.0e-6);

  sresult = 1.0 + scalar_t(1.0);
  CHECK_NEAR(2.0, sresult, 5.0e-6);

  d = scalar_t(1.0) + scalar_t(1.0);
  CHECK_NEAR(2.0, d, 5.0e-6);

  d = scalar_t(1.0) + 1.0;
  CHECK_NEAR(2.0, d, 5.0e-6);

  d = 1.0 + scalar_t(1.0);
  CHECK_NEAR(2.0, d, 5.0e-6);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeUnaryAddition", "[wpimath]") {
  meter_t a_m(1.0);

  CHECK(++a_m == meter_t(2));
  CHECK(a_m++ == meter_t(2));
  CHECK(a_m == meter_t(3));
  CHECK(+a_m == meter_t(3));
  CHECK(a_m == meter_t(3));

  dBW_t b_dBW(1.0);

  CHECK(++b_dBW == dBW_t(2));
  CHECK(b_dBW++ == dBW_t(2));
  CHECK(b_dBW == dBW_t(3));
  CHECK(+b_dBW == dBW_t(3));
  CHECK(b_dBW == dBW_t(3));
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeSubtraction", "[wpimath]") {
  meter_t a_m(1.0), c_m;
  foot_t b_ft(3.28084);

  c_m = a_m - b_ft;
  CHECK_NEAR(0.0, c_m(), 5.0e-5);

  c_m = b_ft - meter_t(1);
  CHECK_NEAR(0.0, c_m(), 5.0e-5);

  auto e_ft = b_ft - meter_t(1);
  CHECK_NEAR(0.0, e_ft(), 5.0e-6);

  scalar_t sresult = scalar_t(1.0) - scalar_t(1.0);
  CHECK_NEAR(0.0, sresult, 5.0e-6);

  sresult = scalar_t(1.0) - 1.0;
  CHECK_NEAR(0.0, sresult, 5.0e-6);

  sresult = 1.0 - scalar_t(1.0);
  CHECK_NEAR(0.0, sresult, 5.0e-6);

  double d = scalar_t(1.0) - scalar_t(1.0);
  CHECK_NEAR(0.0, d, 5.0e-6);

  d = scalar_t(1.0) - 1.0;
  CHECK_NEAR(0.0, d, 5.0e-6);

  d = 1.0 - scalar_t(1.0);
  CHECK_NEAR(0.0, d, 5.0e-6);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeUnarySubtraction", "[wpimath]") {
  meter_t a_m(4.0);

  CHECK(--a_m == meter_t(3));
  CHECK(a_m-- == meter_t(3));
  CHECK(a_m == meter_t(2));
  CHECK(-a_m == meter_t(-2));
  CHECK(a_m == meter_t(2));

  dBW_t b_dBW(4.0);

  CHECK(--b_dBW == dBW_t(3));
  CHECK(b_dBW-- == dBW_t(3));
  CHECK(b_dBW == dBW_t(2));
  CHECK(-b_dBW == dBW_t(-2));
  CHECK(b_dBW == dBW_t(2));
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeMultiplication", "[wpimath]") {
  meter_t a_m(1.0), b_m(2.0);
  foot_t a_ft(3.28084);

  auto c_m2 = a_m * b_m;
  CHECK_NEAR(2.0, c_m2(), 5.0e-5);

  c_m2 = b_m * meter_t(2);
  CHECK_NEAR(4.0, c_m2(), 5.0e-5);

  c_m2 = b_m * a_ft;
  CHECK_NEAR(2.0, c_m2(), 5.0e-5);

  auto c_m = b_m * 2.0;
  CHECK_NEAR(4.0, c_m(), 5.0e-5);

  c_m = 2.0 * b_m;
  CHECK_NEAR(4.0, c_m(), 5.0e-5);

  double convert = scalar_t(3.14);
  CHECK_NEAR(3.14, convert, 5.0e-5);

  scalar_t sresult = scalar_t(5.0) * scalar_t(4.0);
  CHECK_NEAR(20.0, sresult(), 5.0e-5);

  sresult = scalar_t(5.0) * 4.0;
  CHECK_NEAR(20.0, sresult(), 5.0e-5);

  sresult = 4.0 * scalar_t(5.0);
  CHECK_NEAR(20.0, sresult(), 5.0e-5);

  double result = scalar_t(5.0) * scalar_t(4.0);
  CHECK_NEAR(20.0, result, 5.0e-5);

  result = scalar_t(5.0) * 4.0;
  CHECK_NEAR(20.0, result, 5.0e-5);

  result = 4.0 * scalar_t(5.0);
  CHECK_NEAR(20.0, result, 5.0e-5);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeMixedUnitMultiplication", "[wpimath]") {
  meter_t a_m(1.0);
  foot_t b_ft(3.28084);
  unit_t<inverse<meter>> i_m(2.0);

  // resultant unit is square of leftmost unit
  auto c_m2 = a_m * b_ft;
  CHECK_NEAR(1.0, c_m2(), 5.0e-5);

  auto c_ft2 = b_ft * a_m;
  CHECK_NEAR(10.7639111056, c_ft2(), 5.0e-7);

  // you can get whatever (compatible) type you want if you ask explicitly
  square_meter_t d_m2 = b_ft * a_m;
  CHECK_NEAR(1.0, d_m2(), 5.0e-5);

  // a unit times a sclar ends up with the same units.
  meter_t e_m = a_m * scalar_t(3.0);
  CHECK_NEAR(3.0, e_m(), 5.0e-5);

  e_m = scalar_t(4.0) * a_m;
  CHECK_NEAR(4.0, e_m(), 5.0e-5);

  // unit times its inverse results in a scalar
  scalar_t s = a_m * i_m;
  CHECK_NEAR(2.0, s, 5.0e-5);

  c_m2 = b_ft * meter_t(2);
  CHECK_NEAR(2.0, c_m2(), 5.0e-5);

  auto e_ft2 = b_ft * meter_t(3);
  CHECK_NEAR(32.2917333168, e_ft2(), 5.0e-6);

  auto mps = meter_t(10.0) * unit_t<inverse<seconds>>(1.0);
  CHECK(mps == meters_per_second_t(10));
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeScalarMultiplication", "[wpimath]") {
  meter_t a_m(1.0);

  auto result_m = scalar_t(3.0) * a_m;
  CHECK_NEAR(3.0, result_m(), 5.0e-5);

  result_m = a_m * scalar_t(4.0);
  CHECK_NEAR(4.0, result_m(), 5.0e-5);

  result_m = 3.0 * a_m;
  CHECK_NEAR(3.0, result_m(), 5.0e-5);

  result_m = a_m * 4.0;
  CHECK_NEAR(4.0, result_m(), 5.0e-5);

  bool isSame = std::is_same_v<decltype(result_m), meter_t>;
  CHECK(isSame);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unitTypeDivision", "[wpimath]") {
  meter_t a_m(1.0), b_m(2.0);
  foot_t a_ft(3.28084);
  second_t a_sec(10.0);
  bool isSame;

  auto c = a_m / a_ft;
  CHECK_NEAR(1.0, c, 5.0e-5);
  isSame = std::is_same_v<decltype(c), scalar_t>;
  CHECK(isSame);

  c = a_m / b_m;
  CHECK_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same_v<decltype(c), scalar_t>;
  CHECK(isSame);

  c = a_ft / a_m;
  CHECK_NEAR(1.0, c, 5.0e-5);
  isSame = std::is_same_v<decltype(c), scalar_t>;
  CHECK(isSame);

  c = scalar_t(1.0) / 2.0;
  CHECK_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same_v<decltype(c), scalar_t>;
  CHECK(isSame);

  c = 1.0 / scalar_t(2.0);
  CHECK_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same_v<decltype(c), scalar_t>;
  CHECK(isSame);

  double d = scalar_t(1.0) / 2.0;
  CHECK_NEAR(0.5, d, 5.0e-5);

  auto e = a_m / a_sec;
  CHECK_NEAR(0.1, e(), 5.0e-5);
  isSame = std::is_same_v<decltype(e), meters_per_second_t>;
  CHECK(isSame);

  auto f = a_m / 8.0;
  CHECK_NEAR(0.125, f(), 5.0e-5);
  isSame = std::is_same_v<decltype(f), meter_t>;
  CHECK(isSame);

  auto g = 4.0 / b_m;
  CHECK_NEAR(2.0, g(), 5.0e-5);
  isSame = std::is_same_v<decltype(g), unit_t<inverse<meters>>>;
  CHECK(isSame);

  auto mph = mile_t(60.0) / hour_t(1.0);
  meters_per_second_t mps = mph;
  CHECK_NEAR(26.8224, mps(), 5.0e-5);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer compoundAssignmentAddition", "[wpimath]") {
  // units
  meter_t a(0.0);
  a += meter_t(1.0);

  CHECK(meter_t(1.0) == a);

  a += foot_t(meter_t(1));

  CHECK(meter_t(2.0) == a);

  // scalars
  scalar_t b(0);
  b += scalar_t(1.0);

  CHECK(scalar_t(1.0) == b);

  b += 1;

  CHECK(scalar_t(2.0) == b);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer compoundAssignmentSubtraction", "[wpimath]") {
  // units
  meter_t a(2.0);
  a -= meter_t(1.0);

  CHECK(meter_t(1.0) == a);

  a -= foot_t(meter_t(1));

  CHECK(meter_t(0.0) == a);

  // scalars
  scalar_t b(2);
  b -= scalar_t(1.0);

  CHECK(scalar_t(1.0) == b);

  b -= 1;

  CHECK(scalar_t(0) == b);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer compoundAssignmentMultiplication", "[wpimath]") {
  // units
  meter_t a(2.0);
  a *= scalar_t(2.0);

  CHECK(meter_t(4.0) == a);

  a *= 2.0;

  CHECK(meter_t(8.0) == a);

  // scalars
  scalar_t b(2);
  b *= scalar_t(2.0);

  CHECK(scalar_t(4.0) == b);

  b *= 2;

  CHECK(scalar_t(8.0) == b);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer compoundAssignmentDivision", "[wpimath]") {
  // units
  meter_t a(8.0);
  a /= scalar_t(2.0);

  CHECK(meter_t(4.0) == a);

  a /= 2.0;

  CHECK(meter_t(2.0) == a);

  // scalars
  scalar_t b(8);
  b /= scalar_t(2.0);

  CHECK(scalar_t(4.0) == b);

  b /= 2;

  CHECK(scalar_t(2.0) == b);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer scalarTypeImplicitConversion", "[wpimath]") {
  double test = scalar_t(3.0);
  CHECK_DOUBLE_EQ(3.0, test);

  scalar_t testS = 3.0;
  CHECK_DOUBLE_EQ(3.0, testS);

  scalar_t test3(ppm_t(10));
  CHECK_DOUBLE_EQ(0.00001, test3);

  scalar_t test4;
  test4 = ppm_t(1);
  CHECK_DOUBLE_EQ(0.000001, test4);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer valueMethod", "[wpimath]") {
  double test = meter_t(3.0).value();
  CHECK_DOUBLE_EQ(3.0, test);

  auto test2 = meter_t(4.0).value();
  CHECK_DOUBLE_EQ(4.0, test2);
  CHECK((std::is_same_v<decltype(test2), double>));
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer convertMethod", "[wpimath]") {
  double test = meter_t(3.0).convert<feet>().value();
  CHECK_NEAR(9.84252, test, 5.0e-6);
}

#ifdef UNIT_LIB_ENABLE_IOSTREAM
TEST_CASE_METHOD(UnitContainer, "UnitContainer cout", "[wpimath]") {
  auto toString = [](const auto& value) {
    std::ostringstream output;
    output << value;
    return output.str();
  };

  CHECK(std::string_view{"349.87 deg"} ==
        std::string_view{toString(degree_t(349.87))});

  CHECK(std::string_view{"1 m"} == std::string_view{toString(meter_t(1.0))});

  CHECK(std::string_view{"31 dB"} == std::string_view{toString(dB_t(31.0))});

  CHECK(std::string_view{"21.79 V"} ==
        std::string_view{toString(volt_t(21.79))});

  CHECK(std::string_view{"12 dBW"} ==
        std::string_view{toString(dBW_t(12.0))});

  CHECK(std::string_view{"120 dBm"} ==
        std::string_view{toString(dBm_t(120.0))});

  CHECK(std::string_view{"72.1 mph"} ==
        std::string_view{toString(miles_per_hour_t(72.1))});

  // undefined unit
  CHECK(std::string_view{"16 m^4"} ==
        std::string_view{toString(wpi::units::math::cpow<4>(meter_t(2)))});

  CHECK(std::string_view{"8 cu_ft"} ==
        std::string_view{toString(wpi::units::math::cpow<3>(foot_t(2)))});

  std::ostringstream output;
  output << std::setprecision(9) << wpi::units::math::cpow<4>(foot_t(2));
  CHECK(std::string_view{"0.138095597 m^4"} ==
        std::string_view{output.str()});

  // constants
  output.str("");
  output << std::setprecision(8) << constants::k_B;
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"1.3806485e-023 m^2 kg s^-2 K^-1"} ==
        std::string_view{output.str()});
#else
  CHECK(std::string_view{"1.3806485e-23 m^2 kg s^-2 K^-1"} ==
        std::string_view{output.str()});
#endif

  output.str("");
  output << std::setprecision(9) << constants::mu_B;
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"9.27400999e-024 m^2 A"} ==
        std::string_view{output.str()});
#else
  CHECK(std::string_view{"9.27400999e-24 m^2 A"} ==
        std::string_view{output.str()});
#endif

  output.str("");
  output << std::setprecision(7) << constants::sigma;
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"5.670367e-008 kg s^-3 K^-4"} ==
        std::string_view{output.str()});
#else
  CHECK(std::string_view{"5.670367e-08 kg s^-3 K^-4"} ==
        std::string_view{output.str()});
#endif
}
#endif

#if !defined(UNIT_LIB_DISABLE_FMT)
TEST_CASE_METHOD(UnitContainer, "UnitContainer fmtlib", "[wpimath]") {
  CHECK(std::string_view{"349.87 deg"} ==
        std::string_view{std::format("{}", degree_t(349.87))});

  CHECK(std::string_view{"1 m"} ==
        std::string_view{std::format("{}", meter_t(1.0))});

  CHECK(std::string_view{"31 dB"} ==
        std::string_view{std::format("{}", dB_t(31.0))});

  CHECK(std::string_view{"21.79 V"} ==
        std::string_view{std::format("{}", volt_t(21.79))});

  CHECK(std::string_view{"12 dBW"} ==
        std::string_view{std::format("{}", dBW_t(12.0))});

  CHECK(std::string_view{"120 dBm"} ==
        std::string_view{std::format("{}", dBm_t(120.0))});

  CHECK(std::string_view{"72.1 mph"} ==
        std::string_view{std::format("{}", miles_per_hour_t(72.1))});

  // undefined unit
  CHECK(std::string_view{"16 m^4"} == std::string_view{
                                            std::format(
                                                "{}",
                                                wpi::units::math::cpow<4>(
                                                    meter_t(2)))});

  CHECK(std::string_view{"8 cu_ft"} == std::string_view{
                                             std::format(
                                                 "{}",
                                                 wpi::units::math::cpow<3>(
                                                     foot_t(2)))});

  CHECK(std::string_view{"0.138095597 m^4"} == std::string_view{
                                                     std::format(
                                                         "{:.9}",
                                                         wpi::units::math::cpow<
                                                             4>(foot_t(2)))});

  // constants
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"1.3806485e-023 m^2 kg s^-2 K^-1"} ==
        std::string_view{std::format("{:.8}", constants::k_B)});
#else
  CHECK(std::string_view{"1.3806485e-23 m^2 kg s^-2 K^-1"} ==
        std::string_view{std::format("{:.8}", constants::k_B)});
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"9.27400999e-024 m^2 A"} ==
        std::string_view{std::format("{:.9}", constants::mu_B)});
#else
  CHECK(std::string_view{"9.27400999e-24 m^2 A"} ==
        std::string_view{std::format("{:.9}", constants::mu_B)});
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  CHECK(std::string_view{"5.670367e-008 kg s^-3 K^-4"} ==
        std::string_view{std::format("{:.7}", constants::sigma)});
#else
  CHECK(std::string_view{"5.670367e-08 kg s^-3 K^-4"} ==
        std::string_view{std::format("{:.7}", constants::sigma)});
#endif
}
#endif

TEST_CASE_METHOD(UnitContainer, "UnitContainer to string", "[wpimath]") {
  foot_t a(3.5);
  CHECK(std::string_view{"3.5 ft"} == std::string_view{wpi::units::length::to_string(a).c_str()});

  meter_t b(8);
  CHECK(std::string_view{"8 m"} == std::string_view{wpi::units::length::to_string(b).c_str()});
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer DISABLED to string locale",
                 "[wpimath][.]") {
  struct lconv* lc;

  // German locale
#if defined(_MSC_VER)
  setlocale(LC_ALL, "de-DE");
#else
  CHECK(std::string_view{"de_DE.utf8"} == std::string_view{setlocale(LC_ALL, "de_DE.utf8")});
#endif

  lc = localeconv();
  char point_de = *lc->decimal_point;
  CHECK(point_de == ',');

  kilometer_t de = 2_km;
  CHECK(std::string_view{"2 km"} == std::string_view{wpi::units::length::to_string(de).c_str()});

  de = 2.5_km;
  CHECK(std::string_view{"2,5 km"} == std::string_view{wpi::units::length::to_string(de).c_str()});

  // US locale
#if defined(_MSC_VER)
  setlocale(LC_ALL, "en-US");
#else
  CHECK(std::string_view{"en_US.utf8"} == std::string_view{setlocale(LC_ALL, "en_US.utf8")});
#endif

  lc = localeconv();
  char point_us = *lc->decimal_point;
  CHECK(point_us == '.');

  mile_t us = 2_mi;
  CHECK(std::string_view{"2 mi"} == std::string_view{wpi::units::length::to_string(us).c_str()});

  us = 2.5_mi;
  CHECK(std::string_view{"2.5 mi"} == std::string_view{wpi::units::length::to_string(us).c_str()});
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer nameAndAbbreviation", "[wpimath]") {
  foot_t a(3.5);
  CHECK(std::string_view{"ft"} == std::string_view{wpi::units::abbreviation(a)});
  CHECK(std::string_view{"ft"} == std::string_view{a.abbreviation()});
  CHECK(std::string_view{"foot"} == std::string_view{a.name()});

  meter_t b(8);
  CHECK(std::string_view{"m"} == std::string_view{wpi::units::abbreviation(b)});
  CHECK(std::string_view{"m"} == std::string_view{b.abbreviation()});
  CHECK(std::string_view{"meter"} == std::string_view{b.name()});
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer negative", "[wpimath]") {
  meter_t a(5.3);
  meter_t b(-5.3);
  CHECK_NEAR(a.value(), -b.value(), 5.0e-320);
  CHECK_NEAR(b.value(), -a.value(), 5.0e-320);

  dB_t c(2.87);
  dB_t d(-2.87);
  CHECK_NEAR(c.value(), -d.value(), 5.0e-320);
  CHECK_NEAR(d.value(), -c.value(), 5.0e-320);

  ppm_t e = -1 * ppm_t(10);
  CHECK(e == -ppm_t(10));
  CHECK_NEAR(-0.00001, e, 5.0e-10);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer concentration", "[wpimath]") {
  ppb_t a(ppm_t(1));
  CHECK(ppb_t(1000) == a);
  CHECK(0.000001 == a);
  CHECK(0.000001 == a.value());

  scalar_t b(ppm_t(1));
  CHECK(0.000001 == b);

  scalar_t c = ppb_t(1);
  CHECK(0.000000001 == c);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer dBConversion", "[wpimath]") {
  dBW_t a_dbw(23.1);
  watt_t a_w = a_dbw;
  dBm_t a_dbm = a_dbw;

  CHECK_NEAR(204.173794, a_w(), 5.0e-7);
  CHECK_NEAR(53.1, a_dbm(), 5.0e-7);

  milliwatt_t b_mw(100000.0);
  watt_t b_w = b_mw;
  dBm_t b_dbm = b_mw;
  dBW_t b_dbw = b_mw;

  CHECK_NEAR(100.0, b_w(), 5.0e-7);
  CHECK_NEAR(50.0, b_dbm(), 5.0e-7);
  CHECK_NEAR(20.0, b_dbw(), 5.0e-7);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer dBAddition", "[wpimath]") {
  bool isSame;

  auto result_dbw = dBW_t(10.0) + dB_t(30.0);
  CHECK_NEAR(40.0, result_dbw(), 5.0e-5);
  result_dbw = dB_t(12.0) + dBW_t(30.0);
  CHECK_NEAR(42.0, result_dbw(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_dbw), dBW_t>;
  CHECK(isSame);

  auto result_dbm = dB_t(30.0) + dBm_t(20.0);
  CHECK_NEAR(50.0, result_dbm(), 5.0e-5);

  // adding dBW to dBW is something you probably shouldn't do, but let's see if
  // it works...
  auto result_dBW2 = dBW_t(10.0) + dBm_t(40.0);
  CHECK_NEAR(20.0, result_dBW2(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_dBW2),
                        unit_t<squared<watts>, double, decibel_scale>>;
  CHECK(isSame);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer dBSubtraction", "[wpimath]") {
  bool isSame;

  auto result_dbw = dBW_t(10.0) - dB_t(30.0);
  CHECK_NEAR(-20.0, result_dbw(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_dbw), dBW_t>;
  CHECK(isSame);

  auto result_dbm = dBm_t(100.0) - dB_t(30.0);
  CHECK_NEAR(70.0, result_dbm(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_dbm), dBm_t>;
  CHECK(isSame);

  auto result_db = dBW_t(100.0) - dBW_t(80.0);
  CHECK_NEAR(20.0, result_db(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_db), dB_t>;
  CHECK(isSame);

  result_db = dB_t(100.0) - dB_t(80.0);
  CHECK_NEAR(20.0, result_db(), 5.0e-5);
  isSame = std::is_same_v<decltype(result_db), dB_t>;
  CHECK(isSame);
}

TEST_CASE_METHOD(UnitContainer, "UnitContainer unit cast", "[wpimath]") {
  meter_t test1(5.7);
  hectare_t test2(16);

  double dResult1 = 5.7;

  double dResult2 = 16;
  int iResult2 = 16;

  CHECK(dResult1 == unit_cast<double>(test1));
  CHECK(dResult2 == unit_cast<double>(test2));
  CHECK(iResult2 == unit_cast<int>(test2));

  CHECK((std::is_same_v<double, decltype(unit_cast<double>(test1))>));
  CHECK((std::is_same_v<int, decltype(unit_cast<int>(test2))>));
}

// literal syntax is only supported in GCC 4.7+ and MSVC2015+
#if !defined(_MSC_VER) || _MSC_VER > 1800
TEST_CASE_METHOD(UnitContainer, "UnitContainer literals", "[wpimath]") {
  // basic functionality testing
  CHECK((std::is_same_v<decltype(16.2_m), meter_t>));
  CHECK(meter_t(16.2) == 16.2_m);
  CHECK(meter_t(16) == 16_m);

  CHECK((std::is_same_v<decltype(11.2_ft), foot_t>));
  CHECK(foot_t(11.2) == 11.2_ft);
  CHECK(foot_t(11) == 11_ft);

  // auto using literal syntax
  auto x = 10.0_m;
  CHECK((std::is_same_v<decltype(x), meter_t>));
  CHECK(meter_t(10) == x);

  // conversion using literal syntax
  foot_t y = 0.3048_m;
  CHECK(1_ft == y);

  // Pythagorean theorem
  meter_t a = 3_m;
  meter_t b = 4_m;
  meter_t c = sqrt(pow<2>(a) + pow<2>(b));
  CHECK(c == 5_m);
}
#endif

TEST_CASE_METHOD(UnitConversion, "UnitConversion length", "[wpimath]") {
  double test;
  test = convert<meters, nanometers>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, micrometers>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, millimeters>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, centimeters>(0.01);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, kilometers>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, meters>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, feet>(0.3048);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, miles>(1609.344);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, inches>(0.0254);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, nauticalMiles>(1852.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, astronicalUnits>(149597870700.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, lightyears>(9460730472580800.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, parsec>(3.08567758e16);
  CHECK_NEAR(1.0, test, 5.0e7);

  test = convert<feet, feet>(6.3);
  CHECK_NEAR(6.3, test, 5.0e-5);
  test = convert<feet, inches>(6.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
  test = convert<inches, feet>(6.0);
  CHECK_NEAR(0.5, test, 5.0e-5);
  test = convert<meter, feet>(1.0);
  CHECK_NEAR(3.28084, test, 5.0e-5);
  test = convert<miles, nauticalMiles>(6.3);
  CHECK_NEAR(5.47455, test, 5.0e-6);
  test = convert<miles, meters>(11.0);
  CHECK_NEAR(17702.8, test, 5.0e-2);
  test = convert<meters, chains>(1.0);
  CHECK_NEAR(0.0497097, test, 5.0e-7);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion mass", "[wpimath]") {
  double test;

  test = convert<kilograms, grams>(1.0e-3);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, micrograms>(1.0e-9);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, milligrams>(1.0e-6);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, kilograms>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, metric_tons>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, pounds>(0.453592);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, long_tons>(1016.05);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, short_tons>(907.185);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, mass::ounces>(0.0283495);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, carats>(0.0002);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<slugs, kilograms>(1.0);
  CHECK_NEAR(14.593903, test, 5.0e-7);

  test = convert<pounds, carats>(6.3);
  CHECK_NEAR(14288.2, test, 5.0e-2);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion time", "[wpimath]") {
  double result = 0;
  double daysPerYear = 365;
  double hoursPerDay = 24;
  double minsPerHour = 60;
  double secsPerMin = 60;
  double daysPerWeek = 7;

  result = 2 * daysPerYear * hoursPerDay * minsPerHour * secsPerMin *
           (1 / minsPerHour) * (1 / secsPerMin) * (1 / hoursPerDay) *
           (1 / daysPerWeek);
  CHECK_NEAR(104.286, result, 5.0e-4);

  year_t twoYears(2.0);
  week_t twoYearsInWeeks = twoYears;
  CHECK_NEAR(week_t(104.286).value(), twoYearsInWeeks.value(), 5.0e-4);

  double test;

  test = convert<seconds, seconds>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, nanoseconds>(1.0e-9);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, microseconds>(1.0e-6);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, milliseconds>(1.0e-3);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, minutes>(60.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, hours>(3600.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, days>(86400.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, weeks>(604800.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, years>(3.154e7);
  CHECK_NEAR(1.0, test, 5.0e3);

  test = convert<years, weeks>(2.0);
  CHECK_NEAR(104.2857142857143, test, 5.0e-14);
  test = convert<hours, minutes>(4.0);
  CHECK_NEAR(240.0, test, 5.0e-14);
  test = convert<julian_years, days>(1.0);
  CHECK_NEAR(365.25, test, 5.0e-14);
  test = convert<gregorian_years, days>(1.0);
  CHECK_NEAR(365.2425, test, 5.0e-14);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion angle", "[wpimath]") {
  angle::degree_t quarterCircleDeg(90.0);
  angle::radian_t quarterCircleRad = quarterCircleDeg;
  CHECK_NEAR(angle::radian_t(constants::detail::PI_VAL / 2.0).value(), quarterCircleRad.value(), 5.0e-12);

  double test;

  test = convert<angle::radians, angle::radians>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-20);
  test = convert<angle::radians, angle::milliradians>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-4);
  test = convert<angle::radians, angle::degrees>(0.0174533);
  CHECK_NEAR(1.0, test, 5.0e-7);
  test = convert<angle::radians, angle::arcminutes>(0.000290888);
  CHECK_NEAR(0.99999928265913, test, 5.0e-8);
  test = convert<angle::radians, angle::arcseconds>(4.8481e-6);
  CHECK_NEAR(0.999992407, test, 5.0e-10);
  test = convert<angle::radians, angle::turns>(6.28319);
  CHECK_NEAR(1.0, test, 5.0e-6);
  test = convert<angle::radians, angle::gradians>(0.015708);
  CHECK_NEAR(1.0, test, 5.0e-6);

  test = convert<angle::radians, angle::radians>(2.1);
  CHECK_NEAR(2.1, test, 5.0e-6);
  test = convert<angle::arcseconds, angle::gradians>(2.1);
  CHECK_NEAR(0.000648148, test, 5.0e-6);
  test = convert<angle::radians, angle::degrees>(constants::detail::PI_VAL);
  CHECK_NEAR(180.0, test, 5.0e-6);
  test = convert<angle::degrees, angle::radians>(90.0);
  CHECK_NEAR(constants::detail::PI_VAL / 2, test, 5.0e-6);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion current", "[wpimath]") {
  double test;

  test = convert<current::A, current::mA>(2.1);
  CHECK_NEAR(2100.0, test, 5.0e-6);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion temperature", "[wpimath]") {
  // temp conversion are weird/hard since they involve translations AND scaling.
  double test;

  test = convert<kelvin, kelvin>(72.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
  test = convert<fahrenheit, fahrenheit>(72.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
  test = convert<kelvin, fahrenheit>(300.0);
  CHECK_NEAR(80.33, test, 5.0e-5);
  test = convert<fahrenheit, kelvin>(451.0);
  CHECK_NEAR(505.928, test, 5.0e-4);
  test = convert<kelvin, celsius>(300.0);
  CHECK_NEAR(26.85, test, 5.0e-3);
  test = convert<celsius, kelvin>(451.0);
  CHECK_NEAR(724.15, test, 5.0e-3);
  test = convert<fahrenheit, celsius>(72.0);
  CHECK_NEAR(22.2222, test, 5.0e-5);
  test = convert<celsius, fahrenheit>(100.0);
  CHECK_NEAR(212.0, test, 5.0e-5);
  test = convert<fahrenheit, celsius>(32.0);
  CHECK_NEAR(0.0, test, 5.0e-5);
  test = convert<celsius, fahrenheit>(0.0);
  CHECK_NEAR(32.0, test, 5.0e-5);
  test = convert<rankine, kelvin>(100.0);
  CHECK_NEAR(55.5556, test, 5.0e-5);
  test = convert<kelvin, rankine>(100.0);
  CHECK_NEAR(180.0, test, 5.0e-5);
  test = convert<fahrenheit, rankine>(100.0);
  CHECK_NEAR(559.67, test, 5.0e-5);
  test = convert<rankine, fahrenheit>(72.0);
  CHECK_NEAR(-387.67, test, 5.0e-5);
  test = convert<reaumur, kelvin>(100.0);
  CHECK_NEAR(398.0, test, 5.0e-1);
  test = convert<reaumur, celsius>(80.0);
  CHECK_NEAR(100.0, test, 5.0e-5);
  test = convert<celsius, reaumur>(212.0);
  CHECK_NEAR(169.6, test, 5.0e-2);
  test = convert<reaumur, fahrenheit>(80.0);
  CHECK_NEAR(212.0, test, 5.0e-5);
  test = convert<fahrenheit, reaumur>(37.0);
  CHECK_NEAR(2.222, test, 5.0e-3);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion luminous intensity", "[wpimath]") {
  double test;

  test = convert<candela, millicandela>(72.0);
  CHECK_NEAR(72000.0, test, 5.0e-5);
  test = convert<millicandela, candela>(376.0);
  CHECK_NEAR(0.376, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion solid angle", "[wpimath]") {
  double test;
  bool same;

  same = std::is_same_v<traits::base_unit_of<steradians>,
                      traits::base_unit_of<degrees_squared>>;
  CHECK(same);

  test = convert<steradians, steradians>(72.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
  test = convert<steradians, degrees_squared>(1.0);
  CHECK_NEAR(3282.8, test, 5.0e-2);
  test = convert<steradians, spats>(8.0);
  CHECK_NEAR(0.636619772367582, test, 5.0e-14);
  test = convert<degrees_squared, steradians>(3282.8);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<degrees_squared, degrees_squared>(72.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
  test = convert<degrees_squared, spats>(3282.8);
  CHECK_NEAR(1.0 / (4 * constants::detail::PI_VAL), test, 5.0e-5);
  test = convert<spats, steradians>(1.0 / (4 * constants::detail::PI_VAL));
  CHECK_NEAR(1.0, test, 5.0e-14);
  test = convert<spats, degrees_squared>(1.0 / (4 * constants::detail::PI_VAL));
  CHECK_NEAR(3282.8, test, 5.0e-2);
  test = convert<spats, spats>(72.0);
  CHECK_NEAR(72.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion frequency", "[wpimath]") {
  double test;

  test = convert<hertz, kilohertz>(63000.0);
  CHECK_NEAR(63.0, test, 5.0e-5);
  test = convert<hertz, hertz>(6.3);
  CHECK_NEAR(6.3, test, 5.0e-5);
  test = convert<kilohertz, hertz>(5.0);
  CHECK_NEAR(5000.0, test, 5.0e-5);
  test = convert<megahertz, hertz>(1.0);
  CHECK_NEAR(1.0e6, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion velocity", "[wpimath]") {
  double test;
  bool same;

  same = std::is_same_v<meters_per_second,
                      unit<std::ratio<1>, category::velocity_unit>>;
  CHECK(same);
  same = traits::is_convertible_unit_v<miles_per_hour, meters_per_second>;
  CHECK(same);

  test = convert<meters_per_second, miles_per_hour>(1250.0);
  CHECK_NEAR(2796.17, test, 5.0e-3);
  test = convert<feet_per_second, kilometers_per_hour>(2796.17);
  CHECK_NEAR(3068.181418, test, 5.0e-7);
  test = convert<knots, miles_per_hour>(600.0);
  CHECK_NEAR(690.468, test, 5.0e-4);
  test = convert<miles_per_hour, feet_per_second>(120.0);
  CHECK_NEAR(176.0, test, 5.0e-5);
  test = convert<feet_per_second, meters_per_second>(10.0);
  CHECK_NEAR(3.048, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion angular velocity", "[wpimath]") {
  double test;
  bool same;

  same =
      std::is_same_v<radians_per_second,
                   unit<std::ratio<1>, category::angular_velocity_unit>>;
  CHECK(same);
  same = traits::is_convertible_unit_v<rpm, radians_per_second>;
  CHECK(same);

  test = convert<radians_per_second, milliarcseconds_per_year>(1.0);
  CHECK_NEAR(6.504e15, test, 1.0e12);
  test = convert<degrees_per_second, radians_per_second>(1.0);
  CHECK_NEAR(0.0174533, test, 5.0e-8);
  test = convert<rpm, radians_per_second>(1.0);
  CHECK_NEAR(0.10471975512, test, 5.0e-13);
  test = convert<milliarcseconds_per_year, radians_per_second>(1.0);
  CHECK_NEAR(1.537e-16, test, 5.0e-20);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion angular jerk", "[wpimath]") {
  double test;
  bool same;

  same =
      std::is_same_v<radians_per_second_cubed,
                   unit<std::ratio<1>, category::angular_jerk_unit>>;
  CHECK(same);
  same = traits::is_convertible_unit_v<deg_per_s_cu, radians_per_second_cubed>;
  CHECK(same);

  test = convert<degrees_per_second_cubed, radians_per_second_cubed>(1.0);
  CHECK_NEAR(0.0174533, test, 5.0e-8);
  test = convert<turns_per_second_cubed, radians_per_second_cubed>(1.0);
  CHECK_NEAR(6.283185307, test, 5.0e-6);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion acceleration", "[wpimath]") {
  double test;

  test = convert<standard_gravity, meters_per_second_squared>(1.0);
  CHECK_NEAR(9.80665, test, 5.0e-10);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion force", "[wpimath]") {
  double test;

  test = convert<wpi::units::force::newton, wpi::units::force::newton>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<wpi::units::force::newton, wpi::units::force::pounds>(6.3);
  CHECK_NEAR(1.4163, test, 5.0e-5);
  test = convert<wpi::units::force::newton, wpi::units::force::dynes>(5.0);
  CHECK_NEAR(500000.0, test, 5.0e-5);
  test = convert<wpi::units::force::newtons, wpi::units::force::poundals>(2.1);
  CHECK_NEAR(15.1893, test, 5.0e-5);
  test = convert<wpi::units::force::newtons, wpi::units::force::kiloponds>(173.0);
  CHECK_NEAR(17.6411, test, 5.0e-5);
  test = convert<wpi::units::force::poundals, wpi::units::force::kiloponds>(21.879);
  CHECK_NEAR(0.308451933, test, 5.0e-10);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion area", "[wpimath]") {
  double test;

  test = convert<hectares, acres>(6.3);
  CHECK_NEAR(15.5676, test, 5.0e-5);
  test = convert<square_miles, square_kilometers>(10.0);
  CHECK_NEAR(25.8999, test, 5.0e-5);
  test = convert<square_inch, square_meter>(4.0);
  CHECK_NEAR(0.00258064, test, 5.0e-9);
  test = convert<acre, square_foot>(5.0);
  CHECK_NEAR(217800.0, test, 5.0e-5);
  test = convert<square_meter, square_foot>(1.0);
  CHECK_NEAR(10.7639, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion pressure", "[wpimath]") {
  double test;

  test = convert<pascals, torr>(1.0);
  CHECK_NEAR(0.00750062, test, 5.0e-5);
  test = convert<bar, psi>(2.2);
  CHECK_NEAR(31.9083, test, 5.0e-5);
  test = convert<atmospheres, bar>(4.0);
  CHECK_NEAR(4.053, test, 5.0e-5);
  test = convert<torr, pascals>(800.0);
  CHECK_NEAR(106657.89474, test, 5.0e-5);
  test = convert<psi, atmospheres>(38.0);
  CHECK_NEAR(2.58575, test, 5.0e-5);
  test = convert<psi, pascals>(1.0);
  CHECK_NEAR(6894.76, test, 5.0e-3);
  test = convert<pascals, bar>(0.25);
  CHECK_NEAR(2.5e-6, test, 5.0e-5);
  test = convert<torr, atmospheres>(9.0);
  CHECK_NEAR(0.0118421, test, 5.0e-8);
  test = convert<bar, torr>(12.0);
  CHECK_NEAR(9000.74, test, 5.0e-3);
  test = convert<atmospheres, psi>(1.0);
  CHECK_NEAR(14.6959, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion charge", "[wpimath]") {
  double test;

  test = convert<coulombs, ampere_hours>(4.0);
  CHECK_NEAR(0.00111111, test, 5.0e-9);
  test = convert<ampere_hours, coulombs>(1.0);
  CHECK_NEAR(3600.0, test, 5.0e-6);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion energy", "[wpimath]") {
  double test;

  test = convert<joules, calories>(8000.000464);
  CHECK_NEAR(1912.046, test, 5.0e-4);
  test = convert<therms, joules>(12.0);
  CHECK_NEAR(1.266e+9, test, 5.0e5);
  test = convert<megajoules, watt_hours>(100.0);
  CHECK_NEAR(27777.778, test, 5.0e-4);
  test = convert<kilocalories, megajoules>(56.0);
  CHECK_NEAR(0.234304, test, 5.0e-7);
  test = convert<kilojoules, therms>(56.0);
  CHECK_NEAR(0.000530904, test, 5.0e-5);
  test = convert<british_thermal_units, kilojoules>(18.56399995447);
  CHECK_NEAR(19.5860568, test, 5.0e-5);
  test = convert<calories, energy::foot_pounds>(18.56399995447);
  CHECK_NEAR(57.28776190423856, test, 5.0e-5);
  test = convert<megajoules, calories>(1.0);
  CHECK_NEAR(239006.0, test, 5.0e-1);
  test = convert<kilocalories, kilowatt_hours>(2.0);
  CHECK_NEAR(0.00232444, test, 5.0e-9);
  test = convert<therms, kilocalories>(0.1);
  CHECK_NEAR(2521.04, test, 5.0e-3);
  test = convert<watt_hours, megajoules>(67.0);
  CHECK_NEAR(0.2412, test, 5.0e-5);
  test = convert<british_thermal_units, watt_hours>(100.0);
  CHECK_NEAR(29.3071, test, 5.0e-5);
  test = convert<calories, BTU>(100.0);
  CHECK_NEAR(0.396567, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion power", "[wpimath]") {
  double test;

  test = convert<compound_unit<energy::foot_pounds, inverse<seconds>>, watts>(
      550.0);
  CHECK_NEAR(745.7, test, 5.0e-2);
  test = convert<watts, gigawatts>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-4);
  test = convert<microwatts, watts>(200000.0);
  CHECK_NEAR(0.2, test, 5.0e-4);
  test = convert<horsepower, watts>(100.0);
  CHECK_NEAR(74570.0, test, 5.0e-1);
  test = convert<horsepower, megawatts>(5.0);
  CHECK_NEAR(0.0037284994, test, 5.0e-7);
  test = convert<kilowatts, horsepower>(232.0);
  CHECK_NEAR(311.117, test, 5.0e-4);
  test = convert<milliwatts, horsepower>(1001.0);
  CHECK_NEAR(0.001342363, test, 5.0e-9);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion voltage", "[wpimath]") {
  double test;

  test = convert<volts, millivolts>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picovolts, volts>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanovolts, volts>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microvolts, volts>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millivolts, volts>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilovolts, volts>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megavolts, volts>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigavolts, volts>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<statvolts, volts>(299.792458);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millivolts, statvolts>(1000.0);
  CHECK_NEAR(299.792458, test, 5.0e-5);
  test = convert<abvolts, nanovolts>(0.1);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microvolts, abvolts>(0.01);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion capacitance", "[wpimath]") {
  double test;

  test = convert<farads, millifarads>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picofarads, farads>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanofarads, farads>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microfarads, farads>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millifarads, farads>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilofarads, farads>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megafarads, farads>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigafarads, farads>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion impedance", "[wpimath]") {
  double test;

  test = convert<ohms, milliohms>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoohms, ohms>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoohms, ohms>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microohms, ohms>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<milliohms, ohms>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloohms, ohms>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megaohms, ohms>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigaohms, ohms>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion conductance", "[wpimath]") {
  double test;

  test = convert<siemens, millisiemens>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picosiemens, siemens>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanosiemens, siemens>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microsiemens, siemens>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millisiemens, siemens>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilosiemens, siemens>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megasiemens, siemens>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigasiemens, siemens>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion magnetic flux", "[wpimath]") {
  double test;

  test = convert<webers, milliwebers>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picowebers, webers>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanowebers, webers>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microwebers, webers>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<milliwebers, webers>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilowebers, webers>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megawebers, webers>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigawebers, webers>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<maxwells, webers>(100000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanowebers, maxwells>(10.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion magnetic field strength", "[wpimath]") {
  double test;

  test = convert<teslas, milliteslas>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoteslas, teslas>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoteslas, teslas>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microteslas, teslas>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<milliteslas, teslas>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloteslas, teslas>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megateslas, teslas>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigateslas, teslas>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gauss, teslas>(10000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoteslas, gauss>(100000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion inductance", "[wpimath]") {
  double test;

  test = convert<henries, millihenries>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picohenries, henries>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanohenries, henries>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microhenries, henries>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millihenries, henries>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilohenries, henries>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megahenries, henries>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigahenries, henries>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion luminous flux", "[wpimath]") {
  double test;

  test = convert<lumens, millilumens>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picolumens, lumens>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanolumens, lumens>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microlumens, lumens>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millilumens, lumens>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilolumens, lumens>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megalumens, lumens>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigalumens, lumens>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion illuminance", "[wpimath]") {
  double test;

  test = convert<luxes, milliluxes>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoluxes, luxes>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoluxes, luxes>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microluxes, luxes>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<milliluxes, luxes>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloluxes, luxes>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megaluxes, luxes>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigaluxes, luxes>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);

  test = convert<footcandles, luxes>(0.092903);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<lux, lumens_per_square_inch>(1550.0031000062);
  CHECK_NEAR(1.0, test, 5.0e-13);
  test = convert<phots, luxes>(0.0001);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion radiation", "[wpimath]") {
  double test;

  test = convert<becquerels, millibecquerels>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picobecquerels, becquerels>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanobecquerels, becquerels>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microbecquerels, becquerels>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millibecquerels, becquerels>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilobecquerels, becquerels>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megabecquerels, becquerels>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigabecquerels, becquerels>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);

  test = convert<grays, milligrays>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picograys, grays>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanograys, grays>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<micrograys, grays>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<milligrays, grays>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilograys, grays>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megagrays, grays>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigagrays, grays>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);

  test = convert<sieverts, millisieverts>(10.0);
  CHECK_NEAR(10000.0, test, 5.0e-5);
  test = convert<picosieverts, sieverts>(1000000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<nanosieverts, sieverts>(1000000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<microsieverts, sieverts>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<millisieverts, sieverts>(1000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<kilosieverts, sieverts>(0.001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<megasieverts, sieverts>(0.000001);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<gigasieverts, sieverts>(0.000000001);
  CHECK_NEAR(1.0, test, 5.0e-5);

  test = convert<becquerels, curies>(37.0e9);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<becquerels, rutherfords>(1000000.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<rads, grays>(100.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion torque", "[wpimath]") {
  double test;

  test = convert<torque::foot_pounds, newton_meter>(1.0);
  CHECK_NEAR(1.355817948, test, 5.0e-5);
  test = convert<inch_pounds, newton_meter>(1.0);
  CHECK_NEAR(0.112984829, test, 5.0e-5);
  test = convert<foot_poundals, newton_meter>(1.0);
  CHECK_NEAR(4.214011009e-2, test, 5.0e-5);
  test = convert<meter_kilograms, newton_meter>(1.0);
  CHECK_NEAR(9.80665, test, 5.0e-5);
  test = convert<inch_pound, meter_kilogram>(86.79616930855788);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<foot_poundals, inch_pound>(2.681170713);
  CHECK_NEAR(1.0, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion volume", "[wpimath]") {
  double test;

  test = convert<cubic_meters, cubic_meter>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<cubic_millimeters, cubic_meter>(1.0);
  CHECK_NEAR(1.0e-9, test, 5.0e-5);
  test = convert<cubic_kilometers, cubic_meter>(1.0);
  CHECK_NEAR(1.0e9, test, 5.0e-5);
  test = convert<liters, cubic_meter>(1.0);
  CHECK_NEAR(0.001, test, 5.0e-5);
  test = convert<milliliters, cubic_meter>(1.0);
  CHECK_NEAR(1.0e-6, test, 5.0e-5);
  test = convert<cubic_inches, cubic_meter>(1.0);
  CHECK_NEAR(1.6387e-5, test, 5.0e-10);
  test = convert<cubic_feet, cubic_meter>(1.0);
  CHECK_NEAR(0.0283168, test, 5.0e-8);
  test = convert<cubic_yards, cubic_meter>(1.0);
  CHECK_NEAR(0.764555, test, 5.0e-7);
  test = convert<cubic_miles, cubic_meter>(1.0);
  CHECK_NEAR(4.168e+9, test, 5.0e5);
  test = convert<gallons, cubic_meter>(1.0);
  CHECK_NEAR(0.00378541, test, 5.0e-8);
  test = convert<quarts, cubic_meter>(1.0);
  CHECK_NEAR(0.000946353, test, 5.0e-10);
  test = convert<pints, cubic_meter>(1.0);
  CHECK_NEAR(0.000473176, test, 5.0e-10);
  test = convert<cups, cubic_meter>(1.0);
  CHECK_NEAR(0.00024, test, 5.0e-6);
  test = convert<volume::fluid_ounces, cubic_meter>(1.0);
  CHECK_NEAR(2.9574e-5, test, 5.0e-5);
  test = convert<barrels, cubic_meter>(1.0);
  CHECK_NEAR(0.158987294928, test, 5.0e-13);
  test = convert<bushels, cubic_meter>(1.0);
  CHECK_NEAR(0.0352391, test, 5.0e-8);
  test = convert<cords, cubic_meter>(1.0);
  CHECK_NEAR(3.62456, test, 5.0e-6);
  test = convert<cubic_fathoms, cubic_meter>(1.0);
  CHECK_NEAR(6.11644, test, 5.0e-6);
  test = convert<tablespoons, cubic_meter>(1.0);
  CHECK_NEAR(1.4787e-5, test, 5.0e-10);
  test = convert<teaspoons, cubic_meter>(1.0);
  CHECK_NEAR(4.9289e-6, test, 5.0e-11);
  test = convert<pinches, cubic_meter>(1.0);
  CHECK_NEAR(616.11519921875e-9, test, 5.0e-20);
  test = convert<dashes, cubic_meter>(1.0);
  CHECK_NEAR(308.057599609375e-9, test, 5.0e-20);
  test = convert<drops, cubic_meter>(1.0);
  CHECK_NEAR(82.14869322916e-9, test, 5.0e-9);
  test = convert<fifths, cubic_meter>(1.0);
  CHECK_NEAR(0.00075708236, test, 5.0e-12);
  test = convert<drams, cubic_meter>(1.0);
  CHECK_NEAR(3.69669e-6, test, 5.0e-12);
  test = convert<gills, cubic_meter>(1.0);
  CHECK_NEAR(0.000118294, test, 5.0e-10);
  test = convert<pecks, cubic_meter>(1.0);
  CHECK_NEAR(0.00880977, test, 5.0e-9);
  test = convert<sacks, cubic_meter>(9.4591978);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<shots, cubic_meter>(1.0);
  CHECK_NEAR(4.43603e-5, test, 5.0e-11);
  test = convert<strikes, cubic_meter>(1.0);
  CHECK_NEAR(0.07047814033376, test, 5.0e-5);
  test = convert<volume::fluid_ounces, milliliters>(1.0);
  CHECK_NEAR(29.5735, test, 5.0e-5);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion density", "[wpimath]") {
  double test;

  test = convert<kilograms_per_cubic_meter, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(1.0, test, 5.0e-5);
  test = convert<grams_per_milliliter, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(1000.0, test, 5.0e-5);
  test = convert<kilograms_per_liter, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(1000.0, test, 5.0e-5);
  test = convert<ounces_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(1.001153961, test, 5.0e-10);
  test = convert<ounces_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(1.729994044e3, test, 5.0e-7);
  test = convert<ounces_per_gallon, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(7.489151707, test, 5.0e-10);
  test = convert<pounds_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(16.01846337, test, 5.0e-9);
  test = convert<pounds_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(2.767990471e4, test, 5.0e-6);
  test = convert<pounds_per_gallon, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(119.8264273, test, 5.0e-8);
  test = convert<slugs_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  CHECK_NEAR(515.3788184, test, 5.0e-6);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion concentration", "[wpimath]") {
  double test;

  test = ppm_t(1.0);
  CHECK_NEAR(1.0e-6, test, 5.0e-12);
  test = ppb_t(1.0);
  CHECK_NEAR(1.0e-9, test, 5.0e-12);
  test = ppt_t(1.0);
  CHECK_NEAR(1.0e-12, test, 5.0e-12);
  test = percent_t(18.0);
  CHECK_NEAR(0.18, test, 5.0e-12);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion data", "[wpimath]") {
  CHECK(8 == (convert<byte, bit>(1)));

  CHECK(1000 == (convert<kilobytes, bytes>(1)));
  CHECK(1000 == (convert<megabytes, kilobytes>(1)));
  CHECK(1000 == (convert<gigabytes, megabytes>(1)));
  CHECK(1000 == (convert<terabytes, gigabytes>(1)));
  CHECK(1000 == (convert<petabytes, terabytes>(1)));
  CHECK(1000 == (convert<exabytes, petabytes>(1)));

  CHECK(1024 == (convert<kibibytes, bytes>(1)));
  CHECK(1024 == (convert<mebibytes, kibibytes>(1)));
  CHECK(1024 == (convert<gibibytes, mebibytes>(1)));
  CHECK(1024 == (convert<tebibytes, gibibytes>(1)));
  CHECK(1024 == (convert<pebibytes, tebibytes>(1)));
  CHECK(1024 == (convert<exbibytes, pebibytes>(1)));

  CHECK(93750000 == (convert<gigabytes, kibibits>(12)));

  CHECK(1000 == (convert<kilobits, bits>(1)));
  CHECK(1000 == (convert<megabits, kilobits>(1)));
  CHECK(1000 == (convert<gigabits, megabits>(1)));
  CHECK(1000 == (convert<terabits, gigabits>(1)));
  CHECK(1000 == (convert<petabits, terabits>(1)));
  CHECK(1000 == (convert<exabits, petabits>(1)));

  CHECK(1024 == (convert<kibibits, bits>(1)));
  CHECK(1024 == (convert<mebibits, kibibits>(1)));
  CHECK(1024 == (convert<gibibits, mebibits>(1)));
  CHECK(1024 == (convert<tebibits, gibibits>(1)));
  CHECK(1024 == (convert<pebibits, tebibits>(1)));
  CHECK(1024 == (convert<exbibits, pebibits>(1)));

  // Source: https://en.wikipedia.org/wiki/Binary_prefix
  CHECK_NEAR(percent_t(2.4), kibibyte_t(1) / kilobyte_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(4.9), mebibyte_t(1) / megabyte_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(7.4), gibibyte_t(1) / gigabyte_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(10.0), tebibyte_t(1) / terabyte_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(12.6), pebibyte_t(1) / petabyte_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(15.3), exbibyte_t(1) / exabyte_t(1) - 1, 0.005);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion data transfer rate", "[wpimath]") {
  CHECK(8 == (convert<bytes_per_second, bits_per_second>(1)));

  CHECK(1000 == (convert<kilobytes_per_second, bytes_per_second>(1)));
  CHECK(1000 == (convert<megabytes_per_second, kilobytes_per_second>(1)));
  CHECK(1000 == (convert<gigabytes_per_second, megabytes_per_second>(1)));
  CHECK(1000 == (convert<terabytes_per_second, gigabytes_per_second>(1)));
  CHECK(1000 == (convert<petabytes_per_second, terabytes_per_second>(1)));
  CHECK(1000 == (convert<exabytes_per_second, petabytes_per_second>(1)));

  CHECK(1024 == (convert<kibibytes_per_second, bytes_per_second>(1)));
  CHECK(1024 == (convert<mebibytes_per_second, kibibytes_per_second>(1)));
  CHECK(1024 == (convert<gibibytes_per_second, mebibytes_per_second>(1)));
  CHECK(1024 == (convert<tebibytes_per_second, gibibytes_per_second>(1)));
  CHECK(1024 == (convert<pebibytes_per_second, tebibytes_per_second>(1)));
  CHECK(1024 == (convert<exbibytes_per_second, pebibytes_per_second>(1)));

  CHECK(93750000 == (convert<gigabytes_per_second, kibibits_per_second>(12)));

  CHECK(1000 == (convert<kilobits_per_second, bits_per_second>(1)));
  CHECK(1000 == (convert<megabits_per_second, kilobits_per_second>(1)));
  CHECK(1000 == (convert<gigabits_per_second, megabits_per_second>(1)));
  CHECK(1000 == (convert<terabits_per_second, gigabits_per_second>(1)));
  CHECK(1000 == (convert<petabits_per_second, terabits_per_second>(1)));
  CHECK(1000 == (convert<exabits_per_second, petabits_per_second>(1)));

  CHECK(1024 == (convert<kibibits_per_second, bits_per_second>(1)));
  CHECK(1024 == (convert<mebibits_per_second, kibibits_per_second>(1)));
  CHECK(1024 == (convert<gibibits_per_second, mebibits_per_second>(1)));
  CHECK(1024 == (convert<tebibits_per_second, gibibits_per_second>(1)));
  CHECK(1024 == (convert<pebibits_per_second, tebibits_per_second>(1)));
  CHECK(1024 == (convert<exbibits_per_second, pebibits_per_second>(1)));

  // Source: https://en.wikipedia.org/wiki/Binary_prefix
  CHECK_NEAR(percent_t(2.4), kibibytes_per_second_t(1) / kilobytes_per_second_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(4.9), mebibytes_per_second_t(1) / megabytes_per_second_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(7.4), gibibytes_per_second_t(1) / gigabytes_per_second_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(10.0), tebibytes_per_second_t(1) / terabytes_per_second_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(12.6), pebibytes_per_second_t(1) / petabytes_per_second_t(1) - 1, 0.005);
  CHECK_NEAR(percent_t(15.3), exbibytes_per_second_t(1) / exabytes_per_second_t(1) - 1, 0.005);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion pi", "[wpimath]") {
  CHECK(wpi::units::traits::is_dimensionless_unit_v<decltype(constants::pi)>);
  CHECK(wpi::units::traits::is_dimensionless_unit_v<constants::PI>);

  // implicit conversion/arithmetic
  CHECK_NEAR(3.14159, constants::pi, 5.0e-6);
  CHECK_NEAR(6.28318531, (2 * constants::pi), 5.0e-9);
  CHECK_NEAR(6.28318531, (constants::pi + constants::pi), 5.0e-9);
  CHECK_NEAR(0.0, (constants::pi - constants::pi), 5.0e-9);
  CHECK_NEAR(31.00627668, wpi::units::math::cpow<3>(constants::pi), 5.0e-10);
  CHECK_NEAR(0.0322515344, (1.0 / wpi::units::math::cpow<3>(constants::pi)), 5.0e-11);
  CHECK(constants::detail::PI_VAL == constants::pi);
  CHECK(1.0 != constants::pi);
  CHECK(4.0 > constants::pi);
  CHECK(3.0 < constants::pi);
  CHECK(constants::pi > 3.0);
  CHECK(constants::pi < 4.0);

  // explicit conversion
  CHECK_NEAR(3.14159, constants::pi.value(), 5.0e-6);

  // auto multiplication
  CHECK((std::is_same_v<meter_t, decltype(constants::pi * meter_t(1))>));
  CHECK((std::is_same_v<meter_t, decltype(meter_t(1) * constants::pi)>));

  CHECK_NEAR(constants::detail::PI_VAL, (constants::pi * meter_t(1)).value(), 5.0e-10);
  CHECK_NEAR(constants::detail::PI_VAL, (meter_t(1) * constants::pi).value(), 5.0e-10);

  // explicit multiplication
  meter_t a = constants::pi * meter_t(1);
  meter_t b = meter_t(1) * constants::pi;

  CHECK_NEAR(constants::detail::PI_VAL, a.value(), 5.0e-10);
  CHECK_NEAR(constants::detail::PI_VAL, b.value(), 5.0e-10);

  // auto division
  CHECK((std::is_same_v<hertz_t, decltype(constants::pi / second_t(1))>));
  CHECK((std::is_same_v<second_t, decltype(second_t(1) / constants::pi)>));

  CHECK_NEAR(constants::detail::PI_VAL, (constants::pi / second_t(1)).value(), 5.0e-10);
  CHECK_NEAR(1.0 / constants::detail::PI_VAL, (second_t(1) / constants::pi).value(), 5.0e-10);

  // explicit
  hertz_t c = constants::pi / second_t(1);
  second_t d = second_t(1) / constants::pi;

  CHECK_NEAR(constants::detail::PI_VAL, c.value(), 5.0e-10);
  CHECK_NEAR(1.0 / constants::detail::PI_VAL, d.value(), 5.0e-10);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion constants", "[wpimath]") {
  // Source: NIST "2014 CODATA recommended values"
  CHECK_NEAR(299792458, constants::c(), 5.0e-9);
  CHECK_NEAR(6.67408e-11, constants::G(), 5.0e-17);
  CHECK_NEAR(6.626070040e-34, constants::h(), 5.0e-44);
  CHECK_NEAR(1.2566370614e-6, constants::mu0(), 5.0e-17);
  CHECK_NEAR(8.854187817e-12, constants::epsilon0(), 5.0e-21);
  CHECK_NEAR(376.73031346177, constants::Z0(), 5.0e-12);
  CHECK_NEAR(8987551787.3681764, constants::k_e(), 5.0e-6);
  CHECK_NEAR(1.6021766208e-19, constants::e(), 5.0e-29);
  CHECK_NEAR(9.10938356e-31, constants::m_e(), 5.0e-40);
  CHECK_NEAR(1.672621898e-27, constants::m_p(), 5.0e-37);
  CHECK_NEAR(9.274009994e-24, constants::mu_B(), 5.0e-32);
  CHECK_NEAR(6.022140857e23, constants::N_A(), 5.0e14);
  CHECK_NEAR(8.3144598, constants::R(), 5.0e-8);
  CHECK_NEAR(1.38064852e-23, constants::k_B(), 5.0e-31);
  CHECK_NEAR(96485.33289, constants::F(), 5.0e-5);
  CHECK_NEAR(5.670367e-8, constants::sigma(), 5.0e-14);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion std chrono", "[wpimath]") {
  nanosecond_t a = std::chrono::nanoseconds(10);
  CHECK(nanosecond_t(10) == a);
  microsecond_t b = std::chrono::microseconds(10);
  CHECK(microsecond_t(10) == b);
  millisecond_t c = std::chrono::milliseconds(10);
  CHECK(millisecond_t(10) == c);
  second_t d = std::chrono::seconds(1);
  CHECK(second_t(1) == d);
  minute_t e = std::chrono::minutes(120);
  CHECK(minute_t(120) == e);
  hour_t f = std::chrono::hours(2);
  CHECK(hour_t(2) == f);

  std::chrono::nanoseconds g = nanosecond_t(100);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(g).count() == 100);
  std::chrono::nanoseconds h = microsecond_t(2);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(h).count() == 2000);
  std::chrono::nanoseconds i = millisecond_t(1);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(i).count() == 1000000);
  std::chrono::nanoseconds j = second_t(1);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(j).count() == 1000000000);
  std::chrono::nanoseconds k = minute_t(1);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(k).count() == 60000000000);
  std::chrono::nanoseconds l = hour_t(1);
  CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(l).count() == 3600000000000);
}

TEST_CASE_METHOD(UnitConversion, "UnitConversion squaredTemperature", "[wpimath]") {
  using squared_celsius = wpi::units::compound_unit<squared<celsius>>;
  using squared_celsius_t = wpi::units::unit_t<squared_celsius>;
  const squared_celsius_t right(100);
  const celsius_t rootRight = wpi::units::math::sqrt(right);
  CHECK(celsius_t(10) == rootRight);
}

TEST_CASE_METHOD(UnitMath, "UnitMath min", "[wpimath]") {
  meter_t a(1);
  foot_t c(1);
  CHECK(c == math::min(a, c));
}

TEST_CASE_METHOD(UnitMath, "UnitMath max", "[wpimath]") {
  meter_t a(1);
  foot_t c(1);
  CHECK(a == math::max(a, c));
}

TEST_CASE_METHOD(UnitMath, "UnitMath cos", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                cos(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(-0.41614683654), cos(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(-0.70710678118), cos(angle::degree_t(135)), 5.0e-11);
}

TEST_CASE_METHOD(UnitMath, "UnitMath sin", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                sin(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(0.90929742682), sin(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(0.70710678118), sin(angle::degree_t(135)), 5.0e-11);
}

TEST_CASE_METHOD(UnitMath, "UnitMath tan", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                tan(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(-2.18503986326), tan(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(-1.0), tan(angle::degree_t(135)), 5.0e-11);
}

TEST_CASE_METHOD(UnitMath, "UnitMath acos", "[wpimath]") {
  CHECK((std::is_same_v<
          typename std::decay_t<angle::radian_t>,
          typename std::decay_t<decltype(acos(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(2).value(), acos(scalar_t(-0.41614683654)).value(), 5.0e-11);
  CHECK_NEAR(angle::degree_t(135).value(), angle::degree_t(acos(scalar_t(-0.70710678118654752440084436210485)))
          .value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath asin", "[wpimath]") {
  CHECK((std::is_same_v<
          typename std::decay_t<angle::radian_t>,
          typename std::decay_t<decltype(asin(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(1.14159265).value(), asin(scalar_t(0.90929742682)).value(), 5.0e-9);
  CHECK_NEAR(angle::degree_t(45).value(), angle::degree_t(asin(scalar_t(0.70710678118654752440084436210485)))
          .value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath atan", "[wpimath]") {
  CHECK((std::is_same_v<
          typename std::decay_t<angle::radian_t>,
          typename std::decay_t<decltype(atan(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(-1.14159265).value(), atan(scalar_t(-2.18503986326)).value(), 5.0e-9);
  CHECK_NEAR(angle::degree_t(-45).value(), angle::degree_t(atan(scalar_t(-1.0))).value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath atan2", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(atan2(
                                scalar_t(1), scalar_t(1)))>>));
  CHECK_NEAR(angle::radian_t(constants::detail::PI_VAL / 4).value(), atan2(scalar_t(2), scalar_t(2)).value(), 5.0e-12);
  CHECK_NEAR(angle::degree_t(45).value(), angle::degree_t(atan2(scalar_t(2), scalar_t(2))).value(), 5.0e-12);

  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(atan2(
                                scalar_t(1), scalar_t(1)))>>));
  CHECK_NEAR(angle::radian_t(constants::detail::PI_VAL / 6).value(), atan2(scalar_t(1), scalar_t(std::sqrt(3))).value(), 5.0e-12);
  CHECK_NEAR(angle::degree_t(30).value(), angle::degree_t(atan2(scalar_t(1), scalar_t(std::sqrt(3))))
                  .value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath cosh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                cosh(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(3.76219569108), cosh(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(5.32275215), cosh(angle::degree_t(135)), 5.0e-9);
}

TEST_CASE_METHOD(UnitMath, "UnitMath sinh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                sinh(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(3.62686040785), sinh(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(5.22797192), sinh(angle::degree_t(135)), 5.0e-9);
}

TEST_CASE_METHOD(UnitMath, "UnitMath tanh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<scalar_t>,
                            typename std::decay_t<decltype(
                                tanh(angle::radian_t(0)))>>));
  CHECK_NEAR(scalar_t(0.96402758007), tanh(angle::radian_t(2)), 5.0e-11);
  CHECK_NEAR(scalar_t(0.98219338), tanh(angle::degree_t(135)), 5.0e-11);
}

TEST_CASE_METHOD(UnitMath, "UnitMath acosh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(
                                acosh(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(1.316957896924817).value(), acosh(scalar_t(2.0)).value(), 5.0e-11);
  CHECK_NEAR(angle::degree_t(75.456129290216893).value(), angle::degree_t(acosh(scalar_t(2.0))).value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath asinh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(
                                asinh(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(1.443635475178810).value(), asinh(scalar_t(2)).value(), 5.0e-9);
  CHECK_NEAR(angle::degree_t(82.714219883108939).value(), angle::degree_t(asinh(scalar_t(2))).value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath atanh", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(
                                atanh(scalar_t(0)))>>));
  CHECK_NEAR(angle::radian_t(0.549306144334055).value(), atanh(scalar_t(0.5)).value(), 5.0e-9);
  CHECK_NEAR(angle::degree_t(31.472923730945389).value(), angle::degree_t(atanh(scalar_t(0.5))).value(), 5.0e-12);
}

TEST_CASE_METHOD(UnitMath, "UnitMath exp", "[wpimath]") {
  double val = 10.0;
  CHECK(std::exp(val) == exp(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log", "[wpimath]") {
  double val = 100.0;
  CHECK(std::log(val) == log(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log10", "[wpimath]") {
  double val = 100.0;
  CHECK(std::log10(val) == log10(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath modf", "[wpimath]") {
  double val = 100.0;
  double modfr1;
  scalar_t modfr2;
  CHECK(std::modf(val, &modfr1) == modf(scalar_t(val), &modfr2));
  CHECK(modfr1 == modfr2);
}

TEST_CASE_METHOD(UnitMath, "UnitMath exp2", "[wpimath]") {
  double val = 10.0;
  CHECK(std::exp2(val) == exp2(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath expm1", "[wpimath]") {
  double val = 10.0;
  CHECK(std::expm1(val) == expm1(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log1p", "[wpimath]") {
  double val = 10.0;
  CHECK(std::log1p(val) == log1p(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log2", "[wpimath]") {
  double val = 10.0;
  CHECK(std::log2(val) == log2(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath pow", "[wpimath]") {
  bool isSame;
  meter_t value(10.0);

  auto sq = pow<2>(value);
  CHECK_NEAR(100.0, sq(), 5.0e-2);
  isSame = std::is_same_v<decltype(sq), square_meter_t>;
  CHECK(isSame);

  auto cube = pow<3>(value);
  CHECK_NEAR(1000.0, cube(), 5.0e-2);
  isSame = std::is_same_v<decltype(cube), unit_t<cubed<meter>>>;
  CHECK(isSame);

  auto fourth = pow<4>(value);
  CHECK_NEAR(10000.0, fourth(), 5.0e-2);
  isSame = std::is_same_v<
      decltype(fourth),
      unit_t<compound_unit<squared<meter>, squared<meter>>>>;
  CHECK(isSame);
}

TEST_CASE_METHOD(UnitMath, "UnitMath sqrt", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<meter_t>,
                            typename std::decay_t<decltype(sqrt(
                                square_meter_t(4.0)))>>));
  CHECK_NEAR(meter_t(2.0).value(), sqrt(square_meter_t(4.0)).value(), 5.0e-9);

  CHECK((std::is_same_v<typename std::decay_t<angle::radian_t>,
                            typename std::decay_t<decltype(
                                sqrt(steradian_t(16.0)))>>));
  CHECK_NEAR(angle::radian_t(4.0).value(), sqrt(steradian_t(16.0)).value(), 5.0e-9);

  CHECK((std::is_convertible_v<typename std::decay_t<foot_t>,
                                   typename std::decay_t<decltype(sqrt(
                                       square_foot_t(10.0)))>>));

  // for rational conversion (i.e. no integral root) let's check a bunch of
  // different ways this could go wrong
  foot_t resultFt = sqrt(square_foot_t(10.0));
  CHECK_NEAR(foot_t(3.16227766017).value(), sqrt(square_foot_t(10.0)).value(), 5.0e-9);
  CHECK_NEAR(foot_t(3.16227766017).value(), resultFt.value(), 5.0e-9);
  CHECK(resultFt == sqrt(square_foot_t(10.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath hypot", "[wpimath]") {
  CHECK((std::is_same_v<typename std::decay_t<meter_t>,
                            typename std::decay_t<decltype(hypot(
                                meter_t(3.0), meter_t(4.0)))>>));
  CHECK_NEAR(meter_t(5.0).value(), (hypot(meter_t(3.0), meter_t(4.0))).value(), 5.0e-9);

  CHECK((std::is_same_v<typename std::decay_t<foot_t>,
                            typename std::decay_t<decltype(hypot(
                                foot_t(3.0), meter_t(1.2192)))>>));
  CHECK_NEAR(foot_t(5.0).value(), (hypot(foot_t(3.0), meter_t(1.2192))).value(), 5.0e-9);
}

TEST_CASE_METHOD(UnitMath, "UnitMath ceil", "[wpimath]") {
  double val = 101.1;
  CHECK(std::ceil(val) == ceil(meter_t(val)).value());
  CHECK((std::is_same_v<typename std::decay_t<meter_t>,
                            typename std::decay_t<decltype(
                                ceil(meter_t(val)))>>));
}

TEST_CASE_METHOD(UnitMath, "UnitMath floor", "[wpimath]") {
  double val = 101.1;
  CHECK(std::floor(val) == floor(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmod", "[wpimath]") {
  CHECK(std::fmod(100.0, 101.2) == fmod(meter_t(100.0), meter_t(101.2)).value());
}

TEST_CASE_METHOD(UnitMath, "UnitMath trunc", "[wpimath]") {
  double val = 101.1;
  CHECK(std::trunc(val) == trunc(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath round", "[wpimath]") {
  double val = 101.1;
  CHECK(std::round(val) == round(scalar_t(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath copysign", "[wpimath]") {
  double sign = -1;
  meter_t val(5.0);
  CHECK(meter_t(-5.0) == copysign(val, sign));
  CHECK(meter_t(-5.0) == copysign(val, angle::radian_t(sign)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fdim", "[wpimath]") {
  CHECK(meter_t(0.0) == fdim(meter_t(8.0), meter_t(10.0)));
  CHECK(meter_t(2.0) == fdim(meter_t(10.0), meter_t(8.0)));
  CHECK_NEAR(meter_t(9.3904).value(), fdim(meter_t(10.0), foot_t(2.0)).value(), 5.0e-320);  // not sure why they aren't comparing exactly equal,
                          // but clearly they are.
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmin", "[wpimath]") {
  CHECK(meter_t(8.0) == fmin(meter_t(8.0), meter_t(10.0)));
  CHECK(meter_t(8.0) == fmin(meter_t(10.0), meter_t(8.0)));
  CHECK(foot_t(2.0) == fmin(meter_t(10.0), foot_t(2.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmax", "[wpimath]") {
  CHECK(meter_t(10.0) == fmax(meter_t(8.0), meter_t(10.0)));
  CHECK(meter_t(10.0) == fmax(meter_t(10.0), meter_t(8.0)));
  CHECK(meter_t(10.0) == fmax(meter_t(10.0), foot_t(2.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fabs", "[wpimath]") {
  CHECK(meter_t(10.0) == fabs(meter_t(-10.0)));
  CHECK(meter_t(10.0) == fabs(meter_t(10.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath abs", "[wpimath]") {
  CHECK(meter_t(10.0) == abs(meter_t(-10.0)));
  CHECK(meter_t(10.0) == abs(meter_t(10.0)));
}

// Constexpr
#if !defined(_MSC_VER) || _MSC_VER > 1800
TEST_CASE_METHOD(Constexpr, "Constexpr construction", "[wpimath]") {
  constexpr meter_t result0(0);
  constexpr auto result1 = make_unit<meter_t>(1);
  constexpr auto result2 = meter_t(2);
  constexpr auto result3 = -3_m;

  CHECK(meter_t(0) == result0);
  CHECK(meter_t(1) == result1);
  CHECK(meter_t(2) == result2);
  CHECK(meter_t(-3) == result3);

  CHECK(noexcept(result0));
  CHECK(noexcept(result1));
  CHECK(noexcept(result2));
  CHECK(noexcept(result3));
}

TEST_CASE_METHOD(Constexpr, "Constexpr constants", "[wpimath]") {
  CHECK(noexcept(constants::c()));
  CHECK(noexcept(constants::G()));
  CHECK(noexcept(constants::h()));
  CHECK(noexcept(constants::mu0()));
  CHECK(noexcept(constants::epsilon0()));
  CHECK(noexcept(constants::Z0()));
  CHECK(noexcept(constants::k_e()));
  CHECK(noexcept(constants::e()));
  CHECK(noexcept(constants::m_e()));
  CHECK(noexcept(constants::m_p()));
  CHECK(noexcept(constants::mu_B()));
  CHECK(noexcept(constants::N_A()));
  CHECK(noexcept(constants::R()));
  CHECK(noexcept(constants::k_B()));
  CHECK(noexcept(constants::F()));
  CHECK(noexcept(constants::sigma()));
}

TEST_CASE_METHOD(Constexpr, "Constexpr arithmetic", "[wpimath]") {
  constexpr auto result0(1_m + 1_m);
  constexpr auto result1(1_m - 1_m);
  constexpr auto result2(1_m * 1_m);
  constexpr auto result3(1_m / 1_m);
  constexpr auto result4(meter_t(1) + meter_t(1));
  constexpr auto result5(meter_t(1) - meter_t(1));
  constexpr auto result6(meter_t(1) * meter_t(1));
  constexpr auto result7(meter_t(1) / meter_t(1));
  constexpr auto result8(wpi::units::math::cpow<2>(meter_t(2)));
  constexpr auto result9 = wpi::units::math::cpow<3>(2_m);
  constexpr auto result10 = 2_m * 2_m;

  CHECK(noexcept(result0));
  CHECK(noexcept(result1));
  CHECK(noexcept(result2));
  CHECK(noexcept(result3));
  CHECK(noexcept(result4));
  CHECK(noexcept(result5));
  CHECK(noexcept(result6));
  CHECK(noexcept(result7));
  CHECK(noexcept(result8));
  CHECK(noexcept(result9));
  CHECK(noexcept(result10));

  CHECK(8_cu_m == result9);
  CHECK(4_sq_m == result10);
}

TEST_CASE_METHOD(Constexpr, "Constexpr realtional", "[wpimath]") {
  constexpr bool equalityTrue = (1_m == 1_m);
  constexpr bool equalityFalse = (1_m == 2_m);
  constexpr bool lessThanTrue = (1_m < 2_m);
  constexpr bool lessThanFalse = (1_m < 1_m);
  constexpr bool lessThanEqualTrue1 = (1_m <= 1_m);
  constexpr bool lessThanEqualTrue2 = (1_m <= 2_m);
  constexpr bool lessThanEqualFalse = (1_m < 0_m);
  constexpr bool greaterThanTrue = (2_m > 1_m);
  constexpr bool greaterThanFalse = (2_m > 2_m);
  constexpr bool greaterThanEqualTrue1 = (2_m >= 1_m);
  constexpr bool greaterThanEqualTrue2 = (2_m >= 2_m);
  constexpr bool greaterThanEqualFalse = (2_m > 3_m);

  CHECK(equalityTrue);
  CHECK(lessThanTrue);
  CHECK(lessThanEqualTrue1);
  CHECK(lessThanEqualTrue2);
  CHECK(greaterThanTrue);
  CHECK(greaterThanEqualTrue1);
  CHECK(greaterThanEqualTrue2);
  CHECK_FALSE(equalityFalse);
  CHECK_FALSE(lessThanFalse);
  CHECK_FALSE(lessThanEqualFalse);
  CHECK_FALSE(greaterThanFalse);
  CHECK_FALSE(greaterThanEqualFalse);
}

TEST_CASE_METHOD(Constexpr, "Constexpr stdArray", "[wpimath]") {
  constexpr std::array<meter_t, 5> arr = {0_m, 1_m, 2_m, 3_m, 4_m};
  constexpr bool equal = (arr[3] == 3_m);
  CHECK(equal);
}

#endif

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value t", "[wpimath]") {
  using mRatio = unit_value_t<meters, 3, 2>;
  CHECK(meter_t(1.5) == mRatio::value());
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic is unit value t", "[wpimath]") {
  using mRatio = unit_value_t<meters, 3, 2>;

  CHECK(traits::is_unit_value_t_v<mRatio>);
  CHECK_FALSE(traits::is_unit_value_t_v<meter_t>);
  CHECK_FALSE(traits::is_unit_value_t_v<double>);

  CHECK((traits::is_unit_value_t_v<mRatio, meters>));
  CHECK_FALSE((traits::is_unit_value_t_v<meter_t, meters>));
  CHECK_FALSE((traits::is_unit_value_t_v<double, meters>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic is unit value t category", "[wpimath]") {
  using mRatio = unit_value_t<feet, 3, 2>;
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, mRatio>));
  CHECK_FALSE((traits::is_unit_value_t_category_v<category::angle_unit, mRatio>));
  CHECK_FALSE((
      traits::is_unit_value_t_category_v<category::length_unit, meter_t>));
  CHECK_FALSE((traits::is_unit_value_t_category_v<category::length_unit, double>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value add", "[wpimath]") {
  using mRatio = unit_value_t<meters, 3, 2>;

  using sum = unit_value_add<mRatio, mRatio>;
  CHECK(meter_t(3.0) == sum::value());
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, sum>));

  using ftRatio = unit_value_t<feet, 1>;

  using sumf = unit_value_add<ftRatio, mRatio>;
  CHECK((
      std::is_same_v<typename std::decay_t<foot_t>,
                   typename std::decay_t<decltype(sumf::value())>>));
  CHECK_NEAR(5.92125984, sumf::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, sumf>));

  using cRatio = unit_value_t<celsius, 1>;
  using fRatio = unit_value_t<fahrenheit, 2>;

  using sumc = unit_value_add<cRatio, fRatio>;
  CHECK((
      std::is_same_v<typename std::decay_t<celsius_t>,
                   typename std::decay_t<decltype(sumc::value())>>));
  CHECK_NEAR(2.11111111111, sumc::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::temperature_unit,
                                                sumc>));

  using rRatio = unit_value_t<angle::radian, 1>;
  using dRatio = unit_value_t<angle::degree, 3>;

  using sumr = unit_value_add<rRatio, dRatio>;
  CHECK((
      std::is_same_v<typename std::decay_t<angle::radian_t>,
                   typename std::decay_t<decltype(sumr::value())>>));
  CHECK_NEAR(1.05235988, sumr::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::angle_unit, sumr>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value subtract", "[wpimath]") {
  using mRatio = unit_value_t<meters, 3, 2>;

  using diff = unit_value_subtract<mRatio, mRatio>;
  CHECK(meter_t(0) == diff::value());
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, diff>));

  using ftRatio = unit_value_t<feet, 1>;

  using difff = unit_value_subtract<ftRatio, mRatio>;
  CHECK((std::is_same_v<
               typename std::decay_t<foot_t>,
               typename std::decay_t<decltype(difff::value())>>));
  CHECK_NEAR(-3.92125984, difff::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, difff>));

  using cRatio = unit_value_t<celsius, 1>;
  using fRatio = unit_value_t<fahrenheit, 2>;

  using diffc = unit_value_subtract<cRatio, fRatio>;
  CHECK((std::is_same_v<
               typename std::decay_t<celsius_t>,
               typename std::decay_t<decltype(diffc::value())>>));
  CHECK_NEAR(-0.11111111111, diffc::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::temperature_unit,
                                                diffc>));

  using rRatio = unit_value_t<angle::radian, 1>;
  using dRatio = unit_value_t<angle::degree, 3>;

  using diffr = unit_value_subtract<rRatio, dRatio>;
  CHECK((std::is_same_v<
               typename std::decay_t<angle::radian_t>,
               typename std::decay_t<decltype(diffr::value())>>));
  CHECK_NEAR(0.947640122, diffr::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::angle_unit, diffr>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value multiply", "[wpimath]") {
  using mRatio = unit_value_t<meters, 2>;
  using ftRatio = unit_value_t<feet, 656168, 100000>;  // 2 meter

  using product = unit_value_multiply<mRatio, mRatio>;
  CHECK(square_meter_t(4) == product::value());
  CHECK((traits::is_unit_value_t_category_v<category::area_unit, product>));

  using productM = unit_value_multiply<mRatio, ftRatio>;

  CHECK((std::is_same_v<
               typename std::decay_t<square_meter_t>,
               typename std::decay_t<decltype(productM::value())>>));
  CHECK_NEAR(4.0, productM::value().value(), 5.0e-7);
  CHECK((traits::is_unit_value_t_category_v<category::area_unit, productM>));

  using productF = unit_value_multiply<ftRatio, mRatio>;
  CHECK((std::is_same_v<
               typename std::decay_t<square_foot_t>,
               typename std::decay_t<decltype(productF::value())>>));
  CHECK_NEAR(43.0556444224, productF::value().value(), 5.0e-6);
  CHECK((traits::is_unit_value_t_category_v<category::area_unit, productF>));

  using productF2 = unit_value_multiply<ftRatio, ftRatio>;
  CHECK((std::is_same_v<
          typename std::decay_t<square_foot_t>,
          typename std::decay_t<decltype(productF2::value())>>));
  CHECK_NEAR(43.0556444224, productF2::value().value(), 5.0e-8);
  CHECK((
      traits::is_unit_value_t_category_v<category::area_unit, productF2>));

  using nRatio = unit_value_t<wpi::units::force::newton, 5>;

  using productN = unit_value_multiply<nRatio, ftRatio>;
  CHECK_FALSE((std::is_same_v<
          typename std::decay_t<torque::newton_meter_t>,
          typename std::decay_t<decltype(productN::value())>>));
  CHECK((std::is_convertible_v<
               typename std::decay_t<torque::newton_meter_t>,
               typename std::decay_t<decltype(productN::value())>>));
  CHECK_NEAR(32.8084, productN::value().value(), 5.0e-8);
  CHECK_NEAR(10.0, (productN::value().convert<newton_meter>().value()), 5.0e-7);
  CHECK((traits::is_unit_value_t_category_v<category::torque_unit,
                                                productN>));

  using r1Ratio = unit_value_t<angle::radian, 11, 10>;
  using r2Ratio = unit_value_t<angle::radian, 22, 10>;

  using productR = unit_value_multiply<r1Ratio, r2Ratio>;
  CHECK((std::is_same_v<
               typename std::decay_t<steradian_t>,
               typename std::decay_t<decltype(productR::value())>>));
  CHECK_NEAR(2.42, productR::value().value(), 5.0e-8);
  CHECK_NEAR(7944.39137, (productR::value().convert<degrees_squared>().value()), 5.0e-6);
  CHECK((traits::is_unit_value_t_category_v<category::solid_angle_unit,
                                                productR>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value divide", "[wpimath]") {
  using mRatio = unit_value_t<meters, 2>;
  using ftRatio = unit_value_t<feet, 656168, 100000>;  // 2 meter

  using product = unit_value_divide<mRatio, mRatio>;
  CHECK(scalar_t(1) == product::value());
  CHECK((
      traits::is_unit_value_t_category_v<category::scalar_unit, product>));

  using productM = unit_value_divide<mRatio, ftRatio>;

  CHECK((std::is_same_v<
               typename std::decay_t<scalar_t>,
               typename std::decay_t<decltype(productM::value())>>));
  CHECK_NEAR(1, productM::value().value(), 5.0e-7);
  CHECK((traits::is_unit_value_t_category_v<category::scalar_unit,
                                                productM>));

  using productF = unit_value_divide<ftRatio, mRatio>;
  CHECK((std::is_same_v<
               typename std::decay_t<scalar_t>,
               typename std::decay_t<decltype(productF::value())>>));
  CHECK_NEAR(1.0, productF::value().value(), 5.0e-6);
  CHECK((traits::is_unit_value_t_category_v<category::scalar_unit,
                                                productF>));

  using productF2 = unit_value_divide<ftRatio, ftRatio>;
  CHECK((std::is_same_v<
          typename std::decay_t<scalar_t>,
          typename std::decay_t<decltype(productF2::value())>>));
  CHECK_NEAR(1.0, productF2::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::scalar_unit,
                                                productF2>));

  using sRatio = unit_value_t<seconds, 10>;

  using productMS = unit_value_divide<mRatio, sRatio>;
  CHECK((std::is_same_v<
          typename std::decay_t<meters_per_second_t>,
          typename std::decay_t<decltype(productMS::value())>>));
  CHECK_NEAR(0.2, productMS::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::velocity_unit,
                                                productMS>));

  using rRatio = unit_value_t<angle::radian, 20>;

  using productRS = unit_value_divide<rRatio, sRatio>;
  CHECK((std::is_same_v<
          typename std::decay_t<radians_per_second_t>,
          typename std::decay_t<decltype(productRS::value())>>));
  CHECK_NEAR(2, productRS::value().value(), 5.0e-8);
  CHECK_NEAR(114.592, (productRS::value().convert<degrees_per_second>().value()), 5.0e-4);
  CHECK((traits::is_unit_value_t_category_v<category::angular_velocity_unit,
                                                productRS>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value power", "[wpimath]") {
  using mRatio = unit_value_t<meters, 2>;

  using sq = unit_value_power<mRatio, 2>;
  CHECK((std::is_convertible_v<
               typename std::decay_t<square_meter_t>,
               typename std::decay_t<decltype(sq::value())>>));
  CHECK_NEAR(4, sq::value().value(), 5.0e-8);
  CHECK((traits::is_unit_value_t_category_v<category::area_unit, sq>));

  using rRatio = unit_value_t<angle::radian, 18, 10>;

  using sqr = unit_value_power<rRatio, 2>;
  CHECK((std::is_convertible_v<
               typename std::decay_t<steradian_t>,
               typename std::decay_t<decltype(sqr::value())>>));
  CHECK_NEAR(3.24, sqr::value().value(), 5.0e-8);
  CHECK_NEAR(10636.292574038049895092690529904, (sqr::value().convert<degrees_squared>().value()), 5.0e-10);
  CHECK((traits::is_unit_value_t_category_v<category::solid_angle_unit,
                                                sqr>));
}

TEST_CASE_METHOD(CompileTimeArithmetic, "CompileTimeArithmetic unit value sqrt", "[wpimath]") {
  using mRatio = unit_value_t<square_meters, 10>;

  using root = unit_value_sqrt<mRatio>;
  CHECK((std::is_convertible_v<
               typename std::decay_t<meter_t>,
               typename std::decay_t<decltype(root::value())>>));
  CHECK_NEAR(3.16227766017, root::value().value(), 5.0e-9);
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, root>));

  using hRatio = unit_value_t<hectare, 51, 7>;

  using rooth = unit_value_sqrt<hRatio, 100000000>;
  CHECK((std::is_convertible_v<
               typename std::decay_t<mile_t>,
               typename std::decay_t<decltype(rooth::value())>>));
  CHECK_NEAR(2.69920623253, rooth::value().value(), 5.0e-8);
  CHECK_NEAR(269.920623, rooth::value().convert<meters>().value(), 5.0e-6);
  CHECK((traits::is_unit_value_t_category_v<category::length_unit, rooth>));

  using rRatio = unit_value_t<steradian, 18, 10>;

  using rootr = unit_value_sqrt<rRatio>;
  CHECK(traits::is_angle_unit_v<decltype(rootr::value())>);
  CHECK_NEAR(1.3416407865, rootr::value().value(), 5.0e-8);
  CHECK_NEAR(76.870352574, rootr::value().convert<angle::degrees>().value(), 5.0e-6);
  CHECK((traits::is_unit_value_t_category_v<category::angle_unit, rootr>));
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies radarRangeEquation", "[wpimath]") {
  watt_t P_t;            // transmit power
  scalar_t G;            // gain
  meter_t lambda;        // wavelength
  square_meter_t sigma;  // radar cross section
  meter_t R;             // range
  kelvin_t T_s;          // system noise temp
  hertz_t B_n;           // bandwidth
  scalar_t L;            // loss

  P_t = megawatt_t(1.4);
  G = dB_t(33.0);
  lambda = constants::c / megahertz_t(2800);
  sigma = square_meter_t(1.0);
  R = meter_t(111000.0);
  T_s = kelvin_t(950.0);
  B_n = megahertz_t(1.67);
  L = dB_t(8.0);

  scalar_t SNR = (P_t * math::pow<2>(G) * math::pow<2>(lambda) * sigma) /
                 (math::pow<3>(4 * constants::pi) * math::pow<4>(R) *
                  constants::k_B * T_s * B_n * L);

  CHECK_NEAR(1.535, SNR(), 5.0e-4);
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies pythagoreanTheorum", "[wpimath]") {
  CHECK(meter_t(3) == RightTriangle::a::value());
  CHECK(meter_t(4) == RightTriangle::b::value());
  CHECK(meter_t(5) == RightTriangle::c::value());
  CHECK(pow<2>(RightTriangle::a::value()) +
                  pow<2>(RightTriangle::b::value()) ==
              pow<2>(RightTriangle::c::value()));
}

TEST_CASE("Units overloadResolution", "[wpimath]") {
  // Slight hack to get nested functions
  struct Scope {
    static bool f(wpi::units::meter_t) {
      return true;
    };

    static bool f(wpi::units::second_t) {
      return false;
    };
  };
  // Make sure this properly selects the meter overload
  CHECK(Scope::f(1_mm));
}
