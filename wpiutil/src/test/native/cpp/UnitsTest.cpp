/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <array>
#include <chrono>
#include <string>
#include <type_traits>

#include "gtest/gtest.h"
#include "units/acceleration.h"
#include "units/angle.h"
#include "units/angular_acceleration.h"
#include "units/angular_velocity.h"
#include "units/area.h"
#include "units/capacitance.h"
#include "units/charge.h"
#include "units/concentration.h"
#include "units/conductance.h"
#include "units/constants.h"
#include "units/current.h"
#include "units/data.h"
#include "units/data_transfer_rate.h"
#include "units/density.h"
#include "units/dimensionless.h"
#include "units/energy.h"
#include "units/force.h"
#include "units/frequency.h"
#include "units/illuminance.h"
#include "units/impedance.h"
#include "units/inductance.h"
#include "units/length.h"
#include "units/luminous_flux.h"
#include "units/luminous_intensity.h"
#include "units/magnetic_field_strength.h"
#include "units/magnetic_flux.h"
#include "units/mass.h"
#include "units/math.h"
#include "units/power.h"
#include "units/pressure.h"
#include "units/radiation.h"
#include "units/solid_angle.h"
#include "units/substance.h"
#include "units/temperature.h"
#include "units/time.h"
#include "units/torque.h"
#include "units/velocity.h"
#include "units/voltage.h"
#include "units/volume.h"

using namespace units::acceleration;
using namespace units::angle;
using namespace units::angular_acceleration;
using namespace units::angular_velocity;
using namespace units::area;
using namespace units::capacitance;
using namespace units::charge;
using namespace units::concentration;
using namespace units::conductance;
using namespace units::data;
using namespace units::data_transfer_rate;
using namespace units::density;
using namespace units::dimensionless;
using namespace units::energy;
using namespace units::frequency;
using namespace units::illuminance;
using namespace units::impedance;
using namespace units::inductance;
using namespace units::length;
using namespace units::luminous_flux;
using namespace units::luminous_intensity;
using namespace units::magnetic_field_strength;
using namespace units::magnetic_flux;
using namespace units::mass;
using namespace units::math;
using namespace units::power;
using namespace units::pressure;
using namespace units::radiation;
using namespace units::solid_angle;
using namespace units::temperature;
using namespace units::time;
using namespace units::torque;
using namespace units::velocity;
using namespace units::voltage;
using namespace units::volume;
using namespace units;

#if !defined(_MSC_VER) || _MSC_VER > 1800
using namespace units::literals;
#endif

