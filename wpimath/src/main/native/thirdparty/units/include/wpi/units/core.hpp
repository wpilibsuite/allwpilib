//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++23 unit conversion library with no dependencies
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
#include <version>

// We don't want to use iostream, but do want format
#ifndef UNIT_LIB_FORCE_IOSTREAM
#define UNIT_LIB_DISABLE_IOSTREAM
#endif
#define UNIT_LIB_ENABLE_FORMAT

// ---------------------------------------------------------------------------------------------------------------------
//	TEXT-FEATURE CONFIGURATION (opt-out; full capability is the default)
// ---------------------------------------------------------------------------------------------------------------------
// Out of the box every text feature is ON: stream inserters, to_string, and std::format (where <format> is
// available). Consumers opt OUT with the DISABLE_ macros. The ENABLE_* macros are derived internal switches
// — do not define them directly, with the single exception of UNIT_LIB_ENABLE_FORMAT (the documented opt-in
// that restores std::format under UNIT_LIB_DISABLE_IOSTREAM).
//
//   UNIT_LIB_DISABLE_IOSTREAM  Drops the stream inserters. For BACKWARD COMPATIBILITY this also drops
//                              to_string, <string>, and std::format: a legacy iostream-disabled build has
//                              always been the lean, string-free build, and stays byte-for-byte that. To
//                              keep std::format while dropping streams, ALSO define UNIT_LIB_ENABLE_FORMAT.
//   UNIT_LIB_DISABLE_FORMAT    Drops only std::format support; iostream and to_string remain.
//   UNIT_LIB_DISABLE_STRING    The leanest build: forbids <string>, and therefore implies both of the above.
//
// std::format support additionally requires the standard library to provide <format> (__cpp_lib_format).

#if defined(UNIT_LIB_DISABLE_STRING)
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
#define UNIT_LIB_DISABLE_IOSTREAM
#endif
#if !defined(UNIT_LIB_DISABLE_FORMAT)
#define UNIT_LIB_DISABLE_FORMAT
#endif
#endif

// std::format: on by default when <format> exists; off if explicitly disabled or if string is disabled; off
// alongside iostream UNLESS the caller opts back in with UNIT_LIB_ENABLE_FORMAT.
#if !defined(UNIT_LIB_DISABLE_FORMAT) && !defined(UNIT_LIB_DISABLE_STRING) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L &&                       \
    (!defined(UNIT_LIB_DISABLE_IOSTREAM) || defined(UNIT_LIB_ENABLE_FORMAT))
#if !defined(UNIT_LIB_ENABLE_FORMAT)
#define UNIT_LIB_ENABLE_FORMAT
#endif
#else
// If format cannot be enabled, ensure a stray UNIT_LIB_ENABLE_FORMAT does not leak through.
#undef UNIT_LIB_ENABLE_FORMAT
#endif

// The value stringifier + unit-label builders (and <string>) exist whenever any text feature — the stream
// inserters or std::format — is compiled in.
#if !defined(UNIT_LIB_DISABLE_STRING) && (!defined(UNIT_LIB_DISABLE_IOSTREAM) || defined(UNIT_LIB_ENABLE_FORMAT))
#define UNIT_LIB_ENABLE_STRING
#endif

#if defined(UNIT_LIB_ENABLE_STRING)
#include <string>
#endif

#if defined(UNIT_LIB_ENABLE_FORMAT)
#include <format>
#include <string_view>
#endif

#if __has_include(<wpi/telemetry/TelemetryTable.hpp>) && !defined(UNIT_LIB_DISABLE_TELEMETRY)
#include <string_view>
#include <wpi/util/ct_string.hpp>
#include <wpi/telemetry/TelemetryTable.hpp>
#endif
#include <gcem.hpp>

#if defined(UNIT_LIB_ENABLE_STRING)
#include <clocale>

//------------------------------
//	VALUE STRINGIFIER
//------------------------------

namespace wpi::units::detail
{
	template<typename T>
		requires std::is_arithmetic_v<T>    // numbers only: a named unit's associated namespace is wpi::units::detail, so an
											// unconstrained overload here would be an ADL candidate for to_string(someUnit)
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

#endif // UNIT_LIB_ENABLE_STRING

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
#include <sstream>
#endif

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
	} // namespace traits
} // namespace wpi::units

//------------------------------
//	MACROS
//------------------------------

/**
 * @def			UNIT_ADD_TELEMETRY(namespaceName, namePlural, abbreviation)
 * @brief		Macro for generating the boiler-plate code needed for telemetry for a new unit.
 * @details		The macro generates the code to insert units into a wpi::telemetry::TelemetryTable
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		namePlural plural version of the unit name, e.g. 'meters'
 * @param		abbrev - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_LIB_DISABLE_TELEMETRY is defined, the macro does not generate any code
 */
#if __has_include(<wpi/telemetry/TelemetryTable.hpp>) && !defined(UNIT_LIB_DISABLE_TELEMETRY)
  #define UNIT_ADD_TELEMETRY(namespaceName, namePlural, abbrev)\
  inline namespace namespaceName\
  {\
	  inline void LogValueTo(wpi::telemetry::TelemetryTable& table, std::string_view name, const namePlural <>& value)\
	  {\
			table.SetProperty(name, "unit", "\"" #abbrev "\"");\
			table.Log(name, value.value());\
	  }\
  }
#else
	 #define UNIT_ADD_TELEMETRY(namespaceName, namePlural, abbrev)
#endif

/**
 * @def			UNIT_ADD_STRONG_CONVERSION_FACTOR(namespaceName, namePlural, __VA_ARGS__)
 * @brief		Helper macro for generating the boilerplate code generating the tags of a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as aliases for the
 *				unit tag.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param       ... - the conversion factor definition for the unit type. Taken as variadic
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
	namespace detail                                                                                                                                                                                   \
	{                                                                                                                                                                                                  \
		/** ADL registration of the strong type for this conversion factor (see detail::strong_name, #357). */         \
		/** Exact-`__VA_ARGS__*` parameter — a strictly better match than the variadic fallback — so `strong_t` */     \
		/** resolves to the named type once THIS header is included, order-independently. Declared, never defined. */   \
		::wpi::units::namespaceName::namePlural##_ strong_name(__VA_ARGS__*);                                                                                                                                \
	}

/**
 * @def			UNIT_ADD_UNIT_DEFINITION(namespaceName,namePlural)
 * @brief		Macro for generating the boilerplate code for the unit type definition.
 * @details		The macro generates the definition of the unit container types, e.g. `meter`
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param       ... - the conversion factor definition for the unit type. Taken as variadic
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
 * @param       ... - the conversion factor definition for the unit type, used for the definition of
 *              the unit (e.g. `conversion_factor<std::ratio<1>, wpi::units::dimension::length>`). Taken as
 *              variadic arguments because they contain commas in the macro definition. The complete
 *              __VA_ARGS__ represents the full conversion factor type. e.g. `meters<>`.
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_SCALED_UNIT_DEFINITION(unitName, scale, /*conversionFactor*/...)                                                                                                                      \
	/** A named unit is a CLASS deriving from its `unit<...>` (not an alias) so a diagnostic prints the friendly */    \
	/** name (`meters<double>`) instead of `unit<strong, Underlying, scale>` cruft, keeping the `unitName<>` spelling. */\
	template<class Underlying = UNIT_LIB_DEFAULT_TYPE>                                                                                                                                                 \
	struct unitName : ::wpi::units::unit<traits::strong_t<__VA_ARGS__>, Underlying, scale>                                                                                                                  \
	{                                                                                                                                                                                                  \
		using base = ::wpi::units::unit<traits::strong_t<__VA_ARGS__>, Underlying, scale>;                                                                                                                  \
		using base::base;                                                                                                                                                                              \
		/* Keep the named class TRIVIAL (a load-bearing property of the unit type — memcpy-able, zero overhead): */    \
		/* explicitly default the special members. Declaring the converting ctor below would otherwise suppress the */ \
		/* trivial default ctor, and inheriting ctors leaves the special members implicit; defaulting them restores */  \
		/* std::is_trivial. */                                                                                          \
		unitName() = default;                                                                                                                                                                          \
		unitName(const unitName&) = default;                                                                                                                                                           \
		unitName(unitName&&) = default;                                                                                                                                                                 \
		unitName& operator=(const unitName&) = default;                                                                                                                                                 \
		unitName& operator=(unitName&&) = default;                                                                                                                                                      \
		constexpr unitName(const base& other) noexcept : base(other) {}                                                                                                                               \
		/* Explicit consteval forwarding of the base's compile-time narrowing converting constructor. The base is */    \
		/* constructed directly (`base(rhs)`), so the named class does not rely on `using base::base` to SYNTHESIZE an */\
		/* inheriting-constructor wrapper for this consteval ctor. GCC 13's constant evaluator mis-handles that */       \
		/* synthesized inheriting wrapper — it treats the base subobject as uninitialized (accessing uninitialized */    \
		/* member, this is not a constant expression) — while an explicit derived ctor evaluates correctly. The */       \
		/* base's own requires-clause gates viability; the derived constraint keeps this a candidate only for the */     \
		/* narrowing the base ctors accept — a floating-point source, or a finer integral source that is an exact */    \
		/* whole number of this integral unit. `base(rhs)` selects whichever base consteval ctor matches the source. */ \
		template<::wpi::units::ConversionFactorType Cf, ::wpi::units::ArithmeticType Ty, ::wpi::units::NumericalScaleType<Ty> Ns>       \
			requires(::wpi::units::traits::is_same_dimension_unit_v<::wpi::units::unit<Cf, Ty, Ns>, base> &&                       \
					 !::wpi::units::detail::is_losslessly_convertible_unit<::wpi::units::unit<Cf, Ty, Ns>, base> &&                 \
					 (::std::is_floating_point_v<Ty> || ::std::is_integral_v<Ty>) && ::std::is_integral_v<Underlying>)    \
		consteval unitName(const ::wpi::units::unit<Cf, Ty, Ns>& rhs) : base(rhs) {}                                          \
		/* Forward a scalar assignment to the base's operator= so the dimensionless '= 0.30' path (which the derived */ \
		/* class would otherwise route through the raw-value converting ctor, off by the CF ratio) is used. Templated */\
		/* + constrained to arithmetic so it never competes with unit-to-unit assignment (that stays the base's job). */\
		template<class Rhs>                                                                                                                                                                            \
			requires ::std::is_arithmetic_v<Rhs>                                                                                                                                                       \
		constexpr unitName& operator=(const Rhs& rhs) noexcept                                                                                                                                          \
		{                                                                                                                                                                                              \
			base::operator=(rhs);                                                                                                                                                                      \
			return *this;                                                                                                                                                                              \
		}                                                                                                                                                                                              \
		/** Re-create this named unit with a different underlying type, so traits (replace_underlying, common_type) */ \
		/** that swap the storage type PRESERVE the friendly name instead of decaying to the plain `unit<...>`. */     \
		template<class NewUnderlying>                                                                                                                                                                  \
		using rebind = unitName<NewUnderlying>;                                                                                                                                                        \
	};                                                                                                                                                                                                 \
	/** Deduction guide so the BARE name works (no <>): `unitName x(5.0)` / `constexpr unitName c(3e8)` deduces */    \
	/** `unitName<double>`. As an ALIAS template (pre-refactor) a bare name resolved via the default arg; as a CLASS */\
	/** template CTAD is required, and some compilers (e.g. GCC 13) will not deduce it from an arithmetic argument */  \
	/** without this guide. Constrained to arithmetic so it never competes with the base/converting constructors. */  \
	template<class Arg>                                                                                                                                                                                \
		requires ::std::is_arithmetic_v<Arg>                                                                                                                                                           \
	unitName(Arg) -> unitName<Arg>;                                                                                                                                                                    \
	/** Nullary guide so bare default-construction `unitName{}` / `unitName()` deduces `unitName<default>` — again a */\
	/** CLASS template needs this where the old alias resolved via its default arg; GCC 13 requires it explicitly. */ \
	unitName() -> unitName<UNIT_LIB_DEFAULT_TYPE>;                                                                                                                                                      \
	/** And from another same-dimension UNIT (bare): `unitName x(otherUnit)` deduces the underlying the converting */  \
	/** constructor would produce — the SOURCE underlying when losslessly convertible to this named unit, else its */  \
	/** floating-point promotion (so e.g. `radians r(degrees{1})` deduces radians<double>, since degrees->radians is */\
	/** not integer-lossless). GCC 13 will not deduce this implicitly (GCC 15 will), so the guide is required; */      \
	/** constrained to unit types so it never competes with the arithmetic/nullary guides. */                          \
	template<class OtherUnit>                                                                                                                                                                          \
		requires(::wpi::units::traits::is_unit<OtherUnit>::value &&                                                                                                                                         \
				 ::wpi::units::traits::is_same_dimension_unit_v<OtherUnit,                                                                                                                                    \
					 ::wpi::units::unit<traits::strong_t<__VA_ARGS__>, typename ::wpi::units::traits::unit_traits<OtherUnit>::underlying_type, scale>>)                                                            \
	unitName(const OtherUnit&) -> unitName<::wpi::units::detail::deduced_named_underlying_t<OtherUnit, traits::strong_t<__VA_ARGS__>, scale>>;\
	/** And from a std::chrono::duration (bare, for time units): `nanoseconds n(chrono::nanoseconds(10))` deduces */  \
	/** the default underlying; the chrono converting constructor does the Rep/Period conversion. GCC 13 needs the */ \
	/** explicit guide. */                                                                                            \
	template<class Rep, class Period>                                                                                                                                                                  \
	unitName(const ::std::chrono::duration<Rep, Period>&) -> unitName<UNIT_LIB_DEFAULT_TYPE>;                          \
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
 * @def			UNIT_REGISTER_NAMED_CLASS(namespaceName, namePlural)
 * @brief		Register the CF-struct -> NAMED-class ADL map so an arithmetic RESULT is reported as the friendly type.
 * @details		A result unit<strong, U, scale> is rewrapped into `namePlural<U>` via detail::named_class_of (results
 *				stay as friendly as inputs). Registered at `units` scope AFTER the class is defined (so no forward
 *				reference) and only for LINEAR named units — decibel-scale units are excluded because several dB names
 *				share one linear conversion_factor, which would make the reverse map ambiguous. Declared, never defined.
 */
#define UNIT_REGISTER_NAMED_CLASS(namespaceName, namePlural)                                                                                                                                            \
	namespace detail                                                                                                                                                                                   \
	{                                                                                                                                                                                                  \
		/* Keyed on BOTH the conversion_factor AND the numerical scale: the linear and decibel forms of a unit share */ \
		/* one conversion_factor (watts_ for both watts and dBW) and differ only by scale, so scale must disambiguate */\
		/* the reverse map (else watts vs dBW collide). Declared, never defined (decltype-only). */                     \
		::wpi::units::namespaceName::namePlural<UNIT_LIB_DEFAULT_TYPE> named_class_of(                                                                                                                       \
			typename ::wpi::units::namespaceName::namePlural<>::conversion_factor*,                                                                                                                          \
			typename ::wpi::units::namespaceName::namePlural<>::numerical_scale_type*);                                                                                                                      \
	}

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
		/* A literal is always floating-point. It uses the library default type when that is a floating-point */    \
		/* type, and its floating-point promotion otherwise, so a literal is never integer-backed even if the */    \
		/* default representation is integral. */                                                                    \
		constexpr namespaceName::namePlural<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>> operator""_##abbreviation(long double d) noexcept \
		{                                                                                                                                                                                              \
			return namespaceName::namePlural<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>>(static_cast<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>>(d));   \
		}                                                                                                                                                                                              \
		/* An integer literal (5_m) yields the same floating-point type as 5.0_m. A literal is a value a user */    \
		/* writes inline; deducing an integer representation from it silently opts into integer arithmetic */        \
		/* (5_m / 2_m == 0), which is rarely intended, and diverges from the unit constant form (5 * m is always */  \
		/* floating-point). An integer-backed quantity is still available explicitly (namePlural<int>(5)) or by */    \
		/* CTAD from an integer argument (namePlural(5)). */                                                          \
		constexpr namespaceName::namePlural<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>> operator""_##abbreviation(unsigned long long d) noexcept \
		{                                                                                                                                                                                              \
			return namespaceName::namePlural<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>>(static_cast<::wpi::units::detail::floating_point_promotion_t<UNIT_LIB_DEFAULT_TYPE>>(d));   \
		}                                                                                                                                                                                              \
	}
#endif

/**
 * @def			UNIT_ADD_DECIBEL_LITERALS(namespaceName, namePlural, abbreviation)
 * @brief		Like UNIT_ADD_LITERALS but emits only the floating-point literal.
 * @details		A decibel-scale unit requires a floating-point underlying type, so no integer literal
 *				(which would form a `<int>` unit) is generated.
 */
