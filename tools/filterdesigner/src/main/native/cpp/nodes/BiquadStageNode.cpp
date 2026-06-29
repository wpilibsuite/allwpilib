// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"

#include <memory>
#include <string>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <cmath>

#include <imgui.h>

#include "wpi/filterdesigner/graph/Topology.hpp"
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

  // The filter pin walks the Signal chain upstream and emits the cumulative
  // cascade through this stage — see CombinedFilter. The signal pin only
  // applies *this* stage's filter; the chain composes naturally because the
  // input has already been filtered by the upstream stage's signal pin.
  // Both lambdas capture the node so they can reach back through ImNodeFlow.
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
  // BaseNode::inPin returns a raw Pin*; the dynamic_cast filters out the
  // case where the wire is connected to something other than a
  // BiquadStageNode (e.g. WpiLogSource), in which case we treat this stage
  // as the head of the chain.
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
  // Defense-in-depth alongside the Graph-level cycle detector: a user-
  // introduced cycle (`A.signal → B.in`, `B.signal → A.in`) would otherwise
  // stack-overflow this walk every frame. The Graph gate normally stops
  // sinks from pulling first, but callers that walk upstream directly need
  // this depth cap too. A cap several orders above any realistic cascade
  // keeps the per-frame cost ~free while turning the crash into a visible
  // per-stage error banner.
  constexpr int kMaxCascadeDepth = 256;
  if (depth >= kMaxCascadeDepth) {
    m_combinedError = "Filter cascade too deep — graph likely has a cycle.";
    m_haveCombined = false;
    return nullptr;
  }

  const DesignedFilter* self = m_logic->Filter();
  if (!self) {
    // Surface the per-stage design error verbatim so the user sees one
    // message, not "combined: stage error".
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
    // Upstream stage is in an error state — propagate that, don't silently
    // drop its sections.
    m_combinedError = upstreamNode->CombinedError().empty()
                          ? std::string{"Upstream stage has invalid design."}
                          : upstreamNode->CombinedError();
    m_haveCombined = false;
    return nullptr;
  }

  if (upstreamFilter && upstreamFilter->sampleRate != self->sampleRate) {
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
  if (const auto* p = obj.lookup("kind"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    if (v >= 0 && v <= static_cast<int>(StageKind::MovingAverage)) {
      s.kind = static_cast<StageKind>(v);
    }
  }
  if (const auto* p = obj.lookup("family"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    if (v >= 0 && v <= static_cast<int>(Family::Elliptic)) {
      s.family = static_cast<Family>(v);
    }
  }
  if (const auto* p = obj.lookup("order"); p && p->is_number()) {
    s.order = static_cast<int>(p->get_number());
  }
  if (const auto* p = obj.lookup("taps"); p && p->is_number()) {
    s.taps = static_cast<int>(p->get_number());
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
  // BiquadStage pulls input via getInVal + CombinedFilter() below; under a
  // cycle those would recurse through ImNodeFlow's pin behaviours, so gate
  // the whole body on the per-frame banner just like the sink nodes do.
  if (DrawCycleBannerIfAny(this)) {
    return;
  }

  // Keep stage nodes compact so multi-stage chains stay readable.
  const float kItemWidth = 160.0f;

  // Pull the input first so the sample-rate field can mirror it before we
  // draw the InputDouble — otherwise the displayed value lags by one frame.
  const Signal* input = getInVal<const Signal*>("in");
  const bool inputHasRate = input && input->sampleRate > 0.0;
  if (m_logic->sampleRateAutoSync && inputHasRate) {
    // NT4Source infers its rate from the median of timestamp diffs across a
    // sliding ring buffer. Each sample that rolls in/out shifts the median
    // by a few microseconds of jitter — sub-1% wobble around a steady
    // underlying rate. The design cache keys on exact `m_designedFs ==
    // sampleRate` equality, so syncing every frame triggers a full filter
    // redesign + re-apply + downstream cache invalidation cascade on data
    // we know hasn't actually changed rate. Take a 1% deadband: only adopt
    // the input rate when it truly diverges, leaving small jitter to wash
    // out at the inferred-rate layer instead of churning the filter.
    constexpr double kRateSyncTolerance = 0.01;
    const double cur = m_logic->sampleRate;
    if (cur <= 0.0 ||
        std::abs(input->sampleRate - cur) > kRateSyncTolerance * cur) {
      m_logic->sampleRate = input->sampleRate;
    }
  }
  const bool autoActive = m_logic->sampleRateAutoSync && inputHasRate;

  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::BeginDisabled(autoActive);
  if (ImGui::InputDouble("Sample rate (Hz)", &m_logic->sampleRate, 0.0, 0.0,
                         "%.3f")) {
    // Any hand-edit drops the stage out of auto mode; the Auto checkbox
    // below re-enables tracking.
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
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputDouble("Cutoff (Hz)", &s.f1, 0.0, 0.0, "%.3f");
      break;
    }
    case StageKind::BandPass:
    case StageKind::BandStop: {
      ImGui::SetNextItemWidth(kItemWidth);
      ImGui::InputInt("Order", &s.order);
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

  // Force a combined-design pass this frame so the banner reflects both
  // this stage's params and any upstream chain state (sample-rate mismatch,
  // upstream design error). Cheap — cached on (upstream pointer + version,
  // self version).
  const DesignedFilter* combined = CombinedFilter();
  if (!combined) {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       CombinedError().c_str());
  } else if (const DesignedFilter* self = m_logic->Filter();
             self && combined->sections.size() > self->sections.size()) {
    // Chained — tell the user the filter pin emits the full cascade.
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
