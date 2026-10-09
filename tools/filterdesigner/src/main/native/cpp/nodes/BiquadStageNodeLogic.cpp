// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"

#include <cmath>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>

#include "wpi/filterdesigner/model/ApplyFilter.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace wpi::filterdesigner {

namespace {

Sections ToSections(const wpi::math::BiquadFilter& filter) {
  auto span = filter.Sections();
  return Sections(span.begin(), span.end());
}

Sections DesignClassicalKind(wpi::math::BiquadFilter::Kind k, const Stage& s,
                             double fs) {
  using K = wpi::math::BiquadFilter::Kind;
  const auto fsHz = units::hertz_t{fs};
  const auto f1Hz = units::hertz_t{s.f1};
  const auto f2Hz = units::hertz_t{s.f2};
  const bool isBand = (k == K::BandPass || k == K::BandStop);
  switch (s.family) {
    case Family::Butterworth:
      return ToSections(isBand ? wpi::math::BiquadFilter::Butterworth(
                                     k, s.order, fsHz, f1Hz, f2Hz)
                               : wpi::math::BiquadFilter::Butterworth(
                                     k, s.order, fsHz, f1Hz));
    case Family::Chebyshev1:
      return ToSections(
          isBand ? wpi::math::BiquadFilter::ChebyshevI(k, s.order, fsHz, f1Hz,
                                                       f2Hz, s.passbandRippleDb)
                 : wpi::math::BiquadFilter::ChebyshevI(k, s.order, fsHz, f1Hz,
                                                       s.passbandRippleDb));
    case Family::Chebyshev2:
      return ToSections(
          isBand ? wpi::math::BiquadFilter::ChebyshevII(k, s.order, fsHz, f1Hz,
                                                        f2Hz, s.stopbandAttenDb)
                 : wpi::math::BiquadFilter::ChebyshevII(k, s.order, fsHz, f1Hz,
                                                        s.stopbandAttenDb));
    case Family::Elliptic:
      return ToSections(isBand ? wpi::math::BiquadFilter::Elliptic(
                                     k, s.order, fsHz, f1Hz, f2Hz,
                                     s.passbandRippleDb, s.stopbandAttenDb)
                               : wpi::math::BiquadFilter::Elliptic(
                                     k, s.order, fsHz, f1Hz, s.passbandRippleDb,
                                     s.stopbandAttenDb));
  }
  return {};
}

// True when the parameters this stage kind and family actually read are all
// finite. ImGui's number parser turns an overflowing entry such as 1e999 into
// infinity, and a hand-edited file can carry the same; infinity passes every
// ordered check here and in wpi::math::BiquadFilter and reaches the
// coefficient arithmetic. Fields the kind doesn't read are left alone, since
// the UI gives no way to fix one it isn't showing.
bool UsedParametersAreFinite(const Stage& s, double fs) {
  if (!std::isfinite(fs)) {
    return false;
  }
  switch (s.kind) {
    case StageKind::MovingAverage:
      return true;
    case StageKind::Notch:
      return std::isfinite(s.f1) && std::isfinite(s.q);
    case StageKind::BandPass:
    case StageKind::BandStop:
      if (!std::isfinite(s.f2)) {
        return false;
      }
      [[fallthrough]];
    case StageKind::LowPass:
    case StageKind::HighPass:
      break;
  }
  if (!std::isfinite(s.f1)) {
    return false;
  }
  const bool usesRipple =
      s.family == Family::Chebyshev1 || s.family == Family::Elliptic;
  const bool usesAtten =
      s.family == Family::Chebyshev2 || s.family == Family::Elliptic;
  return (!usesRipple || std::isfinite(s.passbandRippleDb)) &&
         (!usesAtten || std::isfinite(s.stopbandAttenDb));
}

// Finite inputs do not guarantee finite coefficients: a notch with a
// subnormal q overflows w0/q to infinity, and tan of that is NaN. Nothing
// downstream rejects a full section list, so the plots would draw nothing and
// export would emit nan literals.
std::pair<Sections, std::string> CheckedSections(Sections sections) {
  for (const Section& sec : sections) {
    if (!std::isfinite(sec.b0) || !std::isfinite(sec.b1) ||
        !std::isfinite(sec.b2) || !std::isfinite(sec.a1) ||
        !std::isfinite(sec.a2)) {
      return {{}, "Parameters produce non-finite coefficients"};
    }
  }
  return {std::move(sections), {}};
}

// Returns the designed Sections or a populated error message; one or the
// other is set on return. Swallows wpi::math::BiquadFilter's
// std::invalid_argument so the node can render an error banner instead of
// crashing the app on out-of-range inputs.
std::pair<Sections, std::string> DesignStage(const Stage& s, double fs) {
  using K = wpi::math::BiquadFilter::Kind;
  if (!UsedParametersAreFinite(s, fs)) {
    return {{}, "Parameters must be finite"};
  }
  if (fs <= 0.0) {
    return {{}, "Sample rate must be positive"};
  }
  // The UI clamps these as they're typed; this catches a loaded file.
  if (s.kind == StageKind::MovingAverage) {
    if (s.taps < 1 || s.taps > kMaxTaps) {
      return {{}, std::format("Taps must be between 1 and {}", kMaxTaps)};
    }
  } else if (KindUsesFamily(s.kind)) {
    if (s.order < 1 || s.order > kMaxOrder) {
      return {{}, std::format("Order must be between 1 and {}", kMaxOrder)};
    }
  }
  try {
    switch (s.kind) {
      case StageKind::LowPass:
        return CheckedSections(DesignClassicalKind(K::LowPass, s, fs));
      case StageKind::HighPass:
        return CheckedSections(DesignClassicalKind(K::HighPass, s, fs));
      case StageKind::BandPass:
        return CheckedSections(DesignClassicalKind(K::BandPass, s, fs));
      case StageKind::BandStop:
        return CheckedSections(DesignClassicalKind(K::BandStop, s, fs));
      case StageKind::Notch:
        return CheckedSections(ToSections(wpi::math::BiquadFilter::Notch(
            units::hertz_t{fs}, units::hertz_t{s.f1}, s.q)));
      case StageKind::MovingAverage:
        return CheckedSections(
            ToSections(wpi::math::BiquadFilter::MovingAverage(s.taps)));
    }
  } catch (const std::invalid_argument& e) {
    return {{}, std::string{"Invalid parameters: "} + e.what()};
  }
  return {{}, "Unknown stage kind"};
}

bool StagesEqual(const Stage& a, const Stage& b) {
  return a.kind == b.kind && a.family == b.family && a.order == b.order &&
         a.taps == b.taps && a.f1 == b.f1 && a.f2 == b.f2 && a.q == b.q &&
         a.passbandRippleDb == b.passbandRippleDb &&
         a.stopbandAttenDb == b.stopbandAttenDb;
}

const char* StageKindSuffix(StageKind k) {
  switch (k) {
    case StageKind::LowPass:
      return " (LP)";
    case StageKind::HighPass:
      return " (HP)";
    case StageKind::BandPass:
      return " (BP)";
    case StageKind::BandStop:
      return " (BS)";
    case StageKind::Notch:
      return " (Notch)";
    case StageKind::MovingAverage:
      return " (MA)";
  }
  return "";
}

}  // namespace

