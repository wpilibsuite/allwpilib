// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

#pragma once

#include "units/area.h"
#include "units/capacitance.h"
#include "units/charge.h"
#include "units/current.h"
#include "units/dimensionless.h"
#include "units/energy.h"
#include "units/force.h"
#include "units/impedance.h"
#include "units/length.h"
#include "units/magnetic_field_strength.h"
#include "units/mass.h"
#include "units/power.h"
#include "units/substance.h"
#include "units/temperature.h"
#include "units/time.h"
#include "units/velocity.h"

namespace units::constants {
/**
 * @name Unit Containers
 * @anchor constantContainers
 * @{
 */
using PI = unit<std::ratio<1>, dimensionless::scalar, std::ratio<1>>;

static constexpr const unit_t<PI> pi(
    1);  ///< Ratio of a circle's circumference to its diameter.
static constexpr const velocity::meters_per_second_t c(
    299792458.0);  ///< Speed of light in vacuum.
static constexpr const unit_t<
    compound_unit<cubed<length::meters>, inverse<mass::kilogram>,
                  inverse<squared<time::seconds>>>>
    G(6.67408e-11);  ///< Newtonian constant of gravitation.
static constexpr const unit_t<compound_unit<energy::joule, time::seconds>> h(
    6.626070040e-34);  ///< Planck constant.
static constexpr const unit_t<
    compound_unit<force::newtons, inverse<squared<current::ampere>>>>
    mu0(pi * 4.0e-7 * force::newton_t(1) /
        units::math::cpow<2>(current::ampere_t(1)));  ///< vacuum permeability.
static constexpr const unit_t<
    compound_unit<capacitance::farad, inverse<length::meter>>>
    epsilon0(1.0 / (mu0 * math::cpow<2>(c)));  ///< vacuum permitivity.
static constexpr const impedance::ohm_t Z0(
    mu0* c);  ///< characteristic impedance of vacuum.
static constexpr const unit_t<compound_unit<force::newtons, area::square_meter,
                                            inverse<squared<charge::coulomb>>>>
    k_e(1.0 / (4 * pi * epsilon0));  ///< Coulomb's constant.
static constexpr const charge::coulomb_t e(
    1.6021766208e-19);  ///< elementary charge.
static constexpr const mass::kilogram_t m_e(
    9.10938356e-31);  ///< electron mass.
static constexpr const mass::kilogram_t m_p(1.672621898e-27);  ///< proton mass.
static constexpr const unit_t<
    compound_unit<energy::joules, inverse<magnetic_field_strength::tesla>>>
mu_B(e* h / (4 * pi * m_e));  ///< Bohr magneton.
static constexpr const unit_t<inverse<substance::mol>> N_A(
    6.022140857e23);  ///< Avagadro's Number.
static constexpr const unit_t<compound_unit<
    energy::joules, inverse<temperature::kelvin>, inverse<substance::moles>>>
    R(8.3144598);  ///< Gas constant.
static constexpr const unit_t<
    compound_unit<energy::joules, inverse<temperature::kelvin>>>
    k_B(R / N_A);  ///< Boltzmann constant.
static constexpr const unit_t<
    compound_unit<charge::coulomb, inverse<substance::mol>>>
F(N_A* e);  ///< Faraday constant.
static constexpr const unit_t<
    compound_unit<power::watts, inverse<area::square_meters>,
                  inverse<squared<squared<temperature::kelvin>>>>>
    sigma((2 * math::cpow<5>(pi) * math::cpow<4>(R)) /
          (15 * math::cpow<3>(h) * math::cpow<2>(c) *
           math::cpow<4>(N_A)));  ///< Stefan-Boltzmann constant.
/** @} */
}  // namespace units::constants
