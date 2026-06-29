// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class Graph;
class NodeRegistry;

/** Current .fdsgn file format version emitted + accepted by this build. */
inline constexpr int kFdsgnVersion = 2;

/** Writes the live graph as a .fdsgn v2 JSON string. */
std::string SerializeGraph(const Graph& graph);

struct DeserializeResult {
  /** Empty on success; a user-facing message otherwise. */
  std::string error;
  /** Non-fatal warnings (e.g. unknown node types are skipped + warned). */
  std::vector<std::string> warnings;

  bool ok() const { return error.empty(); }
};

/**
 * Reads a .fdsgn v2 JSON string and rebuilds the graph contents in @p graph.
 * Clears the graph first. v1 files (or anything missing version: 2) are
 * rejected with a clear, user-facing error.
 *
 * Unknown node types log a warning and skip the node — the remaining graph
 * still loads. Links referencing skipped nodes are dropped silently.
 */
DeserializeResult DeserializeGraph(std::string_view jsonText, Graph& graph,
                                   const NodeRegistry& registry);

/**
 * Convenience wrappers around the std::string variants, used by the UI to
 * load/save through a file path.
 */
std::string SaveGraphToFile(std::string_view path, const Graph& graph);
DeserializeResult LoadGraphFromFile(std::string_view path, Graph& graph,
                                    const NodeRegistry& registry);

}  // namespace wpi::filterdesigner
