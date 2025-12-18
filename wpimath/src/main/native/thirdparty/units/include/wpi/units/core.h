//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++14 unit conversion library with no dependencies
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2016 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
// Parts of this work have been adapted from:
// http://stackoverflow.com/questions/35069778/create-comparison-trait-for-template-classes-whose-parameters-are-in-a-different
// http://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments/28253503
// http://stackoverflow.com/questions/36321295/rational-approximation-of-square-root-of-stdratio-at-compile-time?noredirect=1#comment60266601_36321295
// https://github.com/swatanabe/cppnow17-units
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units/core.h
/// @brief	`unit`, dimensional analysis, generic cmath functions, traits (not dimension-specific),
///			and what they're implemented with (`conversion_factor`, unit manipulators, etc.)
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef UNIT_CORE_H
#define UNIT_CORE_H

#ifndef UNIT_LIB_DEFAULT_TYPE
#define UNIT_LIB_DEFAULT_TYPE double
#endif

//--------------------
//	INCLUDES
//--------------------

#include "core.h"
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <numeric>
#include <ratio>
#include <type_traits>
#include <utility>

#if __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)
#include <fmt/format.h>
#endif // __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)

#include <gcem.hpp>

#if defined(UNIT_LIB_ENABLE_IOSTREAM) || __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)
#include <clocale>
#include <string>

//------------------------------
//	STRING FORMATTER
//------------------------------

namespace wpi::units::detail
{
	template<typename T>
	std::string to_string(const T& t)
	{
		std::string str{std::to_string(t)};

		if constexpr (std::is_floating_point_v<T>)
		{
			unsigned int offset{1};

			// remove trailing decimal points for integer value units. Locale aware!
			std::lconv* lc           = std::localeconv();
			char        decimalPoint = *lc->decimal_point;
			if (str.find_last_not_of('0') == str.find(decimalPoint))
			{
				offset = 0;
			}
			str.erase(str.find_last_not_of('0') + offset, std::string::npos);
		}
		return str;
	}
} // namespace wpi::units::detail

#endif // defined(UNIT_LIB_ENABLE_IOSTREAM) || __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)

//------------------------------
//	FORWARD DECLARATIONS
//------------------------------

namespace wpi::units
{
	struct linear_scale;
	struct decibel_scale;

	template<class Unit>
	struct unit_name
	{
		static constexpr const char* value = nullptr;
	};

	template<class Unit>
	struct unit_abbreviation
	{
		static constexpr const char* value = nullptr;
	};

	template<class Unit>
	inline constexpr const char* unit_name_v = unit_name<Unit>::value;

	template<class Unit>
	inline constexpr const char* unit_abbreviation_v = unit_abbreviation<Unit>::value;

	namespace detail
	{
		inline constexpr UNIT_LIB_DEFAULT_TYPE PI_VAL = 3.14159265358979323846264338327950288419716939937510;
	}

	namespace traits
	{
		template<typename T>
		struct is_dimensionless_unit;
	}
} // namespace wpi::units

//------------------------------
//	MACROS
//------------------------------

/**
 * @def			UNIT_ADD_STRONG_CONVERSION_FACTOR(namespaceName, namePlural, __VA_ARGS__)
 * @brief		Helper macro for generating the boilerplate code generating the tags of a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as aliases for the
 *				unit tag.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 * @note        the purpose of this trait is primarily to improve the readability of
 *              conversion error messages.
 */
#define UNIT_ADD_STRONG_CONVERSION_FACTOR(namespaceName, namePlural, /*conversion factor*/...)                                                                                                         \
	inline namespace namespaceName                                                                                                                                                                     \
	{                                                                                                                                                                                                  \
		/** @name ConversionFactor (full names plural) */ /** @{ */ struct namePlural##_ : __VA_ARGS__                                                                                                 \
		{                                                                                                                                                                                              \
		}; /** @} */                                                                                                                                                                                   \
	}                                                                                                                                                                                                  \
	namespace traits                                                                                                                                                                                   \
	{                                                                                                                                                                                                  \
		template<>                                                                                                                                                                                     \
		struct strong<__VA_ARGS__>                                                                                                                                                                     \
		{                                                                                                                                                                                              \
			using type = ::wpi::units::namespaceName::namePlural##_;                                                                                                                                        \
		};                                                                                                                                                                                             \
                                                                                                                                                                                                       \
		template<class ConversionFactor>                                                                                                                                                               \
		using strong_t = typename strong<ConversionFactor>::type;                                                                                                                                      \
	}

/**
 * @def			UNIT_ADD_UNIT_DEFINITION(namespaceName,namePlural)
 * @brief		Macro for generating the boilerplate code for the unit type definition.
 * @details		The macro generates the definition of the unit container types, e.g. `meter`
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 */
#define UNIT_ADD_UNIT_DEFINITION(namespaceName, namePlural, /*conversionFactor*/...)                                                                                                                   \
	inline namespace namespaceName                                                                                                                                                                     \
	{                                                                                                                                                                                                  \
		/** @name Unit Containers */ /** @{ */ UNIT_ADD_SCALED_UNIT_DEFINITION(namePlural, ::wpi::units::linear_scale, __VA_ARGS__) /** @} */                                                               \
	}

/**
 * @def			UNIT_ADD_SCALED_UNIT_DEFINITION(unitName, scale, definition)
 * @brief		Macro for generating the boilerplate code for the scaled unit template definition.
 * @details		The macro generates the definition of the scaled unit templates as a strong type template alias,
 *				e.g. `meters`
 * @param		unitName unit name, e.g. 'meters'
 * @param		scale the non-linear scale template argument of the unit's base
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, wpi::units::dimension::length>`)
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_SCALED_UNIT_DEFINITION(unitName, scale, /*conversionFactor*/...)                                                                                                                      \
	template<class Underlying = UNIT_LIB_DEFAULT_TYPE>                                                                                                                                                 \
	using unitName = ::wpi::units::unit<traits::strong_t<__VA_ARGS__>, Underlying, scale>;
/**
 * @def		UNIT_ADD_NAME(namespaceName,namePlural,abbreviation)
 * @brief		Macro for generating constexpr names/abbreviations for units.
 * @details	The macro generates names for units. E.g. name() of 1_m would be "meter", and
 *				abbreviation would be "m".
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 */
#define UNIT_ADD_NAME(namespaceName, namePlural, abbrev)                                                                                                                                               \
	template<class Underlying>                                                                                                                                                                         \
	struct unit_name<namespaceName::namePlural<Underlying>>                                                                                                                                            \
	{                                                                                                                                                                                                  \
		static constexpr const char* value = #namePlural;                                                                                                                                              \
	};                                                                                                                                                                                                 \
                                                                                                                                                                                                       \
	template<class Underlying>                                                                                                                                                                         \
	struct unit_abbreviation<namespaceName::namePlural<Underlying>>                                                                                                                                    \
	{                                                                                                                                                                                                  \
		static constexpr const char* value = #abbrev;                                                                                                                                                  \
	};

/**
 * @def			UNIT_ADD_LITERALS(namespaceName,namePlural,abbreviation)
 * @brief		Macro for generating user-defined literals for units.
 * @details		The macro generates user-defined literals for units. A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_NO_LITERAL_SUPPORT is defined, the macro does not generate any code
 */
#ifdef UNIT_NO_LITERAL_SUPPORT
#define UNIT_ADD_LITERALS(namespaceName, namePlural, abbreviation)
#else
#define UNIT_ADD_LITERALS(namespaceName, namePlural, abbreviation)                                                                                                                                     \
	namespace literals                                                                                                                                                                                 \
	{                                                                                                                                                                                                  \
		constexpr namespaceName::namePlural<double> operator""_##abbreviation(long double d) noexcept                                                                                                  \
		{                                                                                                                                                                                              \
			return namespaceName::namePlural<double>(static_cast<double>(d));                                                                                                                          \
		}                                                                                                                                                                                              \
		constexpr namespaceName::namePlural<double> operator""_##abbreviation(unsigned long long d) noexcept                                                                                              \
		{                                                                                                                                                                                              \
			return namespaceName::namePlural<double>(static_cast<double>(d));                                                                                                                                \
		}                                                                                                                                                                                              \
	}
#endif

#define UNIT_ADD_CONSTANT(namespaceName, namePlural, abbreviation) static constexpr namespaceName::namePlural abbreviation{1.0};

/**
 * @def			UNIT_ADD(namespaceName, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boilerplate code needed for a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as well as the
 *				appropriately named unit container (e.g. `meter_t`). A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`). It also defines a class-specific
 *				cout function which prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD(namespaceName, namePlural, abbreviation, /*conversionFactor*/...)                                                                                                                     \
	UNIT_ADD_STRONG_CONVERSION_FACTOR(namespaceName, namePlural, __VA_ARGS__)                                                                                                                          \
	UNIT_ADD_UNIT_DEFINITION(namespaceName, namePlural, __VA_ARGS__)                                                                                                                                   \
	UNIT_ADD_NAME(namespaceName, namePlural, abbreviation)                                                                                                                                             \
	UNIT_ADD_LITERALS(namespaceName, namePlural, abbreviation)                                                                                                                                         \
	UNIT_ADD_CONSTANT(namespaceName, namePlural, abbreviation)

/**
 * @def			UNIT_ADD_DECIBEL(namespaceName, namePlural, abbreviation)
 * @brief		Macro to create decibel container and literals for an existing unit type.
 * @details		This macro generates the decibel unit container, cout overload, and literal definitions.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural plural version of the dimension name, e.g. 'watts'
 * @param		abbreviation - abbreviated decibel unit name, e.g. 'dBW'
 */
#define UNIT_ADD_DECIBEL(namespaceName, namePlural, abbreviation)                                                                                                                                      \
	inline namespace namespaceName                                                                                                                                                                     \
	{                                                                                                                                                                                                  \
		/** @name Unit Containers */ /** @{ */ UNIT_ADD_SCALED_UNIT_DEFINITION(abbreviation, ::wpi::units::decibel_scale, typename ::wpi::units::namespaceName::namePlural<>::conversion_factor) /** @} */       \
	}                                                                                                                                                                                                  \
	UNIT_ADD_NAME(namespaceName, abbreviation, abbreviation)                                                                                                                                           \
	UNIT_ADD_LITERALS(namespaceName, abbreviation, abbreviation)

/**
 * @def			UNIT_ADD_DIMENSION_TRAIT(unitdimension)
 * @brief		Macro to create the `is_dimension_unit` type trait.
 * @details		This trait allows users to test whether a given type matches
 *				an intended dimension. This macro comprises all the boilerplate
 *				code necessary to do so.
 * @param		unitdimension The name of the dimension of unit, e.g. length or mass.
 */

#define UNIT_ADD_DIMENSION_TRAIT(unitdimension)                                                                                                                                                        \
	/** @ingroup	TypeTraits*/                                                                                                                                                                       \
	/** @brief		`UnaryTypeTrait` for querying whether `T` represents a unit of unitdimension*/                                                                                                     \
	/** @details	The base characteristic is a specialization of the template `std::bool_constant`.*/                                                                                                \
	/**				Use `is_ ## unitdimension ## _unit_v<T>` to test the unit represents a unitdimension quantity.*/                                                                                   \
	/** @tparam		T	type to test*/                                                                                                                                                                 \
	namespace traits                                                                                                                                                                                   \
	{                                                                                                                                                                                                  \
		template<typename T>                                                                                                                                                                           \
		struct is_##unitdimension##_unit : ::wpi::units::detail::has_dimension_of<std::decay_t<T>, wpi::units::dimension::unitdimension>                                                                         \
		{                                                                                                                                                                                              \
		};                                                                                                                                                                                             \
		template<typename T>                                                                                                                                                                           \
		inline constexpr bool is_##unitdimension##_unit_v = is_##unitdimension##_unit<T>::value;                                                                                                       \
	}                                                                                                                                                                                                  \
	template<typename T>                                                                                                                                                                               \
	concept unitdimension##_unit = traits::is_##unitdimension##_unit_v<T>;

