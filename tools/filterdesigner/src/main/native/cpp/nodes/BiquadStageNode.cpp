// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"

#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <algorithm>

#include <imgui.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
#include "wpi/filterdesigner/nodes/StatusText.hpp"
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
}  // namespace

BiquadStageNode::BiquadStageNode()
    : m_logic(std::make_unique<BiquadStageNodeLogic>()) {
  setTitle("Biquad Stage");
  setStyle(ImFlow::NodeStyle::brown());

  addIN<const wpi::filterdesigner::Signal*>(
      "in", nullptr, ImFlow::ConnectionFilter::SameType());

  // The filter pin emits the cumulative cascade (see CombinedFilter); the
  // signal pin applies only this stage, since its input has already been
  // filtered by the upstream stage.
  auto* logic = m_logic.get();
  auto* self = this;
  addOUT<const wpi::filterdesigner::DesignedFilter*>("filter")->behaviour(
      [self] { return self->CombinedFilter(); });
  addOUT<const wpi::filterdesigner::Signal*>("signal")->behaviour(
      [self, logic] {
        const Signal* input = self->getInVal<const Signal*>("in");
        return logic->Filtered(input);
      });
}

BiquadStageNode::~BiquadStageNode() = default;

const BiquadStageNode* BiquadStageNode::UpstreamStage() const {
  // A wire from anything but another stage makes this the head of the chain.
  auto* inPinPtr = const_cast<BiquadStageNode*>(this)->inPin("in");
  if (!inPinPtr) {
    return nullptr;
  }
  auto link = inPinPtr->getLink().lock();
  if (!link) {
    return nullptr;
  }
  ImFlow::Pin* leftPin = link->left();
  if (!leftPin) {
    return nullptr;
  }
  return dynamic_cast<const BiquadStageNode*>(leftPin->getParent());
}

std::string BiquadStageNode::UpstreamErrorFor(ImFlow::Pin* inPin) {
  if (!inPin) {
    return {};
  }
  auto link = inPin->getLink().lock();
  if (!link) {
    return {};
  }
  ImFlow::Pin* leftPin = link->left();
  if (!leftPin) {
    return {};
  }
  auto* upstream = dynamic_cast<const BiquadStageNode*>(leftPin->getParent());
  if (!upstream) {
    return {};
  }
  return upstream->CombinedError();
}

const DesignedFilter* BiquadStageNode::CombinedFilter() const {
  return CombinedFilterImpl(0);
}

const DesignedFilter* BiquadStageNode::CombinedFilterImpl(int depth) const {
  // The Graph-level cycle detector normally stops sinks from pulling into a
  // cycle, but a caller walking upstream directly needs its own guard: without
  // one, `A.signal → B.in` plus `B.signal → A.in` overflows the stack every
  // frame. The cap sits far above any realistic cascade.
  constexpr int kMaxCascadeDepth = 256;
  if (depth >= kMaxCascadeDepth) {
    m_combinedError = "Filter cascade too deep — graph likely has a cycle.";
    m_haveCombined = false;
    return nullptr;
  }

  const DesignedFilter* self = m_logic->Filter();
  if (!self) {
    m_combinedError = m_logic->DesignError();
    m_haveCombined = false;
    return nullptr;
  }

  const BiquadStageNode* upstreamNode = UpstreamStage();
  const DesignedFilter* upstreamFilter =
      upstreamNode ? upstreamNode->CombinedFilterImpl(depth + 1) : nullptr;
  std::uint64_t upstreamVersion =
      upstreamNode ? upstreamNode->m_combinedVersion : 0;
  std::uint64_t selfVersion = m_logic->FilterVersion();

  if (upstreamNode && !upstreamFilter) {
    m_combinedError = upstreamNode->CombinedError().empty()
                          ? std::string{"Upstream stage has invalid design."}
                          : upstreamNode->CombinedError();
    m_haveCombined = false;
    return nullptr;
  }

  // The same proportional tolerance the input boundary uses. Auto-sync holds
  // a stage's rate inside a 1% deadband, so chained stages tracking one
  // wobbling live source settle on rates that differ slightly; an exact
  // comparison then withheld the Bode plot and codegen from a chain that was
  // filtering perfectly happily.
  if (upstreamFilter &&
      std::abs(upstreamFilter->sampleRate - self->sampleRate) >
          BiquadStageNodeLogic::kRateMismatchTolerance * self->sampleRate) {
    m_combinedError = "Sample rate mismatch with upstream stage.";
    m_haveCombined = false;
    return nullptr;
  }

  if (m_haveCombined && m_lastUpstreamFilter == upstreamFilter &&
      m_lastUpstreamVersion == upstreamVersion &&
      m_lastSelfVersion == selfVersion) {
    return &*m_combinedCache;
  }

  DesignedFilter combined;
  combined.sampleRate = self->sampleRate;
  if (upstreamFilter) {
    combined.sections = upstreamFilter->sections;
  }
  combined.sections.insert(combined.sections.end(), self->sections.begin(),
                           self->sections.end());

  m_combinedCache = std::move(combined);
  m_lastUpstreamFilter = upstreamFilter;
  m_lastUpstreamVersion = upstreamVersion;
  m_lastSelfVersion = selfVersion;
  m_haveCombined = true;
  m_combinedError.clear();
  ++m_combinedVersion;
  return &*m_combinedCache;
}

