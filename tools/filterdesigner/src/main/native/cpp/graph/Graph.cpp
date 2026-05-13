// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Graph.hpp"

#include <memory>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Topology.hpp"

namespace wpi::filterdesigner {

Graph::Graph()
    : m_editor(std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph")) {
  ConfigureEditor();
}

Graph::~Graph() = default;

void Graph::ConfigureEditor() {
  // ImNodeFlow zooms the editor on mouse-wheel, which (a) eats wheel events
  // before nodes that host their own widgets (ImPlot) can scroll/zoom, and
  // (b) resamples the draw list at zoom != 1.0 which makes text + plot
  // strokes blurry. Leave pan (middle-mouse drag) on; turn zoom off.
  m_editor->getGrid().config().zoom_enabled = false;
}

void Graph::Update() {
  // Recompute the cycle banner before ImNodeFlow drives sink draw()s. Sinks
  // read m_cycleError to decide whether to skip their getInVal pulls. Doing
  // this *before* update() means the banner reflects the topology as it is
  // entering the frame; any links the user adds during draw() (drop-link
  // popups, etc.) get caught on the next frame.
  RecomputeCycleError();
  m_editor->update();
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
