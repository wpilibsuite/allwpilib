// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/FilterDesignView.hpp"

#include <cstdio>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "wpi/filterdesigner/model/Spec.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"
// pfd is included unconditionally so the unique_ptr<pfd::*> destructors in
// this translation unit see the complete type — the file-dialog calls
// themselves are still gated behind the ifndef below.
#include "wpi/gui/portable-file-dialogs.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <imgui.h>
#endif

namespace wpi::filterdesigner {

namespace {
#ifndef RUNNING_FILTERDESIGNER_TESTS
constexpr const char* kKindLabels[] = {"Low pass",  "High pass",
                                       "Band pass", "Band stop",
                                       "Notch",     "Moving average"};
constexpr const char* kFamilyLabels[] = {"Butterworth", "Chebyshev I",
                                         "Chebyshev II", "Elliptic"};
#endif

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

// Wraps the throwing wpi::math::BiquadFilter factories so the UI can keep
// rendering a "Invalid parameters" badge for the offending stage instead of
// terminating. The factories are documented to only throw std::invalid_argument
// for out-of-range inputs, which is exactly the case we want to recover from.
std::optional<Sections> DesignStage(const Stage& s, double fs) {
  using K = wpi::math::BiquadFilter::Kind;
  try {
    switch (s.kind) {
      case StageKind::LowPass:
        return DesignClassicalKind(K::LowPass, s, fs);
      case StageKind::HighPass:
        return DesignClassicalKind(K::HighPass, s, fs);
      case StageKind::BandPass:
        return DesignClassicalKind(K::BandPass, s, fs);
      case StageKind::BandStop:
        return DesignClassicalKind(K::BandStop, s, fs);
      case StageKind::Notch:
        return ToSections(wpi::math::BiquadFilter::Notch(
            units::hertz_t{fs}, units::hertz_t{s.f1}, s.q));
      case StageKind::MovingAverage:
        return ToSections(wpi::math::BiquadFilter::MovingAverage(s.taps));
    }
  } catch (const std::invalid_argument&) {
    return std::nullopt;
  }
  return std::nullopt;
}

std::string FamilySuffix(const Stage& s) {
  char buf[96];
  switch (s.family) {
    case Family::Butterworth:
      return "  family=butter";
    case Family::Chebyshev1:
      std::snprintf(buf, sizeof(buf), "  family=cheby1  ripple=%.6g dB",
                    s.passbandRippleDb);
      return buf;
    case Family::Chebyshev2:
      std::snprintf(buf, sizeof(buf), "  family=cheby2  atten=%.6g dB",
                    s.stopbandAttenDb);
      return buf;
    case Family::Elliptic:
      std::snprintf(buf, sizeof(buf),
                    "  family=ellip  ripple=%.6g dB  atten=%.6g dB",
                    s.passbandRippleDb, s.stopbandAttenDb);
      return buf;
  }
  return {};
}

std::string FormatStage(size_t idx, const Stage& s) {
  char buf[160];
  std::string out;
  switch (s.kind) {
    case StageKind::LowPass:
      std::snprintf(buf, sizeof(buf),
                    "  %zu. Low pass        order=%d  cutoff=%.6g Hz", idx + 1,
                    s.order, s.f1);
      out = buf;
      out.append(FamilySuffix(s));
      break;
    case StageKind::HighPass:
      std::snprintf(buf, sizeof(buf),
                    "  %zu. High pass       order=%d  cutoff=%.6g Hz", idx + 1,
                    s.order, s.f1);
      out = buf;
      out.append(FamilySuffix(s));
      break;
    case StageKind::BandPass:
      std::snprintf(
          buf, sizeof(buf),
          "  %zu. Band pass       order=%d  low=%.6g Hz  high=%.6g Hz", idx + 1,
          s.order, s.f1, s.f2);
      out = buf;
      out.append(FamilySuffix(s));
      break;
    case StageKind::BandStop:
      std::snprintf(
          buf, sizeof(buf),
          "  %zu. Band stop       order=%d  low=%.6g Hz  high=%.6g Hz", idx + 1,
          s.order, s.f1, s.f2);
      out = buf;
      out.append(FamilySuffix(s));
      break;
    case StageKind::Notch:
      std::snprintf(buf, sizeof(buf),
                    "  %zu. Notch           center=%.6g Hz  Q=%.6g", idx + 1,
                    s.f1, s.q);
      out = buf;
      break;
    case StageKind::MovingAverage:
      std::snprintf(buf, sizeof(buf), "  %zu. Moving average  taps=%d", idx + 1,
                    s.taps);
      out = buf;
      break;
  }
  return out;
}
}  // namespace

FilterDesignView::FilterDesignView() {
  m_stages.emplace_back();
}

FilterDesignView::~FilterDesignView() = default;

void FilterDesignView::DoSave(const std::string& path) {
  std::string err =
      SaveSpecToFile(path, m_sampleRate,
                     std::span<const Stage>{m_stages.data(), m_stages.size()});
  if (!err.empty()) {
    m_persistError = std::move(err);
    m_persistInfo.clear();
    return;
  }
  m_persistError.clear();
  m_persistInfo = "Saved " + path;
}

void FilterDesignView::DoOpen(const std::string& path) {
  ParseResult result = LoadSpecFromFile(path);
  if (!result.spec.has_value()) {
    m_persistError = std::move(result.error);
    m_persistInfo.clear();
    return;
  }
  m_sampleRate = result.spec->sampleRate;
  m_stages = std::move(result.spec->stages);
  if (m_stages.empty()) {
    m_stages.emplace_back();
  }
  m_dirty = true;
  m_persistError.clear();
  m_persistInfo = "Loaded " + path;
}

void FilterDesignView::Redesign() {
  m_dirty = false;
  ++m_version;

  m_stageValid.assign(m_stages.size(), false);

  if (m_stages.empty()) {
    m_result.reset();
    return;
  }

  Sections combined;
  bool allValid = true;
  for (size_t i = 0; i < m_stages.size(); ++i) {
    auto sections = DesignStage(m_stages[i], m_sampleRate);
    if (sections.has_value()) {
      m_stageValid[i] = true;
      combined.insert(combined.end(), sections->begin(), sections->end());
    } else {
      allValid = false;
    }
  }
  if (allValid) {
    m_result = std::move(combined);
  } else {
    m_result.reset();
  }
}

std::string FilterDesignView::Describe() const {
  std::string out;
  char buf[64];
  std::snprintf(buf, sizeof(buf), "Sample rate: %.6g Hz\n", m_sampleRate);
  out.append(buf);
  if (m_stages.empty()) {
    out.append("Stages: (none)\n");
    return out;
  }
  out.append("Stages:\n");
  for (size_t i = 0; i < m_stages.size(); ++i) {
    out.append(FormatStage(i, m_stages[i]));
    out.push_back('\n');
  }
  return out;
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void FilterDesignView::PollSaveDialog() {
  if (!m_savePicker || !m_savePicker->ready(0)) {
    return;
  }
  std::string path = m_savePicker->result();
  m_savePicker.reset();
  if (path.empty()) {
    return;
  }
  DoSave(path);
}

void FilterDesignView::PollOpenDialog() {
  if (!m_openPicker || !m_openPicker->ready(0)) {
    return;
  }
  auto results = m_openPicker->result();
  m_openPicker.reset();
  if (results.empty()) {
    return;
  }
  DoOpen(results.front());
}

void FilterDesignView::Display(double inferredSampleRate) {
  PollSaveDialog();
  PollOpenDialog();

  // Save / Open lives at the top of the panel so it sits above the per-stage
  // controls and visually matches the .wpilog open button in the Data Source
  // panel. Filters are *.fdsgn (filter-design JSON). The buttons grey out
  // while a picker is in flight so the user gets visual feedback rather than
  // silent no-op clicks.
  const bool pickerInFlight = m_savePicker || m_openPicker;
  ImGui::BeginDisabled(pickerInFlight);
  if (ImGui::Button("Save...")) {
    m_persistError.clear();
    m_persistInfo.clear();
    m_savePicker = std::make_unique<pfd::save_file>(
        "Save filter spec", "untitled.fdsgn",
        std::vector<std::string>{"Filter Spec", "*.fdsgn"});
  }
  ImGui::SameLine();
  if (ImGui::Button("Open...")) {
    m_persistError.clear();
    m_persistInfo.clear();
    m_openPicker = std::make_unique<pfd::open_file>(
        "Open filter spec", "",
        std::vector<std::string>{"Filter Spec", "*.fdsgn"});
  }
  ImGui::EndDisabled();
  if (!m_persistError.empty()) {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       m_persistError.c_str());
  } else if (!m_persistInfo.empty()) {
    ImGui::TextDisabled("%s", m_persistInfo.c_str());
  }

  ImGui::Separator();

  if (ImGui::InputDouble("Sample rate (Hz)", &m_sampleRate, 0.0, 0.0, "%.3f")) {
    m_dirty = true;
  }
  if (inferredSampleRate > 0.0) {
    ImGui::SameLine();
    char buf[64];
    std::snprintf(buf, sizeof(buf), "Use %.2f", inferredSampleRate);
    if (ImGui::SmallButton(buf)) {
      m_sampleRate = inferredSampleRate;
      m_dirty = true;
    }
  }

  ImGui::Separator();

  int moveSrc = -1;
  int moveDst = -1;
  int deleteIdx = -1;

  // Reserve room on the header row for the trailing "Remove" button so the
  // header button shrinks instead of overflowing.
  const float removeWidth =
      ImGui::CalcTextSize("Remove").x + ImGui::GetStyle().FramePadding.x * 2.0f;
  const float headerTrailing = removeWidth + ImGui::GetStyle().ItemSpacing.x;

  for (size_t i = 0; i < m_stages.size(); ++i) {
    ImGui::PushID(static_cast<int>(i));
    Stage& s = m_stages[i];

    // Header row: drag handle/label + delete button. The header button is both
    // the drag source and the drop target; dropping moves the dragged stage to
    // the target's index (other stages shift to fill).
    char headerLabel[96];
    std::snprintf(headerLabel, sizeof(headerLabel), ":: %zu  %s", i + 1,
                  kKindLabels[static_cast<int>(s.kind)]);
    ImGui::Button(headerLabel, ImVec2{-headerTrailing, 0.0f});

    if (ImGui::BeginDragDropSource()) {
      int srcIdx = static_cast<int>(i);
      ImGui::SetDragDropPayload("FILTER_STAGE_IDX", &srcIdx, sizeof(int));
      ImGui::Text("Move stage %d", srcIdx + 1);
      ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget()) {
      if (const auto* payload =
              ImGui::AcceptDragDropPayload("FILTER_STAGE_IDX")) {
        moveSrc = *static_cast<const int*>(payload->Data);
        moveDst = static_cast<int>(i);
      }
      ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Remove")) {
      deleteIdx = static_cast<int>(i);
    }

    if (i < m_stageValid.size() && !m_stageValid[i]) {
      ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f},
                         "Invalid parameters for this stage.");
    }

