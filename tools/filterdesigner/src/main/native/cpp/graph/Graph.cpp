// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Graph.hpp"

#include <memory>
#include <vector>

#include <ImNodeFlow.h>
#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <imgui.h>
#endif

#include "wpi/filterdesigner/graph/Topology.hpp"

namespace wpi::filterdesigner {

Graph::Graph()
    : m_editor(std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph")) {
  ConfigureEditor();
}

Graph::~Graph() = default;

void Graph::ConfigureEditor() {
  // Grid zoom is on, but the wheel is hover-routed in Update(): if the
  // cursor is over a node, the wheel is suppressed at the editor level so
  // it reaches the node's embedded ImPlot (or other wheel-aware widget)
  // exclusively. Empty canvas → grid zoom. Over a plot → plot zoom.
  // Zoom != 1.0 resamples the draw list, so text/plot strokes get slightly
  // blurry off scale 1. Bound the zoom range narrowly enough that the
  // effect stays acceptable, and rely on the R-key reset for snapping back.
  auto& cfg = m_editor->getGrid().config();
  cfg.zoom_enabled = true;
  cfg.zoom_min = 0.5f;
  cfg.zoom_max = 1.5f;
}

void Graph::Update() {
  // Recompute the cycle banner before ImNodeFlow drives sink draw()s. Sinks
  // read m_cycleError to decide whether to skip their getInVal pulls. Doing
  // this *before* update() means the banner reflects the topology as it is
  // entering the frame; any links the user adds during draw() (drop-link
  // popups, etc.) get caught on the next frame.
  RecomputeCycleError();
#ifndef RUNNING_FILTERDESIGNER_TESTS
  // Hover-aware wheel routing. ImNodeFlow's grid-zoom check reads
  // ImGui::GetIO().MouseWheel from the *outer* context after the sub-
  // context's frame has ended; ImPlot inside nodes reads its events from
  // the sub-context's queue, which is populated from the outer's
  // InputEventsTrail at sub-context begin(). Zeroing the outer's
  // MouseWheel here suppresses the grid zoom for this frame without
  // affecting the events already (about to be) copied into the sub-
  // context, so an ImPlot widget inside a hovered node still receives
  // the wheel and zooms its own axes.
  //
  // Node rects are stable across the begin→draw→end sequence, so we can
  // ask each node for its hover state from the outer context before
  // entering the sub-context.
  if (IsAnyNodeHovered()) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = 0.0f;
    io.MouseWheelH = 0.0f;
  }
#endif
  m_editor->update();
}

bool Graph::IsAnyNodeHovered() {
#ifdef RUNNING_FILTERDESIGNER_TESTS
  return false;
#else
  for (auto& [uid, node] : m_editor->getNodes()) {
    if (node && node->isHovered()) {
      return true;
    }
  }
  return false;
#endif
}

void Graph::RecomputeCycleError() {
  std::vector<int> cycle = FindCycle(*this);
  if (cycle.empty()) {
    m_cycleError.clear();
  } else {
    m_cycleError = FormatCycle(*this, cycle);
  }
}

std::vector<FilterDesignerNode*> Graph::Nodes() const {
  std::vector<FilterDesignerNode*> out;
  for (const auto& [uid, node] : m_editor->getNodes()) {
    if (auto* fd = dynamic_cast<FilterDesignerNode*>(node.get())) {
      out.push_back(fd);
    }
  }
  return out;
}

std::vector<Graph::LiveLink> Graph::Links() const {
  std::vector<LiveLink> out;
  for (const auto& weak : m_editor->getLinks()) {
    auto link = weak.lock();
    if (!link) {
      continue;
    }
    ImFlow::Pin* outPin = link->left();
    ImFlow::Pin* inPin = link->right();
    if (!outPin || !inPin) {
      continue;
    }
    auto* src = dynamic_cast<FilterDesignerNode*>(outPin->getParent());
    auto* dst = dynamic_cast<FilterDesignerNode*>(inPin->getParent());
    if (!src || !dst) {
      continue;
    }
    out.push_back(LiveLink{src->GraphId(), outPin->getName(), dst->GraphId(),
                           inPin->getName()});
  }
  return out;
}

FilterDesignerNode* Graph::FindNodeById(int id) const {
  for (const auto& [uid, node] : m_editor->getNodes()) {
    auto* fd = dynamic_cast<FilterDesignerNode*>(node.get());
    if (fd && fd->GraphId() == id) {
      return fd;
    }
  }
  return nullptr;
}

void Graph::Reset() {
  m_editor = std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph");
  m_nextId = 1;
  m_cycleError.clear();
  ConfigureEditor();
  if (m_onReset) {
    m_onReset();
  }
}

}  // namespace wpi::filterdesigner