namespace {

class TypeTraits : public ::testing::Test {
 protected:
  TypeTraits() {}
  virtual ~TypeTraits() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class UnitManipulators : public ::testing::Test {
 protected:
  UnitManipulators() {}
  virtual ~UnitManipulators() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class UnitContainer : public ::testing::Test {
 protected:
  UnitContainer() {}
  virtual ~UnitContainer() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class UnitConversion : public ::testing::Test {
 protected:
  UnitConversion() {}
  virtual ~UnitConversion() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class UnitMath : public ::testing::Test {
 protected:
  UnitMath() {}
  virtual ~UnitMath() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class CompileTimeArithmetic : public ::testing::Test {
 protected:
  CompileTimeArithmetic() {}
  virtual ~CompileTimeArithmetic() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class Constexpr : public ::testing::Test {
 protected:
  Constexpr() {}
  virtual ~Constexpr() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

class CaseStudies : public ::testing::Test {
 protected:
  CaseStudies() {}
  virtual ~CaseStudies() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  struct RightTriangle {
    using a = unit_value_t<meters, 3>;
    using b = unit_value_t<meters, 4>;
    using c = unit_value_sqrt<
        unit_value_add<unit_value_power<a, 2>, unit_value_power<b, 2>>>;
  };
};
}  // namespace

TEST_F(TypeTraits, isRatio) {
  EXPECT_TRUE(traits::is_ratio<std::ratio<1>>::value);
  EXPECT_FALSE(traits::is_ratio<double>::value);
}

TEST_F(TypeTraits, ratio_sqrt) {
  using rt2 = ratio_sqrt<std::ratio<2>>;
  EXPECT_LT(std::abs(std::sqrt(2 / static_cast<double>(1)) -
                     rt2::num / static_cast<double>(rt2::den)),
            5e-9);

  using rt4 = ratio_sqrt<std::ratio<4>>;
  EXPECT_LT(std::abs(std::sqrt(4 / static_cast<double>(1)) -
                     rt4::num / static_cast<double>(rt4::den)),
            5e-9);

  using rt10 = ratio_sqrt<std::ratio<10>>;
  EXPECT_LT(std::abs(std::sqrt(10 / static_cast<double>(1)) -
                     rt10::num / static_cast<double>(rt10::den)),
            5e-9);

  using rt30 = ratio_sqrt<std::ratio<30>>;
  EXPECT_LT(std::abs(std::sqrt(30 / static_cast<double>(1)) -
                     rt30::num / static_cast<double>(rt30::den)),
            5e-9);

  using rt61 = ratio_sqrt<std::ratio<61>>;
  EXPECT_LT(std::abs(std::sqrt(61 / static_cast<double>(1)) -
                     rt61::num / static_cast<double>(rt61::den)),
            5e-9);

  using rt100 = ratio_sqrt<std::ratio<100>>;
  EXPECT_LT(std::abs(std::sqrt(100 / static_cast<double>(1)) -
                     rt100::num / static_cast<double>(rt100::den)),
            5e-9);

  using rt1000 = ratio_sqrt<std::ratio<1000>>;
  EXPECT_LT(std::abs(std::sqrt(1000 / static_cast<double>(1)) -
                     rt1000::num / static_cast<double>(rt1000::den)),
            5e-9);

  using rt10000 = ratio_sqrt<std::ratio<10000>>;
  EXPECT_LT(std::abs(std::sqrt(10000 / static_cast<double>(1)) -
                     rt10000::num / static_cast<double>(rt10000::den)),
            5e-9);
}

TEST_F(TypeTraits, is_unit) {
  EXPECT_FALSE(traits::is_unit<std::ratio<1>>::value);
  EXPECT_FALSE(traits::is_unit<double>::value);
  EXPECT_TRUE(traits::is_unit<meters>::value);
  EXPECT_TRUE(traits::is_unit<feet>::value);
  EXPECT_TRUE(traits::is_unit<degrees_squared>::value);
  EXPECT_FALSE(traits::is_unit<meter_t>::value);
}

TEST_F(TypeTraits, is_unit_t) {
  EXPECT_FALSE(traits::is_unit_t<std::ratio<1>>::value);
  EXPECT_FALSE(traits::is_unit_t<double>::value);
  EXPECT_FALSE(traits::is_unit_t<meters>::value);
  EXPECT_FALSE(traits::is_unit_t<feet>::value);
  EXPECT_FALSE(traits::is_unit_t<degrees_squared>::value);
  EXPECT_TRUE(traits::is_unit_t<meter_t>::value);
}

TEST_F(TypeTraits, unit_traits) {
  EXPECT_TRUE(
      (std::is_same<void,
                    traits::unit_traits<double>::conversion_ratio>::value));
  EXPECT_FALSE(
      (std::is_same<void,
                    traits::unit_traits<meters>::conversion_ratio>::value));
}

TEST_F(TypeTraits, unit_t_traits) {
  EXPECT_TRUE(
      (std::is_same<void,
                    traits::unit_t_traits<double>::underlying_type>::value));
  EXPECT_TRUE(
      (std::is_same<UNIT_LIB_DEFAULT_TYPE,
                    traits::unit_t_traits<meter_t>::underlying_type>::value));
  EXPECT_TRUE(
      (std::is_same<void, traits::unit_t_traits<double>::value_type>::value));
  EXPECT_TRUE(
      (std::is_same<UNIT_LIB_DEFAULT_TYPE,
                    traits::unit_t_traits<meter_t>::value_type>::value));
}

TEST_F(TypeTraits, all_true) {
  EXPECT_TRUE(all_true<true>::type::value);
  EXPECT_TRUE((all_true<true, true>::type::value));
  EXPECT_TRUE((all_true<true, true, true>::type::value));
  EXPECT_FALSE(all_true<false>::type::value);
  EXPECT_FALSE((all_true<true, false>::type::value));
  EXPECT_FALSE((all_true<true, true, false>::type::value));
  EXPECT_FALSE((all_true<false, false, false>::type::value));
}

TEST_F(TypeTraits, is_convertible_unit) {
  EXPECT_TRUE((traits::is_convertible_unit<meters, meters>::value));
  EXPECT_TRUE((traits::is_convertible_unit<meters, astronicalUnits>::value));
  EXPECT_TRUE((traits::is_convertible_unit<meters, parsecs>::value));

  EXPECT_TRUE((traits::is_convertible_unit<meters, meters>::value));
  EXPECT_TRUE((traits::is_convertible_unit<astronicalUnits, meters>::value));
  EXPECT_TRUE((traits::is_convertible_unit<parsecs, meters>::value));
  EXPECT_TRUE((traits::is_convertible_unit<years, weeks>::value));

  EXPECT_FALSE((traits::is_convertible_unit<meters, seconds>::value));
  EXPECT_FALSE((traits::is_convertible_unit<seconds, meters>::value));
  EXPECT_FALSE((traits::is_convertible_unit<years, meters>::value));
}

TEST_F(TypeTraits, inverse) {
  double test;

  using htz = inverse<seconds>;
  bool shouldBeTrue = std::is_same<htz, hertz>::value;
  EXPECT_TRUE(shouldBeTrue);

  test = convert<inverse<celsius>, inverse<fahrenheit>>(1.0);
  EXPECT_NEAR(5.0 / 9.0, test, 5.0e-5);

  test = convert<inverse<kelvin>, inverse<fahrenheit>>(6.0);
  EXPECT_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_F(TypeTraits, base_unit_of) {
  using base = traits::base_unit_of<years>;
  bool shouldBeTrue = std::is_same<base, category::time_unit>::value;

  EXPECT_TRUE(shouldBeTrue);
}

TEST_F(TypeTraits, has_linear_scale) {
  EXPECT_TRUE((traits::has_linear_scale<scalar_t>::value));
  EXPECT_TRUE((traits::has_linear_scale<meter_t>::value));
  EXPECT_TRUE((traits::has_linear_scale<foot_t>::value));
  EXPECT_TRUE((traits::has_linear_scale<watt_t, scalar_t>::value));
  EXPECT_TRUE((traits::has_linear_scale<scalar_t, meter_t>::value));
  EXPECT_TRUE((traits::has_linear_scale<meters_per_second_t>::value));
  EXPECT_FALSE((traits::has_linear_scale<dB_t>::value));
  EXPECT_FALSE((traits::has_linear_scale<dB_t, meters_per_second_t>::value));
}

TEST_F(TypeTraits, has_decibel_scale) {
  EXPECT_FALSE((traits::has_decibel_scale<scalar_t>::value));
  EXPECT_FALSE((traits::has_decibel_scale<meter_t>::value));
  EXPECT_FALSE((traits::has_decibel_scale<foot_t>::value));
  EXPECT_TRUE((traits::has_decibel_scale<dB_t>::value));
  EXPECT_TRUE((traits::has_decibel_scale<dBW_t>::value));

  EXPECT_TRUE((traits::has_decibel_scale<dBW_t, dB_t>::value));
  EXPECT_TRUE((traits::has_decibel_scale<dBW_t, dBm_t>::value));
  EXPECT_TRUE((traits::has_decibel_scale<dB_t, dB_t>::value));
  EXPECT_TRUE((traits::has_decibel_scale<dB_t, dB_t, dB_t>::value));
  EXPECT_FALSE((traits::has_decibel_scale<dB_t, dB_t, meter_t>::value));
  EXPECT_FALSE((traits::has_decibel_scale<meter_t, dB_t>::value));
}

TEST_F(TypeTraits, is_same_scale) {
  EXPECT_TRUE((traits::is_same_scale<scalar_t, dimensionless_t>::value));
  EXPECT_TRUE((traits::is_same_scale<dB_t, dBW_t>::value));
  EXPECT_FALSE((traits::is_same_scale<dB_t, scalar_t>::value));
}

TEST_F(TypeTraits, is_dimensionless_unit) {
  EXPECT_TRUE((traits::is_dimensionless_unit<scalar_t>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<const scalar_t>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<const scalar_t&>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<dimensionless_t>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<dB_t>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<dB_t, scalar_t>::value));
  EXPECT_TRUE((traits::is_dimensionless_unit<ppm_t>::value));
  EXPECT_FALSE((traits::is_dimensionless_unit<meter_t>::value));
  EXPECT_FALSE((traits::is_dimensionless_unit<dBW_t>::value));
  EXPECT_FALSE((traits::is_dimensionless_unit<dBW_t, scalar_t>::value));
}

TEST_F(TypeTraits, is_length_unit) {
  EXPECT_TRUE((traits::is_length_unit<meter>::value));
  EXPECT_TRUE((traits::is_length_unit<cubit>::value));
  EXPECT_FALSE((traits::is_length_unit<year>::value));
  EXPECT_FALSE((traits::is_length_unit<double>::value));

  EXPECT_TRUE((traits::is_length_unit<meter_t>::value));
  EXPECT_TRUE((traits::is_length_unit<const meter_t>::value));
  EXPECT_TRUE((traits::is_length_unit<const meter_t&>::value));
  EXPECT_TRUE((traits::is_length_unit<cubit_t>::value));
  EXPECT_FALSE((traits::is_length_unit<year_t>::value));
  EXPECT_TRUE((traits::is_length_unit<meter_t, cubit_t>::value));
  EXPECT_FALSE((traits::is_length_unit<meter_t, year_t>::value));
}

TEST_F(TypeTraits, is_mass_unit) {
  EXPECT_TRUE((traits::is_mass_unit<kilogram>::value));
  EXPECT_TRUE((traits::is_mass_unit<stone>::value));
  EXPECT_FALSE((traits::is_mass_unit<meter>::value));
  EXPECT_FALSE((traits::is_mass_unit<double>::value));

  EXPECT_TRUE((traits::is_mass_unit<kilogram_t>::value));
  EXPECT_TRUE((traits::is_mass_unit<const kilogram_t>::value));
  EXPECT_TRUE((traits::is_mass_unit<const kilogram_t&>::value));
  EXPECT_TRUE((traits::is_mass_unit<stone_t>::value));
  EXPECT_FALSE((traits::is_mass_unit<meter_t>::value));
  EXPECT_TRUE((traits::is_mass_unit<kilogram_t, stone_t>::value));
  EXPECT_FALSE((traits::is_mass_unit<kilogram_t, meter_t>::value));
}

TEST_F(TypeTraits, is_time_unit) {
  EXPECT_TRUE((traits::is_time_unit<second>::value));
  EXPECT_TRUE((traits::is_time_unit<year>::value));
  EXPECT_FALSE((traits::is_time_unit<meter>::value));
  EXPECT_FALSE((traits::is_time_unit<double>::value));

  EXPECT_TRUE((traits::is_time_unit<second_t>::value));
  EXPECT_TRUE((traits::is_time_unit<const second_t>::value));
  EXPECT_TRUE((traits::is_time_unit<const second_t&>::value));
  EXPECT_TRUE((traits::is_time_unit<year_t>::value));
  EXPECT_FALSE((traits::is_time_unit<meter_t>::value));
  EXPECT_TRUE((traits::is_time_unit<second_t, year_t>::value));
  EXPECT_FALSE((traits::is_time_unit<second_t, meter_t>::value));
}

TEST_F(TypeTraits, is_angle_unit) {
  EXPECT_TRUE((traits::is_angle_unit<angle::radian>::value));
  EXPECT_TRUE((traits::is_angle_unit<angle::degree>::value));
  EXPECT_FALSE((traits::is_angle_unit<watt>::value));
  EXPECT_FALSE((traits::is_angle_unit<double>::value));

  EXPECT_TRUE((traits::is_angle_unit<angle::radian_t>::value));
  EXPECT_TRUE((traits::is_angle_unit<const angle::radian_t>::value));
  EXPECT_TRUE((traits::is_angle_unit<const angle::radian_t&>::value));
  EXPECT_TRUE((traits::is_angle_unit<angle::degree_t>::value));
  EXPECT_FALSE((traits::is_angle_unit<watt_t>::value));
  EXPECT_TRUE((traits::is_angle_unit<angle::radian_t, angle::degree_t>::value));
  EXPECT_FALSE((traits::is_angle_unit<angle::radian_t, watt_t>::value));
}

TEST_F(TypeTraits, is_current_unit) {
  EXPECT_TRUE((traits::is_current_unit<current::ampere>::value));
  EXPECT_FALSE((traits::is_current_unit<volt>::value));
  EXPECT_FALSE((traits::is_current_unit<double>::value));

  EXPECT_TRUE((traits::is_current_unit<current::ampere_t>::value));
  EXPECT_TRUE((traits::is_current_unit<const current::ampere_t>::value));
  EXPECT_TRUE((traits::is_current_unit<const current::ampere_t&>::value));
  EXPECT_FALSE((traits::is_current_unit<volt_t>::value));
  EXPECT_TRUE((traits::is_current_unit<current::ampere_t,
                                       current::milliampere_t>::value));
  EXPECT_FALSE((traits::is_current_unit<current::ampere_t, volt_t>::value));
}

TEST_F(TypeTraits, is_temperature_unit) {
  EXPECT_TRUE((traits::is_temperature_unit<fahrenheit>::value));
  EXPECT_TRUE((traits::is_temperature_unit<kelvin>::value));
  EXPECT_FALSE((traits::is_temperature_unit<cubit>::value));
  EXPECT_FALSE((traits::is_temperature_unit<double>::value));

  EXPECT_TRUE((traits::is_temperature_unit<fahrenheit_t>::value));
  EXPECT_TRUE((traits::is_temperature_unit<const fahrenheit_t>::value));
  EXPECT_TRUE((traits::is_temperature_unit<const fahrenheit_t&>::value));
  EXPECT_TRUE((traits::is_temperature_unit<kelvin_t>::value));
  EXPECT_FALSE((traits::is_temperature_unit<cubit_t>::value));
  EXPECT_TRUE((traits::is_temperature_unit<fahrenheit_t, kelvin_t>::value));
  EXPECT_FALSE((traits::is_temperature_unit<cubit_t, fahrenheit_t>::value));
}

TEST_F(TypeTraits, is_substance_unit) {
  EXPECT_TRUE((traits::is_substance_unit<substance::mol>::value));
  EXPECT_FALSE((traits::is_substance_unit<year>::value));
  EXPECT_FALSE((traits::is_substance_unit<double>::value));

  EXPECT_TRUE((traits::is_substance_unit<substance::mole_t>::value));
  EXPECT_TRUE((traits::is_substance_unit<const substance::mole_t>::value));
  EXPECT_TRUE((traits::is_substance_unit<const substance::mole_t&>::value));
  EXPECT_FALSE((traits::is_substance_unit<year_t>::value));
  EXPECT_TRUE(
      (traits::is_substance_unit<substance::mole_t, substance::mole_t>::value));
  EXPECT_FALSE((traits::is_substance_unit<year_t, substance::mole_t>::value));
}

TEST_F(TypeTraits, is_luminous_intensity_unit) {
  EXPECT_TRUE((traits::is_luminous_intensity_unit<candela>::value));
  EXPECT_FALSE(
      (traits::is_luminous_intensity_unit<units::radiation::rad>::value));
  EXPECT_FALSE((traits::is_luminous_intensity_unit<double>::value));

  EXPECT_TRUE((traits::is_luminous_intensity_unit<candela_t>::value));
  EXPECT_TRUE((traits::is_luminous_intensity_unit<const candela_t>::value));
  EXPECT_TRUE((traits::is_luminous_intensity_unit<const candela_t&>::value));
  EXPECT_FALSE((traits::is_luminous_intensity_unit<rad_t>::value));
  EXPECT_TRUE(
      (traits::is_luminous_intensity_unit<candela_t, candela_t>::value));
  EXPECT_FALSE((traits::is_luminous_intensity_unit<rad_t, candela_t>::value));
}

TEST_F(TypeTraits, is_solid_angle_unit) {
  EXPECT_TRUE((traits::is_solid_angle_unit<steradian>::value));
  EXPECT_TRUE((traits::is_solid_angle_unit<degree_squared>::value));
  EXPECT_FALSE((traits::is_solid_angle_unit<angle::degree>::value));
  EXPECT_FALSE((traits::is_solid_angle_unit<double>::value));

  EXPECT_TRUE((traits::is_solid_angle_unit<steradian_t>::value));
  EXPECT_TRUE((traits::is_solid_angle_unit<const steradian_t>::value));
  EXPECT_TRUE((traits::is_solid_angle_unit<const degree_squared_t&>::value));
  EXPECT_FALSE((traits::is_solid_angle_unit<angle::degree_t>::value));
  EXPECT_TRUE(
      (traits::is_solid_angle_unit<degree_squared_t, steradian_t>::value));
  EXPECT_FALSE(
      (traits::is_solid_angle_unit<angle::degree_t, steradian_t>::value));
}

TEST_F(TypeTraits, is_frequency_unit) {
  EXPECT_TRUE((traits::is_frequency_unit<hertz>::value));
  EXPECT_FALSE((traits::is_frequency_unit<second>::value));
  EXPECT_FALSE((traits::is_frequency_unit<double>::value));

  EXPECT_TRUE((traits::is_frequency_unit<hertz_t>::value));
  EXPECT_TRUE((traits::is_frequency_unit<const hertz_t>::value));
  EXPECT_TRUE((traits::is_frequency_unit<const hertz_t&>::value));
  EXPECT_FALSE((traits::is_frequency_unit<second_t>::value));
  EXPECT_TRUE((traits::is_frequency_unit<const hertz_t&, gigahertz_t>::value));
  EXPECT_FALSE((traits::is_frequency_unit<second_t, hertz_t>::value));
}

TEST_F(TypeTraits, is_velocity_unit) {
  EXPECT_TRUE((traits::is_velocity_unit<meters_per_second>::value));
  EXPECT_TRUE((traits::is_velocity_unit<miles_per_hour>::value));
  EXPECT_FALSE((traits::is_velocity_unit<meters_per_second_squared>::value));
  EXPECT_FALSE((traits::is_velocity_unit<double>::value));

  EXPECT_TRUE((traits::is_velocity_unit<meters_per_second_t>::value));
  EXPECT_TRUE((traits::is_velocity_unit<const meters_per_second_t>::value));
  EXPECT_TRUE((traits::is_velocity_unit<const meters_per_second_t&>::value));
  EXPECT_TRUE((traits::is_velocity_unit<miles_per_hour_t>::value));
  EXPECT_FALSE((traits::is_velocity_unit<meters_per_second_squared_t>::value));
  EXPECT_TRUE(
      (traits::is_velocity_unit<miles_per_hour_t, meters_per_second_t>::value));
  EXPECT_FALSE((traits::is_velocity_unit<meters_per_second_squared_t,
                                         meters_per_second_t>::value));
}

TEST_F(TypeTraits, is_acceleration_unit) {
  EXPECT_TRUE((traits::is_acceleration_unit<meters_per_second_squared>::value));
  EXPECT_TRUE(
      (traits::is_acceleration_unit<acceleration::standard_gravity>::value));
  EXPECT_FALSE((traits::is_acceleration_unit<inch>::value));
  EXPECT_FALSE((traits::is_acceleration_unit<double>::value));

  EXPECT_TRUE(
      (traits::is_acceleration_unit<meters_per_second_squared_t>::value));
  EXPECT_TRUE(
      (traits::is_acceleration_unit<const meters_per_second_squared_t>::value));
  EXPECT_TRUE((
      traits::is_acceleration_unit<const meters_per_second_squared_t&>::value));
  EXPECT_TRUE((traits::is_acceleration_unit<standard_gravity_t>::value));
  EXPECT_FALSE((traits::is_acceleration_unit<inch_t>::value));
  EXPECT_TRUE(
      (traits::is_acceleration_unit<standard_gravity_t,
                                    meters_per_second_squared_t>::value));
  EXPECT_FALSE(
      (traits::is_acceleration_unit<inch_t,
                                    meters_per_second_squared_t>::value));
}

TEST_F(TypeTraits, is_force_unit) {
  EXPECT_TRUE((traits::is_force_unit<units::force::newton>::value));
  EXPECT_TRUE((traits::is_force_unit<units::force::dynes>::value));
  EXPECT_FALSE((traits::is_force_unit<meter>::value));
  EXPECT_FALSE((traits::is_force_unit<double>::value));

  EXPECT_TRUE((traits::is_force_unit<units::force::newton_t>::value));
  EXPECT_TRUE((traits::is_force_unit<const units::force::newton_t>::value));
  EXPECT_TRUE((traits::is_force_unit<const units::force::newton_t&>::value));
  EXPECT_TRUE((traits::is_force_unit<units::force::dyne_t>::value));
  EXPECT_FALSE((traits::is_force_unit<watt_t>::value));
  EXPECT_TRUE((traits::is_force_unit<units::force::dyne_t,
                                     units::force::newton_t>::value));
  EXPECT_FALSE((traits::is_force_unit<watt_t, units::force::newton_t>::value));
}

TEST_F(TypeTraits, is_pressure_unit) {
  EXPECT_TRUE((traits::is_pressure_unit<pressure::pascals>::value));
  EXPECT_TRUE((traits::is_pressure_unit<atmosphere>::value));
  EXPECT_FALSE((traits::is_pressure_unit<year>::value));
  EXPECT_FALSE((traits::is_pressure_unit<double>::value));

  EXPECT_TRUE((traits::is_pressure_unit<pascal_t>::value));
  EXPECT_TRUE((traits::is_pressure_unit<const pascal_t>::value));
  EXPECT_TRUE((traits::is_pressure_unit<const pascal_t&>::value));
  EXPECT_TRUE((traits::is_pressure_unit<atmosphere_t>::value));
  EXPECT_FALSE((traits::is_pressure_unit<year_t>::value));
  EXPECT_TRUE(
      (traits::is_pressure_unit<atmosphere_t, pressure::pascal_t>::value));
  EXPECT_FALSE((traits::is_pressure_unit<year_t, pressure::pascal_t>::value));
}

TEST_F(TypeTraits, is_charge_unit) {
  EXPECT_TRUE((traits::is_charge_unit<coulomb>::value));
  EXPECT_FALSE((traits::is_charge_unit<watt>::value));
  EXPECT_FALSE((traits::is_charge_unit<double>::value));

  EXPECT_TRUE((traits::is_charge_unit<coulomb_t>::value));
  EXPECT_TRUE((traits::is_charge_unit<const coulomb_t>::value));
  EXPECT_TRUE((traits::is_charge_unit<const coulomb_t&>::value));
  EXPECT_FALSE((traits::is_charge_unit<watt_t>::value));
  EXPECT_TRUE((traits::is_charge_unit<const coulomb_t&, coulomb_t>::value));
  EXPECT_FALSE((traits::is_charge_unit<watt_t, coulomb_t>::value));
}

TEST_F(TypeTraits, is_energy_unit) {
  EXPECT_TRUE((traits::is_energy_unit<joule>::value));
  EXPECT_TRUE((traits::is_energy_unit<calorie>::value));
  EXPECT_FALSE((traits::is_energy_unit<watt>::value));
  EXPECT_FALSE((traits::is_energy_unit<double>::value));

  EXPECT_TRUE((traits::is_energy_unit<joule_t>::value));
  EXPECT_TRUE((traits::is_energy_unit<const joule_t>::value));
  EXPECT_TRUE((traits::is_energy_unit<const joule_t&>::value));
  EXPECT_TRUE((traits::is_energy_unit<calorie_t>::value));
  EXPECT_FALSE((traits::is_energy_unit<watt_t>::value));
  EXPECT_TRUE((traits::is_energy_unit<calorie_t, joule_t>::value));
  EXPECT_FALSE((traits::is_energy_unit<watt_t, joule_t>::value));
}

TEST_F(TypeTraits, is_power_unit) {
  EXPECT_TRUE((traits::is_power_unit<watt>::value));
  EXPECT_FALSE((traits::is_power_unit<henry>::value));
  EXPECT_FALSE((traits::is_power_unit<double>::value));

  EXPECT_TRUE((traits::is_power_unit<watt_t>::value));
  EXPECT_TRUE((traits::is_power_unit<const watt_t>::value));
  EXPECT_TRUE((traits::is_power_unit<const watt_t&>::value));
  EXPECT_FALSE((traits::is_power_unit<henry_t>::value));
  EXPECT_TRUE((traits::is_power_unit<const watt_t&, watt_t>::value));
  EXPECT_FALSE((traits::is_power_unit<henry_t, watt_t>::value));
}

TEST_F(TypeTraits, is_voltage_unit) {
  EXPECT_TRUE((traits::is_voltage_unit<volt>::value));
  EXPECT_FALSE((traits::is_voltage_unit<henry>::value));
  EXPECT_FALSE((traits::is_voltage_unit<double>::value));

  EXPECT_TRUE((traits::is_voltage_unit<volt_t>::value));
  EXPECT_TRUE((traits::is_voltage_unit<const volt_t>::value));
  EXPECT_TRUE((traits::is_voltage_unit<const volt_t&>::value));
  EXPECT_FALSE((traits::is_voltage_unit<henry_t>::value));
  EXPECT_TRUE((traits::is_voltage_unit<const volt_t&, volt_t>::value));
  EXPECT_FALSE((traits::is_voltage_unit<henry_t, volt_t>::value));
}

TEST_F(TypeTraits, is_capacitance_unit) {
  EXPECT_TRUE((traits::is_capacitance_unit<farad>::value));
  EXPECT_FALSE((traits::is_capacitance_unit<ohm>::value));
  EXPECT_FALSE((traits::is_capacitance_unit<double>::value));

  EXPECT_TRUE((traits::is_capacitance_unit<farad_t>::value));
  EXPECT_TRUE((traits::is_capacitance_unit<const farad_t>::value));
  EXPECT_TRUE((traits::is_capacitance_unit<const farad_t&>::value));
  EXPECT_FALSE((traits::is_capacitance_unit<ohm_t>::value));
  EXPECT_TRUE(
      (traits::is_capacitance_unit<const farad_t&, millifarad_t>::value));
  EXPECT_FALSE((traits::is_capacitance_unit<ohm_t, farad_t>::value));
}

TEST_F(TypeTraits, is_impedance_unit) {
  EXPECT_TRUE((traits::is_impedance_unit<ohm>::value));
  EXPECT_FALSE((traits::is_impedance_unit<farad>::value));
  EXPECT_FALSE((traits::is_impedance_unit<double>::value));

  EXPECT_TRUE((traits::is_impedance_unit<ohm_t>::value));
  EXPECT_TRUE((traits::is_impedance_unit<const ohm_t>::value));
  EXPECT_TRUE((traits::is_impedance_unit<const ohm_t&>::value));
  EXPECT_FALSE((traits::is_impedance_unit<farad_t>::value));
  EXPECT_TRUE((traits::is_impedance_unit<const ohm_t&, milliohm_t>::value));
  EXPECT_FALSE((traits::is_impedance_unit<farad_t, ohm_t>::value));
}

TEST_F(TypeTraits, is_conductance_unit) {
  EXPECT_TRUE((traits::is_conductance_unit<siemens>::value));
  EXPECT_FALSE((traits::is_conductance_unit<volt>::value));
  EXPECT_FALSE((traits::is_conductance_unit<double>::value));

  EXPECT_TRUE((traits::is_conductance_unit<siemens_t>::value));
  EXPECT_TRUE((traits::is_conductance_unit<const siemens_t>::value));
  EXPECT_TRUE((traits::is_conductance_unit<const siemens_t&>::value));
  EXPECT_FALSE((traits::is_conductance_unit<volt_t>::value));
  EXPECT_TRUE(
      (traits::is_conductance_unit<const siemens_t&, millisiemens_t>::value));
  EXPECT_FALSE((traits::is_conductance_unit<volt_t, siemens_t>::value));
}

TEST_F(TypeTraits, is_magnetic_flux_unit) {
  EXPECT_TRUE((traits::is_magnetic_flux_unit<weber>::value));
  EXPECT_TRUE((traits::is_magnetic_flux_unit<maxwell>::value));
  EXPECT_FALSE((traits::is_magnetic_flux_unit<inch>::value));
  EXPECT_FALSE((traits::is_magnetic_flux_unit<double>::value));

  EXPECT_TRUE((traits::is_magnetic_flux_unit<weber_t>::value));
  EXPECT_TRUE((traits::is_magnetic_flux_unit<const weber_t>::value));
  EXPECT_TRUE((traits::is_magnetic_flux_unit<const weber_t&>::value));
  EXPECT_TRUE((traits::is_magnetic_flux_unit<maxwell_t>::value));
  EXPECT_FALSE((traits::is_magnetic_flux_unit<inch_t>::value));
  EXPECT_TRUE((traits::is_magnetic_flux_unit<maxwell_t, weber_t>::value));
  EXPECT_FALSE((traits::is_magnetic_flux_unit<inch_t, weber_t>::value));
}

TEST_F(TypeTraits, is_magnetic_field_strength_unit) {
  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<
               units::magnetic_field_strength::tesla>::value));
  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<gauss>::value));
  EXPECT_FALSE((traits::is_magnetic_field_strength_unit<volt>::value));
  EXPECT_FALSE((traits::is_magnetic_field_strength_unit<double>::value));

  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<tesla_t>::value));
  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<const tesla_t>::value));
  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<const tesla_t&>::value));
  EXPECT_TRUE((traits::is_magnetic_field_strength_unit<gauss_t>::value));
  EXPECT_FALSE((traits::is_magnetic_field_strength_unit<volt_t>::value));
  EXPECT_TRUE(
      (traits::is_magnetic_field_strength_unit<gauss_t, tesla_t>::value));
  EXPECT_FALSE(
      (traits::is_magnetic_field_strength_unit<volt_t, tesla_t>::value));
}

TEST_F(TypeTraits, is_inductance_unit) {
  EXPECT_TRUE((traits::is_inductance_unit<henry>::value));
  EXPECT_FALSE((traits::is_inductance_unit<farad>::value));
  EXPECT_FALSE((traits::is_inductance_unit<double>::value));

  EXPECT_TRUE((traits::is_inductance_unit<henry_t>::value));
  EXPECT_TRUE((traits::is_inductance_unit<const henry_t>::value));
  EXPECT_TRUE((traits::is_inductance_unit<const henry_t&>::value));
  EXPECT_FALSE((traits::is_inductance_unit<farad_t>::value));
  EXPECT_TRUE(
      (traits::is_inductance_unit<const henry_t&, millihenry_t>::value));
  EXPECT_FALSE((traits::is_inductance_unit<farad_t, henry_t>::value));
}

