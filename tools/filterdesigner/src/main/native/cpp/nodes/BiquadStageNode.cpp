// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNode.hpp"

#include <memory>
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

  // Capture both the logic pointer and `this` so the signal behaviour can
  // pull the current input through ImNodeFlow before forwarding to the
  // logic. The unique_ptr keeps the logic alive for the node's lifetime;
  // ImNodeFlow tears down links before the pins are destroyed, so neither
  // capture can outlive its target.
  auto* logic = m_logic.get();
  auto* self = this;
  addOUT<const wpi::filterdesigner::DesignedFilter*>("filter")->behaviour(
      [logic] { return logic->Filter(); });
  addOUT<const wpi::filterdesigner::Signal*>("signal")->behaviour([self,
                                                                   logic] {
    const Signal* input = self->getInVal<const Signal*>("in");
    return logic->Filtered(input);
  });
}

BiquadStageNode::~BiquadStageNode() = default;

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

  // Force a design pass this frame so the error banner reflects the just-
  // entered values, not last frame's. Cheap (single design call + lazy
  // compare against the cache).
  const DesignedFilter* design = m_logic->Filter();
  if (!design) {
    ImGui::TextColored(ImVec4{1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       m_logic->DesignError().c_str());
  } else {
    ImGui::TextDisabled("Sections: %zu", design->sections.size());
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void BiquadStageNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
