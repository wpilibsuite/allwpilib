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

#ifndef RUNNING_FILTERDESIGNER_TESTS
namespace {

// Clipboard handlers installed on ImNodeFlow's inner context. ImGui invokes
// them with that context current and its Platform_ClipboardUserData holding
// the host context, whose handlers (the SDL backend's) do the work. Not the
// SDL handlers directly: they find their backend data through the current
// context's IO, which only the host has.
const char* ForwardGetClipboardText(ImGuiContext* inner) {
  auto* host = static_cast<ImGuiContext*>(
      ImGui::GetPlatformIO().Platform_ClipboardUserData);
  ImGui::SetCurrentContext(host);
  const char* text = ImGui::GetClipboardText();
  ImGui::SetCurrentContext(inner);
  return text;
}

void ForwardSetClipboardText(ImGuiContext* inner, const char* text) {
  auto* host = static_cast<ImGuiContext*>(
      ImGui::GetPlatformIO().Platform_ClipboardUserData);
  ImGui::SetCurrentContext(host);
  ImGui::SetClipboardText(text);
  ImGui::SetCurrentContext(inner);
}

}  // namespace
#endif  // RUNNING_FILTERDESIGNER_TESTS

Graph::Graph()
    : m_editor(std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph")) {
  ConfigureEditor();
}

Graph::~Graph() = default;

void Graph::ConfigureEditor() {
  // Zoom != 1.0 resamples the draw list, so strokes blur off scale 1; keep the
  // range narrow enough for that to stay acceptable. Pan is on left-mouse so
  // trackpad users can navigate; Update() resolves the collision with
  // node-drag and link-drag-out per frame.
  auto& cfg = m_editor->getGrid().config();
  cfg.zoom_enabled = true;
  cfg.zoom_min = 0.5f;
  cfg.zoom_max = 1.5f;
  cfg.scroll_button = ImGuiMouseButton_Left;
}

void Graph::Update() {
  // Before ImNodeFlow drives the sink draw()s, which read m_cycleError to
  // decide whether to skip their getInVal pulls. Links the user adds during
  // draw() are caught on the next frame.
  RecomputeCycleError();
#ifndef RUNNING_FILTERDESIGNER_TESTS
  ApplyTheme();
  // Hover-aware wheel routing. ImNodeFlow's grid-zoom check reads MouseWheel
  // from the outer context after the sub-context's frame has ended, while
  // ImPlot inside a node reads the sub-context's queue, copied from the outer
  // InputEventsTrail at begin(). Zeroing it here suppresses grid zoom for the
  // frame without touching the events the sub-context is about to receive.
  if (IsAnyNodeHovered()) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = 0.0f;
    io.MouseWheelH = 0.0f;
  }

  // Left-click pan only when the user isn't otherwise engaged: the same
  // MouseDelta that drives a node drag or a link drag-out would also pan the
  // canvas, moving nodes at 2x cursor speed. The flags are a frame behind, but
  // IsMouseDragging needs a frame of movement anyway, so the gate is set by
  // the time a drag deltas.
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
#ifndef RUNNING_FILTERDESIGNER_TESTS
  // After update(): the inner context is created inside the first one, and a
  // Reset() swaps in a fresh editor with a fresh context. Inner input runs a
  // frame behind the host, so a click can't reach a node before this has run.
  SyncInnerContext();
#endif
}

bool Graph::IsAnyNodeHovered() {
#ifdef RUNNING_FILTERDESIGNER_TESTS
  return false;
#else
  // Not BaseNode::isHovered(): its rect is m_pos + origin + scroll * scale,
  // while the drawn rect comes from sub-context vertices scaled on copy-out,
  // at (m_pos + scroll) * scale + origin. The two agree only at scale 1, so
  // recompute the rect with the transform the node is actually drawn with.
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

void Graph::SyncInnerContext() {
  ImGuiContext* inner = m_editor->getGrid().getRawContext();
  ImGuiContext* host = ImGui::GetCurrentContext();
  if (!inner || inner == host) {
    return;
  }
  // Same atlas on both sides, so the font pointer is valid in either.
  ImFont* hostFont = ImGui::GetIO().FontDefault;
  ImGui::SetCurrentContext(inner);
  ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
  platformIO.Platform_GetClipboardTextFn = ForwardGetClipboardText;
  platformIO.Platform_SetClipboardTextFn = ForwardSetClipboardText;
  platformIO.Platform_ClipboardUserData = host;
  ImGui::GetIO().FontDefault = hostFont;
  ImGui::SetCurrentContext(host);
}

void Graph::ApplyTheme() {
  // Per frame, because the user can switch wpigui's theme from Glass's View
  // menu mid-session.
  const ImVec4* cols = ImGui::GetStyle().Colors;

  // WindowBg rather than ChildBg, which is near-transparent in some themes.
  auto& gridStyle = m_editor->getStyle();
  gridStyle.colors.background =
      ImGui::ColorConvertFloat4ToU32(cols[ImGuiCol_WindowBg]);
  ImVec4 gridLine = cols[ImGuiCol_Separator];
  gridLine.w *= 0.45f;
  gridStyle.colors.grid = ImGui::ColorConvertFloat4ToU32(gridLine);
  ImVec4 subGridLine = cols[ImGuiCol_Separator];
  subGridLine.w *= 0.18f;
  gridStyle.colors.subGrid = ImGui::ColorConvertFloat4ToU32(subGridLine);
  // ContainedContext::begin pushes ChildBg from cfg.color directly.
  m_editor->getGrid().config().color = gridStyle.colors.background;

  // Node styles are shared_ptr-shared across instances of NodeStyle::cyan()
  // and friends, so mutating one mutates the whole category — which is what
  // the per-category header colors want. Only the body and borders follow the
  // theme.
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

void Graph::SyncInnerContext() {}
void Graph::ApplyTheme() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
