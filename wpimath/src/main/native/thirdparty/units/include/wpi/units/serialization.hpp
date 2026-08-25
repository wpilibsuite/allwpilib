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
/// @file	units/serialization.h
/// @brief	self-describing binary serialization of quantities
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_serialization_h_
#define units_serialization_h_

#include <array>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <expected>
#include <functional>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <wpi/units/core.hpp>
#include <utility>
#include <vector>

namespace wpi::units
{
	namespace detail
	{
		/// The library's known dimensions, offered to `visit` as the default candidate set so a stream of any
		/// built-in dimension resolves without the caller naming it. This is NOT the wire vocabulary: the stream
		/// keys each base dimension by a hash of its `name` STRING, so a dimension the library has never seen —
		/// including a user-defined `make_dimension<my_tag>` — still serializes and round-trips. `visit` cannot
		/// resolve a user-defined dimension unless the caller lists it (`visit<my_dimension>(f)`), because C++ cannot
		/// materialize a type from the runtime hash — the runtime→type wall. The set is otherwise open by design.
		using builtin_dimensions = std::tuple<dimension::length, dimension::mass, dimension::time, dimension::current, dimension::temperature, dimension::substance, dimension::luminous_intensity,
			dimension::angle, dimension::data, dimension::solid_angle, dimension::frequency, dimension::velocity, dimension::angular_velocity, dimension::acceleration, dimension::force,
			dimension::area, dimension::volume, dimension::volume_flow_rate, dimension::pressure, dimension::charge, dimension::energy, dimension::power, dimension::voltage, dimension::capacitance,
			dimension::impedance, dimension::conductance, dimension::magnetic_flux, dimension::inductance, dimension::luminous_flux, dimension::illuminance, dimension::luminance,
			dimension::radioactivity, dimension::substance_mass, dimension::substance_concentration, dimension::magnetic_field_strength, dimension::radiant_intensity, dimension::radiance,
			dimension::irradiance, dimension::spectral_intensity, dimension::spectral_flux, dimension::spectral_radiance, dimension::spectral_irradiance, dimension::jerk, dimension::torque,
			dimension::density, dimension::energy_density, dimension::concentration, dimension::data_transfer_rate, dimension::dynamic_viscosity, dimension::kinematic_viscosity>;

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: name_hash [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      FNV-1a 64-bit hash of a base dimension's name
		/// @details	Hashing the library-controlled `name` string (not a compiler-generated type name) is portable
		///				across compilers and versions, is a fixed 8 bytes regardless of name length, and needs no
		///				central table — any dimension name, built-in or user-defined, hashes to a stable wire key.
		/// @param[in]  name  the dimension name
		/// @return     the 64-bit hash
		//------------------------------------------------------------------------------------------------------------------
		constexpr std::uint64_t name_hash(std::string_view name) noexcept
		{
			std::uint64_t h = 1469598103934665603ULL;
			for (const char c : name)
			{
				h ^= static_cast<std::uint8_t>(c);
				h *= 1099511628211ULL;
			}
			return h;
		}
	} // namespace detail

	/// @brief	the reasons a deserialize can fail
	enum class deserialize_error
	{
		truncated,              ///< the byte range ended before a complete quantity was read
		bad_version,            ///< the stream's format version is not understood
		dimension_mismatch,     ///< the stream's dimension does not match the requested target
		unknown_base_dimension, ///< the stream names a base-dimension code this build does not know
		lossy_target            ///< the value cannot be represented in the requested underlying type without loss
	};

	/// @brief	a decoded quantity whose concrete type was not known at the call site
	/// @details	`deserialize` yields this erased value. It carries the dimension signature and the magnitude
	///				in SI canonical base. It has NO arithmetic operators — to compute, collapse it into a concrete
	///				unit with `to<Unit>()` (safe, returns `std::expected`), `try_to<Unit>()` / `unit_cast<Unit>()`
	///				(throwing), or `visit()` (the canonical unit for the decoded dimension, no target named).
	class any_unit;

