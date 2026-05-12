// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/ui/GraphEditor.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <ImNodeFlow.h>
#include <imgui.h>

#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

namespace {

// Two throwaway debug nodes that exist only to prove the spike. They will be
// deleted in milestone 3 once the real source/filter/sink nodes land.

class DebugIntSourceNode final : public ImFlow::BaseNode {
 public:
  DebugIntSourceNode() {
    setTitle("Debug Int Source");
    setStyle(ImFlow::NodeStyle::green());
    addOUT<int>("out")->behaviour([this] { return m_value; });
  }

  void draw() override {
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderInt("##value", &m_value, -100, 100);
  }

  int value() const { return m_value; }
  void setValue(int v) { m_value = v; }

 private:
  int m_value = 42;
};

class DebugIntDisplayNode final : public ImFlow::BaseNode {
 public:
  DebugIntDisplayNode() {
    setTitle("Debug Int Display");
    setStyle(ImFlow::NodeStyle::cyan());
    addIN<int>("in", 0, ImFlow::ConnectionFilter::SameType());
  }

  void draw() override {
    int v = getInVal<int>("in");
    ImGui::Text("value = %d", v);
  }
};

// String tags used by the M1 spike serializer. Throwaway — milestone 2
// introduces a real NodeRegistry keyed by these tags.
constexpr const char* kDebugIntSourceTag = "DebugIntSource";
constexpr const char* kDebugIntDisplayTag = "DebugIntDisplay";

const char* TagFor(const ImFlow::BaseNode& node) {
  if (dynamic_cast<const DebugIntSourceNode*>(&node)) {
    return kDebugIntSourceTag;
  }
  if (dynamic_cast<const DebugIntDisplayNode*>(&node)) {
    return kDebugIntDisplayTag;
  }
  return nullptr;
}

std::string JoinPath(std::string_view dir, std::string_view file) {
  if (dir.empty()) {
    return std::string{file};
  }
  std::string out{dir};
  if (out.back() != '/' && out.back() != '\\') {
    out.push_back('/');
  }
  out.append(file);
  return out;
}

}  // namespace

