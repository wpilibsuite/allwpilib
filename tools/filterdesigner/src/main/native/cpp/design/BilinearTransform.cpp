// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>
#include <utility>

#include "Zpk.hpp"

namespace wpi::filterdesigner::detail {

namespace {

int RelativeDegree(const Zpk& p) {
  return static_cast<int>(p.poles.size()) - static_cast<int>(p.zeros.size());
}

}  // namespace

double PreWarp(double fc, double fs) {
  return 2.0 * fs * std::tan(std::numbers::pi * fc / fs);
}

Zpk ButterworthPrototype(int order) {
  Zpk p;
  p.gain = 1.0;
  for (int k = 0; k < order; ++k) {
    double angle = std::numbers::pi / 2.0 +
                   std::numbers::pi * (2.0 * k + 1.0) / (2.0 * order);
    p.poles.push_back(std::polar(1.0, angle));
  }
  return p;
}

Zpk AnalogLpToLp(const Zpk& p, double wo) {
  Zpk out;
  out.gain = p.gain;
  for (auto& z : p.zeros) {
    out.zeros.push_back(z * wo);
  }
  for (auto& pole : p.poles) {
    out.poles.push_back(pole * wo);
  }
  out.gain *= std::pow(wo, RelativeDegree(p));
  return out;
}

Zpk AnalogLpToHp(const Zpk& p, double wo) {
  // Mirror: s → wo/s. Finite zeros/poles invert and scale; zeros at infinity
  // become zeros at the origin to balance the pole count.
  Zpk out;
  cplx zProd = 1.0;
  cplx pProd = 1.0;
  for (auto& z : p.zeros) {
    out.zeros.push_back(wo / z);
    zProd *= -z;
  }
  for (auto& pole : p.poles) {
    out.poles.push_back(wo / pole);
    pProd *= -pole;
  }
  int degree = RelativeDegree(p);
  for (int i = 0; i < degree; ++i) {
    out.zeros.emplace_back(0.0, 0.0);
  }
  out.gain = p.gain * (zProd / pProd).real();
  return out;
}

// Each finite analog root r of the prototype maps to the two roots of
//   s² - bw·r·s + wo² = 0 (bandpass)
//   s² - (bw/r)·s + wo² = 0 (bandstop)
static std::pair<cplx, cplx> BpRoots(cplx rScaled, double wo) {
  cplx disc = std::sqrt(rScaled * rScaled - wo * wo);
  return {rScaled + disc, rScaled - disc};
}

Zpk AnalogLpToBp(const Zpk& p, double wo, double bw) {
  Zpk out;
  for (auto& z : p.zeros) {
    auto [z1, z2] = BpRoots(z * bw * 0.5, wo);
    out.zeros.push_back(z1);
    out.zeros.push_back(z2);
  }
  for (auto& pole : p.poles) {
    auto [p1, p2] = BpRoots(pole * bw * 0.5, wo);
    out.poles.push_back(p1);
    out.poles.push_back(p2);
  }
  int degree = RelativeDegree(p);
  for (int i = 0; i < degree; ++i) {
    out.zeros.emplace_back(0.0, 0.0);
  }
  out.gain = p.gain * std::pow(bw, degree);
  return out;
}

Zpk AnalogLpToBs(const Zpk& p, double wo, double bw) {
  Zpk out;
  cplx zProd = 1.0;
  cplx pProd = 1.0;
  for (auto& z : p.zeros) {
    auto [z1, z2] = BpRoots(bw * 0.5 / z, wo);
    out.zeros.push_back(z1);
    out.zeros.push_back(z2);
    zProd *= -z;
  }
  for (auto& pole : p.poles) {
    auto [p1, p2] = BpRoots(bw * 0.5 / pole, wo);
    out.poles.push_back(p1);
    out.poles.push_back(p2);
    pProd *= -pole;
  }
  int degree = RelativeDegree(p);
  const cplx jwo{0.0, wo};
  for (int i = 0; i < degree; ++i) {
    out.zeros.push_back(jwo);
    out.zeros.push_back(-jwo);
  }
  out.gain = p.gain * (zProd / pProd).real();
  return out;
}

std::optional<Sections> DesignFromAnalogLp(const Zpk& analogLp, FilterKind kind,
                                           double fs, double f1, double f2) {
  Zpk analog = analogLp;
  switch (kind) {
    case FilterKind::LowPass:
      analog = AnalogLpToLp(analog, PreWarp(f1, fs));
      break;
    case FilterKind::HighPass:
      analog = AnalogLpToHp(analog, PreWarp(f1, fs));
      break;
    case FilterKind::BandPass: {
      const double w1 = PreWarp(f1, fs);
      const double w2 = PreWarp(f2, fs);
      const double wo = std::sqrt(w1 * w2);
      const double bw = w2 - w1;
      analog = AnalogLpToBp(analog, wo, bw);
      break;
    }
    case FilterKind::BandStop: {
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

}  // namespace wpi::filterdesigner::detail
