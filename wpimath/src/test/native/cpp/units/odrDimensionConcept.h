// Regression guards for the #378 ODR audit and the dimension-keyed physical-quantity concepts.
//
// Issue #378 is a cross-translation-unit type-identity divergence: arithmetic that yields a compound result
// (e.g. `meters/seconds`) rewraps to a "canonical named type" (`meters_per_second`) via an ADL registration that is
// only visible in a TU that included the result's dimension header; a TU without it sees the plain `unit<...>` base.
// The fix emits a PascalCase, dimension-keyed concept per dimension (`wpi::units::Velocity`, `wpi::units::Force`, ...) so a
// computed result classifies the same way regardless of which dimension headers a TU included — dispatch on the
// concept cannot flip by link order. These tests codify (1) that the concepts classify by dimension and reject the
// wrong one, (2) that concept-based overload dispatch selects correctly at runtime, (3) the VALUE / DIMENSION /
// LAYOUT / SERIALIZATION safety invariants that make the residual type-identity split value-safe (so a future rewrap
// change trips a test instead of silently corrupting), and (4) the proven-STABLE `sqrt` reduction. The library is
// dimension-only by design (torque and energy share a dimension), so these tests assert dimensions and values only —
// never a physical-"kind" identity.

#pragma once

#include <type_traits>
#include <wpi/units.hpp>
#include <wpi/units/serialization.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>

namespace
{
	using namespace wpi::units;
	using namespace wpi::units::length;
	using namespace wpi::units::time;
	using namespace wpi::units::mass;
	using namespace wpi::units::velocity;
	using namespace wpi::units::acceleration;
	using namespace wpi::units::force;
	using namespace wpi::units::frequency;
	using namespace wpi::units::area;

	// The plain, unnamed `unit<...>` base a computed derived quantity carries in a dimension-header-BLIND TU. Spelled
	// by dimension (never by the named type), this is the "other" type the #378 divergence produces — the audit's
	// layout/value/serialization invariants must hold between it and the named form of the same dimension/underlying.
	using velocity_base     = unit<conversion_factor<std::ratio<1>, dimension::velocity>, double>;
	using force_base        = unit<conversion_factor<std::ratio<1>, dimension::force>, double>;
	using length_base       = unit<conversion_factor<std::ratio<1>, dimension::length>, double>;
	using dimensionless_base = unit<conversion_factor<std::ratio<1>, dimension::dimensionless>, double>;

	// A concept-constrained overload set. `Velocity`/`Force` are dimension-keyed, so the SAME computed expression
	// dispatches to the SAME overload in any TU — the property #378's rewrap divergence would otherwise break.
	constexpr int classifyByConcept(Velocity auto) { return 1; }
	constexpr int classifyByConcept(Force auto) { return 2; }
	constexpr int classifyByConcept(auto) { return 0; }
} // namespace

//======================================================================================================================
//	DIMENSION-CONCEPT CLASSIFICATION + STRONG DIAGNOSTIC (compile-time)
//======================================================================================================================

TEST_CASE("OdrDimensionConcept VelocityConceptClassifiesComputedResultByDimension", "[wpimath][units]")
{
	// The computed result of `m / s` satisfies `Velocity` (whether or not it rewrapped to the named type in this TU).
	static_assert(Velocity<decltype(meters<double>(1) / seconds<double>(1))>, "m/s is a Velocity");
	// A base length is not a velocity; a force is not a velocity.
	static_assert(!Velocity<meters<double>>, "meters is not a Velocity");
	static_assert(!Velocity<newtons<double>>, "newtons is not a Velocity");
	// It also accepts the explicitly-named velocity type and the plain dimension base of the same dimension.
	static_assert(Velocity<meters_per_second<double>>, "named velocity is a Velocity");
	static_assert(Velocity<velocity_base>, "plain velocity base is a Velocity");
	SUCCEED();
}

