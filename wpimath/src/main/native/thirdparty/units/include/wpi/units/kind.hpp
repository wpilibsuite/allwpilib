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
/// @file	units/kind.h
/// @brief	Opt-in affine wrappers: `wpi::units::absolute<U>` (a point on a scale, carries the unit's datum) and
///			`wpi::units::delta<U>` (an offset-free amount). Include this header only where the point-vs-amount
///			distinction should be enforced in the type; plain-unit code never sees these.
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef UNIT_KIND_H
#define UNIT_KIND_H

#include <wpi/units/core.hpp>

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <string>

namespace wpi::units
{
	//----------------------------------------------------------------------------------------------------------------------
	//	AFFINE POINT / DELTA WRAPPERS
	//----------------------------------------------------------------------------------------------------------------------
	// Opt-in wrappers that make the point-vs-amount distinction explicit in the type. `absolute<U>` is a POINT on
	// a scale (it carries the unit's datum — 0 degC is 273.15 K); `delta<U>` is an AMOUNT (offset-free — a 10 degC
	// delta is a 10 K delta). Reach for these where the distinction matters (temperatures, epochs vs durations,
	// absolute vs gauge pressure, positions vs displacements); for a non-affine unit the datum is zero, so the two
	// coincide numerically. The type algebra:
	//   absolute - absolute -> delta      (the datum offsets cancel)
	//   absolute +/- delta   -> absolute  (move the point by a relative amount)
	//   delta +/- delta      -> delta
	//   delta * / scalar     -> delta
	//   absolute + absolute  -> ill-formed
	//
	// The wrappers exist only where <units/kind.h> is included, so a user's own `absolute`/`delta` names are
	// undisturbed otherwise. They live in `inline namespace affine`, so `wpi::units::absolute` and
	// `wpi::units::affine::absolute` name the same type; the `affine::` qualifier disambiguates when needed.

	namespace detail
	{
		/// The offset-free counterpart of a unit: same dimension, scale, and pi factor, but translation
		/// stripped. Converting a `delta` between units uses THIS (scale only, no datum), so a temperature
		/// difference converts by degree size, not as an absolute point.
		template<UnitType U>
		using delta_unit_t = unit<traits::strong_t<conversion_factor<typename traits::conversion_factor_traits<typename traits::unit_traits<U>::conversion_factor>::conversion_ratio,
											 typename traits::conversion_factor_traits<typename traits::unit_traits<U>::conversion_factor>::dimension_type,
											 typename traits::conversion_factor_traits<typename traits::unit_traits<U>::conversion_factor>::pi_exponent_ratio, std::ratio<0>>>,
			typename traits::unit_traits<U>::underlying_type, typename traits::unit_traits<U>::numerical_scale_type>;

		/// The result unit of a wrapper operator that keeps the LHS UNIT (the "LHS-unit tie-break"): the value
		/// stays expressed in `U`'s unit so `.value()` reads intuitively (`absolute<celsius> - absolute<fahrenheit>`
		/// is a difference in CELSIUS-degrees, not a common sub-unit). The one adjustment is to the UNDERLYING
		/// type: keeping `U`'s underlying would NARROW when the RHS unit `V` cannot convert into `U` losslessly
		/// (a coarse integer LHS that cannot hold the RHS), so in that case the underlying is promoted to floating
		/// point. The result UNIT is always `U`'s unit; only its underlying may widen.
		///
		/// This is the wrapper counterpart of `lhs_result_unit_t` (which the plain-unit `operator+`/`operator-`
		/// use). Both keep the LHS unit on the lossless path; they diverge only on the coarse-integer-LHS fallback:
		/// the plain operators reconcile to the common (finest) unit, the wrappers hold the LHS unit and widen only
		/// the underlying (a point keeps its datum in its own unit).
		///
		/// A POINT's lossless test is AFFINE-aware: `is_losslessly_convertible_unit` examines only the conversion
		/// RATIO, so it reports a datum-differing pair whose ratio happens to be 1 (e.g. kelvin↔celsius, ratio 1,
		/// datum 273.15) as lossless — but converting an integer point across that datum truncates the fractional
		/// offset (273 K → celsius<int> truncates −0.15 → 0). The datum-translation ratios must therefore ALSO match
		/// for an integer point conversion to be lossless; otherwise the underlying is promoted so the offset
		/// survives. This applies ONLY to `absolute` (a point carries its datum); a `delta` is offset-free.
		template<UnitType From, UnitType To>
		inline constexpr bool is_losslessly_point_convertible_unit =
			is_losslessly_convertible_unit<From, To> &&
			(std::is_floating_point_v<typename traits::unit_traits<To>::underlying_type> ||
				std::ratio_equal_v<typename traits::conversion_factor_traits<typename traits::unit_traits<From>::conversion_factor>::translation_ratio,
					typename traits::conversion_factor_traits<typename traits::unit_traits<To>::conversion_factor>::translation_ratio>);

		/// The result unit of an `absolute` (point) operator: keep the LHS unit, promote the underlying only when a
		/// point conversion of the RHS into it would be lossy (ratio OR datum). See the LHS-unit tie-break above.
		template<UnitType U, UnitType V>
		using absolute_result_unit_t = std::conditional_t<is_losslessly_point_convertible_unit<V, U>, U, traits::replace_underlying_t<U, floating_point_promotion_t<typename traits::unit_traits<U>::underlying_type>>>;

		/// The result unit of a `delta` (amount) operator: keep the LHS unit, promote the underlying only when the
		/// RHS amount cannot convert into it losslessly. A delta conversion is SCALE-ONLY (the datum is never
		/// applied), so losslessness is judged on the offset-free `delta_unit_t` — the datum translation is
		/// irrelevant, and a same-ratio pair (celsius↔kelvin deltas) stays integral rather than needlessly promoting.
		template<UnitType U, UnitType V>
		using delta_result_unit_t = std::conditional_t<is_losslessly_convertible_unit<delta_unit_t<V>, delta_unit_t<U>>, U,
			traits::replace_underlying_t<U, floating_point_promotion_t<typename traits::unit_traits<U>::underlying_type>>>;
	} // namespace detail

