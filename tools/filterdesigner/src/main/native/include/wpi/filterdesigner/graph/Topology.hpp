// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

namespace wpi::filterdesigner {

class FilterDesignerNode;
class Graph;

/**
 * Returns true if any directed cycle exists in the graph. Links go from a
 * src out-pin to a dst in-pin; a cycle is a directed loop in that relation.
 *
 * Pure function of @ref Graph::Nodes() and @ref Graph::Links() — testable
 * without an ImGui context.
 */
bool HasCycle(const Graph& graph);

/**
 * Returns a cycle as an ordered list of graph-local node ids with the entry
 * node repeated at the end (e.g. `[3, 5, 3]` for a two-node cycle). Returns
 * an empty vector when the graph is acyclic. Finds *some* cycle, not
 * necessarily a shortest one — the first one DFS hits.
 */
std::vector<int> FindCycle(const Graph& graph);

/**
 * Formats a cycle path returned by @ref FindCycle as a human-readable string,
 * e.g. `"Biquad Stage[3] → Biquad Stage[5] → Biquad Stage[3]"`. Uses the
 * node title (from `setTitle`) so the message matches what the user sees on
 * the node header, with the graph-local id appended to disambiguate multiple
 * nodes of the same type. Returns an empty string for an empty path.
 */
std::string FormatCycle(const Graph& graph, const std::vector<int>& path);

/**
 * If @p node's owning graph currently has a cycle, draws a red banner with
 * the formatted cycle path and returns true. Callers should bail before any
 * `getInVal` / pin-behaviour-evaluating call so the cycle doesn't recurse
 * through ImNodeFlow's pin pulls.
 *
 * Returns false (drawing nothing) when @p node is null, has no owning graph
 * (test fixtures), or the graph is acyclic. Lives here next to the cycle
 * detection so the banner format and the cycle check share one home.
 */
bool DrawCycleBannerIfAny(FilterDesignerNode* node);

}  // namespace wpi::filterdesigner
