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
  //
  // Pan is remapped to left-mouse so trackpad users (no middle button) can
  // navigate the canvas. The collision with node-drag / link-drag-out is
  // resolved per-frame in Update() by toggling cfg.block_scroll based on
  // selection and drag state.
  auto& cfg = m_editor->getGrid().config();
  cfg.zoom_enabled = true;
  cfg.zoom_min = 0.5f;
  cfg.zoom_max = 1.5f;
  cfg.scroll_button = ImGuiMouseButton_Left;
}

void Graph::Update() {
  // Recompute the cycle banner before ImNodeFlow drives sink draw()s. Sinks
  // read m_cycleError to decide whether to skip their getInVal pulls. Doing
  // this *before* update() means the banner reflects the topology as it is
  // entering the frame; any links the user adds during draw() (drop-link
  // popups, etc.) get caught on the next frame.
  RecomputeCycleError();
#ifndef RUNNING_FILTERDESIGNER_TESTS
  ApplyTheme();
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

  // Left-click pan only when the user isn't otherwise engaged with the
  // graph. Without this gate, the same MouseDelta that drives a node drag
  // or a link drag-out also pans the canvas — nodes move at 2x cursor
  // speed and link drag-outs leave a visible trail. We read selection /
  // drag state from the previous frame (set during the prior update());
  // node drag and link drag-out both raise their respective flags on the
  // click frame before the scroll check runs in m_context.end(), but since
  // ImGui::IsMouseDragging needs at least one frame of movement past the
  // threshold, a single-frame lag in the gate is harmless: by the time
  // the drag actually deltas, the flag is set.
  bool block = m_editor->isNodeDragged() || m_editor->isLinkDragging();
  if (!block) {
    for (auto& [uid, node] : m_editor->getNodes()) {
      if (node && node->isSelected()) {
        block = true;
        break;
      }
    }
  }
  m_editor->getGrid().config().block_scroll = block;
#endif
  m_editor->update();
}

bool Graph::IsAnyNodeHovered() {
#ifdef RUNNING_FILTERDESIGNER_TESTS
  return false;
#else
  // BaseNode::isHovered() uses grid2screen(m_pos) for its hover rect, which
  // in the outer context resolves to m_pos + origin + scroll * scale — the
  // node position itself is not scaled. The drawn rect, however, comes from
  // sub-context vertices that get multiplied by scale on copy-out, so the
  // visual rect is at (m_pos + scroll) * scale + origin. The two only agree
  // at scale 1; off scale the detection rect drifts proportional to m_pos,
  // which let the wheel reach both the grid and a hovered plot at once and
  // suppressed the wheel over empty canvas. Recompute the rect ourselves
  // using the actual scaled transform.
  auto& grid = m_editor->getGrid();
  const float scale = grid.scale();
  const ImVec2 origin = grid.origin();
  const ImVec2 scroll = grid.scroll();
  const ImVec2 mouse = ImGui::GetMousePos();
  for (auto& [uid, node] : m_editor->getNodes()) {
    if (!node) {
      continue;
    }
    const auto& style = node->getStyle();
    const ImVec2 pos = node->getPos();
    const ImVec2 size = node->getSize();
    const float tlx = origin.x + (pos.x - style->padding.x + scroll.x) * scale;
    const float tly = origin.y + (pos.y - style->padding.y + scroll.y) * scale;
    const float brx =
        origin.x + (pos.x + size.x + style->padding.z + scroll.x) * scale;
    const float bry =
        origin.y + (pos.y + size.y + style->padding.w + scroll.y) * scale;
    if (mouse.x >= tlx && mouse.x <= brx && mouse.y >= tly && mouse.y <= bry) {
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

#ifndef RUNNING_FILTERDESIGNER_TESTS

void Graph::ApplyTheme() {
  // wpigui ships several themes (Classic / Dark / Light / DeepDark) that
  // the user can switch from Glass's View menu mid-session. Mirroring the
  // active ImGui style's colors into ImNodeFlow's grid + per-node style
  // fields each frame keeps the editor coherent with the rest of the UI
  // without the user having to relaunch.
  const ImVec4* cols = ImGui::GetStyle().Colors;

  // Grid. Use the host's WindowBg as the canvas color and the Separator
  // color (subtly visible against any theme) for grid lines, dimmed for
  // sub-grid. ChildBg can be near-transparent in some themes; WindowBg is
  // always opaque and is what reads as "the editor's background plane."
  auto& gridStyle = m_editor->getStyle();
  gridStyle.colors.background =
      ImGui::ColorConvertFloat4ToU32(cols[ImGuiCol_WindowBg]);
  ImVec4 gridLine = cols[ImGuiCol_Separator];
  gridLine.w *= 0.45f;
  gridStyle.colors.grid = ImGui::ColorConvertFloat4ToU32(gridLine);
  ImVec4 subGridLine = cols[ImGuiCol_Separator];
  subGridLine.w *= 0.18f;
  gridStyle.colors.subGrid = ImGui::ColorConvertFloat4ToU32(subGridLine);
  // The ChildBg PushStyleColor inside ContainedContext::begin uses
  // cfg.color directly, so push the themed bg there too.
  m_editor->getGrid().config().color = gridStyle.colors.background;

  // Per-node body bg + border colors. Node styles are shared_ptr-shared
  // across instances of the same NodeStyle::cyan() etc. — mutating one
  // mutates all instances of that category, which is what we want.
  // FrameBg reads as the "input field surface" in every theme, which
  // matches the visual role of the node body. Border tracks the theme's
  // Border color; selection border uses the theme's HeaderActive (the
  // "I have focus" color).
  const ImU32 bodyBg = ImGui::ColorConvertFloat4ToU32(cols[ImGuiCol_FrameBg]);
  const ImU32 border = ImGui::ColorConvertFloat4ToU32(cols[ImGuiCol_Border]);
  const ImU32 borderSel =
      ImGui::ColorConvertFloat4ToU32(cols[ImGuiCol_HeaderActive]);
  for (auto& [uid, node] : m_editor->getNodes()) {
    if (!node) {
      continue;
    }
    const auto& style = node->getStyle();
    if (!style) {
      continue;
    }
    style->bg = bodyBg;
    style->border_color = border;
    style->border_selected_color = borderSel;
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void Graph::ApplyTheme() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