	inline namespace affine
	{
		template<UnitType U>
		class absolute;
		template<UnitType U>
		class delta;
		template<std::size_t N>
		struct fixed_string;
		template<fixed_string Tag, UnitType U>
		class basic_kind;

		/** @cond */ // DOXYGEN IGNORE
		namespace wrap_detail
		{
			// `to<Target>()` on every wrapper follows ONE rule: the TARGET type decides the result. A plain-unit
			// target unwraps to that plain unit (applying the wrapper's own conversion rule); a WRAPPER target keeps
			// that wrapper. These detect whether a `to<>` target is a wrapper (so the method returns the wrapper) or
			// a plain unit (so it unwraps). Declared before the wrappers so their `to<>` can use them.
			template<class T>
			inline constexpr bool is_wrapper = false;
			template<UnitType U>
			inline constexpr bool is_wrapper<absolute<U>> = true;
			template<UnitType U>
			inline constexpr bool is_wrapper<delta<U>> = true;
			template<fixed_string Tag, UnitType U>
			inline constexpr bool is_wrapper<basic_kind<Tag, U>> = true;

			/// The wrapped unit type of a wrapper target (its `unit_type`), used to convert into before re-wrapping.
			template<class Wrapper>
			using wrapper_unit_t = typename Wrapper::unit_type;

			/// The wrapped plain unit of a wrapper, for implementation use inside the free operators/formatters —
			/// the same value the public `to<PlainUnit>()` yields for the wrapper's own unit, expressed once here so
			/// the operators never spell the unwrap out. (A point's datum is intrinsic to its unit, so unwrapping to
			/// its OWN unit is value-preserving — this is not a datum conversion.)
			template<class Wrapper>
			constexpr auto unwrap(const Wrapper& w) noexcept
			{
				return w.template to<typename Wrapper::unit_type>();
			}

			/// A `false` that DEPENDS on template parameters, so a `static_assert(dependent_false<...>)` in a
			/// selected catch-all overload fires ONLY when that overload is instantiated (never eagerly).
			template<fixed_string...>
			inline constexpr bool dependent_false = false;
			template<class...>
			inline constexpr bool dependent_false_t = false;

			/// A three-way ordering of two plain units built from their own relational operators, so a wrapper's
			/// `<=>` inherits the core unit's value-based (signedness-safe) comparison. `unit` exposes `<`/`==`, not
			/// a spaceship, so the ordering is synthesized; an incomparable (NaN) pair is `unordered`.
			template<class LhsUnit, class RhsUnit>
			constexpr std::partial_ordering order(const LhsUnit& lhs, const RhsUnit& rhs) noexcept
			{
				if (lhs < rhs)
					return std::partial_ordering::less;
				if (rhs < lhs)
					return std::partial_ordering::greater;
				if (lhs == rhs)
					return std::partial_ordering::equivalent;
				return std::partial_ordering::unordered;
			}
		} // namespace wrap_detail
		/** @endcond */ // END DOXYGEN IGNORE
	} // inline namespace affine (forward declarations only)

	// The wrapper traits + concepts are defined here — after the forward declarations, before the class bodies — so
	// each wrapper's `to<>` overloads can constrain on `AbsoluteType`/`DeltaType`/`KindType`.
	namespace traits
	{
		/** @cond */ // DOXYGEN IGNORE
		namespace detail
		{
			template<class T>
			struct is_absolute_impl : std::false_type
			{
			};
			template<UnitType U>
			struct is_absolute_impl<wpi::units::affine::absolute<U>> : std::true_type
			{
			};

			template<class T>
			struct is_delta_impl : std::false_type
			{
			};
			template<UnitType U>
			struct is_delta_impl<wpi::units::affine::delta<U>> : std::true_type
			{
			};

			template<class T>
			struct is_kind_impl : std::false_type
			{
			};
			template<wpi::units::affine::fixed_string Tag, UnitType U>
			struct is_kind_impl<wpi::units::affine::basic_kind<Tag, U>> : std::true_type
			{
			};
		} // namespace detail
		/** @endcond */ // END DOXYGEN IGNORE

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether `T` is an `absolute<U>` point wrapper.
		 * @details		`true` for any `absolute<U>`, `false` for a `delta<U>`, a plain unit, or a non-unit type.
		 */
		template<class T>
		struct is_absolute : detail::is_absolute_impl<std::remove_cv_t<T>>
		{
		};

		template<class T>
		inline constexpr bool is_absolute_v = is_absolute<T>::value;

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether `T` is a `delta<U>` amount wrapper.
		 * @details		`true` for any `delta<U>`, `false` for an `absolute<U>`, a plain unit, or a non-unit type.
		 */
		template<class T>
		struct is_delta : detail::is_delta_impl<std::remove_cv_t<T>>
		{
		};

		template<class T>
		inline constexpr bool is_delta_v = is_delta<T>::value;

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether `T` is a string-tagged `kind<Tag, U>`.
		 * @details		`true` for any `kind<Tag, U>`, `false` for an `absolute`/`delta`, a plain unit, or a non-unit.
		 */
		template<class T>
		struct is_kind : detail::is_kind_impl<std::remove_cv_t<T>>
		{
		};

		template<class T>
		inline constexpr bool is_kind_v = is_kind<T>::value;
	} // namespace traits

	/**
	 * @ingroup		Concepts
	 * @brief		Concept satisfied by any `absolute<U>` point wrapper.
	 */
	template<class T>
	concept AbsoluteType = traits::is_absolute_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept satisfied by any `delta<U>` amount wrapper.
	 */
	template<class T>
	concept DeltaType = traits::is_delta_v<T>;

	/**
	 * @ingroup		Concepts
	 * @brief		Concept satisfied by any string-tagged `kind<Tag, U>`.
	 */
	template<class T>
	concept KindType = traits::is_kind_v<T>;

