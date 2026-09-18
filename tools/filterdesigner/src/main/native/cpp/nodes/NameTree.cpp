// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NameTree.hpp"

#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/util/StringExtras.hpp"

namespace wpi::filterdesigner {

namespace {

void SortTree(NameTreeNode& node) {
  std::ranges::sort(node.children, {}, &NameTreeNode::name);
  for (auto& child : node.children) {
    SortTree(child);
  }
}

}  // namespace

NameTreeNode BuildNameTree(std::span<const NameTreeItem> items) {
  NameTreeNode root;
  std::vector<std::string_view> parts;
  for (const auto& item : items) {
    parts.clear();
    // Split on the first ':' when one is present and the text before it is
    // not itself a path — datalogtool's rule, verbatim. `NT` becomes its own
    // level; `/Shooter/ratio:1` is left alone.
    auto [prefix, mainpart] = wpi::util::split(item.name, ':');
    if (mainpart.empty() || wpi::util::contains(prefix, '/')) {
      mainpart = item.name;
    } else {
      parts.emplace_back(prefix);
    }
    // -1: split at every '/', however many there are. false: drop the empty
    // segments, so a leading '/' does not open an unnamed branch.
    wpi::util::split(mainpart, '/', -1, false,
                     [&](auto part) { parts.emplace_back(part); });

    // A name that is all separators ("/", "") names nothing.
    if (parts.empty()) {
      continue;
    }

    NameTreeNode* cursor = &root;
    for (std::size_t i = 0; i < parts.size(); ++i) {
      const std::string_view part = parts[i];
      const bool leaf = i + 1 == parts.size();
      auto it = std::ranges::find(cursor->children, part, &NameTreeNode::name);
      if (leaf) {
        // Two names can split to the same segments — "/foo" and "foo", or
        // "NT:/x" and "NT/x". A leaf takes the node of its name only while
        // that node stands for nothing else; otherwise it gets a sibling, so
        // neither entry hides the other from the picker.
        it = std::ranges::find_if(cursor->children, [&](const NameTreeNode& n) {
          return n.name == part &&
                 (n.fullPath.empty() || n.fullPath == item.name);
        });
      }
      if (it == cursor->children.end()) {
        cursor->children.emplace_back().name = part;
        it = cursor->children.end() - 1;
      }
      if (leaf) {
        it->fullPath = std::string{item.name};
        it->type = std::string{item.type};
        // Two names that split alike keep a row each, and those rows would
        // otherwise carry the same text — "foo  [double]" twice. ImGui derives
        // a Selectable's id from that text, so the pair would share one id and
        // with it their click, focus and selection state. The hidden suffix is
        // the full name, which is what tells them apart in the first place.
        it->label = it->name + "  [" + it->type + "]##" + it->fullPath;
        it->selectable = item.selectable;
      }
      cursor = &*it;
    }
  }
  SortTree(root);
  return root;
}

bool NameTreeNodeMatchesSearch(const NameTreeNode& node,
                               std::string_view search) {
  if (search.empty()) {
    return true;
  }
  if (!node.fullPath.empty() &&
      wpi::util::contains_lower(node.fullPath, search)) {
    return true;
  }
  return std::ranges::any_of(node.children, [&](const NameTreeNode& child) {
    return NameTreeNodeMatchesSearch(child, search);
  });
}

}  // namespace wpi::filterdesigner
