// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace wpi::filterdesigner {

/**
 * One flat name to file into a @ref NameTreeNode tree — an NT topic, or a
 * wpilog entry.
 *
 * Non-owning: @ref BuildNameTree copies whatever it keeps, so the views need
 * only outlive that one call.
 */
struct NameTreeItem {
  /** Full name, as the source spells it: "NT:/Spindexer/VelocityRPM". */
  std::string_view name;
  /** Short type label shown beside the leaf, e.g. "double". */
  std::string_view type;
  /** False for a name the picker should show greyed out and refuse — a
   * non-numeric wpilog entry. */
  bool selectable = true;
};

/**
 * One node of a path-split name tree. A non-empty @ref fullPath means the
 * node names a real item (with @ref type and @ref selectable set) and gets a
 * selectable row; a non-empty @ref children means it has descendants. Both
 * can hold at once, since a source may carry an item at `/foo` alongside
 * `/foo/bar`.
 */
struct NameTreeNode {
  /** This segment alone — what the row is labelled with. */
  std::string name;
  /** Full name of the item this node stands for, or empty for a pure
   * branch. */
  std::string fullPath;
  std::string type;
  /** Pre-formatted "name  [type]##fullPath" for the leaf's row — built once
   * with the tree rather than per frame, which is what @c LogEntry used to
   * cache for the flat combo. ImGui shows the text before the "##" and takes
   * the row's id from the whole string, so two leaves that split alike read
   * the same and still act independently. Empty for a pure branch, which
   * renders @ref name. */
  std::string label;
  bool selectable = true;
  /** Sorted by @ref name. */
  std::vector<NameTreeNode> children;
};

/**
 * Files every item in @p items into a tree, splitting each name on '/' — and,
 * before that, on a leading "prefix:" whose prefix holds no '/', which files
 * `NT:/Spindexer/VelocityRPM` under `NT` / `Spindexer` / `VelocityRPM` while
 * leaving `/Shooter/ratio:1` alone. Both rules come from datalogtool's
 * exporter, which files the same wpilog entries.
 *
 * Empty segments are dropped and a name that splits to nothing is skipped.
 * Two names that split alike — "/foo" and "foo" — each keep a row of their
 * own. Children are sorted by name at every level, so a wpilog's log-order
 * entry list still renders alphabetically.
 *
 * The returned root is a container: callers render @c root.children.
 */
NameTreeNode BuildNameTree(std::span<const NameTreeItem> items);

/**
 * True if @p node or any descendant names an item whose full path contains
 * @p search, case-insensitively. An empty @p search always matches, so the
 * unfiltered tree shows everything.
 *
 * The full path rather than the segment, so a query naming a parent
 * ("spindexer") still surfaces the leaves beneath it.
 */
bool NameTreeNodeMatchesSearch(const NameTreeNode& node,
                               std::string_view search);

}  // namespace wpi::filterdesigner