	inline namespace affine
	{
		//	----------------------------------------------------------------------------
		//	CLASS		absolute
		//  ----------------------------------------------------------------------------
		///	@brief		A point on a (possibly affine) scale — carries the unit's datum.
		///	@tparam		U	the wrapped unit type.
		//  ----------------------------------------------------------------------------
		template<UnitType U>
		class absolute
		{
		public:
			using unit_type       = U;                                               ///< the wrapped unit type
			using underlying_type = typename traits::unit_traits<U>::underlying_type; ///< the wrapped unit's numeric type

			constexpr absolute() noexcept = default;
			constexpr explicit absolute(const U& point) noexcept : m_point(point) {}

			/// Construct a point directly from its underlying numeric value (in `U`'s own unit).
			constexpr explicit absolute(underlying_type value) noexcept : m_point(value) {}

			/// The point's numeric value in its own unit.
			constexpr auto value() const noexcept { return m_point.value(); }
			/// The point's raw (linearized) value.
			constexpr auto raw() const noexcept { return m_point.raw(); }
			/// The point's linearized value (as the wrapped unit's `to_linearized()`).
			constexpr auto to_linearized() const noexcept { return m_point.to_linearized(); }
			/// The wrapped unit's name/abbreviation (a point does not rename the unit).
			[[nodiscard]] constexpr const char* name() const noexcept { return m_point.name(); }
			[[nodiscard]] constexpr const char* abbreviation() const noexcept { return m_point.abbreviation(); }

			/// Express this point's value as a plain arithmetic type — the numeric value in its own unit
			/// (`absolute<celsius<double>>(20).to<int>()` is `20`), matching the wrapped unit's `to<Arithmetic>()`.
			template<ArithmeticType Arithmetic>
			constexpr Arithmetic to() const noexcept
			{
				return m_point.template to<Arithmetic>();
			}

			/// Express this point as a PLAIN unit of the same dimension — unwraps, applying the datum offset
			/// (`absolute<celsius>(0).to<kelvin<double>>()` is `273.15 K`; `.to<celsius<double>>()` is the plain
			/// wrapped point). A plain-unit target is NOT a wrapper, so this overload owns the unwrap path.
			template<UnitType PlainTarget>
				requires traits::is_same_dimension_unit_v<U, PlainTarget>
			constexpr PlainTarget to() const noexcept
			{
				return PlainTarget(m_point); // the datum participates in the unit conversion
			}

			/// Express this point as another `absolute<V>` — stays a point (the datum is applied as it re-wraps).
			/// Only an `absolute` target is a viable wrapper here; a `delta<V>`/`kind<...>` target has no matching
			/// overload (a point cannot become an amount or a tagged kind through `to`).
			template<AbsoluteType WrapperTarget>
				requires traits::is_same_dimension_unit_v<U, typename WrapperTarget::unit_type>
			constexpr WrapperTarget to() const noexcept
			{
				return WrapperTarget(typename WrapperTarget::unit_type(m_point));
			}

		private:
			U m_point{}; ///< the wrapped point
		};

		//	----------------------------------------------------------------------------
		//	CLASS		delta
		//  ----------------------------------------------------------------------------
		///	@brief		An amount of a quantity — offset-free (no datum).
		///	@tparam		U	the wrapped unit type (its datum is ignored; only its scale matters).
		//  ----------------------------------------------------------------------------
		template<UnitType U>
		class delta
		{
		public:
			using unit_type       = U;                                               ///< the wrapped unit type
			using underlying_type = typename traits::unit_traits<U>::underlying_type; ///< the wrapped unit's numeric type

			constexpr delta() noexcept = default;
			constexpr explicit delta(const U& amount) noexcept : m_amount(amount) {}

			/// Construct an amount directly from its underlying numeric value (in `U`'s own unit).
			constexpr explicit delta(underlying_type value) noexcept : m_amount(value) {}

			/// The amount's numeric value in its own unit.
			constexpr auto value() const noexcept { return m_amount.value(); }
			/// The amount's raw (linearized) value.
			constexpr auto raw() const noexcept { return m_amount.raw(); }
			/// The amount's linearized value (as the wrapped unit's `to_linearized()`).
			constexpr auto to_linearized() const noexcept { return m_amount.to_linearized(); }
			/// The wrapped unit's name/abbreviation (an amount does not rename the unit).
			[[nodiscard]] constexpr const char* name() const noexcept { return m_amount.name(); }
			[[nodiscard]] constexpr const char* abbreviation() const noexcept { return m_amount.abbreviation(); }

			/// Express this amount's value as a plain arithmetic type — the numeric value in its own unit, matching
			/// the wrapped unit's `to<Arithmetic>()`.
			template<ArithmeticType Arithmetic>
			constexpr Arithmetic to() const noexcept
			{
				return m_amount.template to<Arithmetic>();
			}

			/// Express this amount as a PLAIN unit of the same dimension — unwraps, SCALE ONLY (the datum is never
			/// applied): a 10 degC delta is an 18 degF delta, never an absolute 50 degF. `.to<celsius<double>>()` is
			/// the plain wrapped amount. A conversion into a coarser integer unit (that would truncate) is rejected
			/// with a clear message rather than template-substitution spew; convert to a floating-point delta first
			/// if the truncation is intended.
			template<UnitType PlainTarget>
				requires traits::is_same_dimension_unit_v<U, PlainTarget>
			constexpr PlainTarget to() const noexcept
			{
				static_assert(wpi::units::detail::is_losslessly_convertible_unit<wpi::units::detail::delta_unit_t<U>, wpi::units::detail::delta_unit_t<PlainTarget>>,
					"wpi::units::delta::to<PlainTarget>(): converting this delta into PlainTarget would truncate an "
					"integer value; convert to a floating-point delta first (e.g. delta<meters<double>>) if intended.");
				return PlainTarget(scaled_to<PlainTarget>());
			}

			/// Express this amount as another `delta<V>` — stays an amount (scale-only). Only a `delta` target is a
			/// viable wrapper here; an `absolute<V>`/`kind<...>` target has no matching overload (an amount cannot
			/// become a point or a tagged kind through `to`).
			template<DeltaType WrapperTarget>
				requires traits::is_same_dimension_unit_v<U, typename WrapperTarget::unit_type>
			constexpr WrapperTarget to() const noexcept
			{
				using V = typename WrapperTarget::unit_type;
				static_assert(wpi::units::detail::is_losslessly_convertible_unit<wpi::units::detail::delta_unit_t<U>, wpi::units::detail::delta_unit_t<V>>,
					"wpi::units::delta::to<delta<V>>(): converting this delta into V would truncate an integer value; "
					"convert to a floating-point delta first (e.g. delta<meters<double>>) if intended.");
				return WrapperTarget(scaled_to<V>());
			}

		private:
			U m_amount{}; ///< the wrapped amount

			/// The wrapped amount reinterpreted into unit `V` by SCALE ONLY (offset-free counterparts on both sides).
			template<UnitType V>
			constexpr V scaled_to() const noexcept
			{
				const wpi::units::detail::delta_unit_t<U> here(m_amount.raw());
				const wpi::units::detail::delta_unit_t<V> there(here); // scale-only conversion (no translation on either)
				return V(there.raw());
			}
		};

		//	----------------------------------------------------------------------------
		//	CLASS		fixed_string
		//  ----------------------------------------------------------------------------
		///	@brief		A compile-time string usable as a non-type template parameter (the tag of a `kind`/`of`).
		///	@tparam		N	the length including the terminating null (deduced from the string literal).
		///	@details	Structural (public array), so `kind<"radial", U>` names a distinct type per spelling and two
		///				identical spellings name the SAME type. Comparison works ACROSS lengths — a `"radial"` and a
		///				`"straight"` (different `N`) compare unequal rather than failing to compile, so a tag-mismatch
		///				constraint (`TagL == TagR`) is well-formed for any two tags.
		//  ----------------------------------------------------------------------------
		template<std::size_t N>
		struct fixed_string
		{
			char value[N]{}; ///< the stored characters (including the terminating null)

			constexpr fixed_string(const char (&str)[N]) noexcept { std::copy_n(str, N, value); }

			/// Compare two tags of possibly-different length: unequal if the lengths differ, else element-wise.
			template<std::size_t M>
			constexpr bool operator==(const fixed_string<M>& rhs) const noexcept
			{
				if constexpr (N != M)
					return false;
				else
					return std::equal(value, value + N, rhs.value);
			}
		};
		template<std::size_t N>
		fixed_string(const char (&)[N]) -> fixed_string<N>;

		//	----------------------------------------------------------------------------
		//	CLASS		kind
		//  ----------------------------------------------------------------------------
		///	@brief		A quantity distinguished by a string TAG — a "kind of quantity" that shares a unit and a
		///				dimension with others but is semantically distinct (radial vs. straight-line distance, torque
		///				vs. energy). Two DIFFERENT tags never interoperate; the same tag does.
		///	@tparam		Tag	a `fixed_string` naming the kind (e.g. `"radial"`).
		///	@tparam		U	the wrapped unit type.
		///	@details	Users spell it `wpi::units::kind<Tag, U>` (the top-level alias); this is the underlying class the
		///				alias resolves to. A kind carries no datum; it wraps a plain magnitude and converts by scale
		///				like the unit it wraps, keeping its tag. Cross to the plain unit only explicitly, via
		///				`to<PlainUnit>()`.
		//  ----------------------------------------------------------------------------
		template<fixed_string Tag, UnitType U>
		class basic_kind
		{
		public:
			using unit_type       = U;                                               ///< the wrapped unit type
			using underlying_type = typename traits::unit_traits<U>::underlying_type; ///< the wrapped unit's numeric type

			constexpr basic_kind() noexcept = default;

			/// A plain unit converts INTO a kind by copy-initialization or assignment — the intent (this value IS a
			/// `Tag` kind) is explicit at the assignment site, so this ctor is non-`explicit`. What is NOT allowed is
			/// MIXING a plain unit with a kind in arithmetic (`aKind + plainUnit`); that stays ill-formed, so a value
			/// only ever enters a kind where you name it, never silently mid-expression.
			constexpr basic_kind(const U& value) noexcept : m_value(value) {}

			/// Construct from the underlying numeric value (in `U`'s own unit) — explicit, since a bare number
			/// carries no unit intent.
			constexpr explicit basic_kind(underlying_type value) noexcept : m_value(value) {}

			/// Assign a plain unit into this kind (same clear-intent conversion as copy-initialization).
			constexpr basic_kind& operator=(const U& value) noexcept
			{
				m_value = value;
				return *this;
			}

			/// The tag of this kind.
			static constexpr auto tag() noexcept { return Tag; }
			/// The numeric value in its own unit.
			constexpr auto value() const noexcept { return m_value.value(); }
			/// The raw (linearized) value.
			constexpr auto raw() const noexcept { return m_value.raw(); }
			/// The linearized value (as the wrapped unit's `to_linearized()`).
			constexpr auto to_linearized() const noexcept { return m_value.to_linearized(); }
			/// The abbreviation is the wrapped unit's (a kind does not change the abbreviation — `"m"` for a
			/// `kind<"radial", meters<double>>`).
			[[nodiscard]] constexpr const char* abbreviation() const noexcept { return m_value.abbreviation(); }
			/// The name is the tag followed by the wrapped unit's name — e.g. `"radial meters"`.
			[[nodiscard]] std::string name() const { return std::string(Tag.value).append(" ").append(m_value.name()); }

			/// Express this kind's value as a plain arithmetic type — the numeric value in its own unit, matching
			/// the wrapped unit's `to<Arithmetic>()`.
			template<ArithmeticType Arithmetic>
			constexpr Arithmetic to() const noexcept
			{
				return m_value.template to<Arithmetic>();
			}

			/// Express this kind as a PLAIN unit of the same dimension — unwraps (dropping the tag), scale-only.
			/// `radial.to<feet<double>>()` is plain `feet`; `.to<meters<double>>()` is the plain wrapped value.
			template<UnitType PlainTarget>
				requires traits::is_same_dimension_unit_v<U, PlainTarget>
			constexpr PlainTarget to() const noexcept
			{
				return PlainTarget(m_value);
			}

			/// Express this kind as another `kind<Tag, V>` — the tag is KEPT (radial metres → radial feet). Only a
			/// SAME-tag kind target is viable; a different-tag `kind` target has no matching overload (converting a
			/// radial distance into a straight-line distance is meaningless).
			template<KindType WrapperTarget>
				requires(WrapperTarget::tag() == Tag && traits::is_same_dimension_unit_v<U, typename WrapperTarget::unit_type>)
			constexpr WrapperTarget to() const noexcept
			{
				return WrapperTarget(typename WrapperTarget::unit_type(m_value));
			}

		private:
			U m_value{}; ///< the wrapped value
		};

		//----------------------------------
		//	ABSOLUTE / DELTA OPERATORS
		//----------------------------------

		// The wrapper operators keep the LHS UNIT (the "LHS-unit tie-break") so `.value()` reads in the left
		// operand's unit — a point difference `100 degC - 32 degF` is 100 celsius-degrees. Keeping the LHS unit
		// would narrow when the LHS is a coarse integer that cannot hold the RHS losslessly, so in that case the
		// underlying is promoted to floating point (an `absolute`
		// operator uses `wpi::units::detail::absolute_result_unit_t`, datum-aware; a `delta` operator uses
		// `wpi::units::detail::delta_result_unit_t`, scale-only); the result UNIT stays the LHS unit, only the underlying
		// widens. This keeps `absolute<kilometers<int>> - absolute<meters<int>>` well-formed (a km delta with a
		// promoted underlying) instead of hard-erroring in the conversion.

		/// point - point -> delta, kept in the LHS unit (the datum offsets cancel).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator-(const absolute<U>& lhs, const absolute<V>& rhs) noexcept
		{
			// The result unit is the LHS unit (underlying promoted only if keeping U's underlying would narrow the
			// RHS). The rhs point is converted into that unit AFFINELY (offset applied), then subtracted; the datum
			// cancels, leaving an offset-free delta expressed in the LHS unit.
			using R = wpi::units::detail::absolute_result_unit_t<U, V>;
			return delta<R>(R(R(wrap_detail::unwrap(lhs)).raw() - R(wrap_detail::unwrap(rhs)).raw()));
		}

		/// point + delta -> point (move the point up by a relative amount), kept in the LHS point's unit. A point
		/// has a datum, and reconciling two affine frames to a common unit mangles the datum; only the delta
		/// (offset-free) is converted, scale-only, into the point's unit.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator+(const absolute<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R = wpi::units::detail::absolute_result_unit_t<U, V>;
			return absolute<R>(R(R(wrap_detail::unwrap(lhs)).raw() + rhs.template to<R>().raw()));
		}

