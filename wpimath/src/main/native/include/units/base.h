/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

// Copyright (c) 2016 Nic Holthaus
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ATTRIBUTION:
// Parts of this work have been adapted from:
// http://stackoverflow.com/questions/35069778/create-comparison-trait-for-template-classes-whose-parameters-are-in-a-different
// http://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments/28253503
// http://stackoverflow.com/questions/36321295/rational-approximation-of-square-root-of-stdratio-at-compile-time?noredirect=1#comment60266601_36321295
//

/// @file	units.h
/// @brief Complete implementation of `units` - a compile-time, header-only,
///        unit conversion library built on c++14 with no dependencies.

#pragma once

#ifdef _MSC_VER
#	pragma push_macro("pascal")
#	undef pascal
#	if _MSC_VER <= 1800
#		define _ALLOW_KEYWORD_MACROS
#		pragma warning(push)
#		pragma warning(disable : 4520)
#		pragma push_macro("constexpr")
#		define constexpr /*constexpr*/
#		pragma push_macro("noexcept")
#		define noexcept throw()
#	endif // _MSC_VER < 1800
#endif // _MSC_VER

#if !defined(_MSC_VER) || _MSC_VER > 1800
#   define UNIT_HAS_LITERAL_SUPPORT
#   define UNIT_HAS_VARIADIC_TEMPLATE_SUPPORT
#endif

#ifndef UNIT_LIB_DEFAULT_TYPE
#   define UNIT_LIB_DEFAULT_TYPE double
#endif

//--------------------
//	INCLUDES
//--------------------

#include <chrono>
#include <ratio>
#include <type_traits>
#include <cstdint>
#include <cmath>
#include <limits>

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	#include <iostream>
	#include <string>
	#include <locale>

	//------------------------------
	//	STRING FORMATTER
	//------------------------------

	namespace units
	{
		namespace detail
		{
			template <typename T> std::string to_string(const T& t)
			{
				std::string str{ std::to_string(t) };
				int offset{ 1 };

				// remove trailing decimal points for integer value units. Locale aware!
				struct lconv * lc;
				lc = localeconv();
				char decimalPoint = *lc->decimal_point;
				if (str.find_last_not_of('0') == str.find(decimalPoint)) { offset = 0; }
				str.erase(str.find_last_not_of('0') + offset, std::string::npos);
				return str;
			}
		}
	}
#endif

namespace units
{
	template<typename T> inline constexpr const char* name(const T&);
	template<typename T> inline constexpr const char* abbreviation(const T&);
}

//------------------------------
//	MACROS
//------------------------------

/**
 * @def			UNIT_ADD_UNIT_TAGS(namespaceName,nameSingular, namePlural, abbreviation, definition)
 * @brief		Helper macro for generating the boiler-plate code generating the tags of a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as aliases for the
 *				unit tag.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `unit<std::ratio<1>, units::category::length_unit>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_UNIT_TAGS(namespaceName,nameSingular, namePlural, abbreviation, /*definition*/...)\
	namespace namespaceName\
	{\
	/** @name Units (full names plural) */ /** @{ */ typedef __VA_ARGS__ namePlural; /** @} */\
	/** @name Units (full names singular) */ /** @{ */ typedef namePlural nameSingular; /** @} */\
	/** @name Units (abbreviated) */ /** @{ */ typedef namePlural abbreviation; /** @} */\
	}

/**
 * @def			UNIT_ADD_UNIT_DEFINITION(namespaceName,nameSingular)
 * @brief		Macro for generating the boiler-plate code for the unit_t type definition.
 * @details		The macro generates the definition of the unit container types, e.g. `meter_t`
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 */
#define UNIT_ADD_UNIT_DEFINITION(namespaceName,nameSingular)\
	namespace namespaceName\
	{\
		/** @name Unit Containers */ /** @{ */ typedef unit_t<nameSingular> nameSingular ## _t; /** @} */\
	}

/**
 * @def			UNIT_ADD_CUSTOM_TYPE_UNIT_DEFINITION(namespaceName,nameSingular,underlyingType)
 * @brief		Macro for generating the boiler-plate code for a unit_t type definition with a non-default underlying type.
 * @details		The macro generates the definition of the unit container types, e.g. `meter_t`
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		underlyingType the underlying type
 */
#define UNIT_ADD_CUSTOM_TYPE_UNIT_DEFINITION(namespaceName,nameSingular, underlyingType)\
	namespace namespaceName\
	{\
	/** @name Unit Containers */ /** @{ */ typedef unit_t<nameSingular,underlyingType> nameSingular ## _t; /** @} */\
	}
/**
 * @def			UNIT_ADD_IO(namespaceName,nameSingular, abbreviation)
 * @brief		Macro for generating the boiler-plate code needed for I/O for a new unit.
 * @details		The macro generates the code to insert units into an ostream. It
 *				prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		abbrev - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_LIB_DISABLE_IOSTREAM is defined, the macro does not generate any code
 */
#if defined(UNIT_LIB_DISABLE_IOSTREAM)
	#define UNIT_ADD_IO(namespaceName, nameSingular, abbrev)
