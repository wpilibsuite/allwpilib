// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/CreationPopup.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <vector>

#include <ImNodeFlow.h>
#include <imgui.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"

namespace wpi::filterdesigner {

namespace {

constexpr const char* kExternalPopupId = "##fd_creation_external";

/**
 * Returns the list of pin types that would let a newly-created node accept
 * @p draggedPin. If the dragged pin is an output, we need a matching input
 * type on the candidate node; if it's an input, we need a matching output.
 */
const std::vector<std::type_index>& AcceptablePinTypes(
    const NodeRegistry::Entry& entry, ImFlow::PinType draggedType) {
  return draggedType == ImFlow::PinType_Output ? entry.inputTypes
                                               : entry.outputTypes;
}

bool MatchesFilter(const NodeRegistry::Entry& entry, ImFlow::Pin* draggedPin) {
  if (!draggedPin) {
    return true;
  }
  const auto& pool = AcceptablePinTypes(entry, draggedPin->getType());
  std::type_index draggedType{draggedPin->getDataType()};
  return std::find(pool.begin(), pool.end(), draggedType) != pool.end();
}

/**
 * After creating a node, find a pin on the new node that matches the dragged
 * pin's type, and link them. Picks the first compatible pin — sufficient for
 * single-output sources + sinks with one matching input.
 */
void AutoLink(FilterDesignerNode& node, ImFlow::Pin* draggedPin) {
  if (!draggedPin) {
    return;
  }
  const std::type_info& draggedType = draggedPin->getDataType();
  const auto& candidates =
      draggedPin->getType() == ImFlow::PinType_Output ? node.getIns()
                                                      : node.getOuts();
  for (const auto& pin : candidates) {
    if (pin->getDataType() == draggedType) {
      if (draggedPin->getType() == ImFlow::PinType_Output) {
        pin->createLink(draggedPin);
      } else {
        draggedPin->createLink(pin.get());
      }
      return;
    }
  }
}

}  // namespace

CreationPopup::CreationPopup(Graph& graph, const NodeRegistry& registry)
    : m_graph(&graph), m_registry(&registry) {}

void CreationPopup::Attach() {
  ImFlow::ImNodeFlow& editor = m_graph->Editor();

  editor.rightClickPopUpContent([this](ImFlow::BaseNode* hovered) {
    if (hovered) {
      // Reserved for a future per-node popup (rename/delete/etc.). For M2
      // the right-click on a node falls through.
      return;
    }
    ImVec2 gridPos = m_graph->Editor().screen2grid(ImGui::GetMousePos());
    if (DrawMenuItems(gridPos, nullptr)) {
      ImGui::CloseCurrentPopup();
    }
  });

  editor.droppedLinkPopUpContent([this](ImFlow::Pin* dragged) {
    ImVec2 gridPos = m_graph->Editor().screen2grid(ImGui::GetMousePos());
    if (DrawMenuItems(gridPos, dragged)) {
      ImGui::CloseCurrentPopup();
    }
  });
}

void CreationPopup::RequestOpenAtMouse() {
  m_openExternalRequested = true;
}

void CreationPopup::DrawExternal() {
  if (m_openExternalRequested) {
    ImGui::OpenPopup(kExternalPopupId);
    m_openExternalRequested = false;
  }
  if (ImGui::BeginPopup(kExternalPopupId)) {
    ImVec2 gridPos = m_graph->Editor().screen2grid(ImGui::GetMousePos());
    if (DrawMenuItems(gridPos, nullptr)) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

bool CreationPopup::DrawMenuItems(const ImVec2& gridPos,
                                  ImFlow::Pin* draggedPin) {
  // Preserve registration order for stability, but group by category so the
  // popup reads top-down ("Sources", "Plots", etc.).
  std::vector<std::string> categoryOrder;
  std::unordered_set<std::string> seenCategories;
  for (const auto& entry : m_registry->All()) {
    if (seenCategories.insert(entry.menuCategory).second) {
      categoryOrder.push_back(entry.menuCategory);
    }
  }

  bool created = false;
  for (const std::string& category : categoryOrder) {
    // Filtered visible entries in this category for the current dragged pin.
    std::vector<const NodeRegistry::Entry*> visible;
    for (const auto& entry : m_registry->All()) {
      if (entry.menuCategory != category) {
        continue;
      }
      if (!MatchesFilter(entry, draggedPin)) {
        continue;
      }
      visible.push_back(&entry);
    }
    if (visible.empty()) {
      continue;
    }

    auto renderEntries = [&]() {
      for (const NodeRegistry::Entry* entry : visible) {
        if (ImGui::MenuItem(entry->menuLabel.c_str())) {
          auto node = entry->factory(*m_graph, gridPos);
          if (node) {
            AutoLink(*node, draggedPin);
            created = true;
          }
        }
      }
    };

    if (category.empty()) {
      renderEntries();
    } else if (ImGui::BeginMenu(category.c_str())) {
      renderEntries();
      ImGui::EndMenu();
    }
  }

  return created;
}

}  // namespace wpi::filterdesigner

#else  // RUNNING_FILTERDESIGNER_TESTS

namespace wpi::filterdesigner {
class Graph;
class NodeRegistry;
CreationPopup::CreationPopup(Graph& graph, const NodeRegistry& registry)
    : m_graph(&graph), m_registry(&registry) {}
void CreationPopup::Attach() {}
void CreationPopup::RequestOpenAtMouse() {}
void CreationPopup::DrawExternal() {}
bool CreationPopup::DrawMenuItems(const ImVec2&, ImFlow::Pin*) { return false; }
}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