		/// delta + point -> point (commutative form). The result keeps the POINT's unit (the point is the affine
		/// operand), matching `point + delta`.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<V, U>
		constexpr auto operator+(const delta<V>& lhs, const absolute<U>& rhs) noexcept
		{
			return rhs + lhs;
		}

		/// point - delta -> point (move the point down by a relative amount), kept in the LHS point's unit.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator-(const absolute<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R = wpi::units::detail::absolute_result_unit_t<U, V>;
			return absolute<R>(R(R(wrap_detail::unwrap(lhs)).raw() - rhs.template to<R>().raw()));
		}

		/// delta + delta -> delta, kept in the LHS unit (underlying promoted only if the RHS would narrow it).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator+(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R = wpi::units::detail::delta_result_unit_t<U, V>;
			return delta<R>(R(lhs.template to<R>().raw() + rhs.template to<R>().raw()));
		}

		/// delta - delta -> delta, kept in the LHS unit (underlying promoted only if the RHS would narrow it).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator-(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R = wpi::units::detail::delta_result_unit_t<U, V>;
			return delta<R>(R(lhs.template to<R>().raw() - rhs.template to<R>().raw()));
		}

		/// Unary negation of a delta (an amount can be negated; a point cannot).
		template<UnitType U>
		constexpr delta<U> operator-(const delta<U>& d) noexcept
		{
			return delta<U>(U(-wrap_detail::unwrap(d).raw()));
		}

		/// delta scaled by a bare number -> delta. The underlying type promotes exactly as the wrapped unit's own
		/// `operator*` does (scaling an integer delta by a floating factor yields a floating delta — the wrapper
		/// is never less precise than the unit it wraps).
		template<UnitType U, ArithmeticType T>
		constexpr auto operator*(const delta<U>& lhs, T rhs) noexcept
		{
			using ScaledUnit = decltype(wrap_detail::unwrap(lhs) * rhs);
			return delta<ScaledUnit>(wrap_detail::unwrap(lhs) * rhs);
		}
		template<UnitType U, ArithmeticType T>
		constexpr auto operator*(T lhs, const delta<U>& rhs) noexcept
		{
			return rhs * lhs;
		}

		/// delta divided by a bare number -> delta (promotes like the wrapped unit's own `operator/`).
		template<UnitType U, ArithmeticType T>
		constexpr auto operator/(const delta<U>& lhs, T rhs) noexcept
		{
			using ScaledUnit = decltype(wrap_detail::unwrap(lhs) / rhs);
			return delta<ScaledUnit>(wrap_detail::unwrap(lhs) / rhs);
		}

		/// Compound move of a point by a delta. The point stays in its own unit (in-place semantics), so the rhs
		/// delta is converted to the lhs unit; this is well-formed whenever that conversion is (a lossy integer
		/// case is cleanly rejected by the delta conversion, as elsewhere in the library).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr absolute<U>& operator+=(absolute<U>& lhs, const delta<V>& rhs) noexcept
		{
			lhs = absolute<U>(U(wrap_detail::unwrap(lhs).raw() + rhs.template to<U>().raw()));
			return lhs;
		}
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr absolute<U>& operator-=(absolute<U>& lhs, const delta<V>& rhs) noexcept
		{
			lhs = absolute<U>(U(wrap_detail::unwrap(lhs).raw() - rhs.template to<U>().raw()));
			return lhs;
		}

		/// Compound add of a delta into a delta, in place (the lhs keeps its unit; the rhs is scale-converted).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr delta<U>& operator+=(delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			lhs = delta<U>(U(wrap_detail::unwrap(lhs).raw() + rhs.template to<U>().raw()));
			return lhs;
		}
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr delta<U>& operator-=(delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			lhs = delta<U>(U(wrap_detail::unwrap(lhs).raw() - rhs.template to<U>().raw()));
			return lhs;
		}

		/// Compound scale of a delta by a bare number, in place (the underlying stays the lhs type, so the scaled
		/// result is assigned back through the lhs unit — an integer delta scaled by an integer stays integer,
		/// matching the plain unit's own `operator*=`).
		template<UnitType U, ArithmeticType T>
		constexpr delta<U>& operator*=(delta<U>& lhs, T rhs) noexcept
		{
			lhs = delta<U>(U(wrap_detail::unwrap(lhs).raw() * rhs));
			return lhs;
		}
		template<UnitType U, ArithmeticType T>
		constexpr delta<U>& operator/=(delta<U>& lhs, T rhs) noexcept
		{
			lhs = delta<U>(U(wrap_detail::unwrap(lhs).raw() / rhs));
			return lhs;
		}

		//----------------------------------
		//	ABSOLUTE / DELTA COMPARISONS
		//----------------------------------

		// Comparisons reconcile to the common (finer) unit — never narrowing — so a mixed integer comparison
		// (e.g. delta<kilometers<int>> vs delta<meters<int>>) is well-formed rather than hard-erroring. A
		// comparison has no `.value()` to keep intuitive, so the common-unit reconciliation (which cannot lose an
		// equality) is the right choice here even though the arithmetic operators keep the LHS unit.

		// Point/amount comparisons delegate to the WRAPPED plain units' own comparison operators, so they inherit
		// the core's value-based (signedness-safe) integer comparison: a signed-rep and an unsigned-rep wrapper of
		// the same dimension compare by mathematical value, not by unsigned wraparound.

		/// Compare two points (the datum is applied on each side as it unwraps to the plain unit).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr bool operator==(const absolute<U>& lhs, const absolute<V>& rhs) noexcept
		{
			return wrap_detail::unwrap(lhs) == wrap_detail::unwrap(rhs);
		}
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr std::partial_ordering operator<=>(const absolute<U>& lhs, const absolute<V>& rhs) noexcept
		{
			return wrap_detail::order(wrap_detail::unwrap(lhs), wrap_detail::unwrap(rhs));
		}

		// Reconcile two deltas to the common UNIT (scale only) while keeping EACH side's own underlying type — so a
		// signed and an unsigned delta reach the plain-unit comparison in their own representations, and the core's
		// value-based comparison orders them by mathematical value rather than wrapping the negative side.
		/// Compare two deltas.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr bool operator==(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using C     = std::common_type_t<U, V>;
			using CLhs  = traits::replace_underlying_t<C, typename traits::unit_traits<U>::underlying_type>;
			using CRhs  = traits::replace_underlying_t<C, typename traits::unit_traits<V>::underlying_type>;
			return lhs.template to<CLhs>() == rhs.template to<CRhs>();
		}
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr std::partial_ordering operator<=>(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using C    = std::common_type_t<U, V>;
			using CLhs = traits::replace_underlying_t<C, typename traits::unit_traits<U>::underlying_type>;
			using CRhs = traits::replace_underlying_t<C, typename traits::unit_traits<V>::underlying_type>;
			return wrap_detail::order(lhs.template to<CLhs>(), rhs.template to<CRhs>());
		}

		//----------------------------------
		//	ABSOLUTE / DELTA FORMATTING
		//----------------------------------

		// A wrapper prints its wrapped quantity with a leading role marker so a point and an amount are visually
		// distinct: `delta ` (an ASCII "delta " prefix) marks a delta; a point prints bare (it is the common,
		// unmarked case). The numeric+abbreviation body forwards to the wrapped unit's own inserter, so the
		// wrapper never re-implements formatting.

		/// Stream a point: its wrapped quantity, unmarked.
		template<UnitType U>
		std::ostream& operator<<(std::ostream& os, const absolute<U>& obj)
		{
			return os << wrap_detail::unwrap(obj);
		}

		/// Stream a delta: a leading `delta ` marker, then its wrapped quantity.
		template<UnitType U>
		std::ostream& operator<<(std::ostream& os, const delta<U>& obj)
		{
			return os << "delta " << wrap_detail::unwrap(obj);
		}

		/// String form of a point: its wrapped quantity, unmarked.
		template<UnitType U>
		std::string to_string(const absolute<U>& obj)
		{
			return wpi::units::to_string(wrap_detail::unwrap(obj));
		}

		/// String form of a delta: a leading `delta ` marker, then its wrapped quantity.
		template<UnitType U>
		std::string to_string(const delta<U>& obj)
		{
			return std::string("delta ").append(wpi::units::to_string(wrap_detail::unwrap(obj)));
		}

		//----------------------------------
		//	ABSOLUTE / DELTA MATH
		//----------------------------------

		// A delta is an AMOUNT, so magnitude/extremum functions are meaningful on it (a point has a datum, so its
		// magnitude is not — but two points still order, so min/max of points ARE meaningful). Each forwards to
		// the wrapped unit's own cmath analog, so the result underlying promotes exactly as the plain unit does.

		/// Absolute magnitude of a delta (|amount|), promoting like the wrapped unit's `wpi::units::abs`.
		template<UnitType U>
		constexpr auto abs(const delta<U>& d) noexcept
		{
			using R = detail::floating_point_promotion_t<U>;
			return delta<R>(wpi::units::abs(R(wrap_detail::unwrap(d))));
		}

		/// The smaller of two deltas, kept in the LHS unit (scale-only reconciliation of the rhs).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto min(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R          = wpi::units::detail::delta_result_unit_t<U, V>;
			const delta<R> a = lhs.template to<delta<R>>();
			const delta<R> b = rhs.template to<delta<R>>();
			return a < b ? a : b;
		}

		/// The larger of two deltas, kept in the LHS unit (scale-only reconciliation of the rhs).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto max(const delta<U>& lhs, const delta<V>& rhs) noexcept
		{
			using R          = wpi::units::detail::delta_result_unit_t<U, V>;
			const delta<R> a = lhs.template to<delta<R>>();
			const delta<R> b = rhs.template to<delta<R>>();
			return a > b ? a : b;
		}

		/// Clamp a delta into `[lo, hi]`, kept in the value's LHS unit.
		template<UnitType U, UnitType V, UnitType W>
			requires(traits::is_same_dimension_unit_v<U, V> && traits::is_same_dimension_unit_v<U, W>)
		constexpr auto clamp(const delta<U>& value, const delta<V>& lo, const delta<W>& hi) noexcept
		{
			return affine::min(affine::max(value, lo), hi);
		}

		/// The smaller of two points, kept in the LHS unit (affine reconciliation of the rhs).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto min(const absolute<U>& lhs, const absolute<V>& rhs) noexcept
		{
			using R             = wpi::units::detail::absolute_result_unit_t<U, V>;
			const absolute<R> a = lhs.template to<absolute<R>>();
			const absolute<R> b = rhs.template to<absolute<R>>();
			return a < b ? a : b;
		}

		/// The larger of two points, kept in the LHS unit (affine reconciliation of the rhs).
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto max(const absolute<U>& lhs, const absolute<V>& rhs) noexcept
		{
			using R             = wpi::units::detail::absolute_result_unit_t<U, V>;
			const absolute<R> a = lhs.template to<absolute<R>>();
			const absolute<R> b = rhs.template to<absolute<R>>();
			return a > b ? a : b;
		}

		/// Clamp a point into `[lo, hi]`, kept in the value's LHS unit.
		template<UnitType U, UnitType V, UnitType W>
			requires(traits::is_same_dimension_unit_v<U, V> && traits::is_same_dimension_unit_v<U, W>)
		constexpr auto clamp(const absolute<U>& value, const absolute<V>& lo, const absolute<W>& hi) noexcept
		{
			return affine::min(affine::max(value, lo), hi);
		}

		//----------------------------------
		//	ABSOLUTE / DELTA MISUSE DIAGNOSTICS (readable, not a candidate wall)
		//----------------------------------
		// The common naive fumbles get a one-line library message instead of an overload-resolution wall. Each
		// returns a real value so the compiler must instantiate the body (fires the static_assert on MSVC too).

		/// Scaling or dividing a POINT is meaningless — a point has no magnitude; only a `delta` (an amount) scales.
		template<UnitType U, ArithmeticType T>
		constexpr auto operator*(const absolute<U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: a point cannot be scaled (a point has no magnitude). Take the difference of two "
				"points for a delta, or scale a delta.");
			return lhs;
		}
		template<UnitType U, ArithmeticType T>
		constexpr auto operator*(T, const absolute<U>& rhs) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: a point cannot be scaled (a point has no magnitude). Scale a delta instead.");
			return rhs;
		}
		template<UnitType U, ArithmeticType T>
		constexpr auto operator/(const absolute<U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: a point cannot be divided by a number (a point has no magnitude). Divide a delta.");
			return lhs;
		}
		/// Dividing a POINT by a POINT is meaningless — there is no ratio of two absolute positions.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator/(const absolute<U>& lhs, const absolute<V>&) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: a point cannot be divided by a point (there is no ratio of two positions). "
				"Take the difference of two points for a delta.");
			return lhs;
		}

		/// Summing two POINTS is meaningless (the sum of two positions has no meaning); subtract them for a delta,
		/// or add a `delta` to move a point.
		template<UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator+(const absolute<U>& lhs, const absolute<V>&) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: cannot add two points (the sum of two positions has no meaning). Subtract them "
				"for a delta, or add a delta to move a point.");
			return lhs;
		}

		/// A `delta`/`absolute` does not combine with a BARE NUMBER — an amount adds an amount, a point moves by an
		/// amount. Wrap the number in a `delta` of the same unit.
		template<UnitType U, ArithmeticType T>
		constexpr auto operator+(const delta<U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::delta: cannot add a bare number to a delta. Wrap the number in a delta of the same unit "
				"(e.g. delta<meters<double>>(3)).");
			return lhs;
		}
		template<UnitType U, ArithmeticType T>
		constexpr auto operator-(const delta<U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::delta: cannot subtract a bare number from a delta. Wrap the number in a delta of the same "
				"unit.");
			return lhs;
		}
		template<UnitType U, ArithmeticType T>
		constexpr auto operator+(const absolute<U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<U>,
				"wpi::units::absolute: cannot add a bare number to a point. Add a delta of the same unit to move the "
				"point (e.g. point + delta<meters<double>>(3)).");
			return lhs;
		}

		//----------------------------------
		//	KIND (TAGGED) OPERATORS
		//----------------------------------

		// A `kind` interoperates ONLY with the SAME tag: every binary operator is constrained on `Tag1 == Tag2`, so
		// two different kinds (radial vs. straight distance, torque vs. energy) do not add, subtract, or compare —
		// mixing them is ill-formed. The result keeps the LHS unit (the same tie-break as the other wrappers),
		// promoting the underlying only when the RHS would narrow it; the tag is preserved.

		/// kind + kind (same tag) -> kind, kept in the LHS unit.
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator+(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			using R = wpi::units::detail::delta_result_unit_t<U, V>;
			return basic_kind<Tag, R>(R(R(wrap_detail::unwrap(lhs)).raw() + R(wrap_detail::unwrap(rhs)).raw()));
		}

		/// kind - kind (same tag) -> kind, kept in the LHS unit.
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator-(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			using R = wpi::units::detail::delta_result_unit_t<U, V>;
			return basic_kind<Tag, R>(R(R(wrap_detail::unwrap(lhs)).raw() - R(wrap_detail::unwrap(rhs)).raw()));
		}

		/// Unary negation of a kind (keeps the tag).
		template<fixed_string Tag, UnitType U>
		constexpr basic_kind<Tag, U> operator-(const basic_kind<Tag, U>& k) noexcept
		{
			return basic_kind<Tag, U>(U(-wrap_detail::unwrap(k).raw()));
		}

		/// kind scaled by a bare number -> kind (same tag), promoting like the wrapped unit's own `operator*`.
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator*(const basic_kind<Tag, U>& lhs, T rhs) noexcept
		{
			using ScaledUnit = decltype(wrap_detail::unwrap(lhs) * rhs);
			return basic_kind<Tag, ScaledUnit>(wrap_detail::unwrap(lhs) * rhs);
		}
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator*(T lhs, const basic_kind<Tag, U>& rhs) noexcept
		{
			return rhs * lhs;
		}

		/// kind divided by a bare number -> kind (same tag).
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator/(const basic_kind<Tag, U>& lhs, T rhs) noexcept
		{
			using ScaledUnit = decltype(wrap_detail::unwrap(lhs) / rhs);
			return basic_kind<Tag, ScaledUnit>(wrap_detail::unwrap(lhs) / rhs);
		}

		/// The ratio of two SAME-tag kinds is a plain dimensionless quantity — the tag cancels, exactly as the
		/// ratio of two lengths is a number. (Different tags do not divide; that stays ill-formed.)
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto operator/(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			return wrap_detail::unwrap(lhs) / wrap_detail::unwrap(rhs);
		}

		/// Compound scale of a same-tag kind by a bare number, in place (mirrors `delta`'s `*=`/`/=`).
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr basic_kind<Tag, U>& operator*=(basic_kind<Tag, U>& lhs, T rhs) noexcept
		{
			lhs = basic_kind<Tag, U>(U(lhs.raw() * rhs));
			return lhs;
		}
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr basic_kind<Tag, U>& operator/=(basic_kind<Tag, U>& lhs, T rhs) noexcept
		{
			lhs = basic_kind<Tag, U>(U(lhs.raw() / rhs));
			return lhs;
		}

		/// Compound add/subtract of a same-tag kind, in place (the rhs is converted to the lhs unit).
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr basic_kind<Tag, U>& operator+=(basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			lhs = basic_kind<Tag, U>(U(wrap_detail::unwrap(lhs).raw() + U(wrap_detail::unwrap(rhs)).raw()));
			return lhs;
		}
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr basic_kind<Tag, U>& operator-=(basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			lhs = basic_kind<Tag, U>(U(wrap_detail::unwrap(lhs).raw() - U(wrap_detail::unwrap(rhs)).raw()));
			return lhs;
		}

		/// Compare two kinds of the SAME tag; the wrapped plain units compare (inheriting the core's value-based,
		/// signedness-safe integer comparison). Different tags do not compare.
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr bool operator==(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			return wrap_detail::unwrap(lhs) == wrap_detail::unwrap(rhs);
		}
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr std::partial_ordering operator<=>(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			return wrap_detail::order(wrap_detail::unwrap(lhs), wrap_detail::unwrap(rhs));
		}

		//----------------------------------
		//	KIND MISMATCH DIAGNOSTICS (great errors, not overload-resolution soup)
		//----------------------------------

		// Combining two DIFFERENT kinds is meaningless (a radial distance is not a straight-line distance; a torque
		// is not an energy). These catch-all overloads are the LEAST-preferred candidates (the same-tag overloads
		// above win whenever the tags match), so a mismatched combination selects one of these and stops at a single
		// readable message naming the mistake — instead of a wall of "no matching operator". They return a real
		// value (not void) so the compiler must instantiate the body to deduce the return type, which fires the
		// static_assert on g++, clang, AND MSVC (a void-returning body is not instantiated by MSVC's return-type
		// deduction, so the assert would be skipped there). To do arithmetic across kinds you must intend it:
		// unwrap one side with to<PlainUnit>().
		template<fixed_string TagL, fixed_string TagR, UnitType U, UnitType V>
			requires(!(TagL == TagR))
		constexpr auto operator+(const basic_kind<TagL, U>&, const basic_kind<TagR, V>&) noexcept
		{
			static_assert(wrap_detail::dependent_false<TagL, TagR>,
				"wpi::units::kind: cannot add two DIFFERENT kinds (their tags differ, e.g. \"radial\" vs \"straight\"). "
				"They share a unit and a dimension but are semantically distinct; unwrap one side with "
				"to<PlainUnit>() to operate on the plain unit if that is truly intended.");
			return basic_kind<TagL, U>{};
		}
		template<fixed_string TagL, fixed_string TagR, UnitType U, UnitType V>
			requires(!(TagL == TagR))
		constexpr auto operator-(const basic_kind<TagL, U>&, const basic_kind<TagR, V>&) noexcept
		{
			static_assert(wrap_detail::dependent_false<TagL, TagR>,
				"wpi::units::kind: cannot subtract two DIFFERENT kinds (their tags differ, e.g. \"radial\" vs "
				"\"straight\"). Unwrap one side with to<PlainUnit>() to operate on the plain unit if intended.");
			return basic_kind<TagL, U>{};
		}
		template<fixed_string TagL, fixed_string TagR, UnitType U, UnitType V>
			requires(!(TagL == TagR))
		constexpr bool operator==(const basic_kind<TagL, U>&, const basic_kind<TagR, V>&) noexcept
		{
			static_assert(wrap_detail::dependent_false<TagL, TagR>,
				"wpi::units::kind: cannot compare two DIFFERENT kinds (their tags differ, e.g. \"radial\" vs "
				"\"straight\"). Unwrap one side with to<PlainUnit>() to compare the plain units if intended.");
			return false;
		}

		// Mixing a `kind` with a PLAIN unit is ill-formed: a plain unit carries no kind. A plain unit becomes a
		// kind only by construction/assignment (`kind<Tag,U> k = plain;`), never in arithmetic. These
		// catch-alls turn the raw "no matching operator" wall into one readable message.
		template<fixed_string Tag, UnitType U, UnitType Plain>
		constexpr auto operator+(const basic_kind<Tag, U>& lhs, const Plain&) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<Plain>,
				"wpi::units::kind: cannot add a plain unit to a kind - a plain unit carries no kind, so mixing them in "
				"arithmetic is disallowed. Wrap the plain unit in the same kind first, or unwrap the kind with "
				"to<PlainUnit>() to work in plain units.");
			return lhs;
		}
		template<fixed_string Tag, UnitType U, UnitType Plain>
		constexpr auto operator+(const Plain&, const basic_kind<Tag, U>& rhs) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<Plain>,
				"wpi::units::kind: cannot add a plain unit to a kind - a plain unit carries no kind. Wrap the plain unit "
				"in the same kind first, or unwrap the kind with to<PlainUnit>() to work in plain units.");
			return rhs;
		}
		template<fixed_string Tag, UnitType U, UnitType Plain>
		constexpr auto operator-(const basic_kind<Tag, U>& lhs, const Plain&) noexcept
		{
			static_assert(wrap_detail::dependent_false_t<Plain>,
				"wpi::units::kind: cannot subtract a plain unit from a kind - a plain unit carries no kind. Wrap it in the "
				"same kind first, or unwrap the kind with to<PlainUnit>().");
			return lhs;
		}

		// A bare NUMBER does not add to / subtract from a kind (only kind +/- same-tag kind, and kind * / scalar
		// are defined). The most common naive fumble (`aKind + 3.0`) gets this message rather than a candidate wall.
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator+(const basic_kind<Tag, U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false<Tag>,
				"wpi::units::kind: cannot add a bare number to a kind. Wrap the number in the same kind, or unwrap the "
				"kind with to<PlainUnit>() to work in plain units.");
			return lhs;
		}
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator+(T, const basic_kind<Tag, U>& rhs) noexcept
		{
			static_assert(wrap_detail::dependent_false<Tag>,
				"wpi::units::kind: cannot add a bare number to a kind. Wrap the number in the same kind first.");
			return rhs;
		}
		template<fixed_string Tag, UnitType U, ArithmeticType T>
		constexpr auto operator-(const basic_kind<Tag, U>& lhs, T) noexcept
		{
			static_assert(wrap_detail::dependent_false<Tag>,
				"wpi::units::kind: cannot subtract a bare number from a kind. Wrap the number in the same kind first.");
			return lhs;
		}

		//----------------------------------
		//	KIND (TAGGED) FORMATTING
		//----------------------------------

		/// Stream a kind: its tag in brackets, then its wrapped quantity (e.g. `[radial] 5 m`).
		template<fixed_string Tag, UnitType U>
		std::ostream& operator<<(std::ostream& os, const basic_kind<Tag, U>& obj)
		{
			return os << '[' << Tag.value << "] " << wrap_detail::unwrap(obj);
		}

		/// String form of a kind: its tag in brackets, then its wrapped value.
		template<fixed_string Tag, UnitType U>
		std::string to_string(const basic_kind<Tag, U>& obj)
		{
			return std::string("[").append(Tag.value).append("] ").append(wpi::units::to_string(wrap_detail::unwrap(obj)));
		}

		//----------------------------------
		//	KIND (TAGGED) MATH
		//----------------------------------
		// Parity with `delta`: magnitude and extrema keep the tag (the tag is preserved through the operation).

		/// Absolute magnitude of a kind (|value|), keeping the tag, promoting like the wrapped unit's `wpi::units::abs`.
		template<fixed_string Tag, UnitType U>
		constexpr auto abs(const basic_kind<Tag, U>& k) noexcept
		{
			using R = detail::floating_point_promotion_t<U>;
			return basic_kind<Tag, R>(wpi::units::abs(R(wrap_detail::unwrap(k))));
		}

		/// The smaller of two same-tag kinds, kept in the LHS unit (the tag is preserved).
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto min(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			using R               = wpi::units::detail::delta_result_unit_t<U, V>;
			const basic_kind<Tag, R> a = lhs.template to<basic_kind<Tag, R>>();
			const basic_kind<Tag, R> b = rhs.template to<basic_kind<Tag, R>>();
			return a < b ? a : b;
		}

		/// The larger of two same-tag kinds, kept in the LHS unit (the tag is preserved).
		template<fixed_string Tag, UnitType U, UnitType V>
			requires traits::is_same_dimension_unit_v<U, V>
		constexpr auto max(const basic_kind<Tag, U>& lhs, const basic_kind<Tag, V>& rhs) noexcept
		{
			using R               = wpi::units::detail::delta_result_unit_t<U, V>;
			const basic_kind<Tag, R> a = lhs.template to<basic_kind<Tag, R>>();
			const basic_kind<Tag, R> b = rhs.template to<basic_kind<Tag, R>>();
			return a > b ? a : b;
		}

		/// Clamp a same-tag kind into `[lo, hi]`, kept in the value's LHS unit.
		template<fixed_string Tag, UnitType U, UnitType V, UnitType W>
			requires(traits::is_same_dimension_unit_v<U, V> && traits::is_same_dimension_unit_v<U, W>)
		constexpr auto clamp(const basic_kind<Tag, U>& value, const basic_kind<Tag, V>& lo, const basic_kind<Tag, W>& hi) noexcept
		{
			return affine::min(affine::max(value, lo), hi);
		}
	} // inline namespace affine


	//----------------------------------------------------------------------------------------------------------------------
	//	TOP-LEVEL `kind` ALIAS (the preferred spelling)
	//----------------------------------------------------------------------------------------------------------------------
	/// The preferred user-facing spelling of a string-tagged quantity kind: `wpi::units::kind<"radial", meters<double>>`.
	/// It resolves to the underlying `basic_kind`.
	template<affine::fixed_string Tag, UnitType U>
	using kind = affine::basic_kind<Tag, U>;
} // end namespace wpi::units

