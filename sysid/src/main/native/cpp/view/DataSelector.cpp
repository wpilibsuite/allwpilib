// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/DataSelector.h"

#include <glass/support/DataLogReaderThread.h>
#include <fmt/format.h>
#include <imgui.h>
#include <wpi/Logger.h>
#include <wpi/StringExtras.h>

using namespace sysid;

static bool EmitEntryTarget(const char* name, bool isString,
                            const glass::DataLogReaderEntry** entry) {
  if (*entry) {
    auto text =
        fmt::format("{}: {} ({})", name, (*entry)->name, (*entry)->type);
    ImGui::TextUnformatted(text.c_str());
  } else {
    ImGui::Text("%s: <none (DROP HERE)> (%s)", name,
                isString ? "string" : "number");
  }
  bool rv = false;
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
            isString ? "DataLogEntryString" : "DataLogEntry")) {
      assert(payload->DataSize == sizeof(const glass::DataLogReaderEntry*));
      *entry = *static_cast<const glass::DataLogReaderEntry**>(payload->Data);
      rv = true;
    }
    ImGui::EndDragDropTarget();
  }
  return rv;
}

void DataSelector::Display() {
  if (EmitEntryTarget("Test State", true, &m_testStateEntry)) {
    LoadTests(*m_testStateEntry);
  }

  if (!m_testStateEntry) {
    return;
  }

  using namespace std::chrono_literals;
  if (m_testsFuture.valid() &&
      m_testsFuture.wait_for(0s) == std::future_status::ready) {
    m_tests = m_testsFuture.get();
  }

  if (m_tests.empty()) {
    if (m_testsFuture.valid()) {
      ImGui::TextUnformatted("Reading tests...");
    } else {
      ImGui::TextUnformatted("No tests found");
    }
    return;
  }

  // Test filtering
  if (ImGui::BeginCombo("Test", m_selectedTest.c_str())) {
    for (auto&& test : m_tests) {
      if (ImGui::Selectable(test.first.c_str(), test.first == m_selectedTest)) {
        m_selectedTest = test.first;
      }
    }
    ImGui::EndCombo();
  }

  // DND targets
  EmitEntryTarget("Velocity", false, &m_velocityEntry);
  EmitEntryTarget("Position", false, &m_positionEntry);
  EmitEntryTarget("Voltage", false, &m_voltageEntry);

  if (!m_selectedTest.empty() && m_velocityEntry && m_positionEntry &&
      m_voltageEntry) {
    if (ImGui::Button("Load")) {
    }
  }
}

void DataSelector::Reset() {
  m_tests = {};
  m_selectedTest.clear();
  m_testStateEntry = nullptr;
  m_velocityEntry = nullptr;
  m_positionEntry = nullptr;
  m_voltageEntry = nullptr;
}

void DataSelector::LoadTests(const glass::DataLogReaderEntry& testStateEntry) {
  m_testsFuture = std::async(std::launch::async, [&] {
    Tests rv;
    for (auto&& range : testStateEntry.ranges) {
      Runs* curRuns = nullptr;
      wpi::log::DataLogReader::iterator lastStart = range.begin();
      for (auto it = range.begin(), end = range.end(); it != end; ++it) {
        std::string_view testState;
        if (it->GetEntry() != testStateEntry.entry ||
            !it->GetString(&testState)) {
          continue;
        }

        if (testState == "none") {
          continue;
        }

        auto [testName, direction] = wpi::rsplit(testState, '-');

        // track runs as iterator ranges of the same test
        if (curRuns) {
          curRuns->emplace_back(lastStart, it);
        }
        lastStart = it;
        auto testIt = rv.find(testName);
        if (testIt == rv.end()) {
          testIt = rv.emplace(std::string{testName}, State{}).first;
        }
        auto stateIt = testIt->second.find(testState);
        if (stateIt == testIt->second.end()) {
          stateIt =
              testIt->second.emplace(std::string{testState}, Runs{}).first;
        }
        curRuns = &stateIt->second;
      }
    }
    return rv;
  });
}