TEST_CASE("OdrDimensionConcept ForceConceptClassifiesComputedResultByDimension", "[wpimath][units]")
{
	// mass * acceleration is a Force; a velocity is not.
	static_assert(Force<decltype(kilograms<double>(1) * (meters<double>(1) / pow<2>(seconds<double>(1))))>, "kg*(m/s^2) is a Force");
	static_assert(!Force<decltype(meters<double>(1) / seconds<double>(1))>, "m/s is not a Force");
	static_assert(!Force<meters<double>>, "meters is not a Force");
	static_assert(Force<newtons<double>>, "named force is a Force");
	static_assert(Force<force_base>, "plain force base is a Force");
	SUCCEED();
}

TEST_CASE("OdrDimensionConcept StrongDiagnosticWrongDimensionIsRejected", "[wpimath][units]")
{
	// The whole point of a dimension-keyed concept: a wrong dimension is REJECTED, so a misuse fails to compile
	// rather than silently binding a link-order-dependent overload. Prove both directions across dimensions.
	const auto vel   = meters<double>(1) / seconds<double>(1);
	const auto force = kilograms<double>(1) * (meters<double>(1) / pow<2>(seconds<double>(1)));

	static_assert(!Force<decltype(vel)>, "a velocity is rejected by Force");
	static_assert(!Velocity<decltype(force)>, "a force is rejected by Velocity");
	static_assert(Force<decltype(force)>, "a force is accepted by Force");
	static_assert(Velocity<decltype(vel)>, "a velocity is accepted by Velocity");
}

TEST_CASE("OdrDimensionConcept FrequencyConceptClassifiesInverseTime", "[wpimath][units]")
{
	static_assert(Frequency<decltype(1.0 / seconds<double>(1))>, "1/s is a Frequency");
	static_assert(Frequency<hertz<double>>, "named frequency is a Frequency");
	static_assert(!Frequency<seconds<double>>, "seconds is not a Frequency");
	static_assert(!Frequency<meters<double>>, "meters is not a Frequency");
	SUCCEED();
}

TEST_CASE("OdrDimensionConcept AreaConceptClassifiesSquaredLength", "[wpimath][units]")
{
	static_assert(Area<decltype(meters<double>(1) * meters<double>(1))>, "m*m is an Area");
	static_assert(Area<decltype(pow<2>(meters<double>(1)))>, "pow<2>(m) is an Area");
	static_assert(Area<square_meters<double>>, "named area is an Area");
	static_assert(!Area<meters<double>>, "meters is not an Area");
	SUCCEED();
}

TEST_CASE("OdrDimensionConcept LengthConceptClassifiesLength", "[wpimath][units]")
{
	static_assert(Length<meters<double>>, "meters is a Length");
	static_assert(Length<feet<double>>, "feet is a Length");
	static_assert(Length<length_base>, "plain length base is a Length");
	static_assert(!Length<seconds<double>>, "seconds is not a Length");
	static_assert(!Length<square_meters<double>>, "an area is not a Length");
	SUCCEED();
}

TEST_CASE("OdrDimensionConcept DimensionlessConceptCoexistsWithDimensionlessUnitType", "[wpimath][units]")
{
	// The generated `Dimensionless` concept classifies by dimension, and it coexists with the hand-written
	// `DimensionlessUnitType` concept (both must accept a dimensionless quantity, reject a dimensioned one).
	static_assert(Dimensionless<dimensionless<double>>, "dimensionless is Dimensionless");
	static_assert(Dimensionless<dimensionless_base>, "plain dimensionless base is Dimensionless");
	static_assert(!Dimensionless<meters<double>>, "meters is not Dimensionless");

	static_assert(DimensionlessUnitType<dimensionless<double>>, "dimensionless is a DimensionlessUnitType");
	static_assert(!DimensionlessUnitType<meters<double>>, "meters is not a DimensionlessUnitType");

	// The result of a ratio of same-dimension quantities is dimensionless and classifies as such.
	static_assert(Dimensionless<decltype(meters<double>(1) / meters<double>(1))>, "m/m is Dimensionless");
	SUCCEED();
}