//----------------------------------------------------------------------------------------------------------------------
//      STD Namespace extensions
//----------------------------------------------------------------------------------------------------------------------
// A wrapper is hashable and has numeric limits exactly as the unit it wraps does — so a wrapper drops into an
// unordered container and generic `numeric_limits`-driven code the same way the plain unit does.

//------------------------------
//	std::hash
//------------------------------

/// Hash a point by the hash of its wrapped quantity (two equal points hash equally).
template<wpi::units::UnitType U>
struct std::hash<wpi::units::affine::absolute<U>>
{
	constexpr std::size_t operator()(const wpi::units::affine::absolute<U>& x) const noexcept { return std::hash<U>()(x.template to<U>()); }
};

/// Hash an amount by the hash of its wrapped quantity.
template<wpi::units::UnitType U>
struct std::hash<wpi::units::affine::delta<U>>
{
	constexpr std::size_t operator()(const wpi::units::affine::delta<U>& x) const noexcept { return std::hash<U>()(x.template to<U>()); }
};

/// Hash a kind by the hash of its wrapped quantity (the tag does not participate — equal values hash equally).
template<wpi::units::affine::fixed_string Tag, wpi::units::UnitType U>
struct std::hash<wpi::units::affine::basic_kind<Tag, U>>
{
	constexpr std::size_t operator()(const wpi::units::affine::basic_kind<Tag, U>& x) const noexcept { return std::hash<U>()(x.template to<U>()); }
};