GraphEditor::GraphEditor(std::string_view saveDir)
    : m_editor(std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph")),
      m_savePath(JoinPath(saveDir, "filterdesigner-nodegraph-spike.fdsgn")) {
  PopulateDefaultGraph();
}

GraphEditor::~GraphEditor() = default;

void GraphEditor::PopulateDefaultGraph() {
  auto src = m_editor->addNode<DebugIntSourceNode>(ImVec2(40.0f, 60.0f));
  auto dst = m_editor->addNode<DebugIntDisplayNode>(ImVec2(280.0f, 60.0f));

  ImFlow::Pin* outPin = src->outPin("out");
  ImFlow::Pin* inPin = dst->inPin("in");
  if (outPin && inPin) {
    inPin->createLink(outPin);
  }
}

void GraphEditor::SaveToDisk() {
  using wpi::util::json;

  auto nodesArr = json::array();
  for (const auto& [uid, node] : m_editor->getNodes()) {
    const char* tag = TagFor(*node);
    if (!tag) {
      continue;
    }
    json entry = json::object(
        "id", static_cast<long long>(uid), "type", std::string{tag}, "pos",
        json::array(node->getPos().x, node->getPos().y));
    if (auto* src = dynamic_cast<DebugIntSourceNode*>(node.get())) {
      entry["value"] = src->value();
    }
    nodesArr.emplace_back(std::move(entry));
  }

  auto linksArr = json::array();
  for (const auto& weak : m_editor->getLinks()) {
    auto link = weak.lock();
    if (!link) {
      continue;
    }
    ImFlow::Pin* outPin = link->left();
    ImFlow::Pin* inPin = link->right();
    if (!outPin || !inPin || !outPin->getParent() || !inPin->getParent()) {
      continue;
    }
    linksArr.emplace_back(json::object(
        "src",
        json::object("node",
                     static_cast<long long>(outPin->getParent()->getUID()),
                     "pin", outPin->getName()),
        "dst",
        json::object("node",
                     static_cast<long long>(inPin->getParent()->getUID()),
                     "pin", inPin->getName())));
  }

  json root = json::object("version", 1, "nodes", std::move(nodesArr), "links",
                           std::move(linksArr));

  std::ofstream out(m_savePath, std::ios::binary | std::ios::trunc);
  if (!out) {
    m_status = "Save failed: cannot open " + m_savePath;
    return;
  }
  out << root.to_string_pretty();
  if (!out) {
    m_status = "Save failed: write error";
    return;
  }
  m_status = "Saved to " + m_savePath;
}

void GraphEditor::LoadFromDisk() {
  using wpi::util::json;

  std::ifstream in(m_savePath, std::ios::binary);
  if (!in) {
    m_status = "Load failed: cannot open " + m_savePath;
    return;
  }
  std::ostringstream ss;
  ss << in.rdbuf();

  auto parsed = json::parse(ss.str());
  if (!parsed) {
    m_status = std::string{"Load failed: "} + parsed.error();
    return;
  }

  const json& root = *parsed;
  if (!root.is_object() || !root.contains("nodes") || !root.contains("links")) {
    m_status = "Load failed: missing nodes/links";
    return;
  }

  // Start with a fresh editor so the load path also doubles as a hard reset.
  m_editor = std::make_unique<ImFlow::ImNodeFlow>("FilterDesignerGraph");

  // Map saved IDs to the freshly-assigned UIDs of the recreated nodes so the
  // links section can resolve pin endpoints.
  std::unordered_map<long long, ImFlow::NodeUID> idMap;

  for (const json& entry : root.at("nodes").get_array()) {
    if (!entry.is_object() || !entry.contains("id") ||
        !entry.contains("type") || !entry.contains("pos")) {
      continue;
    }
    long long savedId = entry.at("id").get_int();
    const std::string& type = entry.at("type").get_string();
    const auto& pos = entry.at("pos").get_array();
    if (pos.size() != 2) {
      continue;
    }
    ImVec2 p{static_cast<float>(pos[0].get_number()),
             static_cast<float>(pos[1].get_number())};

    std::shared_ptr<ImFlow::BaseNode> node;
    if (type == kDebugIntSourceTag) {
      auto src = m_editor->addNode<DebugIntSourceNode>(p);
      if (entry.contains("value")) {
        src->setValue(static_cast<int>(entry.at("value").get_int()));
      }
      node = src;
    } else if (type == kDebugIntDisplayTag) {
      node = m_editor->addNode<DebugIntDisplayNode>(p);
    } else {
      continue;
    }
    idMap[savedId] = node->getUID();
  }

  auto& liveNodes = m_editor->getNodes();
  for (const json& link : root.at("links").get_array()) {
    if (!link.is_object() || !link.contains("src") || !link.contains("dst")) {
      continue;
    }
    const json& src = link.at("src");
    const json& dst = link.at("dst");
    auto srcIt = idMap.find(src.at("node").get_int());
    auto dstIt = idMap.find(dst.at("node").get_int());
    if (srcIt == idMap.end() || dstIt == idMap.end()) {
      continue;
    }
    auto srcNodeIt = liveNodes.find(srcIt->second);
    auto dstNodeIt = liveNodes.find(dstIt->second);
    if (srcNodeIt == liveNodes.end() || dstNodeIt == liveNodes.end()) {
      continue;
    }
    ImFlow::Pin* outPin = srcNodeIt->second->outPin(
        src.at("pin").get_string().c_str());
    ImFlow::Pin* inPin = dstNodeIt->second->inPin(
        dst.at("pin").get_string().c_str());
    if (outPin && inPin) {
      inPin->createLink(outPin);
    }
  }

  m_status = "Loaded from " + m_savePath;
}

void GraphEditor::Display() {
  if (ImGui::Button("Save")) {
    SaveToDisk();
  }
  ImGui::SameLine();
  if (ImGui::Button("Load")) {
    LoadFromDisk();
  }
  ImGui::SameLine();
  ImGui::TextUnformatted(m_status.empty() ? m_savePath.c_str()
                                          : m_status.c_str());
  ImGui::Separator();

  m_editor->update();
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
