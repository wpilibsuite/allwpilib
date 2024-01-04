// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/DataSelector.h"

#include <glass/support/DataLogReaderThread.h>
#include <fmt/format.h>
#include <imgui.h>
#include <wpi/Logger.h>
#include <wpi/StringExtras.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/Storage.h"
#include "wpi/DataLogReader.h"

using namespace sysid;

static constexpr const char* kAnalysisTypes[] = { "Elevator",
                                                 "Arm", "Simple"};

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
  using namespace std::chrono_literals;

  // building test data is modal (due to async access)
  if (m_testdataFuture.valid()) {
    if (m_testdataFuture.wait_for(0s) == std::future_status::ready) {
      TestData data = m_testdataFuture.get();
      for (auto&& motordata : data.motorData) {
        m_testdataStats.emplace_back(
            fmt::format("Test State: {}", motordata.first()));
        int i = 0;
        for (auto&& run : motordata.second.runs) {
          m_testdataStats.emplace_back(fmt::format(
              "  Run {} samples: {} Volt {} Pos {} Vel", ++i,
              run.voltage.size(), run.position.size(), run.velocity.size()));
        }
      }
      if (testdata) {
        testdata(std::move(data));
      }
    }
    ImGui::Text("Loading data...");
    return;
  }

  if (!m_testdataStats.empty()) {
    for (auto&& line : m_testdataStats) {
      ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::Button("Ok")) {
      m_testdataStats.clear();
    }
    return;
  }

  if (EmitEntryTarget("Test State", true, &m_testStateEntry)) {
    m_testsFuture =
        std::async(std::launch::async, [testStateEntry = m_testStateEntry] {
          return LoadTests(*testStateEntry);
        });
  }

  if (!m_testStateEntry) {
    return;
  }

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

#if 0
  // Test filtering
  if (ImGui::BeginCombo("Test", m_selectedTest.c_str())) {
    for (auto&& test : m_tests) {
      if (ImGui::Selectable(test.first.c_str(), test.first == m_selectedTest)) {
        m_selectedTest = test.first;
      }
    }
    ImGui::EndCombo();
  }
#endif

  ImGui::Combo("Analysis Type", &m_selectedAnalysis, kAnalysisTypes,
               IM_ARRAYSIZE(kAnalysisTypes));

  // DND targets
  EmitEntryTarget("Velocity", false, &m_velocityEntry);
  EmitEntryTarget("Position", false, &m_positionEntry);
  EmitEntryTarget("Voltage", false, &m_voltageEntry);

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7);
  ImGui::Combo("Units", &m_selectedUnit, kUnits, IM_ARRAYSIZE(kUnits));

  if (/*!m_selectedTest.empty() &&*/ m_velocityEntry && m_positionEntry &&
      m_voltageEntry) {
    if (ImGui::Button("Load")) {
      m_testdataFuture =
          std::async(std::launch::async, [this] { return BuildTestData(); });
    }
  }
}

void DataSelector::Reset() {
  m_testsFuture = {};
  m_tests.clear();
  m_selectedTest.clear();
  m_testStateEntry = nullptr;
  m_velocityEntry = nullptr;
  m_positionEntry = nullptr;
  m_voltageEntry = nullptr;
  m_testdataFuture = {};
}

DataSelector::Tests DataSelector::LoadTests(
    const glass::DataLogReaderEntry& testStateEntry) {
  Tests tests;
  for (auto&& range : testStateEntry.ranges) {
    std::string_view prevState;
    Runs* curRuns = nullptr;
    wpi::log::DataLogReader::iterator lastStart = range.begin();
    for (auto it = range.begin(), end = range.end(); it != end; ++it) {
      std::string_view testState;
      if (it->GetEntry() != testStateEntry.entry ||
          !it->GetString(&testState)) {
        continue;
      }

      // track runs as iterator ranges of the same test
      if (testState != prevState) {
        if (curRuns) {
          curRuns->emplace_back(lastStart, it);
        }
        lastStart = it;
      }
      prevState = testState;

      if (testState == "none") {
        curRuns = nullptr;
        continue;
      }

      auto [testName, direction] = wpi::rsplit(testState, '-');
      auto testIt = tests.find(testName);
      if (testIt == tests.end()) {
        testIt = tests.emplace(std::string{testName}, State{}).first;
      }
      auto stateIt = testIt->second.find(testState);
      if (stateIt == testIt->second.end()) {
        stateIt = testIt->second.emplace(std::string{testState}, Runs{}).first;
      }
      curRuns = &stateIt->second;
    }

    if (curRuns) {
      curRuns->emplace_back(lastStart, range.end());
    }
  }
  return tests;
}

template <typename T>
static void AddSample(std::vector<MotorData::Run::Sample<T>>& samples,
                      const wpi::log::DataLogRecord& record, bool isDouble) {
  if (isDouble) {
    double val;
    if (record.GetDouble(&val)) {
      samples.emplace_back(units::second_t{record.GetTimestamp() * 1.0e-6},
                           T{val});
    }
  } else {
    float val;
    if (record.GetFloat(&val)) {
      samples.emplace_back(units::second_t{record.GetTimestamp() * 1.0e-6},
                           T{static_cast<double>(val)});
    }
  }
}

TestData DataSelector::BuildTestData() {
  TestData data;
  data.distanceUnit = kUnits[m_selectedUnit];
  data.mechanismType = analysis::FromName(kAnalysisTypes[m_selectedAnalysis]);
  bool voltageDouble = m_voltageEntry->type == "double";
  bool positionDouble = m_positionEntry->type == "double";
  bool velocityDouble = m_velocityEntry->type == "double";

  for (auto&& test : m_tests) {
    for (auto&& state : test.second) {
      auto& motorData = data.motorData[state.first];
      for (auto&& range : state.second) {
        auto& run = motorData.runs.emplace_back();
        for (auto&& record : range) {
          if (record.GetEntry() == m_voltageEntry->entry) {
            AddSample(run.voltage, record, voltageDouble);
          } else if (record.GetEntry() == m_positionEntry->entry) {
            AddSample(run.position, record, positionDouble);
          } else if (record.GetEntry() == m_velocityEntry->entry) {
            AddSample(run.velocity, record, velocityDouble);
          }
        }
      }
    }
  }

  return data;
}