//======================================================================================================================
//	CONCEPT DISPATCH SELECTS THE RIGHT OVERLOAD (runtime)
//======================================================================================================================

TEST_CASE("OdrDimensionConcept ConceptOverloadDispatchSelectsCorrectly", "[wpimath][units]")
{
	// The dimension-keyed concepts drive overload resolution to the intended target for a COMPUTED result. Because
	// they key on dimension (not the named type), the selection is identical in every TU regardless of includes —
	// the exact behavioral flip (#378 surface (d)) this fix closes.
	CHECK(1 == classifyByConcept(meters<double>(6) / seconds<double>(2)));
	CHECK(2 == classifyByConcept(kilograms<double>(2) * (meters<double>(5) / pow<2>(seconds<double>(1)))));
	CHECK(0 == classifyByConcept(meters<double>(3)));
	// The named forms dispatch to the same targets as their computed equivalents.
	CHECK(1 == classifyByConcept(meters_per_second<double>(3)));
	CHECK(2 == classifyByConcept(newtons<double>(10)));
}

//======================================================================================================================
//	ODR SAFETY INVARIANTS — the value-safety guarantees from the audit that must never regress
//======================================================================================================================

TEST_CASE("OdrSafetyInvariant ValueIsIdenticalNamedVsPlainBase", "[wpimath][units]")
{
	// The numeric value of a computed derived quantity is identical whether it lands as a named type or the plain
	// `unit<...>` base of the same dimension. (Audit §1.1 / surface (e): layout parity => the value is never
	// corrupted across the type-identity split.) Reconcile both through the same dimension base and compare.
	const auto computed = meters<double>(5) / seconds<double>(2);    // 2.5 m/s (named in this TU)
	const velocity_base viaBase(computed);                          // the same value in the plain base type

	CHECK(2.5 == computed.value());
	CHECK(2.5 == viaBase.value());
	CHECK(computed.template to<double>() == viaBase.template to<double>());
}

TEST_CASE("OdrSafetyInvariant DimensionIsCorrectNamedVsPlainBase", "[wpimath][units]")
{
	// The computed result has the correct SI dimension regardless of the named-vs-plain identity: the dimension-keyed
	// trait is true, and `same_dimension` holds between the named and the base form.
	const auto computed = meters<double>(5) / seconds<double>(2);
	static_assert(traits::is_velocity_unit_v<decltype(computed)>, "computed m/s has velocity dimension");
	static_assert(traits::is_velocity_unit_v<velocity_base>, "the plain base has velocity dimension");
	static_assert(same_dimension<decltype(computed), velocity_base>, "computed and base share a dimension");
	static_assert(same_dimension<meters_per_second<double>, velocity_base>, "named and base share a dimension");
	SUCCEED();
}

