// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/NT4SourceView.hpp"

#ifndef RUNNING_FILTERDESIGNER_TESTS

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_c.h"

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

NT4SourceView::NT4SourceView()
    : m_inst{wpi::nt::NetworkTableInstance::Create()},
      m_topicPoller{m_inst},
      m_source{[this]() {
        std::vector<NT4Source::Sample> out;
        if (!m_sub) {
          return out;
        }
        auto values = m_sub->ReadQueue();
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
      }} {}

NT4SourceView::~NT4SourceView() {
  Unsubscribe();
  // Tear down NT-owned objects before Destroy(m_inst) so their destructors
  // don't unsubscribe against a freed instance.
  if (m_topicListener != 0) {
    m_topicPoller.RemoveListener(m_topicListener);
    m_topicListener = 0;
  }
  m_topicSub = wpi::nt::MultiSubscriber{};
  m_topicPoller = wpi::nt::NetworkTableListenerPoller{};
  // Sync StopClient here on purpose: shutdown has to wait for the worker
  // before Destroy(m_inst) anyway, and the ImGui loop has stopped rendering
  // by now, so async + join would just add a thread spawn for no UI benefit.
  if (m_clientStarted) {
    m_inst.StopClient();
    m_clientStarted = false;
  }
  if (m_stopThread.joinable()) {
    m_stopThread.join();
  }
  wpi::nt::NetworkTableInstance::Destroy(m_inst);
}

void NT4SourceView::StartClient() {
  if (m_clientStarted || m_stopInProgress) {
    return;
  }
  // The worker has finished by the time we get here (Display gates Connect
  // on m_stopInProgress); join is just to release the std::thread object.
  if (m_stopThread.joinable()) {
    m_stopThread.join();
  }
  m_inst.StartClient("filterdesigner");
  if (m_serverMode == ServerMode::Team) {
    m_inst.SetServerTeam(static_cast<unsigned int>(m_team),
                         static_cast<unsigned int>(m_port));
  } else {
    m_inst.SetServer(m_host, static_cast<unsigned int>(m_port));
  }
  m_clientStarted = true;

  // Discovery sub. Two prefixes mirror Glass/OutlineViewer: "" catches
  // regular topics, "$" picks up meta topics that the empty prefix
  // intentionally skips (see ServerSubscriber::Matches in ntcore).
  // topicsOnly suppresses value traffic on this sub.
  m_topicSub =
      wpi::nt::MultiSubscriber{m_inst, {{"", "$"}}, {.topicsOnly = true}};
  m_topicListener =
      m_topicPoller.AddListener(m_topicSub, wpi::nt::EventFlags::TOPIC);
}

void NT4SourceView::StopClient() {
  Unsubscribe();
  if (m_topicListener != 0) {
    m_topicPoller.RemoveListener(m_topicListener);
    m_topicListener = 0;
  }
  m_topicSub = wpi::nt::MultiSubscriber{};
  if (m_clientStarted) {
    // Run on a worker thread — StopClient blocks until the connection thread
    // unwinds, which can stall the UI for seconds while a connect attempt is
    // in flight. The atomic flag lets Display gate Connect without joining.
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
  m_source.Clear();
  m_topics.clear();
  m_selectedTopic.clear();
}

void NT4SourceView::Subscribe(const std::string& topicName) {
  Unsubscribe();
  if (topicName.empty()) {
    return;
  }
  auto topic = m_inst.GetTopic(topicName);
  m_sub = std::make_unique<wpi::nt::GenericSubscriber>(
      topic.GenericSubscribe({.pollStorage = 4096, .keepDuplicates = true}));
  m_selectedTopic = topicName;
  m_source.SetName(topicName);
  m_source.Clear();
}

void NT4SourceView::Unsubscribe() {
  m_sub.reset();
  m_source.Clear();
}

void NT4SourceView::RefreshTopics() {
  m_topics.clear();
  if (!m_clientStarted) {
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
}

const Signal* NT4SourceView::SelectedSignal() const {
  // Match WpiLogLoaderView's contract: a non-null pointer means there's data
  // to look at. Otherwise downstream views need to special-case empty
  // vectors on every consumer.
  if (!m_sub || m_source.SampleCount() == 0) {
    return nullptr;
  }
  return m_source.GetSignal();
}

void NT4SourceView::Display() {
  // Always drive the source so that a freshly selected topic starts filling
  // on the next frame rather than only after the user toggles freeze.
  m_source.Update();

  // Drain discovery events. Any TOPIC event means the announce list has
  // changed (publish, unpublish, properties changed), so re-query.
  if (m_topicListener != 0 && !m_topicPoller.ReadQueue().empty()) {
    RefreshTopics();
  }

  ImGui::BeginDisabled(m_clientStarted);
  int modeIdx = static_cast<int>(m_serverMode);
  if (ImGui::RadioButton("Host", modeIdx == 1)) {
    m_serverMode = ServerMode::Host;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Team", modeIdx == 0)) {
    m_serverMode = ServerMode::Team;
  }
  if (m_serverMode == ServerMode::Host) {
    ImGui::InputText("Server", &m_host);
  } else {
    // Label disambiguates from the "Team" radio button above so ImGui can
    // distinguish their IDs.
    ImGui::InputInt("Team Number", &m_team);
  }
  ImGui::InputInt("Port", &m_port);
  if (m_team < 0) {
    m_team = 0;
  }
  if (m_port < 1) {
    m_port = NT_DEFAULT_PORT;
  }
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
    if (ImGui::Button("Refresh topics")) {
      RefreshTopics();
    }
    ImGui::SameLine();
    if (m_inst.IsConnected()) {
      ImGui::TextColored({0.3f, 0.9f, 0.3f, 1.0f}, "connected");
    } else {
      ImGui::TextColored({0.9f, 0.6f, 0.3f, 1.0f}, "connecting...");
    }
  }

  ImGui::Separator();

  if (!m_clientStarted) {
    ImGui::TextDisabled("Not connected.");
    return;
  }

  // Topic picker. BeginCombo lets us render the optional type suffix.
  const char* preview =
      m_selectedTopic.empty() ? "<pick a topic>" : m_selectedTopic.c_str();
  if (ImGui::BeginCombo("Topic", preview)) {
    for (const auto& t : m_topics) {
      bool selected = t.name == m_selectedTopic;
      std::string label = t.name + "  [" + t.type + "]";
      if (ImGui::Selectable(label.c_str(), selected)) {
        Subscribe(t.name);
      }
      if (selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  if (m_topics.empty()) {
    ImGui::TextDisabled(
        "No numeric topics yet — try Refresh after the server publishes.");
  }

  float bufferSeconds = static_cast<float>(m_source.BufferSeconds());
  if (ImGui::SliderFloat("Window (s)", &bufferSeconds, 1.0f, 120.0f, "%.1f")) {
    m_source.SetBufferSeconds(bufferSeconds);
  }

  bool frozen = m_source.Frozen();
  if (ImGui::Checkbox("Freeze", &frozen)) {
    m_source.SetFrozen(frozen);
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear")) {
    m_source.Clear();
  }

  ImGui::Text("%zu samples", m_source.SampleCount());
  if (m_source.GetSignal()->sampleRate > 0.0) {
    ImGui::SameLine();
    ImGui::Text("| %.1f Hz inferred", m_source.GetSignal()->sampleRate);
  }
}

}  // namespace wpi::filterdesigner

#endif  // RUNNING_FILTERDESIGNER_TESTS