#ifdef UNIT_NO_LITERAL_SUPPORT
#define UNIT_ADD_DECIBEL_LITERALS(namespaceName, namePlural, abbreviation)
#else
#define UNIT_ADD_DECIBEL_LITERALS(namespaceName, namePlural, abbreviation)                                                                                                                             \
	namespace literals                                                                                                                                                                                 \
	{                                                                                                                                                                                                  \
		constexpr namespaceName::namePlural<double> operator""_##abbreviation(long double d) noexcept                                                                                                  \
		{                                                                                                                                                                                              \
			return namespaceName::namePlural<double>(static_cast<double>(d));                                                                                                                          \
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
 * @param       ... - the conversion factor definition for the unit type. Taken as variadic
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
	UNIT_REGISTER_NAMED_CLASS(namespaceName, namePlural)                                                                                                                                               \
	UNIT_ADD_TELEMETRY(namespaceName, namePlural, abbreviation)                                                                                                                                        \
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
	UNIT_REGISTER_NAMED_CLASS(namespaceName, abbreviation)                                                                                                                                             \
	UNIT_ADD_DECIBEL_LITERALS(namespaceName, abbreviation, abbreviation)

/**
 * @def			UNIT_ADD_DIMENSION_TRAIT(unitdimension, ConceptName)
 * @brief		Macro to create the `is_dimension_unit` type trait and the `ConceptName` concept.
 * @details		The `is_ ## unitdimension ## _unit` trait (in namespace `wpi::units::traits`) allows users to test
 *				whether a given type matches an intended dimension, and the `ConceptName` concept (in namespace
 *				`units`) lets a function constrain a parameter on a physical quantity by dimension
 *				(`void f(Velocity auto)`) rather than a concrete named type. Being dimension-keyed, the concept
 *				classifies a computed result consistently regardless of which dimension headers a translation
 *				unit included. This macro comprises all the boilerplate code necessary to do so. The C
 *				preprocessor cannot uppercase a token, so the PascalCase concept name is supplied as a separate
 *				argument rather than derived from `unitdimension`.
 * @param		unitdimension	The name of the dimension of unit, e.g. length or mass.
 * @param		ConceptName		The PascalCase name of the emitted concept, e.g. Length or Mass.
 */

#define UNIT_ADD_DIMENSION_TRAIT(unitdimension, ConceptName)                                                                                                                                           \
	/** @ingroup	TypeTraits*/                                                                                                                                                                   \
	/** @brief		`UnaryTypeTrait` for querying whether `T` represents a unit of unitdimension*/                                                                                         \
	/** @details	The base characteristic is a specialization of the template `std::bool_constant`.*/                                                                                            \
	/**				Use `is_ ## unitdimension ## _unit_v<T>` to test the unit represents a unitdimension quantity.*/                                                               \
	/** @tparam		T	type to test*/                                                                                                                                                 \
	namespace traits                                                                                                                                                                               \
	{                                                                                                                                                                                              \
		template<typename T>                                                                                                                                                                   \
		struct is_##unitdimension##_unit : ::wpi::units::detail::has_dimension_of<std::decay_t<T>, wpi::units::dimension::unitdimension>                                                                 \
		{                                                                                                                                                                                      \
		};                                                                                                                                                                                     \
		template<typename T>                                                                                                                                                                   \
		inline constexpr bool is_##unitdimension##_unit_v = is_##unitdimension##_unit<T>::value;                                                                                               \
	}                                                                                                                                                                                              \
	/** @ingroup	Concepts*/                                                                                                                                                                     \
	/** @brief		Concept satisfied by any unit whose SI dimension is unitdimension; being dimension-keyed it*/                                                                          \
	/**				classifies a computed result consistently regardless of which dimension headers a translation*/                                                                \
	/**				unit included.*/                                                                                                                                               \
	/** @tparam		T	type to test*/                                                                                                                                                 \
	template<typename T>                                                                                                                                                                           \
	concept ConceptName = ::wpi::units::traits::is_##unitdimension##_unit_v<std::decay_t<T>>;

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
 * @param       ... - the conversion factor definition for the unit type. Taken as variadic
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
 * @param       ... - the conversion factor definition for the unit type. Taken as variadic
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
	 * @defgroup	Concepts Concepts
	 * @brief		Defines the concepts used to constrain unit, conversion-factor, and arithmetic template parameters.
	 */

	/**
	 * @defgroup	ConversionFactor Conversion Factors
	 * @brief		Defines `conversion_factor` and the strong-typed conversion factors that tag each unit.
	 */

	/**
	 * @defgroup	Constructors Constructors
	 * @brief		Defines the constructors of the `unit` container type.
	 */

	/**
	 * @defgroup	Prefixes Prefixes
	 * @ingroup		UnitManipulators
	 * @brief		Defines the metric and binary prefix manipulators (e.g. `milli`, `kilo`, `kibi`).
	 */

	/**
	 * @defgroup	Decimal Decimal Prefixes
	 * @ingroup		Prefixes
	 * @brief		Defines the decimal (metric) prefix manipulators from atto to exa.
	 */

	/**
	 * @defgroup	Binary Binary Prefixes
	 * @ingroup		Prefixes
	 * @brief		Defines the binary prefix manipulators from kibi to exbi.
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

		/**
		 * @brief		Whether `T` is a complete type, decided by SFINAE on `sizeof(T)` without instantiating any
		 *				other trait.
		 * @details		`std::is_base_of<Base, T>` is ill-formed when `T` is an incomplete, non-same class type (a
		 *				conforming library — libc++ — rejects it). `is_unit` must stay usable as a SFINAE probe on
		 *				arbitrary foreign types, and one such type is a standard-library class caught mid-definition:
		 *				libc++'s `<chrono>` evaluates every `std::common_type` partial specialization while
		 *				`std::chrono::duration` is still incomplete, which would instantiate `is_base_of<_unit,
		 *				duration>` on the incomplete `duration`. Gating the base-of test on completeness avoids that.
		 */
		template<class T, class = void>
		struct is_complete : std::false_type
		{
		};

		template<class T>
		struct is_complete<T, std::void_t<decltype(sizeof(T))>> : std::true_type
		{
		};

		/**
		 * @brief		`is_unit` implementation: an incomplete or non-class type is never a unit, decided WITHOUT
		 *				instantiating `is_base_of` (which is ill-formed on an incomplete non-same class type). Only a
		 *				complete type reaches the `is_base_of` test in the specialization below.
		 */
		template<class T, bool = is_complete<T>::value>
		struct is_unit_impl : std::false_type
		{
		};

		template<class T>
		struct is_unit_impl<T, true> : std::is_base_of<_unit, T>::type
		{
		};

		/**
		 * @brief		ADL customization point that maps a `conversion_factor` to its friendly strong type.
		 * @details		This is the anchor `traits::strong` resolves through. A dimension header registers a named
		 *				strong type by declaring a better-matching overload of `strong_name` (see the
		 *				`UNIT_ADD_STRONG_CONVERSION_FACTOR` macro), discoverable by ADL because a `conversion_factor`'s
		 *				associated namespace is `units`. This fallback is the WORST match (variadic `...`), so it is
		 *				chosen only when no dimension header has registered a named type — in which case the strong
		 *				type is the `conversion_factor` itself. Only ever used unevaluated (in `decltype`); never
		 *				defined. Being an overload set rather than an explicit specialization, a later-included header
		 *				merely contributes a stronger candidate — it can never be "declared after instantiation" (#357).
		 *
		 *				The fallback deduces the `conversion_factor` from its pointer argument, followed by a trailing
		 *				ellipsis so that any exact-`CF*` registration overload (a non-template, non-variadic parameter)
		 *				is a strictly better match and wins whenever its dimension header is visible.
		 */
		template<class ConversionFactor>
		ConversionFactor strong_name(ConversionFactor*, ...);
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
		struct is_unit : wpi::units::detail::is_unit_impl<T>::type
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
		 *
		 *					The strong type of a `conversion_factor` is registered by its dimension header (e.g.
		 *					`units/frequency.h` registers `hertz` for `1/time`). Resolution is an ADL customization
		 *					point (`wpi::units::detail::strong_name`), NOT an explicit specialization of `strong`: a named
		 *					type is discovered by overload resolution over the `conversion_factor`'s associated
		 *					namespace at the point `strong_t<T>` is instantiated. This deliberately avoids the
		 *					"explicit specialization after implicit instantiation" ordering trap (#357) — forming an
		 *					expression that reduces to a not-yet-included dimension no longer bakes in a decision a
		 *					later header would contradict; the later header simply contributes a better overload.
		 */
		template<ConversionFactorType T>
			requires std::is_same_v<T, std::remove_cv_t<T>>
		struct strong
		{
			// UNQUALIFIED call so ADL on T* is performed: T is a conversion_factor whose associated namespaces are
			// `units` and (via its base detail::_conversion_factor) `wpi::units::detail`, so every dimension header's
			// strong_name registration in wpi::units::detail is found, along with the identity fallback. A qualified call
			// (::wpi::units::detail::strong_name) would SUPPRESS ADL and see only the fallback — the whole point is ADL.
			using type = decltype(strong_name(static_cast<T*>(nullptr)));
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
		using volume_flow_rate        = dimension_divide<volume, time>;                                                     ///< Represents an SI derived unit of volumetric flow rate
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
		using dynamic_viscosity   = dimension_multiply<pressure, time>;                            ///< Represents an SI derived unit of dynamic (absolute) viscosity
		using kinematic_viscosity = dimension_divide<area, time>;                                  ///< Represents an SI derived unit of kinematic viscosity
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

	/** @cond */ // DOXYGEN IGNORE
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

		// True for dimensionless units whose conversion_ratio is not 1: percent, ppm, ppb, ppt, etc.
		template<class ConversionFactor, class = void>
		struct is_ratio_dimensionless_cf : std::false_type
		{
		};

		template<class ConversionFactor>
		struct is_ratio_dimensionless_cf<ConversionFactor, std::void_t<typename ConversionFactor::dimension_type, typename ConversionFactor::conversion_ratio>>
		  : std::bool_constant<std::is_same_v<typename ConversionFactor::dimension_type, dimension::dimensionless> && !std::ratio_equal_v<typename ConversionFactor::conversion_ratio, std::ratio<1>>>
		{
		};

		template<class ConversionFactor>
		inline constexpr bool is_ratio_dimensionless_cf_v = is_ratio_dimensionless_cf<ConversionFactor>::value;

	} // namespace traits

	template<typename U>
	concept RatioDimensionlessUnitType = wpi::units::DimensionlessUnitType<U> && traits::is_ratio_dimensionless_cf_v<typename U::conversion_factor>;

	template<typename U>
	concept OrdinaryDimensionlessUnitType = DimensionlessUnitType<U> && !RatioDimensionlessUnitType<U>;

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
	 * @ingroup Decimal
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
	/** @} */

	/**
	 * @ingroup Binary
	 * @{
	 */
	template<ConversionFactorType Cf> using kibi	= typename detail::prefix<std::ratio<1024>,					Cf>::type;	///< Represents the type of `class Cf` with the binary 'kibi' prefix appended.	@details E.g. kibi<bytes> represents bytes*2^10	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using mebi	= typename detail::prefix<std::ratio<1048576>,				Cf>::type;	///< Represents the type of `class Cf` with the binary 'mibi' prefix appended.	@details E.g. mebi<bytes> represents bytes*2^20	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using gibi	= typename detail::prefix<std::ratio<1073741824>,			Cf>::type;	///< Represents the type of `class Cf` with the binary 'gibi' prefix appended.	@details E.g. gibi<bytes> represents bytes*2^30	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using tebi	= typename detail::prefix<std::ratio<1099511627776>,		Cf>::type;	///< Represents the type of `class Cf` with the binary 'tebi' prefix appended.	@details E.g. tebi<bytes> represents bytes*2^40	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using pebi	= typename detail::prefix<std::ratio<1125899906842624>,		Cf>::type;	///< Represents the type of `class Cf` with the binary 'pebi' prefix appended.	@details E.g. pebi<bytes> represents bytes*2^50	@tparam Cf unit type to apply the prefix to.
	template<ConversionFactorType Cf> using exbi	= typename detail::prefix<std::ratio<1152921504606846976>,	Cf>::type;	///< Represents the type of `class Cf` with the binary 'exbi' prefix appended.	@details E.g. exbi<bytes> represents bytes*2^60	@tparam Cf unit type to apply the prefix to.
	/** @} */
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

		/**
		 * @brief		`true` when a conversion factor carries a non-zero datum offset — i.e. it is AFFINE, not
		 *				a pure scale (the archetype is temperature: degrees Celsius/Fahrenheit have an offset to
		 *				the Kelvin datum). Absolute affine quantities do not add meaningfully, and their
		 *				difference is a pure delta (the offsets cancel).
		 * @tparam		Cf	the conversion factor to test.
		 */
		template<ConversionFactorType Cf>
		inline constexpr bool is_affine_conversion_factor_v = !std::ratio_equal_v<typename conversion_factor_traits<Cf>::translation_ratio, std::ratio<0>>;
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

		/**
		 * @brief		Cast a floating-point value to an integral type, exactly.
		 * @details		Returns `value` as `To` when it is a whole number in `To`'s range. When it is not — a
		 *				fractional part, or out of range — the `throw` makes this ill-formed in a constant-evaluated
		 *				context, which is the only context the narrowing unit constructor uses it in. `To` is an
		 *				integral type; `From` is floating point.
		 * @tparam		To		the integral target type.
		 * @tparam		From	the floating-point source type.
		 * @param[in]	value	the value to cast.
		 * @return		`value` as `To`.
		 */
		template<class To, class From>
		constexpr To exact_integral_cast(From value)
		{
			const To result = static_cast<To>(value);
			if (static_cast<From>(result) != value)
				throw "a floating-point unit converts to an integral unit only when its value is a whole number in range";
			return result;
		}
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

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/// The widest integer available for a conversion's intermediate. A mul-then-divide conversion
		/// (`value * num / den`) must not overflow the intermediate `value * num` before the divide recovers a
		/// value that fits the target; a double-width intermediate holds the product. `__int128` is used where the
		/// compiler provides it; otherwise the intermediate falls back to `std::intmax_t` (the widest standard
		/// integer) and a manual 128-bit mul-divide guards the product.
		using widest_signed_int   = std::intmax_t;
		using widest_unsigned_int = std::uintmax_t;
		inline constexpr bool has_builtin_int128 = false;

		/// Compute `value * num / den` for an integral `value` without overflowing the intermediate product, in a
		/// double-width intermediate. On a compiler with `__int128` the whole expression rides in 128 bits; without
		/// it, an unsigned 64x64->high/low long multiplication followed by a 128/64 division keeps the product from
		/// overflowing, with the sign handled separately.
		template<class Rep>
		constexpr Rep widening_mul_div(Rep value, std::intmax_t num, std::intmax_t den) noexcept
		{
			if constexpr (has_builtin_int128)
			{
				return static_cast<Rep>(static_cast<widest_signed_int>(value) * static_cast<widest_signed_int>(num) / static_cast<widest_signed_int>(den));
			}
			else
			{
				// Sign-separated 64x64->128 multiply, then 128/64 divide, all in unsigned 64-bit limbs so no
				// intermediate exceeds the representable range. `num`/`den` are positive (a std::ratio is stored in
				// lowest terms with a positive denominator); only `value` may be negative.
				const bool          negative = (value < 0);
				const std::uint64_t a        = negative ? static_cast<std::uint64_t>(-(value + 1)) + 1u : static_cast<std::uint64_t>(value);
				const std::uint64_t b        = static_cast<std::uint64_t>(num);
				const std::uint64_t d        = static_cast<std::uint64_t>(den);

				// 64x64 -> 128 as two 64-bit limbs (hi, lo).
				const std::uint64_t aLo = a & 0xFFFFFFFFull, aHi = a >> 32;
				const std::uint64_t bLo = b & 0xFFFFFFFFull, bHi = b >> 32;
				const std::uint64_t ll = aLo * bLo;
				const std::uint64_t lh = aLo * bHi;
				const std::uint64_t hl = aHi * bLo;
				const std::uint64_t hh = aHi * bHi;
				const std::uint64_t cross = (ll >> 32) + (lh & 0xFFFFFFFFull) + (hl & 0xFFFFFFFFull);
				std::uint64_t       hi    = hh + (lh >> 32) + (hl >> 32) + (cross >> 32);
				std::uint64_t       lo    = (cross << 32) | (ll & 0xFFFFFFFFull);

				// 128 (hi:lo) / d -> long division of the two limbs by a 64-bit divisor.
				std::uint64_t quotient = 0;
				std::uint64_t rem      = 0;
				for (int bit = 127; bit >= 0; --bit)
				{
					rem                     = (rem << 1) | ((bit >= 64 ? (hi >> (bit - 64)) : (lo >> bit)) & 1u);
					const bool canSubtract = (rem >= d);
					rem -= canSubtract ? d : 0u;
					if (bit < 64)
						quotient |= (static_cast<std::uint64_t>(canSubtract) << bit);
				}
				const auto result = static_cast<Rep>(quotient);
				return negative ? static_cast<Rep>(-result) : result;
			}
		}

		/// Whether `value * num` is an exact multiple of `den` for an integral `value` — i.e. `value * num / den`
		/// loses nothing. The product is carried in the widest available signed integer so the divisibility test is
		/// exact and cannot be defeated by an intermediate overflow (a `value * num` that wrapped could spuriously
		/// look divisible). `num`/`den` are the numerator/denominator of a `std::ratio` in lowest terms, so `den`
		/// is positive; only `value` may be negative. Sign does not affect divisibility, so the magnitude of the
		/// product is tested.
		template<class Rep>
		constexpr bool integral_conversion_is_exact(Rep value, std::intmax_t num, std::intmax_t den) noexcept
		{
			const widest_signed_int product = static_cast<widest_signed_int>(value) * static_cast<widest_signed_int>(num);
			return product % static_cast<widest_signed_int>(den) == 0;
		}

		/// Convert an integral `value` from a finer to a coarser same-dimension unit (`num`/`den` the conversion
		/// ratio), exactly. When `value * num` is a whole multiple of `den` the exact quotient is returned; when it
		/// is not, the `throw` makes this a non-constant expression, so the narrowing unit constructor that calls it
		/// in a constant-evaluated context is ill-formed rather than silently truncating. The exactness test and the
		/// quotient both ride in a double-width intermediate, so neither is defeated by an intermediate overflow. `To`
		/// and `From` are integral.
		/// @tparam		To		the integral target type.
		/// @tparam		From	the integral source type.
		/// @param[in]	value	the source magnitude, in the source unit.
		/// @param[in]	num		the conversion ratio numerator.
		/// @param[in]	den		the conversion ratio denominator.
		/// @return		`value * num / den` as `To`, when exact.
		template<class To, class From>
		constexpr To exact_integral_unit_cast(From value, std::intmax_t num, std::intmax_t den)
		{
			if (!integral_conversion_is_exact(value, num, den))
				throw "an integral unit converts to a coarser integral unit only when the value is an exact whole number of the target unit";
			return static_cast<To>(widening_mul_div(value, num, den));
		}
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

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
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;
			// The pi exponent as a real number. Compute in long double: PiRatio::num/PiRatio::den are
			// intmax_t, so an integer division here would truncate a fractional exponent (e.g. ratio<1,2>
			// -> 0), which both corrupts the value and, for the fractional case, produced a non-constant
			// expression / missing-return compile error.
			constexpr long double PiRatioValue      = static_cast<long double>(PiRatio::num) / static_cast<long double>(PiRatio::den);
			constexpr bool        integerExponent   = (PiRatio::num % PiRatio::den == 0);

			// A whole-number exponent uses the constexpr integer `pow`; a fractional exponent needs
			// `std::pow` (not constant-evaluable), so that sole case degrades to a run-time computation.
			if constexpr (integerExponent && PiRatioValue >= 0)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(pow(detail::PI_VAL, PiRatioValue))));
			}
			else if constexpr (integerExponent)    // PiRatioValue < 0
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) / static_cast<CommonUnderlying>(pow(detail::PI_VAL, -PiRatioValue))));
			}
			else    // fractional exponent (either sign): std::pow handles both directions
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
			{
				// A mul-then-divide conversion. The goal is the MOST accurate representable result:
				//   - Integral intermediate: carry `value * num` in a double-width integer so it cannot overflow
				//     before `/ den` recovers a value that fits the target (no wrong answer, no precision lost).
				//   - Floating-point: `(value * num) / den` is the most accurate order (a single rounding) and is
				//     used whenever `value * num` is representable. Only when that product would overflow to
				//     infinity — a blatantly wrong answer where a finite result exists — fall back to the
				//     divide-first order `value / den * num`, which trades a little rounding for a representable
				//     answer. Normal-magnitude conversions therefore keep the correctly-rounded mul-then-divide.
				if constexpr (std::is_integral_v<CommonUnderlying>)
				{
					return static_cast<To>(detail::widening_mul_div(static_cast<CommonUnderlying>(value), Ratio::num, Ratio::den));
				}
				else
				{
					const CommonUnderlying v   = static_cast<CommonUnderlying>(value);
					const CommonUnderlying num = static_cast<CommonUnderlying>(Ratio::num);
					const CommonUnderlying den = static_cast<CommonUnderlying>(Ratio::den);
					// `value * num` overflows the type when |value| exceeds max / num. Guard on that exact threshold
					// so the lossy divide-first path is taken ONLY when the accurate path would produce infinity.
					const CommonUnderlying limit = (std::numeric_limits<CommonUnderlying>::max)() / num;
					if (v > limit || v < -limit)
						return static_cast<To>((v / den) * num);
					return static_cast<To>((v * num) / den);
				}
			}
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
		/// `true` when a unit type is affine — its conversion factor carries a non-zero datum offset (e.g. a
		/// temperature in degrees Celsius/Fahrenheit). Absolute affine quantities do not add meaningfully;
		/// their difference is a pure delta.
		template<UnitType U>
		inline constexpr bool is_affine_unit_v = is_affine_conversion_factor_v<typename unit_traits<U>::conversion_factor>;

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
		// Forward declaration so unit's name()/abbreviation() members (defined below, in the unit class) can name
		// detail::rewrap_to_named_t; the full definition follows after the unit class is complete (it depends on it).
		template<class U, class = void>
		struct rewrap_to_named;
		template<class U>
		using rewrap_to_named_t = typename rewrap_to_named<U>::type;

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

		/// True when both units have a floating-point underlying type.
		template<class L, class R>
		inline constexpr bool both_floating_v = std::is_floating_point_v<typename traits::unit_traits<L>::underlying_type> &&
												std::is_floating_point_v<typename traits::unit_traits<R>::underlying_type>;

		/// The result unit of a same-dimension `+`/`-` of `L` and `R`. It is the LEFT operand's unit — so the caller
		/// controls the result unit by operand order and reads the value in the unit they wrote — whenever that is
		/// lossless: either the right operand converts into the left as-is, or both operands are floating point (a
		/// floating result in either unit loses nothing). When neither holds — an integral left operand that cannot
		/// hold the right without truncation — it falls back to the symmetric common (finest, lossless) unit, the
		/// same exact reconciliation integer comparisons use.
		template<class L, class R>
		using lhs_result_unit_t = std::conditional_t<is_losslessly_convertible_unit<R, L> || both_floating_v<L, R>, L, std::common_type_t<L, R>>;

		// The underlying type a NAMED unit's from-unit deduction guide should produce when constructed from `Source`:
		// the source's own underlying when losslessly convertible into the target (StrongCf, Scale), else its
		// floating-point promotion (so e.g. radians(degrees{1}) deduces radians<double>). A SFINAE-friendly class
		// template (NOT a var-template init), so the guide's return type never eagerly instantiates
		// is_losslessly_convertible_unit for a non-unit / non-same-dimension Source — the primary is chosen and the
		// heavy check only runs in the partial specialization, which is constrained to a same-dimension unit source.
		template<class Source, class StrongCf, class Scale, class = void>
		struct deduced_named_underlying
		{
			using type = typename traits::unit_traits<Source>::underlying_type;
		};
		template<class Source, class StrongCf, class Scale>
		struct deduced_named_underlying<Source, StrongCf, Scale,
			std::enable_if_t<traits::is_unit_v<Source> &&
				traits::is_same_dimension_unit_v<Source, unit<StrongCf, typename traits::unit_traits<Source>::underlying_type, Scale>>>>
		{
		private:
			using Src = typename traits::unit_traits<Source>::underlying_type;

		public:
			using type = std::conditional_t<is_losslessly_convertible_unit<Source, unit<StrongCf, Src, Scale>>, Src, floating_point_promotion_t<Src>>;
		};
		template<class Source, class StrongCf, class Scale>
		using deduced_named_underlying_t = typename deduced_named_underlying<Source, StrongCf, Scale>::type;

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
					 *				using `raw()`: @code
					 *				meter_t m(5.0);
					 *				double val = m.raw(); // val == 5.0	@endcode.
					 * @tparam		ConversionFactor `conversion_factor` of the represented unit (e.g. meters)
					 * @tparam		T underlying type of the storage. Defaults to `UNIT_LIB_DEFAULT_TYPE`.
					 * @tparam		NumericalScale optional scale class for the units. Defaults to linear (i.e. does
					 *				not scale the unit value). Examples of non-linear scales could be logarithmic,
					 *				decibel, or richter scales. Numerical scales must adhere to the numerical-scale
					 *				concept, i.e. `is_numerical_scale_v<...>` must be `true`.
					 * @sa
					 *				- \ref lengthContainers "length units"
					 *				- \ref massContainers "mass units"
					 *				- \ref timeContainers "time units"
					 *				- \ref angleContainers "angle units"
					 *				- \ref currentContainers "current units"
					 *				- \ref temperatureContainers "temperature units"
					 *				- \ref substanceContainers "substance units"
					 *				- \ref luminousIntensityContainers "luminous intensity units"
					 *				- \ref solidAngleContainers "solid angle units"
					 *				- \ref frequencyContainers "frequency units"
					 *				- \ref velocityContainers "velocity units"
					 *				- \ref angularVelocityContainers "angular velocity units"
					 *				- \ref accelerationContainers "acceleration units"
					 *				- \ref forceContainers "force units"
					 *				- \ref pressureContainers "pressure units"
					 *				- \ref chargeContainers "charge units"
					 *				- \ref energyContainers "energy units"
					 *				- \ref powerContainers "power units"
					 *				- \ref voltageContainers "voltage units"
					 *				- \ref capacitanceContainers "capacitance units"
					 *				- \ref impedanceContainers "impedance units"
					 *				- \ref magneticFluxContainers "magnetic flux units"
					 *				- \ref magneticFieldStrengthContainers "magnetic field strength units"
					 *				- \ref inductanceContainers "inductance units"
					 *				- \ref luminousFluxContainers "luminous flux units"
					 *				- \ref illuminanceContainers "illuminance units"
					 *				- \ref radiationContainers "radiation units"
					 *				- \ref torqueContainers "torque units"
					 *				- \ref areaContainers "area units"
					 *				- \ref volumeContainers "volume units"
					 *				- \ref densityContainers "density units"
					 *				- \ref concentrationContainers "concentration units"
					 *				- \ref constantContainers "constant units"
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
		 * @brief		compile-time narrowing converting constructor
		 * @details		Constructs an integral-underlying unit from a same-dimension floating-point one when the
		 *				conversion is exact — the case the ordinary converting constructor rejects as lossy. It is
		 *				`consteval`, so it participates only in a constant-evaluated context (`feet<int> f = 16_ft;`);
		 *				a value not exactly representable in the target (a fractional or out-of-range magnitude, e.g.
		 *				`16.5_ft`) makes the constructor a non-constant expression and the program ill-formed. A
		 *				run-time floating-to-integral unit conversion remains rejected. Wholeness is judged on the
		 *				stored point count (`raw()`), so a ratio-dimensionless unit converts correctly too
		 *				(`percent<int> p = 1_pct;` is percent<int> holding 1, not a rejected 0.01).
		 * @param[in]	rhs unit to convert.
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
			requires(traits::is_same_dimension_unit_v<unit<ConversionFactorRhs, Ty, NsRhs>, unit> &&
					 !detail::is_losslessly_convertible_unit<unit<ConversionFactorRhs, Ty, NsRhs>, unit> &&
					 std::is_floating_point_v<Ty> && std::is_integral_v<T>)
		consteval unit(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs)
		  : _linearized_value(detail::exact_integral_cast<T>(unit<ConversionFactor, detail::floating_point_promotion_t<T>, NumericalScale>(rhs).raw()))
		{
		}

		/**
		 * @brief		compile-time exact integral converting constructor
		 * @details		Constructs an integral-underlying unit from a same-dimension integral one of a FINER unit when
		 *				the value is an exact whole number of this (coarser) unit — the case the ordinary converting
		 *				constructor rejects as potentially-lossy because a run-time value need not divide evenly
		 *				(`bytes<int> b = 16_bit;` is `2`, but a run-time `bits<int>` might be `17`). It is `consteval`,
		 *				so it participates only in a constant-evaluated context; a value that is not a whole number of
		 *				the target unit (`17_bit` into `bytes<int>`) makes the constructor a non-constant expression and
		 *				the program ill-formed — never a silent truncation. The exactness test is exact integer
		 *				arithmetic in a double-width intermediate, so it cannot be defeated by an intermediate overflow.
		 *				A run-time integral-to-coarser-integral unit conversion remains rejected; use `round`/`floor`/
		 *				`ceil`/`trunc<To>` for a deliberate run-time rounding.
		 * @param[in]	rhs unit to convert.
		 */
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
			requires(traits::is_same_dimension_unit_v<unit<ConversionFactorRhs, Ty, NsRhs>, unit> &&
					 !detail::is_losslessly_convertible_unit<unit<ConversionFactorRhs, Ty, NsRhs>, unit> &&
					 std::is_integral_v<Ty> && std::is_integral_v<T>)
		consteval unit(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs)
		  : _linearized_value(detail::exact_integral_unit_cast<T>(rhs.raw(),
				std::ratio_divide<typename ConversionFactorRhs::conversion_ratio, typename ConversionFactor::conversion_ratio>::num,
				std::ratio_divide<typename ConversionFactorRhs::conversion_ratio, typename ConversionFactor::conversion_ratio>::den))
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
			requires detail::is_time_conversion_factor<ConversionFactor> && detail::is_losslessly_convertible<Rep, T> &&
			detail::is_losslessly_convertible_unit<wpi::units::unit<wpi::units::conversion_factor<Period, dimension::time>, Rep>, unit>
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
			return value_compare(rhs) < 0;
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
			return value_compare(rhs) <= 0;
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
			return value_compare(rhs) > 0;
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
			return value_compare(rhs) >= 0;
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
			return value_compare(rhs) == 0;
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
		constexpr auto value() const noexcept
		{
			using CfTraits = traits::conversion_factor_traits<ConversionFactor>;

			static constexpr bool needs_fp = traits::is_ratio_dimensionless_cf_v<ConversionFactor> || !std::ratio_equal_v<typename CfTraits::pi_exponent_ratio, std::ratio<0>> ||
				!std::ratio_equal_v<typename CfTraits::translation_ratio, std::ratio<0>>;

			using normalized_value_type = std::conditional_t<needs_fp, detail::floating_point_promotion_t<underlying_type>, underlying_type>;

			if constexpr (traits::is_dimensionless_unit<ConversionFactor>::value)
			{
				// Always normalize dimensionless units to base dimensionless ratio for "value()"
				// For ratio-dimensionless (pct/ppm/ppb), we *promote* the return type so int percent works.
				using Under = normalized_value_type;

				using BaseDimlessCF = wpi::units::conversion_factor<std::ratio<1>, dimension::dimensionless>;

				using BaseDimlessUnit = unit<BaseDimlessCF, Under, NumericalScale>;

				return NumericalScale::scale(wpi::units::convert<BaseDimlessUnit>(*this).to_linearized());
			}
			else
			{
				return static_cast<normalized_value_type>(raw());
			}
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
		 * @brief		conversion to a named unit
		 * @details		Converts to a different named unit of the same dimension, e.g.
		 *				`(100.0_cm).to<meters>()`. The named-template spelling of `convert()`; provided so a
		 *				single accessor reads for both underlying-type extraction (`to<double>()`) and
		 *				dimensioned conversion (`to<meters>()`).
		 * @tparam		UnitType unit class template to convert to
		 * @returns		a `UnitType<T>` containing the equivalent value to *this.
		 */
		template<template<class> class UnitType>
			requires same_dimension<UnitType<T>, unit>
		constexpr UnitType<T> to() const noexcept
		{
			return UnitType<T>(*this);
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
		constexpr UnitType<T> convert() const noexcept
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
			return static_cast<Ty>(this->value());
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
			// unit_name is specialized on the NAMED class, not this unit<...> base; resolve the named form first so a
			// named unit (feet) reports "feet" instead of null. A compound/unnamed unit has no registered name; report
			// the empty string rather than nullptr so the result is always a valid C string to print or copy.
			constexpr const char* n = unit_name_v<detail::rewrap_to_named_t<Unit>>;
			return n ? n : "";
		}

		/**
		 * @brief		returns the unit abbreviation
		 */
		template<UnitType Unit = unit>
		[[nodiscard]] constexpr const char* abbreviation() const noexcept
		{
			// unit_abbreviation is specialized on the NAMED class, not this unit<...> base; resolve the named form
			// first so a named unit (feet) reports "ft" instead of null. A compound/unnamed unit has no registered
			// abbreviation; report the empty string rather than nullptr so the result is always a valid C string.
			constexpr const char* a = unit_abbreviation_v<detail::rewrap_to_named_t<Unit>>;
			return a ? a : "";
		}

		template<ConversionFactorType Cf, ArithmeticType Ty, NumericalScaleType<Ty> Ns>
		friend class unit;

	private:
		/// Order this against `rhs` by MATHEMATICAL VALUE, reconciled to the finer common unit. When both underlying
		/// types are integral, the comparison is signedness-safe (`std::cmp_*`): a signed and an unsigned
		/// representation of the same dimension compare by their true values, so `meters<int>(-1) < meters<unsigned>(1u)`
		/// is `true` rather than wrapping `-1` to a huge unsigned. Floating-point comparison is unchanged.
		template<ConversionFactorType ConversionFactorRhs, ArithmeticType Ty, NumericalScaleType<Ty> NsRhs>
		constexpr auto value_compare(const unit<ConversionFactorRhs, Ty, NsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<ConversionFactorRhs, Ty, NsRhs>>;
			if constexpr (std::is_integral_v<T> && std::is_integral_v<Ty>)
			{
				// Reconcile each side to the common unit's scale in its OWN (sign-preserving) underlying type, then
				// compare with std::cmp_* so a mixed-signedness pair orders by value, not by unsigned wraparound.
				const T   lhsCommon = unit<typename CommonUnit::conversion_factor, T, NumericalScale>(*this)._linearized_value;
				const Ty  rhsCommon = unit<typename CommonUnit::conversion_factor, Ty, NsRhs>(rhs)._linearized_value;
				if (std::cmp_less(lhsCommon, rhsCommon))
					return std::strong_ordering::less;
				if (std::cmp_greater(lhsCommon, rhsCommon))
					return std::strong_ordering::greater;
				return std::strong_ordering::equal;
			}
			else
			{
				const auto lhsCommon = CommonUnit(*this)._linearized_value;
				const auto rhsCommon = CommonUnit(rhs)._linearized_value;
				return lhsCommon <=> rhsCommon;
			}
		}

	public:
		/// Not to be used as part of the official API, this member is public to allow the use
		/// of `units` as NTTP types.
		T _linearized_value;
	};

	namespace detail
	{
		/**
		 * @brief		Maps any unit type to the canonical `unit<Cf, Underlying, Scale>` it represents.
		 * @details		A NAMED unit (e.g. `length::meters<double>`) is a class deriving from its `unit<...>` so a
		 *				diagnostic prints the friendly name; but the exact-pattern trait specializations
		 *				(`replace_underlying`, `floating_point_promotion`, `std::common_type`) match `unit<Cf,T,Ns>`
		 *				literally, not a derived class. `unit_base_t` reconstructs that canonical base from the type's
		 *				own (inherited) member typedefs, so those traits can unwrap first and work for named and plain
		 *				units alike. Identity when `T` already IS a `unit<...>`.
		 */
		template<class T>
		using unit_base_t = unit<typename T::conversion_factor, typename T::underlying_type, typename T::numerical_scale_type>;

		// True iff T is a unit-derived class that is NOT itself the canonical unit<...> (i.e. a NAMED unit). Guarded:
		// unit_base_t<T> (which reads T::conversion_factor) is only well-formed for a unit, so gate on is_unit FIRST
		// via a helper struct — a plain arithmetic T (e.g. double) has no conversion_factor and must yield false, not
		// a hard error.
		template<class T, bool = traits::is_unit<T>::value>
		struct is_named_unit_impl : std::false_type
		{
		};
		template<class T>
		struct is_named_unit_impl<T, true> : std::bool_constant<!std::is_same_v<T, unit_base_t<T>>>
		{
		};
		template<class T>
		inline constexpr bool is_named_unit_v = is_named_unit_impl<T>::value;

		// Two conversion factors are EQUIVALENT when they describe the same physical mapping — same dimension,
		// conversion ratio, pi exponent, and datum — even if they are different C++ types (a flattened
		// `conversion_factor<ratio<1,100>, length>` versus the composed `centi<meters_>` that `centimeters` is
		// registered as). Type identity is stricter than equivalence; a reconciliation result that is equivalent
		// to an operand's unit should still recover that operand's friendly name.
		template<class Cf1, class Cf2>
		inline constexpr bool is_equivalent_conversion_factor_v =
			traits::is_same_dimension_conversion_factor_v<Cf1, Cf2> &&
			std::ratio_equal_v<typename Cf1::conversion_ratio, typename Cf2::conversion_ratio> &&
			std::ratio_equal_v<typename Cf1::pi_exponent_ratio, typename Cf2::pi_exponent_ratio> &&
			std::ratio_equal_v<typename Cf1::translation_ratio, typename Cf2::translation_ratio>;

		// A conversion factor is RAW when it carries no registered name of its own — a bare reconciliation result
		// such as the flattened gcd of meters and centimeters, for which `named_class_of` finds no registration and
		// `rewrap_to_named` is the identity. A named unit's registered factor (meters_, joules_, …) is NOT raw: it
		// resolves to its named class. Equivalence-based name recovery fires only for a RAW factor, because
		// recovering a name for an already-named factor could rename one physical kind to another that shares its
		// dimension and ratio (torque's newton_meters_ and energy's joules_ are equivalent) — so recovery is
		// restricted to the anonymous reconciliation results that have no name to preserve.
		template<class Cf>
		inline constexpr bool is_raw_conversion_factor_v =
			std::is_void_v<decltype(named_class_of(static_cast<Cf*>(nullptr), static_cast<linear_scale*>(nullptr)))>;

		// Re-wrap a computed base result `unit<Cf, U, Ns>` into a NAMED unit when a candidate operand `Named` is a
		// named unit of the SAME conversion_factor: the friendly name is preserved through the trait (so
		// common_type<meters<int>, meters<double>> is meters<double>, not unit<meters_, double, linear_scale>). When no
		// candidate matches (mixed names, or a plain-unit operand), the base result stands. `Base` is the plain unit<>.
		template<class Base, class Named, class = void>
		struct rewrap_named
		{
			using type = Base;
		};
		template<class Base, class Named>
		struct rewrap_named<Base, Named,
			std::enable_if_t<is_named_unit_v<Named> && std::is_same_v<typename Base::conversion_factor, typename Named::conversion_factor>>>
		{
			using type = typename Named::template rebind<typename Base::underlying_type>;
		};
		// Equivalence recovery: when `Base`'s factor is RAW (an anonymous reconciliation result, e.g. the flattened
		// gcd of meters and centimeters) and is equivalent to a named operand's factor, recover that operand's name.
		// This names an m − cm result `centimeters` and an hr − min result `minutes` where exact-type matching missed
		// them, without renaming an already-named result (the raw guard excludes strong factors such as joules_).
		template<class Base, class Named>
		struct rewrap_named<Base, Named,
			std::enable_if_t<is_named_unit_v<Named> && !std::is_same_v<typename Base::conversion_factor, typename Named::conversion_factor> &&
				is_raw_conversion_factor_v<typename Base::conversion_factor> &&
				is_equivalent_conversion_factor_v<typename Base::conversion_factor, typename Named::conversion_factor>>>
		{
			using type = typename Named::template rebind<typename Base::underlying_type>;
		};
		template<class Base, class Named>
		using rewrap_named_t = typename rewrap_named<Base, Named>::type;

		// Identity fallback for the CF-struct -> named-class ADL map (the exact registrations are emitted per named
		// unit by UNIT_REGISTER_NAMED_CLASS). Worst match (trailing ellipsis); returns void to signal "no named class
		// for this CF". decltype-only, never defined. A real registration's exact strong-CF* parameter beats this.
		template<class ConversionFactor, class Scale>
		void named_class_of(ConversionFactor*, Scale*, ...);

		// Map a plain unit<Cf, U, Ns> to its NAMED class when one is registered for Cf, else identity. Used by the
		// arithmetic operators so a computed result (e.g. unit<square_meters_, int, linear_scale>) is REPORTED as the
		// friendly named type (square_meters<int>). Rebinds the registered class to U so the underlying flows through.
		// SFINAE-guarded: only a unit whose Cf has a registration is rewrapped; everything else is identity.
		// (The primary template + the rewrap_to_named_t alias are forward-declared before the unit class so unit's
		// name()/abbreviation() members can name them; here we DEFINE the primary and the specialization.)
		template<class U, class>
		struct rewrap_to_named
		{
			using type = U;
		};
		template<class U>
		struct rewrap_to_named<U,
			std::enable_if_t<traits::is_unit<U>::value &&
				!std::is_void_v<decltype(named_class_of(static_cast<typename U::conversion_factor*>(nullptr),
					static_cast<typename U::numerical_scale_type*>(nullptr)))>>>
		{
			using type = typename decltype(named_class_of(static_cast<typename U::conversion_factor*>(nullptr),
				static_cast<typename U::numerical_scale_type*>(nullptr)))::template rebind<typename U::underlying_type>;
		};
	} // namespace detail

	namespace traits
	{
		// A NAMED unit (a class deriving from unit<...>) unwraps to its base for these exact-pattern traits, so
		// replace_underlying / floating_point_promotion behave for named units exactly as for the plain unit<...>.
		// The plain-unit<...> specializations are declared earlier; these constrained ones fire only for a named unit.
		template<class Unit, class Underlying>
			requires ::wpi::units::detail::is_named_unit_v<Unit>
		struct replace_underlying<Unit, Underlying>
		{
			// PRESERVE the named type: rebind it to the new underlying (meters<int> -> meters<double>), rather than
			// decaying to the plain unit<...> base. Keeps trait results as friendly as the inputs.
			using type = typename Unit::template rebind<Underlying>;
		};
	} // namespace traits

	namespace detail
	{
		template<class Unit>
			requires is_named_unit_v<Unit>
		struct floating_point_promotion<Unit>
		{
			// Promote the UNDERLYING type but PRESERVE the friendly named type: rebind the named unit to the promoted
			// underlying (meters<int> -> meters<double>), so ceil/floor/round/hypot report the named result, not unit<>.
			using type = typename Unit::template rebind<typename floating_point_promotion<unit_base_t<Unit>>::type::underlying_type>;
		};
	} // namespace detail

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

	//-----------------------------------------
	//	UNIT-LABEL STRING BUILDERS
	//-----------------------------------------

