// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <complex>

namespace wpi::math::filter::internal {

using cplx = std::complex<double>;

/**
 * Complete elliptic integral of the first kind, K(m), via the
 * arithmetic-geometric mean iteration.
 *
 * @param m Parameter (m = k², where k is the modulus). Domain: [0, 1].
 *          m=0 returns π/2; m=1 returns +∞.
 */
double EllipticK(double m);

/** Jacobi elliptic functions evaluated at a single point. */
struct JacobiResult {
  double sn;
  double cn;
  double dn;
};

/**
 * Jacobi elliptic functions sn(u, m), cn(u, m), dn(u, m) for real u and
 * parameter m ∈ [0, 1]. Computed via the descending Landen transformation
 * followed by ascending recovery — the same scheme used by Numerical Recipes
 * and (under the hood) SciPy's special.ellipj.
 */
JacobiResult Ellipj(double u, double m);

/**
 * Inverse Jacobi sn: solves sn(z, m) = w for z, where w may be complex. Used
 * by the elliptic filter design to compute v0.
 *
 * Implements the descending-Landen iteration from Orfanidis, "Lecture Notes
 * on Elliptic Filter Design", Eq. (56).
 */
cplx InverseJacobiSn(cplx w, double m);

/**
 * Real inverse Jacobi sc with complementary modulus: solves sc(z, 1-m) = w
 * for real z. Equivalent to scipy's _arc_jac_sc1(w, m).
 */
double InverseJacobiSc1(double w, double m);

/**
 * Solves the elliptic degree equation
 *   N · K(m) / K(1-m) = K(m1) / K(1-m1)
 * for m given the order N and the small modulus parameter m1. Uses the
 * q-nome series of Orfanidis Eq. (49).
 */
double EllipticDegree(int N, double m1);

}  // namespace wpi::math::filter::internal