#else
	#define UNIT_ADD_IO(namespaceName, nameSingular, abbrev)\
	namespace namespaceName\
	{\
		inline std::ostream& operator<<(std::ostream& os, const nameSingular ## _t& obj) \
		{\
			os << obj() << " "#abbrev; return os; \
		}\
		inline std::string to_string(const nameSingular ## _t& obj)\
		{\
			return units::detail::to_string(obj()) + std::string(" "#abbrev);\
		}\
	}
#endif

 /**
  * @def		UNIT_ADD_NAME(namespaceName,nameSingular,abbreviation)
  * @brief		Macro for generating constexpr names/abbreviations for units.
  * @details	The macro generates names for units. E.g. name() of 1_m would be "meter", and
  *				abbreviation would be "m".
  * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
  *				are placed in the `units::literals` namespace.
  * @param		nameSingular singular version of the unit name, e.g. 'meter'
  * @param		abbreviation - abbreviated unit name, e.g. 'm'
  */
#define UNIT_ADD_NAME(namespaceName, nameSingular, abbrev)\
template<> inline constexpr const char* name(const namespaceName::nameSingular ## _t&)\
{\
	return #nameSingular;\
}\
template<> inline constexpr const char* abbreviation(const namespaceName::nameSingular ## _t&)\
{\
	return #abbrev;\
}

/**
 * @def			UNIT_ADD_LITERALS(namespaceName,nameSingular,abbreviation)
 * @brief		Macro for generating user-defined literals for units.
 * @details		The macro generates user-defined literals for units. A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_HAS_LITERAL_SUPPORT is not defined, the macro does not generate any code
 */
#if defined(UNIT_HAS_LITERAL_SUPPORT)
	#define UNIT_ADD_LITERALS(namespaceName, nameSingular, abbreviation)\
	namespace literals\
	{\
		inline constexpr namespaceName::nameSingular ## _t operator""_ ## abbreviation(long double d)\
		{\
			return namespaceName::nameSingular ## _t(static_cast<namespaceName::nameSingular ## _t::underlying_type>(d));\
		}\
		inline constexpr namespaceName::nameSingular ## _t operator""_ ## abbreviation (unsigned long long d)\
		{\
			return namespaceName::nameSingular ## _t(static_cast<namespaceName::nameSingular ## _t::underlying_type>(d));\
		}\
	}
#else
	#define UNIT_ADD_LITERALS(namespaceName, nameSingular, abbreviation)
#endif

/**
 * @def			UNIT_ADD(namespaceName,nameSingular, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as well as the
 *				appropriately named unit container (e.g. `meter_t`). A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`). It also defines a class-specific
 *				cout function which prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `unit<std::ratio<1>, units::category::length_unit>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...)\
	UNIT_ADD_UNIT_TAGS(namespaceName,nameSingular, namePlural, abbreviation, __VA_ARGS__)\
	UNIT_ADD_UNIT_DEFINITION(namespaceName,nameSingular)\
	UNIT_ADD_NAME(namespaceName,nameSingular, abbreviation)\
	UNIT_ADD_IO(namespaceName,nameSingular, abbreviation)\
	UNIT_ADD_LITERALS(namespaceName,nameSingular, abbreviation)

/**
 * @def			UNIT_ADD_WITH_CUSTOM_TYPE(namespaceName,nameSingular, namePlural, abbreviation, underlyingType, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit with a non-default underlying type.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as well as the
 *				appropriately named unit container (e.g. `meter_t`). A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`). It also defines a class-specific
 *				cout function which prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		underlyingType - the underlying type, e.g. 'int' or 'float'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `unit<std::ratio<1>, units::category::length_unit>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_CUSTOM_TYPE(namespaceName, nameSingular, namePlural, abbreviation, underlyingType, /*definition*/...)\
	UNIT_ADD_UNIT_TAGS(namespaceName,nameSingular, namePlural, abbreviation, __VA_ARGS__)\
	UNIT_ADD_CUSTOM_TYPE_UNIT_DEFINITION(namespaceName,nameSingular,underlyingType)\
	UNIT_ADD_IO(namespaceName,nameSingular, abbreviation)\
	UNIT_ADD_LITERALS(namespaceName,nameSingular, abbreviation)

/**
 * @def			UNIT_ADD_DECIBEL(namespaceName, nameSingular, abbreviation)
 * @brief		Macro to create decibel container and literals for an existing unit type.
 * @details		This macro generates the decibel unit container, cout overload, and literal definitions.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the base unit name, e.g. 'watt'
 * @param		abbreviation - abbreviated decibel unit name, e.g. 'dBW'
 */
#define UNIT_ADD_DECIBEL(namespaceName, nameSingular, abbreviation)\
	namespace namespaceName\
	{\
		/** @name Unit Containers */ /** @{ */ typedef unit_t<nameSingular, UNIT_LIB_DEFAULT_TYPE, units::decibel_scale> abbreviation ## _t; /** @} */\
	}\
	UNIT_ADD_IO(namespaceName, abbreviation, abbreviation)\
	UNIT_ADD_LITERALS(namespaceName, abbreviation, abbreviation)

/**
 * @def			UNIT_ADD_CATEGORY_TRAIT(unitCategory, baseUnit)
 * @brief		Macro to create the `is_category_unit` type trait.
 * @details		This trait allows users to test whether a given type matches
 *				an intended category. This macro comprises all the boiler-plate
 *				code necessary to do so.
 * @param		unitCategory The name of the category of unit, e.g. length or mass.
 */

#define UNIT_ADD_CATEGORY_TRAIT_DETAIL(unitCategory)\
	namespace traits\
	{\
		/** @cond */\
		namespace detail\
		{\
			template<typename T> struct is_ ## unitCategory ## _unit_impl : std::false_type {};\
			template<typename C, typename U, typename P, typename T>\
			struct is_ ## unitCategory ## _unit_impl<units::unit<C, U, P, T>> : std::is_same<units::traits::base_unit_of<typename units::traits::unit_traits<units::unit<C, U, P, T>>::base_unit_type>, units::category::unitCategory ## _unit>::type {};\
			template<typename U, typename S, template<typename> class N>\
			struct is_ ## unitCategory ## _unit_impl<units::unit_t<U, S, N>> : std::is_same<units::traits::base_unit_of<typename units::traits::unit_t_traits<units::unit_t<U, S, N>>::unit_type>, units::category::unitCategory ## _unit>::type {};\
		}\
		/** @endcond */\
	}

#if defined(UNIT_HAS_VARIADIC_TEMPLATE_SUPPORT)
#define UNIT_ADD_IS_UNIT_CATEGORY_TRAIT(unitCategory)\
	namespace traits\
	{\
		template<typename... T> struct is_ ## unitCategory ## _unit : std::integral_constant<bool, units::all_true<units::traits::detail::is_ ## unitCategory ## _unit_impl<std::decay_t<T>>::value...>::value> {};\
	}
#else
#define UNIT_ADD_IS_UNIT_CATEGORY_TRAIT(unitCategory)\
	namespace traits\
	{\
			template<typename T1, typename T2 = T1, typename T3 = T1>\
			struct is_ ## unitCategory ## _unit : std::integral_constant<bool, units::traits::detail::is_ ## unitCategory ## _unit_impl<typename std::decay<T1>::type>::value &&\
				units::traits::detail::is_ ## unitCategory ## _unit_impl<typename std::decay<T2>::type>::value &&\
				units::traits::detail::is_ ## unitCategory ## _unit_impl<typename std::decay<T3>::type>::value>{};\
	}
#endif

#define UNIT_ADD_CATEGORY_TRAIT(unitCategory)\
	UNIT_ADD_CATEGORY_TRAIT_DETAIL(unitCategory)\
    /** @ingroup	TypeTraits*/\
	/** @brief		Trait which tests whether a type represents a unit of unitCategory*/\
	/** @details	Inherits from `std::true_type` or `std::false_type`. Use `is_ ## unitCategory ## _unit<T>::value` to test the unit represents a unitCategory quantity.*/\
	/** @tparam		T	one or more types to test*/\
	UNIT_ADD_IS_UNIT_CATEGORY_TRAIT(unitCategory)

/**
 * @def			UNIT_ADD_WITH_METRIC_PREFIXES(nameSingular, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit, including its metric
 *				prefixes from femto to peta.
 * @details		See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `meters` and 'meter_t',
 *				it also creates corresponding units with metric suffixes such as `millimeters`, and `millimeter_t`), as well as the
 *				literal suffixes (e.g. `10.0_mm`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `unit<std::ratio<1>, units::category::length_unit>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...)\
	UNIT_ADD(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__)\
	UNIT_ADD(namespaceName, femto ## nameSingular, femto ## namePlural, f ## abbreviation, femto<namePlural>)\
	UNIT_ADD(namespaceName, pico ## nameSingular, pico ## namePlural, p ## abbreviation, pico<namePlural>)\
	UNIT_ADD(namespaceName, nano ## nameSingular, nano ## namePlural, n ## abbreviation, nano<namePlural>)\
	UNIT_ADD(namespaceName, micro ## nameSingular, micro ## namePlural, u ## abbreviation, micro<namePlural>)\
	UNIT_ADD(namespaceName, milli ## nameSingular, milli ## namePlural, m ## abbreviation, milli<namePlural>)\
	UNIT_ADD(namespaceName, centi ## nameSingular, centi ## namePlural, c ## abbreviation, centi<namePlural>)\
	UNIT_ADD(namespaceName, deci ## nameSingular, deci ## namePlural, d ## abbreviation, deci<namePlural>)\
	UNIT_ADD(namespaceName, deca ## nameSingular, deca ## namePlural, da ## abbreviation, deca<namePlural>)\
	UNIT_ADD(namespaceName, hecto ## nameSingular, hecto ## namePlural, h ## abbreviation, hecto<namePlural>)\
	UNIT_ADD(namespaceName, kilo ## nameSingular, kilo ## namePlural, k ## abbreviation, kilo<namePlural>)\
	UNIT_ADD(namespaceName, mega ## nameSingular, mega ## namePlural, M ## abbreviation, mega<namePlural>)\
	UNIT_ADD(namespaceName, giga ## nameSingular, giga ## namePlural, G ## abbreviation, giga<namePlural>)\
	UNIT_ADD(namespaceName, tera ## nameSingular, tera ## namePlural, T ## abbreviation, tera<namePlural>)\
	UNIT_ADD(namespaceName, peta ## nameSingular, peta ## namePlural, P ## abbreviation, peta<namePlural>)\

 /**
  * @def		UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(nameSingular, namePlural, abbreviation, definition)
  * @brief		Macro for generating the boiler-plate code needed for a new unit, including its metric
  *				prefixes from femto to peta, and binary prefixes from kibi to exbi.
  * @details	See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `bytes` and 'byte_t',
  *				it also creates corresponding units with metric suffixes such as `millimeters`, and `millimeter_t`), as well as the
  *				literal suffixes (e.g. `10.0_B`).
  * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
  *				are placed in the `units::literals` namespace.
  * @param		nameSingular singular version of the unit name, e.g. 'byte'
  * @param		namePlural - plural version of the unit name, e.g. 'bytes'
  * @param		abbreviation - abbreviated unit name, e.g. 'B'
  * @param		definition - the variadic parameter is used for the definition of the unit
  *				(e.g. `unit<std::ratio<1>, units::category::data_unit>`)
  * @note		a variadic template is used for the definition to allow templates with
  *				commas to be easily expanded. All the variadic 'arguments' should together
  *				comprise the unit definition.
  */
#define UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...)\
	UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__)\
	UNIT_ADD(namespaceName, kibi ## nameSingular, kibi ## namePlural, Ki ## abbreviation, kibi<namePlural>)\
	UNIT_ADD(namespaceName, mebi ## nameSingular, mebi ## namePlural, Mi ## abbreviation, mebi<namePlural>)\
	UNIT_ADD(namespaceName, gibi ## nameSingular, gibi ## namePlural, Gi ## abbreviation, gibi<namePlural>)\
	UNIT_ADD(namespaceName, tebi ## nameSingular, tebi ## namePlural, Ti ## abbreviation, tebi<namePlural>)\
	UNIT_ADD(namespaceName, pebi ## nameSingular, pebi ## namePlural, Pi ## abbreviation, pebi<namePlural>)\
	UNIT_ADD(namespaceName, exbi ## nameSingular, exbi ## namePlural, Ei ## abbreviation, exbi<namePlural>)

//--------------------
//	UNITS NAMESPACE
//--------------------

/**
 * @namespace units
 * @brief Unit Conversion Library namespace
 */
namespace units
{
	//----------------------------------
	//	DOXYGEN
	//----------------------------------

	/**
	 * @defgroup	UnitContainers Unit Containers
	 * @brief		Defines a series of classes which contain dimensioned values. Unit containers
	 *				store a value, and support various arithmetic operations.
	 */

	/**
	 * @defgroup	UnitTypes Unit Types
	 * @brief		Defines a series of classes which represent units. These types are tags used by
	 *				the conversion function, to create compound units, or to create `unit_t` types.
	 *				By themselves, they are not containers and have no stored value.
	 */

	/**
	 * @defgroup	UnitManipulators Unit Manipulators
	 * @brief		Defines a series of classes used to manipulate unit types, such as `inverse<>`, `squared<>`, and metric prefixes.
	 *				Unit manipulators can be chained together, e.g. `inverse<squared<pico<time::seconds>>>` to
	 *				represent picoseconds^-2.
	 */

	 /**
	  * @defgroup	CompileTimeUnitManipulators Compile-time Unit Manipulators
	  * @brief		Defines a series of classes used to manipulate `unit_value_t` types at compile-time, such as `unit_value_add<>`, `unit_value_sqrt<>`, etc.
	  *				Compile-time manipulators can be chained together, e.g. `unit_value_sqrt<unit_value_add<unit_value_power<a, 2>, unit_value_power<b, 2>>>` to
	  *				represent `c = sqrt(a^2 + b^2).
	  */

	 /**
	 * @defgroup	UnitMath Unit Math
	 * @brief		Defines a collection of unit-enabled, strongly-typed versions of `<cmath>` functions.
	 * @details		Includes most c++11 extensions.
	 */

	/**
	 * @defgroup	Conversion Explicit Conversion
	 * @brief		Functions used to convert values of one logical type to another.
	 */

	/**
	 * @defgroup	TypeTraits Type Traits
	 * @brief		Defines a series of classes to obtain unit type information at compile-time.
	 */

	//------------------------------
	//	FORWARD DECLARATIONS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace constants
	{
		namespace detail
		{
			static constexpr const UNIT_LIB_DEFAULT_TYPE PI_VAL = 3.14159265358979323846264338327950288419716939937510;
		}
	}
	/** @endcond */	// END DOXYGEN IGNORE

	//------------------------------
	//	RATIO TRAITS
	//------------------------------

	/**
	 * @ingroup TypeTraits
	 * @{
	 */

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/// has_num implementation.
		template<class T>
		struct has_num_impl
		{
			template<class U>
			static constexpr auto test(U*)->std::is_integral<decltype(U::num)> {return std::is_integral<decltype(U::num)>{}; }
			template<typename>
			static constexpr std::false_type test(...) { return std::false_type{}; }

			using type = decltype(test<T>(0));
		};
	}

	/**
	 * @brief		Trait which checks for the existence of a static numerator.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_num<T>::value` to test
	 *				whether `class T` has a numerator static member.
	 */
	template<class T>
	struct has_num : units::detail::has_num_impl<T>::type {};

	namespace detail
	{
		/// has_den implementation.
		template<class T>
		struct has_den_impl
		{
			template<class U>
			static constexpr auto test(U*)->std::is_integral<decltype(U::den)> { return std::is_integral<decltype(U::den)>{}; }
			template<typename>
			static constexpr std::false_type test(...) { return std::false_type{}; }

			using type = decltype(test<T>(0));
		};
	}

	/**
	 * @brief		Trait which checks for the existence of a static denominator.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_den<T>::value` to test
	 *				whether `class T` has a denominator static member.
	 */
	template<class T>
	struct has_den : units::detail::has_den_impl<T>::type {};

	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @brief		Trait that tests whether a type represents a std::ratio.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_ratio<T>::value` to test
		 *				whether `class T` implements a std::ratio.
		 */
		template<class T>
		struct is_ratio : std::integral_constant<bool,
			has_num<T>::value &&
			has_den<T>::value>
		{};
	}

	//------------------------------
	//	UNIT TRAITS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	/**
	 * @brief		void type.
	 * @details		Helper class for creating type traits.
	 */
	template<class ...>
	struct void_t { typedef void type; };

	/**
	 * @brief		parameter pack for boolean arguments.
	 */
	template<bool...> struct bool_pack {};

	/**
	 * @brief		Trait which tests that a set of other traits are all true.
	 */
	template<bool... Args>
	struct all_true : std::is_same<units::bool_pack<true, Args...>, units::bool_pack<Args..., true>> {};
	/** @endcond */	// DOXYGEN IGNORE

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
		 *				unit_traits instantiation.
		 */
		template<class T>
		struct unit_traits
		{
			typedef typename T::base_unit_type base_unit_type;											///< Unit type that the unit was derived from. May be a `base_unit` or another `unit`. Use the `base_unit_of` trait to find the SI base unit type. This will be `void` if type `T` is not a unit.
			typedef typename T::conversion_ratio conversion_ratio;										///< `std::ratio` representing the conversion factor to the `base_unit_type`. This will be `void` if type `T` is not a unit.
			typedef typename T::pi_exponent_ratio pi_exponent_ratio;									///< `std::ratio` representing the exponent of pi to be used in the conversion. This will be `void` if type `T` is not a unit.
			typedef typename T::translation_ratio translation_ratio;									///< `std::ratio` representing a datum translation to the base unit (i.e. degrees C to degrees F conversion). This will be `void` if type `T` is not a unit.
		};
#endif
		/** @cond */	// DOXYGEN IGNORE
		/**
		 * @brief		unit traits implementation for classes which are not units.
		 */
		template<class T, typename = void>
		struct unit_traits
		{
			typedef void base_unit_type;
			typedef void conversion_ratio;
			typedef void pi_exponent_ratio;
			typedef void translation_ratio;
		};

		template<class T>
		struct unit_traits
			<T, typename void_t<
			typename T::base_unit_type,
			typename T::conversion_ratio,
			typename T::pi_exponent_ratio,
			typename T::translation_ratio>::type>
		{
			typedef typename T::base_unit_type base_unit_type;											///< Unit type that the unit was derived from. May be a `base_unit` or another `unit`. Use the `base_unit_of` trait to find the SI base unit type. This will be `void` if type `T` is not a unit.
			typedef typename T::conversion_ratio conversion_ratio;										///< `std::ratio` representing the conversion factor to the `base_unit_type`. This will be `void` if type `T` is not a unit.
			typedef typename T::pi_exponent_ratio pi_exponent_ratio;									///< `std::ratio` representing the exponent of pi to be used in the conversion. This will be `void` if type `T` is not a unit.
			typedef typename T::translation_ratio translation_ratio;									///< `std::ratio` representing a datum translation to the base unit (i.e. degrees C to degrees F conversion). This will be `void` if type `T` is not a unit.
		};
		/** @endcond */	// END DOXYGEN IGNORE
	}

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify base units.
		 * @details		A non-templated base class for `base_unit` which enables RTTI testing.
		 */
		struct _base_unit_t {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests if a class is a `base_unit` type.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_base_unit<T>::value` to test
		 *				whether `class T` implements a `base_unit`.
		 */
		template<class T>
		struct is_base_unit : std::is_base_of<units::detail::_base_unit_t, T> {};
	}

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify units.
		 * @details		A non-templated base class for `unit` which enables RTTI testing.
		 */
		struct _unit {};

		template<std::intmax_t Num, std::intmax_t Den = 1>
		using meter_ratio = std::ratio<Num, Den>;
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits which tests if a class is a `unit`
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit<T>::value` to test
		 *				whether `class T` implements a `unit`.
		 */
		template<class T>
		struct is_unit : std::is_base_of<units::detail::_unit, T>::type {};
	}

	/** @} */ // end of TypeTraits

	//------------------------------
	//	BASE UNIT CLASS
	//------------------------------

	/**
	 * @ingroup		UnitTypes
	 * @brief		Class representing SI base unit types.
	 * @details		Base units are represented by a combination of `std::ratio` template parameters, each
	 *				describing the exponent of the type of unit they represent. Example: meters per second
	 *				would be described by a +1 exponent for meters, and a -1 exponent for seconds, thus:
	 *				`base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-1>>`
	 * @tparam		Meter		`std::ratio` representing the exponent value for meters.
	 * @tparam		Kilogram	`std::ratio` representing the exponent value for kilograms.
	 * @tparam		Second		`std::ratio` representing the exponent value for seconds.
	 * @tparam		Radian		`std::ratio` representing the exponent value for radians. Although radians are not SI base units, they are included because radians are described by the SI as m * m^-1, which would make them indistinguishable from scalars.
	 * @tparam		Ampere		`std::ratio` representing the exponent value for amperes.
	 * @tparam		Kelvin		`std::ratio` representing the exponent value for Kelvin.
	 * @tparam		Mole		`std::ratio` representing the exponent value for moles.
	 * @tparam		Candela		`std::ratio` representing the exponent value for candelas.
	 * @tparam		Byte		`std::ratio` representing the exponent value for bytes.
	 * @sa			category	 for type aliases for SI base_unit types.
	 */
	template<class Meter = detail::meter_ratio<0>,
	class Kilogram = std::ratio<0>,
	class Second = std::ratio<0>,
	class Radian = std::ratio<0>,
	class Ampere = std::ratio<0>,
	class Kelvin = std::ratio<0>,
	class Mole = std::ratio<0>,
	class Candela = std::ratio<0>,
	class Byte = std::ratio<0>>
	struct base_unit : units::detail::_base_unit_t
	{
		static_assert(traits::is_ratio<Meter>::value, "Template parameter `Meter` must be a `std::ratio` representing the exponent of meters the unit has");
		static_assert(traits::is_ratio<Kilogram>::value, "Template parameter `Kilogram` must be a `std::ratio` representing the exponent of kilograms the unit has");
		static_assert(traits::is_ratio<Second>::value, "Template parameter `Second` must be a `std::ratio` representing the exponent of seconds the unit has");
		static_assert(traits::is_ratio<Ampere>::value, "Template parameter `Ampere` must be a `std::ratio` representing the exponent of amperes the unit has");
		static_assert(traits::is_ratio<Kelvin>::value, "Template parameter `Kelvin` must be a `std::ratio` representing the exponent of kelvin the unit has");
		static_assert(traits::is_ratio<Candela>::value, "Template parameter `Candela` must be a `std::ratio` representing the exponent of candelas the unit has");
		static_assert(traits::is_ratio<Mole>::value, "Template parameter `Mole` must be a `std::ratio` representing the exponent of moles the unit has");
		static_assert(traits::is_ratio<Radian>::value, "Template parameter `Radian` must be a `std::ratio` representing the exponent of radians the unit has");
		static_assert(traits::is_ratio<Byte>::value, "Template parameter `Byte` must be a `std::ratio` representing the exponent of bytes the unit has");

		typedef Meter meter_ratio;
		typedef Kilogram kilogram_ratio;
		typedef Second second_ratio;
		typedef Radian radian_ratio;
		typedef Ampere ampere_ratio;
		typedef Kelvin kelvin_ratio;
		typedef Mole mole_ratio;
		typedef Candela candela_ratio;
		typedef Byte byte_ratio;
	};

	//------------------------------
	//	UNIT CATEGORIES
	//------------------------------

	/**
	 * @brief		namespace representing the implemented base and derived unit types. These will not generally be needed by library users.
	 * @sa			base_unit for the definition of the category parameters.
	 */
	namespace category
	{
		// SCALAR (DIMENSIONLESS) TYPES
		typedef base_unit<> scalar_unit;			///< Represents a quantity with no dimension.
		typedef base_unit<> dimensionless_unit;	///< Represents a quantity with no dimension.

		// SI BASE UNIT TYPES
		//					METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			BYTE		---		CATEGORY
		typedef base_unit<detail::meter_ratio<1>>																																		length_unit;			 		///< Represents an SI base unit of length
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<1>>																														mass_unit;				 		///< Represents an SI base unit of mass
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<1>>																										time_unit;				 		///< Represents an SI base unit of time
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>																						angle_unit;				 		///< Represents an SI base unit of angle
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>																		current_unit;			 		///< Represents an SI base unit of current
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>														temperature_unit;		 		///< Represents an SI base unit of temperature
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>										substance_unit;			 		///< Represents an SI base unit of amount of substance
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>						luminous_intensity_unit; 		///< Represents an SI base unit of luminous intensity

		// SI DERIVED UNIT TYPES
		//					METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			BYTE		---		CATEGORY
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>>						solid_angle_unit;				///< Represents an SI derived unit of solid angle
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<-1>>																										frequency_unit;					///< Represents an SI derived unit of frequency
		typedef base_unit<detail::meter_ratio<1>,	std::ratio<0>,	std::ratio<-1>>																										velocity_unit;					///< Represents an SI derived unit of velocity
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<-1>,	std::ratio<1>>																						angular_velocity_unit;			///< Represents an SI derived unit of angular velocity
		typedef base_unit<detail::meter_ratio<1>,	std::ratio<0>,	std::ratio<-2>>																										acceleration_unit;				///< Represents an SI derived unit of acceleration
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<-2>,	std::ratio<1>>																						angular_acceleration_unit;			///< Represents an SI derived unit of angular acceleration
		typedef base_unit<detail::meter_ratio<1>,	std::ratio<1>,	std::ratio<-2>>																										force_unit;						///< Represents an SI derived unit of force
		typedef base_unit<detail::meter_ratio<-1>,	std::ratio<1>,	std::ratio<-2>>																										pressure_unit;					///< Represents an SI derived unit of pressure
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<1>,	std::ratio<0>,	std::ratio<1>>																		charge_unit;					///< Represents an SI derived unit of charge
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-2>>																										energy_unit;					///< Represents an SI derived unit of energy
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-3>>																										power_unit;						///< Represents an SI derived unit of power
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-3>,	std::ratio<0>,	std::ratio<-1>>																		voltage_unit;					///< Represents an SI derived unit of voltage
		typedef base_unit<detail::meter_ratio<-2>,	std::ratio<-1>,	std::ratio<4>,	std::ratio<0>,	std::ratio<2>>																		capacitance_unit;				///< Represents an SI derived unit of capacitance
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-3>,	std::ratio<0>,	std::ratio<-2>>																		impedance_unit;					///< Represents an SI derived unit of impedance
		typedef base_unit<detail::meter_ratio<-2>,	std::ratio<-1>,	std::ratio<3>,	std::ratio<0>,	std::ratio<2>>																		conductance_unit;				///< Represents an SI derived unit of conductance
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-1>>																		magnetic_flux_unit;				///< Represents an SI derived unit of magnetic flux
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-1>>																		magnetic_field_strength_unit;	///< Represents an SI derived unit of magnetic field strength
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-2>>																		inductance_unit;				///< Represents an SI derived unit of inductance
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>						luminous_flux_unit;				///< Represents an SI derived unit of luminous flux
		typedef base_unit<detail::meter_ratio<-2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>						illuminance_unit;				///< Represents an SI derived unit of illuminance
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<-1>>																										radioactivity_unit;				///< Represents an SI derived unit of radioactivity

		// OTHER UNIT TYPES
		//					METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			BYTE		---		CATEGORY
		typedef base_unit<detail::meter_ratio<2>,	std::ratio<1>,	std::ratio<-2>>																										torque_unit;					///< Represents an SI derived unit of torque
		typedef base_unit<detail::meter_ratio<2>>																																		area_unit;						///< Represents an SI derived unit of area
		typedef base_unit<detail::meter_ratio<3>>																																		volume_unit;					///< Represents an SI derived unit of volume
		typedef base_unit<detail::meter_ratio<-3>,	std::ratio<1>>																														density_unit;					///< Represents an SI derived unit of density
		typedef base_unit<>																																						concentration_unit;				///< Represents a unit of concentration
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>		data_unit;						///< Represents a unit of data size
		typedef base_unit<detail::meter_ratio<0>,	std::ratio<0>,	std::ratio<-1>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>		data_transfer_rate_unit;		///< Represents a unit of data transfer rate
	}

	//------------------------------
	//	UNIT CLASSES
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	/**
	 * @brief		unit type template specialization for units derived from base units.
	 */
	template <class, class, class, class> struct unit;
	template<class Conversion, class... Exponents, class PiExponent, class Translation>
	struct unit<Conversion, base_unit<Exponents...>, PiExponent, Translation> : units::detail::_unit
	{
		static_assert(traits::is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(traits::is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");
		static_assert(traits::is_ratio<Translation>::value, "Template parameter `Translation` must be a `std::ratio` representing an additive translation required by the unit conversion.");

		typedef typename units::base_unit<Exponents...> base_unit_type;
		typedef Conversion conversion_ratio;
		typedef Translation translation_ratio;
		typedef PiExponent pi_exponent_ratio;
	};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Type representing an arbitrary unit.
	 * @ingroup		UnitTypes
	 * @details		`unit` types are used as tags for the `conversion` function. They are *not* containers
	 *				(see `unit_t` for a  container class). Each unit is defined by:
	 *
	 *				- A `std::ratio` defining the conversion factor to the base unit type. (e.g. `std::ratio<1,12>` for inches to feet)
	 *				- A base unit that the unit is derived from (or a unit category. Must be of type `unit` or `base_unit`)
	 *				- An exponent representing factors of PI required by the conversion. (e.g. `std::ratio<-1>` for a radians to degrees conversion)
	 *				- a ratio representing a datum translation required for the conversion (e.g. `std::ratio<32>` for a farenheit to celsius conversion)
	 *
	 *				Typically, a specific unit, like `meters`, would be implemented as a type alias
	 *				of `unit`, i.e. `using meters = unit<std::ratio<1>, units::category::length_unit`, or
	 *				`using inches = unit<std::ratio<1,12>, feet>`.
	 * @tparam		Conversion	std::ratio representing scalar multiplication factor.
	 * @tparam		BaseUnit	Unit type which this unit is derived from. May be a `base_unit`, or another `unit`.
	 * @tparam		PiExponent	std::ratio representing the exponent of pi required by the conversion.
	 * @tparam		Translation	std::ratio representing any datum translation required by the conversion.
	 */
	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>, class Translation = std::ratio<0>>
	struct unit : units::detail::_unit
	{
		static_assert(traits::is_unit<BaseUnit>::value, "Template parameter `BaseUnit` must be a `unit` type.");
		static_assert(traits::is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(traits::is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		typedef typename units::traits::unit_traits<BaseUnit>::base_unit_type base_unit_type;
		typedef typename std::ratio_multiply<typename BaseUnit::conversion_ratio, Conversion> conversion_ratio;
		typedef typename std::ratio_add<typename BaseUnit::pi_exponent_ratio, PiExponent> pi_exponent_ratio;
		typedef typename std::ratio_add<std::ratio_multiply<typename BaseUnit::conversion_ratio, Translation>, typename BaseUnit::translation_ratio> translation_ratio;
	};

	//------------------------------
	//	BASE UNIT MANIPULATORS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		base_unit_of trait implementation
		 * @details		recursively seeks base_unit type that a unit is derived from. Since units can be
		 *				derived from other units, the `base_unit_type` typedef may not represent this value.
		 */
		template<class> struct base_unit_of_impl;
		template<class Conversion, class BaseUnit, class PiExponent, class Translation>
		struct base_unit_of_impl<unit<Conversion, BaseUnit, PiExponent, Translation>> : base_unit_of_impl<BaseUnit> {};
		template<class... Exponents>
		struct base_unit_of_impl<base_unit<Exponents...>>
		{
			typedef base_unit<Exponents...> type;
		};
		template<>
		struct base_unit_of_impl<void>
		{
			typedef void type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @brief		Trait which returns the `base_unit` type that a unit is originally derived from.
		 * @details		Since units can be derived from other `unit` types in addition to `base_unit` types,
		 *				the `base_unit_type` typedef will not always be a `base_unit` (or unit category).
		 *				Since compatible
		 */
		template<class U>
		using base_unit_of = typename units::detail::base_unit_of_impl<U>::type;
	}

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of base_unit_multiply
		 * @details		'multiples' (adds exponent ratios of) two base unit types. Base units can be found
		 *				using `base_unit_of`.
		 */
		template<class, class> struct base_unit_multiply_impl;
		template<class... Exponents1, class... Exponents2>
		struct base_unit_multiply_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
			using type = base_unit<std::ratio_add<Exponents1, Exponents2>...>;
		};

		/**
		 * @brief		represents type of two base units multiplied together
		 */
		template<class U1, class U2>
		using base_unit_multiply = typename base_unit_multiply_impl<U1, U2>::type;

		/**
		 * @brief		implementation of base_unit_divide
		 * @details		'dived' (subtracts exponent ratios of) two base unit types. Base units can be found
		 *				using `base_unit_of`.
		 */
		template<class, class> struct base_unit_divide_impl;
		template<class... Exponents1, class... Exponents2>
		struct base_unit_divide_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
			using type = base_unit<std::ratio_subtract<Exponents1, Exponents2>...>;
		};

		/**
		 * @brief		represents the resulting type of `base_unit` U1 divided by U2.
		 */
		template<class U1, class U2>
		using base_unit_divide = typename base_unit_divide_impl<U1, U2>::type;

		/**
		 * @brief		implementation of inverse_base
		 * @details		multiplies all `base_unit` exponent ratios by -1. The resulting type represents
		 *				the inverse base unit of the given `base_unit` type.
		 */
		template<class> struct inverse_base_impl;

		template<class... Exponents>
		struct inverse_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<-1>>...>;
		};

		/**
		 * @brief		represent the inverse type of `class U`
		 * @details		E.g. if `U` is `length_unit`, then `inverse<U>` will represent `length_unit^-1`.
		 */
		template<class U> using inverse_base = typename inverse_base_impl<U>::type;

		/**
		 * @brief		implementation of `squared_base`
		 * @details		multiplies all the exponent ratios of the given class by 2. The resulting type is
		 *				equivalent to the given type squared.
		 */
		template<class U> struct squared_base_impl;
		template<class... Exponents>
		struct squared_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<2>>...>;
		};

		/**
		 * @brief		represents the type of a `base_unit` squared.
		 * @details		E.g. `squared<length_unit>` will represent `length_unit^2`.
		 */
		template<class U> using squared_base = typename squared_base_impl<U>::type;

		/**
		 * @brief		implementation of `cubed_base`
		 * @details		multiplies all the exponent ratios of the given class by 3. The resulting type is
		 *				equivalent to the given type cubed.
		 */
		template<class U> struct cubed_base_impl;
		template<class... Exponents>
		struct cubed_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<3>>...>;
		};

		/**
		 * @brief		represents the type of a `base_unit` cubed.
		 * @details		E.g. `cubed<length_unit>` will represent `length_unit^3`.
		 */
		template<class U> using cubed_base = typename cubed_base_impl<U>::type;

		/**
		 * @brief		implementation of `sqrt_base`
		 * @details		divides all the exponent ratios of the given class by 2. The resulting type is
		 *				equivalent to the square root of the given type.
		 */
		template<class U> struct sqrt_base_impl;
		template<class... Exponents>
		struct sqrt_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_divide<Exponents, std::ratio<2>>...>;
		};

		/**
		 * @brief		represents the square-root type of a `base_unit`.
		 * @details		E.g. `sqrt<length_unit>` will represent `length_unit^(1/2)`.
		 */
		template<class U> using sqrt_base = typename sqrt_base_impl<U>::type;

		/**
		 * @brief		implementation of `cbrt_base`
		 * @details		divides all the exponent ratios of the given class by 3. The resulting type is
		 *				equivalent to the given type's cube-root.
		 */
		template<class U> struct cbrt_base_impl;
		template<class... Exponents>
		struct cbrt_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_divide<Exponents, std::ratio<3>>...>;
		};

		/**
		 * @brief		represents the cube-root type of a `base_unit` .
		 * @details		E.g. `cbrt<length_unit>` will represent `length_unit^(1/3)`.
		 */
		template<class U> using cbrt_base = typename cbrt_base_impl<U>::type;
	}
	/** @endcond */	// END DOXYGEN IGNORE

	//------------------------------
	//	UNIT MANIPULATORS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `unit_multiply`.
		 * @details		multiplies two units. The base unit becomes the base units of each with their exponents
		 *				added together. The conversion factors of each are multiplied by each other. Pi exponent ratios
		 *				are added, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_multiply_impl
		{
			using type = unit < std::ratio_multiply<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
				base_unit_multiply <traits::base_unit_of<typename Unit1::base_unit_type>, traits::base_unit_of<typename Unit2::base_unit_type>>,
				std::ratio_add<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>,
				std::ratio < 0 >> ;
		};

		/**
		 * @brief		represents the type of two units multiplied together.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `unit_divide`.
		 * @details		divides two units. The base unit becomes the base units of each with their exponents
		 *				subtracted from each other. The conversion factors of each are divided by each other. Pi exponent ratios
		 *				are subtracted, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_divide_impl
		{
			using type = unit < std::ratio_divide<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
				base_unit_divide<traits::base_unit_of<typename Unit1::base_unit_type>, traits::base_unit_of<typename Unit2::base_unit_type>>,
				std::ratio_subtract<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>,
				std::ratio < 0 >> ;
		};

		/**
		 * @brief		represents the type of two units divided by each other.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_divide = typename unit_divide_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `inverse`
		 * @details		inverts a unit (equivalent to 1/unit). The `base_unit` and pi exponents are all multiplied by
		 *				-1. The conversion ratio numerator and denominator are swapped. Datum translation
		 *				ratios are removed.
		 */
		template<class Unit>
		struct inverse_impl
		{
			using type = unit < std::ratio<Unit::conversion_ratio::den, Unit::conversion_ratio::num>,
				inverse_base<traits::base_unit_of<typename units::traits::unit_traits<Unit>::base_unit_type>>,
				std::ratio_multiply<typename units::traits::unit_traits<Unit>::pi_exponent_ratio, std::ratio<-1>>,
				std::ratio < 0 >> ;	// inverses are rates or change, the translation factor goes away.
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the inverse unit type of `class U`.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to invert.
	 * @details		E.g. `inverse<meters>` will represent meters^-1 (i.e. 1/meters).
	 */
	template<class U> using inverse = typename units::detail::inverse_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `squared`
		 * @details		Squares the conversion ratio, `base_unit` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit>
		struct squared_impl
		{
			static_assert(traits::is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit < std::ratio_multiply<Conversion, Conversion>,
				squared_base<traits::base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<2>>,
				typename Unit::translation_ratio
			> ;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the unit type of `class U` squared
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to square.
	 * @details		E.g. `square<meters>` will represent meters^2.
	 */
	template<class U>
	using squared = typename units::detail::squared_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		implementation of `cubed`
			 * @details		Cubes the conversion ratio, `base_unit` exponents, pi exponents, and removes
			 *				datum translation ratios.
			 */
		template<class Unit>
		struct cubed_impl
		{
			static_assert(traits::is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit < std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>,
				cubed_base<traits::base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<3>>,
				typename Unit::translation_ratio> ;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the type of `class U` cubed.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to cube.
	 * @details		E.g. `cubed<meters>` will represent meters^3.
	 */
	template<class U>
	using cubed = typename units::detail::cubed_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		//----------------------------------
		//	RATIO_SQRT IMPLEMENTATION
		//----------------------------------

		using Zero = std::ratio<0>;
		using One = std::ratio<1>;
		template <typename R> using Square = std::ratio_multiply<R, R>;

		// Find the largest std::integer N such that Predicate<N>::value is true.
		template <template <std::intmax_t N> class Predicate, typename enabled = void>
		struct BinarySearch {
			template <std::intmax_t N>
			struct SafeDouble_ {
				static constexpr const std::intmax_t value = 2 * N;
				static_assert(value > 0, "Overflows when computing 2 * N");
			};

			template <intmax_t Lower, intmax_t Upper, typename Condition1 = void, typename Condition2 = void>
			struct DoubleSidedSearch_ : DoubleSidedSearch_<Lower, Upper,
				std::integral_constant<bool, (Upper - Lower == 1)>,
				std::integral_constant<bool, ((Upper - Lower>1 && Predicate<Lower + (Upper - Lower) / 2>::value))>> {};

			template <intmax_t Lower, intmax_t Upper>
			struct DoubleSidedSearch_<Lower, Upper, std::false_type, std::false_type> : DoubleSidedSearch_<Lower, Lower + (Upper - Lower) / 2> {};

			template <intmax_t Lower, intmax_t Upper, typename Condition2>
			struct DoubleSidedSearch_<Lower, Upper, std::true_type, Condition2> : std::integral_constant<intmax_t, Lower>{};

			template <intmax_t Lower, intmax_t Upper, typename Condition1>
			struct DoubleSidedSearch_<Lower, Upper, Condition1, std::true_type> : DoubleSidedSearch_<Lower + (Upper - Lower) / 2, Upper>{};

			template <std::intmax_t Lower, class enabled1 = void>
			struct SingleSidedSearch_ : SingleSidedSearch_<Lower, std::integral_constant<bool, Predicate<SafeDouble_<Lower>::value>::value>>{};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::false_type> : DoubleSidedSearch_<Lower, SafeDouble_<Lower>::value> {};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::true_type> : SingleSidedSearch_<SafeDouble_<Lower>::value>{};

			static constexpr const std::intmax_t value = SingleSidedSearch_<1>::value;
 		};

		template <template <std::intmax_t N> class Predicate>
		struct BinarySearch<Predicate, std::enable_if_t<!Predicate<1>::value>> : std::integral_constant<std::intmax_t, 0>{};

		// Find largest std::integer N such that N<=sqrt(R)
		template <typename R>
		struct Integer {
			template <std::intmax_t N> using Predicate_ = std::ratio_less_equal<std::ratio<N>, std::ratio_divide<R, std::ratio<N>>>;
			static constexpr const std::intmax_t value = BinarySearch<Predicate_>::value;
		};

		template <typename R>
		struct IsPerfectSquare {
			static constexpr const std::intmax_t DenSqrt_ = Integer<std::ratio<R::den>>::value;
			static constexpr const std::intmax_t NumSqrt_ = Integer<std::ratio<R::num>>::value;
			static constexpr const bool value =( DenSqrt_ * DenSqrt_ == R::den && NumSqrt_ * NumSqrt_ == R::num);
			using Sqrt = std::ratio<NumSqrt_, DenSqrt_>;
		};

		// Represents sqrt(P)-Q.
		template <typename Tp, typename Tq>
		struct Remainder {
			using P = Tp;
			using Q = Tq;
		};

		// Represents 1/R = I + Rem where R is a Remainder.
		template <typename R>
		struct Reciprocal {
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
		struct ContinuedFraction {
			template <typename T>
			using Abs_ = std::conditional_t<std::ratio_less<T, Zero>::value, std::ratio_subtract<Zero, T>, T>;

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
		struct ContinuedFraction<Tr, 1> {
			using R = Tr;
			using U = One;
			using V = std::ratio<Integer<R>::value>;
			using W = Zero;
			using Rem = Remainder<R, V>;
			using Error = std::ratio_divide<One, typename Reciprocal<Rem>::I>;
		};

		template <typename R, typename Eps, std::intmax_t N = 1, typename enabled = void>
		struct Sqrt_ : Sqrt_<R, Eps, N + 1> {};

		template <typename R, typename Eps, std::intmax_t N>
		struct Sqrt_<R, Eps, N, std::enable_if_t<std::ratio_less_equal<typename ContinuedFraction<R, N>::Error, Eps>::value>> {
			using type = typename ContinuedFraction<R, N>::V;
		};

		template <typename R, typename Eps, typename enabled = void>
		struct Sqrt {
			static_assert(std::ratio_greater_equal<R, Zero>::value, "R can't be negative");
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<std::ratio_greater_equal<R, Zero>::value && IsPerfectSquare<R>::value>> {
			using type = typename IsPerfectSquare<R>::Sqrt;
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<(std::ratio_greater_equal<R, Zero>::value && !IsPerfectSquare<R>::value)>> : Sqrt_<R, Eps>{};
	}
	/** @endcond */	// END DOXYGEN IGNORE

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
	template<typename Ratio, std::intmax_t Eps = 10000000000>
	using ratio_sqrt = typename  units::detail::Sqrt<Ratio, std::ratio<1, Eps>>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `sqrt`
		 * @details		square roots the conversion ratio, `base_unit` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit, std::intmax_t Eps>
		struct sqrt_impl
		{
			static_assert(traits::is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit <ratio_sqrt<Conversion, Eps>,
				sqrt_base<traits::base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_divide<typename Unit::pi_exponent_ratio, std::ratio<2>>,
				typename Unit::translation_ratio>;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

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
	 * @tparam		U	`unit` type to take the square root of.
	 * @tparam		Eps	Value of epsilon, which represents the inverse of the maximum allowable
	 *					error. This value should be chosen to be as high as possible before
	 *					integer overflow errors occur in the compiler.
	 * @note		USE WITH CAUTION. The is an approximate value. In general, squared<sqrt<meter>> != meter,
	 *				i.e. the operation is not reversible, and it will result in propogated approximations.
	 *				Use only when absolutely necessary.
	 */
	template<class U, std::intmax_t Eps = 10000000000>
	using square_root = typename units::detail::sqrt_impl<U, Eps>::type;

	//------------------------------
	//	COMPOUND UNITS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		implementation of compound_unit
			 * @details		multiplies a variadic list of units together, and is inherited from the resulting
			 *				type.
			 */
		template<class U, class... Us> struct compound_impl;
		template<class U> struct compound_impl<U> { using type = U; };
		template<class U1, class U2, class...Us>
		struct compound_impl<U1, U2, Us...>
			: compound_impl<unit_multiply<U1, U2>, Us...> {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Represents a unit type made up from other units.
	 * @details		Compound units are formed by multiplying the units of all the types provided in
	 *				the template argument. Types provided must inherit from `unit`. A compound unit can
	 *				be formed from any number of other units, and unit manipulators like `inverse` and
	 *				`squared` are supported. E.g. to specify acceleration, on could create
	 *				`using acceleration = compound_unit<length::meters, inverse<squared<seconds>>;`
	 * @tparam		U...	units which, when multiplied together, form the desired compound unit.
	 * @ingroup		UnitTypes
	 */
	template<class U, class... Us>
	using compound_unit = typename units::detail::compound_impl<U, Us...>::type;

	//------------------------------
	//	PREFIXES
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		prefix applicator.
		 * @details		creates a unit type from a prefix and a unit
		 */
		template<class Ratio, class Unit>
		struct prefix
		{
			static_assert(traits::is_ratio<Ratio>::value, "Template parameter `Ratio` must be a `std::ratio`.");
			static_assert(traits::is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			typedef typename units::unit<Ratio, Unit> type;
		};

		/// recursive exponential implementation
		template <int N, class U>
		struct power_of_ratio
		{
			typedef std::ratio_multiply<U, typename power_of_ratio<N - 1, U>::type> type;
		};

		/// End recursion
		template <class U>
		struct power_of_ratio<1, U>
		{
			typedef U type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Decimal Prefixes
	 * @{
	 */
	template<class U> using atto	= typename units::detail::prefix<std::atto,	U>::type;			///< Represents the type of `class U` with the metric 'atto' prefix appended.	@details E.g. atto<meters> represents meters*10^-18		@tparam U unit type to apply the prefix to.
	template<class U> using femto	= typename units::detail::prefix<std::femto,U>::type;			///< Represents the type of `class U` with the metric 'femto' prefix appended.  @details E.g. femto<meters> represents meters*10^-15	@tparam U unit type to apply the prefix to.
	template<class U> using pico	= typename units::detail::prefix<std::pico,	U>::type;			///< Represents the type of `class U` with the metric 'pico' prefix appended.	@details E.g. pico<meters> represents meters*10^-12		@tparam U unit type to apply the prefix to.
	template<class U> using nano	= typename units::detail::prefix<std::nano,	U>::type;			///< Represents the type of `class U` with the metric 'nano' prefix appended.	@details E.g. nano<meters> represents meters*10^-9		@tparam U unit type to apply the prefix to.
	template<class U> using micro	= typename units::detail::prefix<std::micro,U>::type;			///< Represents the type of `class U` with the metric 'micro' prefix appended.	@details E.g. micro<meters> represents meters*10^-6		@tparam U unit type to apply the prefix to.
	template<class U> using milli	= typename units::detail::prefix<std::milli,U>::type;			///< Represents the type of `class U` with the metric 'milli' prefix appended.	@details E.g. milli<meters> represents meters*10^-3		@tparam U unit type to apply the prefix to.
	template<class U> using centi	= typename units::detail::prefix<std::centi,U>::type;			///< Represents the type of `class U` with the metric 'centi' prefix appended.	@details E.g. centi<meters> represents meters*10^-2		@tparam U unit type to apply the prefix to.
	template<class U> using deci	= typename units::detail::prefix<std::deci,	U>::type;			///< Represents the type of `class U` with the metric 'deci' prefix appended.	@details E.g. deci<meters> represents meters*10^-1		@tparam U unit type to apply the prefix to.
	template<class U> using deca	= typename units::detail::prefix<std::deca,	U>::type;			///< Represents the type of `class U` with the metric 'deca' prefix appended.	@details E.g. deca<meters> represents meters*10^1		@tparam U unit type to apply the prefix to.
	template<class U> using hecto	= typename units::detail::prefix<std::hecto,U>::type;			///< Represents the type of `class U` with the metric 'hecto' prefix appended.	@details E.g. hecto<meters> represents meters*10^2		@tparam U unit type to apply the prefix to.
	template<class U> using kilo	= typename units::detail::prefix<std::kilo,	U>::type;			///< Represents the type of `class U` with the metric 'kilo' prefix appended.	@details E.g. kilo<meters> represents meters*10^3		@tparam U unit type to apply the prefix to.
	template<class U> using mega	= typename units::detail::prefix<std::mega,	U>::type;			///< Represents the type of `class U` with the metric 'mega' prefix appended.	@details E.g. mega<meters> represents meters*10^6		@tparam U unit type to apply the prefix to.
	template<class U> using giga	= typename units::detail::prefix<std::giga,	U>::type;			///< Represents the type of `class U` with the metric 'giga' prefix appended.	@details E.g. giga<meters> represents meters*10^9		@tparam U unit type to apply the prefix to.
	template<class U> using tera	= typename units::detail::prefix<std::tera,	U>::type;			///< Represents the type of `class U` with the metric 'tera' prefix appended.	@details E.g. tera<meters> represents meters*10^12		@tparam U unit type to apply the prefix to.
	template<class U> using peta	= typename units::detail::prefix<std::peta,	U>::type;			///< Represents the type of `class U` with the metric 'peta' prefix appended.	@details E.g. peta<meters> represents meters*10^15		@tparam U unit type to apply the prefix to.
	template<class U> using exa		= typename units::detail::prefix<std::exa,	U>::type;			///< Represents the type of `class U` with the metric 'exa' prefix appended.	@details E.g. exa<meters> represents meters*10^18		@tparam U unit type to apply the prefix to.
	/** @} @} */

	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Binary Prefixes
	 * @{
	 */
	template<class U> using kibi	= typename units::detail::prefix<std::ratio<1024>,					U>::type;	///< Represents the type of `class U` with the binary 'kibi' prefix appended.	@details E.g. kibi<bytes> represents bytes*2^10	@tparam U unit type to apply the prefix to.
	template<class U> using mebi	= typename units::detail::prefix<std::ratio<1048576>,				U>::type;	///< Represents the type of `class U` with the binary 'mibi' prefix appended.	@details E.g. mebi<bytes> represents bytes*2^20	@tparam U unit type to apply the prefix to.
	template<class U> using gibi	= typename units::detail::prefix<std::ratio<1073741824>,			U>::type;	///< Represents the type of `class U` with the binary 'gibi' prefix appended.	@details E.g. gibi<bytes> represents bytes*2^30	@tparam U unit type to apply the prefix to.
	template<class U> using tebi	= typename units::detail::prefix<std::ratio<1099511627776>,			U>::type;	///< Represents the type of `class U` with the binary 'tebi' prefix appended.	@details E.g. tebi<bytes> represents bytes*2^40	@tparam U unit type to apply the prefix to.
	template<class U> using pebi	= typename units::detail::prefix<std::ratio<1125899906842624>,		U>::type;	///< Represents the type of `class U` with the binary 'pebi' prefix appended.	@details E.g. pebi<bytes> represents bytes*2^50	@tparam U unit type to apply the prefix to.
	template<class U> using exbi	= typename units::detail::prefix<std::ratio<1152921504606846976>,	U>::type;	///< Represents the type of `class U` with the binary 'exbi' prefix appended.	@details E.g. exbi<bytes> represents bytes*2^60	@tparam U unit type to apply the prefix to.
	/** @} @} */

	//------------------------------
	//	CONVERSION TRAITS
	//------------------------------

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which checks whether two units can be converted to each other
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit<U1, U2>::value` to test
		 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic meaning,
		 *				(i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters can be
		 *				converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2, then
		 *				U2 will be convertible to U1.
		 * @tparam		U1 Unit to convert from.
		 * @tparam		U2 Unit to convert to.
		 * @sa			is_convertible_unit_t
		 */
		template<class U1, class U2>
		struct is_convertible_unit : std::is_same <traits::base_unit_of<typename units::traits::unit_traits<U1>::base_unit_type>,
			base_unit_of<typename units::traits::unit_traits<U2>::base_unit_type >> {};
	}

	//------------------------------
	//	CONVERSION FUNCTION
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		constexpr inline UNIT_LIB_DEFAULT_TYPE pow(UNIT_LIB_DEFAULT_TYPE x, unsigned long long y)
		{
			return y == 0 ? 1.0 : x * pow(x, y - 1);
		}

		constexpr inline UNIT_LIB_DEFAULT_TYPE abs(UNIT_LIB_DEFAULT_TYPE x)
		{
			return x < 0 ? -x : x;
		}

		/// convert dispatch for units which are both the same
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::true_type, std::false_type, std::false_type) noexcept
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::true_type, std::false_type, std::true_type) noexcept
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::true_type, std::true_type, std::false_type) noexcept
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::true_type, std::true_type, std::true_type) noexcept
		{
			return value;
		}

		/// convert dispatch for units of different types w/ no translation and no PI
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::false_type, std::false_type, std::false_type) noexcept
		{
			return ((value * Ratio::num) / Ratio::den);
		}

		/// convert dispatch for units of different types w/ no translation, but has PI in numerator
		// constepxr with PI in numerator
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr
		std::enable_if_t<(PiRatio::num / PiRatio::den >= 1 && PiRatio::num % PiRatio::den == 0), T>
		convert(const T& value, std::false_type, std::true_type, std::false_type) noexcept
		{
			return ((value * pow(constants::detail::PI_VAL, PiRatio::num / PiRatio::den) * Ratio::num) / Ratio::den);
		}

		/// convert dispatch for units of different types w/ no translation, but has PI in denominator
		// constexpr with PI in denominator
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr
		std::enable_if_t<(PiRatio::num / PiRatio::den <= -1 && PiRatio::num % PiRatio::den == 0), T>
 		convert(const T& value, std::false_type, std::true_type, std::false_type) noexcept
 		{
 			return (value * Ratio::num) / (Ratio::den * pow(constants::detail::PI_VAL, -PiRatio::num / PiRatio::den));
 		}

		/// convert dispatch for units of different types w/ no translation, but has PI in numerator
		// Not constexpr - uses std::pow
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline // sorry, this can't be constexpr!
		std::enable_if_t<(PiRatio::num / PiRatio::den < 1 && PiRatio::num / PiRatio::den > -1), T>
		convert(const T& value, std::false_type, std::true_type, std::false_type) noexcept
		{
			return ((value * std::pow(constants::detail::PI_VAL, PiRatio::num / PiRatio::den)  * Ratio::num) / Ratio::den);
		}

		/// convert dispatch for units of different types with a translation, but no PI
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, std::false_type, std::false_type, std::true_type) noexcept
		{
			return ((value * Ratio::num) / Ratio::den) + (static_cast<UNIT_LIB_DEFAULT_TYPE>(Translation::num) / Translation::den);
		}

		/// convert dispatch for units of different types with a translation AND PI
		template<class UnitFrom, class UnitTo, class Ratio, class PiRatio, class Translation, typename T>
		static inline constexpr T convert(const T& value, const std::false_type, const std::true_type, const std::true_type) noexcept
		{
			return ((value * std::pow(constants::detail::PI_VAL, PiRatio::num / PiRatio::den) * Ratio::num) / Ratio::den) + (static_cast<UNIT_LIB_DEFAULT_TYPE>(Translation::num) / Translation::den);
		}
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		Conversion
	 * @brief		converts a <i>value</i> from one type to another.
	 * @details		Converts a <i>value</i> of a built-in arithmetic type to another unit. This does not change
	 *				the type of <i>value</i>, only what it contains. E.g. @code double result = convert<length::meters, length::feet>(1.0);	// result == 3.28084 @endcode
	 * @sa			unit_t	for implicit conversion of unit containers.
	 * @tparam		UnitFrom unit tag to convert <i>value</i> from. Must be a `unit` type (i.e. is_unit<UnitFrom>::value == true),
	 *				and must be convertible to `UnitTo` (i.e. is_convertible_unit<UnitFrom, UnitTo>::value == true).
	 * @tparam		UnitTo unit tag to convert <i>value</i> to. Must be a `unit` type (i.e. is_unit<UnitTo>::value == true),
	 *				and must be convertible from `UnitFrom` (i.e. is_convertible_unit<UnitFrom, UnitTo>::value == true).
	 * @tparam		T type of <i>value</i>. It is inferred from <i>value</i>, and is expected to be a built-in arithmetic type.
	 * @param[in]	value Arithmetic value to convert from `UnitFrom` to `UnitTo`. The value should represent
	 *				a quantity in units of `UnitFrom`.
	 * @returns		value, converted from units of `UnitFrom` to `UnitTo`.
	 */
	template<class UnitFrom, class UnitTo, typename T = UNIT_LIB_DEFAULT_TYPE>
	static inline constexpr T convert(const T& value) noexcept
	{
		static_assert(traits::is_unit<UnitFrom>::value, "Template parameter `UnitFrom` must be a `unit` type.");
		static_assert(traits::is_unit<UnitTo>::value, "Template parameter `UnitTo` must be a `unit` type.");
		static_assert(traits::is_convertible_unit<UnitFrom, UnitTo>::value, "Units are not compatible.");

		using Ratio = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
		using PiRatio = std::ratio_subtract<typename UnitFrom::pi_exponent_ratio, typename UnitTo::pi_exponent_ratio>;
		using Translation = std::ratio_divide<std::ratio_subtract<typename UnitFrom::translation_ratio, typename UnitTo::translation_ratio>, typename UnitTo::conversion_ratio>;

		using isSame = typename std::is_same<std::decay_t<UnitFrom>, std::decay_t<UnitTo>>::type;
		using piRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, PiRatio>::value)>;
		using translationRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, Translation>::value)>;

		return units::detail::convert<UnitFrom, UnitTo, Ratio, PiRatio, Translation, T>
			(value, isSame{}, piRequired{}, translationRequired{});
	}

	//----------------------------------
	//	NON-LINEAR SCALE TRAITS
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace traits
	{
		namespace detail
		{
			/**
			* @brief		implementation of has_operator_parenthesis
			* @details		checks that operator() returns the same type as `Ret`
			*/
			template<class T, class Ret>
			struct has_operator_parenthesis_impl
			{
				template<class U>
				static constexpr auto test(U*) -> decltype(std::declval<U>()()) { return decltype(std::declval<U>()()){}; }
				template<typename>
				static constexpr std::false_type test(...) { return std::false_type{}; }

				using type = typename std::is_same<Ret, decltype(test<T>(0))>::type;
			};
		}

		/**
		 * @brief		checks that `class T` has an `operator()` member which returns `Ret`
		 * @details		used as part of the linear_scale concept.
		 */
		template<class T, class Ret>
		struct has_operator_parenthesis : traits::detail::has_operator_parenthesis_impl<T, Ret>::type {};
	}

	namespace traits
	{
		namespace detail
		{
			/**
			* @brief		implementation of has_value_member
			* @details		checks for a member named `m_member` with type `Ret`
			*/
			template<class T, class Ret>
			struct has_value_member_impl
			{
				template<class U>
				static constexpr auto test(U* p) -> decltype(p->m_value) { return p->m_value; }
				template<typename>
				static constexpr auto test(...)->std::false_type { return std::false_type{}; }

				using type = typename std::is_same<std::decay_t<Ret>, std::decay_t<decltype(test<T>(0))>>::type;
			};
		}

		/**
		 * @brief		checks for a member named `m_member` with type `Ret`
		 * @details		used as part of the linear_scale concept checker.
		 */
		template<class T, class Ret>
		struct has_value_member : traits::detail::has_value_member_impl<T, Ret>::type {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests that `class T` meets the requirements for a non-linear scale
		 * @details		A non-linear scale must:
		 *				- be default constructible
		 *				- have an `operator()` member which returns the non-linear value stored in the scale
		 *				- have an accessible `m_value` member type which stores the linearized value in the scale.
		 *
		 *				Linear/nonlinear scales are used by `units::unit` to store values and scale them
		 *				if they represent things like dB.
		 */
		template<class T, class Ret>
		struct is_nonlinear_scale : std::integral_constant<bool,
			std::is_default_constructible<T>::value &&
			has_operator_parenthesis<T, Ret>::value &&
			has_value_member<T, Ret>::value &&
			std::is_trivial<T>::value>
		{};
	}

	//------------------------------
	//	UNIT_T TYPE TRAITS
	//------------------------------

	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSOES_ONLY
		/**
		* @ingroup		TypeTraits
		* @brief		Trait for accessing the publically defined types of `units::unit_t`
		* @details		The units library determines certain properties of the unit_t types passed to them
		*				and what they represent by using the members of the corresponding unit_t_traits instantiation.
		*/
		template<typename T>
		struct unit_t_traits
		{
			typedef typename T::non_linear_scale_type non_linear_scale_type;	///< Type of the unit_t non_linear_scale (e.g. linear_scale, decibel_scale). This property is used to enable the proper linear or logarithmic arithmetic functions.
			typedef typename T::underlying_type underlying_type;				///< Underlying storage type of the `unit_t`, e.g. `double`.
			typedef typename T::value_type value_type;							///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
			typedef typename T::unit_type unit_type;							///< Type of unit the `unit_t` represents, e.g. `meters`
		};
#endif

		/** @cond */	// DOXYGEN IGNORE
		/**
		 * @brief		unit_t_traits specialization for things which are not unit_t
		 * @details
		 */
		template<typename T, typename = void>
		struct unit_t_traits
		{
			typedef void non_linear_scale_type;
			typedef void underlying_type;
			typedef void value_type;
			typedef void unit_type;
		};

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publically defined types of `units::unit_t`
		 * @details
		 */
		template<typename T>
		struct unit_t_traits <T, typename void_t<
			typename T::non_linear_scale_type,
			typename T::underlying_type,
			typename T::value_type,
			typename T::unit_type>::type>
		{
			typedef typename T::non_linear_scale_type non_linear_scale_type;
			typedef typename T::underlying_type underlying_type;
			typedef typename T::value_type value_type;
			typedef typename T::unit_type unit_type;
		};
		/** @endcond */	// END DOXYGEN IGNORE
	}

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether two container types derived from `unit_t` are convertible to each other
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit_t<U1, U2>::value` to test
		 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic meaning,
		 *				(i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters can be
		 *				converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2, then
		 *				U2 will be convertible to U1.
		 * @tparam		U1 Unit to convert from.
		 * @tparam		U2 Unit to convert to.
		 * @sa			is_convertible_unit
		 */
		template<class U1, class U2>
		struct is_convertible_unit_t : std::integral_constant<bool,
			is_convertible_unit<typename units::traits::unit_t_traits<U1>::unit_type, typename units::traits::unit_t_traits<U2>::unit_type>::value>
		{};
	}

	//----------------------------------
	//	UNIT TYPE
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	// forward declaration
	template<typename T> struct linear_scale;
	template<typename T> struct decibel_scale;

	namespace detail
	{
		/**
		* @brief		helper type to identify units.
		* @details		A non-templated base class for `unit` which enables RTTI testing.
		*/
		struct _unit_t {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
		// forward declaration
		#if !defined(_MSC_VER) || _MSC_VER > 1800 // bug in VS2013 prevents this from working
		template<typename... T> struct is_dimensionless_unit;
		#else
		template<typename T1, typename T2 = T1, typename T3 = T1> struct is_dimensionless_unit;
		#endif

		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits which tests if a class is a `unit`
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit<T>::value` to test
		 *				whether `class T` implements a `unit`.
		 */
		template<class T>
		struct is_unit_t : std::is_base_of<units::detail::_unit_t, T>::type {};
	}

	/**
	 * @ingroup		UnitContainers
	 * @brief		Container for values which represent quantities of a given unit.
	 * @details		Stores a value which represents a quantity in the given units. Unit containers
	 *				(except scalar values) are *not* convertible to built-in c++ types, in order to
	 *				provide type safety in dimensional analysis. Unit containers *are* implicitly
	 *				convertible to other compatible unit container types. Unit containers support
	 *				various types of arithmetic operations, depending on their scale type.
	 *
	 *				The value of a `unit_t` can only be changed on construction, or by assignment
	 *				from another `unit_t` type. If necessary, the underlying value can be accessed
	 *				using `operator()`: @code
	 *				meter_t m(5.0);
	 *				double val = m(); // val == 5.0	@endcode.
	 * @tparam		Units unit tag for which type of units the `unit_t` represents (e.g. meters)
	 * @tparam		T underlying type of the storage. Defaults to double.
	 * @tparam		NonLinearScale optional scale class for the units. Defaults to linear (i.e. does
	 *				not scale the unit value). Examples of non-linear scales could be logarithmic,
	 *				decibel, or richter scales. Non-linear scales must adhere to the non-linear-scale
	 *				concept, i.e. `is_nonlinear_scale<...>::value` must be `true`.
	 * @sa
	 *				- \ref lengthContainers "length unit containers"
	 *				- \ref massContainers "mass unit containers"
	 *				- \ref timeContainers "time unit containers"
	 *				- \ref angleContainers "angle unit containers"
	 *				- \ref currentContainers "current unit containers"
	 *				- \ref temperatureContainers "temperature unit containers"
	 *				- \ref substanceContainers "substance unit containers"
	 *				- \ref luminousIntensityContainers "luminous intensity unit containers"
	 *				- \ref solidAngleContainers "solid angle unit containers"
	 *				- \ref frequencyContainers "frequency unit containers"
	 *				- \ref velocityContainers "velocity unit containers"
	 *				- \ref angularVelocityContainers "angular velocity unit containers"
	 *				- \ref accelerationContainers "acceleration unit containers"
	 *				- \ref forceContainers "force unit containers"
	 *				- \ref pressureContainers "pressure unit containers"
	 *				- \ref chargeContainers "charge unit containers"
	 *				- \ref energyContainers "energy unit containers"
	 *				- \ref powerContainers "power unit containers"
	 *				- \ref voltageContainers "voltage unit containers"
	 *				- \ref capacitanceContainers "capacitance unit containers"
	 *				- \ref impedanceContainers "impedance unit containers"
	 *				- \ref magneticFluxContainers "magnetic flux unit containers"
	 *				- \ref magneticFieldStrengthContainers "magnetic field strength unit containers"
	 *				- \ref inductanceContainers "inductance unit containers"
	 *				- \ref luminousFluxContainers "luminous flux unit containers"
	 *				- \ref illuminanceContainers "illuminance unit containers"
	 *				- \ref radiationContainers "radiation unit containers"
	 *				- \ref torqueContainers "torque unit containers"
	 *				- \ref areaContainers "area unit containers"
	 *				- \ref volumeContainers "volume unit containers"
	 *				- \ref densityContainers "density unit containers"
	 *				- \ref concentrationContainers "concentration unit containers"
	 *				- \ref constantContainers "constant unit containers"
	 */
	template<class Units, typename T = UNIT_LIB_DEFAULT_TYPE, template<typename> class NonLinearScale = linear_scale>
	class unit_t : public NonLinearScale<T>, units::detail::_unit_t
	{
		static_assert(traits::is_unit<Units>::value, "Template parameter `Units` must be a unit tag. Check that you aren't using a unit type (_t).");
		static_assert(traits::is_nonlinear_scale<NonLinearScale<T>, T>::value, "Template parameter `NonLinearScale` does not conform to the `is_nonlinear_scale` concept.");

	protected:

		using nls = NonLinearScale<T>;
		using nls::m_value;

	public:

		typedef NonLinearScale<T> non_linear_scale_type;											///< Type of the non-linear scale of the unit_t (e.g. linear_scale)
		typedef T underlying_type;																	///< Type of the underlying storage of the unit_t (e.g. double)
		typedef T value_type;																		///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
		typedef Units unit_type;																	///< Type of `unit` the `unit_t` represents (e.g. meters)

		/**
		 * @ingroup		Constructors
		 * @brief		default constructor.
		 */
		constexpr unit_t() = default;

		/**
		 * @brief		constructor
		 * @details		constructs a new unit_t using the non-linear scale's constructor.
		 * @param[in]	value	unit value magnitude.
		 * @param[in]	args	additional constructor arguments are forwarded to the non-linear scale constructor. Which
		 *						args are required depends on which scale is used. For the default (linear) scale,
		 *						no additional args are necessary.
		 */
		template<class... Args>
		inline explicit constexpr unit_t(const T value, const Args&... args) noexcept : nls(value, args...)
		{

		}

		/**
		 * @brief		constructor
		 * @details		enable implicit conversions from T types ONLY for linear scalar units
		 * @param[in]	value value of the unit_t
		 */
		template<class Ty, class = typename std::enable_if<traits::is_dimensionless_unit<Units>::value && std::is_arithmetic<Ty>::value>::type>
		inline constexpr unit_t(const Ty value) noexcept : nls(value)
		{

		}

		/**
		 * @brief		chrono constructor
		 * @details		enable implicit conversions from std::chrono::duration types ONLY for time units
		 * @param[in]	value value of the unit_t
		 */
		template<class Rep, class Period, class = std::enable_if_t<std::is_arithmetic<Rep>::value && traits::is_ratio<Period>::value>>
		inline constexpr unit_t(const std::chrono::duration<Rep, Period>& value) noexcept :
		nls(units::convert<unit<std::ratio<1,1000000000>, category::time_unit>, Units>(static_cast<T>(std::chrono::duration_cast<std::chrono::nanoseconds>(value).count())))
		{

		}

		/**
		 * @brief		copy constructor (converting)
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr unit_t(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) noexcept :
		nls(units::convert<UnitsRhs, Units, T>(rhs.m_value), std::true_type() /*store linear value*/)
		{

		}

		/**
		 * @brief		assignment
		 * @details		performs implicit unit conversions if required
		 * @param[in]	rhs unit to copy.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline unit_t& operator=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) noexcept
		{
			nls::m_value = units::convert<UnitsRhs, Units, T>(rhs.m_value);
			return *this;
		}

		/**
		* @brief		assignment
		* @details		performs implicit conversions from built-in types ONLY for scalar units
		* @param[in]	rhs value to copy.
		*/
		template<class Ty, class = std::enable_if_t<traits::is_dimensionless_unit<Units>::value && std::is_arithmetic<Ty>::value>>
		inline unit_t& operator=(const Ty& rhs) noexcept
		{
			nls::m_value = rhs;
			return *this;
		}

		/**
		 * @brief		less-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr bool operator<(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return (nls::m_value < units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		less-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than or equal to the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr bool operator<=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return (nls::m_value <= units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		greater-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr bool operator>(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return (nls::m_value > units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		greater-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than or equal to the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr bool operator>=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return (nls::m_value >= units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		equality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` exactly equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit_t is a double.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs, std::enable_if_t<std::is_floating_point<T>::value || std::is_floating_point<Ty>::value, int> = 0>
		inline constexpr bool operator==(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return detail::abs(nls::m_value - units::convert<UnitsRhs, Units>(rhs.m_value)) < std::numeric_limits<T>::epsilon() *
				detail::abs(nls::m_value + units::convert<UnitsRhs, Units>(rhs.m_value)) ||
				detail::abs(nls::m_value - units::convert<UnitsRhs, Units>(rhs.m_value)) < (std::numeric_limits<T>::min)();
		}

		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs, std::enable_if_t<std::is_integral<T>::value && std::is_integral<Ty>::value, int> = 0>
		inline constexpr bool operator==(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return nls::m_value == units::convert<UnitsRhs, Units>(rhs.m_value);
		}

		/**
		 * @brief		inequality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is not equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit_t is a double.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline constexpr bool operator!=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit in it's underlying, non-safe type.
		 */
		inline constexpr underlying_type value() const noexcept
		{
			return static_cast<underlying_type>(*this);
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit converted to an arithmetic, non-safe type.
		 */
		template<typename Ty, class = std::enable_if_t<std::is_arithmetic<Ty>::value>>
		inline constexpr Ty to() const noexcept
		{
			return static_cast<Ty>(*this);
		}

		/**
		 * @brief		linearized unit value
		 * @returns		linearized value of unit which has a non-linear scale. For `unit_t` types with
		 *				linear scales, this is equivalent to `value`.
		 */
		template<typename Ty, class = std::enable_if_t<std::is_arithmetic<Ty>::value>>
		inline constexpr Ty toLinearized() const noexcept
		{
			return static_cast<Ty>(m_value);
		}

		/**
		 * @brief		conversion
		 * @details		Converts to a different unit container. Units can be converted to other containers
		 *				implicitly, but this can be used in cases where explicit notation of a conversion
		 *				is beneficial, or where an r-value container is needed.
		 * @tparam		U unit (not unit_t) to convert to
		 * @returns		a unit container with the specified units containing the equivalent value to
		 *				*this.
		 */
		template<class U>
		inline constexpr unit_t<U> convert() const noexcept
		{
			static_assert(traits::is_unit<U>::value, "Template parameter `U` must be a unit type.");
			return unit_t<U>(*this);
		}

		/**
		 * @brief		implicit type conversion.
		 * @details		only enabled for scalar unit types.
		 */
		template<class Ty, std::enable_if_t<traits::is_dimensionless_unit<Units>::value && std::is_arithmetic<Ty>::value, int> = 0>
		inline constexpr operator Ty() const noexcept
		{
			// this conversion also resolves any PI exponents, by converting from a non-zero PI ratio to a zero-pi ratio.
			return static_cast<Ty>(units::convert<Units, unit<std::ratio<1>, units::category::scalar_unit>>((*this)()));
		}

		/**
		 * @brief		explicit type conversion.
		 * @details		only enabled for non-dimensionless unit types.
		 */
		template<class Ty, std::enable_if_t<!traits::is_dimensionless_unit<Units>::value && std::is_arithmetic<Ty>::value, int> = 0>
		inline constexpr explicit operator Ty() const noexcept
		{
			return static_cast<Ty>((*this)());
		}

		/**
		 * @brief		chrono implicit type conversion.
		 * @details		only enabled for time unit types.
		 */
		template<typename U = Units, std::enable_if_t<units::traits::is_convertible_unit<U, unit<std::ratio<1>, category::time_unit>>::value, int> = 0>
		inline constexpr operator std::chrono::nanoseconds() const noexcept
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double, std::nano>(units::convert<Units, unit<std::ratio<1,1000000000>, category::time_unit>>((*this)())));
		}

		/**
		 * @brief		returns the unit name
		 */
		inline constexpr const char* name() const noexcept
		{
			return units::name(*this);
		}

		/**
		 * @brief		returns the unit abbreviation
		 */
		inline constexpr const char* abbreviation() const noexcept
		{
			return units::abbreviation(*this);
		}

	public:

		template<class U, typename Ty, template<typename> class Nlt>
		friend class unit_t;
	};

	//------------------------------
	//	UNIT_T NON-MEMBER FUNCTIONS
	//------------------------------

	/**
	 * @ingroup		UnitContainers
	 * @brief		Constructs a unit container from an arithmetic type.
	 * @details		make_unit can be used to construct a unit container from an arithmetic type, as an alternative to
	 *				using the explicit constructor. Unlike the explicit constructor it forces the user to explicitly
	 *				specify the units.
	 * @tparam		UnitType Type to construct.
	 * @tparam		Ty		Arithmetic type.
	 * @param[in]	value	Arithmetic value that represents a quantity in units of `UnitType`.
	 */
	template<class UnitType, typename T, class = std::enable_if_t<std::is_arithmetic<T>::value>>
	inline constexpr UnitType make_unit(const T value) noexcept
	{
		static_assert(traits::is_unit_t<UnitType>::value, "Template parameter `UnitType` must be a unit type (_t).");

		return UnitType(value);
	}

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline std::ostream& operator<<(std::ostream& os, const unit_t<Units, T, NonLinearScale>& obj) noexcept
	{
		using BaseUnits = unit<std::ratio<1>, typename traits::unit_traits<Units>::base_unit_type>;
		os << convert<Units, BaseUnits>(obj());

		if (traits::unit_traits<Units>::base_unit_type::meter_ratio::num != 0) { os << " m"; }
		if (traits::unit_traits<Units>::base_unit_type::meter_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::meter_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::meter_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::meter_ratio::den != 1) { os << "/"   << traits::unit_traits<Units>::base_unit_type::meter_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::kilogram_ratio::num != 0) { os << " kg"; }
		if (traits::unit_traits<Units>::base_unit_type::kilogram_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::kilogram_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::kilogram_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::kilogram_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::kilogram_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::second_ratio::num != 0) { os << " s"; }
		if (traits::unit_traits<Units>::base_unit_type::second_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::second_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::second_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::second_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::second_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::ampere_ratio::num != 0) { os << " A"; }
		if (traits::unit_traits<Units>::base_unit_type::ampere_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::ampere_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::ampere_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::ampere_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::ampere_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::kelvin_ratio::num != 0) { os << " K"; }
		if (traits::unit_traits<Units>::base_unit_type::kelvin_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::kelvin_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::kelvin_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::kelvin_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::kelvin_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::mole_ratio::num != 0) { os << " mol"; }
		if (traits::unit_traits<Units>::base_unit_type::mole_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::mole_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::mole_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::mole_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::mole_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::candela_ratio::num != 0) { os << " cd"; }
		if (traits::unit_traits<Units>::base_unit_type::candela_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::candela_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::candela_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::candela_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::candela_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::radian_ratio::num != 0) { os << " rad"; }
		if (traits::unit_traits<Units>::base_unit_type::radian_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::radian_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::radian_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::radian_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::radian_ratio::den; }

		if (traits::unit_traits<Units>::base_unit_type::byte_ratio::num != 0) { os << " b"; }
		if (traits::unit_traits<Units>::base_unit_type::byte_ratio::num != 0 &&
			traits::unit_traits<Units>::base_unit_type::byte_ratio::num != 1) { os << "^" << traits::unit_traits<Units>::base_unit_type::byte_ratio::num; }
		if (traits::unit_traits<Units>::base_unit_type::byte_ratio::den != 1) { os << "/" << traits::unit_traits<Units>::base_unit_type::byte_ratio::den; }

		return os;
	}
