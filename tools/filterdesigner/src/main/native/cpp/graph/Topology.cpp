// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Topology.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <imgui.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/nodes/StatusText.hpp"

namespace wpi::filterdesigner {

namespace {

// Iterative three-color DFS, returning the first back-edge cycle as an ordered
// id list with the entry id repeated at the end. Iterative because a long user
// chain shouldn't risk a stack overflow inside a per-frame check.
std::vector<int> FindCycleImpl(
    const std::vector<int>& ids,
    const std::unordered_map<int, std::vector<int>>& adj) {
  enum Color : char { White, Gray, Black };
  std::unordered_map<int, Color> color;
  color.reserve(ids.size());
  for (int id : ids) {
    color[id] = White;
  }

  for (int start : ids) {
    if (color[start] != White) {
      continue;
    }

    // Frames are (node, next-neighbor-index), and the stack doubles as the
    // current DFS path: the cycle is its suffix from the gray target on.
    std::vector<std::pair<int, std::size_t>> stack;
    stack.emplace_back(start, 0);
    color[start] = Gray;

    while (!stack.empty()) {
      auto& [cur, idx] = stack.back();
      // at() rather than a find()/ternary, whose mixed value categories
      // collapse to a prvalue and copy the neighbor vector every iteration.
      // FindCycle seeds adj for every id, so the lookup always hits.
      const auto& outs = adj.at(cur);
      if (idx < outs.size()) {
        int next = outs[idx++];
        auto cIt = color.find(next);
        if (cIt == color.end()) {
          // Link target outside the live node set.
          continue;
        }
        if (cIt->second == Gray) {
          // Back-edge → cycle. Slice the path out of the stack.
          std::vector<int> path;
          bool started = false;
          for (auto& frame : stack) {
            if (!started && frame.first == next) {
              started = true;
            }
            if (started) {
              path.push_back(frame.first);
            }
          }
          path.push_back(next);
          return path;
        }
        if (cIt->second == White) {
          cIt->second = Gray;
          stack.emplace_back(next, 0);
        }
        // Black neighbors are fully explored — no cycle through them.
      } else {
        color[cur] = Black;
        stack.pop_back();
      }
    }
  }
  return {};
}

}  // namespace

bool HasCycle(const Graph& graph) {
  return !FindCycle(graph).empty();
}

std::vector<int> FindCycle(const Graph& graph) {
  // Snapshot ids and adjacency once: Nodes() and Links() each walk
  // ImNodeFlow's internal containers.
  auto nodes = graph.Nodes();
  std::vector<int> ids;
  ids.reserve(nodes.size());
  for (auto* n : nodes) {
    ids.push_back(n->GraphId());
  }

  std::unordered_set<int> known(ids.begin(), ids.end());
  std::unordered_map<int, std::vector<int>> adj;
  adj.reserve(ids.size());
  for (int id : ids) {
    adj[id];  // Ensure every node has an entry, even with no out-edges.
  }
  for (const auto& l : graph.Links()) {
    // A graph mid-teardown can surface a half-deleted link.
    if (!known.count(l.srcId) || !known.count(l.dstId)) {
      continue;
    }
    adj[l.srcId].push_back(l.dstId);
  }

  return FindCycleImpl(ids, adj);
}

std::string FormatCycle(const Graph& graph, const std::vector<int>& path) {
  if (path.empty()) {
    return {};
  }

  std::unordered_map<int, FilterDesignerNode*> byId;
  for (auto* n : graph.Nodes()) {
    byId[n->GraphId()] = n;
  }

  std::string out;
  for (std::size_t i = 0; i < path.size(); ++i) {
    if (i > 0) {
      out += " \xe2\x86\x92 ";  // U+2192 RIGHTWARDS ARROW, UTF-8.
    }
    int id = path[i];
    auto it = byId.find(id);
    if (it != byId.end() && it->second) {
      const std::string& title = it->second->getName();
      out += title.empty() ? std::string{it->second->TypeTag()} : title;
      out += '[';
      out += std::to_string(id);
      out += ']';
    } else {
      // Unreachable from FindCycle, which filters unknown ids; keeps the
      // formatter total for a synthetic path.
      out += '[';
      out += std::to_string(id);
      out += ']';
    }
  }
  return out;
}

bool DrawCycleBannerIfAny(FilterDesignerNode* node) {
  if (!node) {
    return false;
  }
  Graph* g = node->GetGraph();
  if (!g || g->CycleError().empty()) {
    return false;
  }
  DrawStatusText(kStatusErrorColor, "Graph cycle: " + g->CycleError());
  return true;
}

}  // namespace wpi::filterdesigner