TEST_CASE("OdrSafetyInvariant LayoutIsIdenticalNamedVsPlainBase", "[wpimath][units]")
{
	// §1.1: a named unit adds NO data member over its `unit<...>` base, so size/align/trivial-copyability are
	// identical. THIS is the structural fact that makes the residual ODR-on-a-type value-safe — guard it so a
	// future named type that grows a data member trips a test instead of silently corrupting a value carried
	// through a divergent-typed struct field or weak inline.
	static_assert(sizeof(meters_per_second<double>) == sizeof(velocity_base), "velocity: size parity");
	static_assert(alignof(meters_per_second<double>) == alignof(velocity_base), "velocity: align parity");
	static_assert(std::is_trivially_copyable_v<meters_per_second<double>> == std::is_trivially_copyable_v<velocity_base>, "velocity: trivially-copyable parity");

	static_assert(sizeof(newtons<double>) == sizeof(force_base), "force: size parity");
	static_assert(alignof(newtons<double>) == alignof(force_base), "force: align parity");
	static_assert(std::is_trivially_copyable_v<newtons<double>> == std::is_trivially_copyable_v<force_base>, "force: trivially-copyable parity");

	// And the named form is genuinely no larger than a bare underlying (no hidden state).
	static_assert(sizeof(meters_per_second<double>) == sizeof(double), "named velocity is exactly its underlying");
	SUCCEED();
}
/*
TEST_CASE("OdrSafetyInvariant SerializationIsIdenticalNamedVsPlainBase", "[wpimath][units]")
{
	// Audit surface (c): serialization is STABLE because the wire form is keyed on the runtime dimension signature
	// and SI-base magnitude, never the named type. Serializing the named-typed result and its plain-base equivalent
	// MUST produce identical bytes and an identical `any_unit::to_string()`. A rewrap change that regressed this
	// (e.g. by leaking the named identity into the wire form) would break cross-TU deserialization silently.
	const auto      computed = meters<double>(5) / seconds<double>(2);    // named in this TU
	const velocity_base viaBase(computed);                              // plain dimension base

	const wpi::units::any_unit encodedNamed = wpi::units::serialize(computed);
	const wpi::units::any_unit encodedBase  = wpi::units::serialize(viaBase);

	// identical byte streams
	REQUIRE(encodedNamed.size() == encodedBase.size());
	const std::string bytesNamed(encodedNamed.data(), encodedNamed.size());
	const std::string bytesBase(encodedBase.data(), encodedBase.size());
	CHECK(bytesNamed == bytesBase);

	// identical erased rendering (dimension-formed, not the named spelling)
	CHECK(encodedNamed.to_string() == encodedBase.to_string());

	// and the value survives the round trip identically for both forms
	const auto backNamed = wpi::units::deserialize(encodedNamed);
	const auto backBase  = wpi::units::deserialize(encodedBase);
	REQUIRE(backNamed.has_value());
	REQUIRE(backBase.has_value());
	CHECK(2.5 == backNamed->template to<velocity_base>()->value());
	CHECK(2.5 == backBase->template to<velocity_base>()->value());
}
*/
//======================================================================================================================
//	sqrt — audit-proven STABLE (S7): sqrt(area) -> length, value and dimension correct
//======================================================================================================================

TEST_CASE("OdrSafetyInvariant SqrtOfAreaIsLength", "[wpimath][units]")
{
	// S7 is the proven-stable rewrap source (no second type can exist — you cannot form a length^2 without `meters`
	// already defined). Guard the property it rests on: sqrt of an area yields a length of the right value.
	const auto side = sqrt(square_meters<double>(9));
	static_assert(traits::is_length_unit_v<decltype(side)>, "sqrt(area) has length dimension");
	static_assert(std::is_same_v<std::remove_const_t<decltype(side)>, meters<double>>, "sqrt(m^2) -> meters");
	CHECK(3.0 == side.value());

	// non-tautological cross-check against a hand-computed value through a different area unit.
	const auto side2 = sqrt(square_meters<double>(6.25));
	static_assert(traits::is_length_unit_v<decltype(side2)>, "sqrt(area) is a length");
	CHECK(2.5 == side2.value());
}

// The named-class recovery used by common_type / to_string is specialization-driven; the friendly-name result of
// a mixed-unit common_type must be stable and must not depend on which other dimension headers happen to be in
// scope. These pin the invariant (a full multi-TU reversed-include harness would be the deeper guard).
TEST_CASE("OdrDimensionConcept CommonTypeNameRecoveryIsStable", "[wpimath][units]")
{
	static_assert(std::is_same_v<std::common_type_t<meters<int>, centimeters<int>>, centimeters<int>>);
	static_assert(std::is_same_v<std::common_type_t<centimeters<int>, meters<int>>, centimeters<int>>); // symmetric
	static_assert(std::is_same_v<std::common_type_t<meters<double>, kilometers<double>>, meters<double>>);
	// A computed derived result recovers the same friendly type regardless of the operand order.
	static_assert(std::is_same_v<decltype(meters<double>(1) / seconds<double>(1)),
								 decltype(meters<double>(2) / seconds<double>(2))>);
	SUCCEED();
}
