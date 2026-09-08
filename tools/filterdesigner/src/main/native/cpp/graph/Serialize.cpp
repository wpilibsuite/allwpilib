// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Serialize.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <fstream>
#include <ios>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

namespace {

using wpi::util::json;

constexpr const char* kRejectV1Message =
    "This .fdsgn file uses an older format (v1) that this tool no longer "
    "supports. Rebuild the design as a node graph and re-save.";

// A loaded node id is bumped by one to mint the next fresh one, and every
// AddNode after the load increments that counter again. Leaving room for a
// single increment is not enough, then: a file naming an id near the top of
// the range would hand the counter to AddNode already at the ceiling. Ids
// only ever count the nodes in one design, so half the range is a ceiling
// no real file approaches and no amount of clicking can climb to.
constexpr int kMaxGraphInt = std::numeric_limits<int>::max() / 2;

// Where a loaded design's top-left node lands, in grid units. Far enough in
// that the node's own frame and title aren't flush against the canvas edge,
// close enough that a fresh viewport shows it.
constexpr float kLoadedGraphMargin = 40.0f;

// Ids and the version live in an int, so on top of ReadJsonInt's range check
// a graph int must be non-negative and within that ceiling.
std::optional<int> ReadGraphInt(const json& value) {
  const std::optional<int> v = ReadJsonInt(value);
  if (!v || *v < 0 || *v > kMaxGraphInt) {
    return std::nullopt;
  }
  return v;
}

// ImFlow::BaseNode's own inPin/outPin assert and UB-deref on a miss, which
// the load path cannot afford: its pin names come from arbitrary JSON.
ImFlow::Pin* FindOutPinByName(ImFlow::BaseNode* node, std::string_view name) {
  for (const auto& pin : node->getOuts()) {
    if (pin && pin->getName() == name) {
      return pin.get();
    }
  }
  return nullptr;
}

ImFlow::Pin* FindInPinByName(ImFlow::BaseNode* node, std::string_view name) {
  for (const auto& pin : node->getIns()) {
    if (pin && pin->getName() == name) {
      return pin.get();
    }
  }
  return nullptr;
}

// Nodes are saved at the grid coordinates they were placed at, which include
// however far the user had panned by then. Loading rebuilds the editor, and
// ImNodeFlow starts a fresh one at scroll zero and exposes no way to move it,
// so a design built after a long pan would reopen on what looks like an empty
// canvas with every node off-screen and no fit action to get back to them.
// Only the nodes' positions relative to one another carry meaning, so bring
// the whole set to the corner the new viewport is already looking at.
void RecenterLoadedNodes(Graph& graph, DeserializeResult& result) {
  const std::vector<FilterDesignerNode*> loaded = graph.Nodes();
  if (loaded.empty()) {
    return;
  }
  double minX = std::numeric_limits<double>::max();
  double minY = std::numeric_limits<double>::max();
  for (FilterDesignerNode* node : loaded) {
    minX = std::min(minX, static_cast<double>(node->getPos().x));
    minY = std::min(minY, static_cast<double>(node->getPos().y));
  }
  const double shiftX = kLoadedGraphMargin - minX;
  const double shiftY = kLoadedGraphMargin - minY;

  // Every position is a finite float on its own, but a design spanning most
  // of that range has no shift that keeps all of them finite. Moving only the
  // ones that fit would rearrange the graph, which is worse than leaving a
  // file no one can have authored where it is.
  for (FilterDesignerNode* node : loaded) {
    const double x = static_cast<double>(node->getPos().x) + shiftX;
    const double y = static_cast<double>(node->getPos().y) + shiftY;
    if (std::abs(x) > std::numeric_limits<float>::max() ||
        std::abs(y) > std::numeric_limits<float>::max()) {
      result.warnings.emplace_back(
          "Nodes are spread too far apart to bring on screen together; "
          "leaving them where the file put them");
      return;
    }
  }
  for (FilterDesignerNode* node : loaded) {
    node->setPos(ImVec2{static_cast<float>(node->getPos().x + shiftX),
                        static_cast<float>(node->getPos().y + shiftY)});
  }
}

}  // namespace

std::string SerializeGraph(const Graph& graph) {
  auto nodesArr = json::array();
  for (FilterDesignerNode* node : graph.Nodes()) {
    json entry = json::object("id", node->GraphId(), "type",
                              std::string{node->TypeTag()}, "pos",
                              json::array(node->getPos().x, node->getPos().y));
    node->SerializeParams(entry);
    nodesArr.emplace_back(std::move(entry));
  }

  auto linksArr = json::array();
  for (const auto& link : graph.Links()) {
    linksArr.emplace_back(json::object(
        "src", json::object("node", link.srcId, "pin", link.srcPin), "dst",
        json::object("node", link.dstId, "pin", link.dstPin)));
  }

  json root = json::object("version", kFdsgnVersion, "nodes",
                           std::move(nodesArr), "links", std::move(linksArr));
  return root.to_string_pretty();
}

