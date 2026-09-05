// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/gui/test/GuiTestEngineRunner.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <print>
#include <string>
#include <string_view>

#include <imgui.h>
#include <imgui_test_engine/imgui_te_engine.h>

#include "wpi/gui/wpigui.hpp"

namespace {
struct RunnerState {
  std::string appName;
  wpi::gui::test::TestRegistrar registrar = nullptr;
  ImGuiTestEngine* engine = nullptr;
  ImGuiTestEngineResultSummary summary;
  int expectedTests = 0;
  int exitCode = 1;
  bool started = false;
  bool completed = false;
};

RunnerState gRunnerState;

std::string MakeDefaultSaveDir(std::string_view appName) {
  std::string safeName;
  safeName.reserve(appName.size());
  for (char ch : appName) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '-' || ch == '_') {
      safeName.push_back(ch);
    } else {
      safeName.push_back('_');
    }
  }

  return (std::filesystem::temp_directory_path() /
          std::format("wpilib-{}-imgui-test", safeName))
      .string();
}

void ResetRunnerState(std::string_view appName,
                      wpi::gui::test::TestRegistrar registrar) {
  gRunnerState.appName = appName;
  gRunnerState.registrar = registrar;
  gRunnerState.engine = nullptr;
  gRunnerState.summary = {};
  gRunnerState.expectedTests = 0;
  gRunnerState.exitCode = 1;
  gRunnerState.started = false;
  gRunnerState.completed = false;
}

void UpdateExitCode() {
  gRunnerState.exitCode =
      gRunnerState.completed &&
              gRunnerState.summary.CountTested == gRunnerState.expectedTests &&
              gRunnerState.summary.CountSuccess == gRunnerState.expectedTests
          ? 0
          : 1;
}

void DestroyEngine() {
  if (gRunnerState.engine) {
    ImGuiTestEngine_DestroyContext(gRunnerState.engine);
    gRunnerState.engine = nullptr;
  }
}
}  // namespace

void wpi::gui::test::InstallTestEngineHooks() {
  wpi::gui::AddInit([] {
    if (!gRunnerState.registrar) {
      std::println(stderr, "No ImGui test registrar was provided.");
      wpi::gui::Exit();
      return;
    }

    gRunnerState.engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& testIo = ImGuiTestEngine_GetIO(gRunnerState.engine);
    testIo.ConfigSavedSettings = false;
    testIo.ConfigRunSpeed = ImGuiTestRunSpeed_Fast;
    testIo.ConfigStopOnError = true;
    testIo.ConfigCaptureEnabled = false;
    testIo.ConfigNoThrottle = true;
    testIo.ConfigVerboseLevel = ImGuiTestVerboseLevel_Warning;
    testIo.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Info;
    testIo.ConfigLogToTTY = true;
    testIo.ConfigWatchdogKillApp = 10.0f;

    ImGuiTestEngine_Start(gRunnerState.engine, ImGui::GetCurrentContext());
    gRunnerState.started = true;

    gRunnerState.registrar(gRunnerState.engine);

    ImVector<ImGuiTest*> tests;
    ImGuiTestEngine_GetTestList(gRunnerState.engine, &tests);
    gRunnerState.expectedTests = tests.Size;
    if (gRunnerState.expectedTests == 0) {
      std::println(stderr, "{} registered no ImGui tests.",
                   gRunnerState.appName);
      wpi::gui::Exit();
      return;
    }

    ImGuiTestEngine_QueueTests(gRunnerState.engine, ImGuiTestGroup_Tests);
  });

  wpi::gui::AddPreSwap([] {
    if (gRunnerState.engine) {
      ImGuiTestEngine_PreSwap(gRunnerState.engine);
    }
  });

  wpi::gui::AddPostSwap([] {
    if (gRunnerState.engine) {
      ImGuiTestEngine_PostSwap(gRunnerState.engine);
    }
  });

  wpi::gui::AddLateExecute([] {
    if (!gRunnerState.engine || gRunnerState.expectedTests == 0) {
      return;
    }

    ImGuiTestEngineIO& testIo = ImGuiTestEngine_GetIO(gRunnerState.engine);
    if (testIo.IsRunningTests ||
        !ImGuiTestEngine_IsTestQueueEmpty(gRunnerState.engine)) {
      return;
    }

    ImGuiTestEngineResultSummary summary;
    ImGuiTestEngine_GetResultSummary(gRunnerState.engine, &summary);
    if (summary.CountTested >= gRunnerState.expectedTests) {
      gRunnerState.summary = summary;
      gRunnerState.completed = true;
      UpdateExitCode();
      wpi::gui::Exit();
    }
  });

  wpi::gui::AddExit([] {
    if (gRunnerState.engine && gRunnerState.started) {
      ImGuiTestEngine_Stop(gRunnerState.engine);
      gRunnerState.started = false;
    }
  });
}

int wpi::gui::test::RunTestApp(std::string_view appName, AppFunc app,
                               TestRegistrar registerTests, int argc,
                               char** argv) {
  ResetRunnerState(appName, registerTests);

  std::string saveDir;
  if (argc >= 2) {
    saveDir = argv[1];
  } else {
    saveDir = MakeDefaultSaveDir(appName);
  }
  std::filesystem::create_directories(saveDir);

  app(saveDir);

  if (!gRunnerState.completed && gRunnerState.engine) {
    ImGuiTestEngineIO& testIo = ImGuiTestEngine_GetIO(gRunnerState.engine);
    if (!testIo.IsRunningTests) {
      ImGuiTestEngine_GetResultSummary(gRunnerState.engine,
                                       &gRunnerState.summary);
      UpdateExitCode();
    }
  }

  std::println(stderr, "{} ImGui tests: {}/{} passed ({} queued)",
               gRunnerState.appName, gRunnerState.summary.CountSuccess,
               gRunnerState.summary.CountTested,
               gRunnerState.summary.CountInQueue);

  DestroyEngine();
  return gRunnerState.exitCode;
}
