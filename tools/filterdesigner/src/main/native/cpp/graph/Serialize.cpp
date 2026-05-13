// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Serialize.hpp"

#include <fstream>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

namespace {

using wpi::util::json;

constexpr const char* kRejectV1Message =
    "This file was saved by the linear-chain Filter Designer. Open it in "
    "that tool, or rebuild the design as a node graph.";

// Pin-name lookups on ImFlow::BaseNode (inPin/outPin) assert + UB-deref on
// miss — fine when the program knows the pin exists, but the load path is
// fed by arbitrary JSON and must tolerate unknown names. Iterate the public
// pin vectors directly so a stale or forward-compat .fdsgn file produces a
// skip-with-warning instead of crashing the tool.
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
    result.error = std::string{"Malformed JSON: "} + parsed.error();
    return result;
  }

  const json& root = *parsed;
  if (!root.is_object()) {
    result.error = "File root must be a JSON object";
    return result;
  }

  // v1 files (the linear-chain Spec format, or the M1 spike) get a dedicated
  // user-facing message instead of a generic "bad version" error. Versions
  // newer than this build understands are rejected for forward compatibility.
  const json* versionNode = root.lookup("version");
  if (!versionNode || !versionNode->is_number()) {
    result.error = kRejectV1Message;
    return result;
  }
  int version = static_cast<int>(versionNode->get_number());
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

  for (const json& entry : nodesNode->get_array()) {
    if (!entry.is_object()) {
      result.warnings.emplace_back("Skipping non-object node entry");
      continue;
    }
    const json* idNode = entry.lookup("id");
    const json* typeNode = entry.lookup("type");
    const json* posNode = entry.lookup("pos");
    if (!idNode || !idNode->is_number() || !typeNode ||
        !typeNode->is_string() || !posNode || !posNode->is_array() ||
        posNode->get_array().size() != 2) {
      result.warnings.emplace_back(
          "Skipping node with missing id/type/pos fields");
      continue;
    }
    int id = static_cast<int>(idNode->get_number());
    const std::string& type = typeNode->get_string();
    const auto& pos = posNode->get_array();
    if (!pos[0].is_number() || !pos[1].is_number()) {
      result.warnings.emplace_back(
          "Skipping node with non-numeric pos coordinates");
      continue;
    }
    ImVec2 p{static_cast<float>(pos[0].get_number()),
             static_cast<float>(pos[1].get_number())};

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
    // The factory just minted a fresh id via Graph::AddNode; overwrite it
    // with the value from disk and re-bump the counter so subsequently-
    // created nodes don't clash with anything still to be loaded.
    node->SetGraphId(id);
    graph.BumpNextIdAbove(id);
    node->DeserializeParams(entry);
  }

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
    if (!srcId || !srcId->is_number() || !srcPin || !srcPin->is_string() ||
        !dstId || !dstId->is_number() || !dstPin || !dstPin->is_string()) {
      result.warnings.emplace_back("Skipping link with malformed endpoints");
      continue;
    }
    FilterDesignerNode* src =
        graph.FindNodeById(static_cast<int>(srcId->get_number()));
    FilterDesignerNode* dst =
        graph.FindNodeById(static_cast<int>(dstId->get_number()));
    if (!src || !dst) {
      // Endpoint was skipped earlier (unknown type, malformed) — drop the link
      // silently rather than spamming the warning list.
      continue;
    }
    ImFlow::Pin* outPin = FindOutPinByName(src, srcPin->get_string());
    ImFlow::Pin* inPin = FindInPinByName(dst, dstPin->get_string());
    if (!outPin || !inPin) {
      result.warnings.emplace_back("Link references missing pin; skipped");
      continue;
    }
    inPin->createLink(outPin);
  }

  return result;
}

std::string SaveGraphToFile(std::string_view path, const Graph& graph) {
  std::string text = SerializeGraph(graph);
  std::ofstream out(std::string{path}, std::ios::binary | std::ios::trunc);
  if (!out) {
    return std::string{"Could not open for writing: "} + std::string{path};
  }
  out << text;
  if (!out) {
    return std::string{"Write failed: "} + std::string{path};
  }
  return {};
}

DeserializeResult LoadGraphFromFile(std::string_view path, Graph& graph,
                                    const NodeRegistry& registry) {
  DeserializeResult result;
  std::ifstream in(std::string{path}, std::ios::binary);
  if (!in) {
    result.error = std::string{"Could not open: "} + std::string{path};
    return result;
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return DeserializeGraph(ss.str(), graph, registry);
}

}  // namespace wpi::filterdesigner
