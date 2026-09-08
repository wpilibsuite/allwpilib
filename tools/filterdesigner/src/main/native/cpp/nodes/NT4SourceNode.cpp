// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/NT4SourceNode.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>

#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/JsonInt.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/NameTree.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableType.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_c.h"

#ifndef RUNNING_FILTERDESIGNER_TESTS
#include <imgui.h>
#include <imgui_stdlib.h>

#include "wpi/filterdesigner/nodes/NameTreePicker.hpp"
#include "wpi/filterdesigner/nodes/SamplingReadout.hpp"
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

  // The logic owns the ring buffer and drains it through this closure.
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
  // Before touching listener or subscriber handles: an in-flight StopClient
  // worker is calling into the same ntcore instance they are bound to.
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
  if (auto v = ReadIntField(obj, "serverMode")) {
    m_logic->serverMode =
        *v == static_cast<int>(NT4SourceNodeLogic::ServerMode::Team)
            ? NT4SourceNodeLogic::ServerMode::Team
            : NT4SourceNodeLogic::ServerMode::Host;
  }
  if (const auto* p = obj.lookup("host"); p && p->is_string()) {
    m_logic->host = p->get_string();
  }
  if (auto v = ReadIntField(obj, "team")) {
    m_logic->team = NT4SourceNodeLogic::SanitizeTeam(*v);
  }
  if (auto v = ReadIntField(obj, "port")) {
    m_logic->port = NT4SourceNodeLogic::SanitizePort(*v);
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
  // The connection isn't auto-restored: opening a graph shouldn't put traffic
  // on the network before the user asks for it.
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
    // A poller can't be re-bound to another instance.
    m_topicPoller = wpi::nt::NetworkTableListenerPoller{m_inst};
  }
  m_inst.StartClient("filterdesigner");
  if (m_logic->serverMode == NT4SourceNodeLogic::ServerMode::Team) {
    m_inst.SetServerTeam(std::to_string(m_logic->team),
                         static_cast<unsigned int>(m_logic->port));
  } else {
    m_inst.SetServer(m_logic->host, static_cast<unsigned int>(m_logic->port));
  }
  m_clientStarted = true;

  // NT4 servers only announce topics some subscriber matches, so the empty
  // prefix opens the floodgates; "$" picks up meta topics.
  m_topicSub =
      wpi::nt::MultiSubscriber{m_inst, {{"", "$"}}, {.topicsOnly = true}};
  m_topicListener =
      m_topicPoller.AddListener(m_topicSub, wpi::nt::EventFlags::TOPIC);

  // A loaded graph keeps the topic name but not the subscriber. Subscribing
  // before discovery completes is fine — values arrive once it is announced,
  // and RefreshTopics settles the discrete flag then.
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
  // Boolean topics are held across grid slots rather than interpolated. A
  // topic the server hasn't announced yet reads as unassigned here; see
  // RefreshTopics for the correction.
  m_logic->SetDiscrete(topic.GetType() == wpi::nt::NetworkTableType::BOOLEAN);
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
    // Subscribe read the type before the announcement when a loaded graph
    // reconnected, and recorded a boolean as continuous. Discovery is where
    // the real type arrives, and every TOPIC event lands here.
    if (t.name == m_logic->TopicName()) {
      m_logic->SetDiscrete(t.type == NT_BOOLEAN);
    }
  }
  RebuildTopicTree();
}

void NT4SourceNode::RebuildTopicTree() {
  // NT names never carry a ':' prefix, so this is BuildNameTree's plain '/'
  // split, and every topic is selectable: discovery only asks for numeric
  // types.
  std::vector<NameTreeItem> items;
  items.reserve(m_topics.size());
  for (const auto& t : m_topics) {
    items.push_back({t.name, t.type, true});
  }
  m_topicTree = BuildNameTree(items);
}

#ifndef RUNNING_FILTERDESIGNER_TESTS

void NT4SourceNode::draw() {
  // Drain discovery events — any TOPIC event invalidates the announce list.
  // Before the sample drain: the announcement carrying a topic's real type
  // lands in the same frame as its first values, and the grid built from
  // those should already know whether to hold or interpolate.
  if (m_topicListener != 0 && !m_topicPoller.ReadQueue().empty()) {
    RefreshTopics();
  }

  // Unconditionally, so a freshly-picked topic starts filling next frame.
  m_logic->Update();

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
    DrawNameTreePicker(m_topicTree, m_topicSearch, m_logic->TopicName(),
                       [this](std::string_view topic) { Subscribe(topic); });
    ImGui::EndCombo();
  }
  if (m_topics.empty()) {
    ImGui::TextDisabled("No numeric topics yet — try Refresh.");
  }

  ImGui::SetNextItemWidth(kItemWidth);
  float bufferSeconds = static_cast<float>(m_logic->BufferSeconds());
  if (ImGui::SliderFloat("Window (s)", &bufferSeconds, 1.0f,
                         static_cast<float>(NT4Source::kMaxBufferSeconds),
                         "%.1f")) {
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
  // Gated on having samples rather than on a known rate: "sample rate
  // unknown" is exactly the case worth telling the user about.
  if (m_logic->SampleCount() > 0) {
    DrawSamplingReadout(*m_logic->Source().GetSignal());
  }
}

#else  // RUNNING_FILTERDESIGNER_TESTS

void NT4SourceNode::draw() {}

#endif  // RUNNING_FILTERDESIGNER_TESTS

}  // namespace wpi::filterdesigner
