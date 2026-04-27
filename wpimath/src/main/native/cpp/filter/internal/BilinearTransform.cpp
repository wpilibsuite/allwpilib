// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "BilinearTransform.hpp"

#include <cmath>
#include <numbers>

#include "Zpk.hpp"

// Standard analog→digital conversion: pre-warp the digital cutoff so the
// post-bilinear digital response hits the requested edge exactly, then map
// each analog pole p (resp. zero z) via s → 2·fs·(z-1)/(z+1):
//
//   p_d = (2fs + p) / (2fs - p)
//
// Background:
//   - https://en.wikipedia.org/wiki/Bilinear_transform
//   - Oppenheim & Schafer, "Discrete-Time Signal Processing" §7.2.2
// SciPy implementations to compare against, line for line:
//   https://github.com/scipy/scipy/blob/main/scipy/signal/_filter_design.py
// (functions bilinear_zpk and _zpkbilinear; constant prewarping is folded
// into the lp2{lp,hp,bp,bs}_zpk callers above the bilinear step).

namespace wpi::math::filter::internal {

namespace {

int RelativeDegree(const Zpk& p) {
  return static_cast<int>(p.poles.size()) - static_cast<int>(p.zeros.size());
}

}  // namespace

double PreWarp(double fc, double fs) {
  return 2.0 * fs * std::tan(std::numbers::pi * fc / fs);
}

Zpk BilinearTransform(const Zpk& analog, double fs) {
  Zpk out;
  double fs2 = 2.0 * fs;
  cplx zNumProd = 1.0;
  cplx zDenProd = 1.0;
  for (auto& z : analog.zeros) {
    out.zeros.push_back((fs2 + z) / (fs2 - z));
    zNumProd *= (fs2 - z);
  }
  for (auto& p : analog.poles) {
    out.poles.push_back((fs2 + p) / (fs2 - p));
    zDenProd *= (fs2 - p);
  }
  // Analog filters with fewer zeros than poles have `degree` zeros at s=∞.
  // The bilinear maps s=∞ to z=-1 (Nyquist), so materialize them here. This
  // is what gives a Butterworth low-pass its N digital zeros at Nyquist and
  // hence its hard rolloff at the top of the band.
  int degree = RelativeDegree(analog);
  for (int i = 0; i < degree; ++i) {
    out.zeros.emplace_back(-1.0, 0.0);
  }
  out.gain = analog.gain * (zNumProd / zDenProd).real();
  return out;
}

Sections DesignFromAnalogLp(const Zpk& analogLp,
                            wpi::math::BiquadFilter::Kind kind, double fs,
                            double f1, double f2) {
  // Pipeline:
  //   1. Pre-warp the requested digital cutoff(s) into the analog cutoff
  //      that maps back to them under the bilinear transform.
  //   2. Reshape the 1 rad/s LP prototype with a kind-specific s-plane
  //      substitution (LP→LP/HP/BP/BS), giving an analog filter at the
  //      requested kind and cutoff.
  //   3. Bilinear-transform the resulting analog ZPK to a digital ZPK
  //      (s-plane → z-plane).
  //   4. Pair conjugate digital roots into a cascade of real-coefficient
  //      biquad sections.
  using Kind = wpi::math::BiquadFilter::Kind;
  Zpk analog = analogLp;
  switch (kind) {
    case Kind::LowPass:
      analog = AnalogLpToLp(analog, PreWarp(f1, fs));
      break;
    case Kind::HighPass:
      analog = AnalogLpToHp(analog, PreWarp(f1, fs));
      break;
    case Kind::BandPass: {
      const double w1 = PreWarp(f1, fs);
      const double w2 = PreWarp(f2, fs);
      const double wo = std::sqrt(w1 * w2);
      const double bw = w2 - w1;
      analog = AnalogLpToBp(analog, wo, bw);
      break;
    }
    case Kind::BandStop: {
      const double w1 = PreWarp(f1, fs);
      const double w2 = PreWarp(f2, fs);
      const double wo = std::sqrt(w1 * w2);
      const double bw = w2 - w1;
      analog = AnalogLpToBs(analog, wo, bw);
      break;
    }
  }
  return ZpkToSos(BilinearTransform(analog, fs));
}

}  // namespace wpi::math::filter::internal