//------------------------------
//	std::numeric_limits
//------------------------------

namespace std
{
	/// Numeric limits of a point: the wrapped unit's limits, re-wrapped as points.
	template<wpi::units::UnitType U>
	struct numeric_limits<wpi::units::affine::absolute<U>>
	{
		static constexpr bool is_specialized = true;
		static constexpr wpi::units::affine::absolute<U> min() { return wpi::units::affine::absolute<U>(std::numeric_limits<U>::min()); }
		static constexpr wpi::units::affine::absolute<U> max() { return wpi::units::affine::absolute<U>(std::numeric_limits<U>::max()); }
		static constexpr wpi::units::affine::absolute<U> lowest() { return wpi::units::affine::absolute<U>(std::numeric_limits<U>::lowest()); }
	};

	/// Numeric limits of an amount: the wrapped unit's limits, re-wrapped as amounts.
	template<wpi::units::UnitType U>
	struct numeric_limits<wpi::units::affine::delta<U>>
	{
		static constexpr bool is_specialized = true;
		static constexpr wpi::units::affine::delta<U> min() { return wpi::units::affine::delta<U>(std::numeric_limits<U>::min()); }
		static constexpr wpi::units::affine::delta<U> max() { return wpi::units::affine::delta<U>(std::numeric_limits<U>::max()); }
		static constexpr wpi::units::affine::delta<U> lowest() { return wpi::units::affine::delta<U>(std::numeric_limits<U>::lowest()); }
	};

	/// Numeric limits of a kind: the wrapped unit's limits, re-wrapped as the same kind (tag preserved).
	template<wpi::units::affine::fixed_string Tag, wpi::units::UnitType U>
	struct numeric_limits<wpi::units::affine::basic_kind<Tag, U>>
	{
		static constexpr bool is_specialized = true;
		static constexpr wpi::units::affine::basic_kind<Tag, U> min() { return wpi::units::affine::basic_kind<Tag, U>(std::numeric_limits<U>::min()); }
		static constexpr wpi::units::affine::basic_kind<Tag, U> max() { return wpi::units::affine::basic_kind<Tag, U>(std::numeric_limits<U>::max()); }
		static constexpr wpi::units::affine::basic_kind<Tag, U> lowest() { return wpi::units::affine::basic_kind<Tag, U>(std::numeric_limits<U>::lowest()); }
	};
} // namespace std

#endif // UNIT_KIND_H