    int kindIdx = static_cast<int>(s.kind);
    if (ImGui::Combo("Kind", &kindIdx, kKindLabels,
                     IM_ARRAYSIZE(kKindLabels))) {
      s.kind = static_cast<StageKind>(kindIdx);
      m_dirty = true;
    }

    if (KindUsesFamily(s.kind)) {
      int familyIdx = static_cast<int>(s.family);
      if (ImGui::Combo("Family", &familyIdx, kFamilyLabels,
                       IM_ARRAYSIZE(kFamilyLabels))) {
        s.family = static_cast<Family>(familyIdx);
        m_dirty = true;
      }
    }

    switch (s.kind) {
      case StageKind::LowPass:
      case StageKind::HighPass: {
        if (ImGui::InputInt("Order", &s.order)) {
          m_dirty = true;
        }
        if (ImGui::InputDouble("Cutoff (Hz)", &s.f1, 0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
        break;
      }
      case StageKind::BandPass:
      case StageKind::BandStop: {
        if (ImGui::InputInt("Order", &s.order)) {
          m_dirty = true;
        }
        if (ImGui::InputDouble("Low edge (Hz)", &s.f1, 0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
        if (ImGui::InputDouble("High edge (Hz)", &s.f2, 0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
        break;
      }
      case StageKind::Notch: {
        if (ImGui::InputDouble("Center (Hz)", &s.f1, 0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
        if (ImGui::InputDouble("Q", &s.q, 0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
        break;
      }
      case StageKind::MovingAverage:
        if (ImGui::InputInt("Taps", &s.taps)) {
          m_dirty = true;
        }
        break;
    }

    // Family-specific extra parameters (only meaningful for the LP/HP/BP/BS
    // kinds that select a family above). Cheby1 and Elliptic share the
    // ripple input; Cheby2 and Elliptic share the attenuation input.
    if (KindUsesFamily(s.kind)) {
      const bool needsRipple =
          s.family == Family::Chebyshev1 || s.family == Family::Elliptic;
      const bool needsAtten =
          s.family == Family::Chebyshev2 || s.family == Family::Elliptic;
      if (needsRipple) {
        if (ImGui::InputDouble("Passband ripple (dB)", &s.passbandRippleDb, 0.0,
                               0.0, "%.3f")) {
          m_dirty = true;
        }
      }
      if (needsAtten) {
        if (ImGui::InputDouble("Stopband attenuation (dB)", &s.stopbandAttenDb,
                               0.0, 0.0, "%.3f")) {
          m_dirty = true;
        }
      }
    }

    ImGui::PopID();
    ImGui::Separator();
  }

  if (moveSrc >= 0 && moveDst >= 0 && moveSrc != moveDst &&
      moveSrc < static_cast<int>(m_stages.size()) &&
      moveDst < static_cast<int>(m_stages.size())) {
    Stage moved = std::move(m_stages[moveSrc]);
    m_stages.erase(m_stages.begin() + moveSrc);
    m_stages.insert(m_stages.begin() + moveDst, std::move(moved));
    m_dirty = true;
  }
  if (deleteIdx >= 0 && deleteIdx < static_cast<int>(m_stages.size())) {
    m_stages.erase(m_stages.begin() + deleteIdx);
    m_dirty = true;
  }

  if (ImGui::Button("Add stage")) {
    m_stages.emplace_back();
    m_dirty = true;
  }

  if (m_dirty) {
    Redesign();
  }

  ImGui::Separator();
  if (m_stages.empty()) {
    ImGui::TextDisabled("No stages — add one to design a filter.");
  } else if (m_result.has_value()) {
    ImGui::Text("Stages: %zu  Sections: %zu", m_stages.size(),
                m_result->size());
  } else {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f},
                       "Invalid parameters — no filter produced.");
  }
}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
