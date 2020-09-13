/*################################################################################
  ##
  ##   Copyright (C) 2016-2020 Keith O'Hara
  ##
  ##   This file is part of the GCE-Math C++ library.
  ##
  ##   Licensed under the Apache License, Version 2.0 (the "License");
  ##   you may not use this file except in compliance with the License.
  ##   You may obtain a copy of the License at
  ##
  ##       http://www.apache.org/licenses/LICENSE-2.0
  ##
  ##   Unless required by applicable law or agreed to in writing, software
  ##   distributed under the License is distributed on an "AS IS" BASIS,
  ##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##   See the License for the specific language governing permissions and
  ##   limitations under the License.
  ##
  ################################################################################*/

#pragma once

#include "gcem/gcem_incl/gcem_options.hpp"

namespace gcem {
#include "gcem/gcem_incl/quadrature/gauss_legendre_50.hpp"

#include "gcem/gcem_incl/is_inf.hpp"
#include "gcem/gcem_incl/is_nan.hpp"
#include "gcem/gcem_incl/is_finite.hpp"

#include "gcem/gcem_incl/abs.hpp"
#include "gcem/gcem_incl/ceil.hpp"
#include "gcem/gcem_incl/floor.hpp"
#include "gcem/gcem_incl/trunc.hpp"
#include "gcem/gcem_incl/is_odd.hpp"
#include "gcem/gcem_incl/is_even.hpp"
#include "gcem/gcem_incl/max.hpp"
#include "gcem/gcem_incl/min.hpp"
#include "gcem/gcem_incl/sqrt.hpp"

#include "gcem/gcem_incl/signbit.hpp"
#include "gcem/gcem_incl/copysign.hpp"
#include "gcem/gcem_incl/neg_zero.hpp"
#include "gcem/gcem_incl/sgn.hpp"

#include "gcem/gcem_incl/find_exponent.hpp"
#include "gcem/gcem_incl/find_fraction.hpp"
#include "gcem/gcem_incl/find_whole.hpp"
#include "gcem/gcem_incl/mantissa.hpp"
#include "gcem/gcem_incl/round.hpp"
#include "gcem/gcem_incl/fmod.hpp"

#include "gcem/gcem_incl/pow_integral.hpp"
#include "gcem/gcem_incl/exp.hpp"
#include "gcem/gcem_incl/expm1.hpp"
#include "gcem/gcem_incl/log.hpp"
#include "gcem/gcem_incl/log1p.hpp"
#include "gcem/gcem_incl/log2.hpp"
#include "gcem/gcem_incl/pow.hpp"

#include "gcem/gcem_incl/gcd.hpp"
#include "gcem/gcem_incl/lcm.hpp"

#include "gcem/gcem_incl/tan.hpp"
#include "gcem/gcem_incl/cos.hpp"
#include "gcem/gcem_incl/sin.hpp"

#include "gcem/gcem_incl/atan.hpp"
#include "gcem/gcem_incl/atan2.hpp"
#include "gcem/gcem_incl/acos.hpp"
#include "gcem/gcem_incl/asin.hpp"

#include "gcem/gcem_incl/tanh.hpp"
#include "gcem/gcem_incl/cosh.hpp"
#include "gcem/gcem_incl/sinh.hpp"

#include "gcem/gcem_incl/atanh.hpp"
#include "gcem/gcem_incl/acosh.hpp"
#include "gcem/gcem_incl/asinh.hpp"

#include "gcem/gcem_incl/binomial_coef.hpp"
#include "gcem/gcem_incl/lgamma.hpp"
#include "gcem/gcem_incl/tgamma.hpp"
#include "gcem/gcem_incl/factorial.hpp"
#include "gcem/gcem_incl/lbeta.hpp"
#include "gcem/gcem_incl/beta.hpp"
#include "gcem/gcem_incl/lmgamma.hpp"
#include "gcem/gcem_incl/log_binomial_coef.hpp"

#include "gcem/gcem_incl/erf.hpp"
#include "gcem/gcem_incl/erf_inv.hpp"
#include "gcem/gcem_incl/incomplete_beta.hpp"
#include "gcem/gcem_incl/incomplete_beta_inv.hpp"
#include "gcem/gcem_incl/incomplete_gamma.hpp"
#include "gcem/gcem_incl/incomplete_gamma_inv.hpp"
}  // namespace gcem