#if defined(UNIT_LIB_ENABLE_STRING)

	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: dimension_to_string [static]
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief      Renders a single dimension term (base dimension + exponent) as text.
		/// @tparam     D    the base dimension (supplies `D::abbreviation`).
		/// @tparam     E    the exponent, a `std::ratio` (`E::num`/`E::den`).
		/// @return     the term as `" <abbrev>"`, plus `"^num"` when the exponent is not 1 and `"/den"`
		///             when the denominator is not 1 — matching the ostream inserter's format, including
		///             its leading space per term.
		//----------------------------------------------------------------------------------------------------------------------
		template<class D, class E>
		std::string dimension_to_string(const dim<D, E>&)
		{
			std::string s;
			if constexpr (E::num != 0)
			{
				s.append(" ").append(D::abbreviation);
			}
			if constexpr (E::num != 0 && E::num != 1)
			{
				s.append("^").append(std::to_string(E::num));
			}
			if constexpr (E::den != 1)
			{
				s.append("/").append(std::to_string(E::den));
			}
			return s;
		}

		//----------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: dimension_to_string [static]
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief      Renders a full dimension list as text by concatenating each term.
		/// @tparam     Dims    the dimension terms of the list.
		/// @return     the concatenation of each term's `dimension_to_string`, e.g. `" m s^-2"`.
		//----------------------------------------------------------------------------------------------------------------------
		template<class... Dims>
		std::string dimension_to_string(const dimension_t<Dims...>&)
		{
			std::string s;
			((s.append(dimension_to_string(Dims{}))), ...);
			return s;
		}

		//----------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: unit_label [static]
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief      Builds the unit-label suffix for a unit — the text that follows its numeric value.
		/// @details    Resolves the named form of the unit first, so a named unit yields its abbreviation
		///             (`meters_per_second` → `"mps"`); an unnamed compound unit yields its dimension list
		///             (`" m s^-2"`). The value itself is NOT included. Depends only on `<string>`, so it
		///             is available whether or not iostream support is compiled in.
		/// @tparam     ConversionFactor    the unit's conversion factor.
		/// @tparam     T                   the unit's underlying arithmetic type.
		/// @tparam     NumericalScale      the unit's numerical scale.
		/// @return     the label, either `" <abbrev>"` (with a leading space) for a named unit, or the
		///             dimension-list text (also leading-space-prefixed per term) for an unnamed unit; empty
		///             for a dimensionless unnamed unit.
		//----------------------------------------------------------------------------------------------------------------------
		/// The form a unit label may take.
		/// @details	`abbreviation` and `name` render the unit's OWN symbol/name and never convert the value;
		///				for an unnamed compound they fall back to the base-dimension list (which is honest,
		///				since an unnamed unit carries no conversion of its own). `base` is the SI base form —
		///				the base-dimension list — and its VALUE must be converted to base SI to stay honest,
		///				because the type system flattens a named unit's identity into a single ratio and cannot
		///				recover a non-SI factor's own symbols (e.g. `feet_per_second` cannot render as
		///				`ft s^-1`; only `m s^-1` against the base-converted value is correct).
		enum class label_form
		{
			abbreviation,    ///< the unit's own abbreviation (`"m"`, `"ft"`), the default; base-dimension list if unnamed.
			name,            ///< the unit's own full name (`"meters"`, `"feet"`); base-dimension list if unnamed.
			base             ///< the SI base-dimension list (`" m s^-1"`); pairs with a base-converted value.
		};

		template<label_form Form = label_form::abbreviation, ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
		std::string unit_label(const unit<ConversionFactor, T, NumericalScale>&)
		{
			// The name/abbreviation traits are specialized on the NAMED class, not the plain unit<...> base,
			// so resolve the named form first and query THAT (a named unit prints its name/abbreviation).
			using NamedForm = detail::rewrap_to_named_t<unit<ConversionFactor, T, NumericalScale>>;
			using DimType   = traits::dimension_of_t<ConversionFactor>;

			if constexpr (Form == label_form::base)
			{
				// SI base-dimension list, regardless of the unit's own name (the caller base-converts the value).
				if constexpr (!DimType::empty)
					return dimension_to_string(DimType{});
				else
					return std::string{};
			}
			else if constexpr (Form == label_form::name && unit_name_v<NamedForm>)
			{
				return std::string(" ").append(unit_name<NamedForm>::value);
			}
			else if constexpr (unit_abbreviation_v<NamedForm>)
			{
				return std::string(" ").append(unit_abbreviation<NamedForm>::value);
			}
			else
			{
				// Unnamed unit: its honest label IS the base-dimension list (no own symbol exists).
				if constexpr (!DimType::empty)
					return dimension_to_string(DimType{});
				else
					return std::string{};
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: label_uses_base_unit [static]
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief      Whether a unit's label is its dimension list rather than a named abbreviation.
		/// @details    An unnamed unit is rendered in its BASE unit (its value must be converted to the base
		///             before the dimension label applies); a named unit prints its value as-is. This
		///             predicate lets the value-rendering paths decide whether to convert to the base unit.
		/// @tparam     ConversionFactor    the unit's conversion factor.
		/// @tparam     T                   the unit's underlying arithmetic type.
		/// @tparam     NumericalScale      the unit's numerical scale.
		/// @return     `true` when the unit is unnamed (dimension-labelled), `false` when it is named.
		//----------------------------------------------------------------------------------------------------------------------
		template<ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
		inline constexpr bool label_uses_base_unit()
		{
			using NamedForm = detail::rewrap_to_named_t<unit<ConversionFactor, T, NumericalScale>>;
			return !static_cast<bool>(unit_abbreviation_v<NamedForm>);
		}
	}    // namespace detail

#endif // UNIT_LIB_ENABLE_STRING

#if defined(UNIT_LIB_ENABLE_FORMAT)

	//-----------------------------------------
	//	std::format SUPPORT
	//-----------------------------------------

	namespace detail
	{
		//----------------------------------------------------------------------------------------------------------------------
		//      STRUCT: unit_format_options
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief      The parsed unit-opts portion of a unit format-spec (see the `std::formatter` below).
		//----------------------------------------------------------------------------------------------------------------------
		struct unit_format_options
		{
			label_form  form      = label_form::abbreviation;    ///< which label form was requested
			bool        showValue = true;                        ///< emit the numeric value
			bool        showUnit  = true;                        ///< emit the unit label
			bool        customSep = false;                       ///< a separator literal was supplied
			std::string separator = " ";                         ///< separator between value and label
		};
	}    // namespace detail

#endif    // UNIT_LIB_ENABLE_FORMAT

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)

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

		// The abbreviation trait is specialized on the NAMED class, not the plain unit<...> base this overload
		// deduces; resolve the named form first and query THAT so a named unit (meters_per_second -> "mps") prints
		// its abbreviation instead of the dimension form.
		using NamedForm = detail::rewrap_to_named_t<unit<ConversionFactor, T, NumericalScale>>;

		if constexpr (unit_abbreviation_v<NamedForm>)
		{
			os << obj.raw();
		}
		else
		{
			os << std::conditional_t<detail::is_losslessly_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>, BaseUnit, PromotedBaseUnit>(obj).raw();
		}
		os << detail::unit_label(obj);

		return os;
	}