#endif

	template<class Units, typename T, template<typename> class NonLinearScale, typename RhsType>
	inline unit_t<Units, T, NonLinearScale>& operator+=(unit_t<Units, T, NonLinearScale>& lhs, const RhsType& rhs) noexcept
	{
		static_assert(traits::is_convertible_unit_t<unit_t<Units, T, NonLinearScale>, RhsType>::value ||
			(traits::is_dimensionless_unit<decltype(lhs)>::value && std::is_arithmetic<RhsType>::value),
			"parameters are not compatible units.");

		lhs = lhs + rhs;
		return lhs;
	}

	template<class Units, typename T, template<typename> class NonLinearScale, typename RhsType>
	inline unit_t<Units, T, NonLinearScale>& operator-=(unit_t<Units, T, NonLinearScale>& lhs, const RhsType& rhs) noexcept
	{
		static_assert(traits::is_convertible_unit_t<unit_t<Units, T, NonLinearScale>, RhsType>::value ||
			(traits::is_dimensionless_unit<decltype(lhs)>::value && std::is_arithmetic<RhsType>::value),
			"parameters are not compatible units.");

		lhs = lhs - rhs;
		return lhs;
	}

	template<class Units, typename T, template<typename> class NonLinearScale, typename RhsType>
	inline unit_t<Units, T, NonLinearScale>& operator*=(unit_t<Units, T, NonLinearScale>& lhs, const RhsType& rhs) noexcept
	{
		static_assert((traits::is_dimensionless_unit<RhsType>::value || std::is_arithmetic<RhsType>::value),
			"right-hand side parameter must be dimensionless.");

		lhs = lhs * rhs;
		return lhs;
	}

	template<class Units, typename T, template<typename> class NonLinearScale, typename RhsType>
	inline unit_t<Units, T, NonLinearScale>& operator/=(unit_t<Units, T, NonLinearScale>& lhs, const RhsType& rhs) noexcept
	{
		static_assert((traits::is_dimensionless_unit<RhsType>::value || std::is_arithmetic<RhsType>::value),
			"right-hand side parameter must be dimensionless.");

		lhs = lhs / rhs;
		return lhs;
	}

	//------------------------------
	//	UNIT_T UNARY OPERATORS
	//------------------------------

	// unary addition: +T
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale> operator+(const unit_t<Units, T, NonLinearScale>& u) noexcept
	{
		return u;
	}

	// prefix increment: ++T
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale>& operator++(unit_t<Units, T, NonLinearScale>& u) noexcept
	{
		u = unit_t<Units, T, NonLinearScale>(u() + 1);
		return u;
	}

	// postfix increment: T++
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale> operator++(unit_t<Units, T, NonLinearScale>& u, int) noexcept
	{
		auto ret = u;
		u = unit_t<Units, T, NonLinearScale>(u() + 1);
		return ret;
	}

	// unary addition: -T
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale> operator-(const unit_t<Units, T, NonLinearScale>& u) noexcept
	{
		return unit_t<Units, T, NonLinearScale>(-u());
	}

	// prefix increment: --T
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale>& operator--(unit_t<Units, T, NonLinearScale>& u) noexcept
	{
		u = unit_t<Units, T, NonLinearScale>(u() - 1);
		return u;
	}

	// postfix increment: T--
	template<class Units, typename T, template<typename> class NonLinearScale>
	inline unit_t<Units, T, NonLinearScale> operator--(unit_t<Units, T, NonLinearScale>& u, int) noexcept
	{
		auto ret = u;
		u = unit_t<Units, T, NonLinearScale>(u() - 1);
		return ret;
	}

	//------------------------------
	//	UNIT_CAST
	//------------------------------

	/**
	 * @ingroup		Conversion
	 * @brief		Casts a unit container to an arithmetic type.
	 * @details		unit_cast can be used to remove the strong typing from a unit class, and convert it
	 *				to a built-in arithmetic type. This may be useful for compatibility with libraries
	 *				and legacy code that don't support `unit_t` types. E.g
	 * @code		meter_t unitVal(5);
	 *  double value = units::unit_cast<double>(unitVal);	// value = 5.0
	 * @endcode
	 * @tparam		T		Type to cast the unit type to. Must be a built-in arithmetic type.
	 * @param		value	Unit value to cast.
	 * @sa			unit_t::to
	 */
	template<typename T, typename Units, class = std::enable_if_t<std::is_arithmetic<T>::value && traits::is_unit_t<Units>::value>>
	inline constexpr T unit_cast(const Units& value) noexcept
	{
		return static_cast<T>(value);
	}

	//------------------------------
	//	NON-LINEAR SCALE TRAITS
	//------------------------------

	// forward declaration
	template<typename T> struct decibel_scale;

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a linear scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_linear_scale<U1 [, U2, ...]>::value` to test
		 *				one or more types to see if they represent unit_t's whose scale is linear.
		 * @tparam		T	one or more types to test.
		 */
#if !defined(_MSC_VER) || _MSC_VER > 1800	// bug in VS2013 prevents this from working
		template<typename... T>
		struct has_linear_scale : std::integral_constant<bool, units::all_true<std::is_base_of<units::linear_scale<typename units::traits::unit_t_traits<T>::underlying_type>, T>::value...>::value > {};
#else
		template<typename T1, typename T2 = T1, typename T3 = T1>
		struct has_linear_scale : std::integral_constant<bool,
			std::is_base_of<units::linear_scale<typename units::traits::unit_t_traits<T1>::underlying_type>, T1>::value &&
			std::is_base_of<units::linear_scale<typename units::traits::unit_t_traits<T2>::underlying_type>, T2>::value &&
			std::is_base_of<units::linear_scale<typename units::traits::unit_t_traits<T3>::underlying_type>, T3>::value> {};
#endif

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a decibel scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_decibel_scale<U1 [, U2, ...]>::value` to test
		 *				one or more types to see if they represent unit_t's whose scale is in decibels.
		 * @tparam		T	one or more types to test.
		 */
