// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "BilinearTransform.hpp"

#include <cmath>
#include <numbers>

#include "Zpk.hpp"

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