/**
 * @def			UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boilerplate code needed for a new unit, including its metric
 *				prefixes from femto to peta.
 * @details		See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `meters` and
 *				'meter_t', it also creates corresponding units with metric suffixes such as `millimeters`, and
 *				`millimeter_t`), as well as the literal suffixes (e.g. `10.0_mm`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, namePlural, abbreviation, /*conversionFactor*/...)                                                                                                \
	UNIT_ADD(namespaceName, namePlural, abbreviation, __VA_ARGS__)                                                                                                                                     \
	UNIT_ADD(namespaceName, femto##namePlural, f##abbreviation, femto<namePlural<>>)                                                                                                                   \
	UNIT_ADD(namespaceName, pico##namePlural, p##abbreviation, pico<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, nano##namePlural, n##abbreviation, nano<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, micro##namePlural, u##abbreviation, micro<namePlural<>>)                                                                                                                   \
	UNIT_ADD(namespaceName, milli##namePlural, m##abbreviation, milli<namePlural<>>)                                                                                                                   \
	UNIT_ADD(namespaceName, centi##namePlural, c##abbreviation, centi<namePlural<>>)                                                                                                                   \
	UNIT_ADD(namespaceName, deci##namePlural, d##abbreviation, deci<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, deca##namePlural, da##abbreviation, deca<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, hecto##namePlural, h##abbreviation, hecto<namePlural<>>)                                                                                                                   \
	UNIT_ADD(namespaceName, kilo##namePlural, k##abbreviation, kilo<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, mega##namePlural, M##abbreviation, mega<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, giga##namePlural, G##abbreviation, giga<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, tera##namePlural, T##abbreviation, tera<namePlural<>>)                                                                                                                     \
	UNIT_ADD(namespaceName, peta##namePlural, P##abbreviation, peta<namePlural<>>)

/**
 * @def		UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(namespaceName, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boilerplate code needed for a new unit, including its metric
 *				prefixes from femto to peta, and binary prefixes from kibi to exbi.
 * @details	See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `bytes` and 'byte_t',
 *				it also creates corresponding units with metric suffixes such as `millimeters`, and `millimeter_t`), as
 *				well as the literal suffixes (e.g. `10.0_B`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `wpi::units::literals` namespace.
 * @param		namePlural - plural version of the unit name, e.g. 'bytes'
 * @param		abbreviation - abbreviated unit name, e.g. 'B'
 * @param       __VA_ARGS__ - the conversion factor definition for the unit type. Taken as variadic
 *              arguments because they contain commas in the macro definition. The complete __VA_ARGS__
 *              represents the full conversion factor type. e.g. `meters<>`.
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(namespaceName, namePlural, abbreviation, /*conversionFactor*/...)                                                                                     \
	UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, namePlural, abbreviation, __VA_ARGS__)                                                                                                                \
	UNIT_ADD(namespaceName, kibi##namePlural, Ki##abbreviation, kibi<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, mebi##namePlural, Mi##abbreviation, mebi<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, gibi##namePlural, Gi##abbreviation, gibi<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, tebi##namePlural, Ti##abbreviation, tebi<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, pebi##namePlural, Pi##abbreviation, pebi<namePlural<>>)                                                                                                                    \
	UNIT_ADD(namespaceName, exbi##namePlural, Ei##abbreviation, exbi<namePlural<>>)

//--------------------
//	UNITS NAMESPACE
//--------------------

/**
 * @namespace wpi::units
 * @brief Unit Conversion Library namespace
 */
namespace wpi::units
{
	//----------------------------------
	//	DOXYGEN
	//----------------------------------

	/**
	 * @defgroup	Units Unit API
	 */

	/**
	 * @defgroup	UnitTypes Unit Types
	 * @ingroup		Units
	 * @brief		Defines a series of classes which contain dimensioned values. Unit types
	 *				store a value, and support various arithmetic operations.
	 */

	/**
	 * @defgroup	UnitManipulators Unit Manipulators
	 * @ingroup		Units
	 * @brief		Defines a series of classes used to manipulate unit types, such as `inverse<>`, `squared<>`, and
	 *				metric prefixes. Unit manipulators can be chained together, e.g.
	 *				`inverse<squared<pico<time::seconds>>>` to represent picoseconds^-2.
	 */

	/**
	 * @defgroup	UnitMath Unit Math
	 * @ingroup		Units
	 * @brief		Defines a collection of unit-enabled, strongly-typed versions of `<cmath>` functions.
	 * @details		Includes most c++11 extensions.
	 */

	/**
	 * @defgroup	Conversion Explicit Conversion
	 * @ingroup		Units
	 * @brief		Functions used to convert values of one logical type to another.
	 */

	/**
	 * @defgroup	TypeTraits Type Traits
	 * @ingroup		Units
	 * @brief		Defines a series of classes to obtain unit type information at compile-time.
	 */

	/**
	 * @defgroup	STDTypeTraits Standard Type Traits Specializations
	 * @ingroup		Units
	 * @brief		Specialization of `std::common_type` for unit types.
	 */

	/**
	 * @namespace	wpi::units::literals
	 * @brief		namespace for unit literal definitions of all categories.
	 * @details		Literals allow using unit values by suffixing numbers. For example, a value
	 *				of `meter_t<double>(6.2)` could be used as `6.2_m`. All literals use an underscore
	 *				followed by the abbreviation for the unit. To enable literal syntax in your code,
	 *				include the statement `using namespace wpi::units::literals;`.
	 * @anchor		unitLiterals
	 * @sa			See unit for more information on unit type containers.
	 */

	//------------------------------
	//	DETECTION IDIOM
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		Detection idiom implementation.
		 * @details		Simplifies the implementation of traits and other metaprogramming use-cases.
		 *				The result is shorter and more expressive code.
		 * @sa			https://wg21.link/N4502, http://wg21.link/N4758#meta.detect
		 */
		template<class Default, class AlwaysVoid, template<class...> class Op, class... Args>
		struct detector
		{
			using value_t = std::false_type;
			using type    = Default;
		};

		template<class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
		{
			using value_t = std::true_type;
			using type    = Op<Args...>;
		};

		struct nonesuch
		{
			nonesuch()                      = delete;
			~nonesuch()                     = delete;
			nonesuch(const nonesuch&)       = delete;
			void operator=(const nonesuch&) = delete;
		};

		template<template<class...> class Op, class... Args>
		using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

		template<template<class...> class Op, class... Args>
		inline constexpr bool is_detected_v = is_detected<Op, Args...>::value;

		template<template<class...> class Op, class... Args>
		using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

		template<class Default, template<class...> class Op, class... Args>
		using detected_or = detector<Default, void, Op, Args...>;

		template<class Default, template<class...> class Op, class... Args>
		using detected_or_t = typename detected_or<Default, Op, Args...>::type;

		template<class Expected, template<class...> class Op, class... Args>
		using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

		template<class Expected, template<class...> class Op, class... Args>
		inline constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

		template<class To, template<class...> class Op, class... Args>
		using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

		template<class To, template<class...> class Op, class... Args>
		inline constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	//------------------------------
	//	RATIO TRAITS
	//------------------------------

	/**
	 * @ingroup TypeTraits
	 * @{
	 */

	namespace traits
	{
		/** @cond */ // DOXYGEN IGNORE
		namespace detail
		{
			template<class T>
			struct is_ratio_impl : std::false_type
			{
			};

			template<std::intmax_t N, std::intmax_t D>
			struct is_ratio_impl<std::ratio<N, D>> : std::true_type
			{
			};
		} // namespace detail
		/** @endcond */ // END DOXYGEN IGNORE

		/**
		 * @brief		`UnaryTypeTrait` for querying whether `T` represents a specialization of `std::ratio`.
		 * @details		The base characteristic is a specialization of the template `std::bool_constant`.
		 *				Use `is_ratio_v<T>` to test whether `T` is a specialization of `std::ratio`.
		 */
		template<class T>
		using is_ratio = detail::is_ratio_impl<T>;

		template<class T>
		inline constexpr bool is_ratio_v = is_ratio<T>::value;
	} // namespace traits

	//------------------------------
	//	CONVERSION FACTOR TRAITS
	//------------------------------

	/**
	 * @brief namespace representing type traits which can access the properties of types provided by the units library.
	 */
	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSES_ONLY
		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits class defining the properties of units.
		 * @details		The units library determines certain properties of the units passed to
		 *				them and what they represent by using the members of the corresponding
		 *				conversion_factor_traits instantiation.
		 */
		template<class T>
		struct conversion_factor_traits
		{
			typedef typename T::dimension_type dimension_type;       ///< Unit type that the unit was derived from. May be a `dimension` or another
																	 ///< `conversion_factor`. Use the `dimension_of_t` trait to find the SI dimension type.
																	 ///< This will be `void` if type `T` is not a unit.
			typedef typename T::conversion_ratio conversion_ratio;   ///< `std::ratio` representing the conversion factor to the `dimension_type`. This
																	 ///< will be `void` if type `T` is not a unit.
			typedef typename T::pi_exponent_ratio pi_exponent_ratio; ///< `std::ratio` representing the exponent of pi to be used in the conversion.
																	 ///< This will be `void` if type `T` is not a unit.
			typedef typename T::translation_ratio translation_ratio; ///< `std::ratio` representing a datum translation to the dimension (i.e. degrees
																	 ///< C to degrees F conversion). This will be `void` if type `T` is not a unit.
		};
#endif
		/** @cond */ // DOXYGEN IGNORE
		/**
		 * @brief		unit traits implementation for classes which are not units.
		 */
		template<class T, typename = void>
		struct conversion_factor_traits
		{
			using dimension_type    = void;
			using conversion_ratio  = void;
			using pi_exponent_ratio = void;
			using translation_ratio = void;
		};

		template<class T>
		struct conversion_factor_traits<T, std::void_t<typename T::dimension_type, typename T::conversion_ratio, typename T::pi_exponent_ratio, typename T::translation_ratio>>
		{
			using dimension_type = typename T::dimension_type;       ///< Unit type that the unit was derived from. May be a `dimension` or
																	 ///< another `conversion_factor`. Use the `dimension_of_t` trait to find the
																	 ///< SI dimension type. This will be `void` if type `T` is not a unit.
			using conversion_ratio = typename T::conversion_ratio;   ///< `std::ratio` representing the conversion factor to the
																	 ///< `dimension_type`. This will be `void` if type `T` is not a unit.
			using pi_exponent_ratio = typename T::pi_exponent_ratio; ///< `std::ratio` representing the exponent of pi to be used in the
																	 ///< conversion. This will be `void` if type `T` is not a unit.
			using translation_ratio = typename T::translation_ratio; ///< `std::ratio` representing a datum translation to the dimension (i.e.
																	 ///< degrees C to degrees F conversion). This will be `void` if type `T`
																	 ///< is not a unit.
		};

		/** @endcond */ // END DOXYGEN IGNORE
	} // namespace traits

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify conversion factors.
		 * @details		A non-templated base class for `conversion_factor` which enables compile-time testing.
		 */
		struct _conversion_factor
		{
		};
	} // namespace detail

	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		`UnaryTypeTrait` for querying whether `T` represents a conversion factor.
		 * @details		The base characteristic is a specialization of the template `std::bool_constant`.
+		 *				Use `is_conversion_factor_v<T>` to test whether `T` represents a conversion factor.
		 */
		template<class T>
		using is_conversion_factor = typename std::is_base_of<wpi::units::detail::_conversion_factor, T>::type;

		template<class T>
		inline constexpr bool is_conversion_factor_v = is_conversion_factor<T>::value;
	} // namespace traits

	/** @} */ // end of TypeTraits

	//------------------------------
	//	UNIT TRAITS
	//------------------------------

	namespace detail
	{
		/**
		 * @brief		helper type to identify units.
		 * @details		A non-templated base class for `unit` which enables compile-time testing.
		 */
		struct _unit
		{
		};
	} // namespace detail

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits which tests if a class is a `unit`
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit_v<T>` to test
		 *				whether `class T` implements a `unit`.
		 */
		template<class T>
		struct is_unit : std::is_base_of<wpi::units::detail::_unit, T>::type
		{
		};

		template<class T>
		inline constexpr bool is_unit_v = is_unit<T>::value && !std::is_arithmetic_v<T>;

		/** @cond */ // DOXYGEN IGNORE
		namespace detail
		{
			template<class NumericalScale>
			struct invocable_scale
			{
				template<class T>
					requires std::is_same_v<decltype(NumericalScale::linearize(T{})), decltype(NumericalScale::scale(T{}))>
				decltype(NumericalScale::scale(T{})) operator()(T)
				{
					return scale(T{});
				}
			};
		} // namespace detail
		/** @endcond */ // END DOXYGEN IGNORE

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether `T` meets the requirements for a numerical scale
		 * @details		A numerical scale must have static member functions named `linearize` and `scale`
		 *				that take one `Ret` argument and return a `Ret` value, where
		 *				`linearize` returns the linearized input value and
		 *				`scale` returns the scaled input value.
		 *
		 *				Numerical scales are used by `wpi::units::unit` to linearize and scale values
		 *				if they represent things like dB.
		 */
		template<class T, class Ret>
		using is_numerical_scale = std::is_invocable_r<Ret, detail::invocable_scale<T>, Ret>;

		template<class T, class Ret>
		inline constexpr bool is_numerical_scale_v = is_numerical_scale<T, Ret>::value;
	} // namespace traits

	//------------------------------
	//	CONCEPTS
	//------------------------------

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent arithmetic types
	 */
	template<typename T>
	concept ArithmeticType = std::is_arithmetic_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent non-arithmetic types
	 */
	template<typename T>
	concept NonArithmeticType = !std::is_arithmetic_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent std::ratios
	 */
	template<typename T>
	concept RatioType = traits::is_ratio_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent conversion factors
	 */
	template<typename T>
	concept ConversionFactorType = traits::is_conversion_factor_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent numerical scales
	 */
	template<typename Scale, typename T>
	concept NumericalScaleType = traits::is_numerical_scale_v<Scale, T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent units
	 */
	template<typename T>
	concept UnitType = traits::is_unit_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent units with a dimension (i.e. not dimensionless)
	 */
	template<typename T>
	concept DimensionedUnitType = traits::is_unit_v<T> && !traits::is_dimensionless_unit<T>::value;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent units without a dimension (dimensionless)
	 */
	template<typename T>
	concept DimensionlessUnitType = traits::is_unit_v<T> && traits::is_dimensionless_unit<T>::value;

	namespace traits
	{
		// forward declaration
		template<UnitType U1, UnitType U2>
		struct is_same_dimension_unit;
	} // namespace traits

	/**
	 * @ingroup		Concepts
	 * @brief		Concept for types which represent units of the same dimensionality
	 */
	template<typename UnitTo, typename UnitFrom>
	concept same_dimension = traits::is_same_dimension_unit<UnitFrom, UnitTo>::value;

	//------------------------------
	//	STRONG UNIT TYPES
	//------------------------------

	namespace traits
	{
		/**
		 * @ingroup			TypeTraits
		 * @brief			SFINAE-able trait that maps a `conversion_factor` to its strengthened type.
		 * @details			If `T` is a cv-unqualified `conversion_factor`, the member `type` alias names the
		 *					strong type alias of `T`, if any, and `T` otherwise. Otherwise, there is no `type` member.
		 *					This may be specialized only if `T` depends on a program-defined type.
		 */
		template<ConversionFactorType T>
			requires std::is_same_v<T, std::remove_cv_t<T>>
		struct strong : T
		{
			typedef T type;
		};

		template<class T>
		using strong_t = typename strong<T>::type;
	} // namespace traits

	//------------------------------
	//	DIMENSIONS
	//------------------------------
	// see: https://github.com/swatanabe/cppnow17-units
	// license for this code: https://github.com/swatanabe/cppnow17-units/blob/master/LICENSE_1_0.txt
	//------------------------------

	template<class D, class E>
	struct dim
	{
		using dimension = D;
		using exponent  = E;
	};

	template<class... D>
	struct dimension_t;

	template<>
	struct dimension_t<>
	{
		static constexpr bool empty = true;
	};

	template<class D0, class... D>
	struct dimension_t<D0, D...>
	{
		static constexpr bool empty = false;
		using front                 = D0;
		using pop_front             = dimension_t<D...>;
	};

	template<class T, class U>
	using combine_dims = dim<typename T::dimension, std::ratio_add<typename T::exponent, typename U::exponent>>;

	template<int Test>
	struct merge_dimensions_impl;

	constexpr int const_strcmp(const char* lhs, const char* rhs)
	{
		return (*lhs && *rhs) ? (*lhs == *rhs ? const_strcmp(lhs + 1, rhs + 1) : (*lhs < *rhs ? -1 : 1)) : ((!*lhs && !*rhs) ? 0 : (!*lhs ? -1 : 1));
	}

	template<bool HasT, bool HasU>
	struct merge_dimensions_recurse_impl;

	template<>
	struct merge_dimensions_recurse_impl<true, true>
	{
		template<class T, class U, class... R>
		using apply = typename merge_dimensions_impl<const_strcmp(T::front::dimension::name, U::front::dimension::name)>::template apply<T, U, R...>;
	};

	template<class T, class U>
	struct append;

	template<class... T, class... U>
	struct append<dimension_t<T...>, dimension_t<U...>>
	{
		using type = dimension_t<T..., U...>;
	};

	template<>
	struct merge_dimensions_recurse_impl<true, false>
	{
		template<class T, class U, class... R>
		using apply = typename append<dimension_t<R...>, T>::type;
	};

	template<>
	struct merge_dimensions_recurse_impl<false, true>
	{
		template<class T, class U, class... R>
		using apply = typename append<dimension_t<R...>, U>::type;
	};

	template<>
	struct merge_dimensions_recurse_impl<false, false>
	{
		template<class T, class U, class... R>
		using apply = dimension_t<R...>;
	};

	template<class T, class U, class... R>
	using merge_dimensions_recurse = typename merge_dimensions_recurse_impl<!T::empty, !U::empty>::template apply<T, U, R...>;

	template<>
	struct merge_dimensions_impl<1>
	{
		template<class T, class U, class... R>
		using apply = merge_dimensions_recurse<T, typename U::pop_front, R..., typename U::front>;
	};

	template<>
	struct merge_dimensions_impl<-1>
	{
		template<class T, class U, class... R>
		using apply = merge_dimensions_recurse<typename T::pop_front, U, R..., typename T::front>;
	};

	template<bool Cancels>
	struct merge_dimensions_combine_impl;

	template<>
	struct merge_dimensions_combine_impl<true>
	{
		template<class T, class U, class X, class... R>
		using apply = merge_dimensions_recurse<T, U, R...>;
	};

	template<>
	struct merge_dimensions_combine_impl<false>
	{
		template<class T, class U, class X, class... R>
		using apply = merge_dimensions_recurse<T, U, R..., X>;
	};

	template<>
	struct merge_dimensions_impl<0>
	{
		template<class T, class U, class... R>
		using apply = typename merge_dimensions_combine_impl<std::ratio_add<typename T::front::exponent, typename U::front::exponent>::num == 0>::template apply<typename T::pop_front,
			typename U::pop_front, dim<typename T::front::dimension, std::ratio_add<typename T::front::exponent, typename U::front::exponent>>, R...>;
	};

	template<class T, class U>
	using merge_dimensions = merge_dimensions_recurse<T, U>;

	template<class T, class E>
	struct dimension_pow_impl;

	template<class... T, class... E, class R>
	struct dimension_pow_impl<dimension_t<dim<T, E>...>, R>
	{
		using type = dimension_t<dim<T, std::ratio_multiply<E, R>>...>;
	};

	template<class T, class E>
	using dimension_pow = typename dimension_pow_impl<T, E>::type;

	template<class T, class E>
	using dimension_root = dimension_pow<T, std::ratio_divide<std::ratio<1>, E>>;

	template<class T, class U>
	using dimension_multiply = merge_dimensions<T, U>;

	template<class T, class U>
	using dimension_divide = merge_dimensions<T, dimension_pow<U, std::ratio<-1>>>;

	template<class T0 = void, class N0 = std::ratio<1>, class... Rest>
	struct make_dimension_list
	{
		using type = dimension_multiply<dimension_t<dim<T0, N0>>, typename make_dimension_list<Rest...>::type>;
	};

	template<class... T, class N0, class... Rest>
	struct make_dimension_list<dimension_t<T...>, N0, Rest...>
	{
		using type = dimension_multiply<dimension_pow<dimension_t<T...>, N0>, typename make_dimension_list<Rest...>::type>;
	};

	template<>
	struct make_dimension_list<>
	{
		using type = dimension_t<>;
	};

	template<class... T>
	using make_dimension = typename make_dimension_list<T...>::type;

	//------------------------------
	//	UNIT DIMENSIONS
	//------------------------------

	/**
	 * @brief		namespace representing the implemented base and derived unit types. These will not generally be
	 *				needed by library users.
	 * @sa			dimension for the definition of the dimension parameters.
	 */
	namespace dimension
	{
		// DIMENSION TAGS
		struct length_tag
		{
			static constexpr auto name         = "length";
			static constexpr auto abbreviation = "m";
		};

		struct mass_tag
		{
			static constexpr auto name         = "mass";
			static constexpr auto abbreviation = "kg";
		};

		struct time_tag
		{
			static constexpr auto name         = "time";
			static constexpr auto abbreviation = "s";
		};

		struct current_tag
		{
			static constexpr auto name         = "current";
			static constexpr auto abbreviation = "A";
		};

		struct temperature_tag
		{
			static constexpr auto name         = "temperature";
			static constexpr auto abbreviation = "K";
		};

		struct substance_tag
		{
			static constexpr auto name         = "amount of substance";
			static constexpr auto abbreviation = "mol";
		};

		struct luminous_intensity_tag
		{
			static constexpr auto name         = "luminous intensity";
			static constexpr auto abbreviation = "cd";
		};

		struct angle_tag
		{
			static constexpr auto name         = "angle";
			static constexpr auto abbreviation = "rad";
		};

		struct data_tag
		{
			static constexpr auto name         = "data";
			static constexpr auto abbreviation = "byte";
		};

		// SI BASE UNITS
		using length             = make_dimension<length_tag>;
		using mass               = make_dimension<mass_tag>;
		using time               = make_dimension<time_tag>;
		using current            = make_dimension<current_tag>;
		using temperature        = make_dimension<temperature_tag>;
		using substance          = make_dimension<substance_tag>;
		using luminous_intensity = make_dimension<luminous_intensity_tag>;

		// dimensionless (DIMENSIONLESS) TYPES
		using dimensionless = dimension_t<>;             ///< Represents a quantity with no dimension.
		using angle         = make_dimension<angle_tag>; ///< Represents a quantity of angle

		// SI DERIVED UNIT TYPES
		using solid_angle             = dimension_pow<angle, std::ratio<2>>;                                                ///< Represents an SI derived unit of solid angle
		using frequency               = make_dimension<time, std::ratio<-1>>;                                               ///< Represents an SI derived unit of frequency
		using velocity                = dimension_divide<length, time>;                                                     ///< Represents an SI derived unit of velocity
		using angular_velocity        = dimension_divide<angle, time>;                                                      ///< Represents an SI derived unit of angular velocity
		using acceleration            = dimension_divide<velocity, time>;                                                   ///< Represents an SI derived unit of acceleration
		using angular_acceleration    = dimension_divide<angular_velocity, time>;                                           ///< Represents an SI derived unit of angular acceleration
		using force                   = dimension_multiply<mass, acceleration>;                                             ///< Represents an SI derived unit of force
		using area                    = dimension_pow<length, std::ratio<2>>;                                               ///< Represents an SI derived unit of area
		using volume                  = dimension_pow<length, std::ratio<3>>;                                               ///< Represents an SI derived unit of volume
		using pressure                = dimension_divide<force, area>;                                                      ///< Represents an SI derived unit of pressure
		using charge                  = dimension_multiply<time, current>;                                                  ///< Represents an SI derived unit of charge
		using energy                  = dimension_multiply<force, length>;                                                  ///< Represents an SI derived unit of energy
		using power                   = dimension_divide<energy, time>;                                                     ///< Represents an SI derived unit of power
		using voltage                 = dimension_divide<power, current>;                                                   ///< Represents an SI derived unit of voltage
		using capacitance             = dimension_divide<charge, voltage>;                                                  ///< Represents an SI derived unit of capacitance
		using impedance               = dimension_divide<voltage, current>;                                                 ///< Represents an SI derived unit of impedance
		using conductance             = dimension_divide<current, voltage>;                                                 ///< Represents an SI derived unit of conductance
		using magnetic_flux           = dimension_divide<energy, current>;                                                  ///< Represents an SI derived unit of magnetic flux
		using inductance              = dimension_multiply<impedance, time>;                                                ///< Represents an SI derived unit of inductance
		using luminous_flux           = dimension_multiply<solid_angle, luminous_intensity>;                                ///< Represents an SI derived unit of luminous flux
		using illuminance             = make_dimension<luminous_flux, std::ratio<1>, length, std::ratio<-2>>;               ///< Represents an SI derived unit of illuminance
		using luminance               = make_dimension<luminous_intensity, std::ratio<1>, length, std::ratio<-2>>;          ///< Represents an SI derived unit of luminance
		using radioactivity           = make_dimension<length, std::ratio<2>, time, std::ratio<-2>>;                        ///< Represents an SI derived unit of radioactivity
		using substance_mass          = dimension_divide<mass, substance>;                                                  ///< Represents an SI derived unit of substance mass
		using substance_concentration = dimension_divide<substance, mass>;                                                  ///< Represents an SI derived unit of substance concentration
		using magnetic_field_strength = make_dimension<mass, std::ratio<1>, time, std::ratio<-2>, current, std::ratio<-1>>; ///< Represents an SI derived unit of magnetic field strength
		using radiant_intensity       = make_dimension<power, std::ratio<1>, solid_angle, std::ratio<-1>>;                  ///< Represents an SI derived unit of radiant intensity
		using radiance                = make_dimension<radiant_intensity, std::ratio<1>, area, std::ratio<-1>>;             ///< Represents an SI derived unit of radiance
		using irradiance              = make_dimension<power, std::ratio<1>, area, std::ratio<-1>>;                         ///< Represents an SI derived unit of irradiance
		using spectral_intensity      = make_dimension<radiant_intensity, std::ratio<1>, length, std::ratio<-1>>;           ///< Represents an SI derived unit of spectral intensity
		using spectral_flux           = make_dimension<power, std::ratio<1>, length, std::ratio<-1>>;                       ///< Represents an SI derived unit of spectral flux
		using spectral_radiance       = make_dimension<radiant_intensity, std::ratio<1>, volume, std::ratio<-1>>;           ///< Represents an SI derived unit of spectral intensity
		using spectral_irradiance     = make_dimension<power, std::ratio<1>, volume, std::ratio<-1>>;                       ///< Represents an SI derived unit of spectral irradiance

		// OTHER UNIT TYPES
		using jerk               = make_dimension<length, std::ratio<1>, time, std::ratio<-3>>;   ///< Represents an SI derived unit of jerk
		using angular_jerk       = make_dimension<angle, std::ratio<1>, time, std::ratio<-3>>;    ///< Represents an SI derived unit of angular jerk
		using torque             = dimension_multiply<force, length>;                             ///< Represents an SI derived unit of torque
		using density            = dimension_divide<mass, volume>;                                ///< Represents an SI derived unit of density
		using energy_density     = make_dimension<energy, std::ratio<1>, volume, std::ratio<-1>>; ///< Represents an SI derived unit of energy density
		using concentration      = make_dimension<volume, std::ratio<-1>>;                        ///< Represents a unit of concentration
		using data               = make_dimension<data_tag>;                                      ///< Represents a unit of data size
		using data_transfer_rate = dimension_divide<data, time>;                                  ///< Represents a unit of data transfer rate
	} // namespace dimension

	//------------------------------
	//	CONVERSION FACTOR CLASSES
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	/**
	 * @brief		unit type template specialization for units derived from dimensions.
	 */
	template<RatioType, class, RatioType, RatioType>
	struct conversion_factor;

	template<RatioType Conversion, class... Exponents, RatioType PiExponent, RatioType Translation>
	struct conversion_factor<Conversion, dimension_t<Exponents...>, PiExponent, Translation> : detail::_conversion_factor
	{
		using dimension_type    = dimension_t<Exponents...>;
		using conversion_ratio  = Conversion;
		using translation_ratio = Translation;
		using pi_exponent_ratio = PiExponent;
	};

	/** @endcond */ // END DOXYGEN IGNORE

	/** @cond */    // DOXYGEN IGNORE
	namespace detail
	{
		template<RatioType C, typename U, RatioType P, RatioType T>
		conversion_factor<C, U, P, T> conversion_factor_base_t_impl(conversion_factor<C, U, P, T>* cf)
		{
			return *cf;
		};

		template<typename T>
		using conversion_factor_base_t = decltype(conversion_factor_base_t_impl(std::declval<T*>()));

		/**
		 * @brief		dimension_of_t trait implementation
		 * @details		recursively seeks dimension type of conversion factor.
		 *				Since their `dimension_type` typedef may be another conversion factor,
		 *				it may not represent a dimension type.
		 */
		template<class ConversionFactor>
		struct dimension_of_impl : dimension_of_impl<conversion_factor_base_t<ConversionFactor>>
		{
		};

		template<RatioType Conversion, class BaseUnit, RatioType PiExponent, RatioType Translation>
		struct dimension_of_impl<conversion_factor<Conversion, BaseUnit, PiExponent, Translation>> : dimension_of_impl<BaseUnit>
		{
		};

		template<class... Exponents>
		struct dimension_of_impl<dimension_t<Exponents...>>
		{
			using type = dimension_t<Exponents...>;
		};

		template<>
		struct dimension_of_impl<void>
		{
			using type = void;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @brief		Names the `dimension_t` of a `conversion_factor`.
		 * @details		Since `conversion_factor`s nest,
		 *				their `dimension_type` typedef will not always be a `dimension_t` (or unit dimension).
		 */
		template<class U>
		using dimension_of_t = typename wpi::units::detail::dimension_of_impl<U>::type;
	} // namespace traits

	template<ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
	class unit;

	namespace detail
	{
		template<typename T, class Dim, bool IsConv = false>
		struct has_dimension_of_impl : std::false_type
		{
		};

		template<typename T, class Dim>
		using has_dimension_of = typename has_dimension_of_impl<T, Dim, traits::is_conversion_factor_v<T>>::type;

		template<typename Cf, class Dim>
		struct has_dimension_of_impl<Cf, Dim, true> : has_dimension_of<conversion_factor_base_t<Cf>, Dim>::type
		{
		};

		template<typename C, typename Cf, typename P, typename T, class Dim>
		struct has_dimension_of_impl<conversion_factor<C, Cf, P, T>, Dim, true> : std::is_same<typename conversion_factor<C, Cf, P, T>::dimension_type, Dim>::type
		{
		};

		template<typename Cf, typename T, class Ns, class Dim>
		struct has_dimension_of_impl<unit<Cf, T, Ns>, Dim> : std::is_same<traits::dimension_of_t<Cf>, Dim>::type
		{
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		SFINAE-able trait which replaces the underlying type of `Unit` with `Underlying`.
		 * @details		If `Unit` is an unit, the member `type` alias names the same unit with an underlying type of
		 *				`Underlying`. Otherwise, there is no `type` member.
		 * @param		Unit The unit type whose underlying type is to be replaced.
		 * @param		Underlying The underlying type to replace that of `Unit`.
		 */
		template<class, class>
		struct replace_underlying
		{
		};

		template<ConversionFactorType Cf, ArithmeticType T, NumericalScaleType<T> Ns, ArithmeticType Underlying>
		struct replace_underlying<unit<Cf, T, Ns>, Underlying>
		{
			using type = unit<Cf, Underlying, Ns>;
		};

		template<class Unit, class Underlying>
		using replace_underlying_t = typename replace_underlying<Unit, Underlying>::type;
	} // namespace traits

	/**
	 * @brief		Type representing an arbitrary conversion factor between units.
	 * @ingroup		ConversionFactor
	 * @details		`conversion_factor`s are used as tags for the `convert` function.
	 *				Each unit is defined by:
	 *
	 *				- A `std::ratio` defining the conversion factor to the dimension type. (e.g. `std::ratio<1,12>` for
	 *					inches to feet)
	 *				- A dimension that the unit is derived from (or a unit dimension. Must be of type
	 *				`conversion_factor` or `dimension_t`)
	 *				- An exponent representing factors of PI required by the conversion. (e.g. `std::ratio<-1>` for a
	 *					radians to degrees conversion)
	 *				- a ratio representing a datum translation required for the conversion (e.g. `std::ratio<32>` for a
	 *					Fahrenheit to Celsius conversion)
	 *
	 *				Typically, a specific conversion factor, like `meters`,
	 *				would be implemented as a strong type alias of `conversion_factor`, i.e.
	 *				`struct meters : conversion_factor<std::ratio<1>, wpi::units::dimension::length> {};`,
	 *				or type alias, i.e. `using inches = conversion_factor<std::ratio<1,12>, feet>`.
	 * @tparam		Conversion	std::ratio representing dimensionless multiplication factor.
	 * @tparam		BaseUnit	Unit type which this unit is derived from. May be a `dimension_t`, or another
	 *				`conversion_factor`.
	 * @tparam		PiExponent	std::ratio representing the exponent of pi required by the conversion.
	 * @tparam		Translation	std::ratio representing any datum translation required by the conversion.
	 */
	template<RatioType Conversion, class BaseUnit, RatioType PiExponent = std::ratio<0>, RatioType Translation = std::ratio<0>>
	struct conversion_factor : detail::_conversion_factor
	{
		using dimension_type    = traits::dimension_of_t<BaseUnit>;
		using conversion_ratio  = std::ratio_multiply<typename BaseUnit::conversion_ratio, Conversion>;
		using pi_exponent_ratio = std::ratio_add<typename BaseUnit::pi_exponent_ratio, PiExponent>;
		using translation_ratio = std::ratio_add<std::ratio_multiply<typename BaseUnit::conversion_ratio, Translation>, typename BaseUnit::translation_ratio>;
	};

	//------------------------------
	//	UNIT MANIPULATORS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `unit_multiply`.
		 * @details		multiplies two units. The dimension becomes the dimensions of each with their exponents
		 *				added. The conversion factors of each are multiplied. Pi exponent ratios
		 *				are added, and datum translations are removed.
		 */
		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		struct unit_multiply_impl
		{
			using type = conversion_factor<std::ratio_multiply<typename Cf1::conversion_ratio, typename Cf2::conversion_ratio>,
				dimension_multiply<traits::dimension_of_t<typename Cf1::dimension_type>, traits::dimension_of_t<typename Cf2::dimension_type>>,
				std::ratio_add<typename Cf1::pi_exponent_ratio, typename Cf2::pi_exponent_ratio>>;
		};

		/**
		 * @brief		represents the type of two units multiplied together.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		using unit_multiply = typename unit_multiply_impl<Cf1, Cf2>::type;

		/**
		 * @brief		implementation of `unit_divide`.
		 * @details		divides two units. The dimension becomes the dimensions of each with their exponents
		 *				subtracted. The conversion factors of each are divided. Pi
		 *				exponent ratios are subtracted, and datum translations are removed.
		 */
		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		struct unit_divide_impl
		{
			using type = conversion_factor<std::ratio_divide<typename Cf1::conversion_ratio, typename Cf2::conversion_ratio>,
				dimension_divide<traits::dimension_of_t<typename Cf1::dimension_type>, traits::dimension_of_t<typename Cf2::dimension_type>>,
				std::ratio_subtract<typename Cf1::pi_exponent_ratio, typename Cf2::pi_exponent_ratio>>;
		};

		/**
		 * @brief		represents the type of two units divided by each other.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		using unit_divide = typename unit_divide_impl<Cf1, Cf2>::type;

		/**
		 * @brief		implementation of `inverse`
		 * @details		inverts a unit (equivalent to 1/unit). The `dimension_t` and pi exponents are all multiplied by
		 *				-1. The conversion ratio numerator and denominator are swapped. Datum translation
		 *				ratios are removed.
		 */
		template<ConversionFactorType Cf>
		struct inverse_impl
		{
			using type = conversion_factor<std::ratio<Cf::conversion_ratio::den, Cf::conversion_ratio::num>, dimension_pow<typename Cf::dimension_type, std::ratio<-1>>,
				std::ratio_multiply<typename Cf::pi_exponent_ratio, std::ratio<-1>>>; // inverses are rates or changes, so translation factor is removed.
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the inverse unit type of `class U`.
	 * @ingroup		UnitManipulators
	 * @tparam		Cf	`unit` type to invert.
	 * @details		E.g. `inverse<meters>` will represent meters^-1 (i.e. 1/meters).
	 */
	template<ConversionFactorType Cf>
	using inverse = typename detail::inverse_impl<Cf>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `squared`
		 * @details		Squares the conversion ratio, `dimension_t` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<ConversionFactorType Cf>
		struct squared_impl
		{
			using Conversion = typename Cf::conversion_ratio;
			using type       = conversion_factor<std::ratio_multiply<Conversion, Conversion>, dimension_pow<traits::dimension_of_t<typename Cf::dimension_type>, std::ratio<2>>,
					  std::ratio_multiply<typename Cf::pi_exponent_ratio, std::ratio<2>>, typename Cf::translation_ratio>;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the unit type of `class U` squared
	 * @ingroup		UnitManipulators
	 * @tparam		Cf	`unit` type to square.
	 * @details		E.g. `square<meters>` will represent meters^2.
	 */
	template<ConversionFactorType Cf>
	using squared = typename detail::squared_impl<Cf>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `cubed`
		 * @details		Cubes the conversion ratio, `dimension` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<ConversionFactorType Cf>
		struct cubed_impl
		{
			using Conversion = typename Cf::conversion_ratio;
			using type       = conversion_factor<std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>,
					  dimension_pow<traits::dimension_of_t<typename Cf::dimension_type>, std::ratio<3>>, std::ratio_multiply<typename Cf::pi_exponent_ratio, std::ratio<3>>, typename Cf::translation_ratio>;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the type of `class U` cubed.
	 * @ingroup		UnitManipulators
	 * @tparam		Cf	`unit` type to cube.
	 * @details		E.g. `cubed<meters>` will represent meters^3.
	 */
	template<ConversionFactorType Cf>
	using cubed = typename detail::cubed_impl<Cf>::type;

	/** @cond */ // DOXYGEN IGNORE
	// clang-format off
	namespace detail
	{
		//----------------------------------
		//	RATIO_SQRT IMPLEMENTATION
		//----------------------------------

		using Zero = std::ratio<0>;
		using One = std::ratio<1>;
		template <RatioType R> using Square = std::ratio_multiply<R, R>;

		// Find the largest std::integer N such that Predicate<N>::value is true.
		template <template <std::intmax_t N> class Predicate, typename = void>
		struct BinarySearch
		{
			template <std::intmax_t N>
			struct SafeDouble_
			{
				static constexpr const std::intmax_t value = 2 * N;
				static_assert(value > 0, "Overflows when computing 2 * N");
			};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition1 = void, typename Condition2 = void>
			struct DoubleSidedSearch_ : DoubleSidedSearch_<Lower, Upper,
				std::integral_constant<bool, (Upper - Lower == 1)>,
				std::integral_constant<bool, ((Upper - Lower>1 && Predicate<Lower + (Upper - Lower) / 2>::value))>> {};

			template <std::intmax_t Lower, std::intmax_t Upper>
			struct DoubleSidedSearch_<Lower, Upper, std::false_type, std::false_type> : DoubleSidedSearch_<Lower, Lower + (Upper - Lower) / 2> {};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition2>
			struct DoubleSidedSearch_<Lower, Upper, std::true_type, Condition2> : std::integral_constant<std::intmax_t, Lower>{};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition1>
			struct DoubleSidedSearch_<Lower, Upper, Condition1, std::true_type> : DoubleSidedSearch_<Lower + (Upper - Lower) / 2, Upper>{};

			template <std::intmax_t Lower, class = void>
			struct SingleSidedSearch_ : SingleSidedSearch_<Lower, std::integral_constant<bool, Predicate<SafeDouble_<Lower>::value>::value>>{};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::false_type> : DoubleSidedSearch_<Lower, SafeDouble_<Lower>::value> {};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::true_type> : SingleSidedSearch_<SafeDouble_<Lower>::value>{};

			static constexpr std::intmax_t value = SingleSidedSearch_<1>::value;
 		};

		template <template <std::intmax_t N> class Predicate>
		struct BinarySearch<Predicate, std::enable_if_t<!Predicate<1>::value>> : std::integral_constant<std::intmax_t, 0>{};

		// Find largest std::integer N such that N<=sqrt(R)
		template <typename R>
		struct Integer
		{
			template <std::intmax_t N> using Predicate_ = std::ratio_less_equal<std::ratio<N>, std::ratio_divide<R, std::ratio<N>>>;
			static constexpr const std::intmax_t value = BinarySearch<Predicate_>::value;
		};

		template <typename R>
		struct IsPerfectSquare
		{
			static constexpr const std::intmax_t DenSqrt_ = Integer<std::ratio<R::den>>::value;
			static constexpr const std::intmax_t NumSqrt_ = Integer<std::ratio<R::num>>::value;
			static constexpr const bool value =( DenSqrt_ * DenSqrt_ == R::den && NumSqrt_ * NumSqrt_ == R::num);
			using Sqrt = std::ratio<NumSqrt_, DenSqrt_>;
		};

		// Represents sqrt(P)-Q.
		template <typename Tp, typename Tq>
		struct Remainder
		{
			using P = Tp;
			using Q = Tq;
		};

		// Represents 1/R = I + Rem where R is a Remainder.
		template <typename R>
		struct Reciprocal
		{
			using P_ = typename R::P;
			using Q_ = typename R::Q;
			using Den_ = std::ratio_subtract<P_, Square<Q_>>;
			using A_ = std::ratio_divide<Q_, Den_>;
			using B_ = std::ratio_divide<P_, Square<Den_>>;
			static constexpr const std::intmax_t I_ = (A_::num + Integer<std::ratio_multiply<B_, Square<std::ratio<A_::den>>>>::value) / A_::den;
			using I = std::ratio<I_>;
			using Rem = Remainder<B_, std::ratio_subtract<I, A_>>;
		};

		// Expands sqrt(R) to continued fraction:
		// f(x)=C1+1/(C2+1/(C3+1/(...+1/(Cn+x)))) = (U*x+V)/(W*x+1) and sqrt(R)=f(Rem).
		// The error |f(Rem)-V| = |(U-W*V)x/(W*x+1)| <= |U-W*V|*Rem <= |U-W*V|/I' where
		// I' is the std::integer part of reciprocal of Rem.
		template <typename Tr, std::intmax_t N>
		struct ContinuedFraction
		{
			template <typename T>
			using Abs_ = std::conditional_t<std::ratio_less_v<T, Zero>, std::ratio_subtract<Zero, T>, T>;

			using R = Tr;
			using Last_ = ContinuedFraction<R, N - 1>;
			using Reciprocal_ = Reciprocal<typename Last_::Rem>;
			using Rem = typename Reciprocal_::Rem;
			using I_ = typename Reciprocal_::I;
			using Den_ = std::ratio_add<typename Last_::W, I_>;
			using U = std::ratio_divide<typename Last_::V, Den_>;
			using V = std::ratio_divide<std::ratio_add<typename Last_::U, std::ratio_multiply<typename Last_::V, I_>>, Den_>;
			using W = std::ratio_divide<One, Den_>;
			using Error = Abs_<std::ratio_divide<std::ratio_subtract<U, std::ratio_multiply<V, W>>, typename Reciprocal<Rem>::I>>;
		};

		template <typename Tr>
		struct ContinuedFraction<Tr, 1>
		{
			using R = Tr;
			using U = One;
			using V = std::ratio<Integer<R>::value>;
			using W = Zero;
			using Rem = Remainder<R, V>;
			using Error = std::ratio_divide<One, typename Reciprocal<Rem>::I>;
		};

		template <typename R, typename Eps, std::intmax_t N = 1, typename = void>
		struct Sqrt_ : Sqrt_<R, Eps, N + 1> {};

		template <typename R, typename Eps, std::intmax_t N>
		struct Sqrt_<R, Eps, N, std::enable_if_t<std::ratio_less_equal_v<typename ContinuedFraction<R, N>::Error, Eps>>>
		{
			using type = typename ContinuedFraction<R, N>::V;
		};

		template <typename R, typename, typename = void>
		struct Sqrt
		{
			static_assert(std::ratio_greater_equal_v<R, Zero>, "R can't be negative");
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<std::ratio_greater_equal_v<R, Zero> && IsPerfectSquare<R>::value>>
		{
			using type = typename IsPerfectSquare<R>::Sqrt;
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<(std::ratio_greater_equal_v<R, Zero> && !IsPerfectSquare<R>::value)>> : Sqrt_<R, Eps>{};
	}
	// clang-format on
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Calculate square root of a ratio at compile-time
	 * @details		Calculates a rational approximation of the square root of the ratio. The error
	 *				in the calculation is bounded by 1/epsilon (Eps). E.g. for the default value
	 *				of 10000000000, the maximum error will be a/10000000000, or 1e-8, or said another way,
	 *				the error will be on the order of 10^-9. Since these calculations are done at
	 *				compile time, it is advisable to set epsilon to the highest value that does not
	 *				cause an integer overflow in the calculation. If you can't compile `ratio_sqrt`
	 *				due to overflow errors, reducing the value of epsilon sufficiently will correct
	 *				the problem.\n\n
	 *				`ratio_sqrt` is guaranteed to converge for all values of `Ratio` which do not
	 *				overflow.
	 * @note		This function provides a rational approximation, _NOT_ an exact value.
	 * @tparam		Ratio	ratio to take the square root of. This can represent any rational value,
	 *						_not_ just integers or values with integer roots.
	 * @tparam		Eps		Value of epsilon, which represents the inverse of the maximum allowable
	 *						error. This value should be chosen to be as high as possible before
	 *						integer overflow errors occur in the compiler.
	 */
	template<RatioType Ratio, std::intmax_t Eps = 10000000000>
	using ratio_sqrt = typename wpi::units::detail::Sqrt<Ratio, std::ratio<1, Eps>>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `sqrt`
		 * @details		square roots the conversion ratio, `dimension` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<ConversionFactorType Unit, std::intmax_t Eps>
		struct sqrt_impl
		{
			using Conversion = typename Unit::conversion_ratio;
			using type       = conversion_factor<ratio_sqrt<Conversion, Eps>, dimension_root<traits::dimension_of_t<typename Unit::dimension_type>, std::ratio<2>>,
					  std::ratio_divide<typename Unit::pi_exponent_ratio, std::ratio<2>>, typename Unit::translation_ratio>;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		UnitManipulators
	 * @brief		represents the square root of type `class U`.
	 * @details		Calculates a rational approximation of the square root of the unit. The error
	 *				in the calculation is bounded by 1/epsilon (Eps). E.g. for the default value
	 *				of 10000000000, the maximum error will be a/10000000000, or 1e-8, or said another way,
	 *				the error will be on the order of 10^-9. Since these calculations are done at
	 *				compile time, it is advisable to set epsilon to the highest value that does not
	 *				cause an integer overflow in the calculation. If you can't compile `ratio_sqrt`
	 *				due to overflow errors, reducing the value of epsilon sufficiently will correct
	 *				the problem.\n\n
	 *				`ratio_sqrt` is guaranteed to converge for all values of `Ratio` which do not
	 *				overflow.
	 * @tparam		Cf	`unit` type to take the square root of.
	 * @tparam		Eps	Value of epsilon, which represents the inverse of the maximum allowable
	 *					error. This value should be chosen to be as high as possible before
	 *					integer overflow errors occur in the compiler.
	 * @note		USE WITH CAUTION. The is an approximate value. In general, square<square_root<meter>> != meter,
	 *				i.e. the operation is not reversible, and it will result in propagated approximations.
	 *				Use only when absolutely necessary.
	 */
	template<ConversionFactorType Cf, std::intmax_t Eps = 10000000000>
	using square_root = typename detail::sqrt_impl<Cf, Eps>::type;

	//------------------------------
	//	COMPOUND UNITS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of compound_unit
		 * @details		multiplies a variadic list of units together, and is inherited from the resulting
		 *				type.
		 */
		template<ConversionFactorType Cf, ConversionFactorType... Cfs>
		struct compound_impl;

		template<ConversionFactorType Cf>
		struct compound_impl<Cf>
		{
			using type = Cf;
		};

		template<ConversionFactorType Cf1, ConversionFactorType Cf2, ConversionFactorType... Cfs>
		struct compound_impl<Cf1, Cf2, Cfs...> : compound_impl<unit_multiply<Cf1, Cf2>, Cfs...>
		{
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		Represents a conversion factor made up from other conversion factors.
	 * @details		Compound conversion factors are formed by multiplying all the conversion factor template arguments.
	 *				Types provided must inherit from `conversion_factor`. A compound conversion factor can
	 *				be formed from any number of other conversion factors, and unit manipulators like `inverse` and
	 *				`squared` are supported. E.g. to specify acceleration, one could declare
	 *				`using acceleration = compound_conversion factor<length::meters, inverse<squared<seconds>>;`
	 * @tparam		Cf	conversion factor which, when multiplied together,
	 *				form the desired compound conversion factor.
	 * @ingroup		ConversionFactor
	 */
	template<ConversionFactorType Cf, ConversionFactorType... Cfs>
	using compound_conversion_factor = typename detail::compound_impl<Cf, Cfs...>::type;

	//------------------------------
	//	PREFIXES
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		prefix applicator.
		 * @details		creates a conversion factor from a prefix and a conversion factor
		 */
		template<RatioType Ratio, ConversionFactorType ConversionFactor>
		struct prefix
		{
			using type = conversion_factor<Ratio, ConversionFactor>;
		};

		/// recursive exponential implementation
		template<int N, RatioType R>
		struct power_of_ratio
		{
			using type = std::ratio_multiply<R, typename power_of_ratio<N - 1, R>::type>;
		};

		/// End recursion
		template<RatioType R>
		struct power_of_ratio<1, R>
		{
			using type = R;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	// clang-format off
	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Decimal Prefixes
	 * @{
	 */
	template<ConversionFactorType Cf> using atto	= typename detail::prefix<std::atto,Cf>::type;			///< Represents the type of `class Cf` with the metric 'atto' prefix appended.	@details E.g. atto<meters> represents meters*10^-18		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using femto	= typename detail::prefix<std::femto,Cf>::type;			///< Represents the type of `class Cf` with the metric 'femto' prefix appended.  @details E.g. femto<meters> represents meters*10^-15	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using pico	= typename detail::prefix<std::pico,Cf>::type;			///< Represents the type of `class Cf` with the metric 'pico' prefix appended.	@details E.g. pico<meters> represents meters*10^-12		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using nano	= typename detail::prefix<std::nano,Cf>::type;			///< Represents the type of `class Cf` with the metric 'nano' prefix appended.	@details E.g. nano<meters> represents meters*10^-9		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using micro	= typename detail::prefix<std::micro,Cf>::type;			///< Represents the type of `class Cf` with the metric 'micro' prefix appended.	@details E.g. micro<meters> represents meters*10^-6		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using milli	= typename detail::prefix<std::milli,Cf>::type;			///< Represents the type of `class Cf` with the metric 'milli' prefix appended.	@details E.g. milli<meters> represents meters*10^-3		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using centi	= typename detail::prefix<std::centi,Cf>::type;			///< Represents the type of `class Cf` with the metric 'centi' prefix appended.	@details E.g. centi<meters> represents meters*10^-2		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using deci	= typename detail::prefix<std::deci,Cf>::type;			///< Represents the type of `class Cf` with the metric 'deci' prefix appended.	@details E.g. deci<meters> represents meters*10^-1		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using deca	= typename detail::prefix<std::deca,Cf>::type;			///< Represents the type of `class Cf` with the metric 'deca' prefix appended.	@details E.g. deca<meters> represents meters*10^1		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using hecto	= typename detail::prefix<std::hecto,Cf>::type;			///< Represents the type of `class Cf` with the metric 'hecto' prefix appended.	@details E.g. hecto<meters> represents meters*10^2		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using kilo	= typename detail::prefix<std::kilo,Cf>::type;			///< Represents the type of `class Cf` with the metric 'kilo' prefix appended.	@details E.g. kilo<meters> represents meters*10^3		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using mega	= typename detail::prefix<std::mega,Cf>::type;			///< Represents the type of `class Cf` with the metric 'mega' prefix appended.	@details E.g. mega<meters> represents meters*10^6		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using giga	= typename detail::prefix<std::giga,Cf>::type;			///< Represents the type of `class Cf` with the metric 'giga' prefix appended.	@details E.g. giga<meters> represents meters*10^9		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using tera	= typename detail::prefix<std::tera,Cf>::type;			///< Represents the type of `class Cf` with the metric 'tera' prefix appended.	@details E.g. tera<meters> represents meters*10^12		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using peta	= typename detail::prefix<std::peta,Cf>::type;			///< Represents the type of `class Cf` with the metric 'peta' prefix appended.	@details E.g. peta<meters> represents meters*10^15		@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using exa		= typename detail::prefix<std::exa,	Cf>::type;			///< Represents the type of `class Cf` with the metric 'exa' prefix appended.	@details E.g. exa<meters> represents meters*10^18		@tparam Cf unit type to apply the prefix to.
	/** @} @} */

	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Binary Prefixes
	 * @{
	 */
	template<ConversionFactorType Cf> using kibi	= typename detail::prefix<std::ratio<1024>,					Cf>::type;	///< Represents the type of `class Cf` with the binary 'kibi' prefix appended.	@details E.g. kibi<bytes> represents bytes*2^10	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using mebi	= typename detail::prefix<std::ratio<1048576>,				Cf>::type;	///< Represents the type of `class Cf` with the binary 'mibi' prefix appended.	@details E.g. mebi<bytes> represents bytes*2^20	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using gibi	= typename detail::prefix<std::ratio<1073741824>,			Cf>::type;	///< Represents the type of `class Cf` with the binary 'gibi' prefix appended.	@details E.g. gibi<bytes> represents bytes*2^30	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using tebi	= typename detail::prefix<std::ratio<1099511627776>,		Cf>::type;	///< Represents the type of `class Cf` with the binary 'tebi' prefix appended.	@details E.g. tebi<bytes> represents bytes*2^40	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using pebi	= typename detail::prefix<std::ratio<1125899906842624>,		Cf>::type;	///< Represents the type of `class Cf` with the binary 'pebi' prefix appended.	@details E.g. pebi<bytes> represents bytes*2^50	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using exbi	= typename detail::prefix<std::ratio<1152921504606846976>,	Cf>::type;	///< Represents the type of `class Cf` with the binary 'exbi' prefix appended.	@details E.g. exbi<bytes> represents bytes*2^60	@tparam Cf unit type to apply the prefix to.
	/** @} @} */
	// clang-format on

	//------------------------------
	//	CONVERSION TRAITS
	//------------------------------

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		`BinaryTypeTrait` for querying whether `Cf1` and `Cf2`
		 *				are conversion factors to the same dimension.
		 * @details		The base characteristic is a specialization of the template `std::bool_constant`.
		 *				Use `is_same_dimension_conversion_factor_v<Cf1, Cf2>` to test whether `Cf1` and `Cf2`
		 *				are conversion factors to the same dimension.
		 * @tparam		Cf1 Conversion factor to query.
		 * @tparam		Cf2 Conversion factor to query.
		 * @sa			is_same_dimension_unit
		 */
		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		struct is_same_dimension_conversion_factor
		  : std::conjunction<std::is_same<dimension_of_t<typename conversion_factor_traits<Cf1>::dimension_type>, dimension_of_t<typename conversion_factor_traits<Cf2>::dimension_type>>>
		{
		};

		template<ConversionFactorType Cf1, ConversionFactorType Cf2>
		inline constexpr bool is_same_dimension_conversion_factor_v = is_same_dimension_conversion_factor<Cf1, Cf2>::value;
	} // namespace traits

	//------------------------------
	//	CONSTEXPR MATH FUNCTIONS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		Helper trait to promote integers or integral units to `double` (units).
		 * @details		Simulates the promotion undergone by integers when calling the standard cmath functions
		 *				overloaded on `float`, `double` and `long double`. Works for both arithmetic types and
		 *				unit types.
		 */
		template<typename T>
		struct floating_point_promotion : std::conditional<std::is_floating_point_v<T>, T, double>
		{
		};

		template<typename T>
		using floating_point_promotion_t = typename floating_point_promotion<T>::type;

		template<ConversionFactorType Cf, typename T, class Ns>
		struct floating_point_promotion<unit<Cf, T, Ns>>
		{
			using type = unit<Cf, floating_point_promotion_t<T>, Ns>;
		};
	} // namespace detail

	namespace Detail
	{
		template<std::floating_point T>
		constexpr T sqrtNewtonRaphson(T x, T curr, T prev)
		{
			return curr == prev ? curr : sqrtNewtonRaphson(x, T{0.5} * (curr + x / curr), curr);
		}
	} // namespace Detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<ArithmeticType T>
	constexpr detail::floating_point_promotion_t<T> sqrt(T x_)
	{
		using FloatingPoint = detail::floating_point_promotion_t<T>;

		const FloatingPoint x(x_);

		return x >= 0 && x < std::numeric_limits<FloatingPoint>::infinity() ? Detail::sqrtNewtonRaphson(x, x, FloatingPoint(0)) : std::numeric_limits<FloatingPoint>::quiet_NaN();
	}

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		template<unsigned long long Exp, typename B>
		constexpr auto pow_acc(B acc, B base [[maybe_unused]]) noexcept
		{
			if constexpr (Exp == 0)
			{
				return static_cast<B>(acc);
			}
			else if constexpr ((Exp & 1) == 0)
			{
				return pow_acc<Exp / 2>(acc, base * base);
			}
			else
			{
				return pow_acc<(Exp - 1) / 2>(acc * base, base * base);
			}
		}
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<signed long long Exp, ArithmeticType B>
	constexpr detail::floating_point_promotion_t<B> pow(B base) noexcept
	{
		using promoted_t   = detail::floating_point_promotion_t<B>;
		constexpr auto one = static_cast<promoted_t>(1);
		if constexpr (Exp >= 0)
		{
			return detail::pow_acc<Exp>(one, static_cast<promoted_t>(base));
		}
		constexpr auto new_exp = static_cast<unsigned long long>(-(Exp + 1));
		return 1 / (base * detail::pow_acc<new_exp>(one, static_cast<promoted_t>(base)));
	}

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		template<typename T1, typename T2>
		constexpr auto pow_acc(T1 acc, T1 x, T2 y) noexcept
		{
			if (y == 0)
			{
				return acc;
			}
			if (y % 2 == 0)
			{
				return pow_acc(acc, x * x, y / 2);
			}
			return pow_acc(acc * x, x * x, (y - 1) / 2);
		}
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<ArithmeticType T1, ArithmeticType T2>
		requires std::is_unsigned_v<T2>
	constexpr detail::floating_point_promotion_t<T1> pow(T1 x, T2 y) noexcept
	{
		using promoted_t = detail::floating_point_promotion_t<T1>;
		return detail::pow_acc(static_cast<promoted_t>(1.0), static_cast<promoted_t>(x), y);
	}

	template<ArithmeticType T1, ArithmeticType T2>
		requires std::is_signed_v<T2>
	constexpr detail::floating_point_promotion_t<T1> pow(T1 x, T2 y) noexcept
	{
		if (y >= 0)
		{
			return pow(x, static_cast<unsigned long long>(y));
		}
		return 1 / (x * pow(x, static_cast<unsigned long long>(-(y + 1))));
	}

	template<ArithmeticType T>
	constexpr T abs(T x)
	{
		return x < 0 ? -x : x;
	}

	//------------------------------
	//	CONVERSION FUNCTIONS
	//------------------------------

	/**
	 * @brief		Tag for `unit` constructors
	 * @details		Tag to disambiguate the `unit` constructor whose value argument is already linearized.
	 */
	struct linearized_value_t
	{
		explicit linearized_value_t() = default;
	};

	inline constexpr linearized_value_t linearized_value{};

	/**
	 * @ingroup		Conversion
	 * @brief		converts a <i>value</i> from an unit to another.
	 * @details		Converts a <i>value</i> of an arithmetic type to another unit. E.g. @code double result =
	 *				convert<meters, feet>(1.0);	// result == 3.28084 @endcode Intermediate computations
	 *				are carried in the widest representation before being converted to `To`.
	 *				`is_same_dimension_conversion_factor_v<ConversionFactorFrom, ConversionFactorTo>` shall be `true`.
	 * @sa			unit	for implicit conversion of units.
	 * @tparam		ConversionFactorFrom conversion factor of the unit to convert <i>value</i> from.
	 *				`is_conversion_factor_v<ConversionFactorFrom>` shall be `true`.
	 * @tparam		ConversionFactorTo conversion factor of the unit to convert <i>value</i> to.
	 *				`is_conversion_factor_v<ConversionFactorTo>` shall be `true`.
	 * @tparam		From type of <i>value</i>. Shall be an arithmetic type.
	 * @param[in]	value Arithmetic value to convert.
	 *				The value should represent a quantity in units of `ConversionFactorFrom`.
	 * @tparam		To type of the converted unit value. Shall be an arithmetic type.
	 * @returns		value, converted from units of `ConversionFactorFrom` to `ConversionFactorTo`.
	 *				The value represents a quantity in units of `ConversionFactorTo`.
	 */
	template<ConversionFactorType ConversionFactorFrom, ConversionFactorType ConversionFactorTo, ArithmeticType To = UNIT_LIB_DEFAULT_TYPE, ArithmeticType From>
		requires(traits::is_same_dimension_conversion_factor_v<ConversionFactorFrom, ConversionFactorTo>)
	constexpr To convert(const From& value) noexcept
	{
		using Ratio   = std::ratio_divide<typename ConversionFactorFrom::conversion_ratio, typename ConversionFactorTo::conversion_ratio>;
		using PiRatio = std::ratio_subtract<typename ConversionFactorFrom::pi_exponent_ratio, typename ConversionFactorTo::pi_exponent_ratio>;
		using Translation =
			std::ratio_divide<std::ratio_subtract<typename ConversionFactorFrom::translation_ratio, typename ConversionFactorTo::translation_ratio>, typename ConversionFactorTo::conversion_ratio>;

		[[maybe_unused]] constexpr auto normal_convert = []<typename T0>(const T0& val)
		{
			using ResolvedUnitFrom = conversion_factor<typename ConversionFactorFrom::conversion_ratio, typename ConversionFactorFrom::dimension_type>;
			using ResolvedUnitTo   = conversion_factor<typename ConversionFactorTo::conversion_ratio, typename ConversionFactorTo::dimension_type>;
			return convert<ResolvedUnitFrom, ResolvedUnitTo, std::decay_t<T0>>(val);
		};

		[[maybe_unused]] constexpr auto pi_convert = []<typename T0>(const T0& val)
		{
			using ResolvedUnitFrom =
				conversion_factor<typename ConversionFactorFrom::conversion_ratio, typename ConversionFactorFrom::dimension_type, typename ConversionFactorFrom::pi_exponent_ratio>;
			using ResolvedUnitTo = conversion_factor<typename ConversionFactorTo::conversion_ratio, typename ConversionFactorTo::dimension_type, typename ConversionFactorTo::pi_exponent_ratio>;
			return convert<ResolvedUnitFrom, ResolvedUnitTo, std::decay_t<T0>>(val);
		};

		// same exact unit on both sides
		if constexpr (std::same_as<ConversionFactorFrom, ConversionFactorTo>)
		{
			return static_cast<To>(value);
		}
		// PI REQUIRED, no translation
		else if constexpr (!std::same_as<std::ratio<0>, PiRatio> && std::same_as<std::ratio<0>, Translation>)
		{
			using CommonUnderlying             = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;
			constexpr long double PiRatioValue = PiRatio::num / PiRatio::den;

			// constexpr pi in numerator
			if constexpr (PiRatioValue >= 1 && PiRatio::num % PiRatio::den == 0)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(pow(detail::PI_VAL, PiRatioValue))));
			}
			// constexpr pi in denominator
			else if constexpr (PiRatioValue <= -1 && PiRatio::num % PiRatio::den == 0)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) / static_cast<CommonUnderlying>(pow(detail::PI_VAL, -PiRatioValue))));
			}
			// non-constexpr pi in numerator. This case (only) isn't actually constexpr.
			else if constexpr (PiRatioValue < 1 && PiRatio::num / PiRatioValue > -1)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(std::pow(detail::PI_VAL, PiRatioValue))));
			}
		}
		// Translation required, no pi variable
		else if constexpr (std::same_as<std::ratio<0>, PiRatio> && !std::same_as<std::ratio<0>, Translation>)
		{
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;

			return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value)) + (static_cast<CommonUnderlying>(Translation::num) / static_cast<CommonUnderlying>(Translation::den)));
		}
		// pi and translation needed
		else if constexpr (!std::same_as<std::ratio<0>, PiRatio> && !std::same_as<std::ratio<0>, Translation>)
		{
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;

			return static_cast<To>(pi_convert(static_cast<CommonUnderlying>(value)) + (static_cast<CommonUnderlying>(Translation::num) / static_cast<CommonUnderlying>(Translation::den)));
		}
		// normal conversion between two different units
		else
		{
			using CommonUnderlying = std::common_type_t<To, From, std::intmax_t>;

			if constexpr (Ratio::num == 1 && Ratio::den == 1)
				return static_cast<To>(value);
			if constexpr (Ratio::num != 1 && Ratio::den == 1)
				return static_cast<To>(static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(Ratio::num));
			if constexpr (Ratio::num == 1 && Ratio::den != 1)
				return static_cast<To>(static_cast<CommonUnderlying>(value) / static_cast<CommonUnderlying>(Ratio::den));
			if constexpr (Ratio::num != 1 && Ratio::den != 1)
				return static_cast<To>((static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(Ratio::num)) / static_cast<CommonUnderlying>(Ratio::den));
		}
	}

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		SFINAE helper to prevent warnings in Clang 6 when `From` or `To` is a `conversion_factor`.
		 * @details		`typename T::conversion_factor` is interpreted as a constructor when `T` is a
		 *				`conversion_factor` (-Winjected-class-name).
		 */
		template<UnitType UnitFrom, UnitType UnitTo>
		struct delayed_is_same_dimension_conversion_factor : std::false_type
		{
			static constexpr bool value = traits::is_same_dimension_conversion_factor_v<typename UnitFrom::conversion_factor, typename UnitTo::conversion_factor>;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		Conversion
	 * @brief		converts an unit to another unit.
	 * @details		Converts the value of an unit to another unit. E.g. @code meter_t result =
	 *				convert<meters>(foot_t(1.0));	// result == 3.28084_m @endcode Intermediate
	 *				computations are carried in the widest representation before being converted to `UnitTo`.
	 *				`is_same_dimension_unit_v<UnitFrom, UnitTo>` shall be `true`.
	 * @sa			unit	for implicit conversion of unit containers.
	 * @tparam		UnitFrom unit to convert to `UnitTo`. `is_unit_v<UnitFrom>` shall be `true`.
	 * @tparam		UnitTo unit to convert `from` to. `is_unit_v<UnitTo>` shall be `true`.
	 * @returns		from, converted from units of `UnitFrom` to `UnitTo`.
	 */
	template<UnitType UnitTo, UnitType UnitFrom>
		requires same_dimension<UnitFrom, UnitTo>
	constexpr UnitTo convert(const UnitFrom& from) noexcept
	{
		return UnitTo(convert<typename UnitFrom::conversion_factor, typename UnitTo::conversion_factor, typename UnitTo::underlying_type>(from.to_linearized()), linearized_value);
	}

	//------------------------------
	//	UNIT TYPE TRAITS
	//------------------------------

	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSES_ONLY
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publicly defined types of `wpi::units::unit`
		 * @details		The units library determines certain properties of the unit types passed to them
		 *				and what they represent by using the members of the corresponding unit_traits instantiation.
		 */
		template<typename T>
		struct unit_traits
		{
			typedef typename T::numerical_scale_type numerical_scale_type; ///< Type of the unit numerical_scale (e.g. linear_scale, decibel_scale).
																		   ///< This property is used to enable the proper linear or logarithmic
																		   ///< arithmetic functions.
			typedef typename T::underlying_type   underlying_type;         ///< Underlying storage type of the `unit`, e.g. `double`.
			typedef typename T::value_type        value_type;              ///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
			typedef typename T::conversion_factor conversion_factor;       ///< Type of unit the `unit` represents, e.g. `meters`
		};
#endif

		/** @cond */ // DOXYGEN IGNORE
		/**
		 * @brief		unit_traits specialization for things which are not unit
		 * @details
		 */
		template<typename, typename = void>
		struct unit_traits
		{
			using numerical_scale_type = void;
			using underlying_type      = void;
			using value_type           = void;
			using conversion_factor    = void;
		};

		template<ArithmeticType T>
		struct unit_traits<T, std::void_t<T>>
		{
			using numerical_scale_type = void;
			using underlying_type      = T;
			using value_type           = void;
			using conversion_factor    = wpi::units::conversion_factor<std::ratio<1>, dimension_t<>>;
		};

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publicly defined types of `wpi::units::unit`
		 * @details
		 */
		template<NonArithmeticType T>
		struct unit_traits<T, std::void_t<typename T::numerical_scale_type, typename T::underlying_type, typename T::value_type, typename T::conversion_factor>>
		{
			using numerical_scale_type = typename T::numerical_scale_type;
			using underlying_type      = typename T::underlying_type;
			using value_type           = typename T::value_type;
			using conversion_factor    = typename T::conversion_factor;
		};

		/** @endcond */ // END DOXYGEN IGNORE
	} // namespace traits

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		`BinaryTypeTrait` for querying whether `U1` and `U2` are units of the same dimension.
		 * @details		The base characteristic is a specialization of the template `std::bool_constant`.
		 *				Use `is_same_dimension_unit_v<U1, U2>` to test whether `U1` and `U2`
		 *				are units of the same dimension.
		 * @tparam		U1 Unit to query.
		 * @tparam		U2 Unit to query.
		 * @sa			is_same_dimension_conversion_factor
		 */
		template<UnitType U1, UnitType U2>
		struct is_same_dimension_unit
		  : std::conjunction<is_unit<U1>, is_unit<U2>, is_same_dimension_conversion_factor<typename unit_traits<U1>::conversion_factor, typename unit_traits<U2>::conversion_factor>>
		{
		};

		template<UnitType U1, UnitType U2>
		inline constexpr bool is_same_dimension_unit_v = is_same_dimension_unit<U1, U2>::value;
	} // namespace traits

	//----------------------------------
	//	UNIT TYPE
	//----------------------------------

	/** @cond */ // DOXYGEN IGNORE

	namespace detail
	{
		/**
		 * @brief		SFINAE helper to test if an arithmetic conversion is lossless.
		 */
		template<class From, class To>
		inline constexpr bool is_losslessly_convertible = std::is_arithmetic_v<From> && (std::is_floating_point_v<To> || !std::is_floating_point_v<From>);

		/**
		 * @brief		Trait which tests if a unit type can be converted to another unit type without truncation error.
		 * @details		Valid only when the involved units have integral underlying types.
		 */
		template<ConversionFactorType ConversionFactorFrom, ConversionFactorType ConversionFactorTo>
		struct is_non_truncated_convertible_unit : std::false_type
		{
			static constexpr bool value = std::ratio_divide<typename ConversionFactorFrom::conversion_ratio, typename ConversionFactorTo::conversion_ratio>::den == 1;
		};

		/**
		 * @brief		SFINAE helper to test if a conversion of units is lossless.
		 */
		template<class UnitFrom, class UnitTo>
		inline constexpr bool is_losslessly_convertible_unit = std::conjunction_v<traits::is_same_dimension_unit<UnitFrom, UnitTo>,
			std::disjunction<std::is_floating_point<typename UnitTo::underlying_type>,
				std::conjunction<std::negation<std::is_floating_point<typename UnitFrom::underlying_type>>,
					is_non_truncated_convertible_unit<typename UnitFrom::conversion_factor, typename UnitTo::conversion_factor>>>>;

		template<RatioType Ratio>
		using time_conversion_factor = conversion_factor<Ratio, dimension::time>;

		/**
		 * @brief		SFINAE helper to test if a `conversion_factor` is of the time dimension.
		 */
		template<ConversionFactorType ConversionFactor>
		inline constexpr bool is_time_conversion_factor = traits::is_same_dimension_conversion_factor_v<ConversionFactor, time_conversion_factor<std::ratio<1>>>;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

					/**
					 * @ingroup		UnitTypes
					 * @brief		Describes objects that represent quantities of a given unit.
					 * @details		Stores a value which represents a quantity in the given units. Units
					 *				(except dimensionless units) are *not* convertible to arithmetic types, in order to
					 *				provide type safety in dimensional analysis. Units *are* implicitly
					 *				convertible to other units types of the same dimension, if such conversion is lossless.
					 *				Units support various types of arithmetic operations, depending on their scale type.
					 *
					 *				The value of an `unit` can only be set on construction, or changed by assignment
					 *				from another `unit` type. If necessary, the underlying value can be accessed
					 *				using `operator()`: @code
					 *				meter_t m(5.0);
					 *				double val = m(); // val == 5.0	@endcode.
					 * @tparam		ConversionFactor `conversion_factor` of the represented unit (e.g. meters)
					 * @tparam		T underlying type of the storage. Defaults to `UNIT_LIB_DEFAULT_TYPE`.
					 * @tparam		NumericalScale optional scale class for the units. Defaults to linear (i.e. does
					 *				not scale the unit value). Examples of non-linear scales could be logarithmic,
					 *				decibel, or richter scales. Numerical scales must adhere to the numerical-scale
					 *				concept, i.e. `is_numerical_scale_v<...>` must be `true`.
					 * @sa
					 *				- \ref lengthUnits "length units"
					 *				- \ref massUnits "mass units"
					 *				- \ref timeUnits "time units"
					 *				- \ref angleUnits "angle units"
					 *				- \ref currentUnits "current units"
					 *				- \ref temperatureUnits "temperature units"
					 *				- \ref substanceUnits "substance units"
					 *				- \ref luminousIntensityUnits "luminous intensity units"
					 *				- \ref solidAngleUnits "solid angle units"
					 *				- \ref frequencyUnits "frequency units"
					 *				- \ref velocityUnits "velocity units"
					 *				- \ref angularVelocityUnits "angular velocity units"
					 *				- \ref accelerationUnits "acceleration units"
					 *				- \ref forceUnits "force units"
					 *				- \ref pressureUnits "pressure units"
					 *				- \ref chargeUnits "charge units"
					 *				- \ref energyUnits "energy units"
					 *				- \ref powerUnits "power units"
					 *				- \ref voltageUnits "voltage units"
					 *				- \ref capacitanceUnits "capacitance units"
					 *				- \ref impedanceUnits "impedance units"
					 *				- \ref magneticFluxUnits "magnetic flux units"
					 *				- \ref magneticFieldStrengthUnits "magnetic field strength units"
					 *				- \ref inductanceUnits "inductance units"
					 *				- \ref luminousFluxUnits "luminous flux units"
					 *				- \ref illuminanceUnits "illuminance units"
					 *				- \ref radiationUnits "radiation units"
					 *				- \ref torqueUnits "torque units"
					 *				- \ref areaUnits "area units"
					 *				- \ref volumeUnits "volume units"
					 *				- \ref densityUnits "density units"
					 *				- \ref concentrationUnits "concentration units"
					 *				- \ref constantUnits "constant units"
					 */
#ifdef _WIN32
	// Microsoft compiler requires explicit activation of empty base class optimization
	// so that sizeof(unit<..., double, ...>) == sizeof(double)
#define MSVC_EBO __declspec(empty_bases)
#else
#define MSVC_EBO
#endif
	template<ConversionFactorType ConversionFactor, ArithmeticType T = UNIT_LIB_DEFAULT_TYPE, NumericalScaleType<T> NumericalScale = linear_scale>
	class MSVC_EBO unit : public ConversionFactor, public NumericalScale, public detail::_unit
	{
	public:
		using numerical_scale_type = NumericalScale;   ///< Type of the numerical scale of the unit (e.g. linear_scale)
		using underlying_type      = T;                ///< Type of the underlying storage of the unit (e.g. double)
		using value_type           = T;                ///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
		using conversion_factor    = ConversionFactor; ///< Type of `conversion_factor` the `unit` represents (e.g. meters)

		/**
		 * @ingroup		Constructors
		 * @brief		default constructor.
		 */
		constexpr unit() = default;

		/**
		 * @ingroup		Constructors
		 * @brief		default copy constructor.
		 */
		constexpr unit(const unit&) = default;

		/**
		 * @brief		converting constructor
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
			requires traits::is_same_dimension_unit_v<unit<ConversionFactorRhs, Ty, NsRhs>, unit> && detail::is_losslessly_convertible_unit<unit<ConversionFactorRhs, Ty, NsRhs>, unit>
		constexpr unit(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) noexcept
		  : _linearized_value(wpi::units::convert<unit>(rhs)._linearized_value)
		{
		}

		/**
		 * @brief		constructor
		 * @details		constructs a new unit with `value`.
		 * @param[in]	value	unit magnitude.
		 */
		template<ArithmeticType Ty>
			requires(!traits::is_dimensionless_unit<ConversionFactor>::value && detail::is_losslessly_convertible<Ty, T>)
		explicit constexpr unit(Ty value) noexcept
		  : _linearized_value(NumericalScale::linearize(static_cast<T>(value)))
		{
		}

		/**
		 * @brief		constructor
		 * @details		constructs a new unit with `value`.
		 * @param[in]	value	linearized unit magnitude.
		 */
		template<ArithmeticType Ty>
			requires detail::is_losslessly_convertible<Ty, T>
		explicit constexpr unit(Ty value, linearized_value_t) noexcept
		  : _linearized_value(value)
		{
		}

		/**
		 * @brief		constructor
		 * @details		enable implicit conversions from T types ONLY for linear dimensionless units
		 * @param[in]	value value of the unit
		 */
		template<ArithmeticType Ty>
			requires traits::is_dimensionless_unit<ConversionFactor>::value && detail::is_losslessly_convertible<Ty, T>
		constexpr unit(Ty value) noexcept
		  : _linearized_value(NumericalScale::linearize(static_cast<T>(value)))
		{
		}

		/**
		 * @brief		chrono constructor
		 * @details		enable implicit conversions from std::chrono::duration types ONLY for time units
		 * @param[in]	value value of the unit
		 */
		template<ArithmeticType Rep, RatioType Period>
			requires detail::is_time_conversion_factor<ConversionFactor> && detail::is_losslessly_convertible<Rep, T>
		constexpr unit(const std::chrono::duration<Rep, Period>& value) noexcept
		  : _linearized_value(wpi::units::convert<unit>(wpi::units::unit<wpi::units::conversion_factor<Period, dimension::time>, Rep>(value.count()))._linearized_value)
		{
		}

		/**
		 * @brief		default assignment
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		constexpr unit& operator=(const unit& rhs) noexcept = default;

		/**
		 * @brief		assignment
		 * @details		performs implicit conversions from built-in types ONLY for dimensionless units
		 * @param[in]	rhs value to copy.
		 */
		template<ConversionFactorType Cf = ConversionFactor>
			requires traits::is_dimensionless_unit<Cf>::value
		constexpr unit& operator=(const underlying_type& rhs) noexcept
		{
			unit<wpi::units::conversion_factor<std::ratio<1>, wpi::units::dimension::dimensionless>, underlying_type, linear_scale> dimensionlessRhs(rhs);
			_linearized_value = wpi::units::convert<unit>(dimensionlessRhs)._linearized_value;
			return *this;
		}

		/**
		 * @brief		less-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than the value of `rhs`
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr bool operator<(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			return (CommonUnit(*this)._linearized_value < CommonUnit(rhs)._linearized_value);
		}

		/**
		 * @brief		less-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than or equal to the value of `rhs`
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr bool operator<=(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			return (CommonUnit(*this)._linearized_value <= CommonUnit(rhs)._linearized_value);
		}

		/**
		 * @brief		greater-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than the value of `rhs`
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr bool operator>(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			return (CommonUnit(*this)._linearized_value > CommonUnit(rhs)._linearized_value);
		}

		/**
		 * @brief		greater-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than or equal to the value of `rhs`
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr bool operator>=(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			return (CommonUnit(*this)._linearized_value >= CommonUnit(rhs)._linearized_value);
		}

		/**
		 * @brief		equality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` exactly equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit is a double.
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
			requires(std::floating_point<T> || std::floating_point<Ty>)
		constexpr bool operator==(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit       = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			using CommonUnderlying = typename CommonUnit::underlying_type;

			const auto common_lhs(CommonUnit(*this)._linearized_value);
			const auto common_rhs(CommonUnit(rhs)._linearized_value);

			return abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::epsilon() * abs(common_lhs + common_rhs) ||
				abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::min();
		}

		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
			requires(std::integral<T> && std::integral<Ty>)
		constexpr bool operator==(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			return CommonUnit(*this)._linearized_value == CommonUnit(rhs)._linearized_value;
		}

		/**
		 * @brief		inequality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is not equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit is a double.
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr bool operator!=(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		/**
		 * @brief		scaled unit value
		 * @details     does NOT normalize dimensionless values.
		 * @returns		value of the unit in it's underlying, non-safe type after applying the scale.
		 *
		 */
		constexpr underlying_type raw() const noexcept
		{
			return static_cast<underlying_type>(NumericalScale::scale(_linearized_value));
		}

		/**
		 * @brief		unit value
		 * @details     Normalizes dimensionless values to remove any scale factor they may have. E.g.
		 * `percent(50).value() == 0.5`.
		 * @returns		value of the unit in it's underlying, non-safe type.
		 *
		 */
		constexpr underlying_type value() const noexcept
		{
			if constexpr (traits::is_dimensionless_unit<ConversionFactor>::value)
				return NumericalScale::scale(
					wpi::units::convert<unit<wpi::units::conversion_factor<std::ratio<1>, dimension::dimensionless, typename traits::conversion_factor_traits<ConversionFactor>::pi_exponent_ratio,
											typename traits::conversion_factor_traits<ConversionFactor>::translation_ratio>,
						T, NumericalScale>>(*this)
						.to_linearized());
			else
				return raw();
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit converted to an arithmetic, non-safe type.
		 */
		template<ArithmeticType Ty>
		constexpr Ty to() const noexcept
		{
			return static_cast<Ty>(*this);
		}

		/**
		 * @brief		linearized unit value
		 * @returns		linearized value of unit which has a (possibly) non-linear scale.
		 */
		constexpr T to_linearized() const noexcept
		{
			return _linearized_value;
		}

		/**
		 * @brief		conversion
		 * @details		Converts to a different unit. Units can be converted to other units
		 *				implicitly, but this can be used in cases where the explicit notation of a conversion
		 *				is beneficial, or where an prvalue unit is needed.
		 * @tparam		Cf conversion factor of the unit to convert to
		 * @tparam		Ty underlying type of the unit to convert to
		 * @returns		a unit with the specified parameters containing the equivalent value to
		 *				*this.
		 */
		template<ConversionFactorType Cf, ArithmeticType Ty = T>
		constexpr unit<Cf, Ty> convert() const noexcept
		{
			return unit<Cf, Ty>(*this);
		}

		/**
		 * @brief		conversion
		 * @details		Converts to a different unit. Units can be converted to other units
		 *				implicitly, but this can be used in cases where the explicit notation of a conversion
		 *				is beneficial, or where a prvalue unit is needed.
		 * @tparam		UnitType unit type to convert to
		 * @returns		a unit with the specified parameters containing the equivalent value to
		 *				*this.
		 */
		template<template<class> class UnitType>
			requires same_dimension<UnitType<T>, unit>
		constexpr UnitType<T> convert() noexcept
		{
			return UnitType<T>(*this);
		}

		/**
		 * @brief		implicit type unsafe conversion.
		 * @details		only enabled for dimensionless unit types.
		 */
		template<ArithmeticType Ty>
			requires traits::is_dimensionless_unit<ConversionFactor>::value
		constexpr operator Ty() const noexcept
		{
			// this conversion also resolves any PI exponents, by converting from a non-zero PI ratio to a zero-pi
			// ratio.
			return NumericalScale::scale(wpi::units::convert<unit<wpi::units::conversion_factor<std::ratio<1>, dimension::dimensionless>, Ty, NumericalScale>>(*this).to_linearized());
		}

		/**
		 * @brief		explicit type unsafe conversion.
		 * @details		only enabled for non-dimensionless unit types.
		 */
		template<ArithmeticType Ty>
			requires(!traits::is_dimensionless_unit<ConversionFactor>::value)
		constexpr explicit operator Ty() const noexcept
		{
			return static_cast<Ty>(this->value());
		}

		/**
		 * @brief		chrono implicit type conversion.
		 * @details		only enabled for time unit types.
		 */
		template<ArithmeticType Rep, RatioType Period, ConversionFactorType Cf = ConversionFactor>
			requires detail::is_time_conversion_factor<Cf> && detail::is_losslessly_convertible<T, Rep>
		constexpr operator std::chrono::duration<Rep, Period>() const noexcept
		{
			return std::chrono::duration<Rep, Period>(wpi::units::unit<wpi::units::conversion_factor<Period, dimension::time>, Rep>(*this).value());
		}

		/**
		 * @brief		returns the unit name
		 */
		template<UnitType Unit = unit>
		[[nodiscard]] constexpr const char* name() const noexcept
		{
			return unit_name_v<Unit>;
		}

		/**
		 * @brief		returns the unit abbreviation
		 */
		template<UnitType Unit = unit>
		[[nodiscard]] constexpr const char* abbreviation() const noexcept
		{
			return unit_abbreviation_v<Unit>;
		}

		template<ConversionFactorType Cf, ArithmeticType Ty, NumericalScaleType<Ty> Ns>
		friend class unit;

		/// Not to be used as part of the official API, this member is public to allow the use
		/// of `units` as NTTP types.
		T _linearized_value;
	};

	//------------------------------
	//	UNIT NON-MEMBER FUNCTIONS
	//------------------------------

	/**
	 * @ingroup		UnitTypes
	 * @brief		Constructs a unit container from an arithmetic type.
	 * @details		make_unit can be used to construct a unit container from an arithmetic type, as an alternative to
	 *				using the explicit constructor. Unlike the explicit constructor it forces the user to explicitly
	 *				specify the units.
	 * @tparam		UnitType Type to construct.
	 * @tparam		T		Arithmetic type.
	 * @param[in]	value	Arithmetic value that represents a quantity in units of `UnitType`.
	 */
	template<UnitType UnitType, ArithmeticType T>
		requires detail::is_losslessly_convertible<T, typename UnitType::underlying_type>
	constexpr UnitType make_unit(const T value) noexcept
	{
		return UnitType(value);
	}

#if defined(UNIT_LIB_ENABLE_IOSTREAM)

	//-----------------------------------------
	//	OSTREAM OPERATOR FOR EPHEMERAL UNITS
	//-----------------------------------------

	template<class D, class E>
	std::ostream& operator<<(std::ostream& os, const dim<D, E>&)
	{
		if constexpr (E::num != 0)
			os << ' ' << D::abbreviation;
		if constexpr (E::num != 0 && E::num != 1)
		{
			os << "^" << E::num;
		}
		if constexpr (E::den != 1)
		{
			os << "/" << E::den;
		}
		return os;
	}

	template<class... Dims>
	std::ostream& operator<<(std::ostream& os, const dimension_t<Dims...>&)
	{
		((os << Dims{}), ...);
		return os;
	}

	template<ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
	std::ostream& operator<<(std::ostream& os, const unit<ConversionFactor, T, NumericalScale>& obj)
	{
		using BaseConversion   = conversion_factor<std::ratio<1>, typename ConversionFactor::dimension_type>;
		using BaseUnit         = unit<BaseConversion, T, NumericalScale>;
		using PromotedBaseUnit = unit<BaseConversion, detail::floating_point_promotion_t<T>, NumericalScale>;

		std::locale loc;
		if constexpr (unit_abbreviation_v<unit<ConversionFactor, T, NumericalScale>>)
		{
			os << obj.raw() << " " << obj.abbreviation();
		}
		else
		{
			if constexpr (detail::is_losslessly_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>)
			{
				os << BaseUnit(obj).raw();
			}
			else
			{
				os << PromotedBaseUnit(obj).raw();
			}

			using DimType = traits::dimension_of_t<ConversionFactor>;
			if constexpr (!DimType::empty)
			{
				os << DimType{};
			}
		}

		return os;
	}
#endif // defined(UNIT_LIB_ENABLE_IOSTREAM)

#if defined(UNIT_LIB_ENABLE_IOSTREAM) || __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)
	//----------------------------
	//  to_string
	//----------------------------

	template<ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
	std::string to_string(const unit<ConversionFactor, T, NumericalScale>& obj)
	{
		using BaseConversion   = conversion_factor<std::ratio<1>, typename ConversionFactor::dimension_type>;
		using BaseUnit         = unit<BaseConversion, T, NumericalScale>;
		using PromotedBaseUnit = unit<BaseConversion, detail::floating_point_promotion_t<T>, NumericalScale>;

		if constexpr (unit_abbreviation_v<unit<ConversionFactor, T, NumericalScale>>)
		{
			std::string s = detail::to_string(obj.raw());
			s.append(" ").append(obj.abbreviation());
			return s;
		}
		else
		{
			std::string s = detail::to_string(std::conditional_t<detail::is_losslessly_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>, BaseUnit, PromotedBaseUnit>(obj).raw());

			using DimType = traits::dimension_of_t<ConversionFactor>;
			if constexpr (!DimType::empty)
			{
				s.append(" ").append(DimType{});
			}
			return s;
		}
	}
#endif // defined(UNIT_LIB_ENABLE_IOSTREAM) || __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)

	//------------------------------
	//	std::ratio helpers
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		greatest common divisor of two ratios.
		 */
		template<RatioType Ratio1, RatioType Ratio2>
		using ratio_gcd = std::ratio<std::gcd(Ratio1::num, Ratio2::num), std::lcm(Ratio1::den, Ratio2::den)>;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE
} // end namespace wpi::units

#if __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)

//-----------------------------
//	FMT SUPPORT
//-----------------------------

template<class D, class E>
struct fmt::formatter<wpi::units::dim<D, E>>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template <typename FmtContext>
	auto format(const wpi::units::dim<D, E>& obj, FmtContext& ctx) const
	{
		auto out = ctx.out();

		if constexpr (E::num != 0)
		{
			out = fmt::format_to(out, " {}", D::abbreviation);
		}
		if constexpr (E::num != 0 && E::num != 1)
		{
			out = fmt::format_to(out, "^{}", E::num);
		}
		if constexpr (E::den != 1)
		{
			out = fmt::format_to(out, "/{}", E::den);
		}

		return out;
	}
};

template<class... Dims>
struct fmt::formatter<wpi::units::dimension_t<Dims...>>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template <typename FmtContext>
	auto format(const wpi::units::dimension_t<Dims...>&, FmtContext& ctx) const
	{
		auto out = ctx.out();
		((out = fmt::format_to(out, "{}", Dims{})), ...);
		return out;
	}
};

template <wpi::units::ConversionFactorType ConversionFactor, wpi::units::ArithmeticType T, wpi::units::NumericalScaleType<T> NumericalScale>
struct fmt::formatter<wpi::units::unit<ConversionFactor, T, NumericalScale>> : fmt::formatter<double>
{
	template <typename FmtContext>
	auto format(const wpi::units::unit<ConversionFactor, T, NumericalScale>& obj, FmtContext& ctx) const
	{
		using BaseConversion   = wpi::units::conversion_factor<std::ratio<1>, typename ConversionFactor::dimension_type>;
		using BaseUnit         = wpi::units::unit<BaseConversion, T, NumericalScale>;
		using PromotedBaseUnit = wpi::units::unit<BaseConversion, wpi::units::detail::floating_point_promotion_t<T>, NumericalScale>;

		auto out = ctx.out();

		if constexpr (wpi::units::unit_abbreviation_v<wpi::units::unit<ConversionFactor, T, NumericalScale>>)
		{
			out = formatter<double>::format(obj.raw(), ctx);
			out = fmt::format_to(out, " {}", obj.abbreviation());
		}
		else
		{
			if constexpr (wpi::units::detail::is_losslessly_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>)
			{
				out = formatter<double>::format(BaseUnit(obj).raw(), ctx);
			}
			else
			{
				out = formatter<double>::format(PromotedBaseUnit(obj).raw(), ctx);
			}

			using DimType = wpi::units::traits::dimension_of_t<ConversionFactor>;
			if constexpr (!DimType::empty)
			{
				out = fmt::format_to(out, "{}", DimType{});
			}
		}

		return out;
	}
};

#endif // __has_include(<fmt/format.h>) && !defined(UNIT_LIB_DISABLE_FMT)

//------------------------------
//	std::common_type
//------------------------------

namespace std
{
	/**
	 * @ingroup		STDTypeTraits
	 * @brief		common type of units
	 * @details		The `type` alias of the `std::common_type` of two `unit`s of the same dimension is the least precise
	 *				`unit` to which both `unit` arguments can be converted to without requiring a division operation or
	 *				truncating any value of these conversions, although floating-point units may have round-off errors.
	 *				If the units have mixed scales, preference is given to `linear_scale` for their common type.
	 */
	template<class ConversionFactorLhs, class Tx, class ConversionFactorRhs, class Ty, class NumericalScale>
	struct common_type<wpi::units::unit<ConversionFactorLhs, Tx, NumericalScale>, wpi::units::unit<ConversionFactorRhs, Ty, NumericalScale>>
	  : std::enable_if<wpi::units::traits::is_same_dimension_conversion_factor_v<ConversionFactorLhs, ConversionFactorRhs>,
			wpi::units::unit<
				wpi::units::traits::strong_t<wpi::units::conversion_factor<wpi::units::detail::ratio_gcd<typename ConversionFactorLhs::conversion_ratio, typename ConversionFactorRhs::conversion_ratio>,
					wpi::units::traits::dimension_of_t<ConversionFactorLhs>, wpi::units::detail::ratio_gcd<typename ConversionFactorLhs::pi_exponent_ratio, typename ConversionFactorRhs::pi_exponent_ratio>,
					wpi::units::detail::ratio_gcd<typename ConversionFactorLhs::translation_ratio, typename ConversionFactorRhs::translation_ratio>>>,
				common_type_t<Tx, Ty>, NumericalScale>>
	{
	};

	// In the case the two units are the same type, just use that type as common type
	template<class UnitConversionT, class T, class NonLinearScale>
	struct common_type<wpi::units::unit<UnitConversionT, T, NonLinearScale>, wpi::units::unit<UnitConversionT, T, NonLinearScale>>
	{
		using type = wpi::units::unit<UnitConversionT, T, NonLinearScale>;
	};

	template<class Ratio, class T, class NumericalScale, class Rep, class Period>
	struct common_type<wpi::units::unit<wpi::units::detail::time_conversion_factor<Ratio>, T, NumericalScale>, chrono::duration<Rep, Period>>
	  : std::common_type<wpi::units::unit<wpi::units::detail::time_conversion_factor<Ratio>, T, NumericalScale>, decltype(wpi::units::unit{chrono::duration<Rep, Period>{}})>
	{
	};

	template<class ConversionFactor, class T, class NumericalScale, class Rep, class Period>
	struct common_type<chrono::duration<Rep, Period>, wpi::units::unit<ConversionFactor, T, NumericalScale>>
	  : std::common_type<wpi::units::unit<ConversionFactor, T, NumericalScale>, chrono::duration<Rep, Period>>
	{
	};

	template<class ConversionFactor, class Tx, class NumericalScale, class Ty>
	struct common_type<Ty, wpi::units::unit<ConversionFactor, Tx, NumericalScale>>
	  : std::enable_if<std::is_arithmetic_v<Ty> && wpi::units::traits::is_dimensionless_unit<wpi::units::unit<ConversionFactor, Tx, NumericalScale>>::value,
			wpi::units::unit<wpi::units::conversion_factor<std::ratio<1>, wpi::units::dimension::dimensionless>, common_type_t<Tx, Ty>, NumericalScale>>
	{
	};

	template<class ConversionFactor, class Tx, class NumericalScale, class Ty>
	struct common_type<wpi::units::unit<ConversionFactor, Tx, NumericalScale>, Ty>
	  : std::enable_if<std::is_arithmetic_v<Ty> && wpi::units::traits::is_dimensionless_unit<wpi::units::unit<ConversionFactor, Tx, NumericalScale>>::value,
			wpi::units::unit<wpi::units::conversion_factor<std::ratio<1>, wpi::units::dimension::dimensionless>, common_type_t<Tx, Ty>, NumericalScale>>
	{
	};

	/** @cond */ // DOXYGEN IGNORE
	/**
	 * @brief		`linear_scale` preferring specializations.
	 */
	template<class ConversionFactorLhs, class Tx, class ConversionFactorRhs, class Ty>
	struct common_type<wpi::units::unit<ConversionFactorLhs, Tx, wpi::units::linear_scale>, wpi::units::unit<ConversionFactorRhs, Ty, wpi::units::decibel_scale>>
	  : common_type<wpi::units::unit<ConversionFactorLhs, Tx, wpi::units::linear_scale>, wpi::units::unit<ConversionFactorRhs, Ty, wpi::units::linear_scale>>
	{
	};

	template<class ConversionFactorLhs, class Tx, class ConversionFactorRhs, class Ty>
	struct common_type<wpi::units::unit<ConversionFactorLhs, Tx, wpi::units::decibel_scale>, wpi::units::unit<ConversionFactorRhs, Ty, wpi::units::linear_scale>>
	  : common_type<wpi::units::unit<ConversionFactorLhs, Tx, wpi::units::linear_scale>, wpi::units::unit<ConversionFactorRhs, Ty, wpi::units::linear_scale>>
	{
	};

	/** @endcond */ // END DOXYGEN IGNORE
} // namespace std

namespace wpi::units
{
	//------------------------------
	//	UNIT_CAST
	//------------------------------

	/**
	 * @ingroup		Conversion
	 * @brief		Casts an unit to an arithmetic type.
	 * @details		unit_cast can be used to remove the strong typing from an unit class, and convert it
	 *				to an arithmetic type. This may be useful for compatibility with libraries
	 *				and legacy code that don't support `unit` types. E.g
	 * @code		meter_t unitVal(5);
	 *				double value = wpi::units::unit_cast<double>(unitVal);	// value == 5.0
	 * @endcode
	 * @tparam		T		Type to cast the unit type to. Shall be an arithmetic type.
	 * @tparam		Unit	Type of the unit to cast to.
	 * @param		value	Unit value to cast.
	 * @sa			unit::to
	 */
	template<ArithmeticType T, UnitType Unit>
	constexpr T unit_cast(const Unit& value) noexcept
	{
		return static_cast<T>(value);
	}

	//------------------------------
	//	NUMERICAL SCALE TRAITS
	//------------------------------

	// forward declaration
	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a linear scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_linear_scale_v<U1 [, U2, ...]>` to
		 *				test one or more types to see if they represent units whose scale is linear.
		 * @tparam		T	one or more types to test.
		 */
		template<typename... T>
		struct has_linear_scale : std::conjunction<std::is_base_of<linear_scale, T>...>
		{
		};

		template<typename... T>
		inline constexpr bool has_linear_scale_v = has_linear_scale<T...>::value;

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a decibel scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_decibel_scale_v<U1 [, U2, ...]>`
		 *				to test one or more types to see if they represent units whose scale is in decibels.
		 * @tparam		T	one or more types to test.
		 */
		template<typename... T>
		struct has_decibel_scale : std::conjunction<std::is_base_of<decibel_scale, T>...>
		{
		};

		template<typename... T>
		inline constexpr bool has_decibel_scale_v = has_decibel_scale<T...>::value;
	} // namespace traits

	//----------------------------------
	//	NUMERICAL SCALES
	//----------------------------------

	// Non-linear transforms may be used to pre- and post-scale units which are defined in terms of non-
	// linear functions of their current value. A good example of a non-linear scale would be a
	// logarithmic or decibel scale

	//------------------------------
	//	LINEAR SCALE
	//------------------------------

	/**
	 * @brief		numerical scale which is linear
	 * @details		Represents a linear numerical scale. This is the appropriate unit scale for almost
	 *				all units almost all the time.
	 * @sa			unit
	 */
	struct linear_scale
	{
		/**
		 * @brief		linearizes `value`
		 * @tparam		T	underlying type of unit
		 * @tparam[in]  value value to linearize
		 * @returns		`value`
		 */
		template<class T>
		static constexpr T linearize(const T value) noexcept
		{
			return value;
		}

		/**
		 * @brief		scales `value`
		 * @tparam		T	underlying type of unit
		 * @tparam[in]  value value to scale
		 * @returns		`value`
		 */
		template<class T>
		static constexpr T scale(const T value) noexcept
		{
			return value;
		}
	};

	//----------------------------------
	//	dimensionless (LINEAR) UNITS
	//----------------------------------

	// dimensionless units are the *ONLY* units implicitly convertible to/from built-in types.

	using dimensionless_ = conversion_factor<std::ratio<1>, dimension::dimensionless>;

	template<>
	struct traits::strong<wpi::units::detail::conversion_factor_base_t<dimensionless_>>
	{
		using type = conversion_factor<std::ratio<1>, dimension::dimensionless>;
	}; // namespace traits
	UNIT_ADD_SCALED_UNIT_DEFINITION(dimensionless, ::wpi::units::linear_scale, conversion_factor<std::ratio<1>, dimension::dimensionless>)

	UNIT_ADD_DIMENSION_TRAIT(dimensionless)

	//----------------------------------------
	//	UNIT COMPOUND ASSIGNMENT OPERATORS
	//----------------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		Helper to make the use of a template parameter a non-deduced context.
		 */
		template<class T>
		struct type_identity
		{
			using type = T;
		};

		template<class T>
		using type_identity_t = typename type_identity<T>::type;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator+=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
	constexpr UnitTypeLhs& operator+=(UnitTypeLhs& lhs, T rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator-=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
	constexpr UnitTypeLhs& operator-=(UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	}

	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator*=(UnitTypeLhs& lhs, const typename UnitTypeLhs::underlying_type& rhs) noexcept
	{
		lhs = lhs * rhs;
		return lhs;
	}

	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator/=(UnitTypeLhs& lhs, const typename UnitTypeLhs::underlying_type& rhs) noexcept
	{
		lhs = lhs / rhs;
		return lhs;
	}

	template<DimensionedUnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	template<DimensionlessUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		lhs              = CommonUnit(lhs.raw() % rhs.raw());
		return lhs;
	}

	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const typename UnitTypeLhs::underlying_type& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	//------------------------------
	//	UNIT UNARY OPERATORS
	//------------------------------

	// unary addition: +T
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs operator+(const UnitTypeLhs& u) noexcept
	{
		return u;
	}

	// prefix increment: ++T
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator++(UnitTypeLhs& u) noexcept
	{
		u = UnitTypeLhs(u.raw() + 1);
		return u;
	}

	// postfix increment: T++
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs operator++(UnitTypeLhs& u, int) noexcept
	{
		auto ret = u;
		u        = UnitTypeLhs(u.raw() + 1);
		return ret;
	}

	// unary addition: -T
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs operator-(const UnitTypeLhs& u) noexcept
	{
		return UnitTypeLhs(-u.raw());
	}

	// prefix increment: --T
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs& operator--(UnitTypeLhs& u) noexcept
	{
		u = UnitTypeLhs(u.raw() - 1);
		return u;
	}

	// postfix increment: T--
	template<UnitType UnitTypeLhs>
	constexpr UnitTypeLhs operator--(UnitTypeLhs& u, int) noexcept
	{
		auto ret = u;
		u        = UnitTypeLhs(u.raw() - 1);
		return ret;
	}

	//------------------------------
	//	LINEAR ARITHMETIC
	//------------------------------

	/// Addition operator for unit types with a linear_scale.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(CommonUnit(lhs).raw() + CommonUnit(rhs).raw());
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator+(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs + rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(lhs.raw() + InverseCommonUnit(rhs).value());
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator+(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs + rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(InverseCommonUnit(lhs).value() + rhs.raw());
	}

	/// Subtraction operator for unit types with a linear_scale.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs - rhs);
		return CommonUnit(CommonUnit(lhs).raw() - CommonUnit(rhs).raw());
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator-(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs - rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(lhs.raw() - InverseCommonUnit(rhs).value());
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator-(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs - rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(InverseCommonUnit(lhs).value() - rhs.raw());
	}

	/// Multiplication type for convertible unit types with a linear scale. @returns the multiplied value, with the same
	/// type as left-hand side unit.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<squared<typename traits::unit_traits<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>::conversion_factor>>,
			typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type>
	{
		using SquaredUnit = decltype(lhs * rhs);
		using CommonUnit  = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return SquaredUnit(CommonUnit(lhs).raw() * CommonUnit(rhs).raw());
	}

	/// Multiplication type for non-convertible unit types with a linear scale. @returns the multiplied value, whose
	/// type is a compound unit of the left and right hand side values.
	template<DimensionedUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(!same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using CompoundUnit     = decltype(lhs * rhs);
		using CommonUnderlying = typename CompoundUnit::underlying_type;
		return CompoundUnit(static_cast<CommonUnderlying>(lhs) * static_cast<CommonUnderlying>(rhs));
	}

	/// Multiplication by a dimensionless unit for unit types with a linear scale.
	template<DimensionedUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		// the cast makes sure factors of PI are handled as expected
		return CommonUnit(CommonUnit(lhs).raw() * static_cast<typename CommonUnit::underlying_type>(rhs));
	}

	/// Multiplication by a dimensionless unit for unit types with a linear scale.
	template<DimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		// the cast makes sure factors of PI are handled as expected
		return CommonUnit(static_cast<typename CommonUnit::underlying_type>(lhs) * CommonUnit(rhs).raw());
	}

	/// Multiplication by an arithmetic type for dimensioned unit types with a linear scale.
	template<DimensionedUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator*(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(CommonUnit(lhs).raw() * rhs);
	}

	/// Multiplication by an arithmetic type for dimensioned unit types with a linear scale.
	template<DimensionedUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator*(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(lhs * CommonUnit(rhs).raw());
	}

	/// Multiplication by an arithmetic type for dimensionless unit types with a linear scale.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator*(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(lhs.raw() * rhs);
	}

	/// Multiplication by an arithmetic type for dimensionless unit types with a linear scale.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator*(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(lhs * rhs.raw());
	}

	/// Division for convertible unit types with a linear scale. @returns the lhs divided by rhs value, whose type is a
	/// dimensionless
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr dimensionless<std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(lhs).raw() / CommonUnit(rhs).raw();
	}

	/// Division for non-convertible unit types with a linear scale. @returns the lhs divided by the rhs, with a
	/// compound unit type of lhs/rhs
	template<DimensionedUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(!same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using CompoundUnit     = decltype(lhs / rhs);
		using CommonUnderlying = typename CompoundUnit::underlying_type;
		return CompoundUnit(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by a dimensionless unit for unit types with a linear scale
	template<DimensionedUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator/(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit       = decltype(lhs / rhs);
		using CommonUnderlying = typename CommonUnit::underlying_type;
		return CommonUnit(CommonUnit(lhs).raw() / static_cast<CommonUnderlying>(rhs));
	}

	/// Division of a dimensionless unit by a unit type with a linear scale
	template<DimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> && traits::is_dimensionless_unit_v<UnitTypeLhs>)
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using CommonUnit       = decltype(lhs / rhs);
		using CommonUnderlying = typename CommonUnit::underlying_type;
		return CommonUnit(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by a dimensionless for unit types with a linear scale
	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator/(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = decltype(lhs / rhs);
		return CommonUnit(CommonUnit(lhs).raw() / rhs);
	}

	/// Division of a dimensionless by a unit type with a linear scale
	template<UnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs>)
	constexpr auto operator/(T lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>, std::common_type_t<T, typename UnitTypeRhs::underlying_type>>
	{
		using InverseUnit      = decltype(lhs / rhs);
		using UnitConversion   = typename traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		using CommonUnit       = unit<UnitConversion, CommonUnderlying>;
		return InverseUnit(lhs / CommonUnit(rhs).raw());
	}

	/// Modulo for convertible unit types with a linear scale. @returns the lhs value modulo the rhs value, whose type
	/// is their common type
	template<DimensionedUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type> operator%(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		return CommonUnit(CommonUnit(lhs).raw() % CommonUnit(rhs).raw());
	}

	/// Modulo by a dimensionless for unit types with a linear scale
	template<DimensionedUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator%(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit       = decltype(lhs % rhs);
		using CommonUnderlying = typename CommonUnit::underlying_type;
		return CommonUnit(CommonUnit(lhs).raw() % static_cast<CommonUnderlying>(rhs));
	}

	/// Modulo for two dimensionless unit types with a linear scale.
	/// @returns the lhs value modulo the rhs value, whose type is their common type
	template<DimensionlessUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type> operator%(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		return CommonUnit(lhs.raw() % rhs.raw());
	}

	/// Modulo by an arithmetic type for unit types with a linear scale
	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator%(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		return CommonUnit(CommonUnit(lhs).raw() % rhs);
	}

	//----------------------------------
	//	DIMENSIONLESS COMPARISONS
	//----------------------------------

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
	constexpr bool operator==(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;

		const auto common_lhs = static_cast<CommonUnderlying>(lhs);
		const auto common_rhs = static_cast<CommonUnderlying>(rhs);

		if constexpr (std::is_integral_v<CommonUnderlying>)
		{
			return common_lhs == common_rhs;
		}
		else
		{
			return abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::epsilon() * abs(common_lhs + common_rhs) ||
				abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::min();
		}
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator==(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		return rhs == lhs;
	}

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::is_dimensionless_unit_v<UnitTypeRhs> && std::is_arithmetic_v<T>)
	constexpr bool operator!=(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator!=(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::is_dimensionless_unit_v<UnitTypeRhs> && std::is_arithmetic_v<T>)
	constexpr bool operator>=(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		return lhs >= static_cast<CommonUnderlying>(rhs);
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator>=(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitTypeLhs::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) >= rhs;
	}

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
	constexpr bool operator>(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		return lhs > static_cast<CommonUnderlying>(rhs);
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator>(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitTypeLhs::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) > rhs;
	}

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
	constexpr bool operator<=(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		return lhs <= static_cast<CommonUnderlying>(rhs);
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator<=(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitTypeLhs::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) <= rhs;
	}

	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
	constexpr bool operator<(const T& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		return lhs < static_cast<CommonUnderlying>(rhs);
	}

	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
	constexpr bool operator<(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitTypeLhs::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) < rhs;
	}

	//----------------------------------
	//	POW
	//----------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/// recursive exponential implementation
		template<int N, class U>
		struct power_of_unit
		{
			template<bool isPos, int V>
			struct power_of_unit_impl;

			template<int V>
			struct power_of_unit_impl<true, V>
			{
				typedef unit_multiply<U, typename power_of_unit<N - 1, U>::type> type;
			};

			template<int V>
			struct power_of_unit_impl<false, V>
			{
				typedef inverse<typename power_of_unit<-N, U>::type> type;
			};

			typedef typename power_of_unit_impl<(N > 0), N>::type type;
		};

		/// End recursion
		template<class U>
		struct power_of_unit<1, U>
		{
			typedef U type;
		};

		template<class U>
		struct power_of_unit<0, U>
		{
			typedef dimensionless_ type;
		};
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		computes the value of <i>value</i> raised to the <i>power</i>
	 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the
	 *				resulting unit type can be deduced.
	 * @tparam		power exponential power to raise <i>value</i> by.
	 * @param[in]	value `unit` derived type to raise to the given <i>power</i>
	 * @returns		new unit, raised to the given exponent
	 */
	template<int power, UnitType UnitType>
		requires(traits::has_linear_scale_v<UnitType>)
	constexpr auto pow(const UnitType& value) noexcept -> unit<traits::strong_t<typename wpi::units::detail::power_of_unit<power, typename wpi::units::traits::unit_traits<UnitType>::conversion_factor>::type>,
		detail::floating_point_promotion_t<typename wpi::units::traits::unit_traits<UnitType>::underlying_type>, linear_scale>
	{
		return decltype(wpi::units::pow<power>(value))(pow<power>(value.raw()));
	}

	//------------------------------
	//	DECIBEL SCALE
	//------------------------------

	/**
	 * @brief		numerical scale which is decibel
	 * @details		Represents a decibel numerical scale. Scales a value to dB.
	 * @sa			unit
	 */
	struct decibel_scale
	{
		/**
		 * @brief		linearizes `value`
		 * @tparam		T	underlying type of unit
		 * @tparam[in]  value value to linearize
		 * @returns		`std::pow(10, value / 10)`
		 */
		template<class T>
		static T linearize(const T value) noexcept
		{
			return static_cast<T>(std::pow(10, value / 10));
		}

		/**
		 * @brief		returns `value` in dB
		 * @tparam		T	underlying type of unit
		 * @tparam[in]  value value to scale
		 * @returns		`10 * std::log10(value)`
		 */
		template<class T>
		static T scale(const T value) noexcept
		{
			return static_cast<T>(10 * std::log10(value));
		}
	};

	//------------------------------
	//	dimensionless (DECIBEL) UNITS
	//------------------------------

	/**
	 * @brief		dimensionless unit with decibel scale
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_SCALED_UNIT_DEFINITION(decibels, ::wpi::units::decibel_scale, dimensionless_)
	template<class Underlying>
	using decibels = unit<dimensionless_, Underlying, decibel_scale>;
#if defined(UNIT_LIB_ENABLE_IOSTREAM)
	template<class Underlying>
	std::ostream& operator<<(std::ostream& os, const decibels<Underlying>& obj)
	{
		os << obj.raw() << " dB";
		return os;
	}
#endif
	template<class Underlying>
	using dBi = decibels<Underlying>;

	//------------------------------
	//	DECIBEL ARITHMETIC
	//------------------------------

	/// Addition for convertible unit types with a decibel_scale
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<squared<typename traits::unit_traits<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>::conversion_factor>>,
			typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type, decibel_scale>
	{
		using SquaredUnit = decltype(lhs + rhs);
		using CommonUnit  = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;

		return SquaredUnit(CommonUnit(lhs).to_linearized() * CommonUnit(rhs).to_linearized(), linearized_value);
	}

	/// Addition between unit types with a decibel_scale and dimensionless dB units
	template<DimensionedUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator+(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs + rhs);
		return CommonUnit(lhs.to_linearized() * rhs.to_linearized(), linearized_value);
	}

	/// Addition between unit types with a decibel_scale and dimensionless dB units
	template<DimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator+(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs + rhs);
		return CommonUnit(lhs.to_linearized() * rhs.to_linearized(), linearized_value);
	}

	/// Subtraction for convertible unit types with a decibel_scale
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> decibels<typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type>
	{
		using Dimensionless = decltype(lhs - rhs);
		using CommonUnit    = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;

		return Dimensionless(CommonUnit(lhs).to_linearized() / CommonUnit(rhs).to_linearized(), linearized_value);
	}

	/// Subtraction between unit types with a decibel_scale and dimensionless dB units
	template<DimensionedUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator-(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs - rhs);
		return CommonUnit(lhs.to_linearized() / rhs.to_linearized(), linearized_value);
	}

	/// Subtraction between unit types with a decibel_scale and dimensionless dB units
	template<DimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>, decibel_scale>
	{
		using InverseUnit = decltype(lhs - rhs);
		return InverseUnit(lhs.to_linearized() / rhs.to_linearized(), linearized_value);
	}

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	//----------------------------------
	//	MIN/MAX FUNCTIONS
	//----------------------------------

	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> min(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		using CommonUnit = decltype(wpi::units::min(lhs, rhs));
		return (lhs < rhs ? CommonUnit(lhs) : CommonUnit(rhs));
	}

	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> max(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		using CommonUnit = decltype(wpi::units::max(lhs, rhs));
		return (lhs > rhs ? CommonUnit(lhs) : CommonUnit(rhs));
	}

	//----------------------------------
	//	TRANSCENDENTAL FUNCTIONS
	//----------------------------------

	// it makes NO SENSE to put dimensioned units into a transcendental function, and if you think it does you are
	// demonstrably wrong. https://en.wikipedia.org/wiki/Transcendental_function#Dimensional_analysis

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute exponential function
	 * @details		Returns the base-e exponential function of x, which is e raised to the power x: ex.
	 * @param[in]	x	dimensionless value of the exponent.
	 * @returns		Exponential value of x.
	 *				If the magnitude of the result is too large to be represented by a value of the return type, the
	 *				function returns HUGE_VAL (or HUGE_VALF or HUGE_VALL) with the proper sign, and an overflow range
	 *				error occurs
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> exp(const UnitType x) noexcept
	{
		return gcem::exp(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute natural logarithm
	 * @details		Returns the natural logarithm of x.
	 * @param[in]	x	dimensionless value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @sa			log10 for more common base-10 logarithms
	 * @returns		Natural logarithm of x.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> log(const UnitType x) noexcept
	{
		return gcem::log(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute common logarithm
	 * @details		Returns the common (base-10) logarithm of x.
	 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @returns		Common logarithm of x.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> log10(const UnitType x) noexcept
	{
		return gcem::log10(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Break into fractional and integral parts.
	 * @details		The integer part is stored in the object pointed by intpart, and the
	 *				fractional part is returned by the function. Both parts have the same sign
	 *				as x.
	 * @param[in]	x		dimensionless value to break into parts.
	 * @param[in]	intpart Pointer to an object (of the same type as x) where the integral part
	 *				is stored with the same sign as x.
	 * @returns		The fractional part of x, with the same sign.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> modf(const UnitType x, UnitType* intpart) noexcept
	{
		typename UnitType::underlying_type intp;
		UnitType                           fracpart = std::modf(x.template to<decltype(intp)>(), &intp);
		*intpart                                    = intp;
		return fracpart;
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute binary exponential function
	 * @details		Returns the base-2 exponential function of x, which is 2 raised to the power x: 2^x.
	 * @param[in]	x	Value of the exponent.
	 * @returns		2 raised to the power of x.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> exp2(const UnitType x) noexcept
	{
		return std::exp2(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute exponential minus one
	 * @details		Returns e raised to the power x minus one: e^x-1. For small magnitude values
	 *				of x, expm1 may be more accurate than exp(x)-1.
	 * @param[in]	x	Value of the exponent.
	 * @returns		e raised to the power of x, minus one.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> expm1(const UnitType x) noexcept
	{
		return gcem::expm1(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute logarithm plus one
	 * @details		Returns the natural logarithm of one plus x. For small magnitude values of
	 *				x, logp1 may be more accurate than log(1+x).
	 * @param[in]	x	Value whose logarithm is calculated. If the argument is less than -1, a
	 *					domain error occurs.
	 * @returns		The natural logarithm of (1+x).
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> log1p(const UnitType x) noexcept
	{
		return gcem::log1p(x.value());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute binary logarithm
	 * @details		Returns the binary (base-2) logarithm of x.
	 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @returns		The binary logarithm of x: log2x.
	 */
	template<DimensionlessUnitType UnitType>
	constexpr dimensionless<detail::floating_point_promotion_t<typename UnitType::underlying_type>> log2(const UnitType x) noexcept
	{
		return gcem::log2(x.value());
	}

	//----------------------------------
	//	POWER FUNCTIONS
	//----------------------------------

	/* pow is implemented earlier in the library since a lot of the unit definitions depend on it */

	/**
	 * @ingroup		UnitMath
	 * @brief		computes the square root of <i>value</i>
	 * @details		Only implemented for linear_scale units.
	 * @param[in]	value `unit` derived type to compute the square root of.
	 * @returns		new unit, whose units are the square root of value's. E.g. if values
	 *				had units of `square_meter`, then the return type will have units of
	 *				`meter`.
	 * @note		`sqrt` provides a _rational approximation_ of the square root of <i>value</i>.
	 *				In some cases, _both_ the returned value _and_ conversion factor of the returned
	 *				unit type may have errors no larger than `1e-10`.
	 */
	template<UnitType UnitType>
		requires(traits::has_linear_scale_v<UnitType>)
	constexpr auto sqrt(const UnitType& value) noexcept
		-> unit<traits::strong_t<square_root<typename traits::unit_traits<UnitType>::conversion_factor>>, detail::floating_point_promotion_t<typename traits::unit_traits<UnitType>::underlying_type>>
	{
		return decltype(wpi::units::sqrt(value))(sqrt(value.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Computes the square root of the sum-of-squares of x and y.
	 * @details		Only implemented for linear_scale units.
	 * @param[in]	x	unit type value
	 * @param[in]	y	unit type value
	 * @returns		square root of the sum-of-squares of x and y in the same units
	 *				as x.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> hypot(const UnitTypeLhs& x, const UnitTypeRhs& y)
	{
		using CommonUnit = decltype(wpi::units::hypot(x, y));
		return CommonUnit(gcem::hypot(CommonUnit(x).value(), CommonUnit(y).value()));
	}

	//----------------------------------
	//	ROUNDING FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Round up value
	 * @details		Rounds x upward, returning the smallest integral value that is not less than x.
	 * @param[in]	x	Unit value to round up.
	 * @returns		The smallest integral value that is not less than x.
	 */
	template<UnitType UnitType>
	constexpr detail::floating_point_promotion_t<UnitType> ceil(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(gcem::ceil(x.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Round down value
	 * @details		Rounds x downward, returning the largest integral value that is not greater than x.
	 * @param[in]	x	Unit value to round down.
	 * @returns		The value of x rounded downward.
	 */
	template<UnitType UnitType>
	constexpr detail::floating_point_promotion_t<UnitType> floor(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(gcem::floor(x.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute remainder of division
	 * @details		Returns the floating-point remainder of numer/denom (rounded towards zero).
	 * @param[in]	numer	Value of the quotient numerator.
	 * @param[in]	denom	Value of the quotient denominator.
	 * @returns		The remainder of dividing the arguments.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmod(const UnitTypeLhs numer, const UnitTypeRhs denom) noexcept
	{
		using CommonUnit = decltype(wpi::units::fmod(numer, denom));
		return CommonUnit(gcem::fmod(CommonUnit(numer).value(), CommonUnit(denom).value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Truncate value
	 * @details		Rounds x toward zero, returning the nearest integral value that is not
	 *				larger in magnitude than x. Effectively rounds towards 0.
	 * @param[in]	x	Value to truncate
	 * @returns		The nearest integral value that is not larger in magnitude than x.
	 */
	template<UnitType UnitType>
	constexpr detail::floating_point_promotion_t<UnitType> trunc(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(gcem::trunc(x.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Round to nearest
	 * @details		Returns the integral value that is nearest to x, with halfway cases rounded
	 *				away from zero.
	 * @param[in]	x	value to round.
	 * @returns		The value of x rounded to the nearest integral.
	 */
	template<UnitType UnitType>
	constexpr detail::floating_point_promotion_t<UnitType> round(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(gcem::round(x.value()));
	}

	//----------------------------------
	//	FLOATING POINT MANIPULATION
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Copy sign
	 * @details		Returns a value with the magnitude and dimension of x, and the sign of y.
	 *				Values x and y do not have to be compatible units.
	 * @param[in]	x	Value with the magnitude of the resulting value.
	 * @param[in]	y	Value with the sign of the resulting value.
	 * @returns		value with the magnitude and dimension of x, and the sign of y.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
	constexpr detail::floating_point_promotion_t<UnitTypeLhs> copysign(const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		return detail::floating_point_promotion_t<UnitTypeLhs>(gcem::copysign(x.value(), y.value())); // no need for conversion to get the correct sign.
	}

	/// Overload to copy the sign from a raw double
	template<UnitType UnitTypeLhs, ArithmeticType T>
	constexpr detail::floating_point_promotion_t<UnitTypeLhs> copysign(const UnitTypeLhs x, const T& y) noexcept
	{
		return detail::floating_point_promotion_t<UnitTypeLhs>(gcem::copysign(x.value(), y));
	}

	//----------------------------------
	//	MIN / MAX / DIFFERENCE
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Positive difference
	 * @details		The function returns x-y if x>y, and zero otherwise, in their common type.
	 * @param[in]	x	Values whose difference is calculated.
	 * @param[in]	y	Values whose difference is calculated.
	 * @returns		The positive difference between x and y.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fdim(const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = decltype(wpi::units::fdim(x, y));
		return CommonUnit(std::fdim(CommonUnit(x).value(), CommonUnit(y).value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Maximum value
	 * @details		Returns the larger of its arguments: either x or y, in their common type.
	 * @param[in]	x	Values among which the function selects a maximum.
	 * @param[in]	y	Values among which the function selects a maximum.
	 * @returns		The maximum numeric value of its arguments.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmax(const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = decltype(wpi::units::fmax(x, y));
		if (std::is_constant_evaluated())
		{
			using UnderlyingType = CommonUnit::underlying_type;
			UnderlyingType xval = CommonUnit(x).value();
			UnderlyingType yval = CommonUnit(y).value();
			// x is NaN, return y (whether or not y is NaN)
			if (xval != xval)
			{
				return CommonUnit(yval);
			}
			// y is NaN, return x
			else if (yval != yval)
			{
				return CommonUnit(xval);
			}
			// non-NaN values, safe to use normal max
			else
			{
				return CommonUnit(gcem::max(xval, yval));
			}
		}
		else
		{
			return CommonUnit(std::fmax(CommonUnit(x).value(), CommonUnit(y).value()));
		}
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Minimum value
	 * @details		Returns the smaller of its arguments: either x or y, in their common type.
	 *				If one of the arguments in a NaN, the other is returned.
	 * @param[in]	x	Values among which the function selects a minimum.
	 * @param[in]	y	Values among which the function selects a minimum.
	 * @returns		The minimum numeric value of its arguments.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmin(const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = decltype(wpi::units::fmin(x, y));
		if (std::is_constant_evaluated()) {
			using UnderlyingType = CommonUnit::underlying_type;
			UnderlyingType xval = CommonUnit(x).value();
			UnderlyingType yval = CommonUnit(y).value();
			// x is NaN, return y (whether or not y is NaN)
			if (xval != xval)
			{
				return CommonUnit(yval);
			}
			// y is NaN, return x
			else if (yval != yval)
			{
				return CommonUnit(xval);
			}
			// non-NaN values, safe to use normal min
			{
				return CommonUnit(gcem::min(xval, yval));
			}
		}
		else
		{
			return CommonUnit(std::fmin(CommonUnit(x).value(), CommonUnit(y).value()));
		}
	}

	//----------------------------------
	//	OTHER FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute absolute value
	 * @details		Returns the absolute value of x, i.e. |x|.
	 * @param[in]	x	Value whose absolute value is returned.
	 * @returns		The absolute value of x.
	 */
	template<UnitType UnitType>
	constexpr detail::floating_point_promotion_t<UnitType> fabs(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(gcem::fabs(x.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute absolute value
	 * @details		Returns the absolute value of x, i.e. |x|.
	 * @param[in]	x	Value whose absolute value is returned.
	 * @returns		The absolute value of x.
	 */
	template<UnitType UnitType>
	constexpr UnitType abs(const UnitType x) noexcept
	{
		return UnitType(gcem::abs(x.value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Multiply-add
	 * @details		Returns x*y+z. The function computes the result without losing precision in
	 *				any intermediate result. The resulting unit type is a compound unit of x* y.
	 * @param[in]	x	Values to be multiplied.
	 * @param[in]	y	Values to be multiplied.
	 * @param[in]	z	Value to be added.
	 * @returns		The result of x*y+z
	 */
	template<UnitType UnitTypeLhs, UnitType UnitMultiply, UnitType UnitAdd>
		requires(traits::is_same_dimension_conversion_factor_v<
			compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, typename traits::unit_traits<UnitMultiply>::conversion_factor>,
			typename traits::unit_traits<UnitAdd>::conversion_factor>)
	constexpr auto fma(const UnitTypeLhs x, const UnitMultiply y, const UnitAdd z) noexcept
		-> std::common_type_t<decltype(detail::floating_point_promotion_t<UnitTypeLhs>(x) * detail::floating_point_promotion_t<UnitMultiply>(y)), UnitAdd>
	{
		using CommonUnit = decltype(wpi::units::fma(x, y, z));
		return CommonUnit(std::fma(x.value(), y.value(), CommonUnit(z).value()));
	}

	//----------------------------
	//  NAN support
	//----------------------------

	template<UnitType UnitType>
	constexpr bool isnan(const UnitType& x) noexcept
	{
		return std::isnan(x.value());
	}

	template<UnitType UnitType>
	constexpr bool isinf(const UnitType& x) noexcept
	{
		return std::isinf(x.value());
	}

	template<UnitType UnitType>
	constexpr bool isfinite(const UnitType& x) noexcept
	{
		return std::isfinite(x.value());
	}

	template<UnitType UnitType>
	constexpr bool isnormal(const UnitType& x) noexcept
	{
		return std::isnormal(x.value());
	}

	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr bool isunordered(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		return std::isunordered(lhs.value(), rhs.value());
	}
} // end namespace wpi::units

#if __has_include(<fmt/format.h>) && !defined(UNITS_LIB_DISABLE_FMT)

template<class Underlying>
struct fmt::formatter<wpi::units::decibels<Underlying>> : fmt::formatter<double>
{
	template <typename FmtContext>
	auto format(const wpi::units::decibels<Underlying>& obj, FmtContext& ctx) const
	{
		auto out = formatter<double>::format(obj.raw(), ctx);
		out = fmt::format_to(out, " dB");
		return out;
	}
};

#endif // __has_include(<fmt/format.h>) && !defined(UNITS_LIB_DISABLE_FMT)

//----------------------------------------------------------------------------------------------------------------------
//      STD Namespace extensions
//----------------------------------------------------------------------------------------------------------------------

//------------------------------
//	std::hash
//------------------------------

template<class ConversionFactor, typename T, class NumericalScale>
struct std::hash<wpi::units::unit<ConversionFactor, T, NumericalScale>>
{
	template<typename U = T>
	constexpr std::size_t operator()(const wpi::units::unit<ConversionFactor, T, NumericalScale>& x) const noexcept
	{
		if constexpr (std::is_integral_v<U>)
		{
			return static_cast<std::size_t>(x.to_linearized());
		}
		else
		{
			return static_cast<std::size_t>(hash<T>()(x.to_linearized()));
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
//  NUMERIC LIMITS
//----------------------------------------------------------------------------------------------------------------------

namespace std
{
	template<wpi::units::ConversionFactorType ConversionFactor, wpi::units::ArithmeticType T, wpi::units::NumericalScaleType<T> NonLinearScale>
	struct numeric_limits<wpi::units::unit<ConversionFactor, T, NonLinearScale>>
	{
		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> min()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::min());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> denorm_min() noexcept
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::denorm_min());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> max()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::max());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> lowest()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::lowest());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> epsilon()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::epsilon());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> round_error()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::round_error());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> infinity()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::infinity());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> quiet_NaN()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::quiet_NaN());
		}

		static constexpr wpi::units::unit<ConversionFactor, T, NonLinearScale> signaling_NaN()
		{
			return wpi::units::unit<ConversionFactor, T, NonLinearScale>(std::numeric_limits<T>::signaling_NaN());
		}

		static constexpr bool is_specialized    = std::numeric_limits<T>::is_specialized;
		static constexpr bool is_signed         = std::numeric_limits<T>::is_signed;
		static constexpr bool is_integer        = std::numeric_limits<T>::is_integer;
		static constexpr bool is_exact          = std::numeric_limits<T>::is_exact;
		static constexpr bool has_infinity      = std::numeric_limits<T>::has_infinity;
		static constexpr bool has_quiet_NaN     = std::numeric_limits<T>::has_quiet_NaN;
		static constexpr bool has_signaling_NaN = std::numeric_limits<T>::has_signaling_NaN;
	};

	template<wpi::units::ConversionFactorType ConversionFactor, wpi::units::ArithmeticType T, wpi::units::NumericalScaleType<T> NonLinearScale>
	constexpr bool isnan(const wpi::units::unit<ConversionFactor, T, NonLinearScale>& x)
	{
		return std::isnan(x());
	}

	template<wpi::units::ConversionFactorType ConversionFactor, wpi::units::ArithmeticType T, wpi::units::NumericalScaleType<T> NonLinearScale>
	constexpr bool isinf(const wpi::units::unit<ConversionFactor, T, NonLinearScale>& x)
	{
		return std::isinf(x());
	}

	template<wpi::units::ConversionFactorType ConversionFactor, wpi::units::ArithmeticType T, wpi::units::NumericalScaleType<T> NonLinearScale>
	constexpr bool signbit(const wpi::units::unit<ConversionFactor, T, NonLinearScale>& x)
	{
		return gcem::signbit(x());
	}
} // namespace std

//------------------------------
//	UNIT DEDUCTION GUIDES
//------------------------------

namespace wpi::units
{
	// Concept to ensure we only apply the dimensionless fallback
	// to a pure, unmodified dimensionless unit.
	template<class Cf>
	concept PureDimensionlessCF = std::is_same_v<typename Cf::dimension_type, dimension::dimensionless> && std::ratio_equal_v<typename Cf::conversion_ratio, std::ratio<1>> &&
		std::ratio_equal_v<typename Cf::pi_exponent_ratio, std::ratio<0>> && std::ratio_equal_v<typename Cf::translation_ratio, std::ratio<0>>;

	// 1) chrono deduction guide
	template<ArithmeticType Rep, RatioType Period>
	unit(std::chrono::duration<Rep, Period>) -> unit<conversion_factor<Period, dimension::time>, Rep>;

	// 2) Dimensionless fallback:
	// Only applies if the source is exactly the base dimensionless unit.
	template<ArithmeticType SourceTy, ConversionFactorType SourceCf>
		requires(traits::is_unit_v<unit<SourceCf, SourceTy>> && PureDimensionlessCF<SourceCf>)
	unit(const unit<SourceCf, SourceTy>&) -> unit<conversion_factor<std::ratio<1>, dimension::dimensionless>, SourceTy>;

	// 3) Lossless integral conversion:
	// For dimensionally compatible units where the conversion is integral and lossless.
	// This applies only if is_losslessly_convertible_unit is true.
	template<ArithmeticType SourceTy, ConversionFactorType SourceCf, ConversionFactorType TargetCf = SourceCf>
		requires(traits::is_unit_v<unit<SourceCf, SourceTy>> && traits::is_conversion_factor_v<TargetCf> && traits::is_same_dimension_conversion_factor_v<SourceCf, TargetCf> &&
			!std::is_same_v<SourceCf, TargetCf> && detail::is_losslessly_convertible_unit<unit<SourceCf, SourceTy>, unit<TargetCf, SourceTy>>)
	unit(const unit<SourceCf, SourceTy>&) -> unit<TargetCf, SourceTy>;

	// 4) Non-lossless conversions:
	// For dimensionally compatible units where integral conversion is not possible.
	// Falls back to floating point.
	template<ArithmeticType SourceTy, ConversionFactorType SourceCf, ConversionFactorType TargetCf = SourceCf>
		requires(traits::is_unit_v<unit<SourceCf, SourceTy>> && traits::is_conversion_factor_v<TargetCf> && traits::is_same_dimension_conversion_factor_v<SourceCf, TargetCf> &&
			!std::is_same_v<SourceCf, TargetCf> && !detail::is_losslessly_convertible_unit<unit<SourceCf, SourceTy>, unit<TargetCf, SourceTy>>)
	unit(const unit<SourceCf, SourceTy>&) -> unit<TargetCf, detail::floating_point_promotion_t<SourceTy>>;

	// 5) Exact matches:
	// If the unit already matches `unit<TargetCf, SourceTy>`, use it directly.
	template<ConversionFactorType TargetCf, ArithmeticType SourceTy>
		requires traits::is_unit_v<unit<TargetCf, SourceTy>>
	unit(const unit<TargetCf, SourceTy>&) -> unit<TargetCf, SourceTy>;

	// 6) Deduce type from arithmetic type (dimensionless by default)
	template<typename T, typename Cf = dimension::dimensionless, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	unit(T) -> unit<Cf, T>;
} // namespace wpi::units

//----------------------------------------------------------------------------------------------------------------------
//  JSON SUPPORT
//----------------------------------------------------------------------------------------------------------------------

#if defined __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
namespace wpi::units
{
	template<class UnitType>
		requires(wpi::units::traits::is_unit_v<UnitType>)
	void from_json(const nlohmann::json& j, UnitType& u)
	{
		using underlying = typename wpi::units::traits::unit_traits<UnitType>::underlying_type;
		underlying value;
		j.get_to(value);
		u = UnitType(value);
	}

	template<class UnitType>
		requires(wpi::units::traits::is_unit_v<UnitType>)
	void to_json(nlohmann::json& j, const UnitType& u)
	{
		j = u.raw();
	}
} // namespace wpi::units
#endif
#endif

#ifndef UNIT_NO_LITERAL_SUPPORT
namespace wpi::units::literals {}
using namespace wpi::units::literals;
#endif

#endif // UNIT_CORE_H
