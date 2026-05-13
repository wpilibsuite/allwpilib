// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NT4SourceNode.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_c.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <imgui.h>
#include <imgui_stdlib.h>
#endif

namespace wpi::filterdesigner {

namespace {

constexpr unsigned int kNumericTypes =
    NT_DOUBLE | NT_FLOAT | NT_INTEGER | NT_BOOLEAN;

std::string TypeLabel(NT_Type type) {
  switch (type) {
    case NT_DOUBLE:
      return "double";
    case NT_FLOAT:
      return "float";
    case NT_INTEGER:
      return "int";
    case NT_BOOLEAN:
      return "boolean";
    default:
      return "?";
  }
}

}  // namespace

NT4SourceNode::NT4SourceNode()
    : m_logic(std::make_unique<NT4SourceNodeLogic>()) {
  setTitle("NT4 Source");
  setStyle(ImFlow::NodeStyle::green());

  // The Logic owns the ring buffer; we hand it a drain closure that reads
  // from this node's NT subscriber (or yields nothing when no subscription
  // is active).
  auto* node = this;
  m_logic->SetDrain([node]() -> std::vector<NT4Source::Sample> {
    std::vector<NT4Source::Sample> out;
    if (!node->m_sub) {
      return out;
    }
    auto values = node->m_sub->ReadQueue();
    out.reserve(values.size());
    for (const auto& v : values) {
      double d = 0.0;
      if (v.IsDouble()) {
        d = v.GetDouble();
      } else if (v.IsFloat()) {
        d = v.GetFloat();
      } else if (v.IsInteger()) {
        d = static_cast<double>(v.GetInteger());
      } else if (v.IsBoolean()) {
        d = v.GetBoolean() ? 1.0 : 0.0;
      } else {
        continue;
      }
      out.push_back({v.time(), d});
    }
    return out;
  });

  auto* logic = m_logic.get();
  addOUT<const wpi::filterdesigner::Signal*>("out")->behaviour(
      [logic] { return logic->Signal(); });
}

NT4SourceNode::~NT4SourceNode() {
  // Drain any in-flight async StopClient worker before touching listener /
  // subscriber handles. The worker holds a copy of m_inst and calls
  // StopClient on the same ntcore instance these handles are bound to;
  // tearing the listener queue down underneath a running StopClient would
  // race on ntcore-internal state. Cheap if no worker is in flight.
  if (m_stopThread.joinable()) {
    m_stopThread.join();
  }
  Unsubscribe();
  if (m_topicListener != 0) {
    m_topicPoller.RemoveListener(m_topicListener);
    m_topicListener = 0;
  }
  m_topicSub = wpi::nt::MultiSubscriber{};
  m_topicPoller = wpi::nt::NetworkTableListenerPoller{};
  // Sync StopClient + Destroy on shutdown — lazy-created: only Destroy when
  // we actually Created.
  if (m_clientStarted) {
    m_inst.StopClient();
    m_clientStarted = false;
  }
  if (m_inst) {
    wpi::nt::NetworkTableInstance::Destroy(m_inst);
  }
}

void NT4SourceNode::SerializeParams(wpi::util::json& obj) const {
  obj["serverMode"] = static_cast<int>(m_logic->serverMode);
  obj["host"] = m_logic->host;
  obj["team"] = m_logic->team;
  obj["port"] = m_logic->port;
  obj["topic"] = m_logic->TopicName();
  obj["bufferSeconds"] = m_logic->BufferSeconds();
  obj["frozen"] = m_logic->Frozen();
}

void NT4SourceNode::DeserializeParams(const wpi::util::json& obj) {
  if (const auto* p = obj.lookup("serverMode"); p && p->is_number()) {
    int v = static_cast<int>(p->get_number());
    m_logic->serverMode =
        v == static_cast<int>(NT4SourceNodeLogic::ServerMode::Team)
            ? NT4SourceNodeLogic::ServerMode::Team
            : NT4SourceNodeLogic::ServerMode::Host;
  }
  if (const auto* p = obj.lookup("host"); p && p->is_string()) {
    m_logic->host = p->get_string();
  }
  if (const auto* p = obj.lookup("team"); p && p->is_number()) {
    m_logic->team =
        NT4SourceNodeLogic::SanitizeTeam(static_cast<int>(p->get_number()));
  }
  if (const auto* p = obj.lookup("port"); p && p->is_number()) {
    m_logic->port =
        NT4SourceNodeLogic::SanitizePort(static_cast<int>(p->get_number()));
  }
  if (const auto* p = obj.lookup("topic"); p && p->is_string()) {
    m_logic->SetTopicName(p->get_string());
  }
  if (const auto* p = obj.lookup("bufferSeconds"); p && p->is_number()) {
    m_logic->SetBufferSeconds(p->get_number());
  }
  if (const auto* p = obj.lookup("frozen"); p && p->is_bool()) {
    m_logic->SetFrozen(p->get_bool());
  }
  // Connection isn't auto-restored — the user has to click Connect after a
  // load. Same pattern WpiLogSource uses for "file may have moved", and
  // avoids surprise network traffic on every graph open.
}

void NT4SourceNode::Register(NodeRegistry& registry) {
  NodeRegistry::Entry entry;
  entry.tag = "NT4Source";
  entry.menuLabel = "NT4 Source";
  entry.menuCategory = "Sources";
  entry.outputTypes.emplace_back(typeid(const wpi::filterdesigner::Signal*));
  entry.factory = [](Graph& g, const ImVec2& pos) {
    return g.AddNode<NT4SourceNode>(pos);
  };
  registry.Register(std::move(entry));
}

void NT4SourceNode::StartClient() {
  if (m_clientStarted || m_stopInProgress) {
    return;
  }
  if (m_stopThread.joinable()) {
    m_stopThread.join();
  }
  if (!m_inst) {
    m_inst = wpi::nt::NetworkTableInstance::Create();
    // Listener poller can't be re-bound to a different instance — rebuild it
    // alongside m_inst so they share lifetime.
    m_topicPoller = wpi::nt::NetworkTableListenerPoller{m_inst};
  }
  m_inst.StartClient("filterdesigner");
  if (m_logic->serverMode == NT4SourceNodeLogic::ServerMode::Team) {
    m_inst.SetServerTeam(static_cast<unsigned int>(m_logic->team),
                         static_cast<unsigned int>(m_logic->port));
  } else {
    m_inst.SetServer(m_logic->host, static_cast<unsigned int>(m_logic->port));
  }
  m_clientStarted = true;

  // Discovery subscription. NT4 servers only announce topics matched by some
  // subscriber, so the empty prefix opens the floodgates; "$" picks up meta
  // topics, topicsOnly suppresses value traffic on this sub.
  m_topicSub =
      wpi::nt::MultiSubscriber{m_inst, {{"", "$"}}, {.topicsOnly = true}};
  m_topicListener =
      m_topicPoller.AddListener(m_topicSub, wpi::nt::EventFlags::TOPIC);

  // After a save+load the topic name survives in the logic but the live
  // GenericSubscriber doesn't — re-arm it on Connect so the user doesn't
  // have to re-pick the topic from the combo. NT4 will deliver values
  // once the server announces the topic, even if we subscribed before
  // discovery completed.
  if (!m_logic->TopicName().empty()) {
    Subscribe(m_logic->TopicName());
  }
}

void NT4SourceNode::StopClient() {
  Unsubscribe();
  if (m_topicListener != 0) {
    m_topicPoller.RemoveListener(m_topicListener);
    m_topicListener = 0;
  }
  m_topicSub = wpi::nt::MultiSubscriber{};
  if (m_clientStarted) {
    if (m_stopThread.joinable()) {
      m_stopThread.join();
    }
    auto inst = m_inst;
    m_stopInProgress = true;
    m_stopThread = std::thread([inst, this]() mutable {
      inst.StopClient();
      m_stopInProgress = false;
    });
    m_clientStarted = false;
  }
  m_logic->Clear();
  m_topics.clear();
}

void NT4SourceNode::Subscribe(std::string_view topicName) {
  Unsubscribe();
  if (topicName.empty()) {
    return;
  }
  auto topic = m_inst.GetTopic(std::string{topicName});
  // sendAll preserves intermediate samples; otherwise the server forwards
  // only the latest sample per periodic tick and the FFT would alias.
  m_sub = std::make_unique<wpi::nt::GenericSubscriber>(
      topic.GenericSubscribe({.pollStorage = 4096,
                              .periodic = 0.001,
                              .sendAll = true,
                              .keepDuplicates = true}));
  m_logic->SetTopicName(topicName);
  m_logic->Clear();
}

void NT4SourceNode::Unsubscribe() {
  m_sub.reset();
  m_logic->Clear();
}

void NT4SourceNode::RefreshTopics() {
  m_topics.clear();
  if (!m_clientStarted) {
    RebuildTopicTree();
    return;
  }
  auto info = m_inst.GetTopicInfo("", kNumericTypes);
  m_topics.reserve(info.size());
  for (const auto& t : info) {
    m_topics.push_back({t.name, TypeLabel(t.type)});
  }
  std::sort(
      m_topics.begin(), m_topics.end(),
      [](const TopicEntry& a, const TopicEntry& b) { return a.name < b.name; });
  RebuildTopicTree();
}

void NT4SourceNode::RebuildTopicTree() {
  // Path-split each topic on '/' (matches Glass / OutlineViewer conventions).
  // Leading '/' produces an empty first segment which we skip. Each path
  // walks down (or creates) branches in the tree; the final segment is the
  // leaf, which carries the full topic path and type for selection.
  m_topicTree = TopicTreeNode{};
  for (const auto& t : m_topics) {
    TopicTreeNode* cursor = &m_topicTree;
    std::size_t start = 0;
    while (start <= t.name.size()) {
      std::size_t slash = t.name.find('/', start);
      std::size_t end = slash == std::string::npos ? t.name.size() : slash;
      std::string segment = t.name.substr(start, end - start);
      bool isLast = slash == std::string::npos;
      if (segment.empty()) {
        if (isLast) {
          break;
        }
        start = end + 1;
        continue;
      }
      auto it = std::find_if(
          cursor->children.begin(), cursor->children.end(),
          [&](const TopicTreeNode& n) { return n.name == segment; });
      if (it == cursor->children.end()) {
        cursor->children.push_back(TopicTreeNode{segment, "", "", {}});
        it = cursor->children.end() - 1;
      }
      if (isLast) {
        it->fullPath = t.name;
        it->type = t.type;
        break;
      }
      cursor = &*it;
      start = end + 1;
    }
  }
}

bool NT4SourceNode::TopicTreeNodeMatchesSearch(
    const TopicTreeNode& node) const {
  if (m_topicSearch.empty()) {
    return true;
  }
  // Case-insensitive substring match. The search query is short and the
  // tree shallow; the per-frame cost is negligible compared to any of the
  // node's other per-frame work.
  auto containsCI = [](std::string_view haystack,
                       std::string_view needle) -> bool {
    if (needle.empty()) {
      return true;
    }
    if (needle.size() > haystack.size()) {
      return false;
    }
    for (std::size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
      bool match = true;
      for (std::size_t j = 0; j < needle.size(); ++j) {
        char h = haystack[i + j];
        char n = needle[j];
        if (h >= 'A' && h <= 'Z') {
          h = static_cast<char>(h - 'A' + 'a');
        }
        if (n >= 'A' && n <= 'Z') {
          n = static_cast<char>(n - 'A' + 'a');
        }
        if (h != n) {
          match = false;
          break;
        }
      }
      if (match) {
        return true;
      }
    }
    return false;
  };
  if (!node.fullPath.empty() && containsCI(node.fullPath, m_topicSearch)) {
    return true;
  }
  for (const auto& c : node.children) {
    if (TopicTreeNodeMatchesSearch(c)) {
      return true;
    }
  }
  return false;
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void NT4SourceNode::draw() {
  // Always drive the source so a freshly-selected topic starts filling on
  // the next frame instead of waiting for the user to toggle freeze.
  m_logic->Update();

  // Drain discovery events — any TOPIC event invalidates the announce list.
  if (m_topicListener != 0 && !m_topicPoller.ReadQueue().empty()) {
    RefreshTopics();
  }

  const float kItemWidth = 200.0f;

  ImGui::BeginDisabled(m_clientStarted);
  int modeIdx = static_cast<int>(m_logic->serverMode);
  if (ImGui::RadioButton("Host", modeIdx == 1)) {
    m_logic->serverMode = NT4SourceNodeLogic::ServerMode::Host;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Team", modeIdx == 0)) {
    m_logic->serverMode = NT4SourceNodeLogic::ServerMode::Team;
  }
  ImGui::SetNextItemWidth(kItemWidth);
  if (m_logic->serverMode == NT4SourceNodeLogic::ServerMode::Host) {
    ImGui::InputText("Server", &m_logic->host);
  } else {
    ImGui::InputInt("Team", &m_logic->team);
  }
  ImGui::SetNextItemWidth(kItemWidth);
  ImGui::InputInt("Port", &m_logic->port);
  m_logic->team = NT4SourceNodeLogic::SanitizeTeam(m_logic->team);
  m_logic->port = NT4SourceNodeLogic::SanitizePort(m_logic->port);
  ImGui::EndDisabled();

  if (!m_clientStarted) {
    ImGui::BeginDisabled(m_stopInProgress);
    if (ImGui::Button("Connect")) {
      StartClient();
      RefreshTopics();
    }
    ImGui::EndDisabled();
    if (m_stopInProgress) {
      ImGui::SameLine();
      ImGui::TextDisabled("disconnecting...");
    }
  } else {
    if (ImGui::Button("Disconnect")) {
      StopClient();
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
      RefreshTopics();
    }
    ImGui::SameLine();
    if (m_inst.IsConnected()) {
      ImGui::TextColored({0.3f, 0.9f, 0.3f, 1.0f}, "connected");
    } else {
      ImGui::TextColored({0.9f, 0.6f, 0.3f, 1.0f}, "connecting...");
    }
  }

  if (!m_clientStarted) {
    ImGui::TextDisabled("Not connected.");
    return;
  }

  const std::string& selected = m_logic->TopicName();
  const char* preview = selected.empty() ? "<pick a topic>" : selected.c_str();
  ImGui::SetNextItemWidth(kItemWidth);
  if (ImGui::BeginCombo("Topic", preview, ImGuiComboFlags_HeightLarge)) {
    // Live search filters the tree; with a non-empty query, surviving
    // branches default to open so matches surface without further clicks.
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputTextWithHint("##topicSearch", "Search...", &m_topicSearch);
    const bool forceOpen = !m_topicSearch.empty();
    for (const auto& child : m_topicTree.children) {
      RenderTopicTreeNode(child, forceOpen);
    }
    ImGui::EndCombo();
  }
  if (m_topics.empty()) {
    ImGui::TextDisabled("No numeric topics yet — try Refresh.");
  }

  ImGui::SetNextItemWidth(kItemWidth);
  float bufferSeconds = static_cast<float>(m_logic->BufferSeconds());
  if (ImGui::SliderFloat("Window (s)", &bufferSeconds, 1.0f, 120.0f, "%.1f")) {
    m_logic->SetBufferSeconds(bufferSeconds);
  }

  bool frozen = m_logic->Frozen();
  if (ImGui::Checkbox("Freeze", &frozen)) {
    m_logic->SetFrozen(frozen);
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear")) {
    m_logic->Clear();
  }

  ImGui::Text("%zu samples", m_logic->SampleCount());
  if (m_logic->Source().GetSignal()->sampleRate > 0.0) {
    ImGui::SameLine();
    ImGui::Text("| %.1f Hz", m_logic->Source().GetSignal()->sampleRate);
  }
}

void NT4SourceNode::RenderTopicTreeNode(const TopicTreeNode& node,
                                        bool forceOpen) {
  if (!TopicTreeNodeMatchesSearch(node)) {
    return;
  }
  const bool isLeaf = !node.fullPath.empty();
  if (isLeaf) {
    bool isSelected = node.fullPath == m_logic->TopicName();
    std::string label = node.name + "  [" + node.type + "]";
    // Leaves indent to align with branch-content level. Tree node-leaf
    // flags would draw a disclosure arrow on a non-expandable row, which
    // looks off — render as a regular Selectable instead.
    ImGui::Indent();
    if (ImGui::Selectable(label.c_str(), isSelected)) {
      Subscribe(node.fullPath);
    }
    ImGui::Unindent();
    if (isSelected) {
      ImGui::SetItemDefaultFocus();
    }
    return;
  }
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
  if (forceOpen) {
    flags |= ImGuiTreeNodeFlags_DefaultOpen;
  }
  if (ImGui::TreeNodeEx(node.name.c_str(), flags)) {
    for (const auto& c : node.children) {
      RenderTopicTreeNode(c, forceOpen);
    }
    ImGui::TreePop();
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void NT4SourceNode::draw() {}

void NT4SourceNode::RenderTopicTreeNode(const TopicTreeNode& /*node*/,
                                        bool /*forceOpen*/) {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