TEST_F(TypeTraits, is_luminous_flux_unit) {
  EXPECT_TRUE((traits::is_luminous_flux_unit<lumen>::value));
  EXPECT_FALSE((traits::is_luminous_flux_unit<pound>::value));
  EXPECT_FALSE((traits::is_luminous_flux_unit<double>::value));

  EXPECT_TRUE((traits::is_luminous_flux_unit<lumen_t>::value));
  EXPECT_TRUE((traits::is_luminous_flux_unit<const lumen_t>::value));
  EXPECT_TRUE((traits::is_luminous_flux_unit<const lumen_t&>::value));
  EXPECT_FALSE((traits::is_luminous_flux_unit<pound_t>::value));
  EXPECT_TRUE(
      (traits::is_luminous_flux_unit<const lumen_t&, millilumen_t>::value));
  EXPECT_FALSE((traits::is_luminous_flux_unit<pound_t, lumen_t>::value));
}

TEST_F(TypeTraits, is_illuminance_unit) {
  EXPECT_TRUE((traits::is_illuminance_unit<illuminance::footcandle>::value));
  EXPECT_TRUE((traits::is_illuminance_unit<illuminance::lux>::value));
  EXPECT_FALSE((traits::is_illuminance_unit<meter>::value));
  EXPECT_FALSE((traits::is_illuminance_unit<double>::value));

  EXPECT_TRUE((traits::is_illuminance_unit<footcandle_t>::value));
  EXPECT_TRUE((traits::is_illuminance_unit<const footcandle_t>::value));
  EXPECT_TRUE((traits::is_illuminance_unit<const footcandle_t&>::value));
  EXPECT_TRUE((traits::is_illuminance_unit<lux_t>::value));
  EXPECT_FALSE((traits::is_illuminance_unit<meter_t>::value));
  EXPECT_TRUE((traits::is_illuminance_unit<lux_t, footcandle_t>::value));
  EXPECT_FALSE((traits::is_illuminance_unit<meter_t, footcandle_t>::value));
}

TEST_F(TypeTraits, is_radioactivity_unit) {
  EXPECT_TRUE((traits::is_radioactivity_unit<becquerel>::value));
  EXPECT_FALSE((traits::is_radioactivity_unit<year>::value));
  EXPECT_FALSE((traits::is_radioactivity_unit<double>::value));

  EXPECT_TRUE((traits::is_radioactivity_unit<becquerel_t>::value));
  EXPECT_TRUE((traits::is_radioactivity_unit<const becquerel_t>::value));
  EXPECT_TRUE((traits::is_radioactivity_unit<const becquerel_t&>::value));
  EXPECT_FALSE((traits::is_radioactivity_unit<year_t>::value));
  EXPECT_TRUE((traits::is_radioactivity_unit<const becquerel_t&,
                                             millibecquerel_t>::value));
  EXPECT_FALSE((traits::is_radioactivity_unit<year_t, becquerel_t>::value));
}

TEST_F(TypeTraits, is_torque_unit) {
  EXPECT_TRUE((traits::is_torque_unit<torque::newton_meter>::value));
  EXPECT_TRUE((traits::is_torque_unit<torque::foot_pound>::value));
  EXPECT_FALSE((traits::is_torque_unit<volume::cubic_meter>::value));
  EXPECT_FALSE((traits::is_torque_unit<double>::value));

  EXPECT_TRUE((traits::is_torque_unit<torque::newton_meter_t>::value));
  EXPECT_TRUE((traits::is_torque_unit<const torque::newton_meter_t>::value));
  EXPECT_TRUE((traits::is_torque_unit<const torque::newton_meter_t&>::value));
  EXPECT_TRUE((traits::is_torque_unit<torque::foot_pound_t>::value));
  EXPECT_FALSE((traits::is_torque_unit<volume::cubic_meter_t>::value));
  EXPECT_TRUE((traits::is_torque_unit<torque::foot_pound_t,
                                      torque::newton_meter_t>::value));
  EXPECT_FALSE((traits::is_torque_unit<volume::cubic_meter_t,
                                       torque::newton_meter_t>::value));
}

TEST_F(TypeTraits, is_area_unit) {
  EXPECT_TRUE((traits::is_area_unit<square_meter>::value));
  EXPECT_TRUE((traits::is_area_unit<hectare>::value));
  EXPECT_FALSE((traits::is_area_unit<astronicalUnit>::value));
  EXPECT_FALSE((traits::is_area_unit<double>::value));

  EXPECT_TRUE((traits::is_area_unit<square_meter_t>::value));
  EXPECT_TRUE((traits::is_area_unit<const square_meter_t>::value));
  EXPECT_TRUE((traits::is_area_unit<const square_meter_t&>::value));
  EXPECT_TRUE((traits::is_area_unit<hectare_t>::value));
  EXPECT_FALSE((traits::is_area_unit<astronicalUnit_t>::value));
  EXPECT_TRUE((traits::is_area_unit<hectare_t, square_meter_t>::value));
  EXPECT_FALSE((traits::is_area_unit<astronicalUnit_t, square_meter_t>::value));
}

TEST_F(TypeTraits, is_volume_unit) {
  EXPECT_TRUE((traits::is_volume_unit<cubic_meter>::value));
  EXPECT_TRUE((traits::is_volume_unit<cubic_foot>::value));
  EXPECT_FALSE((traits::is_volume_unit<square_feet>::value));
  EXPECT_FALSE((traits::is_volume_unit<double>::value));

  EXPECT_TRUE((traits::is_volume_unit<cubic_meter_t>::value));
  EXPECT_TRUE((traits::is_volume_unit<const cubic_meter_t>::value));
  EXPECT_TRUE((traits::is_volume_unit<const cubic_meter_t&>::value));
  EXPECT_TRUE((traits::is_volume_unit<cubic_inch_t>::value));
  EXPECT_FALSE((traits::is_volume_unit<foot_t>::value));
  EXPECT_TRUE((traits::is_volume_unit<cubic_inch_t, cubic_meter_t>::value));
  EXPECT_FALSE((traits::is_volume_unit<foot_t, cubic_meter_t>::value));
}

TEST_F(TypeTraits, is_density_unit) {
  EXPECT_TRUE((traits::is_density_unit<kilograms_per_cubic_meter>::value));
  EXPECT_TRUE((traits::is_density_unit<ounces_per_cubic_foot>::value));
  EXPECT_FALSE((traits::is_density_unit<year>::value));
  EXPECT_FALSE((traits::is_density_unit<double>::value));

  EXPECT_TRUE((traits::is_density_unit<kilograms_per_cubic_meter_t>::value));
  EXPECT_TRUE(
      (traits::is_density_unit<const kilograms_per_cubic_meter_t>::value));
  EXPECT_TRUE(
      (traits::is_density_unit<const kilograms_per_cubic_meter_t&>::value));
  EXPECT_TRUE((traits::is_density_unit<ounces_per_cubic_foot_t>::value));
  EXPECT_FALSE((traits::is_density_unit<year_t>::value));
  EXPECT_TRUE((traits::is_density_unit<ounces_per_cubic_foot_t,
                                       kilograms_per_cubic_meter_t>::value));
  EXPECT_FALSE(
      (traits::is_density_unit<year_t, kilograms_per_cubic_meter_t>::value));
}

TEST_F(TypeTraits, is_data_unit) {
  EXPECT_TRUE((traits::is_data_unit<bit>::value));
  EXPECT_TRUE((traits::is_data_unit<byte>::value));
  EXPECT_TRUE((traits::is_data_unit<exabit>::value));
  EXPECT_TRUE((traits::is_data_unit<exabyte>::value));
  EXPECT_FALSE((traits::is_data_unit<year>::value));
  EXPECT_FALSE((traits::is_data_unit<double>::value));

  EXPECT_TRUE((traits::is_data_unit<bit_t>::value));
  EXPECT_TRUE((traits::is_data_unit<const bit_t>::value));
  EXPECT_TRUE((traits::is_data_unit<const bit_t&>::value));
  EXPECT_TRUE((traits::is_data_unit<byte_t>::value));
  EXPECT_FALSE((traits::is_data_unit<year_t>::value));
  EXPECT_TRUE((traits::is_data_unit<bit_t, byte_t>::value));
  EXPECT_FALSE((traits::is_data_unit<year_t, byte_t>::value));
}

TEST_F(TypeTraits, is_data_transfer_rate_unit) {
  EXPECT_TRUE((traits::is_data_transfer_rate_unit<Gbps>::value));
  EXPECT_TRUE((traits::is_data_transfer_rate_unit<GBps>::value));
  EXPECT_FALSE((traits::is_data_transfer_rate_unit<year>::value));
  EXPECT_FALSE((traits::is_data_transfer_rate_unit<double>::value));

  EXPECT_TRUE(
      (traits::is_data_transfer_rate_unit<gigabits_per_second_t>::value));
  EXPECT_TRUE((
      traits::is_data_transfer_rate_unit<const gigabytes_per_second_t>::value));
  EXPECT_TRUE((traits::is_data_transfer_rate_unit<
               const gigabytes_per_second_t&>::value));
  EXPECT_TRUE(
      (traits::is_data_transfer_rate_unit<gigabytes_per_second_t>::value));
  EXPECT_FALSE((traits::is_data_transfer_rate_unit<year_t>::value));
  EXPECT_TRUE(
      (traits::is_data_transfer_rate_unit<gigabits_per_second_t,
                                          gigabytes_per_second_t>::value));
  EXPECT_FALSE(
      (traits::is_data_transfer_rate_unit<year_t,
                                          gigabytes_per_second_t>::value));
}

TEST_F(UnitManipulators, squared) {
  double test;

  test = convert<squared<meters>, square_feet>(0.092903);
  EXPECT_NEAR(0.99999956944, test, 5.0e-12);

  using scalar_2 = squared<scalar>;  // this is actually nonsensical, and should
                                     // also result in a scalar.
  bool isSame =
      std::is_same<typename std::decay<scalar_t>::type,
                   typename std::decay<unit_t<scalar_2>>::type>::value;
  EXPECT_TRUE(isSame);
}

TEST_F(UnitManipulators, cubed) {
  double test;

  test = convert<cubed<meters>, cubic_feet>(0.0283168);
  EXPECT_NEAR(0.999998354619, test, 5.0e-13);
}

TEST_F(UnitManipulators, square_root) {
  double test;

  test = convert<square_root<square_kilometer>, meter>(1.0);
  EXPECT_TRUE((traits::is_convertible_unit<
               typename std::decay<square_root<square_kilometer>>::type,
               kilometer>::value));
  EXPECT_NEAR(1000.0, test, 5.0e-13);
}

TEST_F(UnitManipulators, compound_unit) {
  using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
  using acceleration2 =
      compound_unit<meters, inverse<seconds>, inverse<seconds>>;
  using acceleration3 =
      unit<std::ratio<1>,
           base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-2>>>;
  using acceleration4 = compound_unit<meters, inverse<squared<seconds>>>;
  using acceleration5 = compound_unit<meters, squared<inverse<seconds>>>;

  bool areSame12 = std::is_same<acceleration1, acceleration2>::value;
  bool areSame23 = std::is_same<acceleration2, acceleration3>::value;
  bool areSame34 = std::is_same<acceleration3, acceleration4>::value;
  bool areSame45 = std::is_same<acceleration4, acceleration5>::value;

  EXPECT_TRUE(areSame12);
  EXPECT_TRUE(areSame23);
  EXPECT_TRUE(areSame34);
  EXPECT_TRUE(areSame45);

  // test that thing with translations still compile
  using arbitrary1 = compound_unit<meters, inverse<celsius>>;
  using arbitrary2 = compound_unit<meters, celsius>;
  using arbitrary3 = compound_unit<arbitrary1, arbitrary2>;
  EXPECT_TRUE((std::is_same<square_meters, arbitrary3>::value));
}

TEST_F(UnitManipulators, dimensionalAnalysis) {
  // these look like 'compound units', but the dimensional analysis can be
  // REALLY handy if the unit types aren't know (i.e. they themselves are
  // template parameters), as you can get the resulting unit of the operation.

  using velocity = units::detail::unit_divide<meters, second>;
  bool shouldBeTrue = std::is_same<meters_per_second, velocity>::value;
  EXPECT_TRUE(shouldBeTrue);

  using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
  using acceleration2 = units::detail::unit_divide<
      meters, units::detail::unit_multiply<seconds, seconds>>;
  shouldBeTrue = std::is_same<acceleration1, acceleration2>::value;
  EXPECT_TRUE(shouldBeTrue);
}

#ifdef _MSC_VER
#if (_MSC_VER >= 1900)
TEST_F(UnitContainer, trivial) {
  EXPECT_TRUE((std::is_trivial<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_assignable<meter_t, meter_t>::value));
  EXPECT_TRUE((std::is_trivially_constructible<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_copy_assignable<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_copy_constructible<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_copyable<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_default_constructible<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_destructible<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_move_assignable<meter_t>::value));
  EXPECT_TRUE((std::is_trivially_move_constructible<meter_t>::value));

  EXPECT_TRUE((std::is_trivial<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_assignable<dB_t, dB_t>::value));
  EXPECT_TRUE((std::is_trivially_constructible<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_copy_assignable<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_copy_constructible<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_copyable<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_default_constructible<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_destructible<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_move_assignable<dB_t>::value));
  EXPECT_TRUE((std::is_trivially_move_constructible<dB_t>::value));
}
#endif
#endif

TEST_F(UnitContainer, has_value_member) {
  EXPECT_TRUE((traits::has_value_member<linear_scale<double>, double>::value));
  EXPECT_FALSE((traits::has_value_member<meter, double>::value));
}

TEST_F(UnitContainer, make_unit) {
  auto dist = units::make_unit<meter_t>(5);
  EXPECT_EQ(meter_t(5), dist);
}

TEST_F(UnitContainer, unitTypeAddition) {
  // units
  meter_t a_m(1.0), c_m;
  foot_t b_ft(3.28084);

  double d = convert<feet, meters>(b_ft());
  EXPECT_NEAR(1.0, d, 5.0e-5);

  c_m = a_m + b_ft;
  EXPECT_NEAR(2.0, c_m(), 5.0e-5);

  c_m = b_ft + meter_t(3);
  EXPECT_NEAR(4.0, c_m(), 5.0e-5);

  auto e_ft = b_ft + meter_t(3);
  EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);

  // scalar
  scalar_t sresult = scalar_t(1.0) + scalar_t(1.0);
  EXPECT_NEAR(2.0, sresult, 5.0e-6);

  sresult = scalar_t(1.0) + 1.0;
  EXPECT_NEAR(2.0, sresult, 5.0e-6);

  sresult = 1.0 + scalar_t(1.0);
  EXPECT_NEAR(2.0, sresult, 5.0e-6);

  d = scalar_t(1.0) + scalar_t(1.0);
  EXPECT_NEAR(2.0, d, 5.0e-6);

  d = scalar_t(1.0) + 1.0;
  EXPECT_NEAR(2.0, d, 5.0e-6);

  d = 1.0 + scalar_t(1.0);
  EXPECT_NEAR(2.0, d, 5.0e-6);
}

TEST_F(UnitContainer, unitTypeUnaryAddition) {
  meter_t a_m(1.0);

  EXPECT_EQ(++a_m, meter_t(2));
  EXPECT_EQ(a_m++, meter_t(2));
  EXPECT_EQ(a_m, meter_t(3));
  EXPECT_EQ(+a_m, meter_t(3));
  EXPECT_EQ(a_m, meter_t(3));

  dBW_t b_dBW(1.0);

  EXPECT_EQ(++b_dBW, dBW_t(2));
  EXPECT_EQ(b_dBW++, dBW_t(2));
  EXPECT_EQ(b_dBW, dBW_t(3));
  EXPECT_EQ(+b_dBW, dBW_t(3));
  EXPECT_EQ(b_dBW, dBW_t(3));
}

TEST_F(UnitContainer, unitTypeSubtraction) {
  meter_t a_m(1.0), c_m;
  foot_t b_ft(3.28084);

  c_m = a_m - b_ft;
  EXPECT_NEAR(0.0, c_m(), 5.0e-5);

  c_m = b_ft - meter_t(1);
  EXPECT_NEAR(0.0, c_m(), 5.0e-5);

  auto e_ft = b_ft - meter_t(1);
  EXPECT_NEAR(0.0, e_ft(), 5.0e-6);

  scalar_t sresult = scalar_t(1.0) - scalar_t(1.0);
  EXPECT_NEAR(0.0, sresult, 5.0e-6);

  sresult = scalar_t(1.0) - 1.0;
  EXPECT_NEAR(0.0, sresult, 5.0e-6);

  sresult = 1.0 - scalar_t(1.0);
  EXPECT_NEAR(0.0, sresult, 5.0e-6);

  double d = scalar_t(1.0) - scalar_t(1.0);
  EXPECT_NEAR(0.0, d, 5.0e-6);

  d = scalar_t(1.0) - 1.0;
  EXPECT_NEAR(0.0, d, 5.0e-6);

  d = 1.0 - scalar_t(1.0);
  EXPECT_NEAR(0.0, d, 5.0e-6);
}

