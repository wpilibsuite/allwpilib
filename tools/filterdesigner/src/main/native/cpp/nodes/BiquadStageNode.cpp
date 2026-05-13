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
#include <cstdio>

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
  // M7 will detect cycles at the Graph level and gate sink pulls; until then
  // a user-introduced cycle (`A.signal → B.in`, `B.signal → A.in`) would
  // stack-overflow this walk every frame. A depth cap several orders above
  // any realistic cascade keeps the per-frame cost ~free while turning the
  // crash into a visible per-stage error banner.
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
  // Slightly narrower than FilterDesignView since each biquad stage is its
  // own node — over-wide nodes make the canvas hard to read with several
  // stages chained together.
  const float kItemWidth = 160.0f;

  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::InputDouble("Sample rate (Hz)", &m_logic->sampleRate, 0.0, 0.0,
                         "%.3f")) {
    // Logic redesigns lazily on the next Filter() call — no explicit dirty
    // bit needed.
  }
  // Mirror FilterDesignView's "Use X" affordance: if the connected input
  // carries a sample rate, offer a one-click sync.
  const Signal* input = getInVal<const Signal*>("in");
  if (input && input->sampleRate > 0.0) {
    ImGui::SameLine();
    char buf[64];
    std::snprintf(buf, sizeof(buf), "Use %.2f", input->sampleRate);
    if (ImGui::SmallButton(buf)) {
      m_logic->sampleRate = input->sampleRate;
    }
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