#if !defined(_MSC_VER) || _MSC_VER > 1800	// bug in VS2013 prevents this from working
		template<typename... T>
		struct has_decibel_scale : std::integral_constant<bool,	units::all_true<std::is_base_of<units::decibel_scale<typename units::traits::unit_t_traits<T>::underlying_type>, T>::value...>::value> {};
#else
		template<typename T1, typename T2 = T1, typename T3 = T1>
		struct has_decibel_scale : std::integral_constant<bool,
			std::is_base_of<units::decibel_scale<typename units::traits::unit_t_traits<T1>::underlying_type>, T1>::value &&
			std::is_base_of<units::decibel_scale<typename units::traits::unit_t_traits<T2>::underlying_type>, T2>::value &&
			std::is_base_of<units::decibel_scale<typename units::traits::unit_t_traits<T2>::underlying_type>, T3>::value> {};
#endif

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether two types has the same non-linear scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_same_scale<U1 , U2>::value` to test
		 *				whether two types have the same non-linear scale.
		 * @tparam		T1	left hand type.
		 * @tparam		T2	right hand type
		 */
		template<typename T1, typename T2>
		struct is_same_scale : std::integral_constant<bool,
			std::is_same<typename units::traits::unit_t_traits<T1>::non_linear_scale_type, typename units::traits::unit_t_traits<T2>::non_linear_scale_type>::value>
		{};
	}

	//----------------------------------
	//	NON-LINEAR SCALES
	//----------------------------------

	// Non-linear transforms are used to pre and post scale units which are defined in terms of non-
	// linear functions of their current value. A good example of a non-linear scale would be a
	// logarithmic or decibel scale

	//------------------------------
	//	LINEAR SCALE
	//------------------------------

	/**
	 * @brief		unit_t scale which is linear
	 * @details		Represents units on a linear scale. This is the appropriate unit_t scale for almost
	 *				all units almost all of the time.
	 * @tparam		T	underlying storage type
	 * @sa			unit_t
	 */
	template<typename T>
	struct linear_scale
	{
		inline constexpr linear_scale() = default;													///< default constructor.
		inline constexpr linear_scale(const linear_scale&) = default;
		inline ~linear_scale() = default;
		inline linear_scale& operator=(const linear_scale&) = default;
#if defined(_MSC_VER) && (_MSC_VER > 1800)
		inline constexpr linear_scale(linear_scale&&) = default;
		inline linear_scale& operator=(linear_scale&&) = default;
#endif
		template<class... Args>
		inline constexpr linear_scale(const T& value, Args&&...) noexcept : m_value(value) {}	///< constructor.
		inline constexpr T operator()() const noexcept { return m_value; }							///< returns value.

		T m_value;																					///< linearized value.
	};

	//----------------------------------
	//	SCALAR (LINEAR) UNITS
	//----------------------------------

	// Scalar units are the *ONLY* units implicitly convertible to/from built-in types.
	namespace dimensionless
	{
		typedef unit<std::ratio<1>, units::category::scalar_unit> scalar;
		typedef unit<std::ratio<1>, units::category::dimensionless_unit> dimensionless;

		typedef unit_t<scalar> scalar_t;
		typedef scalar_t dimensionless_t;
	}

// ignore the redeclaration of the default template parameters
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4348)
#endif
	UNIT_ADD_CATEGORY_TRAIT(scalar)
	UNIT_ADD_CATEGORY_TRAIT(dimensionless)
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

	//------------------------------
	//	LINEAR ARITHMETIC
	//------------------------------

	template<class UnitTypeLhs, class UnitTypeRhs, std::enable_if_t<!traits::is_same_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
	constexpr inline int operator+(const UnitTypeLhs& /* lhs */, const UnitTypeRhs& /* rhs */) noexcept
	{
		static_assert(traits::is_same_scale<UnitTypeLhs, UnitTypeRhs>::value, "Cannot add units with different linear/non-linear scales.");
		return 0;
	}

	/// Addition operator for unit_t types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs, std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
	inline constexpr UnitTypeLhs operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return UnitTypeLhs(lhs() + convert<UnitsRhs, UnitsLhs>(rhs()));
	}

	/// Addition operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
	inline constexpr dimensionless::scalar_t operator+(const dimensionless::scalar_t& lhs, T rhs) noexcept
	{
		return dimensionless::scalar_t(lhs() + rhs);
	}

	/// Addition operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
	inline constexpr dimensionless::scalar_t operator+(T lhs, const dimensionless::scalar_t& rhs) noexcept
	{
		return dimensionless::scalar_t(lhs + rhs());
	}

	/// Subtraction operator for unit_t types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs, std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
	inline constexpr UnitTypeLhs operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return UnitTypeLhs(lhs() - convert<UnitsRhs, UnitsLhs>(rhs()));
	}

	/// Subtraction operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
	inline constexpr dimensionless::scalar_t operator-(const dimensionless::scalar_t& lhs, T rhs) noexcept
	{
		return dimensionless::scalar_t(lhs() - rhs);
	}

	/// Subtraction operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
	inline constexpr dimensionless::scalar_t operator-(T lhs, const dimensionless::scalar_t& rhs) noexcept
	{
		return dimensionless::scalar_t(lhs - rhs());
	}

	/// Multiplication type for convertible unit_t types with a linear scale. @returns the multiplied value, with the same type as left-hand side unit.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
		inline constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<compound_unit<squared<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>>>
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return  unit_t<compound_unit<squared<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>>>
			(lhs() * convert<UnitsRhs, UnitsLhs>(rhs()));
	}

	/// Multiplication type for non-convertible unit_t types with a linear scale. @returns the multiplied value, whose type is a compound unit of the left and right hand side values.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<!traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<compound_unit<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type, typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return unit_t<compound_unit<UnitsLhs, UnitsRhs>>
			(lhs() * rhs());
	}

	/// Multiplication by a dimensionless unit for unit_t types with a linear scale.
	template<class UnitTypeLhs, typename UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value && traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr UnitTypeLhs operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		// the cast makes sure factors of PI are handled as expected
		return UnitTypeLhs(lhs() * static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs));
	}

	/// Multiplication by a dimensionless unit for unit_t types with a linear scale.
	template<class UnitTypeLhs, typename UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && traits::is_dimensionless_unit<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr UnitTypeRhs operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		// the cast makes sure factors of PI are handled as expected
		return UnitTypeRhs(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) * rhs());
	}

	/// Multiplication by a scalar for unit_t types with a linear scale.
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic<T>::value && traits::has_linear_scale<UnitTypeLhs>::value, int> = 0>
		inline constexpr UnitTypeLhs operator*(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		return UnitTypeLhs(lhs() * rhs);
	}

	/// Multiplication by a scalar for unit_t types with a linear scale.
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic<T>::value && traits::has_linear_scale<UnitTypeRhs>::value, int> = 0>
		inline constexpr UnitTypeRhs operator*(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		return UnitTypeRhs(lhs * rhs());
	}

	/// Division for convertible unit_t types with a linear scale. @returns the lhs divided by rhs value, whose type is a scalar
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
		inline constexpr dimensionless::scalar_t operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return dimensionless::scalar_t(lhs() / convert<UnitsRhs, UnitsLhs>(rhs()));
	}

	/// Division for non-convertible unit_t types with a linear scale. @returns the lhs divided by the rhs, with a compound unit type of lhs/rhs
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<!traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept ->  unit_t<compound_unit<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type, inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>>
	{
		using UnitsLhs = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return unit_t<compound_unit<UnitsLhs, inverse<UnitsRhs>>>
			(lhs() / rhs());
	}

	/// Division by a dimensionless unit for unit_t types with a linear scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value && traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr UnitTypeLhs operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		return UnitTypeLhs(lhs() / static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs));
	}

	/// Division of a dimensionless unit  by a unit_t type with a linear scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value && traits::is_dimensionless_unit<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
		inline constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>
	{
		return unit_t<inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>
			(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) / rhs());
	}

	/// Division by a scalar for unit_t types with a linear scale
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic<T>::value && traits::has_linear_scale<UnitTypeLhs>::value, int> = 0>
		inline constexpr UnitTypeLhs operator/(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		return UnitTypeLhs(lhs() / rhs);
	}

	/// Division of a scalar  by a unit_t type with a linear scale
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic<T>::value && traits::has_linear_scale<UnitTypeRhs>::value, int> = 0>
		inline constexpr auto operator/(T lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>
	{
		using UnitsRhs = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		return unit_t<inverse<UnitsRhs>>
			(lhs / rhs());
	}

	//----------------------------------
	//	SCALAR COMPARISONS
	//----------------------------------

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator==(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return detail::abs(lhs - static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs)) < std::numeric_limits<UNIT_LIB_DEFAULT_TYPE>::epsilon() * detail::abs(lhs + static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs)) ||
			detail::abs(lhs - static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs)) < (std::numeric_limits<UNIT_LIB_DEFAULT_TYPE>::min)();
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator==(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return detail::abs(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) - rhs) < std::numeric_limits<UNIT_LIB_DEFAULT_TYPE>::epsilon() * detail::abs(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) + rhs) ||
			detail::abs(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) - rhs) < (std::numeric_limits<UNIT_LIB_DEFAULT_TYPE>::min)();
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator!=(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return!(lhs == static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs));
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator!=(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return !(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) == rhs);
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator>=(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return std::isgreaterequal(lhs, static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs));
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator>=(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return std::isgreaterequal(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs), rhs);
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator>(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return lhs > static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs);
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator>(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) > rhs;
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator<=(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return std::islessequal(lhs, static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs));
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator<=(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return std::islessequal(static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs), rhs);
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator<(const UNIT_LIB_DEFAULT_TYPE lhs, const Units& rhs) noexcept
	{
		return lhs < static_cast<UNIT_LIB_DEFAULT_TYPE>(rhs);
	}

	template<typename Units, class = std::enable_if_t<units::traits::is_dimensionless_unit<Units>::value>>
	constexpr bool operator<(const Units& lhs, const UNIT_LIB_DEFAULT_TYPE rhs) noexcept
	{
		return static_cast<UNIT_LIB_DEFAULT_TYPE>(lhs) < rhs;
	}

	//----------------------------------
	//	POW
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/// recursive exponential implementation
		template <int N, class U> struct power_of_unit
		{
			typedef typename units::detail::unit_multiply<U, typename power_of_unit<N - 1, U>::type> type;
		};

		/// End recursion
		template <class U> struct power_of_unit<1, U>
		{
			typedef U type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace math
	{
		/**
		 * @brief		computes the value of <i>value</i> raised to the <i>power</i>
		 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the resulting unit type can be deduced.
		 * @tparam		power exponential power to raise <i>value</i> by.
		 * @param[in]	value `unit_t` derived type to raise to the given <i>power</i>
		 * @returns		new unit_t, raised to the given exponent
		 */
		template<int power, class UnitType, class = typename std::enable_if<traits::has_linear_scale<UnitType>::value, int>>
		inline auto pow(const UnitType& value) noexcept -> unit_t<typename units::detail::power_of_unit<power, typename units::traits::unit_t_traits<UnitType>::unit_type>::type, typename units::traits::unit_t_traits<UnitType>::underlying_type, linear_scale>
		{
			return unit_t<typename units::detail::power_of_unit<power, typename units::traits::unit_t_traits<UnitType>::unit_type>::type, typename units::traits::unit_t_traits<UnitType>::underlying_type, linear_scale>
				(std::pow(value(), power));
		}

		/**
		 * @brief		computes the value of <i>value</i> raised to the <i>power</i> as a constexpr
		 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the resulting unit type can be deduced.
		 *				Additionally, the power must be <i>a positive, integral, value</i>.
		 * @tparam		power exponential power to raise <i>value</i> by.
		 * @param[in]	value `unit_t` derived type to raise to the given <i>power</i>
		 * @returns		new unit_t, raised to the given exponent
		 */
		template<int power, class UnitType, class = typename std::enable_if<traits::has_linear_scale<UnitType>::value, int>>
		inline constexpr auto cpow(const UnitType& value) noexcept -> unit_t<typename units::detail::power_of_unit<power, typename units::traits::unit_t_traits<UnitType>::unit_type>::type, typename units::traits::unit_t_traits<UnitType>::underlying_type, linear_scale>
		{
			static_assert(power >= 0, "cpow cannot accept negative numbers. Try units::math::pow instead.");
			return unit_t<typename units::detail::power_of_unit<power, typename units::traits::unit_t_traits<UnitType>::unit_type>::type, typename units::traits::unit_t_traits<UnitType>::underlying_type, linear_scale>
				(detail::pow(value(), power));
		}
	}

	//------------------------------
	//	DECIBEL SCALE
	//------------------------------

	/**
	* @brief		unit_t scale for representing decibel values.
	* @details		internally stores linearized values. `operator()` returns the value in dB.
	* @tparam		T	underlying storage type
	* @sa			unit_t
	*/
	template<typename T>
	struct decibel_scale
	{
		inline constexpr decibel_scale() = default;
		inline constexpr decibel_scale(const decibel_scale&) = default;
		inline ~decibel_scale() = default;
		inline decibel_scale& operator=(const decibel_scale&) = default;
#if defined(_MSC_VER) && (_MSC_VER > 1800)
		inline constexpr decibel_scale(decibel_scale&&) = default;
		inline decibel_scale& operator=(decibel_scale&&) = default;
#endif
		inline constexpr decibel_scale(const T value) noexcept : m_value(std::pow(10, value / 10)) {}
		template<class... Args>
		inline constexpr decibel_scale(const T value, std::true_type, Args&&...) noexcept : m_value(value) {}
		inline constexpr T operator()() const noexcept { return 10 * std::log10(m_value); }

		T m_value;	///< linearized value
	};

	//------------------------------
	//	SCALAR (DECIBEL) UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers for units that have no dimension (scalar units)
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace dimensionless
	{
		typedef unit_t<scalar, UNIT_LIB_DEFAULT_TYPE, decibel_scale> dB_t;
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
		inline std::ostream& operator<<(std::ostream& os, const dB_t& obj) { os << obj() << " dB"; return os; }
#endif
		typedef dB_t dBi_t;
	}

	//------------------------------
	//	DECIBEL ARITHMETIC
	//------------------------------

	/// Addition for convertible unit_t types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_decibel_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
	constexpr inline auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<compound_unit<squared<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type>>, typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type, decibel_scale>
	{
		using LhsUnits = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using RhsUnits = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type;

		return unit_t<compound_unit<squared<LhsUnits>>, underlying_type, decibel_scale>
			(lhs.template toLinearized<underlying_type>() * convert<RhsUnits, LhsUnits>(rhs.template toLinearized<underlying_type>()), std::true_type());
	}

	/// Addition between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, std::enable_if_t<traits::has_decibel_scale<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value, int> = 0>
	constexpr inline UnitTypeLhs operator+(const UnitTypeLhs& lhs, const dimensionless::dB_t& rhs) noexcept
	{
		using underlying_type = typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type;
		return UnitTypeLhs(lhs.template toLinearized<underlying_type>() * rhs.template toLinearized<underlying_type>(), std::true_type());
	}

	/// Addition between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeRhs, std::enable_if_t<traits::has_decibel_scale<UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
	constexpr inline UnitTypeRhs operator+(const dimensionless::dB_t& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using underlying_type = typename units::traits::unit_t_traits<UnitTypeRhs>::underlying_type;
		return UnitTypeRhs(lhs.template toLinearized<underlying_type>() * rhs.template toLinearized<underlying_type>(), std::true_type());
	}

	/// Subtraction for convertible unit_t types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs, std::enable_if_t<traits::has_decibel_scale<UnitTypeLhs, UnitTypeRhs>::value, int> = 0>
	constexpr inline auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<compound_unit<typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type, inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>>, typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type, decibel_scale>
	{
		using LhsUnits = typename units::traits::unit_t_traits<UnitTypeLhs>::unit_type;
		using RhsUnits = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type;

		return unit_t<compound_unit<LhsUnits, inverse<RhsUnits>>, underlying_type, decibel_scale>
			(lhs.template toLinearized<underlying_type>() / convert<RhsUnits, LhsUnits>(rhs.template toLinearized<underlying_type>()), std::true_type());
	}

	/// Subtraction between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, std::enable_if_t<traits::has_decibel_scale<UnitTypeLhs>::value && !traits::is_dimensionless_unit<UnitTypeLhs>::value, int> = 0>
	constexpr inline UnitTypeLhs operator-(const UnitTypeLhs& lhs, const dimensionless::dB_t& rhs) noexcept
	{
		using underlying_type = typename units::traits::unit_t_traits<UnitTypeLhs>::underlying_type;
		return UnitTypeLhs(lhs.template toLinearized<underlying_type>() / rhs.template toLinearized<underlying_type>(), std::true_type());
	}

	/// Subtraction between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeRhs, std::enable_if_t<traits::has_decibel_scale<UnitTypeRhs>::value && !traits::is_dimensionless_unit<UnitTypeRhs>::value, int> = 0>
	constexpr inline auto operator-(const dimensionless::dB_t& lhs, const UnitTypeRhs& rhs) noexcept -> unit_t<inverse<typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type>, typename units::traits::unit_t_traits<UnitTypeRhs>::underlying_type, decibel_scale>
	{
		using RhsUnits = typename units::traits::unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename units::traits::unit_t_traits<RhsUnits>::underlying_type;

		return unit_t<inverse<RhsUnits>, underlying_type, decibel_scale>
			(lhs.template toLinearized<underlying_type>() / rhs.template toLinearized<underlying_type>(), std::true_type());
	}

	//----------------------------------
	//	UNIT RATIO CLASS
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		template<class Units>
		struct _unit_value_t {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSES_ONLY
		/**
		* @ingroup		TypeTraits
		* @brief		Trait for accessing the publically defined types of `units::unit_value_t_traits`
		* @details		The units library determines certain properties of the `unit_value_t` types passed to
		*				them and what they represent by using the members of the corresponding `unit_value_t_traits`
		*				instantiation.
		*/
		template<typename T>
		struct unit_value_t_traits
		{
			typedef typename T::unit_type unit_type;	///< Dimension represented by the `unit_value_t`.
			typedef typename T::ratio ratio;			///< Quantity represented by the `unit_value_t`, expressed as arational number.
		};
#endif

		/** @cond */	// DOXYGEN IGNORE
		/**
		 * @brief		unit_value_t_traits specialization for things which are not unit_t
		 * @details
		 */
		template<typename T, typename = void>
		struct unit_value_t_traits
		{
			typedef void unit_type;
			typedef void ratio;
		};

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publically defined types of `units::unit_value_t_traits`
		 * @details
		 */
		template<typename T>
		struct unit_value_t_traits <T, typename void_t<
			typename T::unit_type,
			typename T::ratio>::type>
		{
			typedef typename T::unit_type unit_type;
			typedef typename T::ratio ratio;
		};
		/** @endcond */	// END DOXYGEN IGNORE
	}

	//------------------------------------------------------------------------------
	//	COMPILE-TIME UNIT VALUES AND ARITHMETIC
	//------------------------------------------------------------------------------

	/**
	 * @ingroup		UnitContainers
	 * @brief		Stores a rational unit value as a compile-time constant
	 * @details		unit_value_t is useful for performing compile-time arithmetic on known
	 *				unit quantities.
	 * @tparam		Units	units represented by the `unit_value_t`
	 * @tparam		Num		numerator of the represented value.
	 * @tparam		Denom	denominator of the represented value.
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		This is intentionally identical in concept to a `std::ratio`.
	 *
	 */
	template<typename Units, std::uintmax_t Num, std::uintmax_t Denom = 1>
	struct unit_value_t : units::detail::_unit_value_t<Units>
	{
		typedef Units unit_type;
		typedef std::ratio<Num, Denom> ratio;

		static_assert(traits::is_unit<Units>::value, "Template parameter `Units` must be a unit type.");
		static constexpr const unit_t<Units> value() { return unit_t<Units>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den); }
	};

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is a unit_value_t representing the given unit type.
		 * @details		e.g. `is_unit_value_t<meters, myType>::value` would test that `myType` is a
		 *				`unit_value_t<meters>`.
		 * @tparam		Units	units that the `unit_value_t` is supposed to have.
		 * @tparam		T		type to test.
		 */
		template<typename T, typename Units = typename traits::unit_value_t_traits<T>::unit_type>
		struct is_unit_value_t : std::integral_constant<bool,
			std::is_base_of<units::detail::_unit_value_t<Units>, T>::value>
		{};

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether type T is a unit_value_t with a unit type in the given category.
		 * @details		e.g. `is_unit_value_t_category<units::category::length, unit_value_t<feet>>::value` would be true
		 */
		template<typename Category, typename T>
		struct is_unit_value_t_category : std::integral_constant<bool,
			std::is_same<units::traits::base_unit_of<typename traits::unit_value_t_traits<T>::unit_type>, Category>::value>
		{
			static_assert(is_base_unit<Category>::value, "Template parameter `Category` must be a `base_unit` type.");
		};
	}

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		// base class for common arithmetic
		template<class U1, class U2>
		struct unit_value_arithmetic
		{
			static_assert(traits::is_unit_value_t<U1>::value, "Template parameter `U1` must be a `unit_value_t` type.");
			static_assert(traits::is_unit_value_t<U2>::value, "Template parameter `U2` must be a `unit_value_t` type.");

			using _UNIT1 = typename traits::unit_value_t_traits<U1>::unit_type;
			using _UNIT2 = typename traits::unit_value_t_traits<U2>::unit_type;
			using _CONV1 = typename units::traits::unit_traits<_UNIT1>::conversion_ratio;
			using _CONV2 = typename units::traits::unit_traits<_UNIT2>::conversion_ratio;
			using _RATIO1 = typename traits::unit_value_t_traits<U1>::ratio;
			using _RATIO2 = typename traits::unit_value_t_traits<U2>::ratio;
			using _RATIO2CONV = typename std::ratio_divide<std::ratio_multiply<_RATIO2, _CONV2>, _CONV1>;
			using _PI_EXP = std::ratio_subtract<typename units::traits::unit_traits<_UNIT2>::pi_exponent_ratio, typename units::traits::unit_traits<_UNIT1>::pi_exponent_ratio>;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		adds two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @tparam		U2	right-hand `unit_value_t`
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `std::ratio_add`
	 */
	template<class U1, class U2>
	struct unit_value_add : units::detail::unit_value_arithmetic<U1, U2>, units::detail::_unit_value_t<typename traits::unit_value_t_traits<U1>::unit_type>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U2>;
		typedef typename Base::_UNIT1 unit_type;
		using ratio = std::ratio_add<typename Base::_RATIO1, typename Base::_RATIO2CONV>;

		static_assert(traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, "Unit types are not compatible.");
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of sum
		 * @details		Returns the calculated value of the sum of `U1` and `U2`, in the same
		 *				units as `U1`.
		 * @returns		Value of the sum in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)Base::_RATIO1::num / Base::_RATIO1::den) +
			((UNIT_LIB_DEFAULT_TYPE)Base::_RATIO2CONV::num / Base::_RATIO2CONV::den) * std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE
	};

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		subtracts two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @tparam		U2	right-hand `unit_value_t`
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `std::ratio_subtract`
	 */
	template<class U1, class U2>
	struct unit_value_subtract : units::detail::unit_value_arithmetic<U1, U2>, units::detail::_unit_value_t<typename traits::unit_value_t_traits<U1>::unit_type>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U2>;

		typedef typename Base::_UNIT1 unit_type;
		using ratio = std::ratio_subtract<typename Base::_RATIO1, typename Base::_RATIO2CONV>;

		static_assert(traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, "Unit types are not compatible.");
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of difference
		 * @details		Returns the calculated value of the difference of `U1` and `U2`, in the same
		 *				units as `U1`.
		 * @returns		Value of the difference in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)Base::_RATIO1::num / Base::_RATIO1::den) - ((UNIT_LIB_DEFAULT_TYPE)Base::_RATIO2CONV::num / Base::_RATIO2CONV::den)
				* std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE	};
	};

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		multiplies two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1 * U2`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @tparam		U2	right-hand `unit_value_t`
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `std::ratio_multiply`
	 */
	template<class U1, class U2>
	struct unit_value_multiply : units::detail::unit_value_arithmetic<U1, U2>,
		units::detail::_unit_value_t<typename std::conditional<traits::is_convertible_unit<typename traits::unit_value_t_traits<U1>::unit_type,
			typename traits::unit_value_t_traits<U2>::unit_type>::value, compound_unit<squared<typename traits::unit_value_t_traits<U1>::unit_type>>,
			compound_unit<typename traits::unit_value_t_traits<U1>::unit_type, typename traits::unit_value_t_traits<U2>::unit_type>>::type>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U2>;

		using unit_type = std::conditional_t<traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, compound_unit<squared<typename Base::_UNIT1>>, compound_unit<typename Base::_UNIT1, typename Base::_UNIT2>>;
		using ratio = std::conditional_t<traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, std::ratio_multiply<typename Base::_RATIO1, typename Base::_RATIO2CONV>, std::ratio_multiply<typename Base::_RATIO1, typename Base::_RATIO2>>;
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of product
		 * @details		Returns the calculated value of the product of `U1` and `U2`, in units
		 *				of `U1 x U2`.
		 * @returns		Value of the product in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den) * std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE
	};

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		divides two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @tparam		U2	right-hand `unit_value_t`
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `std::ratio_divide`
	 */
	template<class U1, class U2>
	struct unit_value_divide : units::detail::unit_value_arithmetic<U1, U2>,
		units::detail::_unit_value_t<typename std::conditional<traits::is_convertible_unit<typename traits::unit_value_t_traits<U1>::unit_type,
		typename traits::unit_value_t_traits<U2>::unit_type>::value, dimensionless::scalar, compound_unit<typename traits::unit_value_t_traits<U1>::unit_type,
		inverse<typename traits::unit_value_t_traits<U2>::unit_type>>>::type>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U2>;

		using unit_type = std::conditional_t<traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, dimensionless::scalar, compound_unit<typename Base::_UNIT1, inverse<typename Base::_UNIT2>>>;
		using ratio = std::conditional_t<traits::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, std::ratio_divide<typename Base::_RATIO1, typename Base::_RATIO2CONV>, std::ratio_divide<typename Base::_RATIO1, typename Base::_RATIO2>>;
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of quotient
		 * @details		Returns the calculated value of the quotient of `U1` and `U2`, in units
		 *				of `U1 x U2`.
		 * @returns		Value of the quotient in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den) * std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE
	};

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		raises unit_value_to a power at compile-time
	 * @details		The resulting unit will the `unit_type` of `U1` squared
	 * @tparam		U1	`unit_value_t` to take the exponentiation of.
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `units::math::pow`
	 */
	template<class U1, int power>
	struct unit_value_power : units::detail::unit_value_arithmetic<U1, U1>, units::detail::_unit_value_t<typename units::detail::power_of_unit<power, typename traits::unit_value_t_traits<U1>::unit_type>::type>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U1>;

		using unit_type = typename units::detail::power_of_unit<power, typename Base::_UNIT1>::type;
		using ratio = typename units::detail::power_of_ratio<power, typename Base::_RATIO1>::type;
		using pi_exponent = std::ratio_multiply<std::ratio<power>, typename Base::_UNIT1::pi_exponent_ratio>;
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of exponentiation
		 * @details		Returns the calculated value of the exponentiation of `U1`, in units
		 *				of `U1^power`.
		 * @returns		Value of the exponentiation in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den) * std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)pi_exponent::num / pi_exponent::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE	};
	};

	/**
	 * @ingroup		CompileTimeUnitManipulators
	 * @brief		calculates square root of unit_value_t at compile-time
	 * @details		The resulting unit will the square root `unit_type` of `U1`
	 * @tparam		U1	`unit_value_t` to take the square root of.
	 * @sa			unit_value_t_traits to access information about the properties of the class,
	 *				such as it's unit type and rational value.
	 * @note		very similar in concept to `units::ratio_sqrt`
	 */
	template<class U1, std::intmax_t Eps = 10000000000>
	struct unit_value_sqrt : units::detail::unit_value_arithmetic<U1, U1>, units::detail::_unit_value_t<square_root<typename traits::unit_value_t_traits<U1>::unit_type, Eps>>
	{
		/** @cond */	// DOXYGEN IGNORE
		using Base = units::detail::unit_value_arithmetic<U1, U1>;

		using unit_type = square_root<typename Base::_UNIT1, Eps>;
		using ratio = ratio_sqrt<typename Base::_RATIO1, Eps>;
		using pi_exponent = ratio_sqrt<typename Base::_UNIT1::pi_exponent_ratio, Eps>;
		/** @endcond */	// END DOXYGEN IGNORE

		/**
		 * @brief		Value of square root
		 * @details		Returns the calculated value of the square root of `U1`, in units
		 *				of `U1^1/2`.
		 * @returns		Value of the square root in the appropriate units.
		 */
		static constexpr const unit_t<unit_type> value() noexcept
		{
			using UsePi = std::integral_constant<bool, Base::_PI_EXP::num != 0>;
			return value(UsePi());
		}

		/** @cond */	// DOXYGEN IGNORE
		// value if PI isn't involved
		static constexpr const unit_t<unit_type> value(std::false_type) noexcept
		{
			return unit_t<unit_type>((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static constexpr const unit_t<unit_type> value(std::true_type) noexcept
		{
			return unit_t<unit_type>(((UNIT_LIB_DEFAULT_TYPE)ratio::num / ratio::den) * std::pow(units::constants::detail::PI_VAL, ((UNIT_LIB_DEFAULT_TYPE)pi_exponent::num / pi_exponent::den)));
		}
		/** @endcond */	// END DOXYGEN IGNORE
	};

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	/**
	 * @brief		namespace for unit-enabled versions of the `<cmath>` library
	 * @details		Includes trigonometric functions, exponential/log functions, rounding functions, etc.
	 * @sa			See `unit_t` for more information on unit type containers.
	 */
	namespace math
	{

		//----------------------------------
		//	MIN/MAX FUNCTIONS
		//----------------------------------
		// XXX: min/max are defined here instead of math.h to avoid a conflict with
		// the "_min" user-defined literal in time.h.

		template<class UnitTypeLhs, class UnitTypeRhs>
		UnitTypeLhs (min)(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
		{
			static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value, "Unit types are not compatible.");
			UnitTypeLhs r(rhs);
			return (lhs < r ? lhs : r);
		}

		template<class UnitTypeLhs, class UnitTypeRhs>
		UnitTypeLhs (max)(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
		{
			static_assert(traits::is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value, "Unit types are not compatible.");
			UnitTypeLhs r(rhs);
			return (lhs > r ? lhs : r);
		}
	}
}

#ifdef _MSC_VER
#	if _MSC_VER <= 1800
#		pragma warning(pop)
#		undef constexpr
#		pragma pop_macro("constexpr")
#		undef noexcept
#		pragma pop_macro("noexcept")
#		undef _ALLOW_KEYWORD_MACROS
#	endif // _MSC_VER < 1800
#	pragma pop_macro("pascal")
#endif // _MSC_VER

#if defined(UNIT_HAS_LITERAL_SUPPORT)
namespace units::literals {}
using namespace units::literals;
#endif  // UNIT_HAS_LITERAL_SUPPORT