TEST_F(UnitContainer, unitTypeUnarySubtraction) {
  meter_t a_m(4.0);

  EXPECT_EQ(--a_m, meter_t(3));
  EXPECT_EQ(a_m--, meter_t(3));
  EXPECT_EQ(a_m, meter_t(2));
  EXPECT_EQ(-a_m, meter_t(-2));
  EXPECT_EQ(a_m, meter_t(2));

  dBW_t b_dBW(4.0);

  EXPECT_EQ(--b_dBW, dBW_t(3));
  EXPECT_EQ(b_dBW--, dBW_t(3));
  EXPECT_EQ(b_dBW, dBW_t(2));
  EXPECT_EQ(-b_dBW, dBW_t(-2));
  EXPECT_EQ(b_dBW, dBW_t(2));
}

TEST_F(UnitContainer, unitTypeMultiplication) {
  meter_t a_m(1.0), b_m(2.0);
  foot_t a_ft(3.28084);

  auto c_m2 = a_m * b_m;
  EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

  c_m2 = b_m * meter_t(2);
  EXPECT_NEAR(4.0, c_m2(), 5.0e-5);

  c_m2 = b_m * a_ft;
  EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

  auto c_m = b_m * 2.0;
  EXPECT_NEAR(4.0, c_m(), 5.0e-5);

  c_m = 2.0 * b_m;
  EXPECT_NEAR(4.0, c_m(), 5.0e-5);

  double convert = scalar_t(3.14);
  EXPECT_NEAR(3.14, convert, 5.0e-5);

  scalar_t sresult = scalar_t(5.0) * scalar_t(4.0);
  EXPECT_NEAR(20.0, sresult(), 5.0e-5);

  sresult = scalar_t(5.0) * 4.0;
  EXPECT_NEAR(20.0, sresult(), 5.0e-5);

  sresult = 4.0 * scalar_t(5.0);
  EXPECT_NEAR(20.0, sresult(), 5.0e-5);

  double result = scalar_t(5.0) * scalar_t(4.0);
  EXPECT_NEAR(20.0, result, 5.0e-5);

  result = scalar_t(5.0) * 4.0;
  EXPECT_NEAR(20.0, result, 5.0e-5);

  result = 4.0 * scalar_t(5.0);
  EXPECT_NEAR(20.0, result, 5.0e-5);
}

TEST_F(UnitContainer, unitTypeMixedUnitMultiplication) {
  meter_t a_m(1.0);
  foot_t b_ft(3.28084);
  unit_t<inverse<meter>> i_m(2.0);

  // resultant unit is square of leftmost unit
  auto c_m2 = a_m * b_ft;
  EXPECT_NEAR(1.0, c_m2(), 5.0e-5);

  auto c_ft2 = b_ft * a_m;
  EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-7);

  // you can get whatever (compatible) type you want if you ask explicitly
  square_meter_t d_m2 = b_ft * a_m;
  EXPECT_NEAR(1.0, d_m2(), 5.0e-5);

  // a unit times a sclar ends up with the same units.
  meter_t e_m = a_m * scalar_t(3.0);
  EXPECT_NEAR(3.0, e_m(), 5.0e-5);

  e_m = scalar_t(4.0) * a_m;
  EXPECT_NEAR(4.0, e_m(), 5.0e-5);

  // unit times its inverse results in a scalar
  scalar_t s = a_m * i_m;
  EXPECT_NEAR(2.0, s, 5.0e-5);

  c_m2 = b_ft * meter_t(2);
  EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

  auto e_ft2 = b_ft * meter_t(3);
  EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);

  auto mps = meter_t(10.0) * unit_t<inverse<seconds>>(1.0);
  EXPECT_EQ(mps, meters_per_second_t(10));
}

TEST_F(UnitContainer, unitTypeScalarMultiplication) {
  meter_t a_m(1.0);

  auto result_m = scalar_t(3.0) * a_m;
  EXPECT_NEAR(3.0, result_m(), 5.0e-5);

  result_m = a_m * scalar_t(4.0);
  EXPECT_NEAR(4.0, result_m(), 5.0e-5);

  result_m = 3.0 * a_m;
  EXPECT_NEAR(3.0, result_m(), 5.0e-5);

  result_m = a_m * 4.0;
  EXPECT_NEAR(4.0, result_m(), 5.0e-5);

  bool isSame = std::is_same<decltype(result_m), meter_t>::value;
  EXPECT_TRUE(isSame);
}

TEST_F(UnitContainer, unitTypeDivision) {
  meter_t a_m(1.0), b_m(2.0);
  foot_t a_ft(3.28084);
  second_t a_sec(10.0);
  bool isSame;

  auto c = a_m / a_ft;
  EXPECT_NEAR(1.0, c, 5.0e-5);
  isSame = std::is_same<decltype(c), scalar_t>::value;
  EXPECT_TRUE(isSame);

  c = a_m / b_m;
  EXPECT_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same<decltype(c), scalar_t>::value;
  EXPECT_TRUE(isSame);

  c = a_ft / a_m;
  EXPECT_NEAR(1.0, c, 5.0e-5);
  isSame = std::is_same<decltype(c), scalar_t>::value;
  EXPECT_TRUE(isSame);

  c = scalar_t(1.0) / 2.0;
  EXPECT_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same<decltype(c), scalar_t>::value;
  EXPECT_TRUE(isSame);

  c = 1.0 / scalar_t(2.0);
  EXPECT_NEAR(0.5, c, 5.0e-5);
  isSame = std::is_same<decltype(c), scalar_t>::value;
  EXPECT_TRUE(isSame);

  double d = scalar_t(1.0) / 2.0;
  EXPECT_NEAR(0.5, d, 5.0e-5);

  auto e = a_m / a_sec;
  EXPECT_NEAR(0.1, e(), 5.0e-5);
  isSame = std::is_same<decltype(e), meters_per_second_t>::value;
  EXPECT_TRUE(isSame);

  auto f = a_m / 8.0;
  EXPECT_NEAR(0.125, f(), 5.0e-5);
  isSame = std::is_same<decltype(f), meter_t>::value;
  EXPECT_TRUE(isSame);

  auto g = 4.0 / b_m;
  EXPECT_NEAR(2.0, g(), 5.0e-5);
  isSame = std::is_same<decltype(g), unit_t<inverse<meters>>>::value;
  EXPECT_TRUE(isSame);

  auto mph = mile_t(60.0) / hour_t(1.0);
  meters_per_second_t mps = mph;
  EXPECT_NEAR(26.8224, mps(), 5.0e-5);
}

TEST_F(UnitContainer, compoundAssignmentAddition) {
  // units
  meter_t a(0.0);
  a += meter_t(1.0);

  EXPECT_EQ(meter_t(1.0), a);

  a += foot_t(meter_t(1));

  EXPECT_EQ(meter_t(2.0), a);

  // scalars
  scalar_t b(0);
  b += scalar_t(1.0);

  EXPECT_EQ(scalar_t(1.0), b);

  b += 1;

  EXPECT_EQ(scalar_t(2.0), b);
}

TEST_F(UnitContainer, compoundAssignmentSubtraction) {
  // units
  meter_t a(2.0);
  a -= meter_t(1.0);

  EXPECT_EQ(meter_t(1.0), a);

  a -= foot_t(meter_t(1));

  EXPECT_EQ(meter_t(0.0), a);

  // scalars
  scalar_t b(2);
  b -= scalar_t(1.0);

  EXPECT_EQ(scalar_t(1.0), b);

  b -= 1;

  EXPECT_EQ(scalar_t(0), b);
}

TEST_F(UnitContainer, compoundAssignmentMultiplication) {
  // units
  meter_t a(2.0);
  a *= scalar_t(2.0);

  EXPECT_EQ(meter_t(4.0), a);

  a *= 2.0;

  EXPECT_EQ(meter_t(8.0), a);

  // scalars
  scalar_t b(2);
  b *= scalar_t(2.0);

  EXPECT_EQ(scalar_t(4.0), b);

  b *= 2;

  EXPECT_EQ(scalar_t(8.0), b);
}

TEST_F(UnitContainer, compoundAssignmentDivision) {
  // units
  meter_t a(8.0);
  a /= scalar_t(2.0);

  EXPECT_EQ(meter_t(4.0), a);

  a /= 2.0;

  EXPECT_EQ(meter_t(2.0), a);

  // scalars
  scalar_t b(8);
  b /= scalar_t(2.0);

  EXPECT_EQ(scalar_t(4.0), b);

  b /= 2;

  EXPECT_EQ(scalar_t(2.0), b);
}

TEST_F(UnitContainer, scalarTypeImplicitConversion) {
  double test = scalar_t(3.0);
  EXPECT_DOUBLE_EQ(3.0, test);

  scalar_t testS = 3.0;
  EXPECT_DOUBLE_EQ(3.0, testS);

  scalar_t test3(ppm_t(10));
  EXPECT_DOUBLE_EQ(0.00001, test3);

  scalar_t test4;
  test4 = ppm_t(1);
  EXPECT_DOUBLE_EQ(0.000001, test4);
}

TEST_F(UnitContainer, valueMethod) {
  double test = meter_t(3.0).to<double>();
  EXPECT_DOUBLE_EQ(3.0, test);

  auto test2 = meter_t(4.0).value();
  EXPECT_DOUBLE_EQ(4.0, test2);
  EXPECT_TRUE((std::is_same<decltype(test2), double>::value));
}

TEST_F(UnitContainer, convertMethod) {
  double test = meter_t(3.0).convert<feet>().to<double>();
  EXPECT_NEAR(9.84252, test, 5.0e-6);
}