	//======================================================================================================================
	//	VARINT (LEB128) — the terse integer codec
	//======================================================================================================================

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: put_uvarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      appends an unsigned integer to a byte buffer as an LEB128 varint
		/// @param[out] out    buffer to append to
		/// @param[in]  value  value to encode
		//------------------------------------------------------------------------------------------------------------------
		inline void put_uvarint(std::vector<std::byte>& out, std::uint64_t value)
		{
			do
			{
				std::uint8_t byte = value & 0x7F;
				value >>= 7;
				if (value != 0)
					byte |= 0x80;
				out.push_back(std::byte{byte});
			} while (value != 0);
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: put_svarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      appends a signed integer to a byte buffer as a zig-zag LEB128 varint
		/// @param[out] out    buffer to append to
		/// @param[in]  value  value to encode
		//------------------------------------------------------------------------------------------------------------------
		inline void put_svarint(std::vector<std::byte>& out, std::int64_t value)
		{
			put_uvarint(out, (static_cast<std::uint64_t>(value) << 1) ^ static_cast<std::uint64_t>(value >> 63));
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: get_uvarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      reads an LEB128 unsigned varint from a byte cursor
		/// @param[in,out] cursor  pointer into the buffer; advanced past the value on success
		/// @param[in]  end     one-past-the-end of the buffer
		/// @param[out] value   decoded value
		/// @return     true on success, false if the buffer was truncated
		//------------------------------------------------------------------------------------------------------------------
		inline bool get_uvarint(const std::byte*& cursor, const std::byte* end, std::uint64_t& value)
		{
			value              = 0;
			unsigned int shift = 0;
			while (cursor != end)
			{
				const auto byte = std::to_integer<std::uint8_t>(*cursor++);
				value |= static_cast<std::uint64_t>(byte & 0x7F) << shift;
				if ((byte & 0x80) == 0)
					return true;
				shift += 7;
				if (shift >= 64)
					return false;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: get_svarint [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      reads a zig-zag LEB128 signed varint from a byte cursor
		/// @param[in,out] cursor  pointer into the buffer; advanced past the value on success
		/// @param[in]  end     one-past-the-end of the buffer
		/// @param[out] value   decoded value
		/// @return     true on success, false if the buffer was truncated
		//------------------------------------------------------------------------------------------------------------------
		inline bool get_svarint(const std::byte*& cursor, const std::byte* end, std::int64_t& value)
		{
			std::uint64_t raw;
			if (!get_uvarint(cursor, end, raw))
				return false;
			value = static_cast<std::int64_t>((raw >> 1) ^ (~(raw & 1) + 1));
			return true;
		}
	} // namespace detail

	//======================================================================================================================
	//	UNIT IDENTITY — the compile-time dimension signature, and its runtime form
	//======================================================================================================================

	/// @brief	one base-dimension term of a signature: which base dimension (by name-hash), and its rational exponent
	/// @details	The base dimension is identified by an FNV-1a hash of its `name` string, so an arbitrary — including
	///				user-defined — base dimension round-trips in a fixed 8 bytes with no central registry.
	struct dimension_term
	{
		std::uint64_t hash; ///< FNV-1a hash of the base dimension's name; the wire identity
		std::int64_t  num;  ///< exponent numerator
		std::int64_t  den;  ///< exponent denominator (1 for the common integer-exponent case)
	};

	/// @brief	the runtime identity of a quantity's dimension — the set of nonzero base-dimension terms
	/// @details	Backed by a `std::vector`, so there is NO baked-in ceiling on the number of base dimensions a
	///				quantity may compose. The terms are held sorted by hash so equality is order-independent.
	struct unit_identity
	{
		std::vector<dimension_term> terms;

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator== [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      dimension-signature equality
		/// @param[in]  other  identity to compare against
		/// @return     true iff both carry the same nonzero base-dimension terms (by hash and exponent)
		//------------------------------------------------------------------------------------------------------------------
		bool operator==(const unit_identity& other) const noexcept
		{
			if (terms.size() != other.terms.size())
				return false;
			for (std::size_t i = 0; i < terms.size(); ++i)
				if (terms[i].hash != other.terms[i].hash || terms[i].num != other.terms[i].num || terms[i].den != other.terms[i].den)
					return false;
			return true;
		}
	};

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: dimension_arity [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the number of base-dimension terms in a `dimension_t<...>` list
		/// @tparam     DimensionList  a `dimension_t<...>` specialization
		/// @return     the term count
		//------------------------------------------------------------------------------------------------------------------
		template<class DimensionList>
		consteval std::size_t dimension_arity()
		{
			if constexpr (DimensionList::empty)
				return 0;
			else
				return 1 + dimension_arity<typename DimensionList::pop_front>();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: fill_terms [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      writes a term per base dimension of a `dimension_t<...>` list into a fixed span
		/// @details	Each term is keyed by a hash of the base dimension's `name`, so ANY base dimension — built-in
		///				or user-defined `make_dimension<my_tag>` — is captured; there is no fixed vocabulary.
		/// @tparam     DimensionList  a `dimension_t<...>` specialization
		/// @tparam     N  the fixed capacity of the destination array
		/// @param[out] out  the destination array
		/// @param[in]  at   the index to write the front term at
		//------------------------------------------------------------------------------------------------------------------
		template<class DimensionList, std::size_t N>
		consteval void fill_terms(std::array<dimension_term, N>& out, std::size_t at)
		{
			if constexpr (!DimensionList::empty)
			{
				using front_dim = typename DimensionList::front;
				using tag       = typename front_dim::dimension;
				using exponent  = typename front_dim::exponent;
				out[at]         = dimension_term{name_hash(std::string_view(tag::name)), exponent::num, exponent::den};
				fill_terms<typename DimensionList::pop_front, N>(out, at + 1);
			}
		}

		/// @brief	the compile-time signature of a unit as a fixed-size, sorted array of terms
		/// @tparam	Unit  a `UnitType`
		template<UnitType Unit>
		struct signature
		{
			using Dim                          = traits::dimension_of_t<typename traits::unit_traits<Unit>::conversion_factor>;
			static constexpr std::size_t arity = dimension_arity<Dim>();

			static consteval std::array<dimension_term, arity> compute()
			{
				std::array<dimension_term, arity> terms{};
				fill_terms<Dim, arity>(terms, 0);
				// insertion sort by hash — arity is tiny (the base dimensions of one quantity)
				for (std::size_t i = 1; i < arity; ++i)
				{
					dimension_term key = terms[i];
					std::size_t    j   = i;
					while (j > 0 && terms[j - 1].hash > key.hash)
					{
						terms[j] = terms[j - 1];
						--j;
					}
					terms[j] = key;
				}
				return terms;
			}

			static constexpr std::array<dimension_term, arity> value = compute();
		};

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: identity_of [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the dimension signature of a unit type, as the runtime (vector-backed) identity
		/// @details	Materializes the compile-time fixed-array signature into a `unit_identity` for runtime compare.
		/// @tparam     Unit  a `UnitType`
		/// @return     the unit_identity (nonzero base-dimension terms, sorted by hash)
		//------------------------------------------------------------------------------------------------------------------
		template<UnitType Unit>
		unit_identity identity_of()
		{
			unit_identity id;
			id.terms.assign(signature<Unit>::value.begin(), signature<Unit>::value.end());
			return id;
		}

		/// @brief	the canonical SI base unit of a dimension (ratio 1, no pi, no translation)
		template<class Dim>
		using canonical_unit_t = unit<conversion_factor<std::ratio<1>, Dim>, double>;
	} // namespace detail

	//======================================================================================================================
	//	WIRE FORMAT
	//======================================================================================================================

	namespace detail
	{
		inline constexpr std::uint8_t serialization_version = 1;

		/// header byte layout: [ valueKind:2 | fracExp:1 | reserved:5 ]
		enum class value_kind : std::uint8_t
		{
			ivarint = 0, ///< value is an integer in SI base, zig-zag varint
			f32     = 1, ///< value is an exact 32-bit float
			f64     = 2  ///< value is a 64-bit double
		};

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: encode [static]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      encodes a dimension identity and SI-base magnitude to the self-describing byte stream
		/// @details    The single source of truth for the wire format, used by both `serialize` (from a compile-time
		///             signature) and `any_unit` (from its decoded runtime identity), so a value and its round-tripped
		///             form encode byte-identically. The value encoding is chosen from `base` alone (tersest exact:
		///             integer varint, else exact 32-bit float, else 64-bit double), so it does not depend on the
		///             source unit's underlying type.
		/// @param[in]  identity  the ordered base-dimension terms (sorted by hash)
		/// @param[in]  base      the magnitude in SI canonical base
		/// @return     the encoded bytes
		//------------------------------------------------------------------------------------------------------------------
		inline std::vector<std::byte> encode(const unit_identity& identity, double base)
		{
			value_kind kind;
			if (base == std::floor(base) && std::abs(base) < 9.0e15)
				kind = value_kind::ivarint;
			else if (static_cast<double>(static_cast<float>(base)) == base)
				kind = value_kind::f32;
			else
				kind = value_kind::f64;

			// any fractional exponent forces the fracExp flag
			bool fracExp = false;
			for (const auto& term : identity.terms)
				if (term.den != 1)
					fracExp = true;

			std::vector<std::byte> out;
			out.push_back(std::byte{serialization_version});
			const std::uint8_t header = static_cast<std::uint8_t>(static_cast<std::uint8_t>(kind) | (fracExp ? 0x04 : 0x00));
			out.push_back(std::byte{header});
			put_uvarint(out, identity.terms.size());
			for (const auto& term : identity.terms)
			{
				// base dimension keyed by an 8-byte name-hash: fixed size, no central table, any dimension round-trips
				for (unsigned int i = 0; i < 8; ++i)
					out.push_back(std::byte{static_cast<std::uint8_t>(term.hash >> (8 * i))});
				put_svarint(out, term.num);
				if (fracExp)
					put_svarint(out, term.den);
			}

			switch (kind)
			{
			case value_kind::ivarint: put_svarint(out, static_cast<std::int64_t>(base)); break;
			case value_kind::f32:
			{
				const float   f = static_cast<float>(base);
				std::uint32_t bits;
				std::memcpy(&bits, &f, sizeof(bits));
				for (unsigned int i = 0; i < 4; ++i)
					out.push_back(std::byte{static_cast<std::uint8_t>(bits >> (8 * i))});
				break;
			}
			case value_kind::f64:
			{
				std::uint64_t bits;
				std::memcpy(&bits, &base, sizeof(bits));
				for (unsigned int i = 0; i < 8; ++i)
					out.push_back(std::byte{static_cast<std::uint8_t>(bits >> (8 * i))});
				break;
			}
			}
			return out;
		}
	} // namespace detail

	//======================================================================================================================
	//	any_unit
	//======================================================================================================================

	class any_unit
	{
	public:
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: any_unit [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      constructs an erased quantity from a decoded identity and SI-base magnitude
		/// @details    The serialized byte form is materialized once here (from the same encoder `serialize` uses), so
		///             it is owned by the `any_unit` and shares its lifetime: `bytes()`, `data()`, and `size()` view a
		///             buffer that stays valid for as long as the `any_unit` does.
		/// @param[in]  id     the dimension signature
		/// @param[in]  base   the magnitude in SI canonical base
		//------------------------------------------------------------------------------------------------------------------
		any_unit(unit_identity id, double base)
		  : m_identity(std::move(id)),
			m_base(base),
			m_bytes(detail::encode(m_identity, base))
		{
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: any_unit [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      constructs an empty erased quantity (dimensionless, zero) — the target for stream extraction
		/// @details    Provided so `any_unit value; stream >> value;` is well-formed; `operator>>` overwrites it with
		///             the decoded quantity, or sets the stream's failbit and leaves it empty on malformed input.
		//------------------------------------------------------------------------------------------------------------------
		any_unit()
		  : m_identity(),
			m_base(0.0),
			m_bytes(detail::encode(m_identity, 0.0))
		{
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: is [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      whether this erased quantity is of the requested dimension
		/// @tparam     Dimension  a `dimension::*` type
		/// @return     true iff the decoded dimension matches
		//------------------------------------------------------------------------------------------------------------------
		template<class Dimension>
		[[nodiscard]] bool is() const noexcept
		{
			return m_identity == detail::identity_of<detail::canonical_unit_t<Dimension>>();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: value_in_base [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the magnitude in SI canonical base units, for logging or routing
		/// @return     the value
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] double value_in_base() const noexcept
		{
			return m_base;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: identity [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the decoded dimension signature
		/// @return     the unit_identity
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] const unit_identity& identity() const noexcept
		{
			return m_identity;
		}

		//======================================================================================================================
		//	BYTES — the owned serialized form, in both a type-safe and a C-interface view
		//======================================================================================================================

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: bytes [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the serialized byte stream, as a type-safe view
		/// @details    A view into the buffer owned by this `any_unit`; valid for the object's lifetime. Feed it straight
		///             back to `deserialize`. Copy it (e.g. into a `std::vector`) if it must outlive the `any_unit`.
		/// @return     a span over the owned bytes
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::span<const std::byte> bytes() const noexcept
		{
			return m_bytes;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator std::span<const std::byte> [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      an `any_unit` is viewable as its serialized bytes
		/// @details    Lets an `any_unit` pass directly to anything expecting a byte span — notably `deserialize`, so a
		///             `serialize` result feeds straight back in. The view is tied to this object's lifetime, as `bytes()`.
		/// @return     a span over the owned bytes
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] operator std::span<const std::byte>() const noexcept
		{
			return m_bytes;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: data [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      a pointer to the serialized bytes as `const char*`, for byte-oriented interfaces
		/// @details    Paired with `size()`, this drops directly into the interfaces that take a `const char*`/`const
		///             void*` and a length — `std::ostream::write`, `std::fwrite`, a socket `send` — with no cast at the
		///             call site. The pointer views the buffer owned by this `any_unit` and is valid for its lifetime.
		/// @return     a pointer to the first byte
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] const char* data() const noexcept
		{
			return reinterpret_cast<const char*>(m_bytes.data());
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: size [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the number of serialized bytes
		/// @return     the byte count
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::size_t size() const noexcept
		{
			return m_bytes.size();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: to_string [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      a human-readable text rendering of the erased quantity, for logging and diagnostics
		/// @details    For a dimension the library knows, renders the SI-base magnitude in that dimension's canonical unit
		///             with its unit name — the same text `operator<<(ostream, unit)` produces (e.g. `1000 m`, `9.81 m s^-2`)
		///             — resolved without the caller naming a target, over the same candidate set `visit()` uses. For a
		///             dimension outside that set (a user-defined `make_dimension`), the name cannot be recovered from the
		///             wire's name-hash (the runtime→type wall), so it degrades to the raw hash form of `to_string_raw()`.
		///             This is the TEXT form; `operator<<`/`operator>>` on a stream move the raw BINARY bytes.
		/// @return     the text rendering — a named-unit form when the dimension is known, else the raw hash form
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::string to_string() const
		{
			std::string named;
			// resolve to the canonical named unit of whichever known dimension matches, and render it exactly as a
			// concrete unit streams (name/abbreviation + dimension form); leave `named` empty if no known dimension matched
			try
			{
				visit([&named](const auto& quantity) { named = wpi::units::to_string(quantity); });
			}
			catch (const std::runtime_error&)
			{
			}
			return named.empty() ? to_string_raw() : named;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: to_string_raw [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      the dimension-agnostic text rendering, keyed by name-hash — always available, never resolves a name
		/// @details    The SI-base magnitude followed by each base term as `#<hash>^<exponent>` (a fractional exponent as
		///             `#<hash>^<num>/<den>`, dimensionless as `[dimensionless]`). Unlike `to_string()`, it never attempts
		///             to name the dimension, so it renders identically for a built-in and a user-defined dimension and is
		///             the honest diagnostic for a quantity whose type the library cannot know. `to_string()` falls back to
		///             this whenever no known dimension matched.
		/// @return     the raw hash-keyed text rendering
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::string to_string_raw() const
		{
			std::string out = std::to_string(m_base);
			if (m_identity.terms.empty())
			{
				out += " [dimensionless]";
				return out;
			}
			out += " [";
			for (std::size_t i = 0; i < m_identity.terms.size(); ++i)
			{
				if (i != 0)
					out += ' ';
				char hex[19];
				std::snprintf(hex, sizeof(hex), "#%llx", static_cast<unsigned long long>(m_identity.terms[i].hash));
				out += hex;
				out += '^';
				out += std::to_string(m_identity.terms[i].num);
				if (m_identity.terms[i].den != 1)
				{
					out += '/';
					out += std::to_string(m_identity.terms[i].den);
				}
			}
			out += ']';
			return out;
		}

		//======================================================================================================================
		//	COMPARISON
		//======================================================================================================================

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator== [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      whether two erased quantities are the same dimension and magnitude
		/// @details    Equal iff the dimension signatures match and the SI-base magnitudes compare equal. The magnitude
		///             comparison uses the same relative-epsilon tolerance as the concrete `unit` comparison, so an
		///             `any_unit` compares no more strictly than the units it erases; two quantities of the same base
		///             value are equal regardless of the source unit (a serialized `1000 m` equals a serialized `1 km`).
		/// @param[in]  other  the erased quantity to compare against
		/// @return     true iff same dimension and (tolerantly) equal magnitude
		/// @note       As with `unit`, the tolerance may not suit every application when the base value is a double;
		///             compare `value_in_base()` directly for a different criterion.
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] bool operator==(const any_unit& other) const noexcept
		{
			if (!(m_identity == other.m_identity))
				return false;
			const double diff = std::abs(m_base - other.m_base);
			return diff < std::numeric_limits<double>::epsilon() * std::abs(m_base + other.m_base) || diff < std::numeric_limits<double>::min();
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator!= [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      whether two erased quantities differ in dimension or magnitude
		/// @param[in]  other  the erased quantity to compare against
		/// @return     true iff not equal
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] bool operator!=(const any_unit& other) const noexcept
		{
			return !(*this == other);
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: operator<=> [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      orders two erased quantities of the same dimension by magnitude
		/// @details    Ordering is only meaningful within a dimension: two lengths compare by their SI-base magnitude,
		///             but a length and a time have no order. Same-dimension operands compare by base value; operands of
		///             different dimensions are `unordered`, so `<`, `<=`, `>`, `>=` are all false between them. This is
		///             a `std::partial_ordering` precisely because the relation is partial across dimensions.
		/// @param[in]  other  the erased quantity to compare against
		/// @return     the base-value ordering when same-dimension, else `std::partial_ordering::unordered`
		//------------------------------------------------------------------------------------------------------------------
		[[nodiscard]] std::partial_ordering operator<=>(const any_unit& other) const noexcept
		{
			if (!(m_identity == other.m_identity))
				return std::partial_ordering::unordered;
			if (*this == other)
				return std::partial_ordering::equivalent;
			return m_base <=> other.m_base;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: to [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      collapses into a concrete unit, checked (the safe default)
		/// @tparam     Unit  the target unit type
		/// @return     the value as `Unit` on a dimension match, else `deserialize_error::dimension_mismatch`
		//------------------------------------------------------------------------------------------------------------------
		template<class Unit>
		[[nodiscard]] std::expected<Unit, deserialize_error> to() const
		{
			static_assert(traits::is_unit_v<Unit>,
				"any_unit::to<T>() collapses into a unit type (e.g. meters<double>), not a bare number. To read a plain value, collapse to a unit first, then call .value() or .to<double>() on that "
				"unit.");
			// gate the body so a non-unit Unit produces ONLY the friendly message above, no downstream template soup
			if constexpr (traits::is_unit_v<Unit>)
			{
				if (m_identity != detail::identity_of<Unit>())
					return std::unexpected(deserialize_error::dimension_mismatch);

				using ConversionFactor = typename traits::unit_traits<Unit>::conversion_factor;
				using Dim              = traits::dimension_of_t<ConversionFactor>;
				using UnderlyingTarget = typename traits::unit_traits<Unit>::underlying_type;

				// Express the SI-base magnitude in the TARGET unit's scale, all in double so no lossy unit conversion is
				// attempted: a double-underlying instance of the target unit converts from the canonical base cleanly.
				using TargetAsDouble   = unit<traits::strong_t<ConversionFactor>, double, typename traits::unit_traits<Unit>::numerical_scale_type>;
				const double as_double = TargetAsDouble(detail::canonical_unit_t<Dim>(m_base)).template to<double>();

				// Narrow to the target's underlying type. An integral target that cannot represent the value exactly is
				// a lossy_target error rather than a silent truncation.
				if constexpr (!std::is_floating_point_v<UnderlyingTarget>)
				{
					if (as_double != std::floor(as_double) || std::abs(as_double) > static_cast<double>(std::numeric_limits<UnderlyingTarget>::max()))
						return std::unexpected(deserialize_error::lossy_target);
				}
				return Unit(static_cast<UnderlyingTarget>(as_double));
			}
			else
			{
				return std::unexpected(deserialize_error::dimension_mismatch); // unreachable; the static_assert fired
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: try_to [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      collapses into a concrete unit, throwing on a dimension mismatch
		/// @tparam     Unit  the target unit type
		/// @return     the value as `Unit`
		//------------------------------------------------------------------------------------------------------------------
		template<class Unit>
		[[nodiscard]] Unit try_to() const
		{
			static_assert(traits::is_unit_v<Unit>, "any_unit::try_to<T>() collapses into a unit type (e.g. meters<double>), not a bare number.");
			auto result = to<Unit>();
			if (!result)
				throw std::runtime_error("wpi::units::any_unit: dimension mismatch collapsing to the requested unit");
			return *result;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: assign_to [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      collapses into an existing unit variable, leaving it untouched on a dimension mismatch
		/// @details	The mismatch-tolerant collapse: assigns into `out` and returns `true` iff the decoded dimension is
		///				`out`'s dimension, otherwise returns `false` and leaves `out` unchanged. It is the ergonomic form of
		///				`if (auto v = to<Unit>()) out = *v;` — the target unit is deduced from `out`, so the value need not be
		///				named twice, and the boolean says whether the assignment happened. A value that would not fit `out`'s
		///				underlying type (`to`'s `lossy_target`) is also reported as not assigned, so `out` is written only
		///				with a value it represents exactly. Unlike `try_to`/`unit_cast`, a mismatch is not an error but an
		///				expected outcome — the shape for pulling one erased quantity into whichever of several typed fields it
		///				fits, without a throw or a named target at each site.
		/// @tparam     Unit  the target unit type; deduced from `out`
		/// @param[out] out   receives the collapsed value on a dimension (and representability) match
		/// @return     true iff `out` was assigned
		//------------------------------------------------------------------------------------------------------------------
		template<class Unit>
		bool assign_to(Unit& out) const
		{
			static_assert(traits::is_unit_v<Unit>, "any_unit::assign_to(out) assigns into a unit variable (e.g. meters<double>), not a bare number. Collapse to a unit, then read its value.");
			if (auto result = to<Unit>())
			{
				out = *result;
				return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: visit [public]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      invokes a visitor with the canonical quantity for the decoded dimension
		/// @details	The visitor is called with the canonical SI unit of whichever candidate dimension the stream
		///				holds, so no target type is named at the call site and all arithmetic inside the visitor is
		///				compile-time checked. With no explicit candidates, every dimension the library defines is a
		///				candidate (so velocity/force/energy/... resolve out of the box); pass explicit candidate
		///				dimensions (`visit<my_dimension>(f)`) to resolve a user-defined dimension or to disambiguate
		///				dimensions that share a signature (e.g. torque vs energy — the first listed wins). The visitor
		///				must be a generic callable (e.g. a `[](auto q)` lambda). Throws if no candidate matched.
		/// @tparam     Dimensions  candidate dimension types (defaults to the library's known dimensions)
		/// @tparam     Visitor  a callable invocable with each candidate's canonical unit
		/// @param[in]  visitor  the callable
		//------------------------------------------------------------------------------------------------------------------
		template<class... Dimensions, class Visitor>
		void visit(Visitor&& visitor) const
		{
			// bind to a named lvalue so the traversal passes it through by reference (never moving it), and the
			// single invocation site (try_dispatch_one) is the only place it is used as the caller's value category
			bool matched;
			if constexpr (sizeof...(Dimensions) == 0)
				matched = dispatch_tuple<detail::builtin_dimensions>(std::forward<Visitor>(visitor));
			else
				matched = dispatch_list<Dimensions...>(std::forward<Visitor>(visitor));
			if (!matched)
				throw std::runtime_error("wpi::units::any_unit: no candidate dimension matched the stream");
		}

	private:
		//------------------------------------------------------------------------------------------------------------------
		//      FUNCTION: try_dispatch_one [private]
		//------------------------------------------------------------------------------------------------------------------
		/// @brief      if the erased dimension matches `Dimension`, invokes the visitor with its canonical unit
		/// @tparam     Dimension  the candidate dimension
		/// @tparam     Visitor  the visitor callable
		/// @param[in]  visitor  the callable
		/// @return     true iff the dimension matched and the visitor was invoked
		//------------------------------------------------------------------------------------------------------------------
		template<class Dimension, class Visitor>
		bool try_dispatch_one(Visitor&& visitor) const
		{
			using Base = detail::canonical_unit_t<Dimension>;
			if (m_identity == detail::identity_of<Base>())
			{
				std::forward<Visitor>(visitor)(Base(m_base));
				return true;
			}
			return false;
		}

		/// dispatch over an explicit candidate pack, in order (first match wins). The visitor is passed by
		/// lvalue through the traversal and forwarded only at the single point it is invoked (in
		/// try_dispatch_one), so it is moved-from at most once even for an rvalue visitor.
		template<class... Dimensions, class Visitor>
		bool dispatch_list(Visitor&& visitor) const
		{
			bool matched = false;
			// fold in order; stop invoking once matched
			((matched = matched || try_dispatch_one<Dimensions>(visitor)), ...);
			return matched;
		}

		/// dispatch over a tuple of candidate dimensions, in order (first match wins). As with dispatch_list,
		/// the visitor is passed by lvalue through the recursion and forwarded only where it is invoked.
		template<class DimTuple, std::size_t I = 0, class Visitor>
		bool dispatch_tuple(Visitor&& visitor) const
		{
			if constexpr (I < std::tuple_size_v<DimTuple>)
			{
				if (try_dispatch_one<std::tuple_element_t<I, DimTuple>>(visitor))
					return true;
				return dispatch_tuple<DimTuple, I + 1>(visitor);
			}
			return false;
		}

		unit_identity          m_identity;
		double                 m_base;
		std::vector<std::byte> m_bytes; ///< the owned serialized form; declared last so the ctor can encode from m_identity
	};

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: operator<< [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      writes an erased quantity's self-describing BINARY bytes to a stream
	/// @details    Writes exactly `bytes()` — the serialized form — so `stream << serialize(q)` persists a quantity to
	///             a file/socket and a later `stream >> value` recovers it. Open the stream in binary mode. This moves
	///             the raw bytes, NOT text; for a human-readable rendering use `to_string()`.
	/// @param[in]  os     the output stream
	/// @param[in]  value  the erased quantity
	/// @return     the stream
	//----------------------------------------------------------------------------------------------------------------------
	inline std::ostream& operator<<(std::ostream& os, const any_unit& value)
	{
		os.write(value.data(), static_cast<std::streamsize>(value.size()));
		return os;
	}

	//======================================================================================================================
	//	unit_cast — reclaimed: the explicit throwing collapse from any_unit to a concrete unit
	//======================================================================================================================

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: unit_cast [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      explicit cast from an erased `any_unit` to a concrete unit, throwing on a dimension mismatch
	/// @details	The free-function cast idiom for the same operation as `any_unit::try_to<Unit>()`. Mirrors
	///				`std::any_cast`. This is the purpose `unit_cast` was created for.
	/// @tparam     Target  the target unit type
	/// @param[in]  value   the erased quantity
	/// @return     the value as `Target`
	//----------------------------------------------------------------------------------------------------------------------
	template<class Target>
	[[nodiscard]] Target unit_cast(const any_unit& value)
	{
		static_assert(traits::is_unit_v<Target>, "wpi::units::unit_cast<T>(any_unit) casts to a unit type (e.g. meters<double>), not a bare number. Collapse to a unit, then read its value.");
		return value.try_to<Target>();
	}

	//======================================================================================================================
	//	serialize / deserialize
	//======================================================================================================================

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: serialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      serializes a quantity to a self-describing, erased `any_unit`
	/// @details	The returned `any_unit` carries the dimension signature, the magnitude in SI canonical base, and the
	///				owned byte stream. A peer decodes it with `deserialize` without prior agreement on the type; the byte
	///				form is available through `bytes()` (type-safe span) and `data()`/`size()` (for `ostream::write`,
	///				`fwrite`, a socket `send`, and other byte-oriented interfaces, with no cast at the call site).
	/// @tparam     Unit  a `UnitType`
	/// @param[in]  quantity  the value to serialize
	/// @return     the erased quantity, owning its serialized bytes
	//----------------------------------------------------------------------------------------------------------------------
	template<class Unit>
	[[nodiscard]] any_unit serialize(const Unit& quantity)
	{
		static_assert(traits::is_unit_v<Unit>, "wpi::units::serialize requires a units quantity (e.g. meters<double>). Its argument is not a unit type; wrap the value in a unit before serializing.");
		// gate the body so a non-unit argument produces ONLY the friendly message above, no downstream template soup
		if constexpr (!traits::is_unit_v<Unit>)
			return any_unit{unit_identity{}, 0.0};
		else
		{
			constexpr auto& sig = detail::signature<Unit>::value; // fixed-array compile-time signature (sorted by hash)

			// value in SI canonical base
			using Dim         = traits::dimension_of_t<typename traits::unit_traits<Unit>::conversion_factor>;
			using Base        = detail::canonical_unit_t<Dim>;
			const double base = Base(quantity).value();

			// lift the compile-time signature into the runtime identity, then let any_unit own its encoded form
			unit_identity id;
			id.terms.assign(sig.begin(), sig.end());

			return any_unit{std::move(id), base};
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      decodes a self-describing byte stream into an erased quantity
	/// @details    Decodes exactly one record from the front of `bytes`; any trailing bytes (a following record) are
	///             ignored. The decoded record's byte length is `result->size()` — a reader consuming a sequence
	///             advances by that to reach the next record (this is how `operator>>` reads records back-to-back).
	/// @param[in]  bytes  the encoded stream
	/// @return     an `any_unit` on success, else a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	[[nodiscard]] inline std::expected<any_unit, deserialize_error> deserialize(std::span<const std::byte> bytes)
	{
		const std::byte* cursor = bytes.data();
		const std::byte* end    = bytes.data() + bytes.size();

		if (cursor == end)
			return std::unexpected(deserialize_error::truncated);
		const std::uint8_t version = std::to_integer<std::uint8_t>(*cursor++);
		if (version != detail::serialization_version)
			return std::unexpected(deserialize_error::bad_version);

		if (cursor == end)
			return std::unexpected(deserialize_error::truncated);
		const std::uint8_t header  = std::to_integer<std::uint8_t>(*cursor++);
		const auto         kind    = static_cast<detail::value_kind>(header & 0x03);
		const bool         fracExp = (header & 0x04) != 0;

		std::uint64_t count = 0;
		if (!detail::get_uvarint(cursor, end, count))
			return std::unexpected(deserialize_error::truncated);

		unit_identity id;
		id.terms.reserve(count);
		for (std::uint64_t i = 0; i < count; ++i)
		{
			if (end - cursor < 8)
				return std::unexpected(deserialize_error::truncated);
			std::uint64_t hash = 0;
			for (unsigned int byteIndex = 0; byteIndex < 8; ++byteIndex)
				hash |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * byteIndex);
			std::int64_t num = 0;
			std::int64_t den = 1;
			if (!detail::get_svarint(cursor, end, num))
				return std::unexpected(deserialize_error::truncated);
			if (fracExp && !detail::get_svarint(cursor, end, den))
				return std::unexpected(deserialize_error::truncated);
			id.terms.push_back(dimension_term{hash, num, den});
		}

		double base = 0.0;
		switch (kind)
		{
		case detail::value_kind::ivarint:
		{
			std::int64_t v;
			if (!detail::get_svarint(cursor, end, v))
				return std::unexpected(deserialize_error::truncated);
			base = static_cast<double>(v);
			break;
		}
		case detail::value_kind::f32:
		{
			if (end - cursor < 4)
				return std::unexpected(deserialize_error::truncated);
			std::uint32_t bits = 0;
			for (unsigned int i = 0; i < 4; ++i)
				bits |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * i);
			float f;
			std::memcpy(&f, &bits, sizeof(f));
			base = static_cast<double>(f);
			break;
		}
		case detail::value_kind::f64:
		{
			if (end - cursor < 8)
				return std::unexpected(deserialize_error::truncated);
			std::uint64_t bits = 0;
			for (unsigned int i = 0; i < 8; ++i)
				bits |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(*cursor++)) << (8 * i);
			std::memcpy(&base, &bits, sizeof(base));
			break;
		}
		default: return std::unexpected(deserialize_error::bad_version);
		}

		return any_unit(std::move(id), base);
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      decodes a self-describing byte stream directly into a known unit type (fully static)
	/// @tparam     Unit  the expected unit type
	/// @param[in]  bytes  the encoded stream
	/// @return     the value as `Unit` on success, else a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	template<class Unit>
	[[nodiscard]] std::expected<Unit, deserialize_error> deserialize(std::span<const std::byte> bytes)
	{
		static_assert(traits::is_unit_v<Unit>,
			"wpi::units::deserialize<T>(bytes) decodes into a unit type (e.g. deserialize<meters<double>>). The requested type is not a unit; use deserialize(bytes) for an erased any_unit.");
		auto erased = deserialize(bytes);
		if (!erased)
			return std::unexpected(erased.error());
		return erased->template to<Unit>();
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      reads and decodes one self-describing erased quantity from a binary stream, in a single expression
	/// @details    The stream counterpart to `deserialize(span)`: `auto q = deserialize(file);` reads the next record
	///             written by `stream << serialize(q)` with no pre-declared value and no manual buffer. A record is
	///             self-delimiting, so this decodes exactly one and rewinds the stream to just past it (a subsequent
	///             read gets the following record); the stream must be seekable (a file or memory stream, opened in
	///             binary mode) and, for a non-seekable stream such as a live socket, frame the records yourself and
	///             call `deserialize(span)` on each frame.
	/// @param[in]  is  the input stream, positioned at the start of a record
	/// @return     the decoded `any_unit`, or a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	[[nodiscard]] inline std::expected<any_unit, deserialize_error> deserialize(std::istream& is)
	{
		const std::istream::pos_type start = is.tellg();
		if (start == std::istream::pos_type(-1))
			return std::unexpected(deserialize_error::truncated); // not seekable: records can't be self-delimited here

		const std::vector<char> buffer{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
		is.clear(); // the drain set eofbit; clear it so a good decode leaves the stream usable

		auto decoded = deserialize(std::span<const std::byte>(reinterpret_cast<const std::byte*>(buffer.data()), buffer.size()));
		if (decoded)
			is.seekg(start + static_cast<std::istream::off_type>(decoded->size())); // rewind past exactly this record
		return decoded;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: operator>> [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      reads one self-describing erased quantity from a binary stream (classic stream-extraction form)
	/// @details    The counterpart to `operator<<`: `stream >> value` decodes the next record written by
	///             `stream << serialize(q)`, setting the stream's `failbit` (and leaving `value` unchanged) on a
	///             malformed or truncated record. Equivalent to `deserialize(stream)`; use that overload when the
	///             richer `deserialize_error` is wanted. Same seekable-stream requirement.
	/// @param[in]  is     the input stream
	/// @param[out] value  receives the decoded quantity on success
	/// @return     the stream
	//----------------------------------------------------------------------------------------------------------------------
	inline std::istream& operator>>(std::istream& is, any_unit& value)
	{
		if (auto decoded = deserialize(is))
			value = std::move(*decoded);
		else
			is.setstate(std::ios::failbit);
		return is;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//      FUNCTION: deserialize [public]
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief      reads and decodes one record from a binary stream directly into a known unit type
	/// @details    The single-check read: `auto q = deserialize<meters<double>>(file);` reads the next record and
	///             collapses it in one step, so one `if (q)` guards both the read and the dimension match (a mismatch
	///             or a malformed record is a `deserialize_error`). Same seekable-stream requirement as
	///             `deserialize(std::istream&)`.
	/// @tparam     Unit  the expected unit type
	/// @param[in]  is  the input stream, positioned at the start of a record
	/// @return     the value as `Unit` on success, else a `deserialize_error`
	//----------------------------------------------------------------------------------------------------------------------
	template<class Unit>
	[[nodiscard]] std::expected<Unit, deserialize_error> deserialize(std::istream& is)
	{
		static_assert(traits::is_unit_v<Unit>,
			"wpi::units::deserialize<T>(stream) decodes into a unit type (e.g. deserialize<meters<double>>). The requested type is not a unit; use deserialize(stream) for an erased any_unit.");
		auto erased = deserialize(is);
		if (!erased)
			return std::unexpected(erased.error());
		return erased->template to<Unit>();
	}
} // namespace wpi::units

//----------------------------------------------------------------------------------------------------------------------
//	std::hash<wpi::units::any_unit>
//----------------------------------------------------------------------------------------------------------------------
/// @brief	hashes an erased quantity by its dimension signature and SI-base magnitude, so `any_unit` is usable as an
///			unordered-container key. Consistent with `operator==`: two erased quantities that compare equal (same
///			dimension and base value) hash equally.
template<>
struct std::hash<wpi::units::any_unit>
{
	std::size_t operator()(const wpi::units::any_unit& value) const noexcept
	{
		// FNV-1a-style fold over the term signature, mixed with the base-value hash
		std::size_t seed = std::hash<double>()(value.value_in_base());
		const auto  mix  = [&seed](std::size_t h) noexcept { seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2); };
		for (const auto& term : value.identity().terms)
		{
			mix(std::hash<std::uint64_t>()(term.hash));
			mix(std::hash<std::int64_t>()(term.num));
			mix(std::hash<std::int64_t>()(term.den));
		}
		return seed;
	}
};

#endif // units_serialization_h_