bool BiquadStageNodeLogic::DesignCacheIsFresh() const {
  return m_haveDesigned && StagesEqual(m_designedStage, stage) &&
         m_designedFs == sampleRate;
}

const DesignedFilter* BiquadStageNodeLogic::Filter() const {
  if (DesignCacheIsFresh()) {
    return m_designCache.has_value() ? &*m_designCache : nullptr;
  }

  auto [sections, error] = DesignStage(stage, sampleRate);
  m_designedStage = stage;
  m_designedFs = sampleRate;
  m_haveDesigned = true;
  ++m_filterVersion;
  // A redesign invalidates the filtered-signal cache unconditionally — even
  // if the cascade ended up identical, the cheap thing to do is recompute
  // on the next pull rather than memoize at this layer too.
  m_haveFiltered = false;
  m_filteredCache.reset();

  if (sections.empty()) {
    m_designCache.reset();
    m_designError =
        error.empty() ? std::string{"No filter designed"} : std::move(error);
    return nullptr;
  }

  m_designCache = DesignedFilter{std::move(sections), sampleRate};
  m_designError.clear();
  return &*m_designCache;
}

const Signal* BiquadStageNodeLogic::Filtered(const Signal* input) const {
  if (!input) {
    m_haveFiltered = false;
    m_filteredCache.reset();
    m_filteredInput = nullptr;
    m_filteredInputRev = 0;
    m_filterError.clear();
    return nullptr;
  }
  const DesignedFilter* design = Filter();
  if (!design) {
    m_haveFiltered = false;
    m_filteredCache.reset();
    return nullptr;
  }
  // The cascade check between stages lives in BiquadStageNode; this is the
  // same check at the input boundary, where a hand-set rate can disagree with
  // the wire. Applying the coefficients anyway would scale every cutoff by
  // the ratio while the Bode plot and export kept promising the design.
  if (input->sampleRate > 0.0 &&
      std::abs(input->sampleRate - design->sampleRate) >
          kRateMismatchTolerance * design->sampleRate) {
    m_haveFiltered = false;
    m_filteredCache.reset();
    m_filterError = std::format(
        "Input sample rate {:.3f} Hz does not match design rate {:.3f} Hz",
        input->sampleRate, design->sampleRate);
    return nullptr;
  }
  m_filterError.clear();
  if (m_haveFiltered && m_filteredInput == input &&
      m_filteredInputRev == input->revision &&
      m_filteredAtFilterVersion == m_filterVersion) {
    return &*m_filteredCache;
  }

  Signal out;
  out.timestamps = input->timestamps;
  // A live source hands us a sliding window of a signal that was already
  // running before the window opened, and we re-filter the whole buffer
  // every revision; a zero-state start would park the filter's startup
  // transient at the leading edge and drag it along as the window slides.
  out.values =
      ApplyFilter(input->values, design->sections,
                  input->live ? FilterStart::SteadyState : FilterStart::Zero);
  out.name = input->name + StageKindSuffix(stage.kind);
  out.sampleRate = input->sampleRate;
  // Filtering keeps the input's timestamps, so it inherits their grid
  // quality verbatim — the biquad neither improves nor degrades it.
  out.quality = input->quality;
  out.revision = ++m_outRevision;
  out.live = input->live;
  out.transient = input->transient;
  m_filteredCache = std::move(out);
  m_filteredInput = input;
  m_filteredInputRev = input->revision;
  m_filteredAtFilterVersion = m_filterVersion;
  m_haveFiltered = true;
  return &*m_filteredCache;
}

}  // namespace wpi::filterdesigner