DeserializeResult DeserializeGraph(std::string_view jsonText, Graph& graph,
                                   const NodeRegistry& registry) {
  DeserializeResult result;

  auto parsed = json::parse(jsonText);
  if (!parsed) {
    result.error = std::format("Malformed JSON: {}", parsed.error());
    return result;
  }

  const json& root = *parsed;
  if (!root.is_object()) {
    result.error = "File root must be a JSON object";
    return result;
  }

  // v1 is the pre-node-graph format, and gets its own message; a version
  // newer than this build knows is rejected outright.
  const json* versionNode = root.lookup("version");
  if (!versionNode || !versionNode->is_number()) {
    result.error = kRejectV1Message;
    return result;
  }
  const std::optional<int> parsedVersion = ReadGraphInt(*versionNode);
  if (!parsedVersion) {
    result.error =
        "Unsupported .fdsgn version (not a whole number this build can read)";
    return result;
  }
  const int version = *parsedVersion;
  if (version == 1) {
    result.error = kRejectV1Message;
    return result;
  }
  if (version != kFdsgnVersion) {
    result.error = "Unsupported .fdsgn version " + std::to_string(version) +
                   " (this build understands version " +
                   std::to_string(kFdsgnVersion) + ")";
    return result;
  }

  const json* nodesNode = root.lookup("nodes");
  const json* linksNode = root.lookup("links");
  if (!nodesNode || !nodesNode->is_array()) {
    result.error = "Missing or non-array \"nodes\"";
    return result;
  }
  if (!linksNode || !linksNode->is_array()) {
    result.error = "Missing or non-array \"links\"";
    return result;
  }

  graph.Reset();

  // Ids must be unique or link restoration is a coin toss between the
  // duplicates, silently, and the next save keeps the collision.
  std::unordered_set<int> seenIds;
  for (const json& entry : nodesNode->get_array()) {
    if (!entry.is_object()) {
      result.warnings.emplace_back("Skipping non-object node entry");
      continue;
    }
    const json* idNode = entry.lookup("id");
    const json* typeNode = entry.lookup("type");
    const json* posNode = entry.lookup("pos");
    if (!idNode || !typeNode || !typeNode->is_string() || !posNode ||
        !posNode->is_array() || posNode->get_array().size() != 2) {
      result.warnings.emplace_back(
          "Skipping node with missing id/type/pos fields");
      continue;
    }
    const std::optional<int> parsedId = ReadGraphInt(*idNode);
    if (!parsedId) {
      result.warnings.emplace_back(
          "Skipping node whose id is not a whole number in range");
      continue;
    }
    const int id = *parsedId;
    const std::string& type = typeNode->get_string();
    const auto& pos = posNode->get_array();
    // ImNodeFlow keeps node positions in floats and does canvas arithmetic
    // on them every frame, so an infinity here would put the node where the
    // canvas can never scroll to and take the drawing around it with it.
    const std::optional<float> px = ReadJsonFloat(pos[0]);
    const std::optional<float> py = ReadJsonFloat(pos[1]);
    if (!px || !py) {
      result.warnings.emplace_back(
          "Skipping node whose pos coordinates are not numbers the canvas "
          "can hold");
      continue;
    }
    ImVec2 p{*px, *py};

    if (!seenIds.insert(id).second) {
      result.warnings.emplace_back("Duplicate node id " + std::to_string(id) +
                                   " — skipped");
      continue;
    }

    const NodeRegistry::Entry* regEntry = registry.FindByTag(type);
    if (!regEntry) {
      result.warnings.emplace_back("Unknown node type '" + type +
                                   "' — skipped");
      continue;
    }
    auto node = regEntry->factory(graph, p);
    if (!node) {
      result.warnings.emplace_back("Factory for '" + type + "' returned null");
      continue;
    }
    // The factory minted a fresh id; overwrite it with the one from disk and
    // re-bump the counter past it.
    node->SetGraphId(id);
    graph.BumpNextIdAbove(id);
    node->DeserializeParams(entry);
  }

  RecenterLoadedNodes(graph, result);

  for (const json& link : linksNode->get_array()) {
    if (!link.is_object()) {
      result.warnings.emplace_back("Skipping non-object link entry");
      continue;
    }
    const json* srcNode = link.lookup("src");
    const json* dstNode = link.lookup("dst");
    if (!srcNode || !srcNode->is_object() || !dstNode ||
        !dstNode->is_object()) {
      result.warnings.emplace_back("Skipping link missing src/dst");
      continue;
    }
    const json* srcId = srcNode->lookup("node");
    const json* srcPin = srcNode->lookup("pin");
    const json* dstId = dstNode->lookup("node");
    const json* dstPin = dstNode->lookup("pin");
    if (!srcId || !srcPin || !srcPin->is_string() || !dstId || !dstPin ||
        !dstPin->is_string()) {
      result.warnings.emplace_back("Skipping link with malformed endpoints");
      continue;
    }
    const std::optional<int> srcGraphId = ReadGraphInt(*srcId);
    const std::optional<int> dstGraphId = ReadGraphInt(*dstId);
    if (!srcGraphId || !dstGraphId) {
      result.warnings.emplace_back(
          "Skipping link whose endpoint id is not a whole number in range");
      continue;
    }
    FilterDesignerNode* src = graph.FindNodeById(*srcGraphId);
    FilterDesignerNode* dst = graph.FindNodeById(*dstGraphId);
    if (!src || !dst) {
      // An endpoint listed in "nodes" was skipped there with its own warning.
      // One the file never defines has had none, and dropping its link in
      // silence reports a clean load and then makes the loss permanent on
      // the next save.
      if (!seenIds.contains(*srcGraphId)) {
        result.warnings.emplace_back("Link references node " +
                                     std::to_string(*srcGraphId) +
                                     ", which the file never defines — "
                                     "skipped");
      }
      if (*dstGraphId != *srcGraphId && !seenIds.contains(*dstGraphId)) {
        result.warnings.emplace_back("Link references node " +
                                     std::to_string(*dstGraphId) +
                                     ", which the file never defines — "
                                     "skipped");
      }
      continue;
    }
    ImFlow::Pin* outPin = FindOutPinByName(src, srcPin->get_string());
    ImFlow::Pin* inPin = FindInPinByName(dst, dstPin->get_string());
    if (!outPin || !inPin) {
      result.warnings.emplace_back("Link references missing pin; skipped");
      continue;
    }
    // ImNodeFlow's createLink is a toggle for the same output and a silent
    // replacement for a different one: a file listing a link twice would load
    // with no wire, and one listing two sources for the same input would keep
    // whichever came last. Either way the next save makes it permanent, so
    // the first link into an input wins and the rest are reported.
    if (auto existing = inPin->getLink().lock()) {
      const char* what =
          existing->left() == outPin ? "Duplicate" : "Conflicting";
      result.warnings.emplace_back(std::string{what} + " link into pin '" +
                                   dstPin->get_string() + "' of node " +
                                   std::to_string(*dstGraphId) + " — skipped");
      continue;
    }
    inPin->createLink(outPin);
    // createLink returns void and declines in silence whenever its connection
    // filter says no — a filter output wired into a signal input, say — or
    // when the two ends share a node. The wire is gone from the graph either
    // way, and the next save writes it out without it.
    if (auto created = inPin->getLink().lock();
        !created || created->left() != outPin) {
      result.warnings.emplace_back(
          "Link from node " + std::to_string(*srcGraphId) + " pin '" +
          srcPin->get_string() + "' to node " + std::to_string(*dstGraphId) +
          " pin '" + dstPin->get_string() +
          "' is not a valid connection — "
          "skipped");
    }
  }

  return result;
}

std::string SaveGraphToFile(std::string_view path, const Graph& graph) {
  std::string text = SerializeGraph(graph);
  std::ofstream out(std::string{path}, std::ios::binary | std::ios::trunc);
  if (!out) {
    return std::format("Could not open for writing: {}", path);
  }
  out << text;
  // Not just the stream state here: the last buffered chunk is flushed by the
  // destructor, whose failure nothing can observe. A full disk would report a
  // clean save over a truncated file.
  out.close();
  if (!out) {
    return std::format("Write failed: {}", path);
  }
  return {};
}

DeserializeResult LoadGraphFromFile(std::string_view path, Graph& graph,
                                    const NodeRegistry& registry) {
  DeserializeResult result;
  std::ifstream in(std::string{path}, std::ios::binary);
  if (!in) {
    result.error = std::format("Could not open: {}", path);
    return result;
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return DeserializeGraph(ss.str(), graph, registry);
}

}  // namespace wpi::filterdesigner
