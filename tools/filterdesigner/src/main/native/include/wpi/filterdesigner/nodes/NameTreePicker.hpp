// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cfloat>
#include <string>
#include <string_view>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "wpi/filterdesigner/nodes/NameTree.hpp"

namespace wpi::filterdesigner {

namespace detail {

/**
 * Renders one node of a @ref NameTreeNode tree and its surviving descendants.
 *
 * @param node         Node to render.
 * @param search       Live filter; a node whose subtree contains no match is
 *                     skipped entirely.
 * @param selected     Full path of the currently picked item, so its row can
 *                     be highlighted and focused when the popup opens.
 * @param forceOpen    Branches default to open, so a filtered tree shows its
 *                     matches without further clicks.
 * @param onSelect     Called with the full path of a selectable item's row
 *                     when it is clicked.
 */
template <typename OnSelect>
void RenderNameTreeNode(const NameTreeNode& node, std::string_view search,
                        std::string_view selected, bool forceOpen,
                        const OnSelect& onSelect) {
  if (!NameTreeNodeMatchesSearch(node, search)) {
    return;
  }
  if (!node.fullPath.empty()) {
    const bool isSelected = node.fullPath == selected;
    // Selectable rows indent to align with branch content. A TreeNodeEx leaf
    // would draw a disclosure arrow on a row that cannot expand, which looks
    // off — render as a regular Selectable instead.
    ImGui::Indent();
    // A greyed row rather than a hidden one: an entry the tool cannot plot is
    // still an entry the user went looking for, and its absence would read as
    // a missing log rather than as a wrong type.
    ImGui::BeginDisabled(!node.selectable);
    if (ImGui::Selectable(node.label.c_str(), isSelected)) {
      onSelect(node.fullPath);
    }
    ImGui::EndDisabled();
    ImGui::Unindent();
    if (isSelected) {
      ImGui::SetItemDefaultFocus();
    }
  }
  if (!node.children.empty()) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
    if (forceOpen) {
      flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    if (ImGui::TreeNodeEx(node.name.c_str(), flags)) {
      for (const auto& child : node.children) {
        RenderNameTreeNode(child, search, selected, forceOpen, onSelect);
      }
      ImGui::TreePop();
    }
  }
}

}  // namespace detail

/**
 * Draws a search box and a path-split tree of @p root's children — the body of
 * an already-open @c BeginCombo. Both source nodes pick from lists of hundreds
 * of names, so pass @c ImGuiComboFlags_HeightLarge to the hosting
 * @c BeginCombo.
 *
 * @param root      Tree from @ref BuildNameTree. The root is a container, so
 *                  its children are what get rendered.
 * @param search    Live query, owned by the caller so it survives the frame
 *                  and persists while the popup is open.
 * @param selected  Full path of the currently picked item, or empty.
 * @param onSelect  Called with the full path of the clicked item. Non-
 *                  selectable rows are greyed and never fire it.
 */
template <typename OnSelect>
void DrawNameTreePicker(const NameTreeNode& root, std::string& search,
                        std::string_view selected, const OnSelect& onSelect) {
  ImGui::SetNextItemWidth(-FLT_MIN);
  ImGui::InputTextWithHint("##nameTreeSearch", "Search...", &search);
  // With a query on, every surviving branch opens: the point of typing is to
  // see the matches, not to be told which branches to click into.
  const bool forceOpen = !search.empty();
  for (const auto& child : root.children) {
    detail::RenderNameTreeNode(child, search, selected, forceOpen, onSelect);
  }
}

}  // namespace wpi::filterdesigner