#ifndef UNIT_LIB_DISABLE_IOSTREAM
TEST_F(UnitContainer, cout) {
  testing::internal::CaptureStdout();
  std::cout << degree_t(349.87);
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("349.87 deg", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << meter_t(1.0);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("1 m", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << dB_t(31.0);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("31 dB", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << volt_t(21.79);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("21.79 V", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << dBW_t(12.0);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("12 dBW", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << dBm_t(120.0);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("120 dBm", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << miles_per_hour_t(72.1);
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("72.1 mph", output.c_str());

  // undefined unit
  testing::internal::CaptureStdout();
  std::cout << units::math::cpow<4>(meter_t(2));
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("16 m^4", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << units::math::cpow<3>(foot_t(2));
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("8 cu_ft", output.c_str());

  testing::internal::CaptureStdout();
  std::cout << std::setprecision(9) << units::math::cpow<4>(foot_t(2));
  output = testing::internal::GetCapturedStdout();
  EXPECT_STREQ("0.138095597 m^4", output.c_str());

  // constants
  testing::internal::CaptureStdout();
  std::cout << std::setprecision(8) << constants::k_B;
  output = testing::internal::GetCapturedStdout();
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  EXPECT_STREQ("1.3806485e-023 m^2 kg s^-2 K^-1", output.c_str());
#else
  EXPECT_STREQ("1.3806485e-23 m^2 kg s^-2 K^-1", output.c_str());
#endif

  testing::internal::CaptureStdout();
  std::cout << std::setprecision(9) << constants::mu_B;
  output = testing::internal::GetCapturedStdout();
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  EXPECT_STREQ("9.27400999e-024 m^2 A", output.c_str());
#else
  EXPECT_STREQ("9.27400999e-24 m^2 A", output.c_str());
#endif

  testing::internal::CaptureStdout();
  std::cout << std::setprecision(7) << constants::sigma;
  output = testing::internal::GetCapturedStdout();
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
  EXPECT_STREQ("5.670367e-008 kg s^-3 K^-4", output.c_str());
#else
  EXPECT_STREQ("5.670367e-08 kg s^-3 K^-4", output.c_str());
#endif
}

TEST_F(UnitContainer, to_string) {
  foot_t a(3.5);
  EXPECT_STREQ("3.5 ft", units::length::to_string(a).c_str());

  meter_t b(8);
  EXPECT_STREQ("8 m", units::length::to_string(b).c_str());
}

TEST_F(UnitContainer, DISABLED_to_string_locale) {
  struct lconv* lc;

  // German locale
#if defined(_MSC_VER)
  setlocale(LC_ALL, "de-DE");
#else
  EXPECT_STREQ("de_DE.utf8", setlocale(LC_ALL, "de_DE.utf8"));
#endif

  lc = localeconv();
  char point_de = *lc->decimal_point;
  EXPECT_EQ(point_de, ',');

  kilometer_t de = 2_km;
  EXPECT_STREQ("2 km", units::length::to_string(de).c_str());

  de = 2.5_km;
  EXPECT_STREQ("2,5 km", units::length::to_string(de).c_str());

  // US locale
#if defined(_MSC_VER)
  setlocale(LC_ALL, "en-US");
#else
  EXPECT_STREQ("en_US.utf8", setlocale(LC_ALL, "en_US.utf8"));
#endif

  lc = localeconv();
  char point_us = *lc->decimal_point;
  EXPECT_EQ(point_us, '.');

  mile_t us = 2_mi;
  EXPECT_STREQ("2 mi", units::length::to_string(us).c_str());

  us = 2.5_mi;
  EXPECT_STREQ("2.5 mi", units::length::to_string(us).c_str());
}

TEST_F(UnitContainer, nameAndAbbreviation) {
  foot_t a(3.5);
  EXPECT_STREQ("ft", units::abbreviation(a));
  EXPECT_STREQ("ft", a.abbreviation());
  EXPECT_STREQ("foot", a.name());

  meter_t b(8);
  EXPECT_STREQ("m", units::abbreviation(b));
  EXPECT_STREQ("m", b.abbreviation());
  EXPECT_STREQ("meter", b.name());
}
#endif

TEST_F(UnitContainer, negative) {
  meter_t a(5.3);
  meter_t b(-5.3);
  EXPECT_NEAR(a.to<double>(), -b.to<double>(), 5.0e-320);
  EXPECT_NEAR(b.to<double>(), -a.to<double>(), 5.0e-320);

  dB_t c(2.87);
  dB_t d(-2.87);
  EXPECT_NEAR(c.to<double>(), -d.to<double>(), 5.0e-320);
  EXPECT_NEAR(d.to<double>(), -c.to<double>(), 5.0e-320);

  ppm_t e = -1 * ppm_t(10);
  EXPECT_EQ(e, -ppm_t(10));
  EXPECT_NEAR(-0.00001, e, 5.0e-10);
}

TEST_F(UnitContainer, concentration) {
  ppb_t a(ppm_t(1));
  EXPECT_EQ(ppb_t(1000), a);
  EXPECT_EQ(0.000001, a);
  EXPECT_EQ(0.000001, a.to<double>());

  scalar_t b(ppm_t(1));
  EXPECT_EQ(0.000001, b);

  scalar_t c = ppb_t(1);
  EXPECT_EQ(0.000000001, c);
}

TEST_F(UnitContainer, dBConversion) {
  dBW_t a_dbw(23.1);
  watt_t a_w = a_dbw;
  dBm_t a_dbm = a_dbw;

  EXPECT_NEAR(204.173794, a_w(), 5.0e-7);
  EXPECT_NEAR(53.1, a_dbm(), 5.0e-7);

  milliwatt_t b_mw(100000.0);
  watt_t b_w = b_mw;
  dBm_t b_dbm = b_mw;
  dBW_t b_dbw = b_mw;

  EXPECT_NEAR(100.0, b_w(), 5.0e-7);
  EXPECT_NEAR(50.0, b_dbm(), 5.0e-7);
  EXPECT_NEAR(20.0, b_dbw(), 5.0e-7);
}

TEST_F(UnitContainer, dBAddition) {
  bool isSame;

  auto result_dbw = dBW_t(10.0) + dB_t(30.0);
  EXPECT_NEAR(40.0, result_dbw(), 5.0e-5);
  result_dbw = dB_t(12.0) + dBW_t(30.0);
  EXPECT_NEAR(42.0, result_dbw(), 5.0e-5);
  isSame = std::is_same<decltype(result_dbw), dBW_t>::value;
  EXPECT_TRUE(isSame);

  auto result_dbm = dB_t(30.0) + dBm_t(20.0);
  EXPECT_NEAR(50.0, result_dbm(), 5.0e-5);

  // adding dBW to dBW is something you probably shouldn't do, but let's see if
  // it works...
  auto result_dBW2 = dBW_t(10.0) + dBm_t(40.0);
  EXPECT_NEAR(20.0, result_dBW2(), 5.0e-5);
  isSame = std::is_same<decltype(result_dBW2),
                        unit_t<squared<watts>, double, decibel_scale>>::value;
  EXPECT_TRUE(isSame);
}

TEST_F(UnitContainer, dBSubtraction) {
  bool isSame;

  auto result_dbw = dBW_t(10.0) - dB_t(30.0);
  EXPECT_NEAR(-20.0, result_dbw(), 5.0e-5);
  isSame = std::is_same<decltype(result_dbw), dBW_t>::value;
  EXPECT_TRUE(isSame);

  auto result_dbm = dBm_t(100.0) - dB_t(30.0);
  EXPECT_NEAR(70.0, result_dbm(), 5.0e-5);
  isSame = std::is_same<decltype(result_dbm), dBm_t>::value;
  EXPECT_TRUE(isSame);

  auto result_db = dBW_t(100.0) - dBW_t(80.0);
  EXPECT_NEAR(20.0, result_db(), 5.0e-5);
  isSame = std::is_same<decltype(result_db), dB_t>::value;
  EXPECT_TRUE(isSame);

  result_db = dB_t(100.0) - dB_t(80.0);
  EXPECT_NEAR(20.0, result_db(), 5.0e-5);
  isSame = std::is_same<decltype(result_db), dB_t>::value;
  EXPECT_TRUE(isSame);
}

TEST_F(UnitContainer, unit_cast) {
  meter_t test1(5.7);
  hectare_t test2(16);

  double dResult1 = 5.7;

  double dResult2 = 16;
  int iResult2 = 16;

  EXPECT_EQ(dResult1, unit_cast<double>(test1));
  EXPECT_EQ(dResult2, unit_cast<double>(test2));
  EXPECT_EQ(iResult2, unit_cast<int>(test2));

  EXPECT_TRUE(
      (std::is_same<double, decltype(unit_cast<double>(test1))>::value));
  EXPECT_TRUE((std::is_same<int, decltype(unit_cast<int>(test2))>::value));
}

// literal syntax is only supported in GCC 4.7+ and MSVC2015+
#if !defined(_MSC_VER) || _MSC_VER > 1800
TEST_F(UnitContainer, literals) {
  // basic functionality testing
  EXPECT_TRUE((std::is_same<decltype(16.2_m), meter_t>::value));
  EXPECT_TRUE(meter_t(16.2) == 16.2_m);
  EXPECT_TRUE(meter_t(16) == 16_m);

  EXPECT_TRUE((std::is_same<decltype(11.2_ft), foot_t>::value));
  EXPECT_TRUE(foot_t(11.2) == 11.2_ft);
  EXPECT_TRUE(foot_t(11) == 11_ft);

  // auto using literal syntax
  auto x = 10.0_m;
  EXPECT_TRUE((std::is_same<decltype(x), meter_t>::value));
  EXPECT_TRUE(meter_t(10) == x);

  // conversion using literal syntax
  foot_t y = 0.3048_m;
  EXPECT_TRUE(1_ft == y);

  // Pythagorean theorem
  meter_t a = 3_m;
  meter_t b = 4_m;
  meter_t c = sqrt(pow<2>(a) + pow<2>(b));
  EXPECT_TRUE(c == 5_m);
}
#endif

TEST_F(UnitConversion, length) {
  double test;
  test = convert<meters, nanometers>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, micrometers>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, millimeters>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, centimeters>(0.01);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, kilometers>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, meters>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, feet>(0.3048);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, miles>(1609.344);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, inches>(0.0254);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, nauticalMiles>(1852.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, astronicalUnits>(149597870700.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, lightyears>(9460730472580800.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<meters, parsec>(3.08567758e16);
  EXPECT_NEAR(1.0, test, 5.0e7);

  test = convert<feet, feet>(6.3);
  EXPECT_NEAR(6.3, test, 5.0e-5);
  test = convert<feet, inches>(6.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
  test = convert<inches, feet>(6.0);
  EXPECT_NEAR(0.5, test, 5.0e-5);
  test = convert<meter, feet>(1.0);
  EXPECT_NEAR(3.28084, test, 5.0e-5);
  test = convert<miles, nauticalMiles>(6.3);
  EXPECT_NEAR(5.47455, test, 5.0e-6);
  test = convert<miles, meters>(11.0);
  EXPECT_NEAR(17702.8, test, 5.0e-2);
  test = convert<meters, chains>(1.0);
  EXPECT_NEAR(0.0497097, test, 5.0e-7);
}

TEST_F(UnitConversion, mass) {
  double test;

  test = convert<kilograms, grams>(1.0e-3);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, micrograms>(1.0e-9);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, milligrams>(1.0e-6);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, kilograms>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, metric_tons>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, pounds>(0.453592);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, long_tons>(1016.05);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, short_tons>(907.185);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, mass::ounces>(0.0283495);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<kilograms, carats>(0.0002);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<slugs, kilograms>(1.0);
  EXPECT_NEAR(14.593903, test, 5.0e-7);

  test = convert<pounds, carats>(6.3);
  EXPECT_NEAR(14288.2, test, 5.0e-2);
}

TEST_F(UnitConversion, time) {
  double result = 0;
  double daysPerYear = 365;
  double hoursPerDay = 24;
  double minsPerHour = 60;
  double secsPerMin = 60;
  double daysPerWeek = 7;

  result = 2 * daysPerYear * hoursPerDay * minsPerHour * secsPerMin *
           (1 / minsPerHour) * (1 / secsPerMin) * (1 / hoursPerDay) *
           (1 / daysPerWeek);
  EXPECT_NEAR(104.286, result, 5.0e-4);

  year_t twoYears(2.0);
  week_t twoYearsInWeeks = twoYears;
  EXPECT_NEAR(week_t(104.286).to<double>(), twoYearsInWeeks.to<double>(),
              5.0e-4);

  double test;

  test = convert<seconds, seconds>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, nanoseconds>(1.0e-9);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, microseconds>(1.0e-6);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, milliseconds>(1.0e-3);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, minutes>(60.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, hours>(3600.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, days>(86400.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, weeks>(604800.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<seconds, years>(3.154e7);
  EXPECT_NEAR(1.0, test, 5.0e3);

  test = convert<years, weeks>(2.0);
  EXPECT_NEAR(104.2857142857143, test, 5.0e-14);
  test = convert<hours, minutes>(4.0);
  EXPECT_NEAR(240.0, test, 5.0e-14);
  test = convert<julian_years, days>(1.0);
  EXPECT_NEAR(365.25, test, 5.0e-14);
  test = convert<gregorian_years, days>(1.0);
  EXPECT_NEAR(365.2425, test, 5.0e-14);
}

TEST_F(UnitConversion, angle) {
  angle::degree_t quarterCircleDeg(90.0);
  angle::radian_t quarterCircleRad = quarterCircleDeg;
  EXPECT_NEAR(angle::radian_t(constants::detail::PI_VAL / 2.0).to<double>(),
              quarterCircleRad.to<double>(), 5.0e-12);

  double test;

  test = convert<angle::radians, angle::radians>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-20);
  test = convert<angle::radians, angle::milliradians>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-4);
  test = convert<angle::radians, angle::degrees>(0.0174533);
  EXPECT_NEAR(1.0, test, 5.0e-7);
  test = convert<angle::radians, angle::arcminutes>(0.000290888);
  EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
  test = convert<angle::radians, angle::arcseconds>(4.8481e-6);
  EXPECT_NEAR(0.999992407, test, 5.0e-10);
  test = convert<angle::radians, angle::turns>(6.28319);
  EXPECT_NEAR(1.0, test, 5.0e-6);
  test = convert<angle::radians, angle::gradians>(0.015708);
  EXPECT_NEAR(1.0, test, 5.0e-6);

  test = convert<angle::radians, angle::radians>(2.1);
  EXPECT_NEAR(2.1, test, 5.0e-6);
  test = convert<angle::arcseconds, angle::gradians>(2.1);
  EXPECT_NEAR(0.000648148, test, 5.0e-6);
  test = convert<angle::radians, angle::degrees>(constants::detail::PI_VAL);
  EXPECT_NEAR(180.0, test, 5.0e-6);
  test = convert<angle::degrees, angle::radians>(90.0);
  EXPECT_NEAR(constants::detail::PI_VAL / 2, test, 5.0e-6);
}

TEST_F(UnitConversion, current) {
  double test;

  test = convert<current::A, current::mA>(2.1);
  EXPECT_NEAR(2100.0, test, 5.0e-6);
}

TEST_F(UnitConversion, temperature) {
  // temp conversion are weird/hard since they involve translations AND scaling.
  double test;

  test = convert<kelvin, kelvin>(72.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
  test = convert<fahrenheit, fahrenheit>(72.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
  test = convert<kelvin, fahrenheit>(300.0);
  EXPECT_NEAR(80.33, test, 5.0e-5);
  test = convert<fahrenheit, kelvin>(451.0);
  EXPECT_NEAR(505.928, test, 5.0e-4);
  test = convert<kelvin, celsius>(300.0);
  EXPECT_NEAR(26.85, test, 5.0e-3);
  test = convert<celsius, kelvin>(451.0);
  EXPECT_NEAR(724.15, test, 5.0e-3);
  test = convert<fahrenheit, celsius>(72.0);
  EXPECT_NEAR(22.2222, test, 5.0e-5);
  test = convert<celsius, fahrenheit>(100.0);
  EXPECT_NEAR(212.0, test, 5.0e-5);
  test = convert<fahrenheit, celsius>(32.0);
  EXPECT_NEAR(0.0, test, 5.0e-5);
  test = convert<celsius, fahrenheit>(0.0);
  EXPECT_NEAR(32.0, test, 5.0e-5);
  test = convert<rankine, kelvin>(100.0);
  EXPECT_NEAR(55.5556, test, 5.0e-5);
  test = convert<kelvin, rankine>(100.0);
  EXPECT_NEAR(180.0, test, 5.0e-5);
  test = convert<fahrenheit, rankine>(100.0);
  EXPECT_NEAR(559.67, test, 5.0e-5);
  test = convert<rankine, fahrenheit>(72.0);
  EXPECT_NEAR(-387.67, test, 5.0e-5);
  test = convert<reaumur, kelvin>(100.0);
  EXPECT_NEAR(398.0, test, 5.0e-1);
  test = convert<reaumur, celsius>(80.0);
  EXPECT_NEAR(100.0, test, 5.0e-5);
  test = convert<celsius, reaumur>(212.0);
  EXPECT_NEAR(169.6, test, 5.0e-2);
  test = convert<reaumur, fahrenheit>(80.0);
  EXPECT_NEAR(212.0, test, 5.0e-5);
  test = convert<fahrenheit, reaumur>(37.0);
  EXPECT_NEAR(2.222, test, 5.0e-3);
}

TEST_F(UnitConversion, luminous_intensity) {
  double test;

  test = convert<candela, millicandela>(72.0);
  EXPECT_NEAR(72000.0, test, 5.0e-5);
  test = convert<millicandela, candela>(376.0);
  EXPECT_NEAR(0.376, test, 5.0e-5);
}

TEST_F(UnitConversion, solid_angle) {
  double test;
  bool same;

  same = std::is_same<traits::base_unit_of<steradians>,
                      traits::base_unit_of<degrees_squared>>::value;
  EXPECT_TRUE(same);

  test = convert<steradians, steradians>(72.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
  test = convert<steradians, degrees_squared>(1.0);
  EXPECT_NEAR(3282.8, test, 5.0e-2);
  test = convert<steradians, spats>(8.0);
  EXPECT_NEAR(0.636619772367582, test, 5.0e-14);
  test = convert<degrees_squared, steradians>(3282.8);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<degrees_squared, degrees_squared>(72.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
  test = convert<degrees_squared, spats>(3282.8);
  EXPECT_NEAR(1.0 / (4 * constants::detail::PI_VAL), test, 5.0e-5);
  test = convert<spats, steradians>(1.0 / (4 * constants::detail::PI_VAL));
  EXPECT_NEAR(1.0, test, 5.0e-14);
  test = convert<spats, degrees_squared>(1.0 / (4 * constants::detail::PI_VAL));
  EXPECT_NEAR(3282.8, test, 5.0e-2);
  test = convert<spats, spats>(72.0);
  EXPECT_NEAR(72.0, test, 5.0e-5);
}

TEST_F(UnitConversion, frequency) {
  double test;

  test = convert<hertz, kilohertz>(63000.0);
  EXPECT_NEAR(63.0, test, 5.0e-5);
  test = convert<hertz, hertz>(6.3);
  EXPECT_NEAR(6.3, test, 5.0e-5);
  test = convert<kilohertz, hertz>(5.0);
  EXPECT_NEAR(5000.0, test, 5.0e-5);
  test = convert<megahertz, hertz>(1.0);
  EXPECT_NEAR(1.0e6, test, 5.0e-5);
}

TEST_F(UnitConversion, velocity) {
  double test;
  bool same;

  same = std::is_same<meters_per_second,
                      unit<std::ratio<1>, category::velocity_unit>>::value;
  EXPECT_TRUE(same);
  same = traits::is_convertible_unit<miles_per_hour, meters_per_second>::value;
  EXPECT_TRUE(same);

  test = convert<meters_per_second, miles_per_hour>(1250.0);
  EXPECT_NEAR(2796.17, test, 5.0e-3);
  test = convert<feet_per_second, kilometers_per_hour>(2796.17);
  EXPECT_NEAR(3068.181418, test, 5.0e-7);
  test = convert<knots, miles_per_hour>(600.0);
  EXPECT_NEAR(690.468, test, 5.0e-4);
  test = convert<miles_per_hour, feet_per_second>(120.0);
  EXPECT_NEAR(176.0, test, 5.0e-5);
  test = convert<feet_per_second, meters_per_second>(10.0);
  EXPECT_NEAR(3.048, test, 5.0e-5);
}

TEST_F(UnitConversion, angular_velocity) {
  double test;
  bool same;

  same =
      std::is_same<radians_per_second,
                   unit<std::ratio<1>, category::angular_velocity_unit>>::value;
  EXPECT_TRUE(same);
  same = traits::is_convertible_unit<rpm, radians_per_second>::value;
  EXPECT_TRUE(same);

  test = convert<radians_per_second, milliarcseconds_per_year>(1.0);
  EXPECT_NEAR(6.504e15, test, 1.0e12);
  test = convert<degrees_per_second, radians_per_second>(1.0);
  EXPECT_NEAR(0.0174533, test, 5.0e-8);
  test = convert<rpm, radians_per_second>(1.0);
  EXPECT_NEAR(0.10471975512, test, 5.0e-13);
  test = convert<milliarcseconds_per_year, radians_per_second>(1.0);
  EXPECT_NEAR(1.537e-16, test, 5.0e-20);
}

TEST_F(UnitConversion, acceleration) {
  double test;

  test = convert<standard_gravity, meters_per_second_squared>(1.0);
  EXPECT_NEAR(9.80665, test, 5.0e-10);
}

TEST_F(UnitConversion, force) {
  double test;

  test = convert<units::force::newton, units::force::newton>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<units::force::newton, units::force::pounds>(6.3);
  EXPECT_NEAR(1.4163, test, 5.0e-5);
  test = convert<units::force::newton, units::force::dynes>(5.0);
  EXPECT_NEAR(500000.0, test, 5.0e-5);
  test = convert<units::force::newtons, units::force::poundals>(2.1);
  EXPECT_NEAR(15.1893, test, 5.0e-5);
  test = convert<units::force::newtons, units::force::kiloponds>(173.0);
  EXPECT_NEAR(17.6411, test, 5.0e-5);
  test = convert<units::force::poundals, units::force::kiloponds>(21.879);
  EXPECT_NEAR(0.308451933, test, 5.0e-10);
}

TEST_F(UnitConversion, area) {
  double test;

  test = convert<hectares, acres>(6.3);
  EXPECT_NEAR(15.5676, test, 5.0e-5);
  test = convert<square_miles, square_kilometers>(10.0);
  EXPECT_NEAR(25.8999, test, 5.0e-5);
  test = convert<square_inch, square_meter>(4.0);
  EXPECT_NEAR(0.00258064, test, 5.0e-9);
  test = convert<acre, square_foot>(5.0);
  EXPECT_NEAR(217800.0, test, 5.0e-5);
  test = convert<square_meter, square_foot>(1.0);
  EXPECT_NEAR(10.7639, test, 5.0e-5);
}

TEST_F(UnitConversion, pressure) {
  double test;

  test = convert<pascals, torr>(1.0);
  EXPECT_NEAR(0.00750062, test, 5.0e-5);
  test = convert<bar, psi>(2.2);
  EXPECT_NEAR(31.9083, test, 5.0e-5);
  test = convert<atmospheres, bar>(4.0);
  EXPECT_NEAR(4.053, test, 5.0e-5);
  test = convert<torr, pascals>(800.0);
  EXPECT_NEAR(106657.89474, test, 5.0e-5);
  test = convert<psi, atmospheres>(38.0);
  EXPECT_NEAR(2.58575, test, 5.0e-5);
  test = convert<psi, pascals>(1.0);
  EXPECT_NEAR(6894.76, test, 5.0e-3);
  test = convert<pascals, bar>(0.25);
  EXPECT_NEAR(2.5e-6, test, 5.0e-5);
  test = convert<torr, atmospheres>(9.0);
  EXPECT_NEAR(0.0118421, test, 5.0e-8);
  test = convert<bar, torr>(12.0);
  EXPECT_NEAR(9000.74, test, 5.0e-3);
  test = convert<atmospheres, psi>(1.0);
  EXPECT_NEAR(14.6959, test, 5.0e-5);
}

TEST_F(UnitConversion, charge) {
  double test;

  test = convert<coulombs, ampere_hours>(4.0);
  EXPECT_NEAR(0.00111111, test, 5.0e-9);
  test = convert<ampere_hours, coulombs>(1.0);
  EXPECT_NEAR(3600.0, test, 5.0e-6);
}

TEST_F(UnitConversion, energy) {
  double test;

  test = convert<joules, calories>(8000.000464);
  EXPECT_NEAR(1912.046, test, 5.0e-4);
  test = convert<therms, joules>(12.0);
  EXPECT_NEAR(1.266e+9, test, 5.0e5);
  test = convert<megajoules, watt_hours>(100.0);
  EXPECT_NEAR(27777.778, test, 5.0e-4);
  test = convert<kilocalories, megajoules>(56.0);
  EXPECT_NEAR(0.234304, test, 5.0e-7);
  test = convert<kilojoules, therms>(56.0);
  EXPECT_NEAR(0.000530904, test, 5.0e-5);
  test = convert<british_thermal_units, kilojoules>(18.56399995447);
  EXPECT_NEAR(19.5860568, test, 5.0e-5);
  test = convert<calories, energy::foot_pounds>(18.56399995447);
  EXPECT_NEAR(57.28776190423856, test, 5.0e-5);
  test = convert<megajoules, calories>(1.0);
  EXPECT_NEAR(239006.0, test, 5.0e-1);
  test = convert<kilocalories, kilowatt_hours>(2.0);
  EXPECT_NEAR(0.00232444, test, 5.0e-9);
  test = convert<therms, kilocalories>(0.1);
  EXPECT_NEAR(2521.04, test, 5.0e-3);
  test = convert<watt_hours, megajoules>(67.0);
  EXPECT_NEAR(0.2412, test, 5.0e-5);
  test = convert<british_thermal_units, watt_hours>(100.0);
  EXPECT_NEAR(29.3071, test, 5.0e-5);
  test = convert<calories, BTU>(100.0);
  EXPECT_NEAR(0.396567, test, 5.0e-5);
}

TEST_F(UnitConversion, power) {
  double test;

  test = convert<compound_unit<energy::foot_pounds, inverse<seconds>>, watts>(
      550.0);
  EXPECT_NEAR(745.7, test, 5.0e-2);
  test = convert<watts, gigawatts>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-4);
  test = convert<microwatts, watts>(200000.0);
  EXPECT_NEAR(0.2, test, 5.0e-4);
  test = convert<horsepower, watts>(100.0);
  EXPECT_NEAR(74570.0, test, 5.0e-1);
  test = convert<horsepower, megawatts>(5.0);
  EXPECT_NEAR(0.0037284994, test, 5.0e-7);
  test = convert<kilowatts, horsepower>(232.0);
  EXPECT_NEAR(311.117, test, 5.0e-4);
  test = convert<milliwatts, horsepower>(1001.0);
  EXPECT_NEAR(0.001342363, test, 5.0e-9);
}

TEST_F(UnitConversion, voltage) {
  double test;

  test = convert<volts, millivolts>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picovolts, volts>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanovolts, volts>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microvolts, volts>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millivolts, volts>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilovolts, volts>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megavolts, volts>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigavolts, volts>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<statvolts, volts>(299.792458);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millivolts, statvolts>(1000.0);
  EXPECT_NEAR(299.792458, test, 5.0e-5);
  test = convert<abvolts, nanovolts>(0.1);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microvolts, abvolts>(0.01);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, capacitance) {
  double test;

  test = convert<farads, millifarads>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picofarads, farads>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanofarads, farads>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microfarads, farads>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millifarads, farads>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilofarads, farads>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megafarads, farads>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigafarads, farads>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, impedance) {
  double test;

  test = convert<ohms, milliohms>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoohms, ohms>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoohms, ohms>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microohms, ohms>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<milliohms, ohms>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloohms, ohms>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megaohms, ohms>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigaohms, ohms>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, conductance) {
  double test;

  test = convert<siemens, millisiemens>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picosiemens, siemens>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanosiemens, siemens>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microsiemens, siemens>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millisiemens, siemens>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilosiemens, siemens>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megasiemens, siemens>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigasiemens, siemens>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, magnetic_flux) {
  double test;

  test = convert<webers, milliwebers>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picowebers, webers>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanowebers, webers>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microwebers, webers>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<milliwebers, webers>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilowebers, webers>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megawebers, webers>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigawebers, webers>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<maxwells, webers>(100000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanowebers, maxwells>(10.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, magnetic_field_strength) {
  double test;

  test = convert<teslas, milliteslas>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoteslas, teslas>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoteslas, teslas>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microteslas, teslas>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<milliteslas, teslas>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloteslas, teslas>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megateslas, teslas>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigateslas, teslas>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gauss, teslas>(10000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoteslas, gauss>(100000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, inductance) {
  double test;

  test = convert<henries, millihenries>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picohenries, henries>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanohenries, henries>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microhenries, henries>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millihenries, henries>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilohenries, henries>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megahenries, henries>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigahenries, henries>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, luminous_flux) {
  double test;

  test = convert<lumens, millilumens>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picolumens, lumens>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanolumens, lumens>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microlumens, lumens>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millilumens, lumens>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilolumens, lumens>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megalumens, lumens>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigalumens, lumens>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, illuminance) {
  double test;

  test = convert<luxes, milliluxes>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picoluxes, luxes>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanoluxes, luxes>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microluxes, luxes>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<milliluxes, luxes>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kiloluxes, luxes>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megaluxes, luxes>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigaluxes, luxes>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);

  test = convert<footcandles, luxes>(0.092903);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<lux, lumens_per_square_inch>(1550.0031000062);
  EXPECT_NEAR(1.0, test, 5.0e-13);
  test = convert<phots, luxes>(0.0001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, radiation) {
  double test;

  test = convert<becquerels, millibecquerels>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picobecquerels, becquerels>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanobecquerels, becquerels>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microbecquerels, becquerels>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millibecquerels, becquerels>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilobecquerels, becquerels>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megabecquerels, becquerels>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigabecquerels, becquerels>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);

  test = convert<grays, milligrays>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picograys, grays>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanograys, grays>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<micrograys, grays>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<milligrays, grays>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilograys, grays>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megagrays, grays>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigagrays, grays>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);

  test = convert<sieverts, millisieverts>(10.0);
  EXPECT_NEAR(10000.0, test, 5.0e-5);
  test = convert<picosieverts, sieverts>(1000000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<nanosieverts, sieverts>(1000000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<microsieverts, sieverts>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<millisieverts, sieverts>(1000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<kilosieverts, sieverts>(0.001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<megasieverts, sieverts>(0.000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<gigasieverts, sieverts>(0.000000001);
  EXPECT_NEAR(1.0, test, 5.0e-5);

  test = convert<becquerels, curies>(37.0e9);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<becquerels, rutherfords>(1000000.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<rads, grays>(100.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, torque) {
  double test;

  test = convert<torque::foot_pounds, newton_meter>(1.0);
  EXPECT_NEAR(1.355817948, test, 5.0e-5);
  test = convert<inch_pounds, newton_meter>(1.0);
  EXPECT_NEAR(0.112984829, test, 5.0e-5);
  test = convert<foot_poundals, newton_meter>(1.0);
  EXPECT_NEAR(4.214011009e-2, test, 5.0e-5);
  test = convert<meter_kilograms, newton_meter>(1.0);
  EXPECT_NEAR(9.80665, test, 5.0e-5);
  test = convert<inch_pound, meter_kilogram>(86.79616930855788);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<foot_poundals, inch_pound>(2.681170713);
  EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitConversion, volume) {
  double test;

  test = convert<cubic_meters, cubic_meter>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<cubic_millimeters, cubic_meter>(1.0);
  EXPECT_NEAR(1.0e-9, test, 5.0e-5);
  test = convert<cubic_kilometers, cubic_meter>(1.0);
  EXPECT_NEAR(1.0e9, test, 5.0e-5);
  test = convert<liters, cubic_meter>(1.0);
  EXPECT_NEAR(0.001, test, 5.0e-5);
  test = convert<milliliters, cubic_meter>(1.0);
  EXPECT_NEAR(1.0e-6, test, 5.0e-5);
  test = convert<cubic_inches, cubic_meter>(1.0);
  EXPECT_NEAR(1.6387e-5, test, 5.0e-10);
  test = convert<cubic_feet, cubic_meter>(1.0);
  EXPECT_NEAR(0.0283168, test, 5.0e-8);
  test = convert<cubic_yards, cubic_meter>(1.0);
  EXPECT_NEAR(0.764555, test, 5.0e-7);
  test = convert<cubic_miles, cubic_meter>(1.0);
  EXPECT_NEAR(4.168e+9, test, 5.0e5);
  test = convert<gallons, cubic_meter>(1.0);
  EXPECT_NEAR(0.00378541, test, 5.0e-8);
  test = convert<quarts, cubic_meter>(1.0);
  EXPECT_NEAR(0.000946353, test, 5.0e-10);
  test = convert<pints, cubic_meter>(1.0);
  EXPECT_NEAR(0.000473176, test, 5.0e-10);
  test = convert<cups, cubic_meter>(1.0);
  EXPECT_NEAR(0.00024, test, 5.0e-6);
  test = convert<volume::fluid_ounces, cubic_meter>(1.0);
  EXPECT_NEAR(2.9574e-5, test, 5.0e-5);
  test = convert<barrels, cubic_meter>(1.0);
  EXPECT_NEAR(0.158987294928, test, 5.0e-13);
  test = convert<bushels, cubic_meter>(1.0);
  EXPECT_NEAR(0.0352391, test, 5.0e-8);
  test = convert<cords, cubic_meter>(1.0);
  EXPECT_NEAR(3.62456, test, 5.0e-6);
  test = convert<cubic_fathoms, cubic_meter>(1.0);
  EXPECT_NEAR(6.11644, test, 5.0e-6);
  test = convert<tablespoons, cubic_meter>(1.0);
  EXPECT_NEAR(1.4787e-5, test, 5.0e-10);
  test = convert<teaspoons, cubic_meter>(1.0);
  EXPECT_NEAR(4.9289e-6, test, 5.0e-11);
  test = convert<pinches, cubic_meter>(1.0);
  EXPECT_NEAR(616.11519921875e-9, test, 5.0e-20);
  test = convert<dashes, cubic_meter>(1.0);
  EXPECT_NEAR(308.057599609375e-9, test, 5.0e-20);
  test = convert<drops, cubic_meter>(1.0);
  EXPECT_NEAR(82.14869322916e-9, test, 5.0e-9);
  test = convert<fifths, cubic_meter>(1.0);
  EXPECT_NEAR(0.00075708236, test, 5.0e-12);
  test = convert<drams, cubic_meter>(1.0);
  EXPECT_NEAR(3.69669e-6, test, 5.0e-12);
  test = convert<gills, cubic_meter>(1.0);
  EXPECT_NEAR(0.000118294, test, 5.0e-10);
  test = convert<pecks, cubic_meter>(1.0);
  EXPECT_NEAR(0.00880977, test, 5.0e-9);
  test = convert<sacks, cubic_meter>(9.4591978);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<shots, cubic_meter>(1.0);
  EXPECT_NEAR(4.43603e-5, test, 5.0e-11);
  test = convert<strikes, cubic_meter>(1.0);
  EXPECT_NEAR(0.07047814033376, test, 5.0e-5);
  test = convert<volume::fluid_ounces, milliliters>(1.0);
  EXPECT_NEAR(29.5735, test, 5.0e-5);
}

TEST_F(UnitConversion, density) {
  double test;

  test = convert<kilograms_per_cubic_meter, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(1.0, test, 5.0e-5);
  test = convert<grams_per_milliliter, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(1000.0, test, 5.0e-5);
  test = convert<kilograms_per_liter, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(1000.0, test, 5.0e-5);
  test = convert<ounces_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(1.001153961, test, 5.0e-10);
  test = convert<ounces_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(1.729994044e3, test, 5.0e-7);
  test = convert<ounces_per_gallon, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(7.489151707, test, 5.0e-10);
  test = convert<pounds_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(16.01846337, test, 5.0e-9);
  test = convert<pounds_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(2.767990471e4, test, 5.0e-6);
  test = convert<pounds_per_gallon, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(119.8264273, test, 5.0e-8);
  test = convert<slugs_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
  EXPECT_NEAR(515.3788184, test, 5.0e-6);
}

TEST_F(UnitConversion, concentration) {
  double test;

  test = ppm_t(1.0);
  EXPECT_NEAR(1.0e-6, test, 5.0e-12);
  test = ppb_t(1.0);
  EXPECT_NEAR(1.0e-9, test, 5.0e-12);
  test = ppt_t(1.0);
  EXPECT_NEAR(1.0e-12, test, 5.0e-12);
  test = percent_t(18.0);
  EXPECT_NEAR(0.18, test, 5.0e-12);
}

TEST_F(UnitConversion, data) {
  EXPECT_EQ(8, (convert<byte, bit>(1)));

  EXPECT_EQ(1000, (convert<kilobytes, bytes>(1)));
  EXPECT_EQ(1000, (convert<megabytes, kilobytes>(1)));
  EXPECT_EQ(1000, (convert<gigabytes, megabytes>(1)));
  EXPECT_EQ(1000, (convert<terabytes, gigabytes>(1)));
  EXPECT_EQ(1000, (convert<petabytes, terabytes>(1)));
  EXPECT_EQ(1000, (convert<exabytes, petabytes>(1)));

  EXPECT_EQ(1024, (convert<kibibytes, bytes>(1)));
  EXPECT_EQ(1024, (convert<mebibytes, kibibytes>(1)));
  EXPECT_EQ(1024, (convert<gibibytes, mebibytes>(1)));
  EXPECT_EQ(1024, (convert<tebibytes, gibibytes>(1)));
  EXPECT_EQ(1024, (convert<pebibytes, tebibytes>(1)));
  EXPECT_EQ(1024, (convert<exbibytes, pebibytes>(1)));

  EXPECT_EQ(93750000, (convert<gigabytes, kibibits>(12)));

  EXPECT_EQ(1000, (convert<kilobits, bits>(1)));
  EXPECT_EQ(1000, (convert<megabits, kilobits>(1)));
  EXPECT_EQ(1000, (convert<gigabits, megabits>(1)));
  EXPECT_EQ(1000, (convert<terabits, gigabits>(1)));
  EXPECT_EQ(1000, (convert<petabits, terabits>(1)));
  EXPECT_EQ(1000, (convert<exabits, petabits>(1)));

  EXPECT_EQ(1024, (convert<kibibits, bits>(1)));
  EXPECT_EQ(1024, (convert<mebibits, kibibits>(1)));
  EXPECT_EQ(1024, (convert<gibibits, mebibits>(1)));
  EXPECT_EQ(1024, (convert<tebibits, gibibits>(1)));
  EXPECT_EQ(1024, (convert<pebibits, tebibits>(1)));
  EXPECT_EQ(1024, (convert<exbibits, pebibits>(1)));

  // Source: https://en.wikipedia.org/wiki/Binary_prefix
  EXPECT_NEAR(percent_t(2.4), kibibyte_t(1) / kilobyte_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(4.9), mebibyte_t(1) / megabyte_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(7.4), gibibyte_t(1) / gigabyte_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(10.0), tebibyte_t(1) / terabyte_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(12.6), pebibyte_t(1) / petabyte_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(15.3), exbibyte_t(1) / exabyte_t(1) - 1, 0.005);
}

TEST_F(UnitConversion, data_transfer_rate) {
  EXPECT_EQ(8, (convert<bytes_per_second, bits_per_second>(1)));

  EXPECT_EQ(1000, (convert<kilobytes_per_second, bytes_per_second>(1)));
  EXPECT_EQ(1000, (convert<megabytes_per_second, kilobytes_per_second>(1)));
  EXPECT_EQ(1000, (convert<gigabytes_per_second, megabytes_per_second>(1)));
  EXPECT_EQ(1000, (convert<terabytes_per_second, gigabytes_per_second>(1)));
  EXPECT_EQ(1000, (convert<petabytes_per_second, terabytes_per_second>(1)));
  EXPECT_EQ(1000, (convert<exabytes_per_second, petabytes_per_second>(1)));

  EXPECT_EQ(1024, (convert<kibibytes_per_second, bytes_per_second>(1)));
  EXPECT_EQ(1024, (convert<mebibytes_per_second, kibibytes_per_second>(1)));
  EXPECT_EQ(1024, (convert<gibibytes_per_second, mebibytes_per_second>(1)));
  EXPECT_EQ(1024, (convert<tebibytes_per_second, gibibytes_per_second>(1)));
  EXPECT_EQ(1024, (convert<pebibytes_per_second, tebibytes_per_second>(1)));
  EXPECT_EQ(1024, (convert<exbibytes_per_second, pebibytes_per_second>(1)));

  EXPECT_EQ(93750000, (convert<gigabytes_per_second, kibibits_per_second>(12)));

  EXPECT_EQ(1000, (convert<kilobits_per_second, bits_per_second>(1)));
  EXPECT_EQ(1000, (convert<megabits_per_second, kilobits_per_second>(1)));
  EXPECT_EQ(1000, (convert<gigabits_per_second, megabits_per_second>(1)));
  EXPECT_EQ(1000, (convert<terabits_per_second, gigabits_per_second>(1)));
  EXPECT_EQ(1000, (convert<petabits_per_second, terabits_per_second>(1)));
  EXPECT_EQ(1000, (convert<exabits_per_second, petabits_per_second>(1)));

  EXPECT_EQ(1024, (convert<kibibits_per_second, bits_per_second>(1)));
  EXPECT_EQ(1024, (convert<mebibits_per_second, kibibits_per_second>(1)));
  EXPECT_EQ(1024, (convert<gibibits_per_second, mebibits_per_second>(1)));
  EXPECT_EQ(1024, (convert<tebibits_per_second, gibibits_per_second>(1)));
  EXPECT_EQ(1024, (convert<pebibits_per_second, tebibits_per_second>(1)));
  EXPECT_EQ(1024, (convert<exbibits_per_second, pebibits_per_second>(1)));

  // Source: https://en.wikipedia.org/wiki/Binary_prefix
  EXPECT_NEAR(percent_t(2.4),
              kibibytes_per_second_t(1) / kilobytes_per_second_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(4.9),
              mebibytes_per_second_t(1) / megabytes_per_second_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(7.4),
              gibibytes_per_second_t(1) / gigabytes_per_second_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(10.0),
              tebibytes_per_second_t(1) / terabytes_per_second_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(12.6),
              pebibytes_per_second_t(1) / petabytes_per_second_t(1) - 1, 0.005);
  EXPECT_NEAR(percent_t(15.3),
              exbibytes_per_second_t(1) / exabytes_per_second_t(1) - 1, 0.005);
}

TEST_F(UnitConversion, pi) {
  EXPECT_TRUE(
      units::traits::is_dimensionless_unit<decltype(constants::pi)>::value);
  EXPECT_TRUE(units::traits::is_dimensionless_unit<constants::PI>::value);

  // implicit conversion/arithmetic
  EXPECT_NEAR(3.14159, constants::pi, 5.0e-6);
  EXPECT_NEAR(6.28318531, (2 * constants::pi), 5.0e-9);
  EXPECT_NEAR(6.28318531, (constants::pi + constants::pi), 5.0e-9);
  EXPECT_NEAR(0.0, (constants::pi - constants::pi), 5.0e-9);
  EXPECT_NEAR(31.00627668, units::math::cpow<3>(constants::pi), 5.0e-10);
  EXPECT_NEAR(0.0322515344, (1.0 / units::math::cpow<3>(constants::pi)),
              5.0e-11);
  EXPECT_TRUE(constants::detail::PI_VAL == constants::pi);
  EXPECT_TRUE(1.0 != constants::pi);
  EXPECT_TRUE(4.0 > constants::pi);
  EXPECT_TRUE(3.0 < constants::pi);
  EXPECT_TRUE(constants::pi > 3.0);
  EXPECT_TRUE(constants::pi < 4.0);

  // explicit conversion
  EXPECT_NEAR(3.14159, constants::pi.to<double>(), 5.0e-6);

  // auto multiplication
  EXPECT_TRUE(
      (std::is_same<meter_t, decltype(constants::pi * meter_t(1))>::value));
  EXPECT_TRUE(
      (std::is_same<meter_t, decltype(meter_t(1) * constants::pi)>::value));

  EXPECT_NEAR(constants::detail::PI_VAL,
              (constants::pi * meter_t(1)).to<double>(), 5.0e-10);
  EXPECT_NEAR(constants::detail::PI_VAL,
              (meter_t(1) * constants::pi).to<double>(), 5.0e-10);

  // explicit multiplication
  meter_t a = constants::pi * meter_t(1);
  meter_t b = meter_t(1) * constants::pi;

  EXPECT_NEAR(constants::detail::PI_VAL, a.to<double>(), 5.0e-10);
  EXPECT_NEAR(constants::detail::PI_VAL, b.to<double>(), 5.0e-10);

  // auto division
  EXPECT_TRUE(
      (std::is_same<hertz_t, decltype(constants::pi / second_t(1))>::value));
  EXPECT_TRUE(
      (std::is_same<second_t, decltype(second_t(1) / constants::pi)>::value));

  EXPECT_NEAR(constants::detail::PI_VAL,
              (constants::pi / second_t(1)).to<double>(), 5.0e-10);
  EXPECT_NEAR(1.0 / constants::detail::PI_VAL,
              (second_t(1) / constants::pi).to<double>(), 5.0e-10);

  // explicit
  hertz_t c = constants::pi / second_t(1);
  second_t d = second_t(1) / constants::pi;

  EXPECT_NEAR(constants::detail::PI_VAL, c.to<double>(), 5.0e-10);
  EXPECT_NEAR(1.0 / constants::detail::PI_VAL, d.to<double>(), 5.0e-10);
}

TEST_F(UnitConversion, constants) {
  // Source: NIST "2014 CODATA recommended values"
  EXPECT_NEAR(299792458, constants::c(), 5.0e-9);
  EXPECT_NEAR(6.67408e-11, constants::G(), 5.0e-17);
  EXPECT_NEAR(6.626070040e-34, constants::h(), 5.0e-44);
  EXPECT_NEAR(1.2566370614e-6, constants::mu0(), 5.0e-17);
  EXPECT_NEAR(8.854187817e-12, constants::epsilon0(), 5.0e-21);
  EXPECT_NEAR(376.73031346177, constants::Z0(), 5.0e-12);
  EXPECT_NEAR(8987551787.3681764, constants::k_e(), 5.0e-6);
  EXPECT_NEAR(1.6021766208e-19, constants::e(), 5.0e-29);
  EXPECT_NEAR(9.10938356e-31, constants::m_e(), 5.0e-40);
  EXPECT_NEAR(1.672621898e-27, constants::m_p(), 5.0e-37);
  EXPECT_NEAR(9.274009994e-24, constants::mu_B(), 5.0e-32);
  EXPECT_NEAR(6.022140857e23, constants::N_A(), 5.0e14);
  EXPECT_NEAR(8.3144598, constants::R(), 5.0e-8);
  EXPECT_NEAR(1.38064852e-23, constants::k_B(), 5.0e-31);
  EXPECT_NEAR(96485.33289, constants::F(), 5.0e-5);
  EXPECT_NEAR(5.670367e-8, constants::sigma(), 5.0e-14);
}

TEST_F(UnitConversion, std_chrono) {
  nanosecond_t a = std::chrono::nanoseconds(10);
  EXPECT_EQ(nanosecond_t(10), a);
  microsecond_t b = std::chrono::microseconds(10);
  EXPECT_EQ(microsecond_t(10), b);
  millisecond_t c = std::chrono::milliseconds(10);
  EXPECT_EQ(millisecond_t(10), c);
  second_t d = std::chrono::seconds(1);
  EXPECT_EQ(second_t(1), d);
  minute_t e = std::chrono::minutes(120);
  EXPECT_EQ(minute_t(120), e);
  hour_t f = std::chrono::hours(2);
  EXPECT_EQ(hour_t(2), f);

  std::chrono::nanoseconds g = nanosecond_t(100);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(g).count(),
            100);
  std::chrono::nanoseconds h = microsecond_t(2);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(h).count(),
            2000);
  std::chrono::nanoseconds i = millisecond_t(1);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(i).count(),
            1000000);
  std::chrono::nanoseconds j = second_t(1);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(j).count(),
            1000000000);
  std::chrono::nanoseconds k = minute_t(1);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(k).count(),
            60000000000);
  std::chrono::nanoseconds l = hour_t(1);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(l).count(),
            3600000000000);
}

TEST_F(UnitConversion, squaredTemperature) {
  using squared_celsius = units::compound_unit<squared<celsius>>;
  using squared_celsius_t = units::unit_t<squared_celsius>;
  const squared_celsius_t right(100);
  const celsius_t rootRight = units::math::sqrt(right);
  EXPECT_EQ(celsius_t(10), rootRight);
}

TEST_F(UnitMath, min) {
  meter_t a(1);
  foot_t c(1);
  EXPECT_EQ(c, math::min(a, c));
}

TEST_F(UnitMath, max) {
  meter_t a(1);
  foot_t c(1);
  EXPECT_EQ(a, math::max(a, c));
}

TEST_F(UnitMath, cos) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                cos(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(-0.41614683654), cos(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(-0.70710678118), cos(angle::degree_t(135)),
              5.0e-11);
}

TEST_F(UnitMath, sin) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                sin(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(0.90929742682), sin(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(0.70710678118), sin(angle::degree_t(135)), 5.0e-11);
}

TEST_F(UnitMath, tan) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                tan(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(-2.18503986326), tan(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(-1.0), tan(angle::degree_t(135)), 5.0e-11);
}

TEST_F(UnitMath, acos) {
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<angle::radian_t>::type,
          typename std::decay<decltype(acos(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(2).to<double>(),
              acos(scalar_t(-0.41614683654)).to<double>(), 5.0e-11);
  EXPECT_NEAR(
      angle::degree_t(135).to<double>(),
      angle::degree_t(acos(scalar_t(-0.70710678118654752440084436210485)))
          .to<double>(),
      5.0e-12);
}

TEST_F(UnitMath, asin) {
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<angle::radian_t>::type,
          typename std::decay<decltype(asin(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(1.14159265).to<double>(),
              asin(scalar_t(0.90929742682)).to<double>(), 5.0e-9);
  EXPECT_NEAR(
      angle::degree_t(45).to<double>(),
      angle::degree_t(asin(scalar_t(0.70710678118654752440084436210485)))
          .to<double>(),
      5.0e-12);
}

TEST_F(UnitMath, atan) {
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<angle::radian_t>::type,
          typename std::decay<decltype(atan(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(-1.14159265).to<double>(),
              atan(scalar_t(-2.18503986326)).to<double>(), 5.0e-9);
  EXPECT_NEAR(angle::degree_t(-45).to<double>(),
              angle::degree_t(atan(scalar_t(-1.0))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan2) {
  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(atan2(
                                scalar_t(1), scalar_t(1)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(constants::detail::PI_VAL / 4).to<double>(),
              atan2(scalar_t(2), scalar_t(2)).to<double>(), 5.0e-12);
  EXPECT_NEAR(
      angle::degree_t(45).to<double>(),
      angle::degree_t(atan2(scalar_t(2), scalar_t(2))).to<double>(),
      5.0e-12);

  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(atan2(
                                scalar_t(1), scalar_t(1)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(constants::detail::PI_VAL / 6).to<double>(),
              atan2(scalar_t(1), scalar_t(std::sqrt(3))).to<double>(),
              5.0e-12);
  EXPECT_NEAR(angle::degree_t(30).to<double>(),
              angle::degree_t(atan2(scalar_t(1), scalar_t(std::sqrt(3))))
                  .to<double>(),
              5.0e-12);
}

TEST_F(UnitMath, cosh) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                cosh(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(3.76219569108), cosh(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(5.32275215), cosh(angle::degree_t(135)), 5.0e-9);
}

TEST_F(UnitMath, sinh) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                sinh(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(3.62686040785), sinh(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(5.22797192), sinh(angle::degree_t(135)), 5.0e-9);
}

TEST_F(UnitMath, tanh) {
  EXPECT_TRUE((std::is_same<typename std::decay<scalar_t>::type,
                            typename std::decay<decltype(
                                tanh(angle::radian_t(0)))>::type>::value));
  EXPECT_NEAR(scalar_t(0.96402758007), tanh(angle::radian_t(2)), 5.0e-11);
  EXPECT_NEAR(scalar_t(0.98219338), tanh(angle::degree_t(135)), 5.0e-11);
}

TEST_F(UnitMath, acosh) {
  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(
                                acosh(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(1.316957896924817).to<double>(),
              acosh(scalar_t(2.0)).to<double>(), 5.0e-11);
  EXPECT_NEAR(angle::degree_t(75.456129290216893).to<double>(),
              angle::degree_t(acosh(scalar_t(2.0))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asinh) {
  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(
                                asinh(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(1.443635475178810).to<double>(),
              asinh(scalar_t(2)).to<double>(), 5.0e-9);
  EXPECT_NEAR(angle::degree_t(82.714219883108939).to<double>(),
              angle::degree_t(asinh(scalar_t(2))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atanh) {
  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(
                                atanh(scalar_t(0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(0.549306144334055).to<double>(),
              atanh(scalar_t(0.5)).to<double>(), 5.0e-9);
  EXPECT_NEAR(angle::degree_t(31.472923730945389).to<double>(),
              angle::degree_t(atanh(scalar_t(0.5))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, exp) {
  double val = 10.0;
  EXPECT_EQ(std::exp(val), exp(scalar_t(val)));
}

TEST_F(UnitMath, log) {
  double val = 100.0;
  EXPECT_EQ(std::log(val), log(scalar_t(val)));
}

TEST_F(UnitMath, log10) {
  double val = 100.0;
  EXPECT_EQ(std::log10(val), log10(scalar_t(val)));
}

TEST_F(UnitMath, modf) {
  double val = 100.0;
  double modfr1;
  scalar_t modfr2;
  EXPECT_EQ(std::modf(val, &modfr1), modf(scalar_t(val), &modfr2));
  EXPECT_EQ(modfr1, modfr2);
}

TEST_F(UnitMath, exp2) {
  double val = 10.0;
  EXPECT_EQ(std::exp2(val), exp2(scalar_t(val)));
}

TEST_F(UnitMath, expm1) {
  double val = 10.0;
  EXPECT_EQ(std::expm1(val), expm1(scalar_t(val)));
}

TEST_F(UnitMath, log1p) {
  double val = 10.0;
  EXPECT_EQ(std::log1p(val), log1p(scalar_t(val)));
}

TEST_F(UnitMath, log2) {
  double val = 10.0;
  EXPECT_EQ(std::log2(val), log2(scalar_t(val)));
}

TEST_F(UnitMath, pow) {
  bool isSame;
  meter_t value(10.0);

  auto sq = pow<2>(value);
  EXPECT_NEAR(100.0, sq(), 5.0e-2);
  isSame = std::is_same<decltype(sq), square_meter_t>::value;
  EXPECT_TRUE(isSame);

  auto cube = pow<3>(value);
  EXPECT_NEAR(1000.0, cube(), 5.0e-2);
  isSame = std::is_same<decltype(cube), unit_t<cubed<meter>>>::value;
  EXPECT_TRUE(isSame);

  auto fourth = pow<4>(value);
  EXPECT_NEAR(10000.0, fourth(), 5.0e-2);
  isSame = std::is_same<
      decltype(fourth),
      unit_t<compound_unit<squared<meter>, squared<meter>>>>::value;
  EXPECT_TRUE(isSame);
}

TEST_F(UnitMath, sqrt) {
  EXPECT_TRUE((std::is_same<typename std::decay<meter_t>::type,
                            typename std::decay<decltype(sqrt(
                                square_meter_t(4.0)))>::type>::value));
  EXPECT_NEAR(meter_t(2.0).to<double>(),
              sqrt(square_meter_t(4.0)).to<double>(), 5.0e-9);

  EXPECT_TRUE((std::is_same<typename std::decay<angle::radian_t>::type,
                            typename std::decay<decltype(
                                sqrt(steradian_t(16.0)))>::type>::value));
  EXPECT_NEAR(angle::radian_t(4.0).to<double>(),
              sqrt(steradian_t(16.0)).to<double>(), 5.0e-9);

  EXPECT_TRUE((std::is_convertible<typename std::decay<foot_t>::type,
                                   typename std::decay<decltype(sqrt(
                                       square_foot_t(10.0)))>::type>::value));

  // for rational conversion (i.e. no integral root) let's check a bunch of
  // different ways this could go wrong
  foot_t resultFt = sqrt(square_foot_t(10.0));
  EXPECT_NEAR(foot_t(3.16227766017).to<double>(),
              sqrt(square_foot_t(10.0)).to<double>(), 5.0e-9);
  EXPECT_NEAR(foot_t(3.16227766017).to<double>(), resultFt.to<double>(),
              5.0e-9);
  EXPECT_EQ(resultFt, sqrt(square_foot_t(10.0)));
}

TEST_F(UnitMath, hypot) {
  EXPECT_TRUE((std::is_same<typename std::decay<meter_t>::type,
                            typename std::decay<decltype(hypot(
                                meter_t(3.0), meter_t(4.0)))>::type>::value));
  EXPECT_NEAR(meter_t(5.0).to<double>(),
              (hypot(meter_t(3.0), meter_t(4.0))).to<double>(), 5.0e-9);

  EXPECT_TRUE((std::is_same<typename std::decay<foot_t>::type,
                            typename std::decay<decltype(hypot(
                                foot_t(3.0), meter_t(1.2192)))>::type>::value));
  EXPECT_NEAR(foot_t(5.0).to<double>(),
              (hypot(foot_t(3.0), meter_t(1.2192))).to<double>(), 5.0e-9);
}

TEST_F(UnitMath, ceil) {
  double val = 101.1;
  EXPECT_EQ(std::ceil(val), ceil(meter_t(val)).to<double>());
  EXPECT_TRUE((std::is_same<typename std::decay<meter_t>::type,
                            typename std::decay<decltype(
                                ceil(meter_t(val)))>::type>::value));
}

TEST_F(UnitMath, floor) {
  double val = 101.1;
  EXPECT_EQ(std::floor(val), floor(scalar_t(val)));
}

TEST_F(UnitMath, fmod) {
  EXPECT_EQ(std::fmod(100.0, 101.2),
            fmod(meter_t(100.0), meter_t(101.2)).to<double>());
}

TEST_F(UnitMath, trunc) {
  double val = 101.1;
  EXPECT_EQ(std::trunc(val), trunc(scalar_t(val)));
}

TEST_F(UnitMath, round) {
  double val = 101.1;
  EXPECT_EQ(std::round(val), round(scalar_t(val)));
}

TEST_F(UnitMath, copysign) {
  double sign = -1;
  meter_t val(5.0);
  EXPECT_EQ(meter_t(-5.0), copysign(val, sign));
  EXPECT_EQ(meter_t(-5.0), copysign(val, angle::radian_t(sign)));
}

TEST_F(UnitMath, fdim) {
  EXPECT_EQ(meter_t(0.0), fdim(meter_t(8.0), meter_t(10.0)));
  EXPECT_EQ(meter_t(2.0), fdim(meter_t(10.0), meter_t(8.0)));
  EXPECT_NEAR(meter_t(9.3904).to<double>(),
              fdim(meter_t(10.0), foot_t(2.0)).to<double>(),
              5.0e-320);  // not sure why they aren't comparing exactly equal,
                          // but clearly they are.
}

TEST_F(UnitMath, fmin) {
  EXPECT_EQ(meter_t(8.0), fmin(meter_t(8.0), meter_t(10.0)));
  EXPECT_EQ(meter_t(8.0), fmin(meter_t(10.0), meter_t(8.0)));
  EXPECT_EQ(foot_t(2.0), fmin(meter_t(10.0), foot_t(2.0)));
}

TEST_F(UnitMath, fmax) {
  EXPECT_EQ(meter_t(10.0), fmax(meter_t(8.0), meter_t(10.0)));
  EXPECT_EQ(meter_t(10.0), fmax(meter_t(10.0), meter_t(8.0)));
  EXPECT_EQ(meter_t(10.0), fmax(meter_t(10.0), foot_t(2.0)));
}

TEST_F(UnitMath, fabs) {
  EXPECT_EQ(meter_t(10.0), fabs(meter_t(-10.0)));
  EXPECT_EQ(meter_t(10.0), fabs(meter_t(10.0)));
}

TEST_F(UnitMath, abs) {
  EXPECT_EQ(meter_t(10.0), abs(meter_t(-10.0)));
  EXPECT_EQ(meter_t(10.0), abs(meter_t(10.0)));
}

TEST_F(UnitMath, fma) {
  meter_t x(2.0);
  meter_t y(3.0);
  square_meter_t z(1.0);
  EXPECT_EQ(square_meter_t(7.0), fma(x, y, z));
}

// Constexpr
#if !defined(_MSC_VER) || _MSC_VER > 1800
TEST_F(Constexpr, construction) {
  constexpr meter_t result0(0);
  constexpr auto result1 = make_unit<meter_t>(1);
  constexpr auto result2 = meter_t(2);

  EXPECT_EQ(meter_t(0), result0);
  EXPECT_EQ(meter_t(1), result1);
  EXPECT_EQ(meter_t(2), result2);

  EXPECT_TRUE(noexcept(result0));
  EXPECT_TRUE(noexcept(result1));
  EXPECT_TRUE(noexcept(result2));
}

TEST_F(Constexpr, constants) {
  EXPECT_TRUE(noexcept(constants::c()));
  EXPECT_TRUE(noexcept(constants::G()));
  EXPECT_TRUE(noexcept(constants::h()));
  EXPECT_TRUE(noexcept(constants::mu0()));
  EXPECT_TRUE(noexcept(constants::epsilon0()));
  EXPECT_TRUE(noexcept(constants::Z0()));
  EXPECT_TRUE(noexcept(constants::k_e()));
  EXPECT_TRUE(noexcept(constants::e()));
  EXPECT_TRUE(noexcept(constants::m_e()));
  EXPECT_TRUE(noexcept(constants::m_p()));
  EXPECT_TRUE(noexcept(constants::mu_B()));
  EXPECT_TRUE(noexcept(constants::N_A()));
  EXPECT_TRUE(noexcept(constants::R()));
  EXPECT_TRUE(noexcept(constants::k_B()));
  EXPECT_TRUE(noexcept(constants::F()));
  EXPECT_TRUE(noexcept(constants::sigma()));
}

TEST_F(Constexpr, arithmetic) {
  constexpr auto result0(1_m + 1_m);
  constexpr auto result1(1_m - 1_m);
  constexpr auto result2(1_m * 1_m);
  constexpr auto result3(1_m / 1_m);
  constexpr auto result4(meter_t(1) + meter_t(1));
  constexpr auto result5(meter_t(1) - meter_t(1));
  constexpr auto result6(meter_t(1) * meter_t(1));
  constexpr auto result7(meter_t(1) / meter_t(1));
  constexpr auto result8(units::math::cpow<2>(meter_t(2)));
  constexpr auto result9 = units::math::cpow<3>(2_m);
  constexpr auto result10 = 2_m * 2_m;

  EXPECT_TRUE(noexcept(result0));
  EXPECT_TRUE(noexcept(result1));
  EXPECT_TRUE(noexcept(result2));
  EXPECT_TRUE(noexcept(result3));
  EXPECT_TRUE(noexcept(result4));
  EXPECT_TRUE(noexcept(result5));
  EXPECT_TRUE(noexcept(result6));
  EXPECT_TRUE(noexcept(result7));
  EXPECT_TRUE(noexcept(result8));
  EXPECT_TRUE(noexcept(result9));
  EXPECT_TRUE(noexcept(result10));

  EXPECT_EQ(8_cu_m, result9);
  EXPECT_EQ(4_sq_m, result10);
}

TEST_F(Constexpr, realtional) {
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

  EXPECT_TRUE(equalityTrue);
  EXPECT_TRUE(lessThanTrue);
  EXPECT_TRUE(lessThanEqualTrue1);
  EXPECT_TRUE(lessThanEqualTrue2);
  EXPECT_TRUE(greaterThanTrue);
  EXPECT_TRUE(greaterThanEqualTrue1);
  EXPECT_TRUE(greaterThanEqualTrue2);
  EXPECT_FALSE(equalityFalse);
  EXPECT_FALSE(lessThanFalse);
  EXPECT_FALSE(lessThanEqualFalse);
  EXPECT_FALSE(greaterThanFalse);
  EXPECT_FALSE(greaterThanEqualFalse);
}

TEST_F(Constexpr, stdArray) {
  constexpr std::array<meter_t, 5> arr = {0_m, 1_m, 2_m, 3_m, 4_m};
  constexpr bool equal = (arr[3] == 3_m);
  EXPECT_TRUE(equal);
}

#endif

TEST_F(CompileTimeArithmetic, unit_value_t) {
  typedef unit_value_t<meters, 3, 2> mRatio;
  EXPECT_EQ(meter_t(1.5), mRatio::value());
}

TEST_F(CompileTimeArithmetic, is_unit_value_t) {
  typedef unit_value_t<meters, 3, 2> mRatio;

  EXPECT_TRUE((traits::is_unit_value_t<mRatio>::value));
  EXPECT_FALSE((traits::is_unit_value_t<meter_t>::value));
  EXPECT_FALSE((traits::is_unit_value_t<double>::value));

  EXPECT_TRUE((traits::is_unit_value_t<mRatio, meters>::value));
  EXPECT_FALSE((traits::is_unit_value_t<meter_t, meters>::value));
  EXPECT_FALSE((traits::is_unit_value_t<double, meters>::value));
}

TEST_F(CompileTimeArithmetic, is_unit_value_t_category) {
  typedef unit_value_t<feet, 3, 2> mRatio;
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, mRatio>::value));
  EXPECT_FALSE(
      (traits::is_unit_value_t_category<category::angle_unit, mRatio>::value));
  EXPECT_FALSE((
      traits::is_unit_value_t_category<category::length_unit, meter_t>::value));
  EXPECT_FALSE(
      (traits::is_unit_value_t_category<category::length_unit, double>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_add) {
  typedef unit_value_t<meters, 3, 2> mRatio;

  using sum = unit_value_add<mRatio, mRatio>;
  EXPECT_EQ(meter_t(3.0), sum::value());
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, sum>::value));

  typedef unit_value_t<feet, 1> ftRatio;

  using sumf = unit_value_add<ftRatio, mRatio>;
  EXPECT_TRUE((
      std::is_same<typename std::decay<foot_t>::type,
                   typename std::decay<decltype(sumf::value())>::type>::value));
  EXPECT_NEAR(5.92125984, sumf::value().to<double>(), 5.0e-8);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, sumf>::value));

  typedef unit_value_t<celsius, 1> cRatio;
  typedef unit_value_t<fahrenheit, 2> fRatio;

  using sumc = unit_value_add<cRatio, fRatio>;
  EXPECT_TRUE((
      std::is_same<typename std::decay<celsius_t>::type,
                   typename std::decay<decltype(sumc::value())>::type>::value));
  EXPECT_NEAR(2.11111111111, sumc::value().to<double>(), 5.0e-8);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::temperature_unit,
                                                sumc>::value));

  typedef unit_value_t<angle::radian, 1> rRatio;
  typedef unit_value_t<angle::degree, 3> dRatio;

  using sumr = unit_value_add<rRatio, dRatio>;
  EXPECT_TRUE((
      std::is_same<typename std::decay<angle::radian_t>::type,
                   typename std::decay<decltype(sumr::value())>::type>::value));
  EXPECT_NEAR(1.05235988, sumr::value().to<double>(), 5.0e-8);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::angle_unit, sumr>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_subtract) {
  typedef unit_value_t<meters, 3, 2> mRatio;

  using diff = unit_value_subtract<mRatio, mRatio>;
  EXPECT_EQ(meter_t(0), diff::value());
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, diff>::value));

  typedef unit_value_t<feet, 1> ftRatio;

  using difff = unit_value_subtract<ftRatio, mRatio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<foot_t>::type,
               typename std::decay<decltype(difff::value())>::type>::value));
  EXPECT_NEAR(-3.92125984, difff::value().to<double>(), 5.0e-8);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, difff>::value));

  typedef unit_value_t<celsius, 1> cRatio;
  typedef unit_value_t<fahrenheit, 2> fRatio;

  using diffc = unit_value_subtract<cRatio, fRatio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<celsius_t>::type,
               typename std::decay<decltype(diffc::value())>::type>::value));
  EXPECT_NEAR(-0.11111111111, diffc::value().to<double>(), 5.0e-8);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::temperature_unit,
                                                diffc>::value));

  typedef unit_value_t<angle::radian, 1> rRatio;
  typedef unit_value_t<angle::degree, 3> dRatio;

  using diffr = unit_value_subtract<rRatio, dRatio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<angle::radian_t>::type,
               typename std::decay<decltype(diffr::value())>::type>::value));
  EXPECT_NEAR(0.947640122, diffr::value().to<double>(), 5.0e-8);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::angle_unit, diffr>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_multiply) {
  typedef unit_value_t<meters, 2> mRatio;
  typedef unit_value_t<feet, 656168, 100000> ftRatio;  // 2 meter

  using product = unit_value_multiply<mRatio, mRatio>;
  EXPECT_EQ(square_meter_t(4), product::value());
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::area_unit, product>::value));

  using productM = unit_value_multiply<mRatio, ftRatio>;

  EXPECT_TRUE((std::is_same<
               typename std::decay<square_meter_t>::type,
               typename std::decay<decltype(productM::value())>::type>::value));
  EXPECT_NEAR(4.0, productM::value().to<double>(), 5.0e-7);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::area_unit, productM>::value));

  using productF = unit_value_multiply<ftRatio, mRatio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<square_foot_t>::type,
               typename std::decay<decltype(productF::value())>::type>::value));
  EXPECT_NEAR(43.0556444224, productF::value().to<double>(), 5.0e-6);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::area_unit, productF>::value));

  using productF2 = unit_value_multiply<ftRatio, ftRatio>;
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<square_foot_t>::type,
          typename std::decay<decltype(productF2::value())>::type>::value));
  EXPECT_NEAR(43.0556444224, productF2::value().to<double>(), 5.0e-8);
  EXPECT_TRUE((
      traits::is_unit_value_t_category<category::area_unit, productF2>::value));

  typedef unit_value_t<units::force::newton, 5> nRatio;

  using productN = unit_value_multiply<nRatio, ftRatio>;
  EXPECT_FALSE(
      (std::is_same<
          typename std::decay<torque::newton_meter_t>::type,
          typename std::decay<decltype(productN::value())>::type>::value));
  EXPECT_TRUE((std::is_convertible<
               typename std::decay<torque::newton_meter_t>::type,
               typename std::decay<decltype(productN::value())>::type>::value));
  EXPECT_NEAR(32.8084, productN::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(10.0, (productN::value().convert<newton_meter>().to<double>()),
              5.0e-7);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::torque_unit,
                                                productN>::value));

  typedef unit_value_t<angle::radian, 11, 10> r1Ratio;
  typedef unit_value_t<angle::radian, 22, 10> r2Ratio;

  using productR = unit_value_multiply<r1Ratio, r2Ratio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<steradian_t>::type,
               typename std::decay<decltype(productR::value())>::type>::value));
  EXPECT_NEAR(2.42, productR::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(7944.39137,
              (productR::value().convert<degrees_squared>().to<double>()),
              5.0e-6);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::solid_angle_unit,
                                                productR>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_divide) {
  typedef unit_value_t<meters, 2> mRatio;
  typedef unit_value_t<feet, 656168, 100000> ftRatio;  // 2 meter

  using product = unit_value_divide<mRatio, mRatio>;
  EXPECT_EQ(scalar_t(1), product::value());
  EXPECT_TRUE((
      traits::is_unit_value_t_category<category::scalar_unit, product>::value));

  using productM = unit_value_divide<mRatio, ftRatio>;

  EXPECT_TRUE((std::is_same<
               typename std::decay<scalar_t>::type,
               typename std::decay<decltype(productM::value())>::type>::value));
  EXPECT_NEAR(1, productM::value().to<double>(), 5.0e-7);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::scalar_unit,
                                                productM>::value));

  using productF = unit_value_divide<ftRatio, mRatio>;
  EXPECT_TRUE((std::is_same<
               typename std::decay<scalar_t>::type,
               typename std::decay<decltype(productF::value())>::type>::value));
  EXPECT_NEAR(1.0, productF::value().to<double>(), 5.0e-6);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::scalar_unit,
                                                productF>::value));

  using productF2 = unit_value_divide<ftRatio, ftRatio>;
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<scalar_t>::type,
          typename std::decay<decltype(productF2::value())>::type>::value));
  EXPECT_NEAR(1.0, productF2::value().to<double>(), 5.0e-8);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::scalar_unit,
                                                productF2>::value));

  typedef unit_value_t<seconds, 10> sRatio;

  using productMS = unit_value_divide<mRatio, sRatio>;
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<meters_per_second_t>::type,
          typename std::decay<decltype(productMS::value())>::type>::value));
  EXPECT_NEAR(0.2, productMS::value().to<double>(), 5.0e-8);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::velocity_unit,
                                                productMS>::value));

  typedef unit_value_t<angle::radian, 20> rRatio;

  using productRS = unit_value_divide<rRatio, sRatio>;
  EXPECT_TRUE(
      (std::is_same<
          typename std::decay<radians_per_second_t>::type,
          typename std::decay<decltype(productRS::value())>::type>::value));
  EXPECT_NEAR(2, productRS::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(114.592,
              (productRS::value().convert<degrees_per_second>().to<double>()),
              5.0e-4);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::angular_velocity_unit,
                                                productRS>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_power) {
  typedef unit_value_t<meters, 2> mRatio;

  using sq = unit_value_power<mRatio, 2>;
  EXPECT_TRUE((std::is_convertible<
               typename std::decay<square_meter_t>::type,
               typename std::decay<decltype(sq::value())>::type>::value));
  EXPECT_NEAR(4, sq::value().to<double>(), 5.0e-8);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::area_unit, sq>::value));

  typedef unit_value_t<angle::radian, 18, 10> rRatio;

  using sqr = unit_value_power<rRatio, 2>;
  EXPECT_TRUE((std::is_convertible<
               typename std::decay<steradian_t>::type,
               typename std::decay<decltype(sqr::value())>::type>::value));
  EXPECT_NEAR(3.24, sqr::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(10636.292574038049895092690529904,
              (sqr::value().convert<degrees_squared>().to<double>()), 5.0e-10);
  EXPECT_TRUE((traits::is_unit_value_t_category<category::solid_angle_unit,
                                                sqr>::value));
}

