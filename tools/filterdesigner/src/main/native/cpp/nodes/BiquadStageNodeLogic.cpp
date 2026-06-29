// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"

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

// Returns the designed Sections or a populated error message; one or the
// other is set on return. Swallows wpi::math::BiquadFilter's
// std::invalid_argument so the node can render an error banner instead of
// crashing the app on out-of-range inputs.
std::pair<Sections, std::string> DesignStage(const Stage& s, double fs) {
  using K = wpi::math::BiquadFilter::Kind;
  if (fs <= 0.0) {
    return {{}, "Sample rate must be positive"};
  }
  try {
    switch (s.kind) {
      case StageKind::LowPass:
        return {DesignClassicalKind(K::LowPass, s, fs), {}};
      case StageKind::HighPass:
        return {DesignClassicalKind(K::HighPass, s, fs), {}};
      case StageKind::BandPass:
        return {DesignClassicalKind(K::BandPass, s, fs), {}};
      case StageKind::BandStop:
        return {DesignClassicalKind(K::BandStop, s, fs), {}};
      case StageKind::Notch:
        return {ToSections(wpi::math::BiquadFilter::Notch(
                    units::hertz_t{fs}, units::hertz_t{s.f1}, s.q)),
                {}};
      case StageKind::MovingAverage:
        return {ToSections(wpi::math::BiquadFilter::MovingAverage(s.taps)), {}};
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
    return nullptr;
  }
  const DesignedFilter* design = Filter();
  if (!design) {
    m_haveFiltered = false;
    m_filteredCache.reset();
    return nullptr;
  }
  if (m_haveFiltered && m_filteredInput == input &&
      m_filteredInputRev == input->revision &&
      m_filteredAtFilterVersion == m_filterVersion) {
    return &*m_filteredCache;
  }

  Signal out;
  out.timestamps = input->timestamps;
  out.values = ApplyFilter(input->values, design->sections);
  out.name = input->name + StageKindSuffix(stage.kind);
  out.sampleRate = input->sampleRate;
  out.uniform = input->uniform;
  out.revision = input->revision;  // tied to upstream churn; bumps with input
  out.live = input->live;
  m_filteredCache = std::move(out);
  m_filteredInput = input;
  m_filteredInputRev = input->revision;
  m_filteredAtFilterVersion = m_filterVersion;
  m_haveFiltered = true;
  return &*m_filteredCache;
}

}  // namespace wpi::filterdesigner