void BiquadStageNode::SerializeParams(wpi::util::json& obj) const {
  obj["sampleRate"] = m_logic->sampleRate;
  obj["sampleRateAuto"] = m_logic->sampleRateAutoSync;
  obj["kind"] = static_cast<int>(m_logic->stage.kind);
  obj["family"] = static_cast<int>(m_logic->stage.family);
  obj["order"] = m_logic->stage.order;
  obj["taps"] = m_logic->stage.taps;
  obj["f1"] = m_logic->stage.f1;
  obj["f2"] = m_logic->stage.f2;
  obj["q"] = m_logic->stage.q;
  obj["passbandRippleDb"] = m_logic->stage.passbandRippleDb;
  obj["stopbandAttenDb"] = m_logic->stage.stopbandAttenDb;
}

void BiquadStageNode::DeserializeParams(const wpi::util::json& obj) {
  Stage& s = m_logic->stage;
  if (const auto* p = obj.lookup("sampleRate"); p && p->is_number()) {
    m_logic->sampleRate = p->get_number();
  }
  if (const auto* p = obj.lookup("sampleRateAuto"); p && p->is_bool()) {
    m_logic->sampleRateAutoSync = p->get_bool();
  }
  if (auto v = ReadIntField(obj, "kind");
      v && *v >= 0 && *v <= static_cast<int>(StageKind::MovingAverage)) {
    s.kind = static_cast<StageKind>(*v);
  }
  if (auto v = ReadIntField(obj, "family");
      v && *v >= 0 && *v <= static_cast<int>(Family::Elliptic)) {
    s.family = static_cast<Family>(*v);
  }
  if (auto v = ReadIntField(obj, "order")) {
    s.order = *v;
  }
  if (auto v = ReadIntField(obj, "taps")) {
    s.taps = *v;
  }
  if (const auto* p = obj.lookup("f1"); p && p->is_number()) {
    s.f1 = p->get_number();
  }
  if (const auto* p = obj.lookup("f2"); p && p->is_number()) {
    s.f2 = p->get_number();
  }
  if (const auto* p = obj.lookup("q"); p && p->is_number()) {
    s.q = p->get_number();
  }
  if (const auto* p = obj.lookup("passbandRippleDb"); p && p->is_number()) {
    s.passbandRippleDb = p->get_number();
  }
  if (const auto* p = obj.lookup("stopbandAttenDb"); p && p->is_number()) {
    s.stopbandAttenDb = p->get_number();
  }
}

void BiquadStageNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "BiquadStage";
  entry.menuLabel = "Biquad Stage";
  entry.menuCategory = "Filters";
  entry.inputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.outputTypes.emplace_back(
      typeid(const wpi::filterdesigner::DesignedFilter*));
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<BiquadStageNode>(pos);
  };
  registry.Register(std::move(entry));
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void BiquadStageNode::draw() {
  // getInVal and CombinedFilter both recurse through pin behaviours, so gate
  // the body on the banner as the sinks do.
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  // Keep stage nodes compact so multi-stage chains stay readable.
  const float kItemWidth = 160.0f;

  // Before the InputDouble, or the mirrored rate lags a frame behind.
  const Signal* input = getInVal<const Signal*>("in");
  const bool inputHasRate = input && input->sampleRate > 0.0;
  if (m_logic->sampleRateAutoSync && inputHasRate) {
    const double cur = m_logic->sampleRate;
    if (cur <= 0.0 || std::abs(input->sampleRate - cur) >
                          BiquadStageNodeLogic::kRateSyncTolerance * cur) {
      m_logic->sampleRate = input->sampleRate;
    }
  }
  const bool autoActive = m_logic->sampleRateAutoSync && inputHasRate;

  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::BeginDisabled(autoActive);
  if (ImGui::InputDouble("Sample rate (Hz)", &m_logic->sampleRate, 0.0, 0.0,
                         "%.3f")) {
    // A hand-edit drops out of auto mode; the Auto checkbox re-enables it.
    m_logic->sampleRateAutoSync = false;
  }
  ImGui::EndDisabled();
  if (inputHasRate) {
    ImGui::SameLine();
    ImGui::Checkbox("Auto", &m_logic->sampleRateAutoSync);
  }

  Stage& s = m_logic->stage;
  int kindIdx = static_cast<int>(s.kind);
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::Combo("Kind", &kindIdx, kKindLabels, IM_ARRAYSIZE(kKindLabels))) {
    s.kind = static_cast<StageKind>(kindIdx);
  }

  if (KindUsesFamily(s.kind)) {
    int familyIdx = static_cast<int>(s.family);
    ImGui::SetNextItemWidth(kItemWidth);
    if (ImGui::Combo("Family", &familyIdx, kFamilyLabels,
                     IM_ARRAYSIZE(kFamilyLabels))) {
      s.family = static_cast<Family>(familyIdx);
    }
  }

  switch (s.kind) {
    case StageKind::LowPass:
    case StageKind::HighPass: {
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputInt("Order", &s.order);
      s.order = std::clamp(s.order, 1, kMaxOrder);
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Cutoff (Hz)", &s.f1, 0.0, 0.0, "%.3f");
      break;
    }
    case StageKind::BandPass:
    case StageKind::BandStop: {
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputInt("Order", &s.order);
      s.order = std::clamp(s.order, 1, kMaxOrder);
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Low edge (Hz)", &s.f1, 0.0, 0.0, "%.3f");
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("High edge (Hz)", &s.f2, 0.0, 0.0, "%.3f");
      break;
    }
    case StageKind::Notch:
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Center (Hz)", &s.f1, 0.0, 0.0, "%.3f");
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Q", &s.q, 0.0, 0.0, "%.3f");
      break;
    case StageKind::MovingAverage:
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputInt("Taps", &s.taps);
      s.taps = std::clamp(s.taps, 1, kMaxTaps);
      break;
  }

  if (KindUsesFamily(s.kind)) {
    const bool needsRipple =
        s.family == Family::Chebyshev1 || s.family == Family::Elliptic;
    const bool needsAtten =
        s.family == Family::Chebyshev2 || s.family == Family::Elliptic;
    if (needsRipple) {
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Ripple (dB)", &s.passbandRippleDb, 0.0, 0.0, "%.3f");
    }
    if (needsAtten) {
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Atten (dB)", &s.stopbandAttenDb, 0.0, 0.0, "%.3f");
    }
  }

  // Force a combined-design pass so the banner reflects upstream state too,
  // and a filtering pass so a rate mismatch on the input shows even when no
  // sink is pulling the signal pin. Both are cached: the first on (upstream
  // pointer and version, self version), the second on (input, revision,
  // filter version).
  const DesignedFilter* combined = CombinedFilter();
  m_logic->Filtered(input);
  if (!combined) {
    DrawStatusText(kStatusErrorColor, CombinedError());
  } else if (!m_logic->FilterError().empty()) {
    DrawStatusText(kStatusErrorColor, m_logic->FilterError());
  } else if (const DesignedFilter* self = m_logic->Filter();
             self && combined->sections.size() > self->sections.size()) {
    ImGui::TextDisabled("Cascade: %zu sections (this stage: %zu)",
                        combined->sections.size(), self->sections.size());
  } else {
    ImGui::TextDisabled("Sections: %zu", combined->sections.size());
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void BiquadStageNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