TEST_F(CompileTimeArithmetic, unit_value_sqrt) {
  typedef unit_value_t<square_meters, 10> mRatio;

  using root = unit_value_sqrt<mRatio>;
  EXPECT_TRUE((std::is_convertible<
               typename std::decay<meter_t>::type,
               typename std::decay<decltype(root::value())>::type>::value));
  EXPECT_NEAR(3.16227766017, root::value().to<double>(), 5.0e-9);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, root>::value));

  typedef unit_value_t<hectare, 51, 7> hRatio;

  using rooth = unit_value_sqrt<hRatio, 100000000>;
  EXPECT_TRUE((std::is_convertible<
               typename std::decay<mile_t>::type,
               typename std::decay<decltype(rooth::value())>::type>::value));
  EXPECT_NEAR(2.69920623253, rooth::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(269.920623, rooth::value().convert<meters>().to<double>(),
              5.0e-6);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::length_unit, rooth>::value));

  typedef unit_value_t<steradian, 18, 10> rRatio;

  using rootr = unit_value_sqrt<rRatio>;
  EXPECT_TRUE((traits::is_angle_unit<decltype(rootr::value())>::value));
  EXPECT_NEAR(1.3416407865, rootr::value().to<double>(), 5.0e-8);
  EXPECT_NEAR(76.870352574,
              rootr::value().convert<angle::degrees>().to<double>(), 5.0e-6);
  EXPECT_TRUE(
      (traits::is_unit_value_t_category<category::angle_unit, rootr>::value));
}

TEST_F(CaseStudies, radarRangeEquation) {
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

  EXPECT_NEAR(1.535, SNR(), 5.0e-4);
}

TEST_F(CaseStudies, pythagoreanTheorum) {
  EXPECT_EQ(meter_t(3), RightTriangle::a::value());
  EXPECT_EQ(meter_t(4), RightTriangle::b::value());
  EXPECT_EQ(meter_t(5), RightTriangle::c::value());
  EXPECT_TRUE(pow<2>(RightTriangle::a::value()) +
                  pow<2>(RightTriangle::b::value()) ==
              pow<2>(RightTriangle::c::value()));
}
