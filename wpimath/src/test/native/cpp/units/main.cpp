#ifdef _MSC_VER
#pragma warning(disable : 4244) // Disable 'conversion from double to const int, possible loss of data'. The tests just make sure
// that such assignments work as expected, we don't want to remove them and we don't care about the warning.
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING // officially, The effect of instantiating the template std::complex for any type other than float,
// double, or long double is unspecified. We don't care though, we want them to work with units in this
// test
#endif

#define UNIT_LIB_FORCE_IOSTREAM
#include <array>
#include <atomic>
#include <chrono>
#include <cfloat>
#include <compare>
#include <complex>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <ratio>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <wpi/units.hpp>
#include <unordered_map>
// #include <wpi/units/serialization.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
// Disable all tests that try to use iostream, as Catch2 can't capture stdout
#define UNIT_LIB_DISABLE_IOSTREAM

using namespace wpi::units;
using namespace wpi::units::literals;

// #378 ODR-audit regression guards: dimension-keyed physical-quantity concepts (Velocity/Force/...) and the
// value/dimension/layout/serialization safety invariants that make the cross-TU type-identity split value-safe.
#include "odrDimensionConcept.h"

// A user-defined base dimension + unit, declared outside the library, to prove serialization is extensible to
// dimensions the library has never seen (no central table, no fixed ceiling).
namespace wpi::units
{
	namespace dimension
	{
		struct pixels_tag
		{
			static constexpr auto name         = "pixels";
			static constexpr auto abbreviation = "px";
		};
		using pixels = make_dimension<pixels_tag>;
		// a dimension with a FRACTIONAL exponent (length^(1/2)), to exercise the fractional-exponent serialization path
		using root_length = dimension_pow<length, std::ratio<1, 2>>;
	} // namespace dimension
	UNIT_ADD(screen, dots, px, conversion_factor<std::ratio<1>, dimension::pixels>)
	UNIT_ADD(root_length, root_meters, rt_m, conversion_factor<std::ratio<1>, dimension::root_length>)
} // namespace wpi::units

namespace
{
	// Detector op for the SFINAE-safety guards: is std::common_type_t<A, B> well-formed? Used with the library's
	// detection idiom (detail::is_detected_v) to assert common_type is SFINAE-empty (not a hard error) where expected.
	template<class A, class B>
	using common_type_of = std::common_type_t<A, B>;

	class TypeTraits
	{
	};

	class STDTypeTraits
	{
	};

	class STDSpecializations
	{
	};

	class UnitManipulators
	{
	};

	class UnitType
	{
	};

	class ConversionFactor
	{
	};

	class UnitMath
	{
	};

	class Constexpr
	{
	};

	class ConcentrationSemantics
	{
	};

	class UnitLimits
	{
	};

	class CaseStudies
	{
	};

	class Serialization
	{
	};

	// Tests that two units have the same conversion ratio to the same dimension.
	constexpr auto has_equivalent_conversion_factor = []<typename T0, typename T1>(const T0&, const T1&)
	{
		using T = std::decay_t<T0>;
		using U = std::decay_t<T1>;
		return wpi::units::traits::is_same_dimension_unit_v<T, U> && std::ratio_equal_v<typename T::conversion_factor::conversion_ratio, typename U::conversion_factor::conversion_ratio>;
	};
} // namespace

TEST_CASE_METHOD(TypeTraits, "TypeTraits sizeOf", "[wpimath][units]")
{
	static_assert(sizeof(dimensionless<double>) == sizeof(double));
	static_assert(sizeof(meters<double>) == sizeof(double));
	static_assert(sizeof(degrees_squared<double>) == sizeof(double));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits isRatio", "[wpimath][units]")
{
	static_assert(traits::is_ratio_v<std::ratio<1>>);
	static_assert(!traits::is_ratio_v<double>);
}

// TEST NTTP SUPPORT
template<meters<double> m>
class NTTPTestClass {}; // If this fails to compile, you've broken NTTP support (public inheritence, public members)

TEST_CASE_METHOD(TypeTraits, "TypeTraits NTTPTest", "[wpimath][units]")
{
	// Force an actual NTTP instantiation:
	static_assert(requires { typename NTTPTestClass<1.0_m>; },
				  "Units failed NTTP support (class-type NTTP broken).");
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits ratio_sqrt", "[wpimath][units]")
{
	using rt2 = ratio_sqrt<std::ratio<2>>;
	CHECK(std::abs(std::sqrt(2 / 1.0) - rt2::num / static_cast<double>(rt2::den)) < 5e-9);

	using rt4 = ratio_sqrt<std::ratio<4>>;
	CHECK(std::abs(std::sqrt(4 / 1.0) - rt4::num / static_cast<double>(rt4::den)) < 5e-9);

	using rt10 = ratio_sqrt<std::ratio<10>>;
	CHECK(std::abs(std::sqrt(10 / 1.0) - rt10::num / static_cast<double>(rt10::den)) < 5e-9);

	using rt30 = ratio_sqrt<std::ratio<30>>;
	CHECK(std::abs(std::sqrt(30 / 1.0) - rt30::num / static_cast<double>(rt30::den)) < 5e-9);

	using rt61 = ratio_sqrt<std::ratio<61>>;
	CHECK(std::abs(std::sqrt(61 / 1.0) - rt61::num / static_cast<double>(rt61::den)) < 5e-9);

	using rt100 = ratio_sqrt<std::ratio<100>>;
	CHECK(std::abs(std::sqrt(100 / 1.0) - rt100::num / static_cast<double>(rt100::den)) < 5e-9);

	using rt1000 = ratio_sqrt<std::ratio<1000>>;
	CHECK(std::abs(std::sqrt(1000 / 1.0) - rt1000::num / static_cast<double>(rt1000::den)) < 5e-9);

	using rt10000 = ratio_sqrt<std::ratio<10000>>;
	CHECK(std::abs(std::sqrt(10000 / 1.0) - rt10000::num / static_cast<double>(rt10000::den)) < 5e-9);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_conversion_factor", "[wpimath][units]")
{
	static_assert(!traits::is_conversion_factor_v<std::ratio<1>>);
	static_assert(!traits::is_conversion_factor_v<double>);
	static_assert(traits::is_conversion_factor_v<feet<double>>);
	static_assert(traits::is_conversion_factor_v<degrees_squared<double>>);
	static_assert(traits::is_conversion_factor_v<meters<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_unit", "[wpimath][units]")
{
	static_assert(!traits::is_unit_v<std::ratio<1>>);
	static_assert(!traits::is_unit_v<double>);
	static_assert(traits::is_unit_v<meters<double>>);
	static_assert(traits::is_unit_v<feet<double>>);
	static_assert(traits::is_unit_v<degrees_squared<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits replace_underlying", "[wpimath][units]")
{
	static_assert(std::is_same_v<traits::replace_underlying_t<dimensionless<int>, int>, dimensionless<int>>);
	static_assert(std::is_same_v<traits::replace_underlying_t<dimensionless<int>, double>, dimensionless<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits conversion_factor_traits", "[wpimath][units]")
{
	static_assert(std::is_same_v<void, traits::conversion_factor_traits<double>::conversion_ratio>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits unit_traits", "[wpimath][units]")
{
	static_assert(std::is_same_v<double, traits::unit_traits<double>::underlying_type>);
	static_assert(std::is_same_v<double, traits::unit_traits<meters<double>>::underlying_type>);
	static_assert(std::is_same_v<void, traits::unit_traits<double>::value_type>);
	static_assert(std::is_same_v<double, traits::unit_traits<meters<double>>::value_type>);
	static_assert(std::is_same_v<void, traits::unit_traits<double>::value_type>);
	static_assert(std::is_same_v<int, traits::unit_traits<meters<int>>::value_type>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_same_dimension_conversion_factor", "[wpimath][units]")
{
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, astronomical_units<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, parsecs<double>::conversion_factor>);

	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<astronomical_units<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<parsecs<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<years<double>::conversion_factor, weeks<double>::conversion_factor>);

	static_assert(!traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, seconds<double>::conversion_factor>);
	static_assert(!traits::is_same_dimension_conversion_factor_v<seconds<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(!traits::is_same_dimension_conversion_factor_v<years<double>::conversion_factor, meters<double>::conversion_factor>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits inverse", "[wpimath][units]")
{
	double test;

	using htz         = traits::strong_t<inverse<seconds<double>>>;
	bool shouldBeTrue = std::is_same_v<htz, hertz<double>::conversion_factor>;
	CHECK(shouldBeTrue);

	test = unit<inverse<fahrenheit<double>::conversion_factor>>(unit<inverse<celsius<double>::conversion_factor>>(1.0)).value();
	CHECK_THAT(5.0 / 9.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	test = unit<inverse<fahrenheit<double>::conversion_factor>>(unit<inverse<kelvin<double>::conversion_factor>>(6.0)).value();
	CHECK_THAT(10.0 / 3.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits strong", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless_, traits::strong_t<detail::conversion_factor_base_t<dimensionless_>>>);
	static_assert(std::is_same_v<meters<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::length>>>);
	static_assert(std::is_same_v<kilometers<double>::conversion_factor, traits::strong_t<kilometers<double>::conversion_factor>>);
	static_assert(std::is_same_v<square_meters<double>::conversion_factor, traits::strong_t<squared<meters<double>::conversion_factor>>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits dimension_of", "[wpimath][units]")
{
	using dim = traits::dimension_of_t<years<double>::conversion_factor>;

	static_assert(std::is_same_v<dim, dimension::time>);
	static_assert(!std::is_same_v<dim, dimension::length>);
	static_assert(!std::is_same_v<dim, days<int>>);

	using dim2 = traits::conversion_factor_traits<traits::unit_traits<decltype(meters_per_second<double>(5))>::conversion_factor>::dimension_type;

	static_assert(std::is_same_v<dim2, dimension::velocity>);
	static_assert(!std::is_same_v<dim2, dimension::time>);
	static_assert(!std::is_same_v<dim2, miles_per_hour<int>>);

	using dim = traits::dimension_of_t<years<double>>;

	static_assert(std::is_same_v<dim, dimension::time>);
	static_assert(!std::is_same_v<dim, dimension::length>);
	static_assert(!std::is_same_v<dim, days<int>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits has_linear_scale", "[wpimath][units]")
{
	static_assert(traits::has_linear_scale_v<dimensionless<double>>);
	static_assert(traits::has_linear_scale_v<meters<double>>);
	static_assert(traits::has_linear_scale_v<feet<double>>);
	static_assert(traits::has_linear_scale_v<watts<double>, dimensionless<double>>);
	static_assert(traits::has_linear_scale_v<dimensionless<double>, meters<double>>);
	static_assert(traits::has_linear_scale_v<meters_per_second<double>>);
	static_assert(!traits::has_linear_scale_v<decibels<double>>);
	static_assert(!traits::has_linear_scale_v<decibels<double>, meters_per_second<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits has_decibel_scale", "[wpimath][units]")
{
	static_assert(!traits::has_decibel_scale_v<dimensionless<double>>);
	static_assert(!traits::has_decibel_scale_v<meters<double>>);
	static_assert(!traits::has_decibel_scale_v<feet<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>>);
	static_assert(traits::has_decibel_scale_v<dBW<double>>);

	static_assert(traits::has_decibel_scale_v<dBW<double>, decibels<double>>);
	static_assert(traits::has_decibel_scale_v<dBW<double>, dBm<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>, decibels<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>, decibels<double>, decibels<double>>);
	static_assert(!traits::has_decibel_scale_v<decibels<double>, decibels<double>, meters<double>>);
	static_assert(!traits::has_decibel_scale_v<meters<double>, decibels<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_dimensionless_unit", "[wpimath][units]")
{
	static_assert(traits::is_dimensionless_unit_v<dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<const dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<const dimensionless<double>&>);
	static_assert(traits::is_dimensionless_unit_v<dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<decibels<double>>);
	static_assert(traits::is_dimensionless_unit_v<parts_per_million<double>>);
	static_assert(!traits::is_dimensionless_unit_v<meters<double>>);
	static_assert(!traits::is_dimensionless_unit_v<dBW<double>>);

	static_assert(std::is_arithmetic_v<const double>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_length_unit", "[wpimath][units]")
{
	static_assert(!traits::is_length_unit_v<double>);
	static_assert(traits::is_length_unit_v<meters<double>>);
	static_assert(traits::is_length_unit_v<const meters<double>>);
	static_assert(traits::is_length_unit_v<const meters<double>&>);
	static_assert(traits::is_length_unit_v<cubits<double>>);
	static_assert(!traits::is_length_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_mass_unit", "[wpimath][units]")
{
	static_assert(!traits::is_mass_unit_v<double>);
	static_assert(traits::is_mass_unit_v<kilograms<double>>);
	static_assert(traits::is_mass_unit_v<const kilograms<double>>);
	static_assert(traits::is_mass_unit_v<const kilograms<double>&>);
	static_assert(traits::is_mass_unit_v<stone<double>>);
	static_assert(!traits::is_mass_unit_v<meters<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_time_unit", "[wpimath][units]")
{
	static_assert(!traits::is_time_unit_v<double>);
	static_assert(traits::is_time_unit_v<seconds<double>>);
	static_assert(traits::is_time_unit_v<const seconds<double>>);
	static_assert(traits::is_time_unit_v<const seconds<double>&>);
	static_assert(traits::is_time_unit_v<years<double>>);
	static_assert(!traits::is_time_unit_v<meters<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_angle_unit", "[wpimath][units]")
{
	static_assert(!traits::is_angle_unit_v<double>);
	static_assert(traits::is_angle_unit_v<radians<double>>);
	static_assert(traits::is_angle_unit_v<const radians<double>>);
	static_assert(traits::is_angle_unit_v<const radians<double>&>);
	static_assert(traits::is_angle_unit_v<degrees<double>>);
	static_assert(!traits::is_angle_unit_v<watts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_current_unit", "[wpimath][units]")
{
	static_assert(!traits::is_current_unit_v<double>);
	static_assert(traits::is_current_unit_v<amperes<double>>);
	static_assert(traits::is_current_unit_v<const amperes<double>>);
	static_assert(traits::is_current_unit_v<const amperes<double>&>);
	static_assert(!traits::is_current_unit_v<volts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_temperature_unit", "[wpimath][units]")
{
	static_assert(!traits::is_temperature_unit_v<double>);
	static_assert(traits::is_temperature_unit_v<fahrenheit<double>>);
	static_assert(traits::is_temperature_unit_v<const fahrenheit<double>>);
	static_assert(traits::is_temperature_unit_v<const fahrenheit<double>&>);
	static_assert(traits::is_temperature_unit_v<kelvin<double>>);
	static_assert(!traits::is_temperature_unit_v<cubits<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_substance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_substance_unit_v<double>);
	static_assert(traits::is_substance_unit_v<mols<double>>);
	static_assert(traits::is_substance_unit_v<const mols<double>>);
	static_assert(traits::is_substance_unit_v<const mols<double>&>);
	static_assert(!traits::is_substance_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_luminous_intensity_unit", "[wpimath][units]")
{
	static_assert(!traits::is_luminous_intensity_unit_v<double>);
	static_assert(traits::is_luminous_intensity_unit_v<candelas<double>>);
	static_assert(traits::is_luminous_intensity_unit_v<const candelas<double>>);
	static_assert(traits::is_luminous_intensity_unit_v<const candelas<double>&>);
	static_assert(!traits::is_luminous_intensity_unit_v<radiation_absorbed_dose<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_solid_angle_unit", "[wpimath][units]")
{
	static_assert(!traits::is_solid_angle_unit_v<double>);
	static_assert(traits::is_solid_angle_unit_v<steradians<double>>);
	static_assert(traits::is_solid_angle_unit_v<const steradians<double>>);
	static_assert(traits::is_solid_angle_unit_v<const degrees_squared<double>&>);
	static_assert(!traits::is_solid_angle_unit_v<degrees<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_frequency_unit", "[wpimath][units]")
{
	static_assert(!traits::is_frequency_unit_v<double>);
	static_assert(traits::is_frequency_unit_v<hertz<double>>);
	static_assert(traits::is_frequency_unit_v<const hertz<double>>);
	static_assert(traits::is_frequency_unit_v<const hertz<double>&>);
	static_assert(!traits::is_frequency_unit_v<seconds<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_volume_flow_rate_unit", "[wpimath][units]")
{
	static_assert(!traits::is_volume_flow_rate_unit_v<double>);
	static_assert(traits::is_volume_flow_rate_unit_v<cubic_meters_per_second<double>>);
	static_assert(traits::is_volume_flow_rate_unit_v<const liters_per_second<double>>);
	static_assert(traits::is_volume_flow_rate_unit_v<const gallons_per_minute<double>&>);
	static_assert(!traits::is_volume_flow_rate_unit_v<cubic_meters<double>>);
	static_assert(!traits::is_volume_flow_rate_unit_v<meters_per_second<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_velocity_unit", "[wpimath][units]")
{
	static_assert(!traits::is_velocity_unit_v<double>);
	static_assert(traits::is_velocity_unit_v<meters_per_second<double>>);
	static_assert(traits::is_velocity_unit_v<const meters_per_second<double>>);
	static_assert(traits::is_velocity_unit_v<const meters_per_second<double>&>);
	static_assert(traits::is_velocity_unit_v<miles_per_hour<double>>);
	static_assert(!traits::is_velocity_unit_v<meters_per_second_squared<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_acceleration_unit", "[wpimath][units]")
{
	static_assert(!traits::is_acceleration_unit_v<double>);
	static_assert(traits::is_acceleration_unit_v<meters_per_second_squared<double>>);
	static_assert(traits::is_acceleration_unit_v<const meters_per_second_squared<double>>);
	static_assert(traits::is_acceleration_unit_v<const meters_per_second_squared<double>&>);
	static_assert(traits::is_acceleration_unit_v<standard_gravity<double>>);
	static_assert(!traits::is_acceleration_unit_v<inches<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_force_unit", "[wpimath][units]")
{
	static_assert(!traits::is_force_unit_v<double>);
	static_assert(traits::is_force_unit_v<newtons<double>>);
	static_assert(traits::is_force_unit_v<const newtons<double>>);
	static_assert(traits::is_force_unit_v<const newtons<double>&>);
	static_assert(traits::is_force_unit_v<dynes<double>>);
	static_assert(!traits::is_force_unit_v<watts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_pressure_unit", "[wpimath][units]")
{
	static_assert(!traits::is_pressure_unit_v<double>);
	static_assert(traits::is_pressure_unit_v<pascals<double>>);
	static_assert(traits::is_pressure_unit_v<const pascals<double>>);
	static_assert(traits::is_pressure_unit_v<const pascals<double>&>);
	static_assert(traits::is_pressure_unit_v<atmospheres<double>>);
	static_assert(!traits::is_pressure_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_charge_unit", "[wpimath][units]")
{
	static_assert(!traits::is_charge_unit_v<double>);
	static_assert(traits::is_charge_unit_v<coulombs<double>>);
	static_assert(traits::is_charge_unit_v<const coulombs<double>>);
	static_assert(traits::is_charge_unit_v<const coulombs<double>&>);
	static_assert(!traits::is_charge_unit_v<watts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_energy_unit", "[wpimath][units]")
{
	static_assert(!traits::is_energy_unit_v<double>);
	static_assert(traits::is_energy_unit_v<joules<double>>);
	static_assert(traits::is_energy_unit_v<const joules<double>>);
	static_assert(traits::is_energy_unit_v<const joules<double>&>);
	static_assert(traits::is_energy_unit_v<calories<double>>);
	static_assert(!traits::is_energy_unit_v<watts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_power_unit", "[wpimath][units]")
{
	static_assert(!traits::is_power_unit_v<double>);
	static_assert(traits::is_power_unit_v<watts<double>>);
	static_assert(traits::is_power_unit_v<const watts<double>>);
	static_assert(traits::is_power_unit_v<const watts<double>&>);
	static_assert(!traits::is_power_unit_v<henries<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_voltage_unit", "[wpimath][units]")
{
	static_assert(!traits::is_voltage_unit_v<double>);
	static_assert(traits::is_voltage_unit_v<volts<double>>);
	static_assert(traits::is_voltage_unit_v<const volts<double>>);
	static_assert(traits::is_voltage_unit_v<const volts<double>&>);
	static_assert(!traits::is_voltage_unit_v<henries<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_capacitance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_capacitance_unit_v<double>);
	static_assert(traits::is_capacitance_unit_v<farads<double>>);
	static_assert(traits::is_capacitance_unit_v<const farads<double>>);
	static_assert(traits::is_capacitance_unit_v<const farads<double>&>);
	static_assert(!traits::is_capacitance_unit_v<ohms<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_impedance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_impedance_unit_v<double>);
	static_assert(traits::is_impedance_unit_v<ohms<double>>);
	static_assert(traits::is_impedance_unit_v<const ohms<double>>);
	static_assert(traits::is_impedance_unit_v<const ohms<double>&>);
	static_assert(!traits::is_impedance_unit_v<farads<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_conductance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_conductance_unit_v<double>);
	static_assert(traits::is_conductance_unit_v<siemens<double>>);
	static_assert(traits::is_conductance_unit_v<const siemens<double>>);
	static_assert(traits::is_conductance_unit_v<const siemens<double>&>);
	static_assert(!traits::is_conductance_unit_v<volts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_magnetic_flux_unit", "[wpimath][units]")
{
	static_assert(!traits::is_magnetic_flux_unit_v<double>);
	static_assert(traits::is_magnetic_flux_unit_v<webers<double>>);
	static_assert(traits::is_magnetic_flux_unit_v<const webers<double>>);
	static_assert(traits::is_magnetic_flux_unit_v<const webers<double>&>);
	static_assert(traits::is_magnetic_flux_unit_v<maxwells<double>>);
	static_assert(!traits::is_magnetic_flux_unit_v<inches<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_magnetic_field_strength_unit", "[wpimath][units]")
{
	static_assert(!traits::is_magnetic_field_strength_unit_v<double>);
	static_assert(traits::is_magnetic_field_strength_unit_v<teslas<double>>);
	static_assert(traits::is_magnetic_field_strength_unit_v<const teslas<double>>);
	static_assert(traits::is_magnetic_field_strength_unit_v<const teslas<double>&>);
	static_assert(traits::is_magnetic_field_strength_unit_v<gauss<double>>);
	static_assert(!traits::is_magnetic_field_strength_unit_v<volts<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_inductance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_inductance_unit_v<double>);
	static_assert(traits::is_inductance_unit_v<henries<double>>);
	static_assert(traits::is_inductance_unit_v<const henries<double>>);
	static_assert(traits::is_inductance_unit_v<const henries<double>&>);
	static_assert(!traits::is_inductance_unit_v<farads<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_luminous_flux_unit", "[wpimath][units]")
{
	static_assert(!traits::is_luminous_flux_unit_v<double>);
	static_assert(traits::is_luminous_flux_unit_v<lumens<double>>);
	static_assert(traits::is_luminous_flux_unit_v<const lumens<double>>);
	static_assert(traits::is_luminous_flux_unit_v<const lumens<double>&>);
	static_assert(!traits::is_luminous_flux_unit_v<mass::pounds<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_illuminance_unit", "[wpimath][units]")
{
	static_assert(!traits::is_illuminance_unit_v<double>);
	static_assert(traits::is_illuminance_unit_v<footcandles<double>>);
	static_assert(traits::is_illuminance_unit_v<const footcandles<double>>);
	static_assert(traits::is_illuminance_unit_v<const footcandles<double>&>);
	static_assert(traits::is_illuminance_unit_v<lux<double>>);
	static_assert(!traits::is_illuminance_unit_v<meters<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_radioactivity_unit", "[wpimath][units]")
{
	static_assert(!traits::is_radioactivity_unit_v<double>);
	static_assert(traits::is_radioactivity_unit_v<sieverts<double>>);
	static_assert(traits::is_radioactivity_unit_v<const sieverts<double>>);
	static_assert(traits::is_radioactivity_unit_v<const sieverts<double>&>);
	static_assert(!traits::is_radioactivity_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_torque_unit", "[wpimath][units]")
{
	static_assert(!traits::is_torque_unit_v<double>);
	static_assert(traits::is_torque_unit_v<newton_meters<double>>);
	static_assert(traits::is_torque_unit_v<const newton_meters<double>>);
	static_assert(traits::is_torque_unit_v<const newton_meters<double>&>);
	static_assert(traits::is_torque_unit_v<torque::pound_feet<double>>);
	static_assert(!traits::is_torque_unit_v<cubic_meters<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_area_unit", "[wpimath][units]")
{
	static_assert(!traits::is_area_unit_v<double>);
	static_assert(traits::is_area_unit_v<square_meters<double>>);
	static_assert(traits::is_area_unit_v<const square_meters<double>>);
	static_assert(traits::is_area_unit_v<const square_meters<double>&>);
	static_assert(traits::is_area_unit_v<hectares<double>>);
	static_assert(!traits::is_area_unit_v<astronomical_units<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_volume_unit", "[wpimath][units]")
{
	static_assert(!traits::is_volume_unit_v<double>);
	static_assert(traits::is_volume_unit_v<cubic_meters<double>>);
	static_assert(traits::is_volume_unit_v<const cubic_meters<double>>);
	static_assert(traits::is_volume_unit_v<const cubic_meters<double>&>);
	static_assert(traits::is_volume_unit_v<cubic_inches<double>>);
	static_assert(!traits::is_volume_unit_v<feet<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_density_unit", "[wpimath][units]")
{
	static_assert(!traits::is_density_unit_v<double>);
	static_assert(traits::is_density_unit_v<kilograms_per_cubic_meter<double>>);
	static_assert(traits::is_density_unit_v<const kilograms_per_cubic_meter<double>>);
	static_assert(traits::is_density_unit_v<const kilograms_per_cubic_meter<double>&>);
	static_assert(traits::is_density_unit_v<ounces_per_cubic_foot<double>>);
	static_assert(!traits::is_density_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_data_unit", "[wpimath][units]")
{
	static_assert(!traits::is_data_unit_v<double>);
	static_assert(traits::is_data_unit_v<bits<double>>);
	static_assert(traits::is_data_unit_v<const bits<double>>);
	static_assert(traits::is_data_unit_v<const bits<double>&>);
	static_assert(traits::is_data_unit_v<bytes<double>>);
	static_assert(!traits::is_data_unit_v<years<double>>);
}

TEST_CASE_METHOD(TypeTraits, "TypeTraits is_data_transfer_rate_unit", "[wpimath][units]")
{
	static_assert(!traits::is_data_transfer_rate_unit_v<double>);
	static_assert(traits::is_data_transfer_rate_unit_v<gigabits_per_second<double>>);
	static_assert(traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>>);
	static_assert(traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>&>);
	static_assert(traits::is_data_transfer_rate_unit_v<gigabytes_per_second<double>>);
	static_assert(!traits::is_data_transfer_rate_unit_v<years<double>>);
}

TEST_CASE_METHOD(STDTypeTraits, "STDTypeTraits std_common_type", "[wpimath][units]")
{
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, meters<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, kilometers<double>>(), kilometers()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, millimeters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, kilometers<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, meters<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, millimeters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, meters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, kilometers<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, millimeters<double>>(), millimeters()));
	static_assert(std::is_same_v<std::common_type_t<meters<double>, kilometers<double>>, std::common_type_t<kilometers<double>, meters<double>>>);
	static_assert(std::is_same_v<std::common_type_t<meters<double>, millimeters<double>>, std::common_type_t<millimeters<double>, meters<double>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeters<double>, kilometers<double>>, std::common_type_t<kilometers<double>, millimeters<double>>>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, meters<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, kilometers<int>>(), kilometers<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, kilometers<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, meters<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, meters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, kilometers<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(std::is_same_v<std::common_type_t<meters<int>, kilometers<int>>, std::common_type_t<kilometers<int>, meters<int>>>);
	static_assert(std::is_same_v<std::common_type_t<meters<int>, millimeters<int>>, std::common_type_t<millimeters<int>, meters<int>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeters<int>, kilometers<int>>, std::common_type_t<kilometers<int>, millimeters<int>>>);

	using half_a_second  = unit<conversion_factor<std::ratio<1, 2>, seconds<double>>, int>;
	using third_a_second = unit<conversion_factor<std::ratio<1, 3>, seconds<double>>, int>;
	using sixth_a_second = unit<conversion_factor<std::ratio<1, 6>, seconds<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_second, third_a_second>{}, sixth_a_second{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>, std::common_type_t<third_a_second, half_a_second>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>::underlying_type, int>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<kelvin<double>, celsius<double>>{}, celsius{}));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<celsius<double>, kelvin<double>>{}, celsius{}));
	static_assert(std::is_same_v<std::common_type_t<kelvin<double>, celsius<double>>, std::common_type_t<celsius<double>, kelvin<double>>>);

	using half_a_kelvin  = unit<conversion_factor<std::ratio<1, 2>, kelvin<double>>, double>;
	using third_a_kelvin = unit<conversion_factor<std::ratio<1, 3>, kelvin<double>>, int>;
	using sixth_a_kelvin = unit<conversion_factor<std::ratio<1, 6>, kelvin<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_kelvin, third_a_kelvin>{}, sixth_a_kelvin{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>, std::common_type_t<third_a_kelvin, half_a_kelvin>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>::underlying_type, double>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<radians<double>, degrees<double>>{}, degrees<double>{}));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<degrees<double>, radians<double>>{}, degrees<double>{}));
	static_assert(std::is_same_v<std::common_type_t<radians<double>, degrees<double>>, std::common_type_t<degrees<double>, radians<double>>>);

	using half_a_radian     = unit<conversion_factor<std::ratio<1, 2>, radians<double>>, int>;
	using big_half_a_radian = unit<conversion_factor<std::ratio<2, 4>, radians<double>>, int>;
	using third_a_radian    = unit<conversion_factor<std::ratio<1, 3>, radians<double>>, double>;
	using sixth_a_radian    = unit<conversion_factor<std::ratio<1, 6>, radians<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_radian, third_a_radian>{}, sixth_a_radian{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>, std::common_type_t<third_a_radian, half_a_radian>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>::underlying_type, double>);

	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<int>>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<double>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, dimensionless<int>>, dimensionless<double>>);

	static_assert(std::is_same_v<traits::conversion_factor_traits<traits::unit_traits<std::common_type_t<half_a_radian, big_half_a_radian>>::conversion_factor>::conversion_ratio, std::ratio<1, 2>>);

	using T = std::common_type_t<percent<double>, double>;
	T a     = 50_pct;
	CHECK(a == 0.5);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, unit<conversion_factor<std::ratio<1>, dimension::dimensionless>, int>>);
	static_assert(std::is_same_v<conversion_factor<std::ratio<1>, dimension::dimensionless>, dimensionless_>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, unit<dimensionless_, int>>);

	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<int, dimensionless<int>>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, double>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<double, dimensionless<int>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, int>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<int, dimensionless<double>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, double>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<double, dimensionless<double>>, dimensionless<double>>);

	static_assert(std::is_same_v<std::common_type_t<degrees<double>, degrees<double>>, degrees<double>>);
	static_assert(std::is_same_v<std::common_type_t<celsius<double>, celsius<double>>, celsius<double>>);
}

TEST_CASE_METHOD(STDSpecializations, "STDSpecializations hash", "[wpimath][units]")
{
	CHECK(std::hash<meters<double>>()(3.14_m) == std::hash<double>()(3.14));
	CHECK(std::hash<millimeters<double>>()(3.14_m) == std::hash<double>()(3.14e3));
	CHECK(std::hash<millimeters<double>>()(3.14_mm) == std::hash<double>()(3.14));
	CHECK(std::hash<kilometers<double>>()(3.14_m) == std::hash<double>()(3.14e-3));
	CHECK(std::hash<kilometers<double>>()(3.14_km) == std::hash<double>()(3.14));

	CHECK((std::hash<meters<int>>()(meters<int>(42))) == 42);
	CHECK((std::hash<millimeters<int>>()(meters<int>(42))) == 42000);
	CHECK((std::hash<millimeters<int>>()(millimeters<int>(42))) == 42);
	CHECK((std::hash<kilometers<int>>()(kilometers<int>(42))) == 42);

	CHECK((std::hash<dimensionless<double>>()(3.14)) == std::hash<double>()(3.14));
	CHECK((std::hash<dimensionless<int>>()(42)) == (std::hash<dimensionless<int>>()(42)));

	CHECK(std::hash<dBW<double>>()(2.0_dBW) == std::hash<double>()(dBW<>(2.0).to_linearized()));
}

// General coverage (not tied to a specific change): units must work END-TO-END as STL associative-container
// keys — an ordered container exercises operator<, an unordered one exercises std::hash + operator== together.
// The hash test above only calls std::hash directly; this proves the real use case, and that a scaled key
// (kilometers vs meters) compares/hashes by magnitude, not by stored representation.
TEST_CASE_METHOD(STDSpecializations, "STDSpecializations unitsAsContainerKeys", "[wpimath][units]")
{
	// std::map — ordered by value via operator<.
	std::map<meters<double>, std::string> byValue;
	byValue[meters<double>(1.0)] = "one";
	byValue[meters<double>(2.0)] = "two";
	byValue[kilometers<double>(0.003)] = "three-m"; // 3 m, distinct key from 1 m / 2 m
	CHECK(byValue.size() == 3u);
	CHECK(byValue[meters<double>(1.0)] == "one");
	CHECK(byValue.begin()->second == "one"); // smallest key first

	// std::unordered_map — needs both std::hash<meters<double>> AND operator==.
	std::unordered_map<meters<double>, int> byHash;
	byHash[meters<double>(5.0)] = 50;
	CHECK(byHash.at(meters<double>(5.0)) == 50);
	// a scaled-but-equal key resolves to the SAME bucket (kilometers<double>(0.005) == 5 m). Look it up after
	// converting to the map's key type, since a heterogeneous [] would insert a different key type.
	CHECK(byHash.at(meters<double>(kilometers<double>(0.005))) == 50);

	// std::set — membership by value.
	std::set<seconds<double>> timeSet{seconds<double>(1.0), seconds<double>(2.0), seconds<double>(1.0)};
	CHECK(timeSet.size() == 2u); // the duplicate 1 s collapses
	CHECK(timeSet.count(seconds<double>(2.0)) == 1);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators squared", "[wpimath][units]")
{
	double test;

	test = square_feet<double>(unit<squared<meters<double>>>(0.092903)).value();
	CHECK_THAT(0.99999956944, Catch::Matchers::WithinAbs(test, 5.0e-12));

	using dimensionless_2 = traits::strong_t<squared<wpi::units::dimensionless_>>; // this is actually nonsensical, and should also result in
	// a dimensionless.
	bool isSame = std::is_same_v<unit<dimensionless_>, unit<dimensionless_2>>;
	CHECK(isSame);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators cubed", "[wpimath][units]")
{
	double test;

	test = cubic_feet<double>(unit<cubed<meters<double>>>(0.0283168)).value();
	CHECK_THAT(0.999998354619, Catch::Matchers::WithinAbs(test, 5.0e-13));
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators square_root", "[wpimath][units]")
{
	double test;

	test = meters<double>(unit<square_root<square_kilometers<double>>>(1.0)).value();
	static_assert(traits::is_same_dimension_conversion_factor_v<square_root<square_kilometers<double>>, kilometers<double>>);
	CHECK_THAT(1000.0, Catch::Matchers::WithinAbs(test, 5.0e-13));
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators compound_unit", "[wpimath][units]")
{
	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = compound_conversion_factor<meters<double>, inverse<seconds<double>>, inverse<seconds<double>>>;
	using acceleration3 = conversion_factor<std::ratio<1>, make_dimension<dimension::length, std::ratio<1>, dimension::time, std::ratio<-2>>>;
	using acceleration4 = compound_conversion_factor<meters<double>, inverse<squared<seconds<double>>>>;
	using acceleration5 = compound_conversion_factor<meters<double>, squared<inverse<seconds<double>>>>;

	bool areSame12 = std::is_same_v<acceleration1, acceleration2>;
	bool areSame23 = std::is_same_v<acceleration2, acceleration3>;
	bool areSame34 = std::is_same_v<acceleration3, acceleration4>;
	bool areSame45 = std::is_same_v<acceleration4, acceleration5>;

	CHECK(areSame12);
	CHECK(areSame23);
	CHECK(areSame34);
	CHECK(areSame45);

	// test that thing with translations still compile
	using arbitrary1 = compound_conversion_factor<meters<double>, inverse<celsius<double>>>;
	using arbitrary2 = compound_conversion_factor<meters<double>, celsius<double>>;
	using arbitrary3 = traits::strong_t<compound_conversion_factor<arbitrary1, arbitrary2>>;
	static_assert(std::is_same_v<square_meters<double>::conversion_factor, arbitrary3>);
}

TEST_CASE_METHOD(UnitManipulators, "UnitManipulators dimensionalAnalysis", "[wpimath][units]")
{
	// these look like 'compound units', but the dimensional analysis can be REALLY handy if the
	// unit types aren't know (i.e. they themselves are template parameters), as you can get the resulting unit of the
	// operation.

	using velocity    = decltype(meters<double>{1.0} / seconds<double>{1.0});
	bool shouldBeTrue = std::is_same_v<meters_per_second<double>, velocity>;
	CHECK(shouldBeTrue);

	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = wpi::units::detail::unit_divide<meters<double>, wpi::units::detail::unit_multiply<seconds<double>, seconds<double>>>;
	shouldBeTrue        = std::is_same_v<acceleration1, acceleration2>;
	CHECK(shouldBeTrue);
}

TEST_CASE_METHOD(UnitType, "UnitType trivial", "[wpimath][units]")
{
	static_assert(std::is_trivial_v<meters<double>>);
	static_assert(std::is_trivially_assignable_v<meters<double>, meters<double>>);
	static_assert(std::is_trivially_constructible_v<meters<double>>);
	static_assert(std::is_trivially_copy_assignable_v<meters<double>>);
	static_assert(std::is_trivially_copy_constructible_v<meters<double>>);
	static_assert(std::is_trivially_copyable_v<meters<double>>);
	static_assert(std::is_trivially_default_constructible_v<meters<double>>);
	static_assert(std::is_trivially_destructible_v<meters<double>>);
	static_assert(std::is_trivially_move_assignable_v<meters<double>>);
	static_assert(std::is_trivially_move_constructible_v<meters<double>>);

	static_assert(std::is_trivial_v<decibels<double>>);
	static_assert(std::is_trivially_assignable_v<decibels<double>, decibels<double>>);
	static_assert(std::is_trivially_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_copy_assignable_v<decibels<double>>);
	static_assert(std::is_trivially_copy_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_copyable_v<decibels<double>>);
	static_assert(std::is_trivially_default_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_destructible_v<decibels<double>>);
	static_assert(std::is_trivially_move_assignable_v<decibels<double>>);
	static_assert(std::is_trivially_move_constructible_v<decibels<double>>);
}

TEST_CASE_METHOD(UnitType, "UnitType complexUnits", "[wpimath][units]")
{
	constexpr std::complex<meters<>> x(3_m, 4_m);
	CHECK((std::conj(x) == std::complex{3.0_m, -4.0_m}));
}

TEST_CASE_METHOD(UnitType, "UnitType constructionFromArithmeticType", "[wpimath][units]")
{
	constexpr meters a_m(1.0);
	CHECK(1.0 == a_m.value());

	constexpr meters<double> b_m(1);
	CHECK(1 == b_m.value());

	constexpr meters c_m(1);
	CHECK(1 == c_m.value());

	constexpr dimensionless d_dim(1.0);
	CHECK(1.0 == d_dim.value());

	constexpr dimensionless<double> e_dim(1);
	CHECK(1 == e_dim.value());

	constexpr dimensionless a_dim(1.0);
	CHECK(1.0 == a_dim.value());

	constexpr dimensionless<double> b_dim(1);
	CHECK(1 == b_dim.value());

	constexpr dimensionless c_dim(1);
	CHECK(1 == c_dim.value());

	constexpr dimensionless f_dim(1);
	CHECK(1 == f_dim.value());
}

TEST_CASE_METHOD(UnitType, "UnitType constructionFromUnitType", "[wpimath][units]")
{
	constexpr meters a_m(1);

	constexpr meters b_m(a_m);
	CHECK(1 == b_m.value());

	constexpr millimeters a_mm(b_m);
	CHECK(1000 == a_mm.value());

	constexpr millimeters b_mm(a_mm);
	CHECK(1000 == b_mm.value());

	constexpr millimeters c_mm(b_mm);
	CHECK(1000 == c_mm.value());

	constexpr millimeters d_mm(b_m);
	CHECK(1000 == d_mm.value());

	constexpr meters<double> c_m(b_m);
	CHECK(1.0 == c_m.value());

	constexpr meters<double> d_m(a_mm);
	CHECK(1.0 == d_m.value());

	constexpr meters<double> e_m(b_mm);
	CHECK(1.0 == e_m.value());

	constexpr meters f_m(c_m);
	CHECK(1.0 == f_m.value());

	constexpr meters<double> g_m(kilometers<int>(1));
	CHECK(1000.0 == g_m.value());

	constexpr dimensionless a_dim(1);

	constexpr dimensionless b_dim(a_dim);
	CHECK(1 == b_dim.value());

	constexpr dimensionless c_dim(b_dim);
	CHECK(1 == c_dim.value());

	constexpr dimensionless d_dim(c_dim);
	CHECK(1 == d_dim.value());

	constexpr dimensionless e_dim(d_dim);
	CHECK(1 == e_dim.value());

	constexpr dimensionless f_dim(c_dim);
	CHECK(1 == f_dim.value());

	constexpr dimensionless g_dim(f_dim);
	CHECK(1 == g_dim.value());
}

TEST_CASE_METHOD(UnitType, "UnitType CTAD", "[wpimath][units]")
{
	// Default ctor
	constexpr meters z_m{};
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_m)>, meters<double>>);

	// Underlying type, copy ctor, and same dimensioned units for `int` and `double`.
	constexpr meters a_m(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_m)>, meters<int>>);

	constexpr meters b_m(a_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m)>, meters<int>>);

	constexpr meters b_m2(millimeters(2.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m2)>, meters<double>>);

	constexpr millimeters a_mm(b_m);
	static_assert(std::is_integral_v<decltype(a_mm.value())>);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_mm)>, millimeters<int>>);
	CHECK(a_mm == 1000_mm);

	constexpr meters c_m(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_m)>, meters<double>>);

	constexpr meters d_m(c_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_m)>, meters<double>>);

	constexpr millimeters b_mm(d_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_mm)>, millimeters<double>>);

	constexpr kilometers a_km(b_mm);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_km)>, kilometers<double>>);

	// Other underlying types.
	constexpr meters e_m(static_cast<short>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_m)>, meters<short>>);

	constexpr meters f_m(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_m)>, meters<float>>);

	constexpr meters g_m(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_m)>, meters<long long>>);

	constexpr meters h_m(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_m)>, meters<long double>>);

	// `unit`.
	constexpr meters i_m(meters<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_m)>, meters<int>>);

	constexpr meters j_m(meters<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_m)>, meters<double>>);

	constexpr meters k_m(kilometers(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_m)>, meters<int>>);

	constexpr meters l_m(kilometers<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_m)>, meters<double>>);

	constexpr meters m_m(millimeters<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_m)>, meters<double>>);

	// `std::chrono::duration`.
	using namespace std::chrono_literals;

	constexpr unit a_s(1s);
	static_assert(seconds<int>(1s) == a_s && std::is_integral_v<decltype(a_s.value())>);

	constexpr unit a_min(1.0min);
	static_assert(minutes<double>(1.0) == a_min && std::is_floating_point_v<decltype(a_min.value())>);

	// A unit literal is floating-point (1_s is seconds<double>, the same as 1.0_s), so a quantity deduced from
	// one is floating-point. An integer-backed quantity comes from an explicit seconds<int>(1) or a std::chrono
	// integer duration (1s), both exercised above.
	constexpr seconds b_s(1_s);
	static_assert(std::is_floating_point_v<decltype(b_s.value())>);

	constexpr seconds c_s(1.0_s);
	static_assert(std::is_floating_point_v<decltype(c_s.value())>);

	constexpr seconds d_s(1_min);
	static_assert(std::is_floating_point_v<decltype(d_s.value())>);

	constexpr seconds e_s(1.0_min);
	static_assert(std::is_floating_point_v<decltype(e_s.value())>);

	constexpr seconds f_s(1.0_ms);
	static_assert(std::is_floating_point_v<decltype(f_s.value())>);

	// Dimensionless units.
	constexpr dimensionless z_dim = 1.0;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_dim)>, dimensionless<double>>);

	constexpr dimensionless a_dim(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_dim)>, dimensionless<int>>);

	constexpr dimensionless b_dim(a_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_dim)>, dimensionless<int>>);

	constexpr percent a_per(b_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_per)>, percent<int>>);

	constexpr dimensionless c_dim(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_dim)>, dimensionless<double>>);

	constexpr dimensionless d_dim(c_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_dim)>, dimensionless<double>>);

	constexpr percent b_per(d_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_per)>, percent<double>>);

	constexpr dimensionless e_dim(static_cast<short>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_dim)>, dimensionless<short>>);

	constexpr dimensionless f_dim(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_dim)>, dimensionless<float>>);

	constexpr dimensionless g_dim(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_dim)>, dimensionless<long long>>);

	constexpr dimensionless h_dim(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_dim)>, dimensionless<long double>>);

	constexpr dimensionless i_dim(dimensionless<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_dim)>, dimensionless<int>>);

	constexpr dimensionless j_dim(dimensionless<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_dim)>, dimensionless<double>>);

	constexpr dimensionless k_dim(unit<conversion_factor<std::kilo, dimensionless_>, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_dim)>, dimensionless<int>>);

	constexpr dimensionless l_dim(unit<conversion_factor<std::kilo, dimensionless_>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_dim)>, dimensionless<double>>);

	constexpr dimensionless m_dim(unit<conversion_factor<std::milli, dimensionless_>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_dim)>, dimensionless<double>>);

	constexpr radians n_dim(degrees{1});
	static_assert(std::is_same_v<std::remove_const_t<decltype(n_dim)>, radians<double>>);

	constexpr radians o_dim(degrees{1.0});
	static_assert(std::is_same_v<std::remove_const_t<decltype(o_dim)>, radians<double>>);
}

TEST_CASE_METHOD(UnitType, "UnitType implicitChronoConversions", "[wpimath][units]")
{
	using namespace std::chrono_literals;
	std::chrono::seconds chronoSec(1);
	const seconds<int>   unitsSec = chronoSec;
	chronoSec                     = unitsSec;
	CHECK(unitsSec == 1_s);
	CHECK(chronoSec == 1s);
}

TEST_CASE_METHOD(UnitType, "UnitType negativeConstexprLiterals", "[wpimath][units]")
{
	static constexpr radians ANGULAR_VALUE{-30.0_deg};
	CHECK(-30.0_deg == ANGULAR_VALUE);
}

TEST_CASE_METHOD(UnitType, "UnitType assignmentFromArithmeticType", "[wpimath][units]")
{
	dimensionless<int> a_dim;
	a_dim = 1;
	CHECK(1 == a_dim.value());
	a_dim = 1.0;
	CHECK(1.0 == a_dim.value());

	dimensionless<int> b_dim;
	b_dim = 1;
	CHECK(1 == b_dim.value());
	b_dim = 1.0;
	CHECK(1 == b_dim.value());

	dimensionless<double> c_dim;
	c_dim = 1.0;
	CHECK(1.0 == c_dim.value());
	c_dim = 1;
	CHECK(1 == c_dim.value());

	dimensionless<double> d_dim;
	d_dim = 1.0;
	CHECK(1.0 == d_dim.value());
	d_dim = 1;
	CHECK(1 == d_dim.value());

	percent<double> p_dim;
	p_dim = 0.30;
	CHECK(30_pct == p_dim);
	CHECK(0.3 == p_dim.value());
	CHECK(30 == p_dim.raw());
	p_dim = 1;
	CHECK(100_pct == p_dim);
	CHECK(1 == p_dim.value());
	CHECK(100 == p_dim.raw());
}

TEST_CASE_METHOD(UnitType, "UnitType assignmentFromUnitType", "[wpimath][units]")
{
	meters<int> a_m(1);
	a_m = +a_m;
	CHECK(1 == a_m.value());

	millimeters<int> a_mm;
	a_mm = a_m;
	CHECK(1000 == a_mm.value());
	a_mm = +a_mm;
	CHECK(1000 == a_mm.value());

	millimeters<int> b_mm;
	b_mm = a_m;
	CHECK(1000 == b_mm.value());
	b_mm = a_mm;
	CHECK(1000 == b_mm.value());
	b_mm = +b_mm;
	CHECK(1000 == b_mm.value());

	a_mm = b_mm;
	CHECK(1000 == a_mm.value());

	meters<double> b_m;
	b_m = a_m;
	CHECK(1 == b_m.value());
	b_m = a_mm;
	CHECK(1 == b_m.value());
	b_m = b_mm;
	CHECK(1 == b_m.value());
	b_m = +b_m;
	CHECK(1 == b_m.value());
	b_m = kilometers<int>(1);
	CHECK(1000 == b_m.value());

	dimensionless<int> a_dim(1);
	a_dim = +a_dim;
	CHECK(1 == a_dim.value());

	dimensionless<int> b_dim;
	b_dim = a_dim;
	CHECK(1 == b_dim.value());
	b_dim = +b_dim;
	CHECK(1 == b_dim.value());

	a_dim = b_dim;
	CHECK(1 == a_dim.value());

	dimensionless<double> c_dim;
	c_dim = a_dim;
	CHECK(1 == c_dim.value());
	c_dim = b_dim;
	CHECK(1 == c_dim.value());
	c_dim = +c_dim;
	CHECK(1 == c_dim.value());

	percent<double> d_dim;
	d_dim = dimensionless<double>(0.75);
	CHECK(75_pct == d_dim);
	CHECK(0.75 == d_dim.value());
	d_dim = parts_per_million<double>(8);
	CHECK(0.0008_pct == d_dim);
	CHECK(0.000008 == d_dim.value());
	d_dim = +dimensionless<double>(0.75);
	CHECK(75_pct == d_dim);
	CHECK(0.75 == d_dim.value());
}

TEST_CASE_METHOD(UnitType, "UnitType make_unit", "[wpimath][units]")
{
	constexpr auto a_m = make_unit<meters<double>>(5.0);
	CHECK(meters<double>(5.0) == a_m);

	constexpr auto b_m = make_unit<meters<double>>(5);
	CHECK(meters<double>(5) == b_m);

	constexpr auto c_m = make_unit<meters<int>>(5);
	CHECK((meters<int>(5)) == c_m);

	constexpr auto a_dim = make_unit<dimensionless<double>>(5.0);
	CHECK(dimensionless<double>(5.0) == a_dim);

	constexpr auto b_dim = make_unit<dimensionless<double>>(5);
	CHECK(dimensionless<double>(5) == b_dim);

	constexpr auto c_dim = make_unit<dimensionless<int>>(5);
	CHECK((dimensionless<int>(5)) == c_dim);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeEquality", "[wpimath][units]")
{
	constexpr meters a_m(0.0);
	constexpr meters b_m(1.0);

	CHECK(a_m == a_m);
	CHECK_FALSE(a_m == b_m);
	CHECK(a_m != b_m);
	CHECK_FALSE(b_m != b_m);

	constexpr meters c_m(0);
	constexpr meters d_m(1);

	CHECK(c_m == c_m);
	CHECK_FALSE(c_m == d_m);
	CHECK(c_m != d_m);
	CHECK_FALSE(d_m != d_m);

	CHECK(a_m == c_m);
	CHECK(d_m == b_m);
	CHECK_FALSE(a_m != c_m);
	CHECK_FALSE(d_m != b_m);
	CHECK(a_m != d_m);
	CHECK(c_m != b_m);
	CHECK_FALSE(a_m != c_m);
	CHECK_FALSE(d_m != b_m);

	constexpr percent w_m(100.0);
	constexpr percent x_m(1.0);

	CHECK(w_m == w_m);
	CHECK_FALSE(w_m == x_m);
	CHECK(w_m != x_m);
	CHECK_FALSE(x_m != x_m);

	constexpr percent y_m(100);
	constexpr percent z_m(1);

	CHECK(y_m == y_m);
	CHECK_FALSE(y_m == z_m);
	CHECK(y_m != z_m);
	CHECK_FALSE(z_m != z_m);

	CHECK(w_m == y_m);
	CHECK(z_m == x_m);
	CHECK_FALSE(w_m != y_m);
	CHECK_FALSE(z_m != x_m);
	CHECK(w_m != z_m);
	CHECK(y_m != x_m);
	CHECK_FALSE(w_m != y_m);
	CHECK_FALSE(z_m != x_m);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeMixedEquality", "[wpimath][units]")
{
	constexpr meters<double> a_m(0);
	constexpr feet           a_f(meters<double>(1));

	CHECK_FALSE(a_m == a_f);
	CHECK(a_m != a_f);

	constexpr feet   b_f(0);
	constexpr meters b_m(1);

	CHECK_FALSE(b_f == b_m);
	CHECK(b_f != b_m);

	CHECK(a_m == b_f);
	CHECK(b_m == a_f);
	CHECK_FALSE(a_m != b_f);
	CHECK_FALSE(b_m != a_f);

	constexpr percent           a_pct(1.0);
	constexpr parts_per_million b_ppm(20000.0);

	CHECK_FALSE(a_pct == b_ppm);
	CHECK(a_pct != b_ppm);

	constexpr percent           c_pct(1);
	constexpr parts_per_million d_ppm(20000);

	CHECK_FALSE(c_pct == d_ppm);
	CHECK(c_pct != d_ppm);

	CHECK(a_pct == c_pct);
	CHECK(b_ppm == d_ppm);
	CHECK_FALSE(a_pct != c_pct);
	CHECK_FALSE(b_ppm != d_ppm);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeRelational", "[wpimath][units]")
{
	constexpr meters a_m(0.0);
	constexpr meters b_m(1.0);

	CHECK_FALSE(a_m < a_m);
	CHECK_FALSE(b_m < a_m);
	CHECK(a_m < b_m);
	CHECK(a_m <= a_m);
	CHECK_FALSE(b_m <= a_m);
	CHECK(a_m <= b_m);
	CHECK_FALSE(a_m > a_m);
	CHECK(b_m > a_m);
	CHECK_FALSE(a_m > b_m);
	CHECK(a_m >= a_m);
	CHECK(b_m >= a_m);
	CHECK_FALSE(a_m >= b_m);

	constexpr meters c_m(0);
	constexpr meters d_m(1);

	CHECK_FALSE(c_m < c_m);
	CHECK_FALSE(d_m < c_m);
	CHECK(c_m < d_m);
	CHECK(c_m <= c_m);
	CHECK_FALSE(d_m <= c_m);
	CHECK(c_m <= d_m);
	CHECK_FALSE(c_m > c_m);
	CHECK(d_m > c_m);
	CHECK_FALSE(c_m > d_m);
	CHECK(c_m >= c_m);
	CHECK(d_m >= c_m);
	CHECK_FALSE(c_m >= d_m);

	CHECK_FALSE(a_m < c_m);
	CHECK_FALSE(d_m < a_m);
	CHECK(a_m < d_m);
	CHECK(c_m <= a_m);
	CHECK_FALSE(d_m <= a_m);
	CHECK(a_m <= d_m);
	CHECK_FALSE(a_m > c_m);
	CHECK(d_m > a_m);
	CHECK_FALSE(a_m > d_m);
	CHECK(c_m >= a_m);
	CHECK(d_m >= a_m);
	CHECK_FALSE(a_m >= d_m);

	constexpr dimensionless a_s(0.0);
	constexpr dimensionless b_s(1);

	CHECK_FALSE(a_s < a_s);
	CHECK_FALSE(b_s < a_s);
	CHECK(a_s < b_s);
	CHECK(a_s <= a_s);
	CHECK_FALSE(b_s <= a_s);
	CHECK(a_s <= b_s);
	CHECK_FALSE(a_s > a_s);
	CHECK(b_s > a_s);
	CHECK_FALSE(a_s > b_s);
	CHECK(a_s >= a_s);
	CHECK(b_s >= a_s);
	CHECK_FALSE(a_s >= b_s);

	constexpr dimensionless c_s(0.0);
	constexpr dimensionless d_s(1);

	CHECK_FALSE(c_s < c_s);
	CHECK_FALSE(d_s < c_s);
	CHECK(c_s < d_s);
	CHECK(c_s <= c_s);
	CHECK_FALSE(d_s <= c_s);
	CHECK(c_s <= d_s);
	CHECK_FALSE(c_s > c_s);
	CHECK(d_s > c_s);
	CHECK_FALSE(c_s > d_s);
	CHECK(c_s >= c_s);
	CHECK(d_s >= c_s);
	CHECK_FALSE(c_s >= d_s);

	CHECK_FALSE(a_s < c_s);
	CHECK_FALSE(d_s < a_s);
	CHECK(a_s < d_s);
	CHECK(c_s <= a_s);
	CHECK_FALSE(d_s <= a_s);
	CHECK(a_s <= d_s);
	CHECK_FALSE(a_s > c_s);
	CHECK(d_s > a_s);
	CHECK_FALSE(a_s > d_s);
	CHECK(c_s >= a_s);
	CHECK(d_s >= a_s);
	CHECK_FALSE(a_s >= d_s);

	constexpr percent p(2.0);
	constexpr percent pp(5);

	CHECK_FALSE(p < p);
	CHECK_FALSE(pp < p);
	CHECK(p < pp);
	CHECK(p <= p);
	CHECK_FALSE(pp <= p);
	CHECK(p <= pp);
	CHECK_FALSE(p > p);
	CHECK(pp > p);
	CHECK_FALSE(p > pp);
	CHECK(p >= p);
	CHECK(pp >= p);
	CHECK_FALSE(p >= pp);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeMixedRelational", "[wpimath][units]")
{
	constexpr meters a_m(0.0);
	constexpr feet   a_f(meters{1.0});

	CHECK_FALSE(a_f < a_m);
	CHECK(a_m < a_f);
	CHECK_FALSE(a_f <= a_m);
	CHECK(a_m <= a_f);
	CHECK(a_f > a_m);
	CHECK_FALSE(a_m > a_f);
	CHECK(a_f >= a_m);
	CHECK_FALSE(a_m >= a_f);

	constexpr feet   b_f(0);
	constexpr meters b_m(1);

	CHECK_FALSE(b_m < b_f);
	CHECK(b_f < b_m);
	CHECK_FALSE(b_m <= b_f);
	CHECK(b_f <= b_m);
	CHECK(b_m > b_f);
	CHECK_FALSE(b_f > b_m);
	CHECK(b_m >= b_f);
	CHECK_FALSE(b_f >= b_m);

	CHECK_FALSE(a_m < b_f);
	CHECK_FALSE(a_f < b_m);
	CHECK(b_f <= a_m);
	CHECK(b_m <= a_f);
	CHECK_FALSE(a_m > b_f);
	CHECK_FALSE(a_f > b_m);
	CHECK(b_f >= a_m);
	CHECK(b_m >= a_f);

	constexpr percent           p(1.0);
	constexpr parts_per_million pp(20000);

	CHECK_FALSE(p < p);
	CHECK_FALSE(pp < p);
	CHECK(p < pp);
	CHECK(p <= p);
	CHECK_FALSE(pp <= p);
	CHECK(p <= pp);
	CHECK_FALSE(p > p);
	CHECK(pp > p);
	CHECK_FALSE(p > pp);
	CHECK(p >= p);
	CHECK(pp >= p);
	CHECK_FALSE(p >= pp);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeArithmeticOperatorReturnType", "[wpimath][units]")
{
	percent<int> pcnt;
	meters<int>  length;

	constexpr dimensionless dim{1};
	constexpr auto          test = dim - 0;
	static_assert(detail::has_dimension_of<decltype(dim - 0), dimension::dimensionless>::value);
	CHECK(1 == dim);
	CHECK(test == dim);

	static_assert(std::is_same_v<dimensionless<int>, decltype(+dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(+pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(+length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(-dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(-pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(-length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim + 0)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(0 + dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim + dim)>);
	static_assert(std::is_same_v<percent<double>, decltype(pcnt + 0)>);
	static_assert(std::is_same_v<percent<double>, decltype(0 + pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt + pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length + length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - 0)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(0 - dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - dim)>);
	static_assert(std::is_same_v<percent<double>, decltype(pcnt - 0)>);
	static_assert(std::is_same_v<percent<double>, decltype(0 - pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt - pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length - length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(1 * dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * dim)>);
	// static_assert(std::is_same_v<percent<int>, decltype(pcnt * 1)>);
	// static_assert(std::is_same_v<percent<int>, decltype(1 * pcnt)>);
	static_assert(std::is_same_v<unit<conversion_factor<std::ratio<1, 10000>, wpi::units::dimension::dimensionless>, int>, decltype(pcnt * pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length * 1)>);
	static_assert(std::is_same_v<meters<int>, decltype(1 * length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length * dim)>);
	static_assert(std::is_same_v<meters<int>, decltype(dim * length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length * pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(pcnt * length)>);
	static_assert(std::is_same_v<square_meters<int>, decltype(length * length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim / 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(1 / dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim / dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt / 1)>);
	static_assert(std::is_same_v<wpi::units::dimensionless<double>, decltype(1 / pcnt)>);
	static_assert(std::is_same_v<wpi::units::dimensionless<double>, decltype(pcnt / pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length / 1)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(1 / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / dim)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(dim / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / pcnt)>);
	static_assert(std::is_convertible_v<decltype(pcnt / length), unit<inverse<meters<>>, double>>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(length / length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim % 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim % dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt % 1)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt % pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length % 1)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % dim)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % length)>);
}

// Regression guards for the class-based named-unit refactor: diagnostics/traits must report the FRIENDLY named
// type, and the trait/std specializations must see through the derived named class (not decay to the plain unit<...>
// base or hard-error). These lock in the behavior the coverage audit flagged as correct-but-untested.
TEST_CASE_METHOD(UnitType, "UnitType namedUnitReportedTypeIsPreserved", "[wpimath][units]")
{
	// (1) arithmetic RESULTS report the named type, not the equivalent-but-unnamed unit<strong_t<...>>.
	static_assert(std::is_same_v<decltype(meters<double>(2) * meters<double>(2)), square_meters<double>>);
	static_assert(std::is_same_v<decltype(meters<double>(10) / seconds<double>(2)), meters_per_second<double>>);
	static_assert(std::is_same_v<decltype(1.0 / seconds<double>(1)), hertz<double>>);       // inverse -> named
	static_assert(std::is_same_v<decltype(pow<3>(meters<double>(1))), cubic_meters<double>>);

	// (2) unit-math functions PRESERVE the named type on a dimensioned named input (audit: value-tested only before).
	static_assert(std::is_same_v<decltype(floor(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(round(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(trunc(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(hypot(meters<double>(3), meters<double>(4))), meters<double>>);

	// (3) traits see through the derived named class.
	static_assert(std::is_same_v<traits::replace_underlying_t<meters<int>, double>, meters<double>>);
	static_assert(std::is_same_v<std::common_type_t<meters<int>, meters<double>>, meters<double>>);

	// (4) C1: std::numeric_limits<Named> returns the NAMED type (value AND type), not the plain base.
	static_assert(std::is_same_v<decltype(std::numeric_limits<meters<double>>::max()), meters<double>>);
	static_assert(std::is_same_v<decltype(std::numeric_limits<meters<double>>::lowest()), meters<double>>);
	CHECK(std::numeric_limits<meters<double>>::max().to_linearized() == std::numeric_limits<double>::max());

	// (5) C2: common_type<dimensioned-named, scalar> is SFINAE-EMPTY (no `type`), exactly like the plain unit<...>
	//     form — never a hard error. A dimensionless-named + scalar still HAS a common type (interchangeable).
	static_assert(!detail::is_detected_v<common_type_of, meters<double>, double>, "dimensioned named + scalar: no common type (SFINAE-safe)");
	static_assert(!detail::is_detected_v<common_type_of, unit<conversion_factor<std::ratio<1>, dimension::length>, double>, double>, "plain dimensioned + scalar: also none (parity)");
	static_assert(detail::is_detected_v<common_type_of, percent<double>, double>, "dimensionless named + scalar: has a common type");

	// (6) abbreviation()/name() members resolve on the named form (incl. a COMPOUND named unit, per audit).
	CHECK(std::string_view("m") == meters<double>(1).abbreviation());
	CHECK(std::string_view("meters") == meters<double>(1).name());
	CHECK(std::string_view("mps") == meters_per_second<double>(1).abbreviation());

	// (6a) minutes is defined via UNIT_ADD_WITH_PLURAL_TAG; that macro must still register the named class
	// so name()/abbreviation() resolve (a regression guard: they previously returned null, and a minutes
	// value streamed as its base unit "90 s" and could null-deref on name()).
	CHECK(std::string_view("min") == minutes<double>(1).abbreviation());
	CHECK(std::string_view("minutes") == minutes<double>(1).name());
	CHECK(std::string("90 min") == to_string(minutes<double>(90)));

	// (7) the remaining unit-math functions PRESERVE the named type on a named input (audit: value-only before).
	static_assert(std::is_same_v<decltype(min(meters<double>(1), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(max(meters<double>(1), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(fmod(meters<double>(5), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(copysign(meters<double>(3), -1.0)), meters<double>>);
	static_assert(std::is_same_v<decltype(fabs(meters<double>(-3))), meters<double>>);
	static_assert(std::is_same_v<decltype(abs(meters<double>(-3))), meters<double>>);
	static_assert(std::is_same_v<decltype(sqrt(square_meters<double>(4))), meters<double>>);

	// (8) a NON-registered derived CF stays the plain unit<...> (identity rewrap): dividing two unlike named units
	//     whose quotient has no named class must NOT invent a name. meters/kilograms has no named unit.
	static_assert(!detail::is_named_unit_v<decltype(meters<double>(1) / kilograms<double>(1))>,
		"a derived CF with no registered named class stays the plain unit<...>");
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeAddition", "[wpimath][units]")
{
	// units
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	meters                                               c_m{0.0};
	constexpr meters                                     f_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> g_m(f_m);

	double d_m = meters(b_ft).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m, 5.0e-5));
	d_m = meters(g_m).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m, 5.0e-5));

	c_m = a_m + b_ft;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = f_m + g_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = a_m + g_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = f_m + b_ft;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	c_m = b_ft + meters<double>(3);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = g_m + meters<int>(3);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = b_ft + meters<int>(3);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = g_m + meters<double>(3);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	feet<double> e_ft = b_ft + meters<double>(3);
	CHECK_THAT(13.12336, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = g_m + meters<int>(3);
	CHECK_THAT(13.12336, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = b_ft + meters<int>(3);
	CHECK_THAT(13.12336, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = g_m + meters<double>(3);
	CHECK_THAT(13.12336, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));

	// dimensionless
	dimensionless<double> result = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<int>(1) + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<double>(1.0) + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<int>(1) + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));

	result = dimensionless<double>(1.0) + 1.0;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<int>(1) + 1;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<double>(1.0) + 1;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = dimensionless<int>(1) + 1.0;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));

	result = 1.0 + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = 1 + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = 1.0 + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));
	result = 1 + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(result, 5.0e-6));

	d_m = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<int>(1) + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<double>(1.0) + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<int>(1) + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	d_m = dimensionless<double>(1.0) + 1.0;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<int>(1) + 1;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<double>(1.0) + 1;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = dimensionless<int>(1) + 1.0;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	d_m = 1.0 + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1. + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1.0 + dimensionless<int>(1);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1 + dimensionless<double>(1.0);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	// concentration
	percent<double> pResult = percent<double>(1.0) + percent<double>(1.0);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(2_pct == pResult);
	pResult = percent<int>(1) + percent<int>(1);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(2_pct == pResult);
	pResult = percent<double>(1.0) + percent<int>(1);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(2_pct == pResult);
	pResult = percent<int>(1) + percent<double>(1.0);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(2_pct == pResult);

	pResult = percent<>(1.0) + 1.0;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = percent<int>(1) + 1;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = percent<double>(1.0) + 1;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = percent<int>(1) + 1.0;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);

	pResult = 1.0 + percent<double>(1.0);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = 1 + percent<int>(1);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = 1.0 + percent<int>(1);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);
	pResult = 1 + percent<double>(1.0);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	CHECK(101_pct == pResult);

	d_m = percent<double>(1.0) + percent<double>(1.0);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<int>(1) + percent<int>(1);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<double>(1.0) + percent<int>(1);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<int>(1) + percent<double>(1.0);
	CHECK_THAT(0.02, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	d_m = percent<double>(1.0) + 1.0;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<int>(1) + 1;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<double>(1.0) + 1;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = percent<int>(1) + 1.0;
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	d_m = 1.0 + percent<double>(1.0);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1. + percent<int>(1);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1.0 + percent<int>(1);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));
	d_m = 1 + percent<double>(1.0);
	CHECK_THAT(1.01, Catch::Matchers::WithinAbs(d_m, 5.0e-6));

	percent<double> p = 5_pct + 25_pct;
	CHECK(30_pct == p);
	CHECK_THAT(0.3, Catch::Matchers::WithinAbs(p.value(), 5.0e-6));
	CHECK(30 == p.raw());

	percent<double> z = 5_pct + 10000_ppm;
	CHECK(6_pct == z);
	CHECK_THAT(0.06, Catch::Matchers::WithinAbs(z.value(), 5.0e-6));
	CHECK(6 == z.raw());
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeUnaryAddition", "[wpimath][units]")
{
	meters<double> a_m(1.0);

	CHECK(++a_m == meters<double>(2));
	CHECK(a_m++ == meters<double>(2));
	CHECK(a_m == meters<double>(3));
	CHECK(+a_m == meters<double>(3));
	CHECK(a_m == meters<double>(3));

	dBW<double> b_dBW(1.0);

	CHECK(++b_dBW == dBW<double>(2));
	CHECK(b_dBW++ == dBW<double>(2));
	CHECK(b_dBW == dBW<double>(3));
	CHECK(+b_dBW == dBW<double>(3));
	CHECK(b_dBW == dBW<double>(3));

	percent<double> c_pct(1.0);

	CHECK(++c_pct == percent<double>(2));
	CHECK(c_pct++ == percent<double>(2));
	CHECK(c_pct == percent<double>(3));
	CHECK(+c_pct == percent<double>(3));
	CHECK(c_pct == percent<double>(3));
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeSubtraction", "[wpimath][units]")
{
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	meters                                               c_m{0.0};
	constexpr meters                                     f_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> g_m(f_m);

	c_m = a_m - b_ft;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = f_m - g_m;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = a_m - g_m;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = f_m - b_ft;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	c_m = b_ft - meters<double>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = g_m - meters<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = b_ft - meters<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = g_m - meters<double>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	feet<double> e_ft = b_ft - meters<double>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = g_m - meters<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = b_ft - meters<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));
	e_ft = g_m - meters<double>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(e_ft.value(), 5.0e-6));

	// dimensionless
	dimensionless<double> sResult = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<int>(1) - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<double>(1.0) - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<int>(1) - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));

	sResult = dimensionless<double>(1.0) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<int>(1) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<double>(1.0) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = dimensionless<int>(1) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));

	sResult = 1.0 - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = 1 - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = 1.0 - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));
	sResult = 1 - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sResult, 5.0e-6));

	double dim = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<int>(1) - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<double>(1.0) - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<int>(1) - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));

	dim = dimensionless<double>(1.0) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<int>(1) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<double>(1.0) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = dimensionless<int>(1) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));

	dim = 1.0 - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1 - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1.0 - dimensionless<int>(1);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1 - dimensionless<double>(1.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));

	// concentration
	percent<double> pResult = percent<double>(5) - percent<double>(1.0);
	CHECK_THAT(0.04, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<int>(5) - percent<int>(1);
	CHECK_THAT(4_pct, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<double>(5.0) - percent<int>(1);
	CHECK_THAT(0.04, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<int>(5) - percent<double>(1.0);
	CHECK_THAT(4_pct, Catch::Matchers::WithinAbs(pResult, 5.0e-6));

	pResult = percent<double>(100.0) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<int>(100) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<double>(100.0) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = percent<int>(100) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));

	pResult = 1.0 - percent<double>(100.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = 1 - percent<int>(100);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = 1.0 - percent<int>(100);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));
	pResult = 1 - percent<double>(100.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(pResult, 5.0e-6));

	dim = percent<double>(100.0) - percent<double>(10.0);
	CHECK_THAT(0.9, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<int>(100) - percent<int>(10);
	CHECK_THAT(0.9, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<double>(100.0) - percent<int>(10);
	CHECK_THAT(0.9, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<int>(100) - percent<double>(10.0);
	CHECK_THAT(0.9, Catch::Matchers::WithinAbs(dim, 5.0e-6));

	dim = percent<double>(100.0) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<int>(100) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<double>(100.0) - 1;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = percent<int>(100) - 1.0;
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));

	dim = 1.0 - percent<double>(100.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1 - percent<int>(100);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1.0 - percent<int>(100);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
	dim = 1 - percent<double>(100.0);
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(dim, 5.0e-6));
}

// The result of a same-dimension +/- is expressed in the LEFT operand's unit (the caller controls it by
// operand order), so the value reads in the unit they wrote and no anonymous sub-unit is conjured. The
// left-operand's underlying is widened to the common lossless unit only when it is integral and cannot hold
// the right operand without truncation — the case where integer exactness requires the finest common unit.
TEST_CASE_METHOD(UnitType, "UnitType arithmeticResultIsLeftOperandUnit", "[wpimath][units]")
{
	using namespace wpi::units::length;
	using namespace wpi::units::mass;

	// Floating point: the result is the LEFT operand's unit, both orders — caller controls it by ordering.
	static_assert(std::is_same_v<decltype(meters<double>(1) - feet<double>(1)), meters<double>>);
	static_assert(std::is_same_v<decltype(feet<double>(1) - meters<double>(1)), feet<double>>);
	static_assert(std::is_same_v<decltype(meters<double>(1) + feet<double>(1)), meters<double>>);
	static_assert(std::is_same_v<decltype(kilometers<double>(1) - meters<double>(1)), kilometers<double>>);

	// The value reads correctly in that left-operand unit.
	CHECK_THAT(7.73203815, Catch::Matchers::WithinAbs((kilograms<double>(10) - wpi::units::mass::pounds<double>(5)).value(), 1.0e-6));   // 10 kg - 5 lb, in kg (LHS)
	CHECK_THAT(9.0, Catch::Matchers::WithinAbs((meters<double>(10) - feet<double>(3.280839895013123)).value(), 1.0e-9));  // 10 m - 1 m in m

	// A named result carries its friendly name (not an anonymous unit): abbreviation is non-empty.
	CHECK(std::string_view("") != (meters<double>(1) - feet<double>(1)).abbreviation());
	CHECK(std::string_view("m") == (meters<double>(1) - feet<double>(1)).abbreviation());

	// Integer, commensurable: the left operand cannot hold the finer right operand losslessly, so the result
	// reconciles to the finer real named unit (meters), exact.
	static_assert(std::is_same_v<decltype(kilometers<int>(1) - meters<int>(500)), meters<int>>);
	CHECK(500 == (kilometers<int>(1) - meters<int>(500)).value());

	// Integer, incommensurable (meters vs feet): no real named unit holds the result without truncation, so it
	// falls to the finest common (anonymous) unit — the exact reconciliation. The VALUE stays correct.
	{
		const auto d = meters<int>(3) - feet<int>(1);   // 3 m - 1 ft, exact in the common sub-unit
		CHECK_THAT(2.6952, Catch::Matchers::WithinAbs(meters<double>(d).value(), 1.0e-4));
	}
}

// The common type of two same-dimension units recovers a friendly name even when the finer operand's unit is
// registered as a composed conversion factor (centimeters is centi<meters>, minutes is 60 seconds): the
// reconciliation result names centimeters / minutes rather than an anonymous sub-unit, while a cross-kind pair
// that shares a dimension and ratio (torque's newton_meters and energy's joules) stays symmetric and is not
// renamed from one kind to the other.
TEST_CASE_METHOD(UnitType, "UnitType commonTypeRecoversComposedName", "[wpimath][units]")
{
	using namespace wpi::units::length;
	using namespace wpi::units::time;

	using m_cm  = std::common_type_t<meters<double>, centimeters<double>>;
	using cm_m  = std::common_type_t<centimeters<double>, meters<double>>;
	using hr_min = std::common_type_t<hours<double>, minutes<double>>;
	CHECK(std::string_view("cm") == m_cm().abbreviation());
	CHECK(std::string_view("cm") == cm_m().abbreviation());
	CHECK(std::string_view("min") == hr_min().abbreviation());   // hours (3600 s) and minutes (60 s) reconcile to minutes, the finer

	// common_type stays symmetric (a type function must be); recovery never breaks that.
	static_assert(std::is_same_v<std::common_type_t<meters<double>, centimeters<double>>,
								 std::common_type_t<centimeters<double>, meters<double>>>);
	// Cross-kind equivalent pair (same dimension AND ratio) is NOT renamed and stays order-independent.
	static_assert(std::is_same_v<std::common_type_t<wpi::units::torque::newton_meters<double>, wpi::units::energy::joules<double>>,
								 std::common_type_t<wpi::units::energy::joules<double>, wpi::units::torque::newton_meters<double>>>);

	// The name recovery preserves the RATIO (integer exactness): comparison across scales is unaffected.
	static_assert(wpi::units::length::kilometers<int>(1) == wpi::units::length::meters<int>(1000));
	static_assert(!(wpi::units::length::meters<int>(1) == wpi::units::length::feet<int>(3)));
}

// Affine (offset-carrying) units: the difference of two absolute temperatures is a DELTA — the datum
// offsets cancel and the result must not re-apply an offset. Previously celsius(0) - kelvin(0) read 546.30 K
// (the +273.15 offset was re-applied); the delta is 273.15 K. Absolute affine ADDITION is disabled (no
// physical meaning), so only subtraction is exercised here.
TEST_CASE_METHOD(UnitType, "UnitType affineTemperatureSubtractionIsADelta", "[wpimath][units]")
{
	using namespace wpi::units::temperature;
	CHECK_THAT(273.15, Catch::Matchers::WithinAbs(kelvin<double>(celsius<double>(0.0) - kelvin<double>(0.0)).value(), 5.0e-11));
	CHECK_THAT(100.0, Catch::Matchers::WithinAbs(kelvin<double>(celsius<double>(100.0) - fahrenheit<double>(32.0)).value(), 5.0e-11));
	CHECK_THAT(15.0, Catch::Matchers::WithinAbs(kelvin<double>(celsius<double>(20.0) - celsius<double>(5.0)).value(), 5.0e-11));
	CHECK_THAT(56.0, Catch::Matchers::WithinAbs(kelvin<double>(fahrenheit<double>(212.0) - fahrenheit<double>(111.2)).value(), 5.0e-9));
	// The result of an affine subtraction is a non-affine (delta) unit: converting it applies no offset.
	static_assert(!traits::is_affine_unit_v<decltype(celsius<double>(0.0) - kelvin<double>(0.0))>,
		"an affine temperature difference must be a non-affine delta type");
	// A non-affine same-dimension subtraction/addition is unaffected.
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(meters<double>(meters<double>(5.0) - meters<double>(3.0)).value(), 5.0e-12));
}

// Compound assignment on an affine unit treats the rhs as a RELATIVE delta and moves the absolute point in
// place ("warm/cool by N degrees"), staying in the lhs unit. This is the point-centric convenience: the
// point/delta distinction stays a quiet detail rather than a pervasive type calculus.
TEST_CASE_METHOD(UnitType, "UnitType affineTemperatureCompoundAssignmentMovesPoint", "[wpimath][units]")
{
	using namespace wpi::units::temperature;
	celsius<double> a(20.0);
	a += celsius<double>(5.0);
	CHECK_THAT(25.0, Catch::Matchers::WithinAbs(a.value(), 5.0e-12));    // warmed by 5 degrees, still absolute celsius
	a -= celsius<double>(10.0);
	CHECK_THAT(15.0, Catch::Matchers::WithinAbs(a.value(), 5.0e-12));    // cooled by 10 degrees

	// The result stays an absolute affine point (not converted to a delta type).
	static_assert(traits::is_affine_unit_v<decltype(a)>, "compound assignment keeps the affine point type");

	// Non-affine compound assignment is unchanged.
	meters<double> m(5.0);
	m += meters<double>(3.0);
	CHECK_THAT(8.0, Catch::Matchers::WithinAbs(m.value(), 5.0e-12));
	m -= meters<double>(2.0);
	CHECK_THAT(6.0, Catch::Matchers::WithinAbs(m.value(), 5.0e-12));
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeUnarySubtraction", "[wpimath][units]")
{
	meters<double> a_m(4.0);

	CHECK(--a_m == meters<double>(3));
	CHECK(a_m-- == meters<double>(3));
	CHECK(a_m == meters<double>(2));
	CHECK(-a_m == meters<double>(-2));
	CHECK(a_m == meters<double>(2));

	dBW<double> b_dBW(4.0);

	CHECK(--b_dBW == dBW<double>(3));
	CHECK(b_dBW-- == dBW<double>(3));
	CHECK(b_dBW == dBW<double>(2));
	CHECK(-b_dBW == dBW<double>(-2));
	CHECK(b_dBW == dBW<double>(2));

	percent<double> c_pct(4.0);

	CHECK(--c_pct == percent<double>(3));
	CHECK(c_pct-- == percent<double>(3));
	CHECK(c_pct == percent<double>(2));
	CHECK(-c_pct == percent<double>(-2));
	CHECK(c_pct == percent<double>(2));
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeMultiplication", "[wpimath][units]")
{
	constexpr meters                                     a_m(1.0);
	constexpr meters                                     b_m(2.0);
	constexpr feet                                       a_ft(3.28084);
	constexpr meters<int>                                d_m(1), e_m(2);
	constexpr std::common_type_t<meters<int>, feet<int>> f(d_m);

	auto c_m2 = a_m * b_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = d_m * e_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = a_m * e_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = d_m * b_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));

	c_m2 = b_m * meters<double>(2);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = e_m * meters<int>(2);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = b_m * meters<int>(2);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = e_m * meters<double>(2);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));

	c_m2 = b_m * a_ft;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = e_m * f;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = b_m * f;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = e_m * a_ft;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));

	auto c_m = b_m * 2.0;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = e_m * 2;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = b_m * 2;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = e_m * 2.0;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	c_m = 2.0 * b_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = 2 * e_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = 2.0 * e_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));
	c_m = 2 * b_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(c_m.value(), 5.0e-5));

	double convert = dimensionless<double>(3.14);
	CHECK_THAT(3.14, Catch::Matchers::WithinAbs(convert, 5.0e-5));
	convert = dimensionless<int>(3);
	CHECK_THAT(3, Catch::Matchers::WithinAbs(convert, 5.0e-5));

	// dimensionless
	dimensionless<double> sResult = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<int>(5) * dimensionless<int>(4);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<double>(5.0) * dimensionless<int>(4);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<int>(5) * dimensionless<double>(4.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));

	sResult = dimensionless<double>(5.0) * 4.0;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<int>(5) * 4;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<double>(5.0) * 4;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = dimensionless<int>(5) * 4.0;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));

	sResult = 4.0 * dimensionless<double>(5.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = 4 * dimensionless<int>(5);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = 4.0 * dimensionless<int>(5);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));
	sResult = 4 * dimensionless<double>(5.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(sResult.value(), 5.0e-5));

	double result = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<int>(5) * dimensionless<int>(4);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<double>(5.0) * dimensionless<int>(4);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<int>(5) * dimensionless<double>(4.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));

	result = dimensionless<double>(5.0) * 4.0;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<int>(5) * 4;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<double>(5.0) * 4;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = dimensionless<int>(5) * 4.0;
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));

	result = 4.0 * dimensionless<double>(5.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4 * dimensionless<int>(5);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4.0 * dimensionless<int>(5);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4 * dimensionless<double>(5.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result, 5.0e-5));

	// dimensionless result
	result = 60_km / 400_mm;
	CHECK(150000 == result);

	// concentration
	percent<double> pResult = percent<double>(5.0) * percent<double>(4.0);
	CHECK(0.2_pct == pResult);
	CHECK(0.002 == pResult.value());
	pResult = percent<int>(5) * percent<int>(4);
	CHECK(0.2_pct == pResult);
	CHECK(0.002 == pResult.value());
	pResult = percent<double>(5.0) * percent<int>(4);
	CHECK(0.2_pct == pResult);
	CHECK(0.002 == pResult.value());
	pResult = percent<int>(5) * percent<double>(4.0);
	CHECK(0.2_pct == pResult);
	CHECK(0.002 == pResult.value());

	pResult = percent<double>(5.0) * 4.0;
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = percent<int>(5) * 4;
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = percent<double>(5.0) * 4;
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = percent<int>(5) * 4.0;
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());

	pResult = 4.0 * percent<double>(5.0);
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = 4 * percent<int>(5);
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = 4.0 * percent<int>(5);
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());
	pResult = 4 * percent<double>(5.0);
	CHECK(20.0_pct == pResult);
	CHECK(0.2 == pResult.value());

	result = percent<double>(5.0) * percent<double>(4.0);
	CHECK_THAT(0.002, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<int>(5) * percent<int>(4);
	CHECK_THAT(0.002, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<double>(5.0) * percent<int>(4);
	CHECK_THAT(0.002, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<int>(5) * percent<double>(4.0);
	CHECK_THAT(0.002, Catch::Matchers::WithinAbs(result, 5.0e-5));

	result = percent<double>(5.0) * 4.0;
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<int>(5) * 4;
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<double>(5.0) * 4;
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = percent<int>(5) * 4.0;
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));

	result = 4.0 * percent<double>(5.0);
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4 * percent<int>(5);
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4.0 * percent<int>(5);
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));
	result = 4 * percent<double>(5.0);
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(result, 5.0e-5));

	auto value = 10.0_pct * 100.0_m;
	CHECK(value == 10.0_m);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeMixedUnitMultiplication", "[wpimath][units]")
{
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	constexpr unit<inverse<meters<double>>>              i_m(2.0);
	constexpr meters                                     b_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> f(b_m);
	constexpr unit<inverse<meters<double>>, int>         i_i_m(2);

	// resultant unit is square of the common type unit
	// you can get whatever (compatible) type you want if you ask explicitly
	unit<squared<meters<double>>> c_m2 = a_m * b_ft;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = b_m * f;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = a_m * f;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = b_m * b_ft;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));

	unit<squared<feet<double>>> c_ft2 = b_ft * a_m;
	CHECK_THAT(10.7639111056, Catch::Matchers::WithinAbs(c_ft2.value(), 5.0e-7));
	c_ft2 = f * b_m;
	CHECK_THAT(10.7639111056, Catch::Matchers::WithinAbs(c_ft2.value(), 5.0e-6));
	c_ft2 = b_ft * b_m;
	CHECK_THAT(10.7639111056, Catch::Matchers::WithinAbs(c_ft2.value(), 5.0e-7));
	c_ft2 = f * a_m;
	CHECK_THAT(10.7639111056, Catch::Matchers::WithinAbs(c_ft2.value(), 5.0e-6));

	square_meters<double> d_m2 = b_ft * a_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m2.value(), 5.0e-5));
	d_m2 = f * b_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m2.value(), 5.0e-5));
	d_m2 = b_ft * b_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m2.value(), 5.0e-5));
	d_m2 = f * a_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(d_m2.value(), 5.0e-5));

	// a unit times a scalar ends up with the same units.
	meters<double> e_m = a_m * dimensionless<double>(3.0);
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = b_m * dimensionless<int>(3);
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = a_m * dimensionless<int>(3);
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = b_m * dimensionless<double>(3.0);
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));

	e_m = dimensionless<double>(4.0) * a_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = dimensionless<int>(4) * b_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = dimensionless<double>(4) * b_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));
	e_m = dimensionless<int>(4) * a_m;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(e_m.value(), 5.0e-5));

	// unit times its inverse results in a dimensionless
	dimensionless<double> dim = a_m * i_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(dim, 5.0e-5));
	dim = b_m * i_i_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(dim, 5.0e-5));
	dim = a_m * i_i_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(dim, 5.0e-5));
	dim = b_m * i_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(dim, 5.0e-5));

	c_m2 = b_ft * meters<double>(2);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = f * meters<int>(2);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = b_ft * meters<int>(2);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));
	c_m2 = f * meters<double>(2);
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(c_m2.value(), 5.0e-5));

	unit<squared<feet<double>>> e_ft2 = b_ft * meters<double>(3);
	CHECK_THAT(32.2917333168, Catch::Matchers::WithinAbs(e_ft2.value(), 5.0e-6));
	e_ft2 = f * meters<int>(3);
	CHECK_THAT(32.2917333168, Catch::Matchers::WithinAbs(e_ft2.value(), 5.0e-6));
	e_ft2 = b_ft * meters<int>(3);
	CHECK_THAT(32.2917333168, Catch::Matchers::WithinAbs(e_ft2.value(), 5.0e-6));
	e_ft2 = f * meters<double>(3);
	CHECK_THAT(32.2917333168, Catch::Matchers::WithinAbs(e_ft2.value(), 5.0e-6));

	auto metersPerSecond = meters<double>(10.0) * unit<inverse<seconds<double>>>(1.0);
	CHECK(metersPerSecond == meters_per_second<double>(10));
	metersPerSecond = meters<int>(10) * unit<inverse<seconds<double>>, int>(1);
	CHECK(metersPerSecond == meters_per_second<double>(10));
	metersPerSecond = meters<double>(10.0) * unit<inverse<seconds<double>>, int>(1);
	CHECK(metersPerSecond == meters_per_second<double>(10));
	metersPerSecond = meters<int>(10) * unit<inverse<seconds<double>>>(1.0);
	CHECK(metersPerSecond == meters_per_second<double>(10));
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeDimensionlessMultiplication", "[wpimath][units]")
{
	meters<double> a_m(1.0);

	auto result_m = dimensionless<double>(3.0) * a_m;
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(result_m.value(), 5.0e-5));

	result_m = a_m * dimensionless<double>(4.0);
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(result_m.value(), 5.0e-5));

	result_m = 3.0 * a_m;
	CHECK_THAT(3.0, Catch::Matchers::WithinAbs(result_m.value(), 5.0e-5));

	result_m = a_m * 4.0;
	CHECK_THAT(4.0, Catch::Matchers::WithinAbs(result_m.value(), 5.0e-5));

	bool isSame = std::is_same_v<decltype(result_m), meters<double>>;
	CHECK(isSame);

	kilometers<double> dist = 50.0_pct * 2000_m;
	CHECK(dist == 1_km);
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeDivision", "[wpimath][units]")
{
	constexpr meters<double>                             a_m(1.0), b_m(2.0);
	constexpr feet                                       a_ft(3.28084);
	constexpr seconds                                    a_sec(10.0);
	constexpr meters<int>                                d_m(1), e_m(2);
	constexpr std::common_type_t<meters<int>, feet<int>> j(d_m);
	constexpr seconds                                    b_sec(10);
	bool                                                 isSame;

	auto c = a_m / a_ft;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = d_m / j;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = a_m / j;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = d_m / a_ft;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	CHECK(isSame);

	c = a_m / b_m;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = d_m / e_m;
	CHECK(0 == c);
	c = a_m / e_m;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = d_m / b_m;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	CHECK(isSame);

	c = a_ft / a_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = j / d_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = a_ft / d_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = j / a_m;
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(c, 5.0e-5));
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	CHECK(isSame);

	c = dimensionless<double>(1.0) / 2.0;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = dimensionless<int>(1) / 2;
	CHECK(0 == c);
	c = dimensionless<double>(1.0) / 2;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = dimensionless<int>(1) / 2.0;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	CHECK(isSame);

	c = 1.0 / dimensionless<double>(2.0);
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = 1 / dimensionless<int>(2);
	CHECK(0 == c);
	c = 1.0 / dimensionless<int>(2);
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	c = 1 / dimensionless<double>(2.0);
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(c, 5.0e-5));
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	CHECK(isSame);

	double d_dim = dimensionless<double>(1.0) / 2.0;
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(d_dim, 5.0e-5));

	auto e_mps = a_m / a_sec;
	CHECK_THAT(0.1, Catch::Matchers::WithinAbs(e_mps.value(), 5.0e-5));
	e_mps = d_m / b_sec;
	CHECK(0 == e_mps.value());
	e_mps = a_m / b_sec;
	CHECK_THAT(0.1, Catch::Matchers::WithinAbs(e_mps.value(), 5.0e-5));
	e_mps = d_m / a_sec;
	CHECK_THAT(0.1, Catch::Matchers::WithinAbs(e_mps.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(e_mps), meters_per_second<double>>;
	CHECK(isSame);

	auto f = a_m / 8.0;
	CHECK_THAT(0.125, Catch::Matchers::WithinAbs(f.value(), 5.0e-5));
	f = d_m / 8;
	CHECK(0 == f.value());
	f = a_m / 8;
	CHECK_THAT(0.125, Catch::Matchers::WithinAbs(f.value(), 5.0e-5));
	f = d_m / 8.0;
	CHECK_THAT(0.125, Catch::Matchers::WithinAbs(f.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(f), meters<double>>;
	CHECK(isSame);

	auto invMeters = 4.0 / b_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(invMeters.value(), 5.0e-5));
	invMeters = 4 / e_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(invMeters.value(), 5.0e-5));
	invMeters = 4.0 / e_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(invMeters.value(), 5.0e-5));
	invMeters = 4 / b_m;
	CHECK_THAT(2.0, Catch::Matchers::WithinAbs(invMeters.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(invMeters), unit<inverse<meters<double>>>>;
	CHECK(isSame);

	auto                      miles_speed = miles<double>(60.0) / hours<double>(1.0);
	meters_per_second<double> speed       = miles_speed;
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(speed.value(), 5.0e-5));
	speed = miles<int>(60) / hours<int>(1);
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(speed.value(), 5.0e-5));
	speed = miles<double>(60.0) / hours<int>(1);
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(speed.value(), 5.0e-5));
	speed = miles<int>(60) / hours<double>(1.0);
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(speed.value(), 5.0e-5));

	auto h_dim = 10.0_rad / 2.0_rad;
	CHECK_THAT(5, Catch::Matchers::WithinAbs(h_dim, 5.0e-5));
	h_dim = radians<int>(10) / radians<int>(2);
	CHECK_THAT(5, Catch::Matchers::WithinAbs(h_dim, 5.0e-5));
	h_dim = 10.0_rad / radians<int>(2);
	CHECK_THAT(5, Catch::Matchers::WithinAbs(h_dim, 5.0e-5));
	h_dim = radians<int>(10) / 2.0_rad;
	CHECK_THAT(5, Catch::Matchers::WithinAbs(h_dim, 5.0e-5));
	isSame = std::is_same_v<decltype(h_dim), dimensionless<double>>;
	CHECK(isSame);

	auto i = (3.0_N * 2.0_m) / 6.0_J;
	CHECK_THAT(1, Catch::Matchers::WithinAbs(i, 5.0e-5));
	i = (force::newtons<int>(3) * meters<int>(2)) / joules<int>(6);
	CHECK_THAT(1, Catch::Matchers::WithinAbs(i, 5.0e-5));
	i = (3.0_N * meters<int>(2)) / joules<int>(6);
	CHECK_THAT(1, Catch::Matchers::WithinAbs(i, 5.0e-5));
	i = (force::newtons<int>(3) * meters<int>(2)) / 6.0_J;
	CHECK_THAT(1, Catch::Matchers::WithinAbs(i, 5.0e-5));
	isSame = std::is_same_v<decltype(i), dimensionless<double>>;
	CHECK(isSame);

	unit<inverse<meters<double>>> k = 50.0_pct / 1.0_m;
	CHECK(k.value() == 0.5);
	meters<double> l = 10.0_km / 25.0_pct;
	CHECK(l == 40000.0_m);
	dimensionless<double> m_dim = 5.0_pct / 4.0_pct;
	CHECK(m_dim == 1.25);
	auto n = 5_pct / 4_pct;
	static_assert(std::is_same_v<wpi::units::dimensionless<double>, decltype(n)>);
	CHECK(n == 1.25);

	double o = 5.0 / 20.0_pct;
	CHECK(o == 25.0);
	o = 20.0_pct / 5.0;
	CHECK(o == 0.04);

	double p = 1.0 / pi;
	CHECK_THAT(0.3183098862, Catch::Matchers::WithinAbs(p, 5.0e-11));
}

TEST_CASE_METHOD(UnitType, "UnitType unitTypeModulo", "[wpimath][units]")
{
	constexpr meters     a_m(2200);
	constexpr meters     b_m(1800);
	constexpr kilometers a_km(2);

	constexpr auto c_m = a_m % b_m;
	CHECK(400 == c_m.value());
	static_assert(has_equivalent_conversion_factor(c_m, a_m));

	constexpr auto d_m = a_m % a_km;
	CHECK(200 == d_m.value());
	static_assert(has_equivalent_conversion_factor(d_m, a_m));

	// Coarser lhs: the result is the common (finer) unit, so the operator is not order-dependent. This
	// direction previously failed to compile (the result was declared as the lhs unit, and converting the
	// finer common result back to the coarser integer kilometers is lossy → the constructor was disabled).
	constexpr kilometers f_km(3);
	constexpr auto       g_m = f_km % b_m;    // 3000 m % 1800 m = 1200 m, in meters (the finer common unit)
	CHECK(1200 == g_m.value());
	static_assert(has_equivalent_conversion_factor(g_m, b_m));

	constexpr auto b_km = a_km % dimensionless<int>(3);
	CHECK(2 == b_km.value());
	static_assert(has_equivalent_conversion_factor(b_km, a_km));

	constexpr auto e_m = a_m % 2000;
	CHECK(200 == e_m.value());
	static_assert(has_equivalent_conversion_factor(e_m, a_m));

	constexpr dimensionless a_s(12);
	constexpr dimensionless b_s(5);

	constexpr auto c_s = a_s % b_s;
	CHECK(2 == c_s.value());
	static_assert(has_equivalent_conversion_factor(c_s, a_s));

	constexpr auto d_s = a_s % 20;
	CHECK(12 == d_s.value());
	static_assert(has_equivalent_conversion_factor(d_s, a_s));

	auto y = percent<int>(12) % percent<int>(5);
	CHECK(2_pct == y);
	static_assert(has_equivalent_conversion_factor(y, percent<int>(5)));

	y = percent<int>(5) % percent<int>(12);
	CHECK(5_pct == y);
	static_assert(has_equivalent_conversion_factor(y, percent<int>(5)));

	auto y2 = dimensionless<int>(15) % percent<int>(5);
	CHECK(0_pct == y2);

	auto y3 = percent<int>(29) % percent<int>(3);
	CHECK(2_pct == y3);

	auto z = percent<int>(12) % 20;
	CHECK(12_pct == z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	z = percent<int>(12) % 5;
	CHECK(2_pct == z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	z = percent<int>(12) % dimensionless<int>(5);
	CHECK(2_pct == z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	// Integer-percent iteration with modulo. `%` requires integral operands, so the modulus and remainder are
	// explicit percent<int> (a literal is floating-point); the iterator is likewise percent<int>. This exercises
	// that integer-backed ratio-dimensionless units support modulo arithmetic.
	std::vector<percent<int>> vec;
	for (percent<int> i(1); i <= 100_pct; ++i)
	{
		if (i % percent<int>(10) == percent<int>(0))
			vec.push_back(i);
	}
	CHECK(vec.size() == 10);
	CHECK(vec[0] == 10_pct);
	CHECK(vec[1] == 20_pct);
	CHECK(vec[2] == 30_pct);
	CHECK(vec[3] == 40_pct);
	CHECK(vec[4] == 50_pct);
	CHECK(vec[5] == 60_pct);
	CHECK(vec[6] == 70_pct);
	CHECK(vec[7] == 80_pct);
	CHECK(vec[8] == 90_pct);
	CHECK(vec[9] == 100_pct);
}

TEST_CASE_METHOD(UnitType, "UnitType compoundAssignmentAddition", "[wpimath][units]")
{
	// units
	meters<double> a(0.0);
	a += meters<double>(1.0);

	CHECK(meters<double>(1.0) == a);

	a += feet<double>(meters<double>(1));

	CHECK(meters<double>(2.0) == a);

	a += meters<int>(1);

	CHECK(meters<double>(3.0) == a);

	a += std::common_type_t<meters<int>, feet<int>>(meters<int>(1));

	CHECK(meters<double>(4.0) == a);

	meters<int> c(0);
	c += meters<int>(1);

	CHECK((meters<int>(1)) == c);

	c += kilometers<int>(1);

	CHECK((meters<int>(1001)) == c);

	// dimensionless
	dimensionless<double> b_dim(0);
	b_dim += dimensionless<double>(1.0);

	CHECK(dimensionless<double>(1.0) == b_dim);

	b_dim += 1.0;

	CHECK(dimensionless<double>(2.0) == b_dim);

	b_dim += dimensionless<int>(1);

	CHECK(dimensionless<double>(3.0) == b_dim);

	b_dim += 1;

	CHECK(dimensionless<double>(4.0) == b_dim);

	dimensionless<int> e_dim(0);
	e_dim += dimensionless<int>(1);

	CHECK((dimensionless<int>(1)) == e_dim);

	e_dim += 1;

	CHECK((dimensionless<int>(2)) == e_dim);

	// concentration
	percent<double> e_pct(0);
	e_pct += percent<double>(1.0);

	CHECK(percent<double>(1.0) == e_pct);

	e_pct += 1.0;

	CHECK(percent<double>(101.0) == e_pct);

	e_pct += percent<int>(1);

	CHECK(percent<double>(102.0) == e_pct);

	e_pct += 1;

	CHECK(percent<double>(202.0) == e_pct);

	percent<int> f(0);
	f += percent<int>(1);

	CHECK((percent<int>(1)) == f);

	f += 1;

	CHECK((percent<int>(101)) == f);

	f += dimensionless<int>(1);

	CHECK((percent<int>(201)) == f);
}

TEST_CASE_METHOD(UnitType, "UnitType compoundAssignmentSubtraction", "[wpimath][units]")
{
	// units
	meters<double> a(2.0);
	a -= meters<double>(1.0);

	CHECK(meters<double>(1.0) == a);

	a -= feet<double>(meters<double>(1));

	CHECK(meters<double>(0.0) == a);

	a -= meters<int>(1);

	CHECK(meters<double>(-1.0) == a);

	a -= std::common_type_t<meters<int>, feet<int>>(meters<int>(1));

	CHECK(meters<double>(-2.0) == a);

	meters<int> c(1);
	c -= meters<int>(1);

	CHECK((meters<int>(0)) == c);

	c -= kilometers<int>(1);

	CHECK((meters<int>(-1000)) == c);

	// dimensionless
	dimensionless<double> b_dim(2);
	b_dim -= dimensionless<double>(1.0);

	CHECK(dimensionless<double>(1.0) == b_dim);

	b_dim -= 1.0;

	CHECK(dimensionless<double>(0) == b_dim);

	b_dim -= dimensionless<int>(1);

	CHECK(dimensionless<double>(-1.0) == b_dim);

	b_dim -= 1;

	CHECK(dimensionless<double>(-2.0) == b_dim);

	dimensionless<int> d_dim(2);
	d_dim -= dimensionless<int>(1);

	CHECK((dimensionless<int>(1)) == d_dim);

	d_dim -= 1;

	CHECK((dimensionless<int>(0)) == d_dim);

	// concentration
	percent<double> e_pct(200);
	e_pct -= percent<double>(1.0);

	CHECK(percent<double>(199.0) == e_pct);

	e_pct -= 1.0;

	CHECK(percent<double>(99.0) == e_pct);

	e_pct -= percent<int>(100);

	CHECK(percent<double>(-1.0) == e_pct);

	e_pct -= 1;

	CHECK(percent<double>(-101.0) == e_pct);

	percent<int> f(2);
	f -= percent<int>(1);

	CHECK((percent<int>(1)) == f);

	f -= 1;

	CHECK(percent<int>(-99) == f);

	f -= dimensionless<int>(1);

	CHECK(percent<int>(-199) == f);
}

TEST_CASE_METHOD(UnitType, "UnitType compoundAssignmentMultiplication", "[wpimath][units]")
{
	// units
	meters<double> a(2.0);
	a *= dimensionless<double>(2.0);

	CHECK(meters<double>(4.0) == a);

	a *= 2.0;

	CHECK(meters<double>(8.0) == a);

	a *= dimensionless<int>(2);

	CHECK(meters<double>(16) == a);

	a *= 2;

	CHECK(meters<double>(32) == a);

	meters<int> c(2);
	c *= dimensionless<int>(2);

	CHECK((meters<int>(4)) == c);

	c *= dimensionless<int>(2);

	CHECK((meters<int>(8)) == c);

	c *= 2;

	CHECK((meters<int>(16)) == c);

	// dimensionless
	dimensionless<double> b_dim(2);
	b_dim *= dimensionless<double>(2.0);

	CHECK(dimensionless<double>(4.0) == b_dim);

	b_dim *= 2.0;

	CHECK(dimensionless<double>(8.0) == b_dim);

	b_dim *= dimensionless<int>(2);

	CHECK(dimensionless<double>(16.0) == b_dim);

	b_dim *= 2;

	CHECK(dimensionless<double>(32.0) == b_dim);

	dimensionless<int> d_dim(2);
	d_dim *= dimensionless<int>(2);

	CHECK((dimensionless<int>(4)) == d_dim);

	d_dim *= dimensionless<int>(2);

	CHECK((dimensionless<int>(8)) == d_dim);

	d_dim *= 2;

	CHECK((dimensionless<int>(16)) == d_dim);

	// concentration
	percent<double> e_pct(2);
	e_pct *= percent<double>(2.0);

	CHECK(percent<double>(0.04) == e_pct);
	CHECK(0.0004 == e_pct.value());

	e_pct *= 2.0;

	CHECK(percent<double>(0.08) == e_pct);
	CHECK(0.0008 == e_pct.value());

	e_pct *= percent<int>(2);

	CHECK(percent<double>(0.0016) == e_pct);
	CHECK(0.000016 == e_pct.value());

	e_pct *= 2;

	CHECK(percent<double>(0.0032) == e_pct);
	CHECK(0.000032 == e_pct.value());

	percent<int> f(2);
	f *= percent<int>(200);

	CHECK((percent<int>(4)) == f);

	f *= percent<double>(200.0);

	CHECK((percent<int>(8)) == f);

	f *= 2;

	CHECK((percent<int>(16)) == f);

	f *= 2.0;

	CHECK((percent<int>(32)) == f);

	f *= dimensionless<int>(2);

	CHECK((percent<int>(64)) == f);
}

TEST_CASE_METHOD(UnitType, "UnitType compoundAssignmentDivision", "[wpimath][units]")
{
	// units
	meters<double> a(8.0);
	a /= dimensionless<double>(2.0);

	CHECK(meters<double>(4.0) == a);

	a /= 2.0;

	CHECK(meters<double>(2.0) == a);

	a /= dimensionless<int>(2);

	CHECK(meters<double>(1) == a);

	a /= 2;

	CHECK(meters<double>(0.5) == a);

	a /= percent<double>(50);

	CHECK(1_m == a);

	meters<int> c(32);
	c /= dimensionless<int>(2);

	CHECK((meters<int>(16)) == c);

	c /= dimensionless<int>(2);

	CHECK((meters<int>(8)) == c);

	c /= 2;

	CHECK((meters<int>(4)) == c);

	// dimensionless
	dimensionless<double> b_dim(8);
	b_dim /= dimensionless<double>(2.0);

	CHECK(dimensionless<double>(4.0) == b_dim);

	b_dim /= 2.0;

	CHECK(dimensionless<double>(2.0) == b_dim);

	b_dim /= dimensionless<int>(2);

	CHECK(dimensionless<double>(1.0) == b_dim);

	b_dim /= 2;

	CHECK(dimensionless<double>(0.5) == b_dim);

	dimensionless<int> d_dim(32);
	d_dim /= dimensionless<int>(2);

	CHECK((dimensionless<int>(16)) == d_dim);

	d_dim /= dimensionless<int>(2);

	CHECK((dimensionless<int>(8)) == d_dim);

	d_dim /= 2;

	CHECK((dimensionless<int>(4)) == d_dim);

	// concentration
	percent<double> e_pct(8);
	e_pct /= percent<double>(50.0);

	CHECK(percent<double>(16.0) == e_pct);
	CHECK(0.16 == e_pct.value());

	e_pct /= 2.0;

	CHECK(percent<double>(8.0) == e_pct);
	CHECK(0.08 == e_pct.value());

	e_pct /= percent<int>(2);

	CHECK(percent<double>(400.0) == e_pct);
	CHECK(4 == e_pct.value());

	e_pct /= 2;

	CHECK(percent<double>(200.0) == e_pct);
	CHECK(2 == e_pct.value());
}

TEST_CASE_METHOD(UnitType, "UnitType compoundAssignmentModulo", "[wpimath][units]")
{
	// units
	meters<int> a_m(2200);

	a_m %= meters<int>(2000);
	CHECK(200 == a_m.value());

	a_m %= kilometers<int>(1);
	CHECK(200 == a_m.value());

	a_m %= dimensionless<int>(180);
	CHECK(20 == a_m.value());

	a_m %= dimensionless<double>(15.0);
	CHECK(5 == a_m.value());

	a_m %= 6;
	CHECK(5 == a_m.value());

	a_m %= 3.0;
	CHECK(2 == a_m.value());

	// dimensionless
	dimensionless<int> a_s(12);

	a_s %= dimensionless<int>(20);
	CHECK(12 == a_s.value());

	a_s %= 5;
	CHECK(2 == a_s.value());

	// concentration
	percent<int> b_s(12);

	b_s %= percent<int>(20);
	CHECK(12_pct == b_s);

	b_s %= percent<int>(5);
	CHECK(2_pct == b_s);

	b_s %= 5;
	CHECK(2_pct == b_s);

	b_s %= dimensionless<int>(5);
	CHECK(2_pct == b_s);

	b_s %= 2;
	CHECK(0_pct == b_s);
}

TEST_CASE_METHOD(UnitType, "UnitType PpbPerYearCompoundUnitType", "[wpimath][units]")
{
	using ppb_per_year = decltype(ppb/yr);

	parts_per_million concentration = 9.71_ppb;

	CHECK(concentration == 0.00971_ppm);
	CHECK(concentration == 0.00000000971);

	ppb_per_year rate(0.109);

	auto elapsed = 2013.9_yr-1994_yr;

	parts_per_million val = concentration + rate*elapsed;
	CHECK_THAT(val, Catch::Matchers::WithinAbs(1.18791e-08, 1e-12));
}

TEST_CASE_METHOD(UnitType, "UnitType dimensionlessTypeImplicitConversion", "[wpimath][units]")
{
	double test = dimensionless<double>(3.0);
	CHECK(3.0 == test);

	dimensionless<double> testS = 3.0;
	CHECK(3.0 == testS);

	dimensionless<double> test3(parts_per_million<double>(10));
	CHECK(0.00001 == test3);

	dimensionless<double> test4;
	test4 = parts_per_million<double>(1);
	CHECK(0.000001 == test4);

	dimensionless<double> test5;
	test5 = percent<double>(10);
	CHECK(0.1 == test5);
}

TEST_CASE_METHOD(UnitType, "UnitType valueMethod", "[wpimath][units]")
{
	constexpr auto test = meters(3.0).to<double>();
	CHECK(3.0 == test);

	constexpr auto test2 = meters(4.0).value();
	CHECK(4.0 == test2);
	static_assert(std::is_same_v<std::decay_t<decltype(test2)>, double>);

	constexpr miles_per_hour speed = 100.0_ft / 1.0_min;
	CHECK(speed.value() == 1.1363636363636365);

	constexpr auto test3 = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(test3.value() == test3.to<double>());

	constexpr auto test4 = (2.0 / 1000.0_mm) / 5_hr;
	CHECK(test4.value() == test4.to<double>());

	constexpr percent test5(85.1);
	CHECK(test5.to<double>() == 0.851);

	constexpr percent test6(1.0_m / 2000.0_mm);
	CHECK(test6 == 50_pct);
	CHECK(test6.to<double>() == 0.5);

	constexpr percent test7(1000.0_mm / 2.0_m);
	CHECK(test7 == 50_pct);
	CHECK(test7.to<double>() == 0.5);

	constexpr percent test8 = 1000.0_mm / 2.0_m;
	CHECK(test8 == 50_pct);
	CHECK(test8.to<double>() == 0.5);
}

TEST_CASE_METHOD(UnitType, "UnitType convertMethod", "[wpimath][units]")
{
	constexpr auto test = meters(3.0).convert<feet<double>::conversion_factor>().to<double>();
	CHECK_THAT(9.84252, Catch::Matchers::WithinAbs(test, 5.0e-6));

	constexpr auto unit2 = meters<double>(3.0).convert<feet>();
	constexpr auto test2 = unit2.to<double>();
	CHECK_THAT(9.84252, Catch::Matchers::WithinAbs(test2, 5.0e-6));

	// named-unit to<>(): returns a unit of the requested type, mirroring convert<>()
	constexpr auto asFeet = meters<double>(3.0).to<feet>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(asFeet)>, feet<double>>);
	CHECK_THAT(9.84252, Catch::Matchers::WithinAbs(asFeet.to<double>(), 5.0e-6));

	constexpr auto asMeters = centimeters<double>(100.0).to<meters>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(asMeters)>, meters<double>>);
	CHECK(1.0 == asMeters.to<double>());

	// arithmetic-type to<>() is unchanged: extracts the underlying value
	CHECK(3.0 == meters<double>(3.0).to<double>());

	// convert<>() is callable on a const unit (const-qualified overloads)
	const meters<double> constMeters(3.0);
	CHECK_THAT(9.84252, Catch::Matchers::WithinAbs(constMeters.convert<feet>().to<double>(), 5.0e-6));
	CHECK_THAT(9.84252, Catch::Matchers::WithinAbs(constMeters.to<feet>().to<double>(), 5.0e-6));
}

TEST_CASE_METHOD(UnitType, "UnitType integerConversionWidensIntermediate", "[wpimath][units]")
{
	// A mul-then-divide conversion (feet -> meters is 381/1250) computes the intermediate product in a
	// double-width integer, so `value * 381` does not overflow before `/ 1250` recovers a value that fits the
	// target. Without the widening this silently overflowed for large magnitudes.
	const auto big = wpi::units::convert<feet<std::int64_t>>(feet<std::int64_t>(0)); // (touch the header)
	(void)big;

	// 5e16 ft * 381 = 1.905e19 overflows int64 (max ~9.2e18), but 5e16 * 381 / 1250 = 1.524e16 fits.
	const std::int64_t v      = 50000000000000000LL;
	const auto         meters = wpi::units::convert<wpi::units::length::meters<std::int64_t>>(feet<std::int64_t>(v));
	CHECK(15240000000000000LL == meters.value());

	// Ordinary and negative magnitudes are exact and unchanged (widening never alters a result that already fit).
	CHECK(381 == wpi::units::convert<wpi::units::length::meters<std::int64_t>>(feet<std::int64_t>(1250)).value());
	CHECK(-381 == wpi::units::convert<wpi::units::length::meters<std::int64_t>>(feet<std::int64_t>(-1250)).value());
	CHECK(0 == wpi::units::convert<wpi::units::length::meters<std::int64_t>>(feet<std::int64_t>(0)).value());

	// Floating-point conversions are unaffected (the widening is integer-only).
	CHECK_THAT(0.3048, Catch::Matchers::WithinAbs(wpi::units::convert<wpi::units::length::meters<double>>(feet<double>(1.0)).value(), 1e-9));
}

TEST_CASE_METHOD(UnitType, "UnitType floatingPointConversionIsCorrectlyRounded", "[wpimath][units]")
{
	// The floating-point conversion path is not widened (that would be platform-dependent for no gain); it is
	// already correctly rounded. Pin that so a future refactor cannot silently degrade it: the result must be
	// within half a ULP of the high-precision reference, and a round-trip must be bit-stable.
	for (double v : {1.0, 3.0, 1234.56789, 1.0e6, 987654321.123456, 1.0e15})
	{
		const double      lib = wpi::units::convert<wpi::units::length::meters<double>>(feet<double>(v)).value();
		const long double ref = static_cast<long double>(v) * 381.0L / 1250.0L;
		const double      ulp = static_cast<double>(std::nextafter(static_cast<double>(ref), static_cast<double>(ref) + 1.0) - static_cast<double>(ref));
		CHECK(std::abs(static_cast<long double>(lib) - ref) <= 0.5L * ulp);

		const double back = wpi::units::convert<feet<double>>(wpi::units::convert<wpi::units::length::meters<double>>(feet<double>(v))).value();
		CHECK(v == back); // round-trip bit-stable
	}
	// Affine + pi conversions stay exact to the last bit.
	CHECK(98.6 == wpi::units::convert<fahrenheit<double>>(celsius<double>(37.0)).value());

	// A big value through a fractional ratio must not lose a representable answer to intermediate overflow:
	// 1e306 ft -> m is ~3.048e305 (fits double), even though value * 381 = 3.81e308 would overflow. The
	// conversion divides first in that regime and returns the finite result rather than infinity.
	const double extreme = wpi::units::convert<wpi::units::length::meters<double>>(feet<double>(1.0e306)).value();
	CHECK(std::isfinite(extreme));
	CHECK_THAT(3.048e305, Catch::Matchers::WithinAbs(extreme, 3.048e305 * 1e-12));
}

TEST_CASE_METHOD(UnitType, "UnitType mixedSignednessComparesByValue", "[wpimath][units]")
{
	// A signed-rep and an unsigned-rep unit of the same dimension compare by mathematical value, not by C++'s
	// int/unsigned wraparound: -1 m is less than 1 m even when one side is unsigned.
	CHECK(meters<int>(-1) < meters<unsigned>(1u));
	CHECK_FALSE(meters<int>(-1) > meters<unsigned>(1u));
	CHECK_FALSE(meters<int>(-1) == meters<unsigned>(1u));
	CHECK(meters<int>(-5) != meters<unsigned>(5u));  // NOT equal via wraparound
	CHECK(meters<int>(2) == meters<unsigned>(2u));   // genuinely equal
	CHECK(meters<unsigned>(1u) > meters<int>(-1));   // reversed operands

	// Same-signedness and cross-unit comparisons are unchanged.
	CHECK(meters<int>(3) < meters<int>(5));
	CHECK(kilometers<int>(1) == meters<int>(1000));
	CHECK(meters<int>(1500) > kilometers<int>(1));

	// Floating-point comparison is unaffected.
	CHECK(meters<double>(1.5) < meters<double>(2.5));
	CHECK(meters<double>(-1.0) < meters<double>(1.0));
}

TEST_CASE_METHOD(UnitType, "UnitType hashOfLargeValueDoesNotOverflow", "[wpimath][units]")
{
	// std::hash forwards through a unit conversion; a large value converted to a fine unit used to overflow the
	// intermediate. With the widened conversion the hash of a big value is computed without undefined behavior
	// (run under -fsanitize=undefined this must not trip). Equal values under one key type hash equally.
	const auto h = std::hash<millimeters<std::int64_t>>()(kilometers<std::int64_t>(3000)); // 3e9 mm
	CHECK(h == std::hash<millimeters<std::int64_t>>()(millimeters<std::int64_t>(3000000000LL)));
	CHECK(std::hash<meters<int>>()(meters<int>(7)) == std::hash<meters<int>>()(meters<int>(7)));
}

TEST_CASE_METHOD(UnitType, "UnitType exactIntegralNarrowingConstructor", "[wpimath][units]")
{
	using wpi::units::data::bits;
	using wpi::units::data::bytes;
	using wpi::units::data::nibbles;

	// A compile-time-known finer integral value that is an exact whole number of the coarser unit converts, at
	// compile time, to the exact count. `bits` is ratio<1,8> of `bytes`, so 16 bits is exactly 2 bytes.
	constexpr bytes<int> two = bits<int>(16);
	static_assert(two.value() == 2, "16 bits is 2 bytes");
	CHECK(2 == two.value());

	constexpr bytes<int> one = bits<int>(8);
	static_assert(one.value() == 1, "8 bits is 1 byte");

	// Negative carriers convert exactly (divisibility ignores sign).
	constexpr bytes<int> negTwo = bits<int>(-16);
	static_assert(negTwo.value() == -2, "-16 bits is -2 bytes");
	CHECK(-2 == negTwo.value());

	// A multi-step finer ratio: a nibble is 4 bits, so 2 nibbles is exactly 1 byte.
	constexpr bytes<int> fromNibbles = nibbles<int>(2);
	static_assert(fromNibbles.value() == 1, "2 nibbles is 1 byte");

	// The reverse direction is already lossless and unchanged (coarser -> finer never truncates).
	constexpr bits<int> sixteen = bytes<int>(2);
	static_assert(sixteen.value() == 16, "2 bytes is 16 bits");

	// The exactness test rides in a double-width intermediate, so a large exact value still converts (no overflow
	// of value*num before the divide). 2^60 bits is exactly 2^57 bytes.
	constexpr bytes<std::int64_t> big = bits<std::int64_t>(1LL << 60);
	static_assert(big.value() == (1LL << 57), "2^60 bits is 2^57 bytes");
}

TEST_CASE_METHOD(UnitType, "UnitType runtimeLossyRoundingConversion", "[wpimath][units]")
{
	using wpi::units::data::bits;
	using wpi::units::data::bytes;

	// A genuinely run-time finer value need not be a whole number of the coarser unit; the caller states the
	// rounding intent with the target-taking round/floor/ceil/trunc, mirroring std::chrono::floor<To>.
	const bits<int> seventeen(17); // 2.125 bytes
	CHECK(2 == wpi::units::floor<bytes<int>>(seventeen).value());
	CHECK(3 == wpi::units::ceil<bytes<int>>(seventeen).value());
	CHECK(2 == wpi::units::round<bytes<int>>(seventeen).value());
	CHECK(2 == wpi::units::trunc<bytes<int>>(seventeen).value());

	const bits<int> twenty(20); // 2.5 bytes -> round halfway away from zero
	CHECK(2 == wpi::units::floor<bytes<int>>(twenty).value());
	CHECK(3 == wpi::units::ceil<bytes<int>>(twenty).value());
	CHECK(3 == wpi::units::round<bytes<int>>(twenty).value());
	CHECK(2 == wpi::units::trunc<bytes<int>>(twenty).value());

	// Negative values distinguish floor (toward -inf) from trunc (toward zero).
	const bits<int> negSeventeen(-17); // -2.125 bytes
	CHECK(-3 == wpi::units::floor<bytes<int>>(negSeventeen).value());
	CHECK(-2 == wpi::units::ceil<bytes<int>>(negSeventeen).value());
	CHECK(-2 == wpi::units::round<bytes<int>>(negSeventeen).value());
	CHECK(-2 == wpi::units::trunc<bytes<int>>(negSeventeen).value());

	// An exactly-divisible run-time value rounds to itself under every mode.
	const bits<int> sixteen(16); // exactly 2 bytes
	CHECK(2 == wpi::units::floor<bytes<int>>(sixteen).value());
	CHECK(2 == wpi::units::ceil<bytes<int>>(sixteen).value());
	CHECK(2 == wpi::units::round<bytes<int>>(sixteen).value());
	CHECK(2 == wpi::units::trunc<bytes<int>>(sixteen).value());

	// The rounding is exact integer arithmetic, so a magnitude beyond 2^53 (where a double intermediate would lose
	// the fractional byte and round the wrong way) is still correct. 2^53+1 bits is 1125899906842624.125 bytes.
	const bits<std::int64_t> above2p53(9007199254740993LL);
	CHECK(1125899906842624LL == wpi::units::floor<bytes<std::int64_t>>(above2p53).value());
	CHECK(1125899906842625LL == wpi::units::ceil<bytes<std::int64_t>>(above2p53).value());
	CHECK(1125899906842624LL == wpi::units::round<bytes<std::int64_t>>(above2p53).value());
	CHECK(1125899906842624LL == wpi::units::trunc<bytes<std::int64_t>>(above2p53).value());
	// A byte count above 2^53 itself: 2^56+12 bits is 2^53+1.5 bytes, floor must land on the exact 2^53+1.
	CHECK((std::int64_t(1) << 53) + 1 == wpi::units::floor<bytes<std::int64_t>>(bits<std::int64_t>((std::int64_t(1) << 56) + 12)).value());
	// A large negative value: floor goes toward negative infinity even past 2^53.
	CHECK(-((std::int64_t(1) << 57) + 1) == wpi::units::floor<bytes<std::int64_t>>(bits<std::int64_t>(-((std::int64_t(1) << 60) + 5))).value());

	// A result that does not fit the target integer wraps like any integer narrowing (the semantics of
	// std::chrono::floor<To>), never an out-of-range floating-to-integer conversion: 3e9 bytes exceeds int.
	CHECK(static_cast<int>(3000000000LL) == wpi::units::floor<bytes<int>>(bits<std::int64_t>(8 * 3000000000LL)).value());

	// The target-taking overloads do not shadow the deduced-argument rounding math functions.
	CHECK(3.0 == wpi::units::floor(meters<double>(3.7)).value());
	CHECK(4.0 == wpi::units::ceil(meters<double>(3.7)).value());
	CHECK(4.0 == wpi::units::round(meters<double>(3.7)).value());
	CHECK(3.0 == wpi::units::trunc(meters<double>(3.7)).value());
}

#ifndef UNIT_LIB_DISABLE_IOSTREAM
TEST_CASE_METHOD(UnitType, "UnitType cout", "[wpimath][units]")
{
	testing::internal::CaptureStdout();
	std::cout << meters_per_second<double>(5);
	std::string output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("5 mps") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << degrees<double>(349.87);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("349.87 deg") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << meters<double>(1.0);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("1 m") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << decibels<double>(31.0);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("31 dB") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << volts<double>(21.79);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("21.79 V") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBW<double>(12.0);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("12 dBW") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBm<double>(120.0);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("120 dBm") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << miles_per_hour<double>(72.1);
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("72.1 mph") == output.c_str());

	// undefined unit
	testing::internal::CaptureStdout();
	std::cout << pow<4>(meters<double>(2));
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("16 m^4") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << pow<3>(feet<double>(2));
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("8 ft3") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(9) << pow<4>(feet<double>(2));
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("0.138095597 m^4") == output.c_str());

	// constants
	testing::internal::CaptureStdout();
	std::cout << std::setprecision(8) << constants::k_B;
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("1.380649e-23 m^2 kg K^-1 s^-2") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(11) << constants::mu_B;
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("9.2740100783e-24 A m^2") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(10) << constants::sigma;
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("5.670374419e-08 kg K^-4 s^-3") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(7) << 51_pct;
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("51 pct") == output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(7) << 15_ppb;
	output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("15 ppb") == output.c_str());
}

TEST_CASE_METHOD(UnitType, "UnitType to_string", "[wpimath][units]")
{
	feet<double> a_ft(3.5);
	CHECK(std::string_view("3.5 ft") == to_string(a_ft).c_str());

	meters<double> b_m(8);
	CHECK(std::string_view("8 m") == to_string(b_m).c_str());

	percent<double> c_pct(25.1);
	CHECK(std::string_view("25.1 pct") == to_string(c_pct).c_str());

	// a unit with no abbreviation renders its DIMENSION form, and must match operator<< exactly (single-spaced)
	using canonical_acceleration = unit<conversion_factor<std::ratio<1>, dimension::acceleration>, double>;
	const canonical_acceleration d_accel(9.81);
	std::ostringstream streamed;
	streamed << d_accel;
	CHECK(streamed.str() == to_string(d_accel));
	CHECK(std::string_view("9.81 m s^-2") == to_string(d_accel).c_str());
}

// The platform spellings of the two locales this test needs, and the command that installs them.
#if defined(_MSC_VER)
static constexpr const char* GERMAN_LOCALE = "de-DE";
static constexpr const char* US_LOCALE     = "en-US";
static constexpr const char* LOCALE_HINT   = "install the German and US locales";
#elif defined(__APPLE__)
// BSD libc (macOS) only recognizes the canonical `.UTF-8` spelling, not glibc's `de_DE.utf8` alias.
static constexpr const char* GERMAN_LOCALE = "de_DE.UTF-8";
static constexpr const char* US_LOCALE     = "en_US.UTF-8";
static constexpr const char* LOCALE_HINT   = "install the German and US locales";
#else
static constexpr const char* GERMAN_LOCALE = "de_DE.utf8";
static constexpr const char* US_LOCALE     = "en_US.utf8";
static constexpr const char* LOCALE_HINT   = "install the German and US locales, e.g. `sudo locale-gen de_DE.UTF-8 en_US.UTF-8`";
#endif

TEST_CASE_METHOD(UnitType, "UnitType to_string_locale", "[wpimath][units]")
{
	struct lconv*     lc;
	std::string       output;
	std::stringstream os1;
	std::stringstream os2;

	// A locale this test needs may not be present on the host; that is an environmental precondition,
	// not a library defect, so skip (with the install hint) rather than throw from std::locale.
	if (setlocale(LC_ALL, GERMAN_LOCALE) == nullptr || setlocale(LC_ALL, US_LOCALE) == nullptr)
	{
		GTEST_SKIP() << "requires the German and US locales; " << LOCALE_HINT;
	}

	// German locale
	setlocale(LC_ALL, GERMAN_LOCALE);
	os1.imbue(std::locale(GERMAN_LOCALE));

	lc            = localeconv();
	char point_de = *lc->decimal_point;
	CHECK(point_de == ',');

	kilometers<double> de = 2.0_km;
	CHECK(std::string_view("2 km") == to_string(de).c_str());

	de = 2.5_km;
	CHECK(std::string_view("2,5 km") == to_string(de).c_str());

	os1 << std::setprecision(11) << constants::mu_B;
	output = os1.str();
	CHECK(std::string_view("9,2740100783e-24 A m^2") == output.c_str());

	// US locale
	setlocale(LC_ALL, US_LOCALE);
	os2.imbue(std::locale(US_LOCALE));

	lc            = localeconv();
	char point_us = *lc->decimal_point;
	CHECK(point_us == '.');

	miles<double> dist = 2.0_mi;
	CHECK(std::string_view("2 mi") == to_string(dist).c_str());

	dist = 2.5_mi;
	CHECK(std::string_view("2.5 mi") == to_string(dist).c_str());

	os2 << std::setprecision(11) << constants::mu_B;
	output = os2.str();
	CHECK(std::string_view("9.2740100783e-24 A m^2") == output.c_str());
}

TEST_CASE_METHOD(UnitType, "UnitType nameAndAbbreviation", "[wpimath][units]")
{
	feet a_ft(3.5);
	CHECK(std::string_view("ft") == unit_abbreviation_v<decltype(a_ft)>);
	CHECK(std::string_view("ft") == a_ft.abbreviation());
	CHECK(std::string_view("feet") == a_ft.name());

	meters b_m(8.0);
	CHECK(std::string_view("m") == unit_abbreviation_v<decltype(b_m)>);
	CHECK(std::string_view("m") == b_m.abbreviation());
	CHECK(std::string_view("meters") == b_m.name());
}
#endif

TEST_CASE("Consistency recovers_input_values", "[wpimath][units]")
{
	for (int i = 0; i <= 100; ++i)
	{
		CHECK_THAT(i, Catch::Matchers::WithinULP(wpi::units::concentration::percent<double>(i).value() * 100, 1));
	}
}

TEST_CASE("Consistency percent", "[wpimath][units]")
{
	percent<double> a_pct(50);
	percent<double> b_pct = 50;

	CHECK(a_pct == 50_pct);
	CHECK(b_pct == 50_pct);
}

TEST_CASE_METHOD(UnitType, "UnitType identity", "[wpimath][units]")
{
	auto            a_pct = percent<double>(50);
	percent<double> b_pct = a_pct;
	percent<double> d_pct = 1.0 * a_pct;
	percent<double> f_pct = a_pct * 1.0;
	CHECK(a_pct == percent<double>(50));
	CHECK(b_pct == percent<double>(50));
	CHECK(d_pct == percent<double>(50));
	CHECK(f_pct == percent<double>(50));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(a_pct, 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(b_pct, 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(d_pct, 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(f_pct, 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(a_pct.value(), 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(b_pct.value(), 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(d_pct.value(), 5.0e-10));
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(f_pct.value(), 5.0e-10));

	a_pct = percent<double>(50);
	b_pct = a_pct / 1;
	CHECK(a_pct == b_pct);
	CHECK(b_pct == percent<double>(50));

	a_pct = percent<double>(50);
	a_pct *= 1;
	CHECK(a_pct == percent<double>(50));

	a_pct = percent<double>(50);
	a_pct /= 1;
	CHECK(a_pct == percent<double>(50));

	meters<double> w(5.3);
	meters<double> x = w;
	meters<double> y = 1 * w;
	meters<double> z = w * 1;
	CHECK(w == meters<double>(5.3));
	CHECK(x == meters<double>(5.3));
	CHECK(y == meters<double>(5.3));
	CHECK(z == meters<double>(5.3));
	CHECK_THAT(5.3, Catch::Matchers::WithinAbs(w.value(), 5.0e-10));
	CHECK_THAT(5.3, Catch::Matchers::WithinAbs(x.value(), 5.0e-10));
	CHECK_THAT(5.3, Catch::Matchers::WithinAbs(y.value(), 5.0e-10));
	CHECK_THAT(5.3, Catch::Matchers::WithinAbs(z.value(), 5.0e-10));

	double v = 1.0 * pi;
	CHECK(v == pi.to<double>());
	v = pi * 1.0;
	CHECK(v == pi.to<double>());
}

TEST_CASE_METHOD(UnitType, "UnitType negative", "[wpimath][units]")
{
	meters<double> a_m(5.3);
	meters<double> b_m(-5.3);
	CHECK_THAT(a_m.to<double>(), Catch::Matchers::WithinAbs(-b_m.to<double>(), 5.0e-320));
	CHECK_THAT(b_m.to<double>(), Catch::Matchers::WithinAbs(-a_m.to<double>(), 5.0e-320));

	decibels<double> c_dB(2.87);
	decibels<double> d_dB(-2.87);
	CHECK_THAT(c_dB.to<double>(), Catch::Matchers::WithinAbs(-d_dB.to<double>(), 5.0e-320));
	CHECK_THAT(d_dB.to<double>(), Catch::Matchers::WithinAbs(-c_dB.to<double>(), 5.0e-320));

	parts_per_million<double> e_ppm = -1 * parts_per_million<double>(10);
	CHECK(e_ppm == -parts_per_million<double>(10));
	CHECK_THAT(-0.00001, Catch::Matchers::WithinAbs(e_ppm, 5.0e-10));

	percent<double> f = -1 * percent<double>(50);
	CHECK(f == -percent<double>(50));
	CHECK_THAT(-0.5, Catch::Matchers::WithinAbs(f, 5.0e-10));

	percent<double> g_pct = percent<double>(50) * -1;
	CHECK(g_pct == -percent<double>(50));
	CHECK_THAT(-0.5, Catch::Matchers::WithinAbs(g_pct, 5.0e-10));
}

TEST_CASE_METHOD(UnitType, "UnitType concentration", "[wpimath][units]")
{
	parts_per_billion<double> a_ppb(parts_per_million<double>(1));
	CHECK(parts_per_billion<double>(1000) == a_ppb);
	CHECK(0.000001 == a_ppb);
	CHECK(0.000001 == a_ppb.to<double>());

	dimensionless<double> b_ppm(parts_per_million<double>(1));
	CHECK(0.000001 == b_ppm);

	dimensionless<double> c_dim = parts_per_billion<double>(1);
	CHECK(0.000000001 == c_dim);

	static_assert(traits::is_same_dimension_unit_v<percent<double>, dimensionless<double>>);
}

TEST_CASE_METHOD(UnitType, "UnitType dBConversion", "[wpimath][units]")
{
	dBW<double>   a_dbw(23.1);
	watts<double> a_w   = a_dbw;
	dBm<double>   a_dbm = a_dbw;

	CHECK_THAT(204.173794, Catch::Matchers::WithinAbs(a_w.value(), 5.0e-7));
	CHECK_THAT(53.1, Catch::Matchers::WithinAbs(a_dbm.value(), 5.0e-7));

	milliwatts<double> b_mw(100000.0);
	watts<double>      b_w   = b_mw;
	dBm<double>        b_dbm = b_mw;
	dBW<double>        b_dbw = b_mw;

	CHECK_THAT(100.0, Catch::Matchers::WithinAbs(b_w.value(), 5.0e-7));
	CHECK_THAT(50.0, Catch::Matchers::WithinAbs(b_dbm.value(), 5.0e-7));
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(b_dbw.value(), 5.0e-7));
}

TEST_CASE_METHOD(UnitType, "UnitType dimensionlessDecibelLiteral", "[wpimath][units]")
{
	// the `_dB` literal yields the dimensionless decibel; its stored value is the dB figure
	auto gainDb = -20.0_dB;
	static_assert(std::is_same_v<decltype(gainDb), decibels<double>>);
	CHECK(-20.0 == gainDb.raw());

	// only a floating-point _dB literal exists; an integer decibel is rejected at compile time
	// (see the errorMessages harness), because a decibel scale cannot use an integral underlying type
	auto gainDb2 = 6.0_dB;
	static_assert(std::is_same_v<decltype(gainDb2), decibels<double>>);

	// name/abbreviation resolve for the dimensionless decibel
	decibels<double> ratioDb(6.0);
	CHECK(std::string_view("decibels") == ratioDb.name());
	CHECK(std::string_view("dB") == ratioDb.abbreviation());

	// coexists with the power decibel literals (distinct types, distinct suffixes)
	static_assert(std::is_same_v<decltype(0.0_dBW), dBW<double>>);
	static_assert(std::is_same_v<decltype(0.0_dBm), dBm<double>>);
	static_assert(!std::is_same_v<decibels<double>, dBW<double>>);

	// a floating-point underlying type other than double is allowed (only integral types are rejected)
	decibels<float> asFloat(20.0f);
	CHECK_THAT(100.0, Catch::Matchers::WithinAbs(static_cast<double>(asFloat.to_linearized()), 5.0e-4));

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	// the streamed form of a decibel-arithmetic result uses the dB abbreviation (adding dB multiplies linear)
	testing::internal::CaptureStdout();
	std::cout << (decibels<double>(3.0) + decibels<double>(3.0));
	std::string output = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("6 dB") == output.c_str());
#endif
}

TEST_CASE_METHOD(UnitType, "UnitType dBAddition", "[wpimath][units]")
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) + decibels<double>(30.0);
	CHECK_THAT(40.0, Catch::Matchers::WithinAbs(result_dbw.value(), 5.0e-5));
	result_dbw = dBW<double>(10.0) + decibels<double>(30.0);
	CHECK_THAT(40.0, Catch::Matchers::WithinAbs(result_dbw.value(), 5.0e-5));
	result_dbw = decibels<double>(12.0) + dBW<double>(30.0);
	CHECK_THAT(42.0, Catch::Matchers::WithinAbs(result_dbw.value(), 5.0e-5));
	result_dbw = decibels<double>(12.0) + dBW<double>(30.0);
	CHECK_THAT(42.0, Catch::Matchers::WithinAbs(result_dbw.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	CHECK(isSame);

	auto result_dbm = decibels<double>(30.0) + dBm<double>(20.0);
	CHECK_THAT(50.0, Catch::Matchers::WithinAbs(result_dbm.value(), 5.0e-5));
	result_dbm = decibels<double>(30.0) + dBm<double>(20.0);
	CHECK_THAT(50.0, Catch::Matchers::WithinAbs(result_dbm.value(), 5.0e-5));

	// Adding two absolute decibel LEVELS (dBW + dBm, both dimensioned) is a point + point and is ill-formed:
	// two power levels do not sum by adding their dB numbers (that would be a product of powers). The addition
	// operator for two dimensioned same-dimension decibel operands is deleted; the compile-time rejection is
	// proven by the errorMessages case decibel_level_plus_level.cpp (a deleted overload is still selected by
	// overload resolution, so it cannot be probed with a `requires` expression — the negative test lives there).
}

TEST_CASE_METHOD(UnitType, "UnitType dBAffineSemantics", "[wpimath][units]")
{
	// A dimensioned decibel value (dBW, dBm) is an absolute LEVEL — a point on a logarithmic reference scale.
	// A dimensionless decibel (decibels) is a relative GAIN — a delta. The defined operations mirror an affine
	// space: level + gain -> level, gain + gain -> gain, level - level -> gain. (level + level is ill-formed;
	// see dBAddition and the decibel_level_plus_level errorMessages case.)

	// level + gain -> level (the point stays a dimensioned power level, moved by the gain)
	const auto boosted = dBW<double>(10.0) + decibels<double>(3.0);
	CHECK_THAT(13.0, Catch::Matchers::WithinAbs(boosted.value(), 5.0e-5));
	static_assert(std::is_same_v<std::remove_const_t<decltype(boosted)>, dBW<double>>, "level + gain stays a level");
	static_assert(traits::is_power_unit_v<decltype(boosted)>, "a dBW level is a power");

	// gain + gain -> gain (two relative ratios compound; their dB numbers add)
	const auto chained = decibels<double>(3.0) + decibels<double>(3.0);
	CHECK_THAT(6.0, Catch::Matchers::WithinAbs(chained.value(), 5.0e-5));
	static_assert(traits::is_dimensionless_unit_v<decltype(chained)>, "gain + gain is a dimensionless gain");

	// level - level -> gain (the ratio of two levels is a relative dB, i.e. a delta)
	const auto ratio = dBW<double>(30.0) - dBW<double>(10.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(ratio.value(), 5.0e-5));
	static_assert(traits::is_dimensionless_unit_v<decltype(ratio)>, "level - level is a dimensionless gain");
}

TEST_CASE_METHOD(UnitType, "UnitType dBSubtraction", "[wpimath][units]")
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) - decibels<double>(30.0);
	CHECK_THAT(-20.0, Catch::Matchers::WithinAbs(result_dbw.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	CHECK(isSame);

	auto result_dbm = dBm<double>(100.0) - decibels<double>(30.0);
	CHECK_THAT(70.0, Catch::Matchers::WithinAbs(result_dbm.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(result_dbm), dBm<double>>;
	CHECK(isSame);

	auto result_db = dBW<double>(100.0) - dBW<double>(80.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result_db.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(result_db), decibels<double>>;
	CHECK(isSame);

	result_db = decibels<double>(100.0) - decibels<double>(80.0);
	CHECK_THAT(20.0, Catch::Matchers::WithinAbs(result_db.value(), 5.0e-5));
	isSame = std::is_same_v<decltype(result_db), decibels<double>>;
	CHECK(isSame);
}

TEST_CASE_METHOD(UnitType, "UnitType unit_cast", "[wpimath][units]")
{
	meters<double>   test1(5.7);
	hectares<double> test2(16);

	double dResult1 = 5.7;

	double dResult2 = 16;
	int    iResult2 = 16;

	CHECK(dResult1 == unit_cast<double>(test1));
	CHECK(dResult2 == unit_cast<double>(test2));
	CHECK(iResult2 == unit_cast<int>(test2));

	static_assert(std::is_same_v<double, decltype(unit_cast<double>(test1))>);
	static_assert(std::is_same_v<int, decltype(unit_cast<int>(test2))>);
}

// literal syntax is only supported in GCC 4.7+ and MSVC2015+
TEST_CASE_METHOD(UnitType, "UnitType literals", "[wpimath][units]")
{
	// A literal is always floating-point — an integer literal (16_m) yields the same type as 16.0_m, matching
	// the unit-constant form (16 * m is also floating-point), so a value written inline never silently becomes
	// integer-backed (16_m / 5_m is 3.2, not 3). An integer-backed quantity remains available explicitly
	// (meters<int>(16)) or by CTAD from an integer argument (meters(16)); a whole-number literal converts into
	// one at compile time (meters<int> m = 16_m), while a fractional literal is a compile error.
	static_assert(std::is_same_v<decltype(16.2_m), meters<double>>);
	static_assert(std::is_same_v<decltype(16_m), meters<double>>);
	static_assert(std::is_same_v<decltype(16_m), decltype(16.0_m)>);
	CHECK(meters<double>(16.2) == 16.2_m);
	CHECK(meters<double>(16) == 16.0_m);
	CHECK(meters<double>(16) == 16_m);
	CHECK(3.2 == (16_m / 5_m).value());   // floating-point division, not integer truncation
	static_assert(std::is_same_v<decltype(meters<int>(16)), meters<int>>);   // integer reachable explicitly
	static_assert(std::is_same_v<decltype(meters(16)), meters<int>>);        // and by CTAD from an int argument
	CHECK(16 == (meters<int>{16_m}).value());     // whole-number literal narrows into meters<int> at compile time
	CHECK(meters<int>(16) == 16_m);           // meters<int> compares equal to the floating-point literal

	static_assert(std::is_same_v<decltype(11.2_ft), feet<double>>);
	static_assert(std::is_same_v<decltype(11_ft), feet<double>>);
	CHECK(feet<double>(11.2) == 11.2_ft);
	CHECK(feet<double>(11) == 11.0_ft);
	CHECK(11 == (feet<int>{11_ft}).value());      // whole-number literal narrows into feet<int>
	CHECK(feet<int>(11) == 11_ft);

	// auto using literal syntax
	auto x = 10.0_m;
	static_assert(std::is_same_v<decltype(x), meters<double>>);
	CHECK(meters<double>(10) == x);

	// conversion using literal syntax
	feet<double> y = 0.3048_m;
	CHECK(1.0_ft == y);

	// Pythagorean theorem
	meters<double> a_m = 3.0_m;
	meters<double> b_m = 4.0_m;
	meters<double> c_m = sqrt(pow<2>(a_m) + pow<2>(b_m));
	CHECK(c_m == 5.0_m);
}

// A whole-number floating-point quantity converts into an integer-backed unit of the same dimension at
// compile time; the ordinary run-time converting constructor still rejects a floating-to-integral conversion
// (a fractional or run-time value is ill-formed — proven by the errorMessages case
// narrow_fractional_literal_to_int.cpp). This is what lets `feet<int> f = 16_ft;` compile now that a literal
// is floating-point, without allowing a lossy run-time narrowing.
TEST_CASE_METHOD(UnitType, "UnitType compileTimeNarrowingToIntegral", "[wpimath][units]")
{
	// Whole-number literal narrows into the same unit's integral form, at compile time.
	static_assert(meters<int>{16_m}.value() == 16);
	static_assert(feet<int>{11_ft}.value() == 11);

	// Cross-unit whole conversions narrow when exact: 1000 m is exactly 1 km.
	static_assert(kilometers<int>{1000_m}.value() == 1);
	static_assert(meters<int>{1_km}.value() == 1000);

	// Ratio-dimensionless units narrow on their stored point count, not the fraction: 50_pct is percent<int> 50.
	static_assert(percent<int>{50_pct}.raw() == 50);

	// Widening int -> double is a normal implicit conversion.
	static_assert(std::is_constructible_v<meters<double>, meters<int>>);
	// The floating-to-integral narrowing constructor is consteval, so it is well-formed only in a constant
	// expression — a run-time value cannot invoke it, and a fractional value is ill-formed even in one. Both
	// rejections are proven by the errorMessages case narrow_fractional_literal_to_int.cpp; a type trait cannot
	// express "constructible only in a constant expression," so the guard lives there rather than as a
	// static_assert here.

	// Run-time confirmation the compile-time narrowing stored the right value.
	constexpr feet<int> f{16_ft};
	CHECK(16 == f.value());
}

TEST_CASE_METHOD(UnitType, "UnitType Constants", "[wpimath][units]")
{
	// simple numeric operations
	static_assert(10 * km / 2 == 5 * km);

	// conversions to common units
	static_assert(1 * hr == 3600 * s);
	static_assert(1 * km + 1 * m == 1001 * m);

	// derived quantities
	static_assert(1 * km / (1 * s) == 1000 * m / s);
	static_assert(2 * km / hr * (2 * hr) == 4 * km);
	static_assert(2 * km / (2 * km / hr) == 1 * hr);

	static_assert(2 * m * (3 * m) == 6 * m2);

	static_assert(10 * km / (5 * km) == 2);

	static_assert(1000 / (1 * s) == 1 * kHz);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor length", "[wpimath][units]")
{
	double test;
	test = nanometers<double>(0.000000001_m).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = micrometers<double>(meters<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = millimeters<double>(meters<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = centimeters<double>(meters<double>(0.01)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = kilometers<double>(meters<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = meters<double>(meters<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = feet<double>(meters<double>(0.3048)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = miles<double>(meters<double>(1609.344)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = inches<double>(meters<double>(0.0254)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = nautical_miles<double>(meters<double>(1852.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = astronomical_units<double>(meters<double>(149597870700.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = lightyears<double>(meters<double>(9460730472580800.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = parsecs<double>(meters<double>(3.08567758e16)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e7));

	test = feet<double>(feet<double>(6.3)).value();
	CHECK_THAT(6.3, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = inches<double>(feet<double>(6.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = feet<double>(inches<double>(6.0)).value();
	CHECK_THAT(0.5, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = feet<double>(meters<double>(1.0)).value();
	CHECK_THAT(3.28084, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = nautical_miles<double>(miles<double>(6.3)).value();
	CHECK_THAT(5.47455, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = meters<double>(miles<double>(11.0)).value();
	CHECK_THAT(17702.8, Catch::Matchers::WithinAbs(test, 5.0e-2));
	test = chains<double>(meters<double>(1.0)).value();
	CHECK_THAT(0.0497097, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = inches<double>(mils<double>(1.0)).value();
	CHECK_THAT(0.001, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = mils<double>(inches<double>(1.0)).value();
	CHECK_THAT(1000, Catch::Matchers::WithinAbs(test, 5.0e-7));

	CHECK(metres<double>(1) == meters<double>(1));

	CHECK(1_m == 1 * m);
	CHECK(2_ft == 2 * ft);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor mass", "[wpimath][units]")
{
	double test;

	test = grams<double>(kilograms<double>(1.0e-3)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = micrograms<double>(kilograms<double>(1.0e-9)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = milligrams<double>(kilograms<double>(1.0e-6)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = kilograms<double>(kilograms<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = tonnes<double>(kilograms<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = mass::pounds<double>(kilograms<double>(0.453592)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = long_tons<double>(kilograms<double>(1016.05)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = short_tons<double>(kilograms<double>(907.185)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = mass::ounces<double>(kilograms<double>(0.0283495)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = carats<double>(kilograms<double>(0.0002)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = kilograms<double>(slugs<double>(1.0)).value();
	CHECK_THAT(14.593902937206364, Catch::Matchers::WithinAbs(test, 5.0e-13));

	test = carats<double>(mass::pounds<double>(6.3)).value();
	CHECK_THAT(14288.2, Catch::Matchers::WithinAbs(test, 5.0e-2));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor time", "[wpimath][units]")
{
	double result = 0;

	constexpr double daysPerYear = 365;
	constexpr double hoursPerDay = 24;
	constexpr double minsPerHour = 60;
	constexpr double secsPerMin  = 60;
	constexpr double daysPerWeek = 7;

	result = 2 * daysPerYear * hoursPerDay * minsPerHour * secsPerMin * (1 / minsPerHour) * (1 / secsPerMin) * (1 / hoursPerDay) * (1 / daysPerWeek);
	CHECK_THAT(104.286, Catch::Matchers::WithinAbs(result, 5.0e-4));

	years<double> twoYears(2.0);
	weeks<double> twoYearsInWeeks = twoYears;
	CHECK_THAT(weeks<double>(104.286).to<double>(), Catch::Matchers::WithinAbs(twoYearsInWeeks.to<double>(), 5.0e-4));

	double test;

	test = seconds<double>(seconds<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = nanoseconds<double>(seconds<double>(1.0e-9)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = microseconds<double>(seconds<double>(1.0e-6)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = milliseconds<double>(seconds<double>(1.0e-3)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = minutes<double>(seconds<double>(60.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = hours<double>(seconds<double>(3600.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = days<double>(seconds<double>(86400.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = weeks<double>(seconds<double>(604800.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = years<double>(seconds<double>(3.154e7)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e3));

	test = weeks<double>(years<double>(2.0)).value();
	CHECK_THAT(104.2857142857143, Catch::Matchers::WithinAbs(test, 5.0e-14));
	test = minutes<double>(hours<double>(4.0)).value();
	CHECK_THAT(240.0, Catch::Matchers::WithinAbs(test, 5.0e-14));
	test = days<double>(julian_years<double>(1.0)).value();
	CHECK_THAT(365.25, Catch::Matchers::WithinAbs(test, 5.0e-14));
	test = days<double>(gregorian_years<double>(1.0)).value();
	CHECK_THAT(365.2425, Catch::Matchers::WithinAbs(test, 5.0e-14));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor angle", "[wpimath][units]")
{
	angle::degrees<double> quarterCircleDeg(90.0);
	angle::radians<double> quarterCircleRad = quarterCircleDeg;
	CHECK_THAT(angle::radians<double>(detail::PI_VAL / 2.0).to<double>(), Catch::Matchers::WithinAbs(quarterCircleRad.to<double>(), 5.0e-12));

	double test;

	test = angle::radians<double>(angle::radians<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = angle::milliradians<double>(angle::radians<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = angle::degrees<double>(angle::radians<double>(0.0174533)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = angle::arcminutes<double>(angle::radians<double>(0.000290888)).value();
	CHECK_THAT(0.99999928265913, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = angle::arcseconds<double>(angle::radians<double>(4.8481e-6)).value();
	CHECK_THAT(0.999992407, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = angle::turns<double>(angle::radians<double>(6.28319)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = angle::gradians<double>(angle::radians<double>(0.015708)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-6));

	test = angle::radians<double>(angle::radians<double>(2.1)).value();
	CHECK_THAT(2.1, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = angle::gradians<double>(angle::arcseconds<double>(2.1)).value();
	CHECK_THAT(0.000648148, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = angle::degrees<double>(angle::radians<double>(detail::PI_VAL)).value();
	CHECK_THAT(180.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = angle::radians<double>(angle::degrees<double>(90.0)).value();
	CHECK_THAT(detail::PI_VAL / 2, Catch::Matchers::WithinAbs(test, 5.0e-6));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor current", "[wpimath][units]")
{
	double test;

	test = current::milliamperes<double>(current::amperes<double>(2.1)).value();
	CHECK_THAT(2100.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor temperature", "[wpimath][units]")
{
	// temp conversion are weird/hard since they involve translations AND scaling.
	double test;

	test = kelvin<double>(kelvin<double>(72.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = fahrenheit<double>(fahrenheit<double>(72.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = fahrenheit<double>(kelvin<double>(300.0)).value();
	CHECK_THAT(80.33, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kelvin<double>(fahrenheit<double>(451.0)).value();
	CHECK_THAT(505.928, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = celsius<double>(kelvin<double>(300.0)).value();
	CHECK_THAT(26.85, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = kelvin<double>(celsius<double>(451.0)).value();
	CHECK_THAT(724.15, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = celsius<double>(fahrenheit<double>(72.0)).value();
	CHECK_THAT(22.2222, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = fahrenheit<double>(celsius<double>(100.0)).value();
	CHECK_THAT(212.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = celsius<double>(fahrenheit<double>(32.0)).value();
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = fahrenheit<double>(celsius<double>(0.0)).value();
	CHECK_THAT(32.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kelvin<double>(rankine<double>(100.0)).value();
	CHECK_THAT(55.5556, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = rankine<double>(kelvin<double>(100.0)).value();
	CHECK_THAT(180.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = rankine<double>(fahrenheit<double>(100.0)).value();
	CHECK_THAT(559.67, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = fahrenheit<double>(rankine<double>(72.0)).value();
	CHECK_THAT(-387.67, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kelvin<double>(reaumur<double>(100.0)).value();
	CHECK_THAT(398.0, Catch::Matchers::WithinAbs(test, 5.0e-1));
	test = celsius<double>(reaumur<double>(80.0)).value();
	CHECK_THAT(100.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = reaumur<double>(celsius<double>(212.0)).value();
	CHECK_THAT(169.6, Catch::Matchers::WithinAbs(test, 5.0e-2));
	test = fahrenheit<double>(reaumur<double>(80.0)).value();
	CHECK_THAT(212.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = reaumur<double>(fahrenheit<double>(37.0)).value();
	CHECK_THAT(2.222, Catch::Matchers::WithinAbs(test, 5.0e-3));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor luminous_intensity", "[wpimath][units]")
{
	double test;

	test = millicandelas<double>(candelas<double>(72.0)).value();
	CHECK_THAT(72000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = candelas<double>(millicandelas<double>(376.0)).value();
	CHECK_THAT(0.376, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor substance", "[wpimath][units]")
{
	static_assert(1_g / 1_mol == 1_g_per_mol);
	static_assert(1_mol / 1_g == 1_M);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor solid_angle", "[wpimath][units]")
{
	double test;
	bool   same;

	same = std::is_same_v<traits::dimension_of_t<steradians<double>>, traits::dimension_of_t<degrees_squared<double>>>;
	CHECK(same);

	test = steradians<double>(steradians<double>(72.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = degrees_squared<double>(steradians<double>(1.0)).value();
	CHECK_THAT(3282.8, Catch::Matchers::WithinAbs(test, 5.0e-2));
	test = spats<double>(steradians<double>(8.0)).value();
	CHECK_THAT(0.636619772367582, Catch::Matchers::WithinAbs(test, 5.0e-14));
	test = steradians<double>(degrees_squared<double>(3282.8)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = degrees_squared<double>(degrees_squared<double>(72.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = spats<double>(degrees_squared<double>(3282.8)).value();
	CHECK_THAT(1.0 / (4 * detail::PI_VAL), Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = steradians<double>(spats<double>(1.0 / (4 * detail::PI_VAL))).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-14));
	test = degrees_squared<double>(spats<double>(1.0 / (4 * detail::PI_VAL))).value();
	CHECK_THAT(3282.8, Catch::Matchers::WithinAbs(test, 5.0e-2));
	test = spats<double>(spats<double>(72.0)).value();
	CHECK_THAT(72.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor frequency", "[wpimath][units]")
{
	double test;

	test = kilohertz<double>(hertz<double>(63000.0)).value();
	CHECK_THAT(63.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = hertz<double>(hertz<double>(6.3)).value();
	CHECK_THAT(6.3, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = hertz<double>(kilohertz<double>(5.0)).value();
	CHECK_THAT(5000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = hertz<double>(megahertz<double>(1.0)).value();
	CHECK_THAT(1.0e6, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor volume_flow_rate", "[wpimath][units]")
{
	double test;
	bool   same;

	same = std::is_same_v<cubic_meters_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::volume_flow_rate>>>;
	CHECK(same);

	same = traits::is_same_dimension_unit_v<liters_per_second<double>, cubic_meters_per_second<double>>;
	CHECK(same);

	// a volume divided by a time is a volume flow rate (dimension derived from volume/time)
	same = traits::is_same_dimension_unit_v<decltype(liters<double>(1.0) / seconds<double>(1.0)), liters_per_second<double>>;
	CHECK(same);

	test = liters_per_second<double>(cubic_meters_per_second<double>(1.0)).value();
	CHECK(1000.0 == test);
	test = liters_per_minute<double>(liters_per_second<double>(1.0)).value();
	CHECK(60.0 == test);
	test = cubic_feet_per_minute<double>(cubic_feet_per_second<double>(1.0)).value();
	CHECK(60.0 == test);
	test = liters_per_second<double>(gallons_per_minute<double>(1.0)).value();
	CHECK_THAT(0.0630901964, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = liters_per_second<double>(cubic_feet_per_second<double>(1.0)).value();
	CHECK_THAT(28.316846592, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = cubic_meters_per_hour<double>(gallons_per_minute<double>(100.0)).value();
	CHECK_THAT(22.712470704, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = cubic_meters_per_hour<double>(cubic_feet_per_minute<double>(1.0)).value();
	CHECK_THAT(1.69901079552, Catch::Matchers::WithinAbs(test, 5.0e-11));

	// each remaining named unit checked against its most natural base
	test = cubic_meters_per_hour<double>(cubic_meters_per_second<double>(1.0)).value();
	CHECK(3600.0 == test);
	test = gallons_per_minute<double>(gallons_per_hour<double>(60.0)).value();
	CHECK(1.0 == test);
	test = liters_per_second<double>(gallons_per_hour<double>(1.0)).value();
	CHECK_THAT(0.00105150327, Catch::Matchers::WithinAbs(test, 5.0e-11));

	// deriving the dimension from volume / time means the composed quantity IS the named unit: it
	// resolves to liters_per_second and streams with that unit's abbreviation, and converts as expected
	same = std::is_same_v<decltype(5.0_L / 1.0_s), liters_per_second<double>>;
	CHECK(same);
	test = gallons_per_minute<double>(5.0_L / 1.0_s).value();
	CHECK_THAT(79.2516157, Catch::Matchers::WithinAbs(test, 5.0e-7));
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	{
		testing::internal::CaptureStdout();
		std::cout << (5.0_L / 1.0_s);
		std::string output = testing::internal::GetCapturedStdout();
		CHECK(std::string_view("5 L_per_s") == output.c_str());
	}
#endif
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor velocity", "[wpimath][units]")
{
	double test;
	bool   same;

	same = std::is_same_v<meters_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::velocity>>>;
	CHECK(same);

	same = traits::is_same_dimension_unit_v<miles_per_hour<double>, meters_per_second<double>>;
	CHECK(same);

	test = miles_per_hour<double>(meters_per_second<double>(1250.0)).value();
	CHECK_THAT(2796.17, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = kilometers_per_hour<double>(feet_per_second<double>(2796.17)).value();
	CHECK_THAT(3068.181418, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = miles_per_hour<double>(knots<double>(600.0)).value();
	CHECK_THAT(690.468, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = feet_per_second<double>(miles_per_hour<double>(120.0)).value();
	CHECK_THAT(176.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = meters_per_second<double>(feet_per_second<double>(10.0)).value();
	CHECK_THAT(3.048, Catch::Matchers::WithinAbs(test, 5.0e-5));

	CHECK(10_mps == 10 * m / s);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor angular_velocity", "[wpimath][units]")
{
	double test;
	bool   same;

	same = std::is_same_v<radians_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_velocity>>>;
	CHECK(same);

	same = traits::is_same_dimension_conversion_factor_v<revolutions_per_minute<double>, radians_per_second<double>>;
	CHECK(same);

	test = milliarcseconds_per_year<double>(radians_per_second<double>(1.0)).value();
	CHECK_THAT(6.504e15, Catch::Matchers::WithinAbs(test, 1.0e12));
	test = radians_per_second<double>(degrees_per_second<double>(1.0)).value();
	CHECK_THAT(0.0174533, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = radians_per_second<double>(revolutions_per_minute<double>(1.0)).value();
	CHECK_THAT(0.10471975512, Catch::Matchers::WithinAbs(test, 5.0e-13));
	test = radians_per_second<double>(milliarcseconds_per_year<double>(1.0)).value();
	CHECK_THAT(1.537e-16, Catch::Matchers::WithinAbs(test, 5.0e-20));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor acceleration", "[wpimath][units]")
{
	double test;

	test = meters_per_second_squared<double>(standard_gravity<double>(1.0)).value();
	CHECK_THAT(9.80665, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = standard_gravity<double>(feet_per_second_squared<double>(1.0)).value();
	CHECK_THAT(0.0310810, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = feet_per_second_squared<double>(gals<double>(1.0)).value();
	CHECK_THAT(0.0328084, Catch::Matchers::WithinAbs(test, 5.0e-6));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor force", "[wpimath][units]")
{
	double test;

	test = wpi::units::force::newtons<double>(wpi::units::force::newtons<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = wpi::units::force::pounds<double>(wpi::units::force::newtons<double>(6.3)).value();
	CHECK_THAT(1.4163, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = wpi::units::force::dynes<double>(wpi::units::force::newtons<double>(5.0)).value();
	CHECK_THAT(500000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = wpi::units::force::poundals<double>(wpi::units::force::newtons<double>(2.1)).value();
	CHECK_THAT(15.1893, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = wpi::units::force::kiloponds<double>(wpi::units::force::newtons<double>(173.0)).value();
	CHECK_THAT(17.6411, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = wpi::units::force::kiloponds<double>(wpi::units::force::poundals<double>(21.879)).value();
	CHECK_THAT(0.308451933, Catch::Matchers::WithinAbs(test, 5.0e-10));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor area", "[wpimath][units]")
{
	double test;

	test = acres<double>(hectares<double>(6.3)).value();
	CHECK_THAT(15.5676, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = square_kilometers<double>(square_miles<double>(10.0)).value();
	CHECK_THAT(25.8999, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = square_meters<double>(square_inches<double>(4.0)).value();
	CHECK_THAT(0.00258064, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = square_feet<double>(acres<double>(5.0)).value();
	CHECK_THAT(217800.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = square_feet<double>(square_meters<double>(1.0)).value();
	CHECK_THAT(10.7639, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor pressure", "[wpimath][units]")
{
	double test;

	test = torrs<double>(pascals<double>(1.0)).value();
	CHECK_THAT(0.00750062, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = pounds_per_square_inch<double>(bars<double>(2.2)).value();
	CHECK_THAT(31.9083, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = bars<double>(atmospheres<double>(4.0)).value();
	CHECK_THAT(4.053, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = pascals<double>(torrs<double>(800.0)).value();
	CHECK_THAT(106657.89474, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = atmospheres<double>(pounds_per_square_inch<double>(38.0)).value();
	CHECK_THAT(2.58575, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = pascals<double>(pounds_per_square_inch<double>(1.0)).value();
	CHECK_THAT(6894.76, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = bars<double>(pascals<double>(0.25)).value();
	CHECK_THAT(2.5e-6, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = atmospheres<double>(torrs<double>(9.0)).value();
	CHECK_THAT(0.0118421, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = torrs<double>(bars<double>(12.0)).value();
	CHECK_THAT(9000.74, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = pounds_per_square_inch<double>(atmospheres<double>(1.0)).value();
	CHECK_THAT(14.6959, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = millimeters_of_mercury(inches_of_mercury{1.0}).value();
	CHECK_THAT(25.4, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = pascals(inches_of_mercury{1.0}).value();
	CHECK_THAT(3386.38864, Catch::Matchers::WithinAbs(test, 5.0e-5));

	CHECK(133.322387415_Pa == 1.0_mmHg);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor charge", "[wpimath][units]")
{
	double test;

	test = ampere_hours<double>(coulombs<double>(4.0)).value();
	CHECK_THAT(0.00111111, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = coulombs<double>(ampere_hours<double>(1.0)).value();
	CHECK_THAT(3600.0, Catch::Matchers::WithinAbs(test, 5.0e-6));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor energy", "[wpimath][units]")
{
	double test;

	test = calories<double>(joules<double>(8000.000464)).value();
	CHECK_THAT(1912.046, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = joules<double>(therms<double>(12.0)).value();
	CHECK_THAT(1.266e+9, Catch::Matchers::WithinAbs(test, 5.0e5));
	test = watt_hours<double>(megajoules<double>(100.0)).value();
	CHECK_THAT(27777.778, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = megajoules<double>(kilocalories<double>(56.0)).value();
	CHECK_THAT(0.234304, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = therms<double>(kilojoules<double>(56.0)).value();
	CHECK_THAT(0.000530904, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kilojoules<double>(british_thermal_units<double>(18.56399995447)).value();
	CHECK_THAT(19.5860568, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = energy::foot_pounds<double>(calories<double>(18.56399995447)).value();
	CHECK_THAT(57.28776190423856, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = calories<double>(megajoules<double>(1.0)).value();
	CHECK_THAT(239006.0, Catch::Matchers::WithinAbs(test, 5.0e-1));
	test = kilowatt_hours<double>(kilocalories<double>(2.0)).value();
	CHECK_THAT(0.00232444, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = kilocalories<double>(therms<double>(0.1)).value();
	CHECK_THAT(2521.04, Catch::Matchers::WithinAbs(test, 5.0e-3));
	test = megajoules<double>(watt_hours<double>(67.0)).value();
	CHECK_THAT(0.2412, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = watt_hours<double>(british_thermal_units<double>(100.0)).value();
	CHECK_THAT(29.3071, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = british_thermal_units<double>(calories<double>(100.0)).value();
	CHECK_THAT(0.396567, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor power", "[wpimath][units]")
{
	double test;

	test = watts<double>(unit<compound_conversion_factor<energy::foot_pounds<double>, inverse<seconds<double>>>>(550.0)).value();
	CHECK_THAT(745.7, Catch::Matchers::WithinAbs(test, 5.0e-2));
	test = gigawatts<double>(watts<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = watts<double>(microwatts<double>(200000.0)).value();
	CHECK_THAT(0.2, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = watts<double>(horsepower<double>(100.0)).value();
	CHECK_THAT(74570.0, Catch::Matchers::WithinAbs(test, 5.0e-1));
	test = megawatts<double>(horsepower<double>(5.0)).value();
	CHECK_THAT(0.0037284994, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = horsepower<double>(kilowatts<double>(232.0)).value();
	CHECK_THAT(311.117, Catch::Matchers::WithinAbs(test, 5.0e-4));
	test = horsepower<double>(milliwatts<double>(1001.0)).value();
	CHECK_THAT(0.001342363, Catch::Matchers::WithinAbs(test, 5.0e-9));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor voltage", "[wpimath][units]")
{
	double test;

	test = millivolts<double>(volts<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(picovolts<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(nanovolts<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(microvolts<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(millivolts<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(kilovolts<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(megavolts<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = volts<double>(gigavolts<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	// 1 statvolt == c/1e6 volts == 299.792458 V (was defined inverted before 3.4.2).
	test = volts<double>(statvolts<double>(1.0)).value();
	CHECK_THAT(299.792458, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = statvolts<double>(volts<double>(299.792458)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = nanovolts<double>(abvolts<double>(0.1)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = abvolts<double>(microvolts<double>(0.01)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor capacitance", "[wpimath][units]")
{
	double test;

	test = millifarads<double>(farads<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(picofarads<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(nanofarads<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(microfarads<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(millifarads<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(kilofarads<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(megafarads<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = farads<double>(gigafarads<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	auto                            f  = coulombs<double>(1) / volts<double>(1);
	[[maybe_unused]] farads<double> f2 = coulombs<double>(1) / volts<double>(1);
	static_assert(std::is_convertible_v<decltype(f), farads<double>>);

	auto one_farad = []() -> farads<double> { return coulombs<double>(1) / volts<double>(1); };

	CHECK(1.0_F == one_farad());
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor impedance", "[wpimath][units]")
{
	double test;

	test = milliohms<double>(ohms<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(picoohms<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(nanoohms<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(microohms<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(milliohms<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(kiloohms<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(megaohms<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = ohms<double>(gigaohms<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor conductance", "[wpimath][units]")
{
	double test;

	test = millisiemens<double>(siemens<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(picosiemens<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(nanosiemens<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(microsiemens<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(millisiemens<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(kilosiemens<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(megasiemens<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = siemens<double>(gigasiemens<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor magnetic_flux", "[wpimath][units]")
{
	double test;

	test = milliwebers<double>(webers<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(picowebers<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(nanowebers<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(microwebers<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(milliwebers<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(kilowebers<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(megawebers<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(gigawebers<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = webers<double>(maxwells<double>(100000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = maxwells<double>(nanowebers<double>(10.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor magnetic_field_strength", "[wpimath][units]")
{
	double test;

	test = milliteslas<double>(teslas<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(picoteslas<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(nanoteslas<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(microteslas<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(milliteslas<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(kiloteslas<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(megateslas<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(gigateslas<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = teslas<double>(gauss<double>(10000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = gauss<double>(nanoteslas<double>(100000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor inductance", "[wpimath][units]")
{
	double test;

	test = millihenries<double>(henries<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(picohenries<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(nanohenries<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(microhenries<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(millihenries<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(kilohenries<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(megahenries<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = henries<double>(gigahenries<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor jerk", "[wpimath][units]")
{
	double test;
	test = meters_per_second_cubed(feet_per_second_cubed{3.280839895}).value();
	CHECK_THAT(test, Catch::Matchers::WithinAbs(1.0, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor luminous_flux", "[wpimath][units]")
{
	double test;

	test = millilumens<double>(lumens<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(picolumens<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(nanolumens<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(microlumens<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(millilumens<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(kilolumens<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(megalumens<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lumens<double>(gigalumens<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor illuminance", "[wpimath][units]")
{
	double test;

	test = millilux<double>(lux<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(picolux<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(nanolux<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(microlux<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(millilux<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(kilolux<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(megalux<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = lux<double>(gigalux<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	test = lumens_per_square_inch<double>(lux<double>(1550.0031000062)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-13));
	test = lux<double>(phots<double>(0.0001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor luminance", "[wpimath][units]")
{
	candelas_per_square_meter<double> test;

	test = stilbs<double>(1.0);
	CHECK(test.value() == pow<4>(10.0));
	test = apostilbs<double>(2.0);
	CHECK(test.value() == 0.63661977236758138);
	test = brils<double>(1.0);
	CHECK(test.value() == 3.1830988618379068e-08);
	test = skots<double>(1.0);
	CHECK(test.value() == 0.0003183098861837907);
	test = lamberts<double>(1.0);
	CHECK(test.value() == 3183.098861837907);
	test = foot_lamberts<double>(1.0);
	CHECK(test.value() == 3.4262590996353905);

	brils<double> test2 = foot_lamberts<double>(89.46);
	CHECK(test2.value() == 9629394258.788517);

	millilamberts<double> test3 = blondels<double>(89.46);
	CHECK(test3.value() == 8.946);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor radiation", "[wpimath][units]")
{
	double test;

	test = millibecquerels<double>(becquerels<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(picobecquerels<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(nanobecquerels<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(microbecquerels<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(millibecquerels<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(kilobecquerels<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(megabecquerels<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = becquerels<double>(gigabecquerels<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	test = milligrays<double>(grays<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(picograys<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(nanograys<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(micrograys<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(milligrays<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(kilograys<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(megagrays<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(gigagrays<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	test = millisieverts<double>(sieverts<double>(10.0)).value();
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(picosieverts<double>(1000000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(nanosieverts<double>(1000000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(microsieverts<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(millisieverts<double>(1000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(kilosieverts<double>(0.001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(megasieverts<double>(0.000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = sieverts<double>(gigasieverts<double>(0.000000001)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));

	test = curies<double>(becquerels<double>(37.0e9)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = rutherfords<double>(becquerels<double>(1000000.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = grays<double>(radiation_absorbed_dose<double>(100.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor torque", "[wpimath][units]")
{
	double test;

	test = newton_meters<double>(torque::pound_feet<double>(1.0)).value();
	CHECK_THAT(1.355817948, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = newton_meters<double>(inch_pounds<double>(1.0)).value();
	CHECK_THAT(0.112984829, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = newton_meters<double>(foot_poundals<double>(1.0)).value();
	CHECK_THAT(4.214011009e-2, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = newton_meters<double>(meter_kilograms<double>(1.0)).value();
	CHECK_THAT(9.80665, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = meter_kilograms<double>(inch_pounds<double>(86.79616930855788)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = inch_pounds<double>(foot_poundals<double>(2.681170713)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor volume", "[wpimath][units]")
{
	double test;

	test = cubic_meters<double>(cubic_meters<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(cubic_millimeters<double>(1.0)).value();
	CHECK_THAT(1.0e-9, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(cubic_kilometers<double>(1.0)).value();
	CHECK_THAT(1.0e9, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(liters<double>(1.0)).value();
	CHECK_THAT(0.001, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(milliliters<double>(1.0)).value();
	CHECK_THAT(1.0e-6, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(cubic_inches<double>(1.0)).value();
	CHECK_THAT(1.6387e-5, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = cubic_meters<double>(cubic_feet<double>(1.0)).value();
	CHECK_THAT(0.0283168, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = cubic_meters<double>(cubic_yards<double>(1.0)).value();
	CHECK_THAT(0.764555, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = cubic_meters<double>(cubic_miles<double>(1.0)).value();
	CHECK_THAT(4.168e+9, Catch::Matchers::WithinAbs(test, 5.0e5));
	test = cubic_meters<double>(gallons<double>(1.0)).value();
	CHECK_THAT(0.00378541, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = cubic_meters<double>(quarts<double>(1.0)).value();
	CHECK_THAT(0.000946353, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = cubic_meters<double>(pints<double>(1.0)).value();
	CHECK_THAT(0.000473176, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = cubic_meters<double>(cups<double>(1.0)).value();
	CHECK_THAT(0.00024, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = cubic_meters<double>(volume::fluid_ounces<double>(1.0)).value();
	CHECK_THAT(2.9574e-5, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(barrels<double>(1.0)).value();
	CHECK_THAT(0.158987294928, Catch::Matchers::WithinAbs(test, 5.0e-13));
	test = cubic_meters<double>(bushels<double>(1.0)).value();
	CHECK_THAT(0.0352391, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = cubic_meters<double>(cords<double>(1.0)).value();
	CHECK_THAT(3.62456, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = cubic_meters<double>(cubic_fathoms<double>(1.0)).value();
	CHECK_THAT(6.11644, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = cubic_meters<double>(tablespoons<double>(1.0)).value();
	CHECK_THAT(1.4787e-5, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = cubic_meters<double>(teaspoons<double>(1.0)).value();
	CHECK_THAT(4.9289e-6, Catch::Matchers::WithinAbs(test, 5.0e-11));
	test = cubic_meters<double>(pinches<double>(1.0)).value();
	CHECK_THAT(616.11519921875e-9, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = cubic_meters<double>(dashes<double>(1.0)).value();
	CHECK_THAT(308.057599609375e-9, Catch::Matchers::WithinAbs(test, 5.0e-20));
	test = cubic_meters<double>(drops<double>(1.0)).value();
	CHECK_THAT(82.14869322916e-9, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = cubic_meters<double>(fifths<double>(1.0)).value();
	CHECK_THAT(0.00075708236, Catch::Matchers::WithinAbs(test, 5.0e-12));
	test = cubic_meters<double>(drams<double>(1.0)).value();
	CHECK_THAT(3.69669e-6, Catch::Matchers::WithinAbs(test, 5.0e-12));
	test = cubic_meters<double>(gills<double>(1.0)).value();
	CHECK_THAT(0.000118294, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = cubic_meters<double>(pecks<double>(1.0)).value();
	CHECK_THAT(0.00880977, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = cubic_meters<double>(sacks<double>(9.4591978)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = cubic_meters<double>(shots<double>(1.0)).value();
	CHECK_THAT(4.43603e-5, Catch::Matchers::WithinAbs(test, 5.0e-11));
	test = cubic_meters<double>(strikes<double>(1.0)).value();
	CHECK_THAT(0.07047814033376, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = milliliters<double>(volume::fluid_ounces<double>(1.0)).value();
	CHECK_THAT(29.5735, Catch::Matchers::WithinAbs(test, 5.0e-5));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor density", "[wpimath][units]")
{
	double test;

	test = kilograms_per_cubic_meter<double>(kilograms_per_cubic_meter<double>(1.0)).value();
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kilograms_per_cubic_meter<double>(grams_per_milliliter<double>(1.0)).value();
	CHECK_THAT(1000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kilograms_per_cubic_meter<double>(kilograms_per_liter<double>(1.0)).value();
	CHECK_THAT(1000.0, Catch::Matchers::WithinAbs(test, 5.0e-5));
	test = kilograms_per_cubic_meter<double>(ounces_per_cubic_foot<double>(1.0)).value();
	CHECK_THAT(1.001153961, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = kilograms_per_cubic_meter<double>(ounces_per_cubic_inch<double>(1.0)).value();
	CHECK_THAT(1.729994044e3, Catch::Matchers::WithinAbs(test, 5.0e-7));
	test = kilograms_per_cubic_meter<double>(ounces_per_gallon<double>(1.0)).value();
	CHECK_THAT(7.489151707, Catch::Matchers::WithinAbs(test, 5.0e-10));
	test = kilograms_per_cubic_meter<double>(pounds_per_cubic_foot<double>(1.0)).value();
	CHECK_THAT(16.01846337, Catch::Matchers::WithinAbs(test, 5.0e-9));
	test = kilograms_per_cubic_meter<double>(pounds_per_cubic_inch<double>(1.0)).value();
	CHECK_THAT(2.767990471e4, Catch::Matchers::WithinAbs(test, 5.0e-6));
	test = kilograms_per_cubic_meter<double>(pounds_per_gallon<double>(1.0)).value();
	CHECK_THAT(119.8264273, Catch::Matchers::WithinAbs(test, 5.0e-8));
	test = kilograms_per_cubic_meter<double>(slugs_per_cubic_foot<double>(1.0)).value();
	CHECK_THAT(515.3788183931962, Catch::Matchers::WithinAbs(test, 5.0e-11));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor concentration", "[wpimath][units]")
{
	double test;

	test = parts_per_million<double>(1.0);
	CHECK_THAT(1.0e-6, Catch::Matchers::WithinAbs(test, 5.0e-12));
	test = parts_per_billion<double>(1.0);
	CHECK_THAT(1.0e-9, Catch::Matchers::WithinAbs(test, 5.0e-12));
	test = parts_per_trillion<double>(1.0);
	CHECK_THAT(1.0e-12, Catch::Matchers::WithinAbs(test, 5.0e-12));
	test = percent<double>(18.0);
	CHECK_THAT(0.18, Catch::Matchers::WithinAbs(test, 5.0e-12));

	auto test2 = percent(18.0);
	CHECK(test2.to<double>() == 0.18);
	CHECK(test2.value() == 0.18);
	test2 = 0.5;
	CHECK(test2 == 50_pct);
	CHECK(test2.value() == 0.5);
	CHECK(test2.to<double>() == 0.5);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor data", "[wpimath][units]")
{
	CHECK(8 == (bits<double>(bytes<double>(1)).value()));

	CHECK(1000 == (bytes<double>(kilobytes<double>(1)).value()));
	CHECK(1000 == (kilobytes<double>(megabytes<double>(1)).value()));
	CHECK(1000 == (megabytes<double>(gigabytes<double>(1)).value()));
	CHECK(1000 == (gigabytes<double>(terabytes<double>(1)).value()));
	CHECK(1000 == (terabytes<double>(petabytes<double>(1)).value()));
	CHECK(1000 == (petabytes<double>(exabytes<double>(1)).value()));

	CHECK(1024 == (bytes<double>(kibibytes<double>(1)).value()));
	CHECK(1024 == (kibibytes<double>(mebibytes<double>(1)).value()));
	CHECK(1024 == (mebibytes<double>(gibibytes<double>(1)).value()));
	CHECK(1024 == (gibibytes<double>(tebibytes<double>(1)).value()));
	CHECK(1024 == (tebibytes<double>(pebibytes<double>(1)).value()));
	CHECK(1024 == (pebibytes<double>(exbibytes<double>(1)).value()));

	CHECK(93750000 == (kibibits<double>(gigabytes<double>(12)).value()));

	CHECK(1000 == (bits<double>(kilobits<double>(1)).value()));
	CHECK(1000 == (kilobits<double>(megabits<double>(1)).value()));
	CHECK(1000 == (megabits<double>(gigabits<double>(1)).value()));
	CHECK(1000 == (gigabits<double>(terabits<double>(1)).value()));
	CHECK(1000 == (terabits<double>(petabits<double>(1)).value()));
	CHECK(1000 == (petabits<double>(exabits<double>(1)).value()));

	CHECK(1024 == (bits<double>(kibibits<double>(1)).value()));
	CHECK(1024 == (kibibits<double>(mebibits<double>(1)).value()));
	CHECK(1024 == (mebibits<double>(gibibits<double>(1)).value()));
	CHECK(1024 == (gibibits<double>(tebibits<double>(1)).value()));
	CHECK(1024 == (tebibits<double>(pebibits<double>(1)).value()));
	CHECK(1024 == (pebibits<double>(exbibits<double>(1)).value()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	CHECK_THAT(percent<double>(2.4), Catch::Matchers::WithinAbs(kibibytes<double>(1) / kilobytes<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(4.9), Catch::Matchers::WithinAbs(mebibytes<double>(1) / megabytes<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(7.4), Catch::Matchers::WithinAbs(gibibytes<double>(1) / gigabytes<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(10.0), Catch::Matchers::WithinAbs(tebibytes<double>(1) / terabytes<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(12.6), Catch::Matchers::WithinAbs(pebibytes<double>(1) / petabytes<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(15.3), Catch::Matchers::WithinAbs(exbibytes<double>(1) / exabytes<double>(1) - 1, 0.005));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor data_transfer_rate", "[wpimath][units]")
{
	CHECK(8 == (bits_per_second<double>(bytes_per_second<double>(1)).value()));

	CHECK(1000 == (bytes_per_second<double>(kilobytes_per_second<double>(1)).value()));
	CHECK(1000 == (kilobytes_per_second<double>(megabytes_per_second<double>(1)).value()));
	CHECK(1000 == (megabytes_per_second<double>(gigabytes_per_second<double>(1)).value()));
	CHECK(1000 == (gigabytes_per_second<double>(terabytes_per_second<double>(1)).value()));
	CHECK(1000 == (terabytes_per_second<double>(petabytes_per_second<double>(1)).value()));
	CHECK(1000 == (petabytes_per_second<double>(exabytes_per_second<double>(1)).value()));

	CHECK(1024 == (bytes_per_second<double>(kibibytes_per_second<double>(1)).value()));
	CHECK(1024 == (kibibytes_per_second<double>(mebibytes_per_second<double>(1)).value()));
	CHECK(1024 == (mebibytes_per_second<double>(gibibytes_per_second<double>(1)).value()));
	CHECK(1024 == (gibibytes_per_second<double>(tebibytes_per_second<double>(1)).value()));
	CHECK(1024 == (tebibytes_per_second<double>(pebibytes_per_second<double>(1)).value()));
	CHECK(1024 == (pebibytes_per_second<double>(exbibytes_per_second<double>(1)).value()));

	CHECK(93750000 == (kibibits_per_second<double>(gigabytes_per_second<double>(12)).value()));

	CHECK(1000 == (bits_per_second<double>(kilobits_per_second<double>(1)).value()));
	CHECK(1000 == (kilobits_per_second<double>(megabits_per_second<double>(1)).value()));
	CHECK(1000 == (megabits_per_second<double>(gigabits_per_second<double>(1)).value()));
	CHECK(1000 == (gigabits_per_second<double>(terabits_per_second<double>(1)).value()));
	CHECK(1000 == (terabits_per_second<double>(petabits_per_second<double>(1)).value()));
	CHECK(1000 == (petabits_per_second<double>(exabits_per_second<double>(1)).value()));

	CHECK(1024 == (bits_per_second<double>(kibibits_per_second<double>(1)).value()));
	CHECK(1024 == (kibibits_per_second<double>(mebibits_per_second<double>(1)).value()));
	CHECK(1024 == (mebibits_per_second<double>(gibibits_per_second<double>(1)).value()));
	CHECK(1024 == (gibibits_per_second<double>(tebibits_per_second<double>(1)).value()));
	CHECK(1024 == (tebibits_per_second<double>(pebibits_per_second<double>(1)).value()));
	CHECK(1024 == (pebibits_per_second<double>(exbibits_per_second<double>(1)).value()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	CHECK_THAT(percent<double>(2.4), Catch::Matchers::WithinAbs(kibibytes_per_second<double>(1) / kilobytes_per_second<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(4.9), Catch::Matchers::WithinAbs(mebibytes_per_second<double>(1) / megabytes_per_second<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(7.4), Catch::Matchers::WithinAbs(gibibytes_per_second<double>(1) / gigabytes_per_second<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(10.0), Catch::Matchers::WithinAbs(tebibytes_per_second<double>(1) / terabytes_per_second<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(12.6), Catch::Matchers::WithinAbs(pebibytes_per_second<double>(1) / petabytes_per_second<double>(1) - 1, 0.005));
	CHECK_THAT(percent<double>(15.3), Catch::Matchers::WithinAbs(exbibytes_per_second<double>(1) / exabytes_per_second<double>(1) - 1, 0.005));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor energy_density", "[wpimath][units]")
{
	static_assert(wpi::units::traits::is_same_dimension_unit_v<joules_per_meter_cubed<double>, decltype(J / (m * m * m))>);
	static_assert(traits::is_energy_density_unit_v<decltype(J / m3)>);
	static_assert(traits::is_pressure_unit_v<decltype(J / m3)>); // energy density IS pressure

	constexpr joules_per_meter_cubed test = 64.0 * J / 2.0_m3;
	CHECK(test == 32.0_J_per_m3);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor irradiance", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_meter_squared<double>, decltype(W / m2)>);
	static_assert(traits::is_irradiance_unit_v<decltype(W / m2)>);

	constexpr watts_per_meter_squared test = 42.0 * W / 2.0_m2;
	CHECK(test == 21.0_W_per_m2);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor radiance", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_squared<double>, decltype(W / sr / m2)>);
	static_assert(traits::is_radiance_unit_v<decltype(W / sr / m2)>);

	constexpr watts_per_steradian_per_meter_squared test = 44.0 * W / 2.0_sr / 2.0_m2;
	CHECK(test == 11.0_W_per_srm2);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor radiant_intensity", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_steradian<double>, decltype(W / sr)>);
	static_assert(traits::is_radiant_intensity_unit_v<decltype(W / sr)>);

	constexpr watts_per_steradian test = 44.0 * W / 2.0_sr;
	CHECK(test == 22.0_W_per_sr);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor spectral_flux", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_meter<double>, decltype(W / m)>);
	static_assert(traits::is_spectral_flux_unit_v<decltype(W / m)>);

	constexpr watts_per_meter test = 44.0 * W / 4.0_m;
	CHECK(test == 11.0_W_per_m);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor spectral_intensity", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter<double>, decltype(W / sr / m)>);
	static_assert(traits::is_spectral_intensity_unit_v<decltype(W / sr / m)>);

	constexpr watts_per_steradian_per_meter test = 44.0 * W / 2.0_sr / 4.0_m;
	CHECK(test == 5.5_W_per_srm);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor spectral_irradiance", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_meter_cubed<double>, decltype(W / m3)>);
	static_assert(traits::is_spectral_irradiance_unit_v<decltype(W / m3)>);

	constexpr watts_per_meter_cubed test = 44.0 * W / 2.0_m3;
	CHECK(test == 22_W_per_m3);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor spectral_radiance", "[wpimath][units]")
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_cubed<double>, decltype(W / sr / m3)>);
	static_assert(traits::is_spectral_radiance_unit_v<decltype(W / sr / m3)>);

	constexpr watts_per_steradian_per_meter_cubed test = 44.0 * W / 2.0_sr / 2.0_m3;
	CHECK(test == 11_W_per_srm3);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor pi", "[wpimath][units]")
{
	CHECK(wpi::units::traits::is_dimensionless_unit_v<decltype(constants::pi)>);
	CHECK(wpi::units::traits::is_dimensionless_unit_v<detail::PI>);

	// implicit conversion/arithmetic
	CHECK_THAT(3.14159, Catch::Matchers::WithinAbs(constants::pi, 5.0e-6));
	CHECK_THAT(6.28318531, Catch::Matchers::WithinAbs((2 * constants::pi), 5.0e-9));
	CHECK_THAT(6.28318531, Catch::Matchers::WithinAbs((constants::pi + constants::pi), 5.0e-9));
	CHECK_THAT(0.0, Catch::Matchers::WithinAbs((constants::pi - constants::pi), 5.0e-9));
	CHECK_THAT(31.00627668, Catch::Matchers::WithinAbs(pow<3>(constants::pi), 5.0e-10));
	CHECK_THAT(0.0322515344, Catch::Matchers::WithinAbs((1.0 / pow<3>(constants::pi)), 5.0e-11));
	CHECK(detail::PI_VAL == constants::pi);
	CHECK(1.0 != constants::pi);
	CHECK(4.0 > constants::pi);
	CHECK(3.0 < constants::pi);
	CHECK(constants::pi > 3.0);
	CHECK(constants::pi < 4.0);

	// explicit conversion
	CHECK_THAT(3.14159, Catch::Matchers::WithinAbs(constants::pi.to<double>(), 5.0e-6));

	// auto multiplication
	static_assert(std::is_same_v<meters<double>, decltype(constants::pi * meters<double>(1))>);
	static_assert(std::is_same_v<meters<double>, decltype(meters<double>(1) * constants::pi)>);

	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs((constants::pi * meters<double>(1)).to<double>(), 5.0e-10));
	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs((meters<double>(1) * constants::pi).to<double>(), 5.0e-10));

	// explicit multiplication
	meters<double> a_m = pi * meters(1);
	meters<double> b_m = meters(1) * pi;

	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs(a_m.to<double>(), 5.0e-10));
	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs(b_m.to<double>(), 5.0e-10));

	// auto division
	static_assert(std::is_same_v<hertz<double>, decltype(constants::pi / seconds<double>(1))>);
	static_assert(std::is_same_v<seconds<double>, decltype(seconds<double>(1) / constants::pi)>);

	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs((constants::pi / seconds<double>(1)).to<double>(), 5.0e-10));
	CHECK_THAT(1.0 / detail::PI_VAL, Catch::Matchers::WithinAbs((seconds<double>(1) / constants::pi).to<double>(), 5.0e-10));

	// explicit
	hertz<double>   c_Hz = constants::pi / seconds<double>(1);
	seconds<double> d_s  = seconds<double>(1) / constants::pi;

	CHECK_THAT(detail::PI_VAL, Catch::Matchers::WithinAbs(c_Hz.to<double>(), 5.0e-10));
	CHECK_THAT(1.0 / detail::PI_VAL, Catch::Matchers::WithinAbs(d_s.to<double>(), 5.0e-10));
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor constants", "[wpimath][units]")
{
	// Source: https://physics.nist.gov/cuu/Constants/index.html
	CHECK(3.141592653589793 == constants::pi);
	CHECK(299792458.0 == constants::c.value());
	CHECK(6.67430e-11 == constants::G.value());
	CHECK(6.62607015e-34 == constants::h.value());
	CHECK(1.054571817e-34 == constants::h_bar.value());
	CHECK(1.25663706212e-6 == constants::mu0.value());
	CHECK(8.8541878128e-12 == constants::epsilon0.value());
	CHECK(376.730313668 == constants::Z0.value());
	CHECK(8.9875517923e9 == constants::k_e.value());
	CHECK(1.602176634e-19 == constants::e.value());
	CHECK(9.1093837015e-31 == constants::m_e.value());
	CHECK(1.67262192369e-27 == constants::m_p.value());
	CHECK(9.2740100783e-24 == constants::mu_B.value());
	CHECK(6.02214076e23 == constants::N_A.value());
	CHECK(8.314462618 == constants::R.value());
	CHECK(1.380649e-23 == constants::k_B.value());
	CHECK(96485.33212 == constants::F.value());
	CHECK(5.670374419e-8 == constants::sigma.value());
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor std_chrono", "[wpimath][units]")
{
	nanoseconds a_ns(std::chrono::nanoseconds(10));
	CHECK(nanoseconds(10) == a_ns);
	microseconds b_us(std::chrono::microseconds(10));

	CHECK(microseconds(10) == b_us);
	milliseconds c_ms = std::chrono::milliseconds(10);
	CHECK(milliseconds(10) == c_ms);
	seconds d_s = std::chrono::seconds(1);
	CHECK(seconds(1) == d_s);
	minutes e_min = std::chrono::minutes(120);
	CHECK(minutes(120) == e_min);
	hours f_hr = std::chrono::hours(2);
	CHECK(hours(2) == f_hr);

	std::chrono::nanoseconds g_ns = nanoseconds<int>(100);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(g_ns).count() == 100);
	std::chrono::nanoseconds h_ns = microseconds<int>(2);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(h_ns).count() == 2000);
	std::chrono::nanoseconds i_ns = milliseconds<int>(1);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(i_ns).count() == 1000000);
	std::chrono::nanoseconds j_ns = seconds<int>(1);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(j_ns).count() == 1000000000);
	std::chrono::nanoseconds k_ns = minutes<int>(1);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(k_ns).count() == 60000000000);
	std::chrono::nanoseconds l_ns = hours<int>(1);
	CHECK(std::chrono::duration_cast<std::chrono::nanoseconds>(l_ns).count() == 3600000000000);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor squaredTemperature", "[wpimath][units]")
{
	using squared_celsius   = compound_conversion_factor<squared<celsius<double>>>;
	using squared_celsius_t = unit<squared_celsius>;
	constexpr squared_celsius_t right(100);
	constexpr celsius           rootRight = sqrt(right);
	CHECK(celsius<double>(10) == rootRight);
}

TEST_CASE_METHOD(ConversionFactor, "ConversionFactor unitsAddedIn3_4_2", "[wpimath][units]")
{
	// Each new unit is checked against the exact ratio to its canonical parent.
	// length
	CHECK(4.0 == rods<double>(chains<double>(1.0)).value());
	CHECK(100.0 == links<double>(chains<double>(1.0)).value());
	CHECK(1.0 == inches<double>(barleycorns<double>(3.0)).value());
	CHECK(1.0 == yards<double>(nails<double>(16.0)).value());
	CHECK(9.0 == inches<double>(spans<double>(1.0)).value());
	CHECK(1.0 == inches<double>(picas<double>(6.0)).value());
	CHECK(1.0 == inches<double>(points<double>(72.0)).value());
	// velocity
	CHECK(1.0 == feet_per_second<double>(feet_per_minute<double>(60.0)).value());
	CHECK(1000.0 == meters_per_second<double>(kilometers_per_second<double>(1.0)).value());
	CHECK(0.0254 == meters_per_second<double>(inches_per_second<double>(1.0)).value());
	CHECK(1.0 == feet_per_second<double>(meters_per_minute<double>(18.288)).value());
	// area
	CHECK(1.0 == acres<double>(roods<double>(4.0)).value());
	CHECK(1.0 == acres<double>(square_rods<double>(160.0)).value());
	// angle
	CHECK(1.0 == turns<double>(angular_mils<double>(6400.0)).value());
	CHECK(1.0 == turns<double>(compass_points<double>(32.0)).value());
	// time
	CHECK(14.0 == days<double>(fortnights<double>(1.0)).value());
	CHECK(10.0 == julian_years<double>(decades<double>(1.0)).value());
	CHECK(100.0 == julian_years<double>(centuries<double>(1.0)).value());
	CHECK(1000.0 == julian_years<double>(millennia<double>(1.0)).value());
	// data
	CHECK(1.0 == bytes<double>(nibbles<double>(2.0)).value());
	// radiation
	CHECK(0.01 == sieverts<double>(roentgens_equivalent_man<double>(1.0)).value());
	// substance
	CHECK(453.59237 == mols<double>(pound_moles<double>(1.0)).value());
	// mass
	CHECK(1.0 == mass::pounds<double>(grains<double>(7000.0)).value());
	CHECK(1.0 == mass::ounces<double>(avoirdupois_drams<double>(16.0)).value());
	CHECK(480.0 == grains<double>(troy_ounces<double>(1.0)).value());
	CHECK(12.0 == troy_ounces<double>(troy_pounds<double>(1.0)).value());
	CHECK(24.0 == grains<double>(pennyweights<double>(1.0)).value());
	CHECK(112.0 == mass::pounds<double>(hundredweights<double>(1.0)).value());
	CHECK(100.0 == mass::pounds<double>(short_hundredweights<double>(1.0)).value());
	// force
	CHECK(1000.0 == force::pounds<double>(kips<double>(1.0)).value());
	CHECK(1.0 == force::pounds<double>(ounces_force<double>(16.0)).value());
	CHECK(0.00980665 == newtons<double>(grams_force<double>(1.0)).value());
	CHECK(2000.0 == force::pounds<double>(short_tons_force<double>(1.0)).value());
	CHECK(2240.0 == force::pounds<double>(long_tons_force<double>(1.0)).value());
	CHECK(1000.0 == newtons<double>(sthenes<double>(1.0)).value());
	// pressure
	CHECK(98066.5 == pascals<double>(technical_atmospheres<double>(1.0)).value());
	CHECK(1000.0 == pounds_per_square_inch<double>(kips_per_square_inch<double>(1.0)).value());
	CHECK(0.1 == pascals<double>(baryes<double>(1.0)).value());
	CHECK(1000.0 == pascals<double>(piezes<double>(1.0)).value());
	CHECK(98.0665 == pascals<double>(centimeters_of_water<double>(1.0)).value());
	CHECK(9.80665 == pascals<double>(millimeters_of_water<double>(1.0)).value());
	CHECK_THAT(1.0, Catch::Matchers::WithinAbs(pounds_per_square_inch<double>(pounds_per_square_foot<double>(144.0)).value(), 1e-9));
	// energy
	CHECK(1.0e-7 == joules<double>(ergs<double>(1.0)).value());
	CHECK(4.1868 == joules<double>(calories_it<double>(1.0)).value());
	CHECK(4.184e9 == joules<double>(tons_of_tnt<double>(1.0)).value());
	// power
	CHECK(735.49875 == watts<double>(metric_horsepower<double>(1.0)).value());
	CHECK(746.0 == watts<double>(electrical_horsepower<double>(1.0)).value());
	// charge / current
	CHECK(10.0 == coulombs<double>(abcoulombs<double>(1.0)).value());
	CHECK(10.0 == amperes<double>(abamperes<double>(1.0)).value());
}

TEST_CASE_METHOD(UnitMath, "UnitMath min", "[wpimath][units]")
{
	meters a_m(1.0);
	feet   c_ft(1.0);
	CHECK(c_ft == wpi::units::min(a_m, c_ft));

	constexpr meters      d_m(1);
	constexpr centimeters e_cm(99);
	CHECK(e_cm == wpi::units::min(d_m, e_cm));
}

TEST_CASE_METHOD(UnitMath, "UnitMath max", "[wpimath][units]")
{
	meters<double> a_m(1);
	feet<double>   c_ft(1);
	CHECK(a_m == max(a_m, c_ft));

	meters<int>      d_m(1);
	centimeters<int> e_cm(101);
	CHECK(e_cm == max(d_m, e_cm));
}

TEST_CASE_METHOD(UnitMath, "UnitMath ternaryOperator", "[wpimath][units]")
{
	degrees val1 = 10_deg;
	degrees val2 = 90_deg;

	bool value   = true;
	auto new_val = value ? val1 - val2 : val2;
	CHECK(new_val == -80_deg);

	value   = false;
	new_val = value ? val1 - val2 : val2;
	CHECK(new_val == 90_deg);
}

TEST_CASE_METHOD(UnitMath, "UnitMath cos", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(cos(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(cos(degrees<int>(0)))>);
	CHECK_THAT(dimensionless<double>(-0.41614683654), Catch::Matchers::WithinAbs(cos(angle::radians<double>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-0.41614683654), Catch::Matchers::WithinAbs(cos(radians<int>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-0.70710678118), Catch::Matchers::WithinAbs(cos(angle::degrees<double>(135)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-0.70710678118), Catch::Matchers::WithinAbs(cos(degrees<int>(135)), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath sin", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(sin(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(sin(degrees<int>(0)))>);
	CHECK_THAT(dimensionless<double>(0.90929742682), Catch::Matchers::WithinAbs(sin(angle::radians<double>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(0.90929742682), Catch::Matchers::WithinAbs(sin(radians<int>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(0.70710678118), Catch::Matchers::WithinAbs(sin(angle::degrees<double>(135)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(0.70710678118), Catch::Matchers::WithinAbs(sin(degrees<int>(135)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(0), Catch::Matchers::WithinAbs(sin(1.0_rad * wpi::units::constants::pi), 5.0e-16));
}

TEST_CASE_METHOD(UnitMath, "UnitMath tan", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(tan(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(tan(degrees<int>(0)))>);
	CHECK_THAT(dimensionless<double>(-2.18503986326), Catch::Matchers::WithinAbs(tan(angle::radians<double>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-2.18503986326), Catch::Matchers::WithinAbs(tan(radians<int>(2)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-1.0), Catch::Matchers::WithinAbs(tan(angle::degrees<double>(135)), 5.0e-11));
	CHECK_THAT(dimensionless<double>(-1.0), Catch::Matchers::WithinAbs(tan(degrees<int>(135)), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath acos", "[wpimath][units]")
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(acos(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(acos(dimensionless<int>(0)))>);
	auto input1  = -0.41614683654;
	auto input2  = 0;
	auto input3  = -0.70710678118654752440084436210485;
	auto input4  = 0;
	auto out1 = 2;
	auto out2 = 1.570796326795;
	auto out3 = 135;
	auto out4 = 90;
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(acos(dimensionless<double>(input1)).to<double>(), 5.0e-11));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(acos(dimensionless<int>(input2)).to<double>(), 5.0e-11));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(acos(dimensionless<double>(input3))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(acos(dimensionless<int>(input4))).to<double>(), 5.0e-12));
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(acos(uin1).to<double>(), 5.0e-11));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(acos(uin2).to<double>(), 5.0e-11));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(acos(uin3)).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(acos(uin4)).to<double>(), 5.0e-12));
}

TEST_CASE_METHOD(UnitMath, "UnitMath asin", "[wpimath][units]")
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<int>(0)))>);
	auto input1  = 0.90929742682;
	auto input2  = 1;
	auto input3  = 0.70710678118654752440084436210485;
	auto input4  = 1;
	auto out1 = 1.14159265;
	auto out2 = 1.570796326795;
	auto out3 = 45;
	auto out4 = 90;
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(asin(dimensionless<double>(input1)).to<double>(), 5.0e-9));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(asin(dimensionless<int>(input2)).to<double>(), 5.0e-9));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(asin(dimensionless<double>(input3))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(asin(dimensionless<int>(input4))).to<double>(), 5.0e-12));
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(asin(uin1).to<double>(), 5.0e-9));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(asin(uin2).to<double>(), 5.0e-9));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(asin(uin3)).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(asin(uin4)).to<double>(), 5.0e-12));
}

// Regression: the inverse trig / hyperbolic family must promote a FRACTIONAL integer-underlying
// dimensionless argument to floating point before the cmath call. A scaled dimensionless such as
// percent<int>(50) has value 0.5 but an integer underlying of 50; converting to the raw underlying
// truncated 0.5 -> 0, so asin/acos/atan (and the inverse hyperbolics) returned the wrong result for any
// non-whole ratio. They must match std::* on the promoted value, exactly as the forward trig already does.
TEST_CASE_METHOD(UnitMath, "UnitMath inverseTrigPromotesFractionalIntegerUnderlying", "[wpimath][units]")
{
	const percent<int> half(50);    // value() == 0.5, underlying int == 50
	CHECK_THAT(std::asin(0.5), Catch::Matchers::WithinAbs(asin(half).to<double>(), 5.0e-12));
	CHECK_THAT(std::acos(0.5), Catch::Matchers::WithinAbs(acos(half).to<double>(), 5.0e-12));
	CHECK_THAT(std::atan(0.5), Catch::Matchers::WithinAbs(atan(half).to<double>(), 5.0e-12));
	CHECK_THAT(std::asinh(0.5), Catch::Matchers::WithinAbs(asinh(half).to<double>(), 5.0e-12));
	CHECK_THAT(std::atanh(0.5), Catch::Matchers::WithinAbs(atanh(half).to<double>(), 5.0e-12));
	// acosh needs an argument >= 1; use 150% = 1.5.
	const percent<int> onePointFive(150);
	CHECK_THAT(std::acosh(1.5), Catch::Matchers::WithinAbs(acosh(onePointFive).to<double>(), 5.0e-12));
}

TEST_CASE_METHOD(UnitMath, "UnitMath atan", "[wpimath][units]")
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<int>(0)))>);
	auto input1  = -2.18503986326;
	auto input2  = 1;
	auto input3  = -1;
	auto input4  = 1;
	auto out1 = -1.14159265;
	auto out2 = 0.785398163397;
	auto out3 = -45;
	auto out4 = 45;
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(atan(dimensionless<double>(input1)).to<double>(), 5.0e-9));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(atan(dimensionless<int>(input2)).to<double>(), 5.0e-9));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan(dimensionless<double>(input3))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan(dimensionless<int>(input4))).to<double>(), 5.0e-12));
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
	CHECK_THAT(angle::radians<double>(out1).to<double>(), Catch::Matchers::WithinAbs(atan(uin1).to<double>(), 5.0e-9));
	CHECK_THAT(angle::radians<double>(out2).to<double>(), Catch::Matchers::WithinAbs(atan(uin2).to<double>(), 5.0e-9));
	CHECK_THAT(angle::degrees<double>(out3).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan(uin3)).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(out4).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan(uin4)).to<double>(), 5.0e-12));
}

TEST_CASE_METHOD(UnitMath, "UnitMath atan2", "[wpimath][units]")
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<int>(1), dimensionless<int>(1)))>);
	CHECK_THAT(angle::radians<double>(detail::PI_VAL / 4).to<double>(), Catch::Matchers::WithinAbs(atan2(dimensionless<double>(2), dimensionless<double>(2)).to<double>(), 5.0e-12));
	CHECK_THAT(angle::radians<double>(detail::PI_VAL / 4).to<double>(), Catch::Matchers::WithinAbs(atan2(dimensionless<int>(2), dimensionless<int>(2)).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(45).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan2(dimensionless<double>(2), dimensionless<double>(2))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(45).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan2(dimensionless<int>(2), dimensionless<int>(2))).to<double>(), 5.0e-12));

	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>);
	CHECK_THAT(angle::radians<double>(detail::PI_VAL / 6).to<double>(), Catch::Matchers::WithinAbs(atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::radians<double>(detail::PI_VAL / 6).to<double>(), Catch::Matchers::WithinAbs(atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(30).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3)))).to<double>(), 5.0e-12));
	CHECK_THAT(angle::degrees<double>(30).to<double>(), Catch::Matchers::WithinAbs(angle::degrees<double>(atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3)))).to<double>(), 5.0e-12));
}

// Hyperbolic functions operate on a dimensionless real (a hyperbolic angle), not a geometric angle: they
// take a dimensionless argument with no radian conversion, and the inverse functions return dimensionless.
TEST_CASE_METHOD(UnitMath, "UnitMath cosh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(dimensionless<int>(0)))>);
	CHECK_THAT(std::cosh(2.0), Catch::Matchers::WithinAbs(cosh(dimensionless<double>(2.0)).to<double>(), 5.0e-11));
	CHECK_THAT(std::cosh(2.0), Catch::Matchers::WithinAbs(cosh(dimensionless<int>(2)).to<double>(), 5.0e-11));
	// a ratio-dimensionless argument uses its normalized value (50% -> 0.5)
	CHECK_THAT(std::cosh(0.5), Catch::Matchers::WithinAbs(cosh(percent<double>(50)).to<double>(), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath sinh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(dimensionless<int>(0)))>);
	CHECK_THAT(std::sinh(2.0), Catch::Matchers::WithinAbs(sinh(dimensionless<double>(2.0)).to<double>(), 5.0e-11));
	CHECK_THAT(std::sinh(2.0), Catch::Matchers::WithinAbs(sinh(dimensionless<int>(2)).to<double>(), 5.0e-11));
	CHECK_THAT(std::sinh(0.5), Catch::Matchers::WithinAbs(sinh(percent<double>(50)).to<double>(), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath tanh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(dimensionless<int>(0)))>);
	CHECK_THAT(std::tanh(2.0), Catch::Matchers::WithinAbs(tanh(dimensionless<double>(2.0)).to<double>(), 5.0e-11));
	CHECK_THAT(std::tanh(2.0), Catch::Matchers::WithinAbs(tanh(dimensionless<int>(2)).to<double>(), 5.0e-11));
	CHECK_THAT(std::tanh(0.5), Catch::Matchers::WithinAbs(tanh(percent<double>(50)).to<double>(), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath acosh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(acosh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(acosh(dimensionless<int>(0)))>);
	CHECK_THAT(std::acosh(2.0), Catch::Matchers::WithinAbs(acosh(dimensionless<double>(2.0)).to<double>(), 5.0e-11));
	CHECK_THAT(std::acosh(2.0), Catch::Matchers::WithinAbs(acosh(dimensionless<int>(2)).to<double>(), 5.0e-11));
	auto uins = 2.0 * 1.0_m * (1.0 / (1000.0_mm));   // a dimensionless expression
	CHECK_THAT(std::acosh(2.0), Catch::Matchers::WithinAbs(acosh(uins).to<double>(), 5.0e-11));
}

TEST_CASE_METHOD(UnitMath, "UnitMath asinh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(asinh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(asinh(dimensionless<int>(0)))>);
	CHECK_THAT(std::asinh(2.0), Catch::Matchers::WithinAbs(asinh(dimensionless<double>(2.0)).to<double>(), 5.0e-9));
	CHECK_THAT(std::asinh(2.0), Catch::Matchers::WithinAbs(asinh(dimensionless<int>(2)).to<double>(), 5.0e-9));
	auto uins = 2.0 * 1.0_m * (1.0 / (1000.0_mm));
	CHECK_THAT(std::asinh(2.0), Catch::Matchers::WithinAbs(asinh(uins).to<double>(), 5.0e-9));
}

TEST_CASE_METHOD(UnitMath, "UnitMath atanh", "[wpimath][units]")
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(atanh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(atanh(dimensionless<int>(0)))>);
	CHECK_THAT(std::atanh(0.5), Catch::Matchers::WithinAbs(atanh(dimensionless<double>(0.5)).to<double>(), 5.0e-9));
	CHECK_THAT(std::atanh(0.0), Catch::Matchers::WithinAbs(atanh(dimensionless<int>(0)).to<double>(), 5.0e-9));
	auto uins = 0.5 * 1.0_m * (1.0 / (1000.0_mm));
	CHECK_THAT(std::atanh(0.5), Catch::Matchers::WithinAbs(atanh(uins).to<double>(), 5.0e-9));
}

TEST_CASE_METHOD(UnitMath, "UnitMath exp", "[wpimath][units]")
{
	double val = 10.0;
	CHECK(std::exp(val) == exp(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(static_cast<double>(uval) == static_cast<typename decltype(uval)::underlying_type>(uval));
	CHECK(std::exp(uval.to<double>()) == wpi::units::exp(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log", "[wpimath][units]")
{
	double val = 100.0;
	CHECK(std::log(val) == log(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::log(uval.to<double>()) == wpi::units::log(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log10", "[wpimath][units]")
{
	double val = 100.0;
	CHECK(std::log10(val) == log10(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::log10(uval.to<double>()) == wpi::units::log10(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath modf", "[wpimath][units]")
{
	double                val = 100.0;
	double                modfr1;
	dimensionless<double> modfr2;
	CHECK(std::modf(val, &modfr1) == modf(dimensionless<double>(val), &modfr2));
	CHECK(modfr1 == modfr2);
	auto           uval = 5.0_m * (2.0 / 1000.0_mm);
	double         umodfr1;
	decltype(uval) umodfr2;
	CHECK(std::modf(uval.to<double>(), &umodfr1) == wpi::units::modf(uval, &umodfr2));

	// A scaled dimensionless unit (percent) must not have its scale applied twice: modf(202.5%) is an
	// integral 200% and a fractional 2.5%, i.e. value() 2.0 and 0.025 (regression for issue #312). The
	// fractional part carries only the rounding of std::modf itself, so it is compared with a tolerance.
	percent<double> pintpart;
	auto            pfracpart = modf(percent<double>(202.5), &pintpart);
	CHECK(2.0 == pintpart.value());
	CHECK_THAT(0.025, Catch::Matchers::WithinAbs(pfracpart.value(), 1e-12));
	// sign is carried on both parts
	auto npfracpart = modf(percent<double>(-202.5), &pintpart);
	CHECK(-2.0 == pintpart.value());
	CHECK_THAT(-0.025, Catch::Matchers::WithinAbs(npfracpart.value(), 1e-12));
}

TEST_CASE_METHOD(UnitMath, "UnitMath exp2", "[wpimath][units]")
{
	double val = 10.0;
	CHECK(std::exp2(val) == exp2(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::exp2(uval.to<double>()) == wpi::units::exp2(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath expm1", "[wpimath][units]")
{
	double val = 10.0;
	CHECK(std::expm1(val) == expm1(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::expm1(uval.to<double>()) == wpi::units::expm1(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log1p", "[wpimath][units]")
{
	double val = 10.0;
	CHECK(std::log1p(val) == log1p(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::log1p(uval.to<double>()) == wpi::units::log1p(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath log2", "[wpimath][units]")
{
	double val = 10.0;
	CHECK(std::log2(val) == log2(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	CHECK(std::log2(uval.to<double>()) == wpi::units::log2(uval));
}

TEST_CASE_METHOD(UnitMath, "UnitMath pow", "[wpimath][units]")
{
	constexpr meters value(10.0);

	auto inv_sq = pow<-2>(value);
	CHECK_THAT(0.01, Catch::Matchers::WithinAbs(inv_sq.value(), 5.0e-5));
	static_assert(std::is_same_v<decltype(inv_sq), unit<inverse<square_meters<>>>>);

	auto inv = pow<-1>(value);
	CHECK_THAT(0.1, Catch::Matchers::WithinAbs(inv.value(), 5.0e-4));
	static_assert(std::is_same_v<decltype(inv), unit<inverse<meters<>>>>);

	auto scalar = pow<0>(value);
	CHECK_THAT(1, Catch::Matchers::WithinAbs(scalar.value(), 5.0e-3));
	static_assert(std::is_same_v<decltype(scalar), dimensionless<>>);

	auto sq = pow<2>(value);
	CHECK_THAT(100.0, Catch::Matchers::WithinAbs(sq.value(), 5.0e-2));
	static_assert(std::is_same_v<decltype(sq), square_meters<double>>);

	auto cube = pow<3>(value);
	CHECK_THAT(1000.0, Catch::Matchers::WithinAbs(cube.value(), 5.0e-2));
	// Named-result parity with pow<2> -> square_meters above: pow<3> of a length reports the named volume unit
	// (cubic_meters<double>), the friendly type, rather than the equivalent-but-unnamed unit<strong_t<cubed<...>>>.
	static_assert(std::is_same_v<decltype(cube), cubic_meters<double>>);

	auto fourth = pow<4>(value);
	CHECK_THAT(10000.0, Catch::Matchers::WithinAbs(fourth.value(), 5.0e-2));
	static_assert(std::is_same_v<decltype(fourth), unit<compound_conversion_factor<squared<meters<double>>, squared<meters<double>>>>>);
}

TEST_CASE_METHOD(UnitMath, "UnitMath sqrt", "[wpimath][units]")
{
	static_assert(std::is_same_v<meters<double>, decltype(sqrt(square_meters<double>(4.0)))>);
	CHECK_THAT(meters<double>(2.0).to<double>(), Catch::Matchers::WithinAbs(sqrt(square_meters<double>(4.0)).to<double>(), 5.0e-9));

	static_assert(std::is_same_v<angle::radians<double>, decltype(sqrt(steradians<double>(16.0)))>);
	CHECK_THAT(angle::radians<double>(4.0).to<double>(), Catch::Matchers::WithinAbs(sqrt(steradians<double>(16.0)).to<double>(), 5.0e-9));

	static_assert(std::is_convertible_v<feet<double>, decltype(sqrt(square_feet<double>(10.0)))>);

	// for rational conversion (i.e. no integral root) let's check a bunch of different ways this could go wrong
	feet<double> resultFt = sqrt(square_feet<double>(10.0));
	CHECK_THAT(feet<double>(3.16227766017).to<double>(), Catch::Matchers::WithinAbs(sqrt(square_feet<double>(10.0)).to<double>(), 5.0e-9));
	CHECK_THAT(feet<double>(3.16227766017).to<double>(), Catch::Matchers::WithinAbs(resultFt.to<double>(), 5.0e-9));
	CHECK(resultFt == sqrt(square_feet<double>(10.0)));

	percent resultPct = sqrt(16.0_pct);
	CHECK(resultPct == 40.0_pct);
	CHECK(0.4 == resultPct);
}

TEST_CASE_METHOD(UnitMath, "UnitMath hypot", "[wpimath][units]")
{
	static_assert(std::is_same_v<meters<double>, decltype(hypot(meters<double>(3.0), meters<double>(4.0)))>);
	CHECK_THAT(meters<double>(5.0).to<double>(), Catch::Matchers::WithinAbs((hypot(meters<double>(3.0), meters<double>(4.0))).to<double>(), 5.0e-9));

	static_assert(traits::is_same_dimension_unit_v<feet<double>, decltype(hypot(feet<double>(3.0), meters<double>(1.2192)))>);
	CHECK_THAT(feet<double>(5.0).to<double>(), Catch::Matchers::WithinAbs(feet<double>(hypot(feet<double>(3.0), meters<double>(1.2192))).to<double>(), 5.0e-9));
}

TEST_CASE_METHOD(UnitMath, "UnitMath ceil", "[wpimath][units]")
{
	double val = 101.1;
	CHECK(ceil(val) == ceil(meters<double>(val)).to<double>());
	static_assert(std::is_same_v<meters<double>, decltype(ceil(meters<double>(val)))>);

	// ceil must match std::ceil across the WHOLE domain, including non-finite and magnitudes beyond the
	// 2^63 range of a signed integer. A prior hand-rolled `static_cast<long long>` path returned garbage
	// (~-9.22e18) for NaN, +/-Inf, and |x| >= 2^63; delegating to std::ceil fixes it.
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	CHECK(std::isnan(ceil(meters<double>(nan)).to<double>()));
	CHECK(inf == ceil(meters<double>(inf)).to<double>());
	CHECK(-inf == ceil(meters<double>(-inf)).to<double>());
	CHECK(std::ceil(1e19) == ceil(meters<double>(1e19)).to<double>());
	CHECK(std::ceil(-1e19) == ceil(meters<double>(-1e19)).to<double>());
	// ordinary positive/negative rounding stays correct.
	CHECK(3.0 == ceil(meters<double>(2.3)).to<double>());
	CHECK(-2.0 == ceil(meters<double>(-2.3)).to<double>());
}

TEST_CASE_METHOD(UnitMath, "UnitMath floor", "[wpimath][units]")
{
	double val = 101.1;
	CHECK(floor(val) == floor(dimensionless<double>(val)));

	// floor must match std::floor across the whole domain (see ceil above for the fixed regression).
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	CHECK(std::isnan(floor(meters<double>(nan)).to<double>()));
	CHECK(inf == floor(meters<double>(inf)).to<double>());
	CHECK(-inf == floor(meters<double>(-inf)).to<double>());
	CHECK(std::floor(1e19) == floor(meters<double>(1e19)).to<double>());
	CHECK(std::floor(-1e19) == floor(meters<double>(-1e19)).to<double>());
	CHECK(2.0 == floor(meters<double>(2.3)).to<double>());
	CHECK(-3.0 == floor(meters<double>(-2.3)).to<double>());
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmod", "[wpimath][units]")
{
	CHECK(fmod(100.0, 101.2) == fmod(meters<double>(100.0), meters<double>(101.2)).to<double>());
}

TEST_CASE_METHOD(UnitMath, "UnitMath trunc", "[wpimath][units]")
{
	double val = 101.1;
	CHECK(trunc(val) == trunc(dimensionless<double>(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath round", "[wpimath][units]")
{
	double val = 101.1;
	CHECK(round(val) == round(dimensionless<double>(val)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath copysign", "[wpimath][units]")
{
	double         sign = -1;
	meters<double> val(5.0);
	CHECK(meters<double>(-5.0) == copysign(val, sign));
	CHECK(meters<double>(-5.0) == copysign(val, angle::radians<double>(sign)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fdim", "[wpimath][units]")
{
	CHECK(meters<double>(0.0) == fdim(meters<double>(8.0), meters<double>(10.0)));
	CHECK(meters<double>(2.0) == fdim(meters<double>(10.0), meters<double>(8.0)));
	CHECK_THAT(meters<double>(9.3904).to<double>(), Catch::Matchers::WithinAbs(meters<double>(fdim(meters<double>(10.0), feet<double>(2.0))).to<double>(), 5.0e-320)); // not sure why they aren't comparing exactly equal, but clearly they are.
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmin", "[wpimath][units]")
{
	CHECK(meters<double>(8.0) == fmin(meters<double>(8.0), meters<double>(10.0)));
	CHECK(meters<double>(8.0) == fmin(meters<double>(10.0), meters<double>(8.0)));
	CHECK(feet<double>(2.0) == fmin(meters<double>(10.0), feet<double>(2.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fmax", "[wpimath][units]")
{
	CHECK(meters<double>(10.0) == fmax(meters<double>(8.0), meters<double>(10.0)));
	CHECK(meters<double>(10.0) == fmax(meters<double>(10.0), meters<double>(8.0)));
	CHECK(meters<double>(10.0) == fmax(meters<double>(10.0), feet<double>(2.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fabs", "[wpimath][units]")
{
	CHECK(meters<double>(10.0) == fabs(meters<double>(-10.0)));
	CHECK(meters<double>(10.0) == fabs(meters<double>(10.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath abs", "[wpimath][units]")
{
	CHECK(meters<double>(10.0) == abs(meters<double>(-10.0)));
	CHECK(meters<double>(10.0) == abs(meters<double>(10.0)));
}

TEST_CASE_METHOD(UnitMath, "UnitMath fma", "[wpimath][units]")
{
	meters<double>        x(2.0);
	meters<double>        y(3.0);
	square_meters<double> z(1.0);
	CHECK(square_meters<double>(7.0) == (wpi::units::fma(x, y, z)));

	// Regression for #373: the three operands may be in DIFFERENT units of their dimensions, and each must
	// be reconciled to the result unit before the fused multiply-add. Feeding each operand's own-unit raw
	// value combined inconsistent bases (6 ft * 3 ft + 1 m^2 wrongly gave ~1.0 m^2 instead of 2.672255).
	using wpi::units::literals::operator""_ft;
	using wpi::units::literals::operator""_m2;
	const auto crossUnit = wpi::units::fma(6.0_ft, 3.0_ft, 1.0_m2);
	CHECK_THAT(2.67225472, Catch::Matchers::WithinAbs(square_meters<double>(crossUnit).to<double>(), 1.0e-6));

	// Cross-DIMENSION product: (speed * time) + length, all reconciled to meters.
	const auto crossDim = wpi::units::fma(10.0_mps, 2.0_s, 5.0_m);
	CHECK_THAT(25.0, Catch::Matchers::WithinAbs(meters<double>(crossDim).to<double>(), 1.0e-9));

	// Same-unit exact case stays exact.
	CHECK(square_meters<double>(10.0) == (wpi::units::fma(meters<double>(2.0), meters<double>(3.0), square_meters<double>(4.0))));

	// Integer-underlying operands promote (matching C's usual arithmetic conversions) and stay correct.
	const auto intFma = wpi::units::fma(meters<int>(2), meters<int>(3), square_meters<int>(4));
	CHECK_THAT(10.0, Catch::Matchers::WithinAbs(square_meters<double>(intFma).to<double>(), 1.0e-9));
	static_assert(!std::is_integral_v<typename decltype(intFma)::underlying_type>, "fma result must be floating-point-promoted like C");
}

TEST_CASE_METHOD(UnitMath, "UnitMath isnan", "[wpimath][units]")
{
	meters<> zero(0.0);
	meters<> nan(NAN);
	meters<> inf(INFINITY);

	CHECK(wpi::units::isnan(nan));
	CHECK_FALSE(wpi::units::isnan(inf));
	CHECK_FALSE(wpi::units::isnan(0.0_m));
	CHECK_FALSE(wpi::units::isnan(DBL_MIN / 2.0 * 1_m));
	CHECK(wpi::units::isnan(zero / zero));
	CHECK(wpi::units::isnan(inf - inf));
}

TEST_CASE_METHOD(UnitMath, "UnitMath isinf", "[wpimath][units]")
{
	constexpr meters zero(0.0);
	constexpr meters nan(NAN);
	constexpr meters inf(INFINITY);

	CHECK_FALSE(wpi::units::isnan(zero));
	CHECK_FALSE(wpi::units::isinf(nan));
	CHECK(wpi::units::isinf(inf));
	CHECK_FALSE(wpi::units::isinf(0.0_m));
	CHECK(wpi::units::isinf(exp(1600_rad / 2_rad)));
	CHECK_FALSE(wpi::units::isinf(DBL_MIN / 2.0 * 1_m));
}

TEST_CASE_METHOD(UnitMath, "UnitMath isfinite", "[wpimath][units]")
{
	meters zero(0.0);
	meters nan(NAN);
	meters inf(INFINITY);

	CHECK(wpi::units::isfinite(zero));
	CHECK_FALSE(wpi::units::isfinite(nan));
	CHECK_FALSE(wpi::units::isfinite(inf));
	CHECK(wpi::units::isfinite(0.0_m));
	CHECK_FALSE(wpi::units::isfinite(exp(1600_rad / 2_rad)));
	CHECK(wpi::units::isfinite(DBL_MIN / 2.0 * 1_m));
}

TEST_CASE_METHOD(UnitMath, "UnitMath isnormal", "[wpimath][units]")
{
	meters zero(0.0);
	meters nan(NAN);
	meters inf(INFINITY);

	CHECK_FALSE(wpi::units::isnormal(zero));
	CHECK_FALSE(wpi::units::isnormal(nan));
	CHECK_FALSE(wpi::units::isnormal(inf));
	CHECK_FALSE(wpi::units::isnormal(0.0_m));
	CHECK(wpi::units::isnormal(1.0_m));
}

TEST_CASE_METHOD(UnitMath, "UnitMath isunordered", "[wpimath][units]")
{
	meters zero(0.0);
	meters nan(NAN);

	CHECK(wpi::units::isunordered(nan, zero));
	CHECK(wpi::units::isunordered(zero, nan));
	CHECK_FALSE(wpi::units::isunordered(zero, zero));
}

TEST_CASE_METHOD(UnitMath, "UnitMath signbit", "[wpimath][units]")
{
	meters<> zero(0.0);
	meters<> pos(1.0);
	meters<> neg(-1.0);
	meters<> negZero(-0.0);

	CHECK_FALSE(std::signbit(zero));
	CHECK_FALSE(std::signbit(pos));
	CHECK(std::signbit(neg));
	CHECK(std::signbit(negZero));
}

TEST_CASE_METHOD(UnitMath, "UnitMath stdExtensions", "[wpimath][units]")
{
	meters<> zero(0.0);
	meters<> nan(NAN);
	meters<> inf(INFINITY);

	CHECK(std::isnan(nan));
	CHECK_FALSE(std::isnan(inf));
	CHECK_FALSE(std::isnan(zero));

	CHECK(std::isinf(inf));
	CHECK_FALSE(std::isinf(nan));
	CHECK_FALSE(std::isinf(zero));

	CHECK(std::isfinite(zero));
	CHECK_FALSE(std::isfinite(nan));
	CHECK_FALSE(std::isfinite(inf));
}

// Constexpr
TEST_CASE_METHOD(Constexpr, "Constexpr construction", "[wpimath][units]")
{
	constexpr meters<double> result0(0);
	constexpr auto           result1 = make_unit<meters<double>>(1);
	constexpr auto           result2 = meters<double>(2);

	CHECK(meters<double>(0) == result0);
	CHECK(meters<double>(1) == result1);
	CHECK(meters<double>(2) == result2);

	CHECK(noexcept(meters<double>(0)));
	CHECK(noexcept(make_unit<meters<double>>(1)));
}

TEST_CASE_METHOD(Constexpr, "Constexpr constants", "[wpimath][units]")
{
	CHECK(noexcept(constants::c.value()));
	CHECK(noexcept(constants::G.value()));
	CHECK(noexcept(constants::h.value()));
	CHECK(noexcept(constants::mu0.value()));
	CHECK(noexcept(constants::epsilon0.value()));
	CHECK(noexcept(constants::Z0.value()));
	CHECK(noexcept(constants::k_e.value()));
	CHECK(noexcept(constants::e.value()));
	CHECK(noexcept(constants::m_e.value()));
	CHECK(noexcept(constants::m_p.value()));
	CHECK(noexcept(constants::mu_B.value()));
	CHECK(noexcept(constants::N_A.value()));
	CHECK(noexcept(constants::R.value()));
	CHECK(noexcept(constants::k_B.value()));
	CHECK(noexcept(constants::F.value()));
	CHECK(noexcept(constants::sigma.value()));
}

TEST_CASE_METHOD(Constexpr, "Constexpr arithmetic", "[wpimath][units]")
{
	[[maybe_unused]] constexpr auto result0(1.0_m + 1.0_m);
	[[maybe_unused]] constexpr auto result1(1.0_m - 1.0_m);
	[[maybe_unused]] constexpr auto result2(1.0_m * 1.0_m);
	[[maybe_unused]] constexpr auto result3(1.0_m / 1.0_m);
	[[maybe_unused]] constexpr auto result4(meters<int>(1) + meters<int>(1));
	[[maybe_unused]] constexpr auto result5(meters<int>(1) - meters<int>(1));
	[[maybe_unused]] constexpr auto result6(meters<int>(1) * meters<int>(1));
	[[maybe_unused]] constexpr auto result7(meters<int>(1) / meters<int>(1));
	[[maybe_unused]] constexpr auto result8(pow<2>(meters<int>(2)));
	constexpr auto                  result9  = pow<3>(2.0_m);
	constexpr auto                  result10 = 2.0_m * 2.0_m;

	CHECK(noexcept(1.0_m + 1.0_m));
	CHECK(noexcept(1.0_m - 1.0_m));
	CHECK(noexcept(1.0_m * 1.0_m));
	CHECK(noexcept(1.0_m / 1.0_m));
	CHECK(noexcept(meters<int>(1) + meters<int>(1)));
	CHECK(noexcept(meters<int>(1) - meters<int>(1)));
	CHECK(noexcept(meters<int>(1) * meters<int>(1)));
	CHECK(noexcept(meters<int>(1) / meters<int>(1)));
	CHECK(noexcept(pow<2>(meters<double>(2))));
	CHECK(noexcept(pow<3>(2.0_m)));
	CHECK(noexcept(2.0_m * 2.0_m));

	meters<double> length{42};
	CHECK(noexcept(+length));
	CHECK(noexcept(-length));
	CHECK(noexcept(++length));
	CHECK(noexcept(--length));
	CHECK(noexcept(length++));
	CHECK(noexcept(length--));

	CHECK(8.0_m3 == result9);
	CHECK(4.0_m2 == result10);
}

TEST_CASE_METHOD(Constexpr, "Constexpr assignment", "[wpimath][units]")
{
	auto testConstexpr = []() constexpr noexcept
	{
		meters<double> length{42.};
		+length;
		-length;
		++length;
		--length;
		length++;
		length--;
		length += 2.0_m;
		length -= 2.0_m;
		length *= 2;
		length /= 2;
		return length;
	};

	[[maybe_unused]] constexpr auto length = testConstexpr();
}

TEST_CASE_METHOD(Constexpr, "Constexpr realtional", "[wpimath][units]")
{
	constexpr bool equalityTrue          = (1.0_m == 1.0_m);
	constexpr bool equalityFalse         = (1.0_m == 2.0_m);
	constexpr bool lessThanTrue          = (1.0_m < 2.0_m);
	constexpr bool lessThanFalse         = (1.0_m < 1.0_m);
	constexpr bool lessThanEqualTrue1    = (1.0_m <= 1.0_m);
	constexpr bool lessThanEqualTrue2    = (1.0_m <= 2.0_m);
	constexpr bool lessThanEqualFalse    = (1.0_m < 0.0_m);
	constexpr bool greaterThanTrue       = (2.0_m > 1.0_m);
	constexpr bool greaterThanFalse      = (2.0_m > 2.0_m);
	constexpr bool greaterThanEqualTrue1 = (2.0_m >= 1.0_m);
	constexpr bool greaterThanEqualTrue2 = (2.0_m >= 2.0_m);
	constexpr bool greaterThanEqualFalse = (2.0_m > 3.0_m);

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

TEST_CASE_METHOD(Constexpr, "Constexpr stdArray", "[wpimath][units]")
{
	constexpr std::array<meters<double>, 5> arr{{0.0_m, 1.0_m, 2.0_m, 3.0_m, 4.0_m}};
	constexpr bool                          equal = (arr[3] == 3.0_m);
	CHECK(equal);
}

TEST_CASE("ConcentrationSemantics scalar_multiply_returns_dimensionless", "[wpimath][units]")
{
	auto x = 2 * 50_pct;
	CHECK(x.value() == 1.0);  // dimensionless
}

TEST_CASE("ConcentrationSemantics abs_preserves_percent", "[wpimath][units]")
{
	percent<double> pct1 = 100.0_pct;
	percent<double> pct2 = 70.0_pct;
	auto delta = wpi::units::fabs(pct1 - pct2);
	CHECK(delta == 30.0_pct);
}

TEST_CASE("ConcentrationSemantics PercentPpmPpbConvertToDimensionlessFraction", "[wpimath][units]")
{
	// Percent: 50% == 0.5 dimensionless
	CHECK_THAT(dimensionless(50.0_pct).to<double>(), Catch::Matchers::WithinAbs(0.5, 0.0));

	// ppm: 1 ppm == 1e-6 dimensionless
	CHECK_THAT(dimensionless(1.0_ppm).to<double>(), Catch::Matchers::WithinAbs(1.0e-6, 0.0));

	// ppb: 1 ppb == 1e-9 dimensionless
	CHECK_THAT(dimensionless(1.0_ppb).to<double>(), Catch::Matchers::WithinAbs(1.0e-9, 0.0));

	// chained conversion sanity: 1000 ppb == 1 ppm
	CHECK_THAT(parts_per_million(1000.0_ppb).raw(), Catch::Matchers::WithinAbs(1.0, 0.0));
	CHECK_THAT(parts_per_million(1.0_ppm).raw(), Catch::Matchers::WithinAbs(1.0, 0.0));
}

TEST_CASE("ConcentrationSemantics ScalarTimesPercentYieldsDimensionless", "[wpimath][units]")
{
	auto x = 2 * 50.0_pct;

	// We want this to be a pure dimensionless "1", not 100_pct.
	CHECK_THAT(dimensionless(x).to<double>(), Catch::Matchers::WithinAbs(1.0, 0.0));

	// Also check commutativity
	auto y = 50.0_pct * 2;
	CHECK_THAT(dimensionless(y).to<double>(), Catch::Matchers::WithinAbs(1.0, 0.0));
}

TEST_CASE("ConcentrationSemantics PercentMathPreservesPercentRepresentation", "[wpimath][units]")
{
	auto whole = 100.0_pct;
	auto most  = 70.0_pct;

	// subtraction should preserve the unit: 100% - 70% = 30%
	auto diff = whole - most;
	CHECK_THAT(diff.raw(), Catch::Matchers::WithinAbs(30.0, 0.0));

	// fabs should preserve percent representation: fabs(30%) == 30%
	auto f = wpi::units::fabs(diff);
	CHECK_THAT(f.raw(), Catch::Matchers::WithinAbs(30.0, 0.0));

	// abs should preserve percent representation
	auto magnitude = wpi::units::abs(-30_pct);
	CHECK_THAT(magnitude.raw(), Catch::Matchers::WithinAbs(30.0, 0.0));

	// fmin/fmax should preserve percent representation
	auto mn = wpi::units::fmin(whole, most);
	CHECK_THAT(mn.raw(), Catch::Matchers::WithinAbs(70.0, 0.0));

	auto mx = wpi::units::fmax(whole, most);
	CHECK_THAT(mx.raw(), Catch::Matchers::WithinAbs(100.0, 0.0));

	// fdim should preserve percent representation: fdim(70%, 100%) == 0%
	auto pd = wpi::units::fdim(most, whole);
	CHECK_THAT(pd.raw(), Catch::Matchers::WithinAbs(0.0, 0.0));
}

TEST_CASE("ConcentrationSemantics TranscendentalsUseNormalizedValue", "[wpimath][units]")
{
	// log(50%) = log(0.5)
	auto x = wpi::units::log(50_pct);
	CHECK_THAT(dimensionless(x).to<double>(), Catch::Matchers::WithinAbs(std::log(0.5), 1e-15));

	// exp(0%) = exp(0) = 1
	auto y = wpi::units::exp(0_pct);
	CHECK_THAT(dimensionless(y).to<double>(), Catch::Matchers::WithinAbs(1.0, 1e-15));
}

TEST_CASE("ConcentrationSemantics RatioDimlessPreservedInCompoundDivision", "[wpimath][units]")
{
	using pct_per_m = decltype(1_pct / 1_m);
	using inv_m     = unit<inverse<meters<>>, double>;

	static_assert(!std::is_same_v<pct_per_m, inv_m>);
	static_assert(wpi::units::traits::is_same_dimension_unit_v<pct_per_m, inv_m>);
	static_assert(std::is_convertible_v<pct_per_m, inv_m>);
}

TEST_CASE("ConcentrationSemantics PctPerMeterUsesPointsNumerator", "[wpimath][units]")
{
	auto x = 50_pct / 2_m;           // 25 pct/m in points space
	CHECK(x.raw() == 25.0);

	// When converted to 1/m it should be fraction per meter: 0.25 / m
	unit<inverse<meters<>>, double> y = x;
	CHECK(y.value() == 0.25 / 1.0); // == 0.25 (per m)
}

TEST_CASE("ConcentrationSemantics CommonTypePpmPpb", "[wpimath][units]")
{
	using CT = std::common_type_t<parts_per_million<double>, parts_per_billion<double>>;
	static_assert(wpi::units::traits::is_same_dimension_unit_v<CT, parts_per_million<double>>);

	CT inPpm  = 1.0_ppm;
	CT inPpb  = 1000.0_ppb;
	CHECK(inPpm.raw() == inPpb.raw());
	CHECK(inPpm.value() == inPpb.value());
}

TEST_CASE("ConcentrationSemantics UnitCastUsesNormalizedForRatioDimless", "[wpimath][units]")
{
	CHECK(wpi::units::unit_cast<double>(50_pct) == 0.5);
	CHECK(wpi::units::unit_cast<int>(50_pct) == 0); // yes, surprising, but locks policy
}

TEST_CASE("ConcentrationSemantics DimensionlessDivPercentIsNotSameAsScalarDivPercent", "[wpimath][units]")
{
	auto scalarQuotient = 1.0 / 50_pct;                 // scalar/percent -> dimensionless, uses rhs.value()
	CHECK(scalarQuotient == 2.0);

	double dimensionlessQuotient = dimensionless(1.0) / 50_pct;  // currently -> inverse(percent) style
	// Nail down expected behavior (whatever you decide it should be).
	// If keeping current behavior:
	CHECK(dimensionlessQuotient == 2);     // because 1 / rhs.raw() = 1/50
}


TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitMin", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::min()) == std::numeric_limits<meters<double>>::min());
	CHECK(seconds(std::numeric_limits<double>::min()) == std::numeric_limits<seconds<double>>::min());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitDenormMin", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::denorm_min()) == std::numeric_limits<meters<double>>::denorm_min());
	CHECK(seconds(std::numeric_limits<double>::denorm_min()) == std::numeric_limits<seconds<double>>::denorm_min());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitMax", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::max()) == std::numeric_limits<meters<double>>::max());
	CHECK(seconds(std::numeric_limits<double>::max()) == std::numeric_limits<seconds<double>>::max());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitLowest", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::lowest()) == std::numeric_limits<meters<double>>::lowest());
	CHECK(seconds(std::numeric_limits<double>::lowest()) == std::numeric_limits<seconds<double>>::lowest());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitEpsilon", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::epsilon()) == std::numeric_limits<meters<double>>::epsilon());
	CHECK(seconds(std::numeric_limits<double>::epsilon()) == std::numeric_limits<seconds<double>>::epsilon());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitRoundError", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::round_error()) == std::numeric_limits<meters<double>>::round_error());
	CHECK(seconds(std::numeric_limits<double>::round_error()) == std::numeric_limits<seconds<double>>::round_error());
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitInfinity", "[wpimath][units]")
{
	CHECK(std::numeric_limits<meters<double>>::has_infinity);
	CHECK(std::numeric_limits<meters<double>>::infinity() > std::numeric_limits<meters<double>>::max());
	CHECK_FALSE(std::numeric_limits<seconds<int>>::has_infinity);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitQuietNaN", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::quiet_NaN()) != std::numeric_limits<meters<double>>::quiet_NaN());
	CHECK(seconds(std::numeric_limits<double>::quiet_NaN()) != std::numeric_limits<seconds<double>>::quiet_NaN());
	CHECK(wpi::units::isnan(std::numeric_limits<meters<double>>::quiet_NaN()));
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitSignalingNaN", "[wpimath][units]")
{
	CHECK(meters(std::numeric_limits<double>::signaling_NaN()) != std::numeric_limits<meters<double>>::signaling_NaN());
	CHECK(seconds(std::numeric_limits<double>::signaling_NaN()) != std::numeric_limits<seconds<double>>::signaling_NaN());
	CHECK(wpi::units::isnan(std::numeric_limits<meters<double>>::signaling_NaN()));
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitIsSpecialized", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::is_specialized == std::numeric_limits<meters<double>>::is_specialized);
	CHECK(std::numeric_limits<double>::is_specialized == std::numeric_limits<seconds<double>>::is_specialized);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitIsSigned", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::is_signed == std::numeric_limits<meters<double>>::is_signed);
	CHECK(std::numeric_limits<double>::is_signed == std::numeric_limits<seconds<double>>::is_signed);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitIsInteger", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::is_integer == std::numeric_limits<meters<double>>::is_integer);
	CHECK(std::numeric_limits<double>::is_integer == std::numeric_limits<seconds<double>>::is_integer);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitIsExact", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::is_exact == std::numeric_limits<meters<double>>::is_exact);
	CHECK(std::numeric_limits<double>::is_exact == std::numeric_limits<seconds<double>>::is_exact);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitHasInifinity", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::has_infinity == std::numeric_limits<meters<double>>::has_infinity);
	CHECK(std::numeric_limits<double>::has_infinity == std::numeric_limits<seconds<double>>::has_infinity);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitHasQuietNaN", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::has_quiet_NaN == std::numeric_limits<meters<double>>::has_quiet_NaN);
	CHECK(std::numeric_limits<double>::has_quiet_NaN == std::numeric_limits<seconds<double>>::has_quiet_NaN);
}

TEST_CASE_METHOD(UnitLimits, "UnitLimits UnitHasSignalingNaN", "[wpimath][units]")
{
	CHECK(std::numeric_limits<double>::has_signaling_NaN == std::numeric_limits<meters<double>>::has_signaling_NaN);
	CHECK(std::numeric_limits<double>::has_signaling_NaN == std::numeric_limits<seconds<double>>::has_signaling_NaN);
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies radarRangeEquation", "[wpimath][units]")
{
	watts<>         P_t;    // transmit power
	dimensionless<> gain;   // gain
	meters<>        lambda; // wavelength
	square_meters<> rcs;    // radar cross-section
	meters<>        range;  // range
	kelvin<>        T_s;    // system noise temp
	hertz<>         B_n;    // bandwidth
	dimensionless<> loss;   // loss

	P_t    = megawatts<>(1.4);
	gain   = decibels<>(33.0);
	lambda = constants::c / megahertz<>(2800.0);
	rcs    = square_meters<>(1.0);
	range  = meters<>(111000.0);
	T_s    = kelvin<>(950.0);
	B_n    = megahertz<>(1.67);
	loss   = decibels<>(8.0);

	const dimensionless<double> SNR = (P_t * pow<2>(gain) * pow<2>(lambda) * rcs) / (pow<3>(4 * pi) * pow<4>(range) * k_B * T_s * B_n * loss);

	CHECK_THAT(1.535, Catch::Matchers::WithinAbs(SNR.value(), 5.0e-4));
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies rightTriangle", "[wpimath][units]")
{
	constexpr auto a_m = 3.0_m;
	constexpr auto b_m = 4.0_m;
	constexpr auto c_m = sqrt(pow<2>(a_m) + pow<2>(b_m));
	CHECK(5.0_m == c_m);
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies dataReadSimulation", "[wpimath][units]")
{
	constexpr auto data_size     = 100_MB;
	constexpr auto read_rate     = 2_MBps;
	bytes          read_progress = 10_MB;

	auto advance_simulation = [&](auto time) { read_progress = wpi::units::min(read_progress + time * read_rate, data_size); };

	advance_simulation(10_s);
	CHECK(read_progress == 30_MB);

	advance_simulation(25_s);
	CHECK(read_progress == 80_MB);

	advance_simulation(500_ms);
	CHECK(read_progress == 81_MB);

	advance_simulation(25_s);
	CHECK(read_progress == data_size);
}

TEST_CASE_METHOD(CaseStudies, "CaseStudies selfDefinedUnits", "[wpimath][units]")
{
	// A composed unit the library does not name prints as the raw dimension form (value + dimension
	// exponents), not a friendly abbreviation. Volume per time-squared has no named unit.
	using liters_per_second_squared = decltype(1.0_L / (1.0_s * 1.0_s));

	liters_per_second_squared original(5);
	liters_per_second_squared copy = original;

	CHECK(original.to<double>() == copy.to<double>());
	}

TEST_CASE_METHOD(CaseStudies, "CaseStudies idealGasLaw", "[wpimath][units]")
{
	// PV = nRT, solved for pressure. Temperature is a factor in the product just like any other quantity —
	// a physicist writes the equation directly, with no unwrapping of the affine scale. One mole at 273.15 K
	// in 22.414 L is one standard atmosphere.
	const substance::mols<>       n = substance::mols<>(1.0);
	const temperature::kelvin<>   T = temperature::kelvin<>(273.15);
	const volume::liters<>        V = volume::liters<>(22.414);
	const auto                    R = energy::joules<>(8.314462618) / (substance::mols<>(1.0) * temperature::kelvin<>(1.0));

	const pressure::pascals<double> P = (n * R * T) / V;

	CHECK_THAT(101325.0, Catch::Matchers::WithinAbs(P.value(), 1.0));
}

//======================================================================================================================
//	BACKLOG-CLEANUP additions — viscosity (#205), compound-assign (#257), torque naming (#311)
//======================================================================================================================

namespace
{
	class Viscosity
	{
	};
	class CompoundAssign
	{
	};
	class TorqueNaming
	{
	};
	class CgsBiot
	{
	};
} // namespace

// ---- #205: biot ----------------------------------------------------------------------------------------------------
TEST_CASE_METHOD(CgsBiot, "CgsBiot biotIsAbampere", "[wpimath][units]")
{
	using wpi::units::current::biots;
	// biot is the CGS-EMU name for the abampere: exactly 10 amperes
	CHECK(10.0 == wpi::units::amperes<double>(biots<double>(1.0)).value());
	CHECK(30.0 == wpi::units::amperes<double>(biots<double>(3.0)).value());
	// round-trips through amperes
	CHECK(5.0 == biots<double>(wpi::units::amperes<double>(50.0)).value());
	// same type as abamperes (it is an alias)
	static_assert(std::is_same_v<biots<double>, wpi::units::current::abamperes<double>>);
	// same dimension as current
	static_assert(traits::is_current_unit_v<biots<double>>);
}

// ---- #205: dynamic viscosity (poise) -------------------------------------------------------------------------------
TEST_CASE_METHOD(Viscosity, "Viscosity dynamicViscosityUnitsAndDimension", "[wpimath][units]")
{
	using namespace wpi::units::dynamic_viscosity;
	// poise = 0.1 Pa*s ; centipoise = 0.01 poise = 0.001 Pa*s
	CHECK(0.1 == pascal_seconds<double>(poise<double>(1.0)).value());
	CHECK(1.0 == poise<double>(centipoise<double>(100.0)).value());
	CHECK_THAT(0.001, Catch::Matchers::WithinAbs(pascal_seconds<double>(centipoise<double>(1.0)).value(), 5.0e-15));
	// water is ~1 cP ~ 0.001 Pa*s
	CHECK_THAT(0.001, Catch::Matchers::WithinAbs(pascal_seconds<double>(centipoise<double>(1.0)).value(), 5.0e-15));
	// dimension: pressure * time
	static_assert(traits::is_dynamic_viscosity_unit_v<poise<double>>);
	static_assert(traits::is_dynamic_viscosity_unit_v<pascal_seconds<double>>);
	static_assert(!traits::is_dynamic_viscosity_unit_v<wpi::units::pascals<double>>);
	static_assert(traits::is_same_dimension_unit_v<poise<double>, pascal_seconds<double>>);
	// a pressure times a time IS a dynamic viscosity
	static_assert(traits::is_same_dimension_unit_v<decltype(wpi::units::pascals<double>(1) * wpi::units::seconds<double>(1)), pascal_seconds<double>>);
}

// ---- #205: kinematic viscosity (stokes) ----------------------------------------------------------------------------
TEST_CASE_METHOD(Viscosity, "Viscosity kinematicViscosityUnitsAndDimension", "[wpimath][units]")
{
	using namespace wpi::units::kinematic_viscosity;
	// stokes = 1e-4 m^2/s ; centistokes = 0.01 stokes = 1e-6 m^2/s
	CHECK_THAT(1.0e-4, Catch::Matchers::WithinAbs(square_meters_per_second<double>(stokes<double>(1.0)).value(), 5.0e-16));
	CHECK(1.0 == stokes<double>(centistokes<double>(100.0)).value());
	CHECK_THAT(1.0e-6, Catch::Matchers::WithinAbs(square_meters_per_second<double>(centistokes<double>(1.0)).value(), 5.0e-18));
	// dimension: area / time
	static_assert(traits::is_kinematic_viscosity_unit_v<stokes<double>>);
	static_assert(traits::is_kinematic_viscosity_unit_v<square_meters_per_second<double>>);
	static_assert(!traits::is_kinematic_viscosity_unit_v<wpi::units::square_meters<double>>);
	// dynamic and kinematic viscosity are DISTINCT dimensions
	static_assert(!traits::is_same_dimension_unit_v<stokes<double>, wpi::units::dynamic_viscosity::poise<double>>);
}

// ---- #257: compound assignment keeps the lhs type and value; warns on lossy integer scale -------------------------
TEST_CASE_METHOD(CompoundAssign, "CompoundAssign multiplyKeepsTypeAndValue", "[wpimath][units]")
{
	// double lhs: exact
	wpi::units::meters<double> len(10.0);
	len *= 2.0;
	static_assert(std::is_same_v<decltype(len), wpi::units::meters<double>>);
	CHECK(20.0 == len.value());
	len *= 0.5;
	CHECK(10.0 == len.value());
	// integer lhs, integer factor: exact, no narrowing
	wpi::units::meters<int> ilen(10);
	ilen *= 3;
	static_assert(std::is_same_v<decltype(ilen), wpi::units::meters<int>>);
	CHECK(30 == ilen.value());
	// integer lhs scaled by a floating-point factor narrows and surfaces -Wfloat-conversion; that lossy path's
	// value/type behavior is proven in the dedicated lossyCompoundAssign translation unit (compiled with the
	// float-conversion diagnostic disabled) and the diagnostic itself by test/errorMessages/cases.
}

TEST_CASE_METHOD(CompoundAssign, "CompoundAssign divideKeepsTypeAndValue", "[wpimath][units]")
{
	wpi::units::meters<double> len(10.0);
	len /= 4.0;
	CHECK(2.5 == len.value());
	wpi::units::meters<int> ilen(10);
	ilen /= 2;
	CHECK(5 == ilen.value());
	static_assert(std::is_same_v<decltype(ilen), wpi::units::meters<int>>);
}

TEST_CASE_METHOD(CompoundAssign, "CompoundAssign worksAcrossDimensions", "[wpimath][units]")
{
	// the operators are generic over the (non-ratio-dimensionless) unit; spot-check several dimensions
	wpi::units::seconds<double> dur(60.0);
	dur *= 2.0;
	CHECK(120.0 == dur.value());
	wpi::units::kilograms<double> mass(5.0);
	mass /= 2.0;
	CHECK(2.5 == mass.value());
	wpi::units::newtons<double> f(10.0);
	f *= 3.0;
	CHECK(30.0 == f.value());
	wpi::units::meters_per_second<double> v(26.8224);
	v *= 2.0;
	CHECK_THAT(53.6448, Catch::Matchers::WithinAbs(v.value(), 5.0e-9));
}
/*
// ---- #311: torque pound_feet is the named unit; foot_pounds is a deprecated alias ---------------------------------
TEST_CASE_METHOD(TorqueNaming, "TorqueNaming poundFeetIsTheTorqueUnit", "[wpimath][units]")
{
	using wpi::units::torque::pound_feet;
	// pound_feet == foot * pound-force
	CHECK_THAT(1.3558179483314004, Catch::Matchers::WithinAbs(wpi::units::newton_meters<double>(pound_feet<double>(1.0)).value(), 5.0e-9));
	static_assert(traits::is_torque_unit_v<pound_feet<double>>);
	// torque and energy share the force*length dimension in this library, so the distinction is by NAME, not
	// dimension: pound_feet (torque, lbf*ft) and energy::foot_pounds (energy, ft*lbf) are the same magnitude but
	// carry different abbreviations so a reader can tell which is meant
	static_assert(traits::is_energy_unit_v<wpi::units::energy::foot_pounds<double>>);
	CHECK_THAT(wpi::units::newton_meters<double>(pound_feet<double>(1.0)).value(), Catch::Matchers::WithinAbs(wpi::units::joules<double>(wpi::units::energy::foot_pounds<double>(1.0)).value(), 5.0e-9));
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	// prints with the correct engineering abbreviation
	testing::internal::CaptureStdout();
	std::cout << pound_feet<double>(10.0);
	std::string out = testing::internal::GetCapturedStdout();
	CHECK(std::string_view("10 lbf_ft") == out.c_str());
#endif
}

//======================================================================================================================
//	SERIALIZATION — exhaustive coverage
//======================================================================================================================

namespace
{
	// Round-trips a quantity through a REAL external boundary: serialize -> write the raw bytes to a temp file ->
	// read them back into a fresh buffer (no link to the original any_unit) -> deserialize that buffer. This proves the
	// on-disk byte stream is self-sufficient and decodes correctly; it is deliberately NOT `deserialize(serialize(q))`,
	// which could short-circuit an in-memory any_unit and never exercise the wire encode/decode.
	template<class Q>
	void expectRoundTrip(Q quantity)
	{
		const wpi::units::any_unit encoded = wpi::units::serialize(quantity);

		// write the bytes out through the C-interface face (data()/size()) exactly as a caller would to a file
		static std::atomic<unsigned> counter{0};
		const std::filesystem::path  path =
			std::filesystem::temp_directory_path() / ("units_roundtrip_" + std::to_string(counter.fetch_add(1)) + ".bin");
		{
			std::ofstream out(path, std::ios::binary);
			REQUIRE(out.is_open());
			out.write(encoded.data(), static_cast<std::streamsize>(encoded.size()));
		}

		// read the raw bytes back into a fresh buffer that has NO connection to `encoded`
		std::vector<std::byte> fromDisk;
		{
			std::ifstream in(path, std::ios::binary);
			REQUIRE(in.is_open());
			in.seekg(0, std::ios::end);
			const std::streamoff length = in.tellg();
			in.seekg(0, std::ios::beg);
			fromDisk.resize(static_cast<std::size_t>(length));
			in.read(reinterpret_cast<char*>(fromDisk.data()), length);
		}
		std::filesystem::remove(path);

		// the bytes on disk match what the any_unit reported it wrote
		REQUIRE(encoded.size() == fromDisk.size());

		// decode the disk buffer with no prior knowledge of the type
		const auto erased = wpi::units::deserialize(fromDisk);
		REQUIRE(erased.has_value());
		const auto back = erased->template to<Q>();
		REQUIRE(back.has_value());
		const double a = quantity.template to<double>();
		const double b = back->template to<double>();
		if (std::isnan(a))
			CHECK(std::isnan(b));
		else
			CHECK(a == b);
		// the typed fast path decodes the same disk buffer to the same value
		const auto direct = wpi::units::deserialize<Q>(fromDisk);
		REQUIRE(direct.has_value());
		if (std::isnan(a))
			CHECK(std::isnan(direct->template to<double>()));
		else
			CHECK(a == direct->template to<double>());
	}
} // namespace

TEST_CASE_METHOD(Serialization, "Serialization roundTripLength", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::meters<double>(100.0));
	expectRoundTrip(wpi::units::feet<double>(3.5));
	expectRoundTrip(wpi::units::miles<double>(2.0));
	expectRoundTrip(wpi::units::kilometers<double>(2.5));
	expectRoundTrip(wpi::units::nanometers<double>(500.0));
	expectRoundTrip(wpi::units::furlongs<double>(10.0));
	expectRoundTrip(wpi::units::nautical_miles<double>(1.0));
	expectRoundTrip(wpi::units::rods<double>(4.0));
	expectRoundTrip(wpi::units::picas<double>(6.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripMass", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::kilograms<double>(5.0));
	expectRoundTrip(wpi::units::grams<double>(250.0));
	expectRoundTrip(wpi::units::mass::pounds<double>(10.0));
	expectRoundTrip(wpi::units::slugs<double>(1.0));
	expectRoundTrip(wpi::units::stone<double>(11.0));
	expectRoundTrip(wpi::units::troy_ounces<double>(2.0));
	expectRoundTrip(wpi::units::carats<double>(0.5));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripTime", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::seconds<double>(60.0));
	expectRoundTrip(wpi::units::minutes<double>(1.5));
	expectRoundTrip(wpi::units::hours<double>(24.0));
	expectRoundTrip(wpi::units::milliseconds<double>(500.0));
	expectRoundTrip(wpi::units::fortnights<double>(1.0));
	expectRoundTrip(wpi::units::julian_years<double>(1.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripAngle", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::radians<double>(3.14159));
	expectRoundTrip(wpi::units::degrees<double>(90.0));
	expectRoundTrip(wpi::units::turns<double>(0.25));
	expectRoundTrip(wpi::units::gradians<double>(100.0));
	expectRoundTrip(wpi::units::angular_mils<double>(1600.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripTemperature", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::kelvin<double>(300.0));
	expectRoundTrip(wpi::units::celsius<double>(100.0));
	expectRoundTrip(wpi::units::fahrenheit<double>(212.0));
	expectRoundTrip(wpi::units::rankine<double>(491.67));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripCurrentAndCharge", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::amperes<double>(2.0));
	expectRoundTrip(wpi::units::milliamperes<double>(500.0));
	expectRoundTrip(wpi::units::coulombs<double>(1.0));
	expectRoundTrip(wpi::units::ampere_hours<double>(3.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripSubstanceAndLuminous", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::mols<double>(2.0));
	expectRoundTrip(wpi::units::candelas<double>(60.0));
	expectRoundTrip(wpi::units::lumens<double>(800.0));
	expectRoundTrip(wpi::units::lux<double>(500.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripData", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::bytes<double>(1024.0));
	expectRoundTrip(wpi::units::kilobytes<double>(4.0));
	expectRoundTrip(wpi::units::gigabytes<double>(2.0));
	expectRoundTrip(wpi::units::kibibytes<double>(1.0));
	expectRoundTrip(wpi::units::exbibytes<double>(1.0));
	expectRoundTrip(wpi::units::bits<double>(8.0));
	expectRoundTrip(wpi::units::gigabits<double>(10.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripDerivedDimensions", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::meters_per_second<double>(26.8224));
	expectRoundTrip(wpi::units::miles_per_hour<double>(60.0));
	expectRoundTrip(wpi::units::knots<double>(100.0));
	expectRoundTrip(wpi::units::meters_per_second_squared<double>(9.81));
	expectRoundTrip(wpi::units::newtons<double>(10.0));
	expectRoundTrip(wpi::units::force::pounds<double>(25.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripEnergyPowerPressure", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::joules<double>(500.0));
	expectRoundTrip(wpi::units::kilowatt_hours<double>(3.0));
	expectRoundTrip(wpi::units::watts<double>(60.0));
	expectRoundTrip(wpi::units::horsepower<double>(1.0));
	expectRoundTrip(wpi::units::pascals<double>(101325.0));
	expectRoundTrip(wpi::units::atmospheres<double>(1.0));
	expectRoundTrip(wpi::units::pounds_per_square_inch<double>(14.7));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripElectromagnetic", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::volts<double>(120.0));
	expectRoundTrip(wpi::units::farads<double>(0.001));
	expectRoundTrip(wpi::units::ohms<double>(50.0));
	expectRoundTrip(wpi::units::henries<double>(2.0));
	expectRoundTrip(wpi::units::webers<double>(1.0));
	expectRoundTrip(wpi::units::teslas<double>(1.5));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripAreaVolumeFlow", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::square_meters<double>(25.0));
	expectRoundTrip(wpi::units::acres<double>(2.0));
	expectRoundTrip(wpi::units::cubic_meters<double>(3.0));
	expectRoundTrip(wpi::units::liters<double>(2.0));
	expectRoundTrip(wpi::units::gallons<double>(5.0));
	expectRoundTrip(wpi::units::liters_per_second<double>(1.5));
	expectRoundTrip(wpi::units::gallons_per_minute<double>(10.0));
	expectRoundTrip(wpi::units::cubic_feet_per_second<double>(1.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripFrequencyDensityTorque", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::hertz<double>(60.0));
	expectRoundTrip(wpi::units::kilohertz<double>(44.1));
	expectRoundTrip(wpi::units::kilograms_per_cubic_meter<double>(1000.0));
	expectRoundTrip(wpi::units::newton_meters<double>(50.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripDecibelScale", "[wpimath][units]")
{
	// the decibel scale is non-linear (logarithmic); the stored SI-base value must still round-trip the dB reading
	expectRoundTrip(wpi::units::dBW<double>(10.0));
	expectRoundTrip(wpi::units::dBW<double>(-20.0));
	expectRoundTrip(wpi::units::dBm<double>(-3.0));
	expectRoundTrip(wpi::units::decibels<double>(6.0));
	expectRoundTrip(wpi::units::decibels<double>(0.0));

	// a decibel stream collapses back to the same reading
	const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::dBW<double>(23.0)));
	REQUIRE(v);
	const auto back = v->to<wpi::units::dBW<double>>();
	REQUIRE(back);
	CHECK_THAT(23.0, Catch::Matchers::WithinAbs(back->to<double>(), 5.0e-9));
}

TEST_CASE_METHOD(Serialization, "Serialization crossUnitConversionNotTautological", "[wpimath][units]")
{
	// These serialize as unit A and deserialize into a DIFFERENT unit B of the same dimension, then assert against
	// values computed here by hand (NOT read back from the library). A tautological round-trip (serialize/deserialize
	// merely shuffling the same number) would fail these, because the value must actually cross a unit conversion.

	// 60 mph -> m/s : 1 mile = 1609.344 m, 1 h = 3600 s -> 60*1609.344/3600 = 26.8224 exactly
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::miles_per_hour<double>(60.0)));
		REQUIRE(v);
		const auto mpsA = v->to<wpi::units::meters_per_second<double>>();
		REQUIRE(mpsA);
		CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(mpsA->value(), 5.0e-10));
	}
	// 100 ft -> m : 1 ft = 0.3048 m -> 30.48 m
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::feet<double>(100.0)));
		REQUIRE(v);
		CHECK_THAT(30.48, Catch::Matchers::WithinAbs(v->to<wpi::units::meters<double>>()->value(), 5.0e-11));
	}
	// 1 kg -> pounds (mass): 1 lb = 0.45359237 kg -> 1/0.45359237 = 2.2046226218... lb
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::kilograms<double>(1.0)));
		REQUIRE(v);
		CHECK_THAT(2.2046226218487757, Catch::Matchers::WithinAbs(v->to<wpi::units::mass::pounds<double>>()->value(), 5.0e-12));
	}
	// 1 hour -> seconds : 3600
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::hours<double>(1.0)));
		REQUIRE(v);
		CHECK(3600.0 == v->to<wpi::units::seconds<double>>()->value());
	}
	// 100 celsius -> fahrenheit : 100*9/5 + 32 = 212 (affine translation must survive the stream)
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::celsius<double>(100.0)));
		REQUIRE(v);
		CHECK_THAT(212.0, Catch::Matchers::WithinAbs(v->to<wpi::units::fahrenheit<double>>()->value(), 5.0e-11));
	}
	// 0 celsius -> kelvin : 273.15
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::celsius<double>(0.0)));
		REQUIRE(v);
		CHECK_THAT(273.15, Catch::Matchers::WithinAbs(v->to<wpi::units::kelvin<double>>()->value(), 5.0e-12));
	}
	// 180 degrees -> radians : pi
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::degrees<double>(180.0)));
		REQUIRE(v);
		CHECK_THAT(3.141592653589793, Catch::Matchers::WithinAbs(v->to<wpi::units::radians<double>>()->value(), 5.0e-15));
	}
	// 1 kibibyte -> bytes : 1024 ; 1 kilobyte -> bytes : 1000 (binary vs decimal prefixes, distinct)
	{
		const auto kib = wpi::units::deserialize(wpi::units::serialize(wpi::units::kibibytes<double>(1.0)));
		const auto kbV  = wpi::units::deserialize(wpi::units::serialize(wpi::units::kilobytes<double>(1.0)));
		REQUIRE(kib && kbV);
		CHECK(1024.0 == kib->to<wpi::units::bytes<double>>()->value());
		CHECK(1000.0 == kbV->to<wpi::units::bytes<double>>()->value());
	}
	// 1 atmosphere -> pascals : 101325 exactly
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::atmospheres<double>(1.0)));
		REQUIRE(v);
		CHECK_THAT(101325.0, Catch::Matchers::WithinAbs(v->to<wpi::units::pascals<double>>()->value(), 5.0e-7));
	}
	// 1 kWh -> joules : 3.6e6
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::kilowatt_hours<double>(1.0)));
		REQUIRE(v);
		CHECK_THAT(3.6e6, Catch::Matchers::WithinAbs(v->to<wpi::units::joules<double>>()->value(), 5.0e-3));
	}
	// serialize m/s, read back as mph : 26.8224 m/s -> 60 mph (the reverse of the first case)
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::meters_per_second<double>(26.8224)));
		REQUIRE(v);
		CHECK_THAT(60.0, Catch::Matchers::WithinAbs(v->to<wpi::units::miles_per_hour<double>>()->value(), 5.0e-10));
	}
}

TEST_CASE_METHOD(Serialization, "Serialization edgeValues", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::meters<double>(0.0));
	expectRoundTrip(wpi::units::meters<double>(-42.5));
	expectRoundTrip(wpi::units::meters<double>(1e300));
	expectRoundTrip(wpi::units::meters<double>(1e-300));
	expectRoundTrip(wpi::units::meters<double>(3.141592653589793));
	expectRoundTrip(wpi::units::meters<double>(9000000000000.0));
	expectRoundTrip(wpi::units::meters<double>(std::numeric_limits<double>::quiet_NaN()));
	expectRoundTrip(wpi::units::meters<double>(std::numeric_limits<double>::infinity()));
	expectRoundTrip(wpi::units::meters<double>(-std::numeric_limits<double>::infinity()));
}

TEST_CASE_METHOD(Serialization, "Serialization underlyingTypes", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::meters<double>(1.5));
	expectRoundTrip(wpi::units::meters<float>(1.5f));
	expectRoundTrip(wpi::units::meters<int>(7));
	expectRoundTrip(wpi::units::seconds<float>(0.25f));
	expectRoundTrip(wpi::units::kilograms<int>(5));
}

TEST_CASE_METHOD(Serialization, "Serialization valueKinds", "[wpimath][units]")
{
	// whole value -> integer varint (tersest)
	CHECK(wpi::units::serialize(wpi::units::meters<double>(5.0)).size() < wpi::units::serialize(wpi::units::meters<double>(5.5)).size());
	// exact-float value -> f32; irrational -> f64
	const auto f32bytes = wpi::units::serialize(wpi::units::meters<double>(1.5));   // exact as float
	const auto f64bytes = wpi::units::serialize(wpi::units::meters<double>(0.1));   // not exact as float
	CHECK(f32bytes.size() < f64bytes.size());
}

TEST_CASE_METHOD(Serialization, "Serialization collapseMethods", "[wpimath][units]")
{
	const auto bytes = wpi::units::serialize(60.0_mph);
	const auto v     = wpi::units::deserialize(bytes);
	REQUIRE(v);

	// to<> : safe, expected
	const auto mpsV = v->to<wpi::units::meters_per_second<double>>();
	REQUIRE(mpsV);
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(mpsV->value(), 5.0e-9));
	const auto mphV = v->to<wpi::units::miles_per_hour<double>>();
	REQUIRE(mphV);
	CHECK_THAT(60.0, Catch::Matchers::WithinAbs(mphV->value(), 5.0e-9));

	// to<> wrong dimension -> error, no throw
	const auto wrong = v->to<wpi::units::kilograms<double>>();
	CHECK_FALSE(wrong);
	CHECK(wpi::units::deserialize_error::dimension_mismatch == wrong.error());

	// try_to : throwing
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(v->try_to<wpi::units::meters_per_second<double>>().value(), 5.0e-9));
	CHECK_THROWS_AS((void)v->try_to<wpi::units::kilograms<double>>(), std::runtime_error);

	// unit_cast : the free-function throwing idiom, same result as try_to
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(wpi::units::unit_cast<wpi::units::meters_per_second<double>>(*v).value(), 5.0e-9));
	CHECK_THROWS_AS((void)wpi::units::unit_cast<wpi::units::kilograms<double>>(*v), std::runtime_error);

	// visit : canonical unit, no target named
	bool visited = false;
	v->visit(
		[&](auto q)
		{
			visited = true;
			CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(q.template to<double>(), 5.0e-9));
		});
	CHECK(visited);

	// is<>
	CHECK(v->is<wpi::units::dimension::velocity>());
	CHECK_FALSE(v->is<wpi::units::dimension::mass>());
	CHECK_THAT(26.8224, Catch::Matchers::WithinAbs(v->value_in_base(), 5.0e-9));
}

TEST_CASE_METHOD(Serialization, "Serialization visitResolvesManyDimensions", "[wpimath][units]")
{
	auto check = [](auto quantity, double expectedBase)
	{
		const auto v = wpi::units::deserialize(wpi::units::serialize(quantity));
		REQUIRE(v);
		bool visited = false;
		v->visit(
			[&](auto q)
			{
				visited = true;
				CHECK_THAT(expectedBase, Catch::Matchers::WithinAbs(q.template to<double>(), std::abs(expectedBase) * 1e-9 + 1e-12));
			});
		CHECK(visited);
	};
	check(wpi::units::meters<double>(100.0), 100.0);
	check(wpi::units::newtons<double>(10.0), 10.0);
	check(wpi::units::joules<double>(5.0), 5.0);
	check(wpi::units::watts<double>(60.0), 60.0);
	check(wpi::units::pascals<double>(101325.0), 101325.0);
	check(wpi::units::hertz<double>(60.0), 60.0);
	check(wpi::units::amperes<double>(2.0), 2.0);
	check(wpi::units::liters_per_second<double>(1.0), 0.001);
}

TEST_CASE_METHOD(Serialization, "Serialization lossyIntegerTargetRejected", "[wpimath][units]")
{
	// a fractional value cannot be represented in an integer-underlying target
	const auto bytes = wpi::units::serialize(1.5_m);
	const auto v     = wpi::units::deserialize(bytes);
	REQUIRE(v);
	const auto asInt = v->to<wpi::units::meters<int>>();
	CHECK_FALSE(asInt);
	CHECK(wpi::units::deserialize_error::lossy_target == asInt.error());
	// a whole value converts to an integer target fine
	const auto whole = wpi::units::deserialize(wpi::units::serialize(wpi::units::meters<double>(3.0)));
	REQUIRE(whole);
	const auto ok = whole->to<wpi::units::meters<int>>();
	REQUIRE(ok);
	CHECK(3 == ok->value());
}

TEST_CASE_METHOD(Serialization, "Serialization errorPaths", "[wpimath][units]")
{
	// an owning copy of the bytes, so the tamper cases below can mutate it
	const wpi::units::any_unit    encoded = wpi::units::serialize(60.0_mph);
	const std::vector<std::byte> good(encoded.bytes().begin(), encoded.bytes().end());

	// empty buffer -> truncated
	{
		std::vector<std::byte> empty;
		const auto             r = wpi::units::deserialize(empty);
		CHECK_FALSE(r);
		CHECK(wpi::units::deserialize_error::truncated == r.error());
	}
	// bad version byte
	{
		auto bad = good;
		bad[0]   = std::byte{0xFF};
		const auto r = wpi::units::deserialize(bad);
		CHECK_FALSE(r);
		CHECK(wpi::units::deserialize_error::bad_version == r.error());
	}
	// truncated mid-stream (drop the value/tail)
	{
		std::vector<std::byte> partial(good.begin(), good.begin() + 3);
		const auto             r = wpi::units::deserialize(partial);
		CHECK_FALSE(r);
		CHECK(wpi::units::deserialize_error::truncated == r.error());
	}
	// typed deserialize of a wrong dimension -> dimension_mismatch
	{
		const auto r = wpi::units::deserialize<wpi::units::kilograms<double>>(good);
		CHECK_FALSE(r);
		CHECK(wpi::units::deserialize_error::dimension_mismatch == r.error());
	}
}

TEST_CASE_METHOD(Serialization, "Serialization userDefinedDimensionIsExtensible", "[wpimath][units]")
{
	// the whole point: a dimension the library never defined round-trips with no central table.
	// serialize via the generated unit constant (wpi::units::px, in the inline screen namespace) to exercise it too.
	const auto bytes = wpi::units::serialize(1920.0 * wpi::units::px);
	const auto v     = wpi::units::deserialize(bytes);
	REQUIRE(v);
	CHECK(v->is<wpi::units::dimension::pixels>());
	CHECK_FALSE(v->is<wpi::units::dimension::length>());
	const auto pxV = v->to<wpi::units::screen::dots<double>>();
	REQUIRE(pxV);
	CHECK(1920.0 == pxV->value());
	// zero-candidate visit cannot know a user dimension (throws); explicit candidate resolves it
	CHECK_THROWS_AS(v->visit([](auto) {}), std::runtime_error);
	bool visited = false;
	v->visit<wpi::units::dimension::pixels>([&](auto q) { visited = true; CHECK(1920.0 == q.value()); });
	CHECK(visited);
}

TEST_CASE_METHOD(Serialization, "Serialization compoundOfUserDimensions", "[wpimath][units]")
{
	// a compound spanning multiple base dimensions (pixels / second): arbitrary arity, no fixed ceiling
	auto rate  = wpi::units::screen::dots<double>(60.0) / wpi::units::seconds<double>(1.0);
	auto bytes = wpi::units::serialize(rate);
	auto v     = wpi::units::deserialize(bytes);
	REQUIRE(v);
	CHECK(2u == v->identity().terms.size());
	auto back = v->to<decltype(rate)>();
	REQUIRE(back);
	CHECK(rate.template to<double>() == back->template to<double>());
}

TEST_CASE_METHOD(Serialization, "Serialization dimensionMismatchAcrossManyPairs", "[wpimath][units]")
{
	// every serialized dimension rejects collapse into an unrelated dimension
	auto reject = [](auto quantity, auto wrongTargetPrototype)
	{
		using Wrong  = decltype(wrongTargetPrototype);
		const auto v = wpi::units::deserialize(wpi::units::serialize(quantity));
		REQUIRE(v);
		const auto r = v->template to<Wrong>();
		CHECK_FALSE(r);
		if (!r)
		{
			CHECK(wpi::units::deserialize_error::dimension_mismatch == r.error());
		}
	};
	reject(wpi::units::meters<double>(1.0), wpi::units::seconds<double>(0.0));
	reject(wpi::units::kilograms<double>(1.0), wpi::units::newtons<double>(0.0));
	reject(wpi::units::joules<double>(1.0), wpi::units::watts<double>(0.0));
	reject(wpi::units::hertz<double>(1.0), wpi::units::seconds<double>(0.0));
	reject(wpi::units::amperes<double>(1.0), wpi::units::coulombs<double>(0.0));
	reject(wpi::units::square_meters<double>(1.0), wpi::units::meters<double>(0.0));
	reject(wpi::units::cubic_meters<double>(1.0), wpi::units::square_meters<double>(0.0));
	reject(wpi::units::meters_per_second<double>(1.0), wpi::units::meters<double>(0.0));
}

TEST_CASE_METHOD(Serialization, "Serialization wireStabilityGolden", "[wpimath][units]")
{
	// a frozen fixture guards the wire format against silent drift. 100 m: version(1) + header(kind=ivarint,0)
	// + count(1) + hash("length" 8 bytes) + zigzag-exponent(1 -> 2) + value-varint(100 -> 200 = 0xC8 0x01)
	const wpi::units::any_unit bytesU = wpi::units::serialize(wpi::units::meters<double>(100.0));
	const auto bytes = bytesU.bytes();
	REQUIRE(14u == bytes.size());
	CHECK(std::byte{1} == bytes[0]);       // version
	CHECK(std::byte{0} == bytes[1]);       // header: value_kind::ivarint, no fracExp
	CHECK(std::byte{1} == bytes[2]);       // one dimension term
	// bytes[3..10] the 8-byte length-name hash; bytes[11] exponent(=2 zigzag); bytes[12..13] value 100 (varint 200)
	const std::uint64_t lengthHash = wpi::units::detail::name_hash("length");
	for (unsigned int i = 0; i < 8; ++i)
		CHECK(std::byte{static_cast<std::uint8_t>(lengthHash >> (8 * i))} == bytes[3 + i]);
}

TEST_CASE_METHOD(Serialization, "Serialization nameHashIsStableAndDistinct", "[wpimath][units]")
{
	// the wire key is a hash of the dimension name; distinct built-in names must not collide
	std::array<std::string_view, 9> names{"length", "mass", "time", "current", "temperature", "amount of substance", "luminous intensity", "angle", "data"};
	for (std::size_t i = 0; i < names.size(); ++i)
		for (std::size_t j = i + 1; j < names.size(); ++j)
			CHECK(wpi::units::detail::name_hash(names[i]) != wpi::units::detail::name_hash(names[j]));
	// stable: same input, same hash
	CHECK(wpi::units::detail::name_hash("length") == wpi::units::detail::name_hash("length"));
}

// ---- second wave: broader, more redundant coverage --------------------------------------------------------------

TEST_CASE_METHOD(Serialization, "Serialization roundTripManyLengthUnits", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::millimeters<double>(12.0));
	expectRoundTrip(wpi::units::centimeters<double>(2.5));
	expectRoundTrip(wpi::units::decimeters<double>(3.0));
	expectRoundTrip(wpi::units::micrometers<double>(50.0));
	expectRoundTrip(wpi::units::inches<double>(6.0));
	expectRoundTrip(wpi::units::yards<double>(100.0));
	expectRoundTrip(wpi::units::chains<double>(2.0));
	expectRoundTrip(wpi::units::fathoms<double>(3.0));
	expectRoundTrip(wpi::units::mils<double>(500.0));
	expectRoundTrip(wpi::units::astronomical_units<double>(1.0));
	expectRoundTrip(wpi::units::lightyears<double>(4.0));
	expectRoundTrip(wpi::units::parsecs<double>(1.0));
	expectRoundTrip(wpi::units::hands<double>(15.0));
	expectRoundTrip(wpi::units::barleycorns<double>(9.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripManyTimeUnits", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::nanoseconds<double>(250.0));
	expectRoundTrip(wpi::units::microseconds<double>(100.0));
	expectRoundTrip(wpi::units::days<double>(3.0));
	expectRoundTrip(wpi::units::weeks<double>(2.0));
	expectRoundTrip(wpi::units::years<double>(1.0));
	expectRoundTrip(wpi::units::decades<double>(1.0));
	expectRoundTrip(wpi::units::centuries<double>(1.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripManyVelocityUnits", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::feet_per_second<double>(100.0));
	expectRoundTrip(wpi::units::kilometers_per_hour<double>(120.0));
	expectRoundTrip(wpi::units::feet_per_minute<double>(500.0));
	expectRoundTrip(wpi::units::inches_per_second<double>(12.0));
	expectRoundTrip(wpi::units::kilometers_per_second<double>(7.8));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripManyPressureUnits", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::bars<double>(2.0));
	expectRoundTrip(wpi::units::millibars<double>(1013.0));
	expectRoundTrip(wpi::units::torrs<double>(760.0));
	expectRoundTrip(wpi::units::millimeters_of_mercury<double>(760.0));
	expectRoundTrip(wpi::units::kilopascals<double>(101.325));
	expectRoundTrip(wpi::units::baryes<double>(10.0));
}

TEST_CASE_METHOD(Serialization, "Serialization roundTripManyEnergyPowerUnits", "[wpimath][units]")
{
	expectRoundTrip(wpi::units::kilojoules<double>(4.0));
	expectRoundTrip(wpi::units::calories<double>(500.0));
	expectRoundTrip(wpi::units::british_thermal_units<double>(1.0));
	expectRoundTrip(wpi::units::ergs<double>(1000.0));
	expectRoundTrip(wpi::units::watt_hours<double>(50.0));
	expectRoundTrip(wpi::units::kilowatts<double>(3.0));
	expectRoundTrip(wpi::units::megawatts<double>(1.0));
	expectRoundTrip(wpi::units::metric_horsepower<double>(2.0));
}

TEST_CASE_METHOD(Serialization, "Serialization everyBuiltinDimensionRoundTripsViaVisit", "[wpimath][units]")
{
	// serialize one quantity per built-in dimension and confirm the default zero-candidate visit resolves it,
	// with the visited canonical value equal to the SI-base value. Exhaustive over the dimension zoo.
	auto viaVisit = [](auto quantity)
	{
		const double base = wpi::units::detail::canonical_unit_t<traits::dimension_of_t<typename decltype(quantity)::conversion_factor>>(quantity).value();
		const auto   v    = wpi::units::deserialize(wpi::units::serialize(quantity));
		CHECK(v.has_value());
		bool visited = false;
		v->visit(
			[&](auto q)
			{
				visited = true;
				CHECK_THAT(base, Catch::Matchers::WithinAbs(q.template to<double>(), std::abs(base) * 1e-9 + 1e-12));
			});
		CHECK(visited);
	};
	viaVisit(wpi::units::meters<double>(2.0));
	viaVisit(wpi::units::kilograms<double>(3.0));
	viaVisit(wpi::units::seconds<double>(4.0));
	viaVisit(wpi::units::amperes<double>(1.0));
	viaVisit(wpi::units::kelvin<double>(300.0));
	viaVisit(wpi::units::mols<double>(2.0));
	viaVisit(wpi::units::candelas<double>(5.0));
	viaVisit(wpi::units::radians<double>(1.0));
	viaVisit(wpi::units::steradians<double>(1.0));
	viaVisit(wpi::units::bytes<double>(64.0));
	viaVisit(wpi::units::hertz<double>(50.0));
	viaVisit(wpi::units::meters_per_second<double>(10.0));
	viaVisit(wpi::units::radians_per_second<double>(2.0));
	viaVisit(wpi::units::meters_per_second_squared<double>(9.8));
	viaVisit(wpi::units::newtons<double>(5.0));
	viaVisit(wpi::units::square_meters<double>(4.0));
	viaVisit(wpi::units::cubic_meters<double>(2.0));
	viaVisit(wpi::units::liters_per_second<double>(1.0));
	viaVisit(wpi::units::pascals<double>(1000.0));
	viaVisit(wpi::units::coulombs<double>(1.0));
	viaVisit(wpi::units::joules<double>(7.0));
	viaVisit(wpi::units::watts<double>(9.0));
	viaVisit(wpi::units::volts<double>(12.0));
	viaVisit(wpi::units::farads<double>(0.01));
	viaVisit(wpi::units::ohms<double>(100.0));
	viaVisit(wpi::units::siemens<double>(0.1));
	viaVisit(wpi::units::webers<double>(1.0));
	viaVisit(wpi::units::henries<double>(2.0));
	viaVisit(wpi::units::lumens<double>(500.0));
	viaVisit(wpi::units::lux<double>(300.0));
	viaVisit(wpi::units::newton_meters<double>(20.0));
	viaVisit(wpi::units::kilograms_per_cubic_meter<double>(998.0));
}

TEST_CASE_METHOD(Serialization, "Serialization determinismSameInputSameBytes", "[wpimath][units]")
{
	// serialization is a pure function of the value: same quantity -> identical bytes, every time
	CHECK(std::ranges::equal(wpi::units::serialize(60.0_mph).bytes(), wpi::units::serialize(60.0_mph).bytes()));
	CHECK(std::ranges::equal(wpi::units::serialize(wpi::units::meters<double>(3.14)).bytes(),
								   wpi::units::serialize(wpi::units::meters<double>(3.14)).bytes()));
}

TEST_CASE_METHOD(Serialization, "Serialization distinctQuantitiesDistinctBytes", "[wpimath][units]")
{
	// different value -> different quantity; different dimension -> different quantity
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1.0)) != wpi::units::serialize(wpi::units::meters<double>(2.0)));
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1.0)) != wpi::units::serialize(wpi::units::seconds<double>(1.0)));
	// same dimension, different unit but SAME base value -> EQUAL (self-describing by dimension+base)
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1000.0)) == wpi::units::serialize(wpi::units::kilometers<double>(1.0)));
	// and byte-identical on the wire, since the encoding is a pure function of dimension + base
	CHECK(std::ranges::equal(wpi::units::serialize(wpi::units::meters<double>(1000.0)).bytes(),
								   wpi::units::serialize(wpi::units::kilometers<double>(1.0)).bytes()));
}

TEST_CASE_METHOD(Serialization, "Serialization valueKindIvarintForWholeNumbers", "[wpimath][units]")
{
	// a whole SI-base value uses the integer-varint kind (header low bits == 0)
	const wpi::units::any_unit wholeU = wpi::units::serialize(wpi::units::meters<double>(42.0));
	const auto whole = wholeU.bytes();
	CHECK(std::byte{0} == std::byte{static_cast<std::uint8_t>(std::to_integer<std::uint8_t>(whole[1]) & 0x03)});
	// an exact-float value uses f32 (kind == 1)
	const wpi::units::any_unit f32U = wpi::units::serialize(wpi::units::meters<double>(1.5));
	const auto f32 = f32U.bytes();
	CHECK(1 == std::to_integer<std::uint8_t>(f32[1]) & 0x03);
	// an irrational value uses f64 (kind == 2)
	const wpi::units::any_unit f64U = wpi::units::serialize(wpi::units::meters<double>(0.1));
	const auto f64 = f64U.bytes();
	CHECK(2 == std::to_integer<std::uint8_t>(f64[1]) & 0x03);
}

TEST_CASE_METHOD(Serialization, "Serialization negativeAndFractionalExponents", "[wpimath][units]")
{
	// frequency is time^-1 (a negative exponent); the fracExp path and negative exponents must survive
	expectRoundTrip(wpi::units::hertz<double>(60.0));
	expectRoundTrip(wpi::units::becquerels<double>(1000.0));
	// area (length^2) and volume (length^3): positive multi-exponents
	expectRoundTrip(wpi::units::square_feet<double>(100.0));
	expectRoundTrip(wpi::units::cubic_inches<double>(50.0));
}

TEST_CASE_METHOD(Serialization, "Serialization typedFastPathMatchesErased", "[wpimath][units]")
{
	// deserialize<Unit> equals deserialize(...).to<Unit>() for a spread of units
	auto agree = [](auto quantity)
	{
		using Q          = decltype(quantity);
		const auto bytes = wpi::units::serialize(quantity);
		const auto typed = wpi::units::deserialize<Q>(bytes);
		const auto erased = wpi::units::deserialize(bytes);
		REQUIRE(typed.has_value());
		REQUIRE(erased.has_value());
		const auto viaErased = erased->template to<Q>();
		REQUIRE(viaErased.has_value());
		CHECK(typed->template to<double>() == viaErased->template to<double>());
	};
	agree(wpi::units::meters<double>(1.5));
	agree(wpi::units::newtons<double>(9.81));
	agree(wpi::units::celsius<double>(37.0));
	agree(wpi::units::dBW<double>(3.0));
	agree(wpi::units::gallons_per_minute<double>(12.0));
}

TEST_CASE_METHOD(Serialization, "Serialization peekIdentityMatchesDeserialized", "[wpimath][units]")
{
	// deserialize surfaces the identity; it must equal the compile-time identity of the source unit
	const auto v = wpi::units::deserialize(wpi::units::serialize(60.0_mph));
	REQUIRE(v);
	CHECK(v->identity() == wpi::units::detail::identity_of<wpi::units::meters_per_second<double>>());
	CHECK_FALSE(v->identity() == wpi::units::detail::identity_of<wpi::units::kilograms<double>>());
}

TEST_CASE_METHOD(Serialization, "Serialization explicitCandidateDisambiguatesSharedSignature", "[wpimath][units]")
{
	// torque and energy share a dimension (force*length); an explicit candidate selects which the visitor sees
	const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::newton_meters<double>(10.0)));
	REQUIRE(v);
	bool asTorque = false;
	v->visit<wpi::units::dimension::torque>([&](auto q) { asTorque = true; CHECK_THAT(10.0, Catch::Matchers::WithinAbs(q.template to<double>(), 5.0e-9)); });
	CHECK(asTorque);
}

TEST_CASE_METHOD(Serialization, "Serialization truncatedAtEveryBoundary", "[wpimath][units]")
{
	// truncating the stream at EVERY length short of complete must yield truncated (never a crash or bad decode)
	const wpi::units::any_unit fullU = wpi::units::serialize(60.0_mph);
	const auto full = fullU.bytes();
	for (std::size_t n = 0; n < full.size(); ++n)
	{
		std::vector<std::byte> partial(full.begin(), full.begin() + static_cast<std::ptrdiff_t>(n));
		const auto             r = wpi::units::deserialize(partial);
		CHECK_FALSE(r.has_value()) << "n=" << n;
		if (!r)
		{
			CHECK(r.error() == wpi::units::deserialize_error::truncated || r.error() == wpi::units::deserialize_error::bad_version) << "n=" << n;
		}
	}
	// the full stream decodes
	CHECK(wpi::units::deserialize(full).has_value());
}

TEST_CASE_METHOD(Serialization, "Serialization streamOfManyQuantitiesConcatenated", "[wpimath][units]")
{
	// serialize several quantities into one buffer and confirm each is independently well-formed on its own
	auto a = wpi::units::serialize(wpi::units::meters<double>(1.0));
	auto bytesB = wpi::units::serialize(wpi::units::seconds<double>(2.0));
	auto c = wpi::units::serialize(wpi::units::kilograms<double>(3.0));
	CHECK(wpi::units::deserialize(a)->is<wpi::units::dimension::length>());
	CHECK(wpi::units::deserialize(bytesB)->is<wpi::units::dimension::time>());
	CHECK(wpi::units::deserialize(c)->is<wpi::units::dimension::mass>());
}

TEST_CASE_METHOD(Serialization, "Serialization floatUnderlyingUsesF32OrSmaller", "[wpimath][units]")
{
	// a float-underlying quantity never needs f64 in the stream (its value is representable in <= 4 value bytes)
	const wpi::units::any_unit bytesFU = wpi::units::serialize(wpi::units::meters<float>(3.14159f));
	const auto bytesF = bytesFU.bytes();
	const std::uint8_t kind = std::to_integer<std::uint8_t>(bytesF[1]) & 0x03;
	CHECK(2 != kind); // not f64
}

TEST_CASE_METHOD(Serialization, "Serialization unitCastAndTryToAgree", "[wpimath][units]")
{
	const auto v = wpi::units::deserialize(wpi::units::serialize(wpi::units::joules<double>(500.0)));
	REQUIRE(v);
	CHECK(v->try_to<wpi::units::joules<double>>().value() == wpi::units::unit_cast<wpi::units::joules<double>>(*v).value());
	CHECK(v->try_to<wpi::units::kilojoules<double>>().value() == wpi::units::unit_cast<wpi::units::kilojoules<double>>(*v).value());
}

// serialize returns an any_unit that OWNS its bytes; bytes()/data()/size() are three views of that one buffer.
TEST_CASE_METHOD(Serialization, "Serialization anyUnitOwnsItsBytes", "[wpimath][units]")
{
	const wpi::units::any_unit q = wpi::units::serialize(60.0_mph);
	CHECK(q.size() > 0u);
	CHECK(q.size() == q.bytes().size());
	// data() is a const char* view of the same buffer bytes() spans
	CHECK(static_cast<const void*>(q.data()) == static_cast<const void*>(q.bytes().data()));
	// the buffer is valid for the object's lifetime: repeated access is stable
	CHECK(q.data() == q.data());
	CHECK(std::ranges::equal(q.bytes(), q.bytes()));
}

// The C-interface face (data()/size()) drops straight into a std::ostream::write with NO cast at the call site,
// and the bytes read back from that stream decode without any prior knowledge of the type.
TEST_CASE_METHOD(Serialization, "Serialization dataAndSizeFeedAStreamNoCast", "[wpimath][units]")
{
	const wpi::units::any_unit q = wpi::units::serialize(wpi::units::meters<double>(100.0));

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	stream.write(q.data(), static_cast<std::streamsize>(q.size())); // const char* + size, no reinterpret_cast here
	REQUIRE(stream.good());

	const std::string blob = stream.str();
	REQUIRE(q.size() == blob.size());

	// read the raw bytes back into a fresh byte buffer and decode with no prior type knowledge
	std::vector<std::byte> raw(blob.size());
	std::memcpy(raw.data(), blob.data(), blob.size());
	const auto decoded = wpi::units::deserialize(raw);
	REQUIRE(decoded.has_value());
	const auto back = decoded->to<wpi::units::meters<double>>();
	REQUIRE(back.has_value());
	CHECK(100.0 == back->value());
}

// The bytes on disk are self-sufficient: written from one process-state, read into a disconnected buffer, decoded.
TEST_CASE_METHOD(Serialization, "Serialization bytesAreSelfSufficientAcrossAFreshBuffer", "[wpimath][units]")
{
	const wpi::units::any_unit q = wpi::units::serialize(wpi::units::kilograms<double>(2.5));

	// copy the bytes into a buffer that has no relationship to q, then drop q entirely
	std::vector<std::byte> detached(q.bytes().begin(), q.bytes().end());

	const auto decoded = wpi::units::deserialize(detached);
	REQUIRE(decoded.has_value());
	const auto mass = decoded->to<wpi::units::kilograms<double>>();
	REQUIRE(mass.has_value());
	CHECK(2.5 == mass->value());
}

// any_unit equality is "same dimension AND same base magnitude" — not an encoding or unit-name comparison.
TEST_CASE_METHOD(Serialization, "Serialization equalityIsDimensionAndMagnitude", "[wpimath][units]")
{
	// same quantity, different source unit -> equal (both 1000 m in SI base)
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1000.0)) == wpi::units::serialize(wpi::units::kilometers<double>(1.0)));
	// same unit, same value -> equal
	CHECK(wpi::units::serialize(60.0_mph) == wpi::units::serialize(60.0_mph));
	// same dimension, different magnitude -> not equal
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1.0)) != wpi::units::serialize(wpi::units::meters<double>(2.0)));
	// different dimension, same numeric magnitude -> not equal
	CHECK(wpi::units::serialize(wpi::units::meters<double>(1.0)) != wpi::units::serialize(wpi::units::seconds<double>(1.0)));
	// a round-tripped any_unit equals a freshly serialized one of the same quantity
	const auto decoded = wpi::units::deserialize(wpi::units::serialize(wpi::units::meters<double>(3.0)));
	REQUIRE(decoded.has_value());
	CHECK(*decoded == wpi::units::serialize(wpi::units::meters<double>(3.0)));
	CHECK(*decoded != wpi::units::serialize(wpi::units::meters<double>(4.0)));
}

// any_unit is ordered WITHIN a dimension (by base magnitude) and UNORDERED across dimensions (partial_ordering).
TEST_CASE_METHOD(Serialization, "Serialization orderingWithinDimensionOnly", "[wpimath][units]")
{
	const wpi::units::any_unit shorter = wpi::units::serialize(wpi::units::meters<double>(3.0));
	const wpi::units::any_unit longer  = wpi::units::serialize(wpi::units::meters<double>(5.0));
	const wpi::units::any_unit sameLen = wpi::units::serialize(wpi::units::kilometers<double>(0.003)); // == 3 m
	const wpi::units::any_unit time    = wpi::units::serialize(wpi::units::seconds<double>(3.0));

	// same dimension -> ordered by base magnitude
	CHECK(shorter < longer);
	CHECK(longer > shorter);
	CHECK(shorter <= sameLen);
	CHECK(sameLen >= shorter);
	CHECK((shorter <=> sameLen) == std::partial_ordering::equivalent);
	CHECK((shorter <=> longer) == std::partial_ordering::less);

	// different dimension -> unordered: every relational is false
	CHECK_FALSE(shorter < time);
	CHECK_FALSE(shorter > time);
	CHECK_FALSE(shorter <= time);
	CHECK_FALSE(shorter >= time);
	CHECK((shorter <=> time) == std::partial_ordering::unordered);
}

// to_string() renders a NAMED-unit text form for a known dimension — the same text a concrete unit streams.
TEST_CASE_METHOD(Serialization, "Serialization toStringNamesKnownDimensions", "[wpimath][units]")
{
	// a known dimension renders in its canonical named unit, exactly as operator<<(ostream, unit) would
	const std::string length = wpi::units::serialize(wpi::units::meters<double>(100.0)).to_string();
	CHECK(wpi::units::to_string(wpi::units::meters<double>(100.0)) == length);
	CHECK(std::string::npos != length.find("100"));
	CHECK(std::string::npos != length.find('m'));
	CHECK(std::string::npos == length.find('#')); // NOT the raw hash form

	// a value expressed in a non-canonical unit still names the canonical unit of its dimension (1 km -> "1000 m")
	const std::string serialized_km = wpi::units::serialize(wpi::units::kilometers<double>(1.0)).to_string();
	CHECK(wpi::units::to_string(wpi::units::meters<double>(1000.0)) == serialized_km);

	// a compound dimension renders its canonical dimension form (m s^-2), still no hash
	const std::string accel = wpi::units::serialize(wpi::units::meters_per_second_squared<double>(9.81)).to_string();
	CHECK("9.81 m s^-2" == accel);
	CHECK(std::string::npos == accel.find('#'));
}

// to_string() degrades to the raw hash form for a dimension the library cannot name (the runtime->type wall).
TEST_CASE_METHOD(Serialization, "Serialization toStringFallsBackForUnknownDimension", "[wpimath][units]")
{
	// decode a hand-built record whose base dimension is a hash no built-in dimension owns
	wpi::units::any_unit value;
	{
		const wpi::units::any_unit meters = wpi::units::serialize(wpi::units::meters<double>(3.0));
		std::vector<std::byte> raw(meters.bytes().begin(), meters.bytes().end());
		// the record is [version][header][term-count][8-byte name-hash]...; the single term's hash begins at byte 3
		// (version=1, header=1, count=1 for a base dimension). Flip every hash byte so no known dimension matches.
		for (std::size_t i = 3; i < 3 + 8; ++i)
			raw[i] = static_cast<std::byte>(std::to_integer<std::uint8_t>(raw[i]) ^ 0xFF);
		const auto decoded = wpi::units::deserialize(std::span<const std::byte>(raw));
		REQUIRE(decoded.has_value());
		value = *decoded;
	}
	const std::string named = value.to_string();
	CHECK(value.to_string_raw() == named);      // no known dimension matched -> raw fallback
	CHECK(std::string::npos != named.find('#')); // the raw hash form
}

// to_string_raw() is the honest, name-free rendering: always the hashed signature, identical for any dimension.
TEST_CASE_METHOD(Serialization, "Serialization toStringRawIsAlwaysHashKeyed", "[wpimath][units]")
{
	const std::string dimensionless = wpi::units::serialize(wpi::units::dimensionless<double>(0.25)).to_string_raw();
	CHECK(std::string::npos != dimensionless.find("0.25"));
	CHECK(std::string::npos != dimensionless.find("dimensionless"));

	// dimensionless names nothing, so to_string() and to_string_raw() agree
	CHECK(dimensionless == wpi::units::serialize(wpi::units::dimensionless<double>(0.25)).to_string());

	const std::string length = wpi::units::serialize(wpi::units::meters<double>(100.0)).to_string_raw();
	CHECK(std::string::npos != length.find("100"));
	CHECK(std::string::npos != length.find('['));
	CHECK(std::string::npos != length.find('#')); // a hashed base-dimension term, even for a known dimension
}

// assign_to() collapses into an existing variable, returning whether the dimension matched and leaving it untouched if not.
TEST_CASE_METHOD(Serialization, "Serialization assignToMismatchTolerant", "[wpimath][units]")
{
	const wpi::units::any_unit erased = wpi::units::serialize(wpi::units::kilometers<double>(1.5));

	// a matching dimension is assigned (into the target's own unit), and reported assigned
	wpi::units::meters<double> length{0.0};
	CHECK(erased.assign_to(length));
	CHECK(1500.0 == length.value());

	// a mismatched dimension leaves the target untouched and returns false — an expected outcome, not a throw
	wpi::units::seconds<double> duration{42.0};
	CHECK_FALSE(erased.assign_to(duration));
	CHECK(42.0 == duration.value()); // unchanged

	// the realiq idiom: fan one erased quantity across several typed fields, assigning only where it fits
	wpi::units::meters<double>  intoLength{0.0};
	wpi::units::seconds<double> intoTime{0.0};
	const bool tookLength = erased.assign_to(intoLength);
	const bool tookTime   = erased.assign_to(intoTime);
	CHECK(tookLength);
	CHECK_FALSE(tookTime);

	// a value that cannot be represented exactly in an integral target is reported not-assigned (to's lossy_target)
	wpi::units::meters<int> integralLength{7};
	CHECK(wpi::units::serialize(wpi::units::meters<double>(5.0)).assign_to(integralLength)); // 5 fits
	CHECK(5 == integralLength.value());
	CHECK_FALSE(wpi::units::serialize(wpi::units::meters<double>(2.5)).assign_to(integralLength)); // 2.5 does not
	CHECK(5 == integralLength.value());                                                 // unchanged
}

// operator<< writes the raw binary bytes; operator>> and deserialize(istream) read them back.
TEST_CASE_METHOD(Serialization, "Serialization streamOperatorsRoundTripBinary", "[wpimath][units]")
{
	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

	// << writes the exact serialized bytes (not text)
	const wpi::units::any_unit written = wpi::units::serialize(wpi::units::meters<double>(100.0));
	stream << written;
	CHECK(written.size() == stream.str().size());

	// deserialize(istream) reads one record in a single expression
	const auto decoded = wpi::units::deserialize(stream);
	REQUIRE(decoded.has_value());
	CHECK(100.0 == decoded->to<wpi::units::meters<double>>()->value());

	// operator>> reads the classic way; back-to-back records advance correctly
	std::stringstream seq(std::ios::in | std::ios::out | std::ios::binary);
	seq << wpi::units::serialize(wpi::units::meters<double>(1.0)) << wpi::units::serialize(wpi::units::seconds<double>(2.0));
	wpi::units::any_unit first;
	wpi::units::any_unit second;
	seq >> first >> second;
	REQUIRE(seq.good() || seq.eof());
	CHECK(1.0 == first.to<wpi::units::meters<double>>()->value());
	CHECK(2.0 == second.to<wpi::units::seconds<double>>()->value());

	// a malformed stream sets failbit and leaves the target unchanged
	std::stringstream bad(std::ios::in | std::ios::out | std::ios::binary);
	bad << "not a unit record";
	wpi::units::any_unit target = wpi::units::serialize(wpi::units::meters<double>(7.0));
	bad >> target;
	CHECK(bad.fail());
	CHECK(7.0 == target.to<wpi::units::meters<double>>()->value()); // unchanged
}

// deserialize<Unit>(istream) reads and collapses in one checked step (the front-page idiom).
TEST_CASE_METHOD(Serialization, "Serialization typedDeserializeFromStream", "[wpimath][units]")
{
	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	stream << wpi::units::serialize(60.0_mph);

	// one call, one std::expected: reads the record AND collapses to the requested unit
	const auto speed = wpi::units::deserialize<wpi::units::kilometers_per_hour<double>>(stream);
	REQUIRE(speed.has_value());
	CHECK_THAT(96.56064, Catch::Matchers::WithinAbs(speed->value(), 1e-4));

	// a dimension mismatch is reported, not dereferenced blindly
	std::stringstream length(std::ios::in | std::ios::out | std::ios::binary);
	length << wpi::units::serialize(wpi::units::meters<double>(5.0));
	const auto wrong = wpi::units::deserialize<wpi::units::seconds<double>>(length);
	CHECK_FALSE(wrong.has_value());
	CHECK(wpi::units::deserialize_error::dimension_mismatch == wrong.error());
}

// visit() resolves EVERY library dimension by default — including ones added after the erased-visit feature.
// Guards against builtin_dimensions drifting behind the dimension set (a CI check also enforces this).
TEST_CASE_METHOD(Serialization, "Serialization visitResolvesRecentlyAddedDimensions", "[wpimath][units]")
{
	const auto expectResolves = [](const wpi::units::any_unit& v) {
		bool resolved = false;
		v.visit([&](auto) { resolved = true; });
		return resolved;
	};
	// dimensions that were once missing from builtin_dimensions must now resolve without being named
	CHECK(expectResolves(wpi::units::serialize(wpi::units::pascal_seconds<double>(5.0))));            // dynamic_viscosity
	CHECK(expectResolves(wpi::units::serialize(wpi::units::square_meters_per_second<double>(2.0))));  // kinematic_viscosity
	CHECK(expectResolves(wpi::units::serialize(wpi::units::gigabytes_per_second<double>(1.0))));      // data_transfer_rate
}

// std::hash makes any_unit a usable unordered-container key, consistent with operator==.
TEST_CASE_METHOD(Serialization, "Serialization hashableAsAKey", "[wpimath][units]")
{
	std::unordered_map<wpi::units::any_unit, std::string> byQuantity;
	byQuantity[wpi::units::serialize(wpi::units::meters<double>(1000.0))] = "one kilometer";

	// a different unit of the SAME quantity finds the same entry (equal => equal hash)
	const auto found = byQuantity.find(wpi::units::serialize(wpi::units::kilometers<double>(1.0)));
	REQUIRE(byQuantity.end() != found);
	CHECK("one kilometer" == found->second);

	// a different magnitude is a different key
	CHECK(byQuantity.end() == byQuantity.find(wpi::units::serialize(wpi::units::meters<double>(2000.0))));

	// equal quantities hash equally (the contract std::hash must honor)
	const std::hash<wpi::units::any_unit> hasher;
	CHECK(hasher(wpi::units::serialize(wpi::units::meters<double>(1000.0))) == hasher(wpi::units::serialize(wpi::units::kilometers<double>(1.0))));
}

// A dimension with a FRACTIONAL exponent exercises the fracExp encode/decode path and the to_string denominator.
TEST_CASE_METHOD(Serialization, "Serialization fractionalExponentRoundTripsAndRenders", "[wpimath][units]")
{
	const wpi::units::any_unit q = wpi::units::serialize(3.0 * wpi::units::rt_m); // via the generated unit constant

	// to_string renders the fractional exponent as num/den
	CHECK(std::string::npos != q.to_string().find("1/2"));

	// the fracExp-flagged stream round-trips exactly through a real buffer
	std::vector<std::byte> raw(q.bytes().begin(), q.bytes().end());
	const auto             back = wpi::units::deserialize<wpi::units::root_meters<double>>(raw);
	REQUIRE(back.has_value());
	CHECK(3.0 == back->value());

	// and the erased path preserves the fractional exponent in the decoded identity
	const auto erased = wpi::units::deserialize(raw);
	REQUIRE(erased.has_value());
	REQUIRE(1u == erased->identity().terms.size());
	CHECK(1 == erased->identity().terms[0].num);
	CHECK(2 == erased->identity().terms[0].den);
}

// deserialize reports truncation at each point a record can be cut short (not just an empty buffer).
TEST_CASE_METHOD(Serialization, "Serialization truncationAtEveryStage", "[wpimath][units]")
{
	const wpi::units::any_unit          full = wpi::units::serialize(wpi::units::root_meters<double>(2.5)); // has terms + fracExp + value
	const std::vector<std::byte>   bytes(full.bytes().begin(), full.bytes().end());

	// cutting the stream at every length from 1 .. size-1 must fail cleanly (truncated/bad_version), never crash or
	// silently succeed — this walks the version/header/count/hash/exponent/den/value decode points.
	for (std::size_t n = 1; n < bytes.size(); ++n)
	{
		std::span<const std::byte> partial(bytes.data(), n);
		const auto                 r = wpi::units::deserialize(partial);
		CHECK_FALSE(r.has_value()) << "a " << n << "-byte prefix should not decode";
		if (!r)
		{
			CHECK(r.error() == wpi::units::deserialize_error::truncated || r.error() == wpi::units::deserialize_error::bad_version) << "n=" << n;
		}
	}
}

// deserialize(std::istream&) on a non-seekable stream reports truncated rather than misreading.
TEST_CASE_METHOD(Serialization, "Serialization nonSeekableStreamReportsTruncated", "[wpimath][units]")
{
	// an ostringstream has no get area; reading from it via the istream overload cannot self-delimit a record
	std::ostringstream sink;
	std::istream       notReadable(sink.rdbuf()); // a stream whose tellg() is unusable for framing
	notReadable.setstate(std::ios::eofbit);        // force the unseekable/at-end condition
	const auto r = wpi::units::deserialize(notReadable);
	CHECK_FALSE(r.has_value());
}

// the typed deserialize<Unit>(bytes) fast path propagates a decode error (not just a dimension mismatch).
TEST_CASE_METHOD(Serialization, "Serialization typedFastPathPropagatesDecodeError", "[wpimath][units]")
{
	std::vector<std::byte> garbage{std::byte{0xFF}, std::byte{0x00}}; // bad version byte
	const auto             r = wpi::units::deserialize<wpi::units::meters<double>>(garbage);
	CHECK_FALSE(r.has_value());
	CHECK(wpi::units::deserialize_error::bad_version == r.error());
}
*/
//======================================================================================================================
//  std::format SUPPORT
//======================================================================================================================
//
// These exercise the units-aware std::formatter specialization: value-spec passthrough to the underlying arithmetic
// type's formatter, the unit-opts mini-language after '%', the label forms, the show flags, separators, the byte-
// identical cross-check against to_string/operator<<, the full public API surface, and every runtime throw path.

namespace
{
	// Render a unit through operator<< into a string, for the byte-identical cross-check against std::format("{}").
	template<class T>
	std::string ostreamString(const T& value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}
} // namespace

//-----------------------------
//  DEFAULT: {} == to_string == operator<<
//-----------------------------

// The default spec "{}" must be byte-identical to wpi::units::to_string AND to an operator<< ostringstream for a named
// unit — this three-way equality is the anchor invariant of the whole feature.
TEST_CASE("Format defaultMatchesToStringAndOstreamNamed", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK(std::format("{}", m) == "3.5 m");
	CHECK(std::format("{}", m) == wpi::units::to_string(m));
	CHECK(std::format("{}", m) == ostreamString(m));

	const auto ft = 6.0_ft;
	CHECK(std::format("{}", ft) == "6 ft");
	CHECK(std::format("{}", ft) == wpi::units::to_string(ft));
	CHECK(std::format("{}", ft) == ostreamString(ft));
}

// The same three-way equality for a percent (named dimensionless) unit: it prints its abbreviation "pct", NOT "%".
TEST_CASE("Format defaultMatchesToStringAndOstreamPercent", "[wpimath][units]")
{
	const auto p = wpi::units::percent<double>(50);
	CHECK(std::format("{}", p) == "50 pct");
	CHECK(std::format("{}", p) == wpi::units::to_string(p));
	CHECK(std::format("{}", p) == ostreamString(p));
}

// A genuinely unnamed compound unit (ampere*meter has no named form) prints in dimension-list form; the three-way
// equality still holds, and the value is the base-unit value.
TEST_CASE("Format defaultMatchesToStringAndOstreamUnnamedCompound", "[wpimath][units]")
{
	const auto am = wpi::units::amperes<double>(3) * wpi::units::meters<double>(2);
	CHECK(std::format("{}", am) == "6 A m");
	CHECK(std::format("{}", am) == wpi::units::to_string(am));
	CHECK(std::format("{}", am) == ostreamString(am));
}

// A named acceleration compound (meters/second^2 == "mps2") reduces to its NAMED abbreviation, not a dimension list.
TEST_CASE("Format defaultMatchesToStringAndOstreamNamedCompound", "[wpimath][units]")
{
	const auto accel = wpi::units::meters<double>(6) / (wpi::units::seconds<double>(2) * wpi::units::seconds<double>(1));
	CHECK(std::format("{}", accel) == "3 mps2");
	CHECK(std::format("{}", accel) == wpi::units::to_string(accel));
	CHECK(std::format("{}", accel) == ostreamString(accel));
}

// An integer-underlying named unit: the value formatter is the underlying int type, so the default renders the int
// as-is; still byte-identical to to_string and operator<<.
TEST_CASE("Format defaultMatchesToStringAndOstreamIntUnderlying", "[wpimath][units]")
{
	const wpi::units::meters<int> mi(42);
	CHECK(std::format("{}", mi) == "42 m");
	CHECK(std::format("{}", mi) == wpi::units::to_string(mi));
	CHECK(std::format("{}", mi) == ostreamString(mi));
}

// A unit needing floating-point promotion (float underlying) matches across all three sinks.
TEST_CASE("Format defaultMatchesToStringAndOstreamFloatUnderlying", "[wpimath][units]")
{
	const wpi::units::meters<float> mf(3.5f);
	CHECK(std::format("{}", mf) == "3.5 m");
	CHECK(std::format("{}", mf) == wpi::units::to_string(mf));
	CHECK(std::format("{}", mf) == ostreamString(mf));
}

//-----------------------------
//  VALUE-SPEC PASSTHROUGH (float/double delegate)
//-----------------------------

// Precision variants reach the NUMBER and the unit label still appends.
TEST_CASE("Format precisionReachesValueLabelAppends", "[wpimath][units]")
{
	CHECK(std::format("{:.0f}", 3.5_m) == "4 m"); // banker's-agnostic: 3.5 -> "4" at .0f (round-half-to-even)
	CHECK(std::format("{:.2f}", 3.5_m) == "3.50 m");
	CHECK(std::format("{:.5f}", 3.5_m) == "3.50000 m");
}

// Width pads the numeric field; the unit label is appended after the padded value.
TEST_CASE("Format widthPadsValueThenLabel", "[wpimath][units]")
{
	CHECK(std::format("{:8.2f}", 3.5_m) == "    3.50 m");
	CHECK(std::format("{:>10.2f}", 3.5_m) == "      3.50 m");
}

// Fill + alignment (left/right/center) apply to the numeric field only.
TEST_CASE("Format fillAndAlignApplyToValue", "[wpimath][units]")
{
	CHECK(std::format("{:*>10.1f}", 3.5_m) == "*******3.5 m");
	CHECK(std::format("{:_^12}", 3.5_m) == "____3.5_____ m");
	CHECK(std::format("{:<8.1f}", 3.5_m) == "3.5      m");
}

// Sign controls '+' and space-for-positive on the value.
TEST_CASE("Format signControls", "[wpimath][units]")
{
	CHECK(std::format("{:+.1f}", 3.5_m) == "+3.5 m");
	CHECK(std::format("{: }", 3.5_m) == " 3.5 m");
	CHECK(std::format("{:+.1f}", wpi::units::meters<double>(-3.5)) == "-3.5 m");
}

// Zero-fill pads the numeric field with leading zeros, still appending the label.
TEST_CASE("Format zeroFillValue", "[wpimath][units]")
{
	CHECK(std::format("{:08.2f}", 3.5_m) == "00003.50 m");
}

// Integer presentation types (x/#06x/b/d) work when the underlying type is an integer named unit — the value
// formatter delegate is the underlying int in that case, so the standard int grammar passes through.
TEST_CASE("Format integerPresentationTypesOnIntUnit", "[wpimath][units]")
{
	const wpi::units::meters<int> mi(255);
	CHECK(std::format("{:x}", mi) == "ff m");
	CHECK(std::format("{:#06x}", mi) == "0x00ff m");
	CHECK(std::format("{:b}", mi) == "11111111 m");
	CHECK(std::format("{:d}", mi) == "255 m");
	// combined with a show flag
	CHECK(std::format("{:x%v}", mi) == "ff");
	CHECK(std::format("{:x%u}", mi) == "m");
}

//-----------------------------
//  LABEL FORMS: %a %n %b
//-----------------------------

// %a is explicitly the abbreviation form and equals the default.
TEST_CASE("Format abbreviationFlagEqualsDefault", "[wpimath][units]")
{
	CHECK(std::format("{:%a}", 3.5_m) == std::format("{}", 3.5_m));
	CHECK(std::format("{:%a}", 3.5_m) == "3.5 m");
	CHECK(std::format("{:.2f%a}", 3.5_m) == std::format("{:.2f}", 3.5_m));
}

// %n emits the full unit name for a named unit.
TEST_CASE("Format nameFlagFullName", "[wpimath][units]")
{
	CHECK(std::format("{:%n}", 3.5_m) == "3.5 meters");
	CHECK(std::format("{:.3f%n}", 6.0_ft) == "6.000 feet");
	CHECK(std::format("{:%n}", wpi::units::kilometers<double>(2)) == "2 kilometers");
	CHECK(std::format("{:%n}", wpi::units::degrees<double>(90)) == "90 degrees");
}

// %b converts BOTH the value and the label to SI base units.
TEST_CASE("Format baseFlagConvertsToBaseSI", "[wpimath][units]")
{
	// A non-base named unit is converted: 6 ft = 1.8288 m.
	CHECK(std::format("{:%b}", 6.0_ft) == "1.8288 m");
	CHECK(std::format("{:%b}", wpi::units::kilometers<double>(2)) == "2000 m");
	// A named compound already expressed in base units is unchanged in value; its label decomposes.
	CHECK(std::format("{:%b}", 9.81_mps) == "9.81 m s^-1");
	// An already-base unit is unchanged.
	CHECK(std::format("{:%b}", 3.5_m) == "3.5 m");
	// The value-spec still applies to the (converted) number.
	CHECK(std::format("{:.4f%b}", 10.0_fps) == "3.0480 m s^-1");
}

// %a and %n never convert the value — they render the unit's OWN symbol/name.
TEST_CASE("Format abbreviationAndNameNeverConvert", "[wpimath][units]")
{
	CHECK(std::format("{:%a}", 6.0_ft) == "6 ft");
	CHECK(std::format("{:%n}", 6.0_ft) == "6 feet");
	CHECK(std::format("{:%a}", 10.0_fps) == "10 fps");
	CHECK(std::format("{:%a}", wpi::units::kilometers<double>(2)) == "2 km");
}

// %b on an already-unnamed unit yields the same base-symbol form as the default label (the value is
// already in base units, so nothing changes).
TEST_CASE("Format baseFlagOnUnnamedUnit", "[wpimath][units]")
{
	const auto am = wpi::units::amperes<double>(3) * wpi::units::meters<double>(2);
	CHECK(std::format("{:%b}", am) == "6 A m");
	CHECK(std::format("{:%b}", am) == std::format("{}", am));
}

// %n on an unnamed compound falls back to the base-symbol form (there is no full name to print).
TEST_CASE("Format nameFlagOnUnnamedFallsBackToDimension", "[wpimath][units]")
{
	const auto am = wpi::units::amperes<double>(3) * wpi::units::meters<double>(2);
	CHECK(std::format("{:%n}", am) == "6 A m");
	CHECK(std::format("{:%n}", am) == std::format("{:%a}", am));
}

//-----------------------------
//  SHOW FLAGS: %v (value only) %u (unit only)
//-----------------------------

// %v suppresses the unit label and its separator — value only, no trailing space.
TEST_CASE("Format showValueOnly", "[wpimath][units]")
{
	CHECK(std::format("{:%v}", 3.5_m) == "3.5");
	CHECK(std::format("{:.2f%v}", 3.5_m) == "3.50");
	CHECK(std::format("{:*>10.1f%v}", 3.5_m) == "*******3.5");
}

// %u suppresses the value AND the separator — unit label only, no leading space.
TEST_CASE("Format showUnitOnly", "[wpimath][units]")
{
	CHECK(std::format("{:%u}", 3.5_m) == "m");
	CHECK(std::format("{:%u}", 6.0_ft) == "ft");
	// a value-spec is harmlessly parsed but the value is not emitted under %u.
	CHECK(std::format("{:.2f%u}", 3.5_m) == "m");
	// the full name under unit-only.
	CHECK(std::format("{:%nu}", 3.5_m) == "meters");
	// the base-SI form under unit-only (no leading space).
	CHECK(std::format("{:%bu}", 9.81_mps) == "m s^-1");
}

// %v / %u on an integer-underlying named unit.
TEST_CASE("Format showFlagsIntUnderlying", "[wpimath][units]")
{
	const wpi::units::meters<int> mi(42);
	CHECK(std::format("{:%v}", mi) == "42");
	CHECK(std::format("{:%u}", mi) == "m");
}

// %u never emits a separator regardless of a supplied separator literal.
TEST_CASE("Format unitOnlyIgnoresSeparator", "[wpimath][units]")
{
	CHECK(std::format("{:%u'_'}", 3.5_m) == "m");
	CHECK(std::format("{:%u''}", 3.5_m) == "m");
	CHECK(std::format("{:%'_'u}", 3.5_m) == "m");
}

//-----------------------------
//  SEPARATORS
//-----------------------------

// The default separator (no quotes) is a single space.
TEST_CASE("Format defaultSeparatorIsSingleSpace", "[wpimath][units]")
{
	CHECK(std::format("{:%a}", 3.5_m) == "3.5 m");
	CHECK(std::format("{}", 3.5_m) == "3.5 m");
}

// An empty separator '' glues value and label together.
TEST_CASE("Format emptySeparator", "[wpimath][units]")
{
	CHECK(std::format("{:%a''}", 3.5_m) == "3.5m");
	CHECK(std::format("{:.2f%a''}", 3.5_m) == "3.50m");
}

// A single-character separator literal.
TEST_CASE("Format underscoreSeparator", "[wpimath][units]")
{
	CHECK(std::format("{:%a'_'}", 3.5_m) == "3.5_m");
}

// Escape sequences inside the separator: tab, newline, backslash, quote.
TEST_CASE("Format escapeSeparators", "[wpimath][units]")
{
	CHECK(std::format("{:%a'\t'}", 3.5_m) == "3.5\tm");
	CHECK(std::format("{:%a'\n'}", 3.5_m) == "3.5\nm");
	CHECK(std::format("{:%a'\\\\'}", 3.5_m) == "3.5\\m");
	CHECK(std::format("{:%a'\\''}", 3.5_m) == "3.5'm");
}

// A multi-character separator literal.
TEST_CASE("Format multiCharacterSeparator", "[wpimath][units]")
{
	CHECK(std::format("{:%a' - '}", 3.5_m) == "3.5 - m");
}

// A separator combined with the name form.
TEST_CASE("Format separatorWithNameForm", "[wpimath][units]")
{
	CHECK(std::format("{:%n'_'}", 3.5_m) == "3.5_meters");
}

//-----------------------------
//  FLAG-ORDER INDEPENDENCE
//-----------------------------

// Form-then-separator and separator-then-form parse identically.
TEST_CASE("Format formSeparatorOrderIndependent", "[wpimath][units]")
{
	CHECK(std::format("{:%n'_'}", 3.5_m) == std::format("{:%'_'n}", 3.5_m));
	CHECK(std::format("{:%'_'n}", 3.5_m) == "3.5_meters");
}

// Show-then-form and form-then-show parse identically (each category may appear once).
TEST_CASE("Format showFormOrderIndependent", "[wpimath][units]")
{
	CHECK(std::format("{:%va}", 3.5_m) == std::format("{:%av}", 3.5_m));
	CHECK(std::format("{:%va}", 3.5_m) == "3.5");
	CHECK(std::format("{:%ua}", 3.5_m) == std::format("{:%au}", 3.5_m));
	CHECK(std::format("{:%ua}", 3.5_m) == "m");
}

//-----------------------------
//  API SURFACE
//-----------------------------

// std::format_to into a back_inserter produces the same text as std::format.
TEST_CASE("Format formatToBackInserter", "[wpimath][units]")
{
	std::string out;
	std::format_to(std::back_inserter(out), "{:.2f%n}", 3.5_m);
	CHECK(out == "3.50 meters");
	CHECK(out == std::format("{:.2f%n}", 3.5_m));
}

// std::vformat with make_format_args honors a runtime spec.
TEST_CASE("Format vformatRuntimeSpec", "[wpimath][units]")
{
	const auto  m    = 3.5_m;
	std::string spec = "{:%u}";
	CHECK(std::vformat(spec, std::make_format_args(m)) == "m");
	spec = "{:.2f%n}";
	CHECK(std::vformat(spec, std::make_format_args(m)) == "3.50 meters");
}

//-----------------------------
//  MANY UNIT TYPES / UNDERLYING TYPES
//-----------------------------

// A spread of unit types and underlying arithmetic types all format sensibly.
TEST_CASE("Format manyUnitTypes", "[wpimath][units]")
{
	CHECK(std::format("{}", wpi::units::meters<double>(1.5)) == "1.5 m");
	CHECK(std::format("{}", wpi::units::feet<double>(2.0)) == "2 ft");
	CHECK(std::format("{}", wpi::units::kilometers<double>(3.0)) == "3 km");
	CHECK(std::format("{}", wpi::units::degrees<double>(45.0)) == "45 deg");
	CHECK(std::format("{}", wpi::units::seconds<double>(10.0)) == "10 s");
	CHECK(std::format("{}", 9.81_mps) == "9.81 mps");
	CHECK(std::format("{}", wpi::units::percent<double>(25.0)) == "25 pct");

	// vary the underlying type on the same dimension.
	CHECK(std::format("{}", wpi::units::meters<int>(7)) == "7 m");
	CHECK(std::format("{}", wpi::units::meters<float>(7.25f)) == "7.25 m");
	CHECK(std::format("{}", wpi::units::meters<long>(7L)) == "7 m");
}

//======================================================================================================================
//  std::format ERROR PATHS
//======================================================================================================================
//
// Every throw the parser can raise, one message per case, driven through std::vformat so the (runtime) format string
// reaches parse() and the std::format_error escapes to the caller. A LITERAL bad spec is a compile error instead —
// those live under test/errorMessages/cases/format_*.cpp.

// An unknown unit-format flag throws.
TEST_CASE("Format throwsOnUnknownFlag", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:%z}", std::make_format_args(m)), std::format_error);
}

// A duplicated label-form flag throws.
TEST_CASE("Format throwsOnDuplicateLabelForm", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:%aa}", std::make_format_args(m)), std::format_error);
	CHECK_THROWS_AS((void)std::vformat("{:%an}", std::make_format_args(m)), std::format_error);
	CHECK_THROWS_AS((void)std::vformat("{:%ba}", std::make_format_args(m)), std::format_error);
}

// A duplicated show flag throws.
TEST_CASE("Format throwsOnDuplicateShowFlag", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:%vv}", std::make_format_args(m)), std::format_error);
	CHECK_THROWS_AS((void)std::vformat("{:%vu}", std::make_format_args(m)), std::format_error);
	CHECK_THROWS_AS((void)std::vformat("{:%uv}", std::make_format_args(m)), std::format_error);
}

// An unterminated separator literal throws.
TEST_CASE("Format throwsOnUnterminatedSeparator", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:%a'foo}", std::make_format_args(m)), std::format_error);
}

// A dangling escape at the end of a separator throws.
TEST_CASE("Format throwsOnDanglingEscape", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:%a'\\}", std::make_format_args(m)), std::format_error);
}

// A value-spec the underlying value formatter rejects throws.
TEST_CASE("Format throwsOnInvalidValueSpec", "[wpimath][units]")
{
	const auto m = 3.5_m;
	CHECK_THROWS_AS((void)std::vformat("{:Zf}", std::make_format_args(m)), std::format_error);
}

// A float presentation type on an integer-underlying unit's value formatter throws (the delegate is the int
// formatter, which rejects '.2f'); an int presentation type on a floating-point delegate likewise throws.
TEST_CASE("Format throwsOnMismatchedValueTypeSpec", "[wpimath][units]")
{
	const wpi::units::meters<int> mi(3);
	CHECK_THROWS_AS((void)std::vformat("{:.2f}", std::make_format_args(mi)), std::format_error);

	const wpi::units::meters<double> md(3.5);
	CHECK_THROWS_AS((void)std::vformat("{:x}", std::make_format_args(md)), std::format_error);
}
