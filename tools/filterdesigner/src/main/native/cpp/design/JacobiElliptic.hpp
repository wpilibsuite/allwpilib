// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <complex>

namespace wpi::filterdesigner::detail {

using cplx = std::complex<double>;

/**
 * Complete elliptic integral of the first kind, @c K(m), via the
 * arithmetic-geometric mean iteration.
 *
 * @param m Parameter (m = k², where k is the modulus). Domain: [0, 1].
 *          @c m=0 returns @c π/2; @c m=1 returns @c +∞.
 */
double EllipticK(double m);

/** Jacobi elliptic functions evaluated at a single point. */
struct JacobiResult {
  double sn;
  double cn;
  double dn;
};

/**
 * Jacobi elliptic functions @c sn(u, m), @c cn(u, m), @c dn(u, m) for real
 * @a u and parameter @a m ∈ [0, 1]. Computed via the descending Landen
 * transformation followed by ascending recovery — the same scheme used by
 * Numerical Recipes and (under the hood) SciPy's @c special.ellipj.
 */
JacobiResult Ellipj(double u, double m);

/**
 * Inverse Jacobi @c sn: solves @c sn(z, m) = w for @a z, where @a w may be
 * complex. Used by the elliptic filter design to compute @c v0.
 *
 * Implements the descending-Landen iteration from Orfanidis,
 * "Lecture Notes on Elliptic Filter Design", Eq. (56).
 */
cplx InverseJacobiSn(cplx w, double m);

/**
 * Real inverse Jacobi @c sc with complementary modulus: solves
 * @c sc(z, 1-m) = w for real @a z. Equivalent to scipy's
 * @c _arc_jac_sc1(w, m).
 */
double InverseJacobiSc1(double w, double m);

/**
 * Solves the elliptic degree equation
 *   N · K(m) / K(1-m) = K(m1) / K(1-m1)
 * for @a m given the order @a N and the small modulus parameter @a m1.
 * Uses the q-nome series of Orfanidis Eq. (49).
 */
double EllipticDegree(int N, double m1);

}  // namespace wpi::filterdesigner::detail