#endif

	//----------------------------
	//  to_string
	//----------------------------

	template<ConversionFactorType ConversionFactor, ArithmeticType T, NumericalScaleType<T> NumericalScale>
	std::string to_string(const unit<ConversionFactor, T, NumericalScale>& obj)
	{
		using BaseConversion   = conversion_factor<std::ratio<1>, typename ConversionFactor::dimension_type>;
		using BaseUnit         = unit<BaseConversion, T, NumericalScale>;
		using PromotedBaseUnit = unit<BaseConversion, detail::floating_point_promotion_t<T>, NumericalScale>;

		// The abbreviation trait (unit_name/unit_abbreviation) is specialized on the NAMED class, not the plain
		// unit<...> base this overload deduces, so resolve the named form first and query THAT — a named unit
		// (feet<double>) then still prints its abbreviation ("ft") instead of falling to the dimension path.
		using NamedForm = detail::rewrap_to_named_t<unit<ConversionFactor, T, NumericalScale>>;

		std::string s;
		if constexpr (unit_abbreviation_v<NamedForm>)
			s = detail::to_string(obj.raw());
		else
			s = detail::to_string(std::conditional_t<detail::is_losslessly_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>, BaseUnit, PromotedBaseUnit>(obj).raw());

		s.append(detail::unit_label(obj));
		return s;
	}

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

		/// The datum offset a reconciliation of two units should carry. When both operands share the offset it is
		/// kept (celsius with celsius stays celsius). When they differ — one carries a datum and the other does not
		/// (celsius vs kelvin) — the common unit takes the CLEAN, offset-free value: a sane user reconciling degrees
		/// Celsius with kelvin expects kelvin, not a hybrid scale carrying an arbitrary zero. Applied only to the
		/// translation ratio; the conversion and pi ratios keep their greatest common measure so magnitudes stay
		/// lossless (that is what integer comparisons rely on).
		template<RatioType Ratio1, RatioType Ratio2>
		using common_baggage_ratio = std::conditional_t<std::ratio_equal_v<Ratio1, Ratio2>, Ratio1, std::ratio<0>>;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE
} // end namespace wpi::units

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
					wpi::units::detail::common_baggage_ratio<typename ConversionFactorLhs::translation_ratio, typename ConversionFactorRhs::translation_ratio>>>,
				common_type_t<Tx, Ty>, NumericalScale>>
	{
	};

	// In the case the two units are the same type, just use that type as common type
	template<class UnitConversionT, class T, class NonLinearScale>
	struct common_type<wpi::units::unit<UnitConversionT, T, NonLinearScale>, wpi::units::unit<UnitConversionT, T, NonLinearScale>>
	{
		using type = wpi::units::unit<UnitConversionT, T, NonLinearScale>;
	};

	// A NAMED unit is a class deriving from unit<...>; the exact-pattern specializations above do not match it. When
	// either operand is a named unit, compute the common type of the canonical unit<...> BASES, then RE-WRAP the result
	// into the named type when an operand shares its conversion_factor — so common_type<meters<int>, meters<double>> is
	// meters<double>, not the plain unit<...> (the friendly name survives through the trait). Constrained to "both are
	// units AND at least one is named" so it never overlaps the exact-unit<...> cases above.
	template<class Lhs, class Rhs>
		requires(wpi::units::traits::is_unit<Lhs>::value && wpi::units::traits::is_unit<Rhs>::value &&
				 (wpi::units::detail::is_named_unit_v<Lhs> || wpi::units::detail::is_named_unit_v<Rhs>) &&
				 // ONLY when the plain-base common type EXISTS (same dimension). For different dimensions the bases have
				 // no common type, so this specialization must be SFINAE-EMPTY too (no `type`) — matching the plain
				 // unit<...> behavior. Without this, computing `base` below is a hard error on stricter compilers
				 // (clang) where g++ tolerated the absent member.
				 requires { typename common_type<wpi::units::detail::unit_base_t<Lhs>, wpi::units::detail::unit_base_t<Rhs>>::type; })
	struct common_type<Lhs, Rhs>
	{
	private:
		using base = common_type_t<wpi::units::detail::unit_base_t<Lhs>, wpi::units::detail::unit_base_t<Rhs>>;
		// prefer to re-wrap into Lhs's name; if that doesn't share the CF, try Rhs's.
		using viaLhs = wpi::units::detail::rewrap_named_t<base, Lhs>;

	public:
		using type = wpi::units::detail::rewrap_named_t<viaLhs, Rhs>;
	};

	// A NAMED DIMENSIONLESS unit (e.g. percent) mixed with a plain arithmetic scalar: the exact-unit<...>-vs-scalar
	// specializations below do not match the named class, so unwrap the named operand to its base and re-wrap the
	// result to keep the friendly name. dimensionless units stay fully interchangeable with int/double. Gated on
	// is_dimensionless_unit (mirroring the plain unit<...>-vs-scalar specializations): a DIMENSIONED named unit + a
	// scalar must NOT match — it falls through to the primary std::common_type and is SFINAE-empty (no `type`), the
	// same SFINAE-friendly behavior the plain form has (never a hard error).
	template<class Named, class Scalar>
		requires(wpi::units::detail::is_named_unit_v<Named> && std::is_arithmetic_v<Scalar> &&
				 wpi::units::traits::is_dimensionless_unit<typename wpi::units::detail::unit_base_t<Named>::conversion_factor>::value)
	struct common_type<Named, Scalar>
	{
		using type = wpi::units::detail::rewrap_named_t<common_type_t<wpi::units::detail::unit_base_t<Named>, Scalar>, Named>;
	};
	template<class Scalar, class Named>
		requires(wpi::units::detail::is_named_unit_v<Named> && std::is_arithmetic_v<Scalar> &&
				 wpi::units::traits::is_dimensionless_unit<typename wpi::units::detail::unit_base_t<Named>::conversion_factor>::value)
	struct common_type<Scalar, Named>
	{
		using type = wpi::units::detail::rewrap_named_t<common_type_t<Scalar, wpi::units::detail::unit_base_t<Named>>, Named>;
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
		requires std::is_arithmetic_v<Ty>    // constrain so a unit `Ty` never matches (that is a unit+unit case above)
	struct common_type<Ty, wpi::units::unit<ConversionFactor, Tx, NumericalScale>>
	  : std::enable_if<wpi::units::traits::is_dimensionless_unit<wpi::units::unit<ConversionFactor, Tx, NumericalScale>>::value,
			wpi::units::unit<wpi::units::conversion_factor<std::ratio<1>, wpi::units::dimension::dimensionless>, common_type_t<Tx, Ty>, NumericalScale>>
	{
	};

	template<class ConversionFactor, class Tx, class NumericalScale, class Ty>
		requires std::is_arithmetic_v<Ty>    // constrain so a unit `Ty` never matches (that is a unit+unit case above)
	struct common_type<wpi::units::unit<ConversionFactor, Tx, NumericalScale>, Ty>
	  : std::enable_if<wpi::units::traits::is_dimensionless_unit<wpi::units::unit<ConversionFactor, Tx, NumericalScale>>::value,
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
		 * @param[in]   value value to linearize
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
		 * @param[in]   value value to scale
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

	namespace detail
	{
		// ADL registration of the dimensionless strong type (see detail::strong_name, #357). The base-form CF maps
		// back to the canonical dimensionless conversion_factor. Declared, never defined (used only in decltype).
		conversion_factor<std::ratio<1>, dimension::dimensionless> strong_name(
			wpi::units::detail::conversion_factor_base_t<dimensionless_>*);
	}
	// The PURE dimensionless unit (ratio 1) stays a plain alias to unit<...>, NOT a named class: it must remain
	// totally interchangeable with the built-in arithmetic types (int/double) and identity-equal to its unit<...>
	// base (so common_type<dimensionless<int>, int> is the plain unit and dimensionless<int> IS unit<dimensionless_,
	// int>). A distinct class would break that interchangeability. Named ratio-dimensionless units (percent/ppm/...)
	// are still classes — they carry a meaningful name.
	template<class Underlying = UNIT_LIB_DEFAULT_TYPE>
	using dimensionless = unit<traits::strong_t<conversion_factor<std::ratio<1>, dimension::dimensionless>>, Underlying, linear_scale>;

	UNIT_ADD_DIMENSION_TRAIT(dimensionless, Dimensionless)

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
		requires(!traits::is_affine_unit_v<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator+=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	}

	/// Compound addition for AFFINE units (e.g. temperatures). The lhs is an absolute point; the rhs is
	/// interpreted as a RELATIVE delta and the point is moved in place by that magnitude, staying in the lhs
	/// unit (celsius(20) += celsius(5) -> celsius(25), i.e. "warm by 5 degrees"). The rhs's datum offset is
	/// intentionally not applied — only its magnitude in the lhs unit matters for a delta. (Binary `a + b`
	/// of two absolute affine points is disabled; use `+=` to move a point by a relative amount.)
	template<UnitType UnitTypeLhs>
		requires(traits::is_affine_unit_v<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator+=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = UnitTypeLhs(lhs.raw() + rhs.raw());
		return lhs;
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator+=(UnitTypeLhs& lhs, T rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr U& operator+=(U& lhs, T rhs) noexcept
	{
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		// points_per_one converts "fraction-space 1.0" into "points" for this unit.
		// Example: percent ratio = 1/100 -> points_per_one = 100
		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		// Do the math in points space to avoid truncation of lhs.value() for integral percent.
		const long double new_points = static_cast<long double>(lhs.raw()) + (static_cast<long double>(rhs) * points_per_one);

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(new_points)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(new_points));
		}

		return lhs;
	}

	template<RatioDimensionlessUnitType U, DimensionlessUnitType D>
		requires(traits::has_linear_scale_v<U, D> && !RatioDimensionlessUnitType<D>)
	constexpr U& operator+=(U& lhs, const D& rhs) noexcept
	{
		// rhs.value() is plain scalar (e.g. dimensionless<int>(1) => 1)
		return (lhs += rhs.value());
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr U& operator-=(U& lhs, T rhs) noexcept
	{
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		const long double new_points = static_cast<long double>(lhs.raw()) - (static_cast<long double>(rhs) * points_per_one);

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(new_points)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(new_points));
		}

		return lhs;
	}

	template<UnitType UnitTypeLhs>
		requires(!traits::is_affine_unit_v<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator-=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	}

	/// Compound subtraction for AFFINE units (e.g. temperatures). The lhs is an absolute point; the rhs is
	/// interpreted as a RELATIVE delta and the point is moved down in place by that magnitude, staying in the
	/// lhs unit (celsius(20) -= celsius(5) -> celsius(15), i.e. "cool by 5 degrees"). The rhs's datum offset
	/// is intentionally not applied. (Binary `a - b` of two absolute affine points yields a non-affine delta;
	/// use `-=` to move a point down by a relative amount.)
	template<UnitType UnitTypeLhs>
		requires(traits::is_affine_unit_v<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator-=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = UnitTypeLhs(lhs.raw() - rhs.raw());
		return lhs;
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator-=(UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	}

	template<RatioDimensionlessUnitType U, DimensionlessUnitType D>
		requires(traits::has_linear_scale_v<U, D> && !RatioDimensionlessUnitType<D>)
	constexpr U& operator-=(U& lhs, const D& rhs) noexcept
	{
		return (lhs -= rhs.value());
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator*=(UnitTypeLhs& lhs, const T& rhs)
	{
		// The rhs is taken as its own arithmetic type (not narrowed to the lhs underlying type at the call
		// boundary), so a value-narrowing scale (e.g. meters<int> *= 2.0) applies normal conversion rules. The
		// narrowing is performed by an implicit conversion into a local of the lhs's underlying type, which surfaces
		// the compiler's -Wfloat-conversion warning naming `meters<int>::underlying_type (aka int)` rather than
		// truncating silently; it is a warning, not an error, and the result stays a UnitTypeLhs.
		typename UnitTypeLhs::underlying_type scaled = lhs.raw() * rhs;
		lhs                                          = UnitTypeLhs(scaled, linearized_value);
		return lhs;
	}

	template<RatioDimensionlessUnitType U, RatioDimensionlessUnitType URhs>
		requires(traits::has_linear_scale_v<U, URhs>)
	constexpr U& operator*=(U& lhs, const URhs& rhs) noexcept
	{
		using LhsUnder = typename U::underlying_type;
		using RhsUnder = typename URhs::underlying_type;

		using Calc0 = std::common_type_t<LhsUnder, RhsUnder>;
		using Calc  = detail::floating_point_promotion_t<Calc0>;

		// rhs interpreted as normalized fraction (e.g. 200_pct -> 2.0, 2_pct -> 0.02)
		const Calc rhs_frac = static_cast<Calc>(rhs.value());

		// lhs.raw() is "points" (e.g. 12_pct raw() == 12)
		const Calc new_points = static_cast<Calc>(lhs.raw()) * rhs_frac;

		if constexpr (std::is_integral_v<LhsUnder>)
		{
			// Deterministic: truncate toward zero
			lhs = U(static_cast<LhsUnder>(new_points));
		}
		else
		{
			lhs = U(static_cast<LhsUnder>(new_points));
		}

		return lhs;
	}

	template<RatioDimensionlessUnitType U>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr U& operator*=(U& lhs, const U& rhs) noexcept
	{
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		// percent: 1/100 -> points_per_one = 100
		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		const long double lhs_frac = static_cast<long double>(lhs.value()); // normalized fraction
		const long double rhs_frac = static_cast<long double>(rhs.value()); // normalized fraction

		const long double out_frac   = lhs_frac * rhs_frac;
		const long double out_points = out_frac * points_per_one;

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(out_points)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(out_points));
		}

		return lhs;
	}

	template<RatioDimensionlessUnitType U, wpi::units::ArithmeticType T>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr U& operator*=(U& lhs, T rhs) noexcept
	{
		// scalar is interpreted as base-dimensionless fraction (world-2)
		// so rhs = 2 means multiply fraction by 2
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		const long double lhs_frac = static_cast<long double>(lhs.value());
		const long double out_frac = lhs_frac * static_cast<long double>(rhs);
		const long double out_pts  = out_frac * points_per_one;

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(out_pts)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(out_pts));
		}
		return lhs;
	}

	template<RatioDimensionlessUnitType U, DimensionlessUnitType D>
		requires(wpi::units::traits::has_linear_scale_v<U, D> && !RatioDimensionlessUnitType<D>)
	constexpr U& operator*=(U& lhs, const D& rhs) noexcept
	{
		// dimensionless is a scalar fraction; use its numeric value
		return (lhs *= rhs.value());
	}

	// scale a dimensioned quantity by a dimensionless quantity: use its numeric value and route through the
	// arithmetic overload above (preserves the warn-on-lossy-integer-scale behavior)
	template<UnitType UnitTypeLhs, DimensionlessUnitType D>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator*=(UnitTypeLhs& lhs, const D& rhs)
	{
		return (lhs *= rhs.value());
	}

	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator/=(UnitTypeLhs& lhs, const T& rhs)
	{
		// see operator*= above: a floating-point divisor narrowing an integer-underlying quantity surfaces
		// -Wfloat-conversion via the implicit narrow into a local of the lhs underlying type
		typename UnitTypeLhs::underlying_type scaled = lhs.raw() / rhs;
		lhs                                          = UnitTypeLhs(scaled, linearized_value);
		return lhs;
	}

	template<UnitType UnitTypeLhs, DimensionlessUnitType D>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator/=(UnitTypeLhs& lhs, const D& rhs)
	{
		return (lhs /= rhs.value());
	}

	template<RatioDimensionlessUnitType U, RatioDimensionlessUnitType URhs>
		requires(traits::has_linear_scale_v<U, URhs>)
	constexpr U& operator/=(U& lhs, const URhs& rhs) noexcept
	{
		using Under0 = std::common_type_t<typename U::underlying_type, typename URhs::underlying_type>;
		using Under  = detail::floating_point_promotion_t<Under0>;

		const Under rhs_frac = static_cast<Under>(rhs.value()); // normalized fraction

		const Under new_points = static_cast<Under>(lhs.raw()) / rhs_frac;

		lhs = U(new_points);
		return lhs;
	}

	template<RatioDimensionlessUnitType U>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr U& operator/=(U& lhs, const U& rhs) noexcept
	{
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		const long double lhs_frac = static_cast<long double>(lhs.value());
		const long double rhs_frac = static_cast<long double>(rhs.value());

		const long double out_frac   = lhs_frac / rhs_frac;
		const long double out_points = out_frac * points_per_one;

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(out_points)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(out_points));
		}
		return lhs;
	}

	template<RatioDimensionlessUnitType U, wpi::units::ArithmeticType T>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr U& operator/=(U& lhs, T rhs) noexcept
	{
		using Underlying = typename U::underlying_type;
		using R          = typename U::conversion_factor::conversion_ratio;

		constexpr long double points_per_one = static_cast<long double>(R::den) / static_cast<long double>(R::num);

		const long double lhs_frac = static_cast<long double>(lhs.value());
		const long double out_frac = lhs_frac / static_cast<long double>(rhs);
		const long double out_pts  = out_frac * points_per_one;

		if constexpr (std::is_integral_v<Underlying>)
		{
			lhs = U(static_cast<Underlying>(std::llround(out_pts)));
		}
		else
		{
			lhs = U(static_cast<Underlying>(out_pts));
		}
		return lhs;
	}

	template<RatioDimensionlessUnitType U, DimensionlessUnitType D>
		requires(wpi::units::traits::has_linear_scale_v<U, D> && !RatioDimensionlessUnitType<D>)
	constexpr U& operator/=(U& lhs, const D& rhs) noexcept
	{
		return (lhs /= rhs.value());
	}

	template<DimensionedUnitType UnitTypeLhs>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const detail::type_identity_t<UnitTypeLhs>& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	template<DimensionlessUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(!(RatioDimensionlessUnitType<UnitTypeLhs> || RatioDimensionlessUnitType<UnitTypeRhs>))
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		lhs              = CommonUnit(lhs.raw() % rhs.raw());
		return lhs;
	}

	template<UnitType UnitTypeLhs>
		requires(!RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr UnitTypeLhs& operator%=(UnitTypeLhs& lhs, const typename UnitTypeLhs::underlying_type& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	// ratio-dimensionless %= ratio-dimensionless  (percent points modulo percent points)
	template<RatioDimensionlessUnitType U>
		requires(traits::has_linear_scale_v<U>)
	constexpr U& operator%=(U& lhs, const U& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	// ratio-dimensionless %= scalar  (percent points modulo scalar)
	template<RatioDimensionlessUnitType U>
		requires(traits::has_linear_scale_v<U>)
	constexpr U& operator%=(U& lhs, const typename U::underlying_type& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	// ratio-dimensionless %= base dimensionless unit  (treat as scalar)
	template<RatioDimensionlessUnitType U, DimensionlessUnitType D>
		requires(traits::has_linear_scale_v<U, D> && !RatioDimensionlessUnitType<D>)
	constexpr U& operator%=(U& lhs, const D& rhs) noexcept
	{
		// D is ordinary dimensionless: safe scalar conversion
		lhs = lhs % static_cast<typename U::underlying_type>(rhs);
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
	/// @note	Disabled when either operand is AFFINE (carries a datum offset, e.g. degrees Celsius): summing two
	///			offset quantities adds their datums, which is arithmetically corrupt (20 degC + 5 degC is not
	///			25 degC in any absolute sense). Zero-offset scales of the same dimension (including kelvin and
	///			rankine) add normally — the sum is well-defined arithmetically even where it is rarely the
	///			physically intended operation.
	/// @details	The result is expressed in the LEFT operand's unit, so the caller controls the result unit by
	///			operand order (`meters + feet` is meters, `feet + meters` is feet) and the value reads in the unit
	///			they named. The underlying is widened only when the left operand's is integral and the right cannot
	///			convert into it without truncation, in which case the result reconciles to the common (finest,
	///			lossless) unit — the same exact behavior integer comparisons rely on.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
			!traits::is_affine_unit_v<UnitTypeLhs> && !traits::is_affine_unit_v<UnitTypeRhs>)
	constexpr auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		// The result unit is computed in the body (not the signature) so the trait is never instantiated for a
		// non-unit operand that the constraint above already rejects — a stricter compiler evaluates a trailing
		// return type during overload resolution and would otherwise hard-error on, e.g., a vector iterator's
		// pointer subtraction that briefly considers this operator.
		using ResultUnit = detail::lhs_result_unit_t<UnitTypeLhs, UnitTypeRhs>;
		return ResultUnit(ResultUnit(lhs).raw() + ResultUnit(rhs).raw());
	}

	/// Addition template for ratio-like dimensionless units (concentrations, etc)
	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr auto operator+(const U& lhs, T rhs) noexcept -> traits::replace_underlying_t<U, detail::floating_point_promotion_t<std::common_type_t<typename U::underlying_type, T>>>
	{
		using Under0 = std::common_type_t<typename U::underlying_type, T>;
		using Under  = detail::floating_point_promotion_t<Under0>;
		using Ret    = traits::replace_underlying_t<U, Under>;

		using R                        = typename U::conversion_factor::conversion_ratio; // e.g. percent: 1/100
		constexpr Under points_per_one = static_cast<Under>(R::den) / static_cast<Under>(R::num);

		// fraction-space math, then back to points
		const Under frac = static_cast<Under>(lhs.value()) + static_cast<Under>(rhs);
		return Ret(frac * points_per_one);
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr auto operator+(T lhs, const U& rhs) noexcept -> traits::replace_underlying_t<U, detail::floating_point_promotion_t<std::common_type_t<T, typename U::underlying_type>>>
	{
		using Under0 = std::common_type_t<T, typename U::underlying_type>;
		using Under  = detail::floating_point_promotion_t<Under0>;
		using Ret    = traits::replace_underlying_t<U, Under>;

		using R                        = typename U::conversion_factor::conversion_ratio;
		constexpr Under points_per_one = static_cast<Under>(R::den) / static_cast<Under>(R::num);

		const Under frac = static_cast<Under>(lhs) + static_cast<Under>(rhs.value());
		return Ret(frac * points_per_one);
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator+(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using ret = traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return ret(lhs.raw() + static_cast<ret::underlying_type>(rhs));
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs> && !RatioDimensionlessUnitType<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator+(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs + rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(InverseCommonUnit(lhs).value() + rhs.raw());
	}

	/// Subtraction operator for NON-AFFINE unit types with a linear_scale.
	/// @details	Like `operator+`, the result is in the LEFT operand's unit (caller controls the result unit by
	///			operand order), widening to the common lossless unit only when the left operand is integral and cannot
	///			hold the right without truncation.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
			!traits::is_affine_unit_v<UnitTypeLhs> && !traits::is_affine_unit_v<UnitTypeRhs>)
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		// Result unit computed in the body, not the signature — see operator+ above.
		using ResultUnit = detail::lhs_result_unit_t<UnitTypeLhs, UnitTypeRhs>;
		return ResultUnit(ResultUnit(lhs).raw() - ResultUnit(rhs).raw());
	}

	/// Subtraction operator for AFFINE unit types (e.g. temperatures with a datum offset).
	/// @details	The difference of two absolute affine quantities is a DELTA: the datum offsets cancel, so
	///				the result must be a pure (non-affine) quantity — otherwise storing it back into an affine
	///				unit would re-apply the offset (e.g. celsius(0) - kelvin(0) would read 546.30 K instead of
	///				the true 273.15 K delta). Both operands are reconciled to their common affine unit, their
	///				raw values subtracted (the offsets cancel exactly), and the result returned in the
	///				offset-stripped counterpart of that common unit so it never re-applies a datum.
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
			(traits::is_affine_unit_v<UnitTypeLhs> || traits::is_affine_unit_v<UnitTypeRhs>))
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		// Reconcile to the LEFT operand's affine unit (its datum applied to the right operand as it converts),
		// so the delta is expressed in the left operand's scale — celsius(100) - fahrenheit(32) is 100 celsius
		// degrees, not a value in an anonymous sub-unit of the two scales' common measure. The result is the
		// offset-STRIPPED counterpart of the left unit so no datum is ever re-applied to the delta.
		using LhsCf     = typename traits::unit_traits<UnitTypeLhs>::conversion_factor;
		using DeltaCf   = conversion_factor<typename traits::conversion_factor_traits<LhsCf>::conversion_ratio,
			typename traits::conversion_factor_traits<LhsCf>::dimension_type,
			typename traits::conversion_factor_traits<LhsCf>::pi_exponent_ratio, std::ratio<0>>;
		using DeltaUnit = unit<traits::strong_t<DeltaCf>, typename UnitTypeLhs::underlying_type, typename UnitTypeLhs::numerical_scale_type>;
		return DeltaUnit(lhs.raw() - UnitTypeLhs(rhs).raw());
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator-(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		// Apply any necessary scale factor to T using multiplication for lossless conversion
		// for non-scaled dimensionless units it's a no-op
		using CommonUnit        = decltype(lhs - rhs);
		using InverseCommonUnit = decltype(1 / CommonUnit(1));
		return CommonUnit(lhs.raw() - InverseCommonUnit(rhs).value());
	}

	/// Subtraction for ratio-like dimensionless units
	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr auto operator-(const U& lhs, T rhs) noexcept -> traits::replace_underlying_t<U, detail::floating_point_promotion_t<std::common_type_t<typename U::underlying_type, T>>>
	{
		using Under0 = std::common_type_t<typename U::underlying_type, T>;
		using Under  = detail::floating_point_promotion_t<Under0>;
		using Ret    = traits::replace_underlying_t<U, Under>;

		using R                        = typename U::conversion_factor::conversion_ratio;
		constexpr Under points_per_one = static_cast<Under>(R::den) / static_cast<Under>(R::num);

		const Under frac = static_cast<Under>(lhs.value()) - static_cast<Under>(rhs);
		return Ret(frac * points_per_one);
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr auto operator-(T lhs, const U& rhs) noexcept -> traits::replace_underlying_t<U, detail::floating_point_promotion_t<std::common_type_t<T, typename U::underlying_type>>>
	{
		using Under0 = std::common_type_t<T, typename U::underlying_type>;
		using Under  = detail::floating_point_promotion_t<Under0>;
		using Ret    = traits::replace_underlying_t<U, Under>;

		using R                        = typename U::conversion_factor::conversion_ratio;
		constexpr Under points_per_one = static_cast<Under>(R::den) / static_cast<Under>(R::num);

		const Under frac = static_cast<Under>(lhs) - static_cast<Under>(rhs.value());
		return Ret(frac * points_per_one);
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs> && !RatioDimensionlessUnitType<UnitTypeRhs>)
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
		-> detail::rewrap_to_named_t<unit<traits::strong_t<squared<typename traits::unit_traits<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>::conversion_factor>>,
			typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type>>
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
		-> detail::rewrap_to_named_t<unit<traits::strong_t<compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>>
	{
		using CompoundUnit     = decltype(lhs * rhs);
		using CommonUnderlying = typename CompoundUnit::underlying_type;
		return CompoundUnit(static_cast<CommonUnderlying>(lhs) * static_cast<CommonUnderlying>(rhs));
	}

	/// Multiplication by an ordinary dimensionless unit for unit types with a linear scale.
	template<DimensionedUnitType UnitTypeLhs, OrdinaryDimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(CommonUnit(lhs).raw() * static_cast<typename CommonUnit::underlying_type>(rhs));
	}

	/// Multiplication by a ratio-dimensionless unit for unit types with a linear scale.
	/// Treat ratio-dimensionless as a scalar (normalized fraction).
	template<DimensionedUnitType UnitTypeLhs, RatioDimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using Out = decltype(lhs * rhs);
		using U0  = std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using U   = detail::floating_point_promotion_t<U0>;

		// rhs.value() is normalized fraction (e.g. 200_pct -> 2.0, 50_ppb -> 50e-9)
		return Out(static_cast<U>(lhs.raw()) * static_cast<U>(rhs.value()));
	}

	/// Multiplication by an ordinary dimensionless unit for unit types with a linear scale.
	template<OrdinaryDimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(static_cast<typename CommonUnit::underlying_type>(lhs) * CommonUnit(rhs).raw());
	}

	/// ratio-dimensionless * dimensioned -> dimensioned (scalar multiply)
	template<RatioDimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator*(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using Out = decltype(lhs * rhs);
		using U0  = std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using U   = detail::floating_point_promotion_t<U0>;

		return Out(static_cast<U>(lhs.value()) * static_cast<U>(rhs.raw()));
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

	/// scalar * ratio-dimensionless -> base dimensionless (PROMOTED)
	template<RatioDimensionlessUnitType U, wpi::units::ArithmeticType T>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr wpi::units::dimensionless<wpi::units::detail::floating_point_promotion_t<std::common_type_t<T, typename U::underlying_type>>> operator*(T lhs, const U& rhs) noexcept
	{
		using Under0 = std::common_type_t<T, typename U::underlying_type>;
		using Under  = wpi::units::detail::floating_point_promotion_t<Under0>;

		// rhs converts to Under as normalized fraction (e.g. 50_pct -> 0.5)
		return wpi::units::dimensionless<Under>(static_cast<Under>(lhs) * static_cast<Under>(rhs));
	}

	/// ratio-dimensionless * scalar -> base dimensionless (PROMOTED)
	template<RatioDimensionlessUnitType U, wpi::units::ArithmeticType T>
		requires(wpi::units::traits::has_linear_scale_v<U>)
	constexpr wpi::units::dimensionless<wpi::units::detail::floating_point_promotion_t<std::common_type_t<typename U::underlying_type, T>>> operator*(const U& lhs, T rhs) noexcept
	{
		using Under0 = std::common_type_t<typename U::underlying_type, T>;
		using Under  = wpi::units::detail::floating_point_promotion_t<Under0>;

		return wpi::units::dimensionless<Under>(static_cast<Under>(lhs) * static_cast<Under>(rhs));
	}

	/// Multiplication by an arithmetic type for dimensionless unit types with a linear scale.
	template<DimensionlessUnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator*(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(lhs.raw() * rhs);
	}

	/// Multiplication by an arithmetic type for dimensionless unit types with a linear scale.
	template<DimensionlessUnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs> && !RatioDimensionlessUnitType<UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeRhs, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator*(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = decltype(lhs * rhs);
		return CommonUnit(lhs * rhs.raw());
	}

	/// Division for convertible unit types with a linear scale. @returns the lhs divided by rhs value, whose type is a
	/// dimensionless
	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(
			same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> && !RatioDimensionlessUnitType<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeRhs>)
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
		-> detail::rewrap_to_named_t<unit<traits::strong_t<compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>>
	{
		using CompoundUnit     = decltype(lhs / rhs);
		using CommonUnderlying = typename CompoundUnit::underlying_type;
		return CompoundUnit(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by an ordinary dimensionless unit for unit types with a linear scale
	template<DimensionedUnitType UnitTypeLhs, OrdinaryDimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>> operator/(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit       = decltype(lhs / rhs);
		using CommonUnderlying = typename CommonUnit::underlying_type;

		// Ordinary dimensionless is a true scalar
		return CommonUnit(CommonUnit(lhs).raw() / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by a ratio-dimensionless unit for unit types with a linear scale.
	/// Treat ratio-dimensionless as a scalar (normalized fraction).
	template<DimensionedUnitType UnitTypeLhs, RatioDimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr traits::replace_underlying_t<
		UnitTypeLhs,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>
	>
	operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using Out = decltype(lhs / rhs);
		using U0  = std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using U   = detail::floating_point_promotion_t<U0>;

		return Out(static_cast<U>(lhs.raw()) / static_cast<U>(rhs.value()));
	}

	/// Division of an ordinary dimensionless unit by a ratio-dimensionless unit.
	/// Produces inverse<ratio-dimensionless> (compound) and uses raw() for rhs.
	template<OrdinaryDimensionlessUnitType UnitTypeLhs, RatioDimensionlessUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> detail::rewrap_to_named_t<unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>>
	{
		using Out              = decltype(lhs / rhs);
		using CommonUnderlying = typename Out::underlying_type;

		// lhs is true scalar, rhs is points (not scalar fraction)
		return Out(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs.raw()));
	}

	/// Division of a dimensionless unit by a unit type with a linear scale
	template<OrdinaryDimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> && traits::is_dimensionless_unit_v<UnitTypeLhs>)
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> detail::rewrap_to_named_t<unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>>
	{
		using CommonUnit       = decltype(lhs / rhs);
		using CommonUnderlying = typename CommonUnit::underlying_type;
		return CommonUnit(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division of a ratio-dimensionless unit (pct/ppm/ppb/...) by a dimensioned unit.
	/// This MUST preserve the numerator ratio semantics (do NOT collapse to 1/unit).
	template<RatioDimensionlessUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<
			traits::strong_t<
				compound_conversion_factor<
					typename traits::unit_traits<UnitTypeLhs>::conversion_factor,
					inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>
				>
			>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>
		>
	{
		using Out              = decltype(lhs / rhs);
		using CommonUnderlying = typename Out::underlying_type;

		// numeric part: ppb points / years -> "ppb per year" numeric value
		// keep lhs as points (raw), keep rhs in its own units (raw)
		return Out(
			static_cast<CommonUnderlying>(lhs.raw()) / static_cast<CommonUnderlying>(rhs.raw()),
			linearized_value
		);
	}

	/// Division by a dimensionless for unit types with a linear scale
	template<UnitType UnitTypeLhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator/(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = decltype(lhs / rhs);
		return CommonUnit(CommonUnit(lhs).raw() / rhs);
	}

	/// Division of a dimensionless by a unit type with a linear scale
	template<UnitType UnitTypeRhs, ArithmeticType T>
		requires(traits::has_linear_scale_v<UnitTypeRhs> && !RatioDimensionlessUnitType<UnitTypeRhs>)
	constexpr auto operator/(T lhs, const UnitTypeRhs& rhs) noexcept
		-> detail::rewrap_to_named_t<unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>, std::common_type_t<T, typename UnitTypeRhs::underlying_type>>>
	{
		using InverseUnit      = decltype(lhs / rhs);
		using UnitConversion   = typename traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		using CommonUnit       = unit<UnitConversion, CommonUnderlying>;
		return InverseUnit(lhs / CommonUnit(rhs).raw());
	}

	/// Division of ratio-like dimensionless units with arithmetic types
	// U / scalar -> U   (percent points divided, still percent)
	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr traits::replace_underlying_t<U, std::common_type_t<typename U::underlying_type, T>> operator/(const U& lhs, T rhs) noexcept
	{
		using Out = traits::replace_underlying_t<U, std::common_type_t<typename U::underlying_type, T>>;
		return Out(Out(lhs).raw() / rhs);
	}

	// scalar / ratio-dimensionless -> dimensionless (normalized)
	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr wpi::units::dimensionless<detail::floating_point_promotion_t<std::common_type_t<T, typename U::underlying_type>>> operator/(T lhs, const U& rhs) noexcept
	{
		using CommonType   = std::common_type_t<T, typename U::underlying_type>;
		using PromotedType = detail::floating_point_promotion_t<CommonType>;

		// rhs.value() is normalized fraction (e.g. 50_pct -> 0.5)
		return wpi::units::dimensionless<PromotedType>(static_cast<PromotedType>(lhs) / static_cast<PromotedType>(rhs.value()));
	}

	// U / U -> dimensionless (normalized)
	template<RatioDimensionlessUnitType U1, RatioDimensionlessUnitType U2>
		requires(traits::has_linear_scale_v<U1, U2>)
	constexpr dimensionless<detail::floating_point_promotion_t<std::common_type_t<typename U1::underlying_type, typename U2::underlying_type>>> operator/(const U1& lhs, const U2& rhs) noexcept
	{
		using Under0 = std::common_type_t<typename U1::underlying_type, typename U2::underlying_type>;
		using Under  = detail::floating_point_promotion_t<Under0>;
		return dimensionless<Under>(static_cast<Under>(lhs.value()) / static_cast<Under>(rhs.value()));
	}

	/// Modulo for convertible unit types with a linear scale. @returns the lhs value modulo the rhs value, in
	/// their common (finer) unit.
	/// @note	The result is the `std::common_type` of the operands — the finer of the two units — not the
	///			lhs unit. Returning the lhs unit made the operator order-dependent: `meters % kilometers`
	///			compiled (finer lhs) but `kilometers % meters` did not (converting the finer common result
	///			back to the coarser lhs is lossy for an integer underlying, disabling the constructor). The
	///			common-unit result mirrors `fmod` and removes the asymmetry.
	template<DimensionedUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator%(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
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
		requires(traits::has_linear_scale_v<UnitTypeLhs> && !RatioDimensionlessUnitType<UnitTypeLhs>)
	constexpr traits::replace_underlying_t<UnitTypeLhs, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator%(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnit = decltype(lhs % rhs);
		return CommonUnit(CommonUnit(lhs).raw() % rhs);
	}

	// Modulos for ratio-like dimensionless units
	template<RatioDimensionlessUnitType U>
		requires(traits::has_linear_scale_v<U>)
	constexpr U operator%(const U& lhs, const U& rhs) noexcept
	{
		return U(lhs.raw() % rhs.raw());
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr U operator%(const U& lhs, T rhs) noexcept
	{
		return U(lhs.raw() % rhs);
	}

	template<RatioDimensionlessUnitType U, ArithmeticType T>
		requires(traits::has_linear_scale_v<U>)
	constexpr U operator%(T lhs, const U& rhs) noexcept
	{
		using Under = detail::floating_point_promotion_t<std::common_type_t<T, typename U::underlying_type>>;
		// If lhs is integral, keep integer modulo semantics
		if constexpr (std::is_integral_v<T> && std::is_integral_v<typename U::underlying_type>)
			return U(lhs % rhs.raw());
		else
			return U(static_cast<Under>(std::fmod(static_cast<Under>(lhs), static_cast<Under>(rhs.raw()))));
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
	constexpr auto pow(const UnitType& value) noexcept -> detail::rewrap_to_named_t<unit<traits::strong_t<typename wpi::units::detail::power_of_unit<power, typename wpi::units::traits::unit_traits<UnitType>::conversion_factor>::type>,
		detail::floating_point_promotion_t<typename wpi::units::traits::unit_traits<UnitType>::underlying_type>, linear_scale>>
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
		 * @param[in]   value value to linearize
		 * @returns		`std::pow(10, value / 10)`
		 */
		template<class T>
		static T linearize(const T value) noexcept
		{
			// A decibel value is stored through a base-10 logarithm, so an integral underlying type cannot
			// represent it: most decibel figures round to a wrong integer (3 dB stores as 0) and large ones
			// overflow. Asserted here, at the point a value is actually stored, so merely naming a
			// decibel-scale type for trait/overload resolution does not trip it.
			static_assert(std::is_floating_point_v<T>,
				"a decibel-scale unit requires a floating-point underlying type (an integral type cannot represent a logarithmic value)");
			return static_cast<T>(std::pow(10, value / 10));
		}

		/**
		 * @brief		returns `value` in dB
		 * @tparam		T	underlying type of unit
		 * @param[in]   value value to scale
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
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	template<class Underlying>
	std::ostream& operator<<(std::ostream& os, const decibels<Underlying>& obj)
	{
		os << obj.raw() << " dB";
		return os;
	}
#endif
	template<class Underlying>
	using dBi = decibels<Underlying>;

	// Register the name/abbreviation for the dimensionless decibel and its `_dB` literal. The reverse
	// named-class map is keyed on (conversion_factor, scale); the (dimensionless, decibel_scale) key
	// belongs to `decibels` alone (the power dB units use the watts/milliwatts factors), so the mapping
	// is unambiguous and the member name()/abbreviation() resolve through it.
	template<class Underlying>
	struct unit_name<decibels<Underlying>>
	{
		static constexpr const char* value = "decibels";
	};

	template<class Underlying>
	struct unit_abbreviation<decibels<Underlying>>
	{
		static constexpr const char* value = "dB";
	};

	namespace detail
	{
		::wpi::units::decibels<UNIT_LIB_DEFAULT_TYPE> named_class_of(
			typename ::wpi::units::decibels<>::conversion_factor*, typename ::wpi::units::decibels<>::numerical_scale_type*);
	}

#ifndef UNIT_NO_LITERAL_SUPPORT
	namespace literals
	{
		// only a floating-point literal: a decibel scale requires a floating-point underlying type
		constexpr decibels<double> operator""_dB(long double d) noexcept
		{
			return decibels<double>(static_cast<double>(d));
		}
	} // namespace literals
#endif

	//------------------------------
	//	DECIBEL ARITHMETIC
	//------------------------------

	/// Addition of two absolute decibel LEVELS (both dimensioned, same dimension — e.g. `dBW + dBW`) is ill-formed.
	/// A dimensioned decibel value is an absolute point on a logarithmic reference scale, exactly like an affine
	/// temperature: `dBW + dBW` is a point + point, which has no meaning (two 10 dBW sources are not a 20 dBW
	/// source). The defined operations are `level + gain -> level` (add a dimensionless dB gain), `gain + gain ->
	/// gain`, and `level - level -> gain`. To combine two independent power levels, add them in the linear domain
	/// (two equal powers sum to +3 dB), not by adding their dB numbers.
	template<DimensionedUnitType UnitTypeLhs, DimensionedUnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs> && traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept = delete;

	/// Addition of two dimensionless decibel GAINS (`dB + dB`). Both are relative ratios, so their dB numbers add
	/// (a linear multiplication of the ratios): 3 dB + 3 dB is 6 dB. The result is a dimensionless dB gain.
	template<DimensionlessUnitType UnitTypeLhs, DimensionlessUnitType UnitTypeRhs>
		requires(traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>)
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(CommonUnit(lhs).to_linearized() * CommonUnit(rhs).to_linearized(), linearized_value);
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
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> detail::rewrap_to_named_t<unit<traits::strong_t<inverse<typename traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>, decibel_scale>>
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
		using promoted = detail::floating_point_promotion_t<typename UnitType::underlying_type>;
		// std::modf splits the NORMALIZED value; the integral and fractional parts are already in the
		// quantity's own (normalized) units. Re-wrapping the fractional double through UnitType's
		// value constructor would re-apply the unit's scale (e.g. percent's 1/100) a second time, so the
		// fraction is returned as a plain dimensionless value and the integral part is converted back to
		// UnitType through its converting constructor.
		promoted intp;
		promoted fracpart = std::modf(x.template to<promoted>(), &intp);
		*intpart          = dimensionless<promoted>{intp};
		return dimensionless<promoted>{fracpart};
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
		-> detail::rewrap_to_named_t<unit<traits::strong_t<square_root<typename traits::unit_traits<UnitType>::conversion_factor>>, detail::floating_point_promotion_t<typename traits::unit_traits<UnitType>::underlying_type>>>
	{
		return decltype(wpi::units::sqrt(value))(sqrt(value.raw()));
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
		return CommonUnit(gcem::hypot(CommonUnit(x).raw(), CommonUnit(y).raw()));
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
	template<UnitType Unit>
	constexpr detail::floating_point_promotion_t<Unit> ceil(const Unit x) noexcept
	{
		return detail::floating_point_promotion_t<Unit>(std::ceil(x.raw()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Round down value
	 * @details		Rounds x downward, returning the largest integral value that is not greater than x.
	 * @param[in]	x	Unit value to round down.
	 * @returns		The value of x rounded downward.
	 */
	template<UnitType Unit>
	constexpr detail::floating_point_promotion_t<Unit> floor(const Unit x) noexcept
	{
		return detail::floating_point_promotion_t<Unit>(std::floor(x.raw()));
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
		return CommonUnit(gcem::fmod(CommonUnit(numer).raw(), CommonUnit(denom).raw()));
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
		return detail::floating_point_promotion_t<UnitType>(gcem::trunc(x.raw()));
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
		return detail::floating_point_promotion_t<UnitType>(gcem::round(x.raw()));
	}

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/// The rounding direction for a run-time lossy conversion to a coarser integral unit.
		enum class rounding_mode
		{
			toward_neg_infinity, ///< floor
			toward_pos_infinity, ///< ceil
			nearest_half_away,   ///< round (halfway cases away from zero)
			toward_zero          ///< trunc
		};

		/// Round an exact integer quotient `q = value*num/den` (truncated toward zero) with remainder
		/// `r = value*num % den` in the requested direction, entirely in integer arithmetic. `den` is positive (a
		/// `std::ratio` denominator); `r` carries the sign of the dividend. This is the integer counterpart of
		/// applying `std::floor`/`ceil`/`round`/`trunc` to `value*num/den`, without ever forming that ratio as a
		/// floating-point number — so it is exact at every magnitude, unlike a double-promoted rounding which loses
		/// the fractional part above 2^53.
		template<class Int>
		constexpr Int apply_integer_rounding(Int q, Int r, Int den, rounding_mode mode) noexcept
		{
			if (r == 0)
				return q; // exact — every mode agrees
			switch (mode)
			{
			case rounding_mode::toward_zero:
				return q; // integer division already truncated toward zero
			case rounding_mode::toward_neg_infinity:
				return r < 0 ? q - 1 : q; // a nonzero negative remainder means the true value is below q
			case rounding_mode::toward_pos_infinity:
				return r > 0 ? q + 1 : q; // a nonzero positive remainder means the true value is above q
			case rounding_mode::nearest_half_away:
			{
				// Halfway-away-from-zero: step away from zero when twice the remainder magnitude reaches den.
				const Int twiceRemainder = (r < 0 ? -r : r) * 2;
				if (twiceRemainder >= den)
					return r < 0 ? q - 1 : q + 1;
				return q;
			}
			}
			return q;
		}

		/// A run-time conversion to a coarser same-dimension unit that need not divide evenly, with the caller's
		/// rounding intent applied in the target unit. When both source and target are integral the divide and the
		/// rounding are done in exact integer arithmetic carried in a double-width intermediate (so the result is
		/// exact at every magnitude and the final store is an ordinary — implementation-defined — integer narrowing,
		/// matching `std::chrono::floor<To>`); when a floating-point underlying is involved the value is expressed in
		/// the target unit and the matching `std::` rounding is applied. The result is constructed from the
		/// already-linearized whole value, bypassing the lossless-conversion constructor that correctly rejects the
		/// implicit form. `To` and `From` are same-dimension units, `To` integral; `From` need not be.
		template<class To, class From>
		constexpr To rounded_unit_cast(const From& x, rounding_mode mode) noexcept
		{
			using ToRep   = typename To::underlying_type;
			using FromRep = typename From::underlying_type;

			if constexpr (std::is_integral_v<FromRep>)
			{
				// Exact integer path: value (in From units) * num / den, rounded on the integer remainder.
				using Ratio = std::ratio_divide<typename From::conversion_factor::conversion_ratio, typename To::conversion_factor::conversion_ratio>;
				const widest_signed_int value   = static_cast<widest_signed_int>(x.raw());
				const widest_signed_int product = value * static_cast<widest_signed_int>(Ratio::num);
				const widest_signed_int den     = static_cast<widest_signed_int>(Ratio::den);
				const widest_signed_int quotient  = product / den;
				const widest_signed_int remainder = product % den;
				const widest_signed_int rounded   = apply_integer_rounding(quotient, remainder, den, mode);
				return To(static_cast<ToRep>(rounded), linearized_value);
			}
			else
			{
				// A floating-point source: express in the target unit and apply the matching std:: rounding.
				using Promoted = unit<typename To::conversion_factor, floating_point_promotion_t<ToRep>, typename To::numerical_scale_type>;
				const auto inTarget = Promoted(x).to_linearized();
				const auto rounded  = mode == rounding_mode::toward_neg_infinity ? std::floor(inTarget)
									: mode == rounding_mode::toward_pos_infinity ? std::ceil(inTarget)
									: mode == rounding_mode::nearest_half_away   ? std::round(inTarget)
																				 : std::trunc(inTarget);
				return To(static_cast<ToRep>(rounded), linearized_value);
			}
		}

		/// Whether a run-time rounding conversion from `From` to `To` is meaningful: same dimension, an integral
		/// target, and a source not already losslessly convertible into the target (a lossless conversion needs no
		/// rounding and the ordinary converting constructor serves it). Gating the target-unit rounding overloads on
		/// this keeps them from shadowing the deduced-argument `round`/`floor`/`ceil`/`trunc` math functions.
		template<class To, class From>
		inline constexpr bool is_roundable_unit_conversion =
			traits::is_unit_v<To> && traits::is_unit_v<From> && same_dimension<From, To> &&
			std::is_integral_v<typename To::underlying_type> && !is_losslessly_convertible_unit<From, To>;
	} // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		UnitMath
	 * @brief		Convert to a coarser integral unit, rounding down (toward negative infinity).
	 * @details		The run-time counterpart to the compile-time exact narrowing conversion: where
	 *				`bytes<int> b = someRuntimeBits;` is correctly rejected (a run-time value need not be a whole
	 *				number of bytes), `wpi::units::floor<bytes<int>>(someRuntimeBits)` states the rounding intent and
	 *				yields the number of whole bytes at or below the value. Same shape as `std::chrono::floor<To>`.
	 * @tparam		To		the coarser integral target unit (e.g. `bytes<int>`).
	 * @tparam		From	the source unit (deduced), same dimension as `To`.
	 * @param[in]	x		the value to convert.
	 * @return		`x` in units of `To`, rounded toward negative infinity.
	 */
	template<class To, UnitType From>
		requires detail::is_roundable_unit_conversion<To, From>
	constexpr To floor(const From& x) noexcept
	{
		return detail::rounded_unit_cast<To>(x, detail::rounding_mode::toward_neg_infinity);
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Convert to a coarser integral unit, rounding up (toward positive infinity).
	 * @details		Run-time lossy conversion with explicit rounding intent; see `floor<To>`.
	 * @tparam		To		the coarser integral target unit.
	 * @tparam		From	the source unit (deduced), same dimension as `To`.
	 * @param[in]	x		the value to convert.
	 * @return		`x` in units of `To`, rounded toward positive infinity.
	 */
	template<class To, UnitType From>
		requires detail::is_roundable_unit_conversion<To, From>
	constexpr To ceil(const From& x) noexcept
	{
		return detail::rounded_unit_cast<To>(x, detail::rounding_mode::toward_pos_infinity);
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Convert to a coarser integral unit, rounding to nearest (halfway away from zero).
	 * @details		Run-time lossy conversion with explicit rounding intent; see `floor<To>`.
	 * @tparam		To		the coarser integral target unit.
	 * @tparam		From	the source unit (deduced), same dimension as `To`.
	 * @param[in]	x		the value to convert.
	 * @return		`x` in units of `To`, rounded to the nearest whole target unit.
	 */
	template<class To, UnitType From>
		requires detail::is_roundable_unit_conversion<To, From>
	constexpr To round(const From& x) noexcept
	{
		return detail::rounded_unit_cast<To>(x, detail::rounding_mode::nearest_half_away);
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Convert to a coarser integral unit, rounding toward zero.
	 * @details		Run-time lossy conversion with explicit rounding intent; see `floor<To>`.
	 * @tparam		To		the coarser integral target unit.
	 * @tparam		From	the source unit (deduced), same dimension as `To`.
	 * @param[in]	x		the value to convert.
	 * @return		`x` in units of `To`, rounded toward zero.
	 */
	template<class To, UnitType From>
		requires detail::is_roundable_unit_conversion<To, From>
	constexpr To trunc(const From& x) noexcept
	{
		return detail::rounded_unit_cast<To>(x, detail::rounding_mode::toward_zero);
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
		return detail::floating_point_promotion_t<UnitTypeLhs>(gcem::copysign(x.raw(), y.raw())); // no need for conversion to get the correct sign.
	}

	/// Overload to copy the sign from a raw double
	template<UnitType UnitTypeLhs, ArithmeticType T>
	constexpr detail::floating_point_promotion_t<UnitTypeLhs> copysign(const UnitTypeLhs x, const T& y) noexcept
	{
		return detail::floating_point_promotion_t<UnitTypeLhs>(gcem::copysign(x.raw(), y));
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
		return CommonUnit(std::fdim(CommonUnit(x).raw(), CommonUnit(y).raw()));
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
			return CommonUnit(std::fmax(CommonUnit(x).raw(), CommonUnit(y).raw()));
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
			return CommonUnit(std::fmin(CommonUnit(x).raw(), CommonUnit(y).raw()));
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
		return detail::floating_point_promotion_t<UnitType>(gcem::fabs(x.raw()));
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
		return UnitType(gcem::abs(x.raw()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Multiply-add
	 * @details		Returns x*y+z, computed with a single rounding via `std::fma` — preserving both the
	 *				accuracy and the performance contract of the underlying operation (a fused multiply-add
	 *				maps to one hardware instruction where available). The three operands may be expressed in
	 *				different units of their respective dimensions; each is reconciled to the result unit
	 *				within the single fused step so the multiply and the add share a consistent basis. The
	 *				result unit is the common type of the product `x*y` and the addend `z`.
	 * @param[in]	x	Value to be multiplied.
	 * @param[in]	y	Value to be multiplied.
	 * @param[in]	z	Value to be added.
	 * @returns		The result of x*y+z.
	 */
	template<UnitType UnitTypeLhs, UnitType UnitMultiply, UnitType UnitAdd>
		requires(traits::is_same_dimension_conversion_factor_v<
			compound_conversion_factor<typename traits::unit_traits<UnitTypeLhs>::conversion_factor, typename traits::unit_traits<UnitMultiply>::conversion_factor>,
			typename traits::unit_traits<UnitAdd>::conversion_factor>)
	constexpr auto fma(const UnitTypeLhs x, const UnitMultiply y, const UnitAdd z) noexcept
		-> std::common_type_t<decltype(detail::floating_point_promotion_t<UnitTypeLhs>(x) * detail::floating_point_promotion_t<UnitMultiply>(y)), UnitAdd>
	{
		using CommonUnit  = decltype(wpi::units::fma(x, y, z));
		using ProductUnit = decltype(detail::floating_point_promotion_t<UnitTypeLhs>(x) * detail::floating_point_promotion_t<UnitMultiply>(y));

		// Fold the product-unit -> result-unit conversion into one multiplicand (a compile-time-constant
		// scale), so a SINGLE std::fma performs the multiply and the add in the result's basis with one
		// rounding: x_raw * (y_raw * scale) + z_in_result. Feeding the raw operands directly (each in its
		// own unit) would combine inconsistent bases and give a wrong result.
		constexpr auto scale = CommonUnit(ProductUnit(1)).raw();
		return CommonUnit(std::fma(x.raw(), y.raw() * scale, CommonUnit(z).raw()));
	}

	//----------------------------
	//  NAN support
	//----------------------------

	template<UnitType UnitType>
	constexpr bool isnan(const UnitType& x) noexcept
	{
		return std::isnan(x.raw());
	}

	template<UnitType UnitType>
	constexpr bool isinf(const UnitType& x) noexcept
	{
		return std::isinf(x.raw());
	}

	template<UnitType UnitType>
	constexpr bool isfinite(const UnitType& x) noexcept
	{
		return std::isfinite(x.raw());
	}

	template<UnitType UnitType>
	constexpr bool isnormal(const UnitType& x) noexcept
	{
		return std::isnormal(x.raw());
	}

	template<UnitType UnitTypeLhs, UnitType UnitTypeRhs>
		requires(same_dimension<UnitTypeLhs, UnitTypeRhs>)
	constexpr bool isunordered(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		return std::isunordered(lhs.raw(), rhs.raw());
	}
} // end namespace wpi::units

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

// A NAMED unit is a class deriving from unit<...>; the exact-pattern specialization above does not match it, so its
// std::hash falls to the deleted primary. Inherit the base unit's hash (it operates on the linearized value, which the
// named unit has via its base) so a named unit is hashable exactly like the plain unit<...> it represents.
template<class Named>
	requires wpi::units::detail::is_named_unit_v<Named>
struct std::hash<Named> : std::hash<wpi::units::detail::unit_base_t<Named>>
{
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

	// A NAMED unit is a class deriving from unit<...>; the exact-pattern specialization above does not match it.
	// Return the NAMED type from each limit (the named unit converts from its base), so both the VALUE and the
	// reported TYPE match the named unit — generic code that asks for numeric_limits<meters<double>>::max() gets a
	// meters<double> back, not the plain unit<...> base.
	template<class Named>
		requires wpi::units::detail::is_named_unit_v<Named>
	struct numeric_limits<Named> : numeric_limits<wpi::units::detail::unit_base_t<Named>>
	{
	private:
		using Base = numeric_limits<wpi::units::detail::unit_base_t<Named>>;

	public:
		// Inherit every flag/member from the base (has_infinity, is_signed, digits, ...); only SHADOW the
		// value-returning statics to return the NAMED type (the named unit converts from its base), so both the value
		// and the reported type match the named unit.
		static constexpr Named min() { return Named(Base::min()); }
		static constexpr Named max() { return Named(Base::max()); }
		static constexpr Named lowest() { return Named(Base::lowest()); }
		static constexpr Named epsilon() { return Named(Base::epsilon()); }
		static constexpr Named round_error() { return Named(Base::round_error()); }
		static constexpr Named denorm_min() { return Named(Base::denorm_min()); }
		static constexpr Named infinity() { return Named(Base::infinity()); }
		static constexpr Named quiet_NaN() { return Named(Base::quiet_NaN()); }
		static constexpr Named signaling_NaN() { return Named(Base::signaling_NaN()); }
	};

	// These overloads accept ANY unit — including a NAMED unit, which is a class DERIVING from wpi::units::unit<...>.
	// Constraining on the wpi::units::UnitType concept (rather than an exact `unit<Cf,T,Ns>&` parameter) makes a named
	// unit an EXACT match, so it wins over <cmath>'s own generic isnan/isinf/... templates. With the exact-type
	// parameter, a named (derived) unit only bound via a derived->base conversion — a WORSE match than <cmath>'s
	// template — so on some standard libraries (MSVC) the generic <cmath> overload was selected and forwarded the
	// unit to fpclassify(), which has no unit overload (error C2665). raw() yields the arithmetic magnitude.
	template<wpi::units::UnitType U>
	constexpr bool isnan(U x)
	{
		return std::isnan(x.raw());
	}

	template<wpi::units::UnitType U>
	constexpr bool isinf(U x)
	{
		return std::isinf(x.raw());
	}

	template<wpi::units::UnitType U>
	constexpr bool isfinite(U x)
	{
		return std::isfinite(x.raw());
	}

	template<wpi::units::UnitType U>
	constexpr bool signbit(U x)
	{
		return gcem::signbit(x.raw());
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
//  std::format SUPPORT
//----------------------------------------------------------------------------------------------------------------------

#if defined(UNIT_LIB_ENABLE_FORMAT)

//----------------------------------------------------------------------------------------------------------------------
//      CLASS: std::formatter<wpi::units::unit<...>, char>
//----------------------------------------------------------------------------------------------------------------------
/// @brief      Formats a `wpi::units::unit` for `std::format`, `std::print`, `std::println`, and `std::format_to`,
///             with a units-aware mini-language.
/// @details    The value-spec (everything before an optional `%`) is delegated to the underlying arithmetic
///             type's `std::formatter`, so the full standard numeric grammar is supported (precision, width,
///             fill/align, sign, `#`, `0`, `L`, type, …). The unit-opts (after `%`) control the label form,
///             whether the value/unit are shown, and the separator. This support needs only `<format>`, not
///             iostream, so it is available even under `UNIT_LIB_DISABLE_IOSTREAM`.
///
///             Grammar: `{:` value-spec [ `%` unit-opts ] `}`, where unit-opts are, in any order:
///               - `a` own abbreviated label (default), `n` own full-name label — neither converts the value
///               - `b` convert the value and label to SI base units (`6 ft` → `1.8288 m`)
///               - `v` value only, `u` unit only
///               - `'`…`'` a separator literal between value and label (`''` none, `'_'`, `'\t'`, …)
/// @tparam     U    any unit type — the `unit<...>` template itself OR a named unit derived from it
///                  (e.g. `meters<double>`), matched through the `wpi::units::UnitType` concept so a named unit
///                  formats the same as its `unit<...>` base.
//----------------------------------------------------------------------------------------------------------------------
template<wpi::units::UnitType U>
struct std::formatter<U, char>
{
	using conversion_factor   = typename wpi::units::traits::unit_traits<U>::conversion_factor;
	using value_type          = typename wpi::units::traits::unit_traits<U>::underlying_type;
	using scale_type          = typename wpi::units::traits::unit_traits<U>::numerical_scale_type;
	using promoted_value_type = wpi::units::detail::floating_point_promotion_t<value_type>;

	// A named unit prints its stored value as-is, so its value formatter is the underlying type — integer
	// specs (d/x/b/…) then work for an integer-underlying unit. An unnamed unit is rendered in its base
	// unit, a conversion that is floating-point, so its value formatter is the promoted type.
	static constexpr bool renders_in_base_unit = wpi::units::detail::label_uses_base_unit<conversion_factor, value_type, scale_type>();
	using formatted_value_type                  = std::conditional_t<renders_in_base_unit, promoted_value_type, value_type>;

	// The %b flag base-converts a NAMED unit's value to SI, which is a floating-point result; it is emitted
	// through a promoted-type formatter. (For an unnamed unit the primary formatter is already promoted.)
	std::formatter<formatted_value_type, char> m_valueFormatter;        ///< delegate for the stored-value portion
	std::formatter<promoted_value_type, char>  m_baseFormatter;         ///< delegate for the %b base-converted value
	wpi::units::detail::unit_format_options         m_options;               ///< the parsed unit-opts
	bool                                       m_usesBaseFormatter = false;    ///< value-spec was parsed into m_baseFormatter (%b)

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: parse [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      Parses the format-spec: the value-spec is forwarded to the value formatter, then the
	///             unit-opts after `%` are decoded.
	/// @param[in,out]	ctx		the parse context, positioned at the start of the spec.
	/// @return     an iterator past the consumed spec (at the closing `}`).
	//----------------------------------------------------------------------------------------------------------------------
	constexpr auto parse(std::format_parse_context& ctx)
	{
		auto it  = ctx.begin();
		auto end = ctx.end();

		// The value-spec runs to the first '%' (or to the closing '}').
		auto valueSpecEnd = it;
		for (auto scan = it; scan != end && *scan != '}'; ++scan)
		{
			if (*scan == '%')
				break;
			valueSpecEnd = scan + 1;
		}

		// The %b flag (base-SI conversion) needs the promoted-type formatter; every other flag uses the
		// stored-type formatter. Determine which is in play by scanning the unit-opts for 'b' before
		// delegating the value-spec, so the value-spec is parsed into exactly the formatter that will emit
		// it (parsing an integer spec such as `d` into a floating-point formatter would wrongly reject it).
		m_usesBaseFormatter = false;
		for (auto scan = valueSpecEnd; scan != end && *scan != '}'; ++scan)
		{
			if (*scan == 'b')
			{
				m_usesBaseFormatter = true;
				break;
			}
		}

		// Delegate the value-spec to the chosen value formatter. Present it a parse context spanning only
		// the value-spec and require it consumed the whole thing.
		if (valueSpecEnd != it)
		{
			std::string_view valueSpec(it, valueSpecEnd);
			if (m_usesBaseFormatter)
			{
				std::format_parse_context baseCtx(valueSpec);
				if (m_baseFormatter.parse(baseCtx) != valueSpec.end())
					throw std::format_error("units: invalid value format-spec");
			}
			else
			{
				std::format_parse_context valueCtx(valueSpec);
				if (m_valueFormatter.parse(valueCtx) != valueSpec.end())
					throw std::format_error("units: invalid value format-spec");
			}
		}

		it = valueSpecEnd;

		// Unit-opts after '%'.
		if (it != end && *it == '%')
		{
			++it;
			bool sawForm = false;
			bool sawShow = false;
			while (it != end && *it != '}')
			{
				const char c = *it;
				if (c == 'a' || c == 'n' || c == 'b')
				{
					if (sawForm)
						throw std::format_error("units: duplicate label-form flag");
					sawForm        = true;
					m_options.form = (c == 'a') ? wpi::units::detail::label_form::abbreviation
					    : (c == 'n')            ? wpi::units::detail::label_form::name
					                            : wpi::units::detail::label_form::base;
					++it;
				}
				else if (c == 'v' || c == 'u')
				{
					if (sawShow)
						throw std::format_error("units: duplicate show flag");
					sawShow             = true;
					m_options.showValue = (c == 'v');
					m_options.showUnit  = (c == 'u');
					++it;
				}
				else if (c == '\'')
				{
					++it;    // opening quote
					std::string sep;
					bool        closed = false;
					while (it != end && *it != '}')
					{
						if (*it == '\\')
						{
							++it;
							if (it == end || *it == '}')
								throw std::format_error("units: dangling escape in separator");
							switch (*it)
							{
								case 't': sep.push_back('\t'); break;
								case 'n': sep.push_back('\n'); break;
								case '\\': sep.push_back('\\'); break;
								case '\'': sep.push_back('\''); break;
								default: sep.push_back(*it); break;
							}
							++it;
						}
						else if (*it == '\'')
						{
							closed = true;
							++it;    // closing quote
							break;
						}
						else
						{
							sep.push_back(*it);
							++it;
						}
					}
					if (!closed)
						throw std::format_error("units: unterminated separator literal");
					m_options.separator = std::move(sep);
					m_options.customSep = true;
				}
				else
				{
					throw std::format_error("units: unknown unit-format flag");
				}
			}
		}

		return it;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: format [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      Renders the unit: the numeric value (via the delegated value formatter) then the unit
	///             label, honoring the parsed show flags, label form, and separator.
	/// @param[in]	obj		the unit to format.
	/// @param[in,out]	ctx		the format context / output iterator.
	/// @return     the output iterator past the written characters.
	//----------------------------------------------------------------------------------------------------------------------
	template<class FormatContext>
	auto format(const U& obj, FormatContext& ctx) const
	{
		using base_unit_type = wpi::units::unit<wpi::units::conversion_factor<std::ratio<1>, typename conversion_factor::dimension_type>, promoted_value_type, scale_type>;

		// The value: an unnamed unit is always rendered in its base unit (its honest label is the
		// base-dimension list); the %b flag likewise base-converts a named unit's value so the base-SI
		// label is honest. Otherwise a named unit shows its stored value as-is (so integer specs work).
		formatted_value_type value{};
		promoted_value_type  baseValue{};
		if constexpr (renders_in_base_unit)
			value = base_unit_type(obj).raw();
		else
			value = static_cast<formatted_value_type>(obj.raw());
		if (m_options.form == wpi::units::detail::label_form::base)
			baseValue = base_unit_type(obj).raw();

		std::string label;
		if (m_options.showUnit)
		{
			switch (m_options.form)
			{
				case wpi::units::detail::label_form::name: label = wpi::units::detail::unit_label<wpi::units::detail::label_form::name>(obj); break;
				case wpi::units::detail::label_form::base: label = wpi::units::detail::unit_label<wpi::units::detail::label_form::base>(obj); break;
				case wpi::units::detail::label_form::abbreviation:
				default: label = wpi::units::detail::unit_label<wpi::units::detail::label_form::abbreviation>(obj); break;
			}
		}

		auto out = ctx.out();

		if (m_options.showValue)
		{
			if (m_usesBaseFormatter)
			{
				// %b: emit the base-SI value through the promoted-type formatter. For an unnamed unit the
				// value is already the promoted base value; for a named unit it is the base-converted one.
				const promoted_value_type emitted = renders_in_base_unit ? static_cast<promoted_value_type>(value) : baseValue;
				out                               = m_baseFormatter.format(emitted, ctx);
			}
			else
			{
				out = m_valueFormatter.format(value, ctx);
			}
		}

		if (m_options.showUnit && !label.empty())
		{
			// The core builders prefix a label with a single space (the default separator). Keep it when no
			// separator was overridden and a value precedes the label; otherwise strip it and, for a shown
			// value, emit the chosen separator.
			std::string_view labelView(label);
			const bool       hasLeadingSpace = !labelView.empty() && labelView.front() == ' ';

			if (m_options.showValue)
			{
				if (m_options.customSep)
				{
					if (hasLeadingSpace)
						labelView.remove_prefix(1);
					for (char ch : m_options.separator)
						*out++ = ch;
				}
			}
			else
			{
				if (hasLeadingSpace)
					labelView.remove_prefix(1);
			}

			for (char ch : labelView)
				*out++ = ch;
		}

		return out;
	}
};

#endif    // UNIT_LIB_ENABLE_FORMAT

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

//----------------------------------------------------------------------------------------------------------------------
//  TELEMETRY SUPPORT
//----------------------------------------------------------------------------------------------------------------------
namespace wpi::units {
#if (__has_include(<wpi/telemetry/TelemetryTable.hpp>) && !defined(UNIT_LIB_DISABLE_TELEMETRY)) || (__has_include(<wpi/tunables/Tunable.hpp>) && !defined(UNIT_LIB_DISABLE_TUNABLE))
	namespace detail
	{
		template<bool b>
		constexpr auto only_if(const auto& s)
		{
			if constexpr (b)
			{
				return s;
			}
			else
			{
				using namespace wpi::util::literals;
				return ""_ct_string;
			}
		}

		template<bool separator, class D, class E>
		consteval auto dim_to_string(const dim<D, E>&)
		{
			using namespace wpi::util::literals;
			return wpi::util::Concat(only_if<separator>(" "_ct_string),
				only_if<(E::num != 0)>(wpi::util::ct_string<char, std::char_traits<char>, std::string_view(D::abbreviation).size()>(std::string_view(D::abbreviation))),
				only_if<(E::num != 0 && E::num != 1)>("^"_ct_string), only_if<(E::num != 0 && E::num != 1)>(wpi::util::NumToCtString<E::num>()), only_if<(E::den != 1)>("/"_ct_string),
				only_if<(E::den != 1)>(wpi::util::NumToCtString<E::den>()));
		}
		template<class Dim, class... Dims>
		consteval auto dims_to_string(const dimension_t<Dim, Dims...>&)
		{
			using namespace wpi::util::literals;
			return wpi::util::Concat(dim_to_string<false>(Dim{}), dim_to_string<true>(Dims{})...);
		}
		// For dimensionless
		consteval auto dims_to_string(const dimension_t<>&)
		{
			using namespace wpi::util::literals;
			return ""_ct_string;
		}
		template<UnitType Units>
		consteval auto ComplexAbbrev()
		{
			using conversion_factor = typename traits::unit_traits<Units>::conversion_factor;
			using DimType           = traits::dimension_of_t<conversion_factor>;
			return dims_to_string(DimType{});
		}
	} // namespace detail
#endif

#if __has_include(<wpi/telemetry/TelemetryTable.hpp>) && !defined(UNIT_LIB_DISABLE_TELEMETRY)
	template<UnitType Unit>
	inline void LogValueTo(wpi::telemetry::TelemetryTable& table, std::string_view n, const Unit& value)
	{
		using BaseUnits      = unit<conversion_factor<std::ratio<1>, traits::dimension_of_t<Unit>>>;
		using UnderlyingType = typename traits::unit_traits<Unit>::underlying_type;
		using namespace wpi::util::literals;
		auto unitJson = wpi::util::Concat("\""_ct_string, detail::ComplexAbbrev<BaseUnits>(), "\""_ct_string);
		table.SetProperty(n, "unit", unitJson);
		table.Log(n, BaseUnits(value).template to<UnderlyingType>());
	}
#endif
} // namespace wpi::units

#ifndef UNIT_NO_LITERAL_SUPPORT
namespace wpi::units::literals {}
using namespace wpi::units::literals;
#endif

#endif // UNIT_CORE_H
