// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>
#include <fieldmap.h>
#include <fmap.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <numbers>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <portable-file-dialogs.h>
#include <tagpose.h>
#include <wpi/json.h>
#include <wpigui.h>

namespace gui = wpi::gui;

const char* GetWPILibVersion();

#ifdef __linux__
const bool showDebug = false;
#else
const bool showDebug = true;
#endif

namespace wpical {
std::string_view GetResource_wpical_16_png();
std::string_view GetResource_wpical_32_png();
std::string_view GetResource_wpical_48_png();
std::string_view GetResource_wpical_64_png();
std::string_view GetResource_wpical_128_png();
std::string_view GetResource_wpical_256_png();
std::string_view GetResource_wpical_512_png();
}  // namespace wpical

void drawCheck() {
  ImGui::SameLine();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  ImVec2 pos = ImGui::GetCursorScreenPos();
  float size = ImGui::GetFontSize();

  ImVec2 p1 = ImVec2(pos.x + size * 0.25f, pos.y + size * 0.5f);
  ImVec2 p2 = ImVec2(pos.x + size * 0.45f, pos.y + size * 0.7f);
  ImVec2 p3 = ImVec2(pos.x + size * 0.75f, pos.y + size * 0.3f);

  float thickness = 3.0f;
  draw_list->AddLine(p1, p2, IM_COL32(0, 255, 0, 255), thickness);
  draw_list->AddLine(p2, p3, IM_COL32(0, 255, 0, 255), thickness);
  ImGui::NewLine();
}

static void DisplayGui() {
  ImGui::GetStyle().WindowRounding = 0;

  // fill entire OS window with this window
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  int width, height;
  glfwGetWindowSize(gui::GetSystemWindow(), &width, &height);
  ImGui::SetNextWindowSize(
      ImVec2(static_cast<float>(width), static_cast<float>(height)));

  ImGui::Begin("Entries", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoCollapse);

  // main menu
  ImGui::BeginMenuBar();
  gui::EmitViewMenu();
  if (ImGui::BeginMenu("View")) {
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();

  static std::unique_ptr<pfd::open_file> camera_intrinsics_selector;
  static std::string selected_camera_intrinsics;

  static std::unique_ptr<pfd::open_file> field_map_selector;
  static std::string selected_field_map;

  static std::unique_ptr<pfd::select_folder>
      field_calibration_directory_selector;
  static std::string selected_field_calibration_directory;

  static std::unique_ptr<pfd::select_folder> download_directory_selector;
  static std::string selected_download_directory;

  static std::string calibration_json_path;

  cameracalibration::CameraModel cameraModel = {
      .intrinsic_matrix = Eigen::Matrix<double, 3, 3>::Identity(),
      .distortion_coefficients = Eigen::Matrix<double, 8, 1>::Zero(),
      .avg_reprojection_error = 0.0};
  static bool mrcal = true;

  static double squareWidth = 0.709;
  static double markerWidth = 0.551;
  static int boardWidth = 12;
  static int boardHeight = 8;
  static double imagerWidth = 1920;
  static double imagerHeight = 1080;

  static int pinnedTag = 1;

  static int focusedTag = 1;
  static int referenceTag = 1;

  static int maxFRCTag = 22;

  static Fieldmap currentCalibrationMap;
  static Fieldmap currentReferenceMap;

  // camera matrix selector button
  if (ImGui::Button("Upload Camera Intrinsics")) {
    camera_intrinsics_selector = std::make_unique<pfd::open_file>(
        "Select Camera Intrinsics JSON", "",
        std::vector<std::string>{"JSON", "*.json"}, pfd::opt::none);
  }

  ImGui::SameLine();
  ImGui::Text("Or");
  ImGui::SameLine();

  // camera calibration button
  if (ImGui::Button("Calibrate Camera")) {
    selected_camera_intrinsics.clear();
    ImGui::OpenPopup("Camera Calibration");
  }

  if (camera_intrinsics_selector) {
    auto selectedFiles = camera_intrinsics_selector->result();
    if (!selectedFiles.empty()) {
      selected_camera_intrinsics = selectedFiles[0];
    }
    camera_intrinsics_selector.reset();
  }

  if (!selected_camera_intrinsics.empty()) {
    drawCheck();
  }

  // field json selector button
  if (ImGui::Button("Select Field Map JSON")) {
    field_map_selector = std::make_unique<pfd::open_file>(
        "Select Json File", "",
        std::vector<std::string>{"JSON Files", "*.json"}, pfd::opt::none);
  }

  if (field_map_selector) {
    auto selectedFiles = field_map_selector->result();
    if (!selectedFiles.empty()) {
      selected_field_map = selectedFiles[0];
    }
    field_map_selector.reset();
  }

  if (!selected_field_map.empty()) {
    drawCheck();
  }

  // field calibration directory selector button
  if (ImGui::Button("Select Field Calibration Folder")) {
    field_calibration_directory_selector = std::make_unique<pfd::select_folder>(
        "Select Field Calibration Folder", "");
  }

  if (field_calibration_directory_selector) {
    auto selectedFiles = field_calibration_directory_selector->result();
    if (!selectedFiles.empty()) {
      selected_field_calibration_directory = selectedFiles;
    }
    field_calibration_directory_selector.reset();
  }

  if (!selected_field_calibration_directory.empty()) {
    drawCheck();
  }

  // pinned tag text field
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Pinned Tag", &pinnedTag);

  // calibrate button
  if (ImGui::Button("Calibrate!!!")) {
    if (!selected_field_calibration_directory.empty() &&
        !selected_camera_intrinsics.empty() && !selected_field_map.empty()) {
      download_directory_selector =
          std::make_unique<pfd::select_folder>("Select Download Folder", "");
      if (download_directory_selector) {
        auto selectedFiles = download_directory_selector->result();
        if (!selectedFiles.empty()) {
          selected_download_directory = selectedFiles;
        }
        download_directory_selector.reset();
      }

      calibration_json_path = selected_download_directory + "/output.json";

      int calibrationOutput = fieldcalibration::calibrate(
          selected_field_calibration_directory.c_str(), calibration_json_path,
          selected_camera_intrinsics, selected_field_map.c_str(), pinnedTag,
          showDebug);

      if (calibrationOutput == 1) {
        ImGui::OpenPopup("Field Calibration Error");
      } else if (calibrationOutput == 0) {
        std::ifstream caljsonpath(calibration_json_path);
        try {
          wpi::json fmap = fmap::convertfmap(wpi::json::parse(caljsonpath));
          std::ofstream out(selected_download_directory + "/output.fmap");
          out << fmap.dump(4);
          out.close();
          ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
          ImGui::OpenPopup("Visualize Calibration");
        } catch (...) {
          ImGui::OpenPopup("Fmap Conversion Error");
        }
      }
    }
  }
  if (ImGui::Button("Visualize")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Visualize Calibration");
  }
  if (selected_field_calibration_directory.empty() ||
      selected_camera_intrinsics.empty() || selected_field_map.empty()) {
    ImGui::TextWrapped(
        "Some inputs are empty! please enter your camera calibration video, "
        "field map, and field calibration directory");
  } else if (!(pinnedTag > 0 && pinnedTag <= maxFRCTag)) {
    ImGui::TextWrapped(
        "The pinned tag is not within the normal range for FRC fields (1-22), "
        "If you proceed, You may experience a bad calibration.");
  } else {
    ImGui::TextWrapped("Calibration Ready");
  }

  // error popup window
  if (ImGui::BeginPopupModal("Field Calibration Error", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextWrapped(
        "Field Calibration Failed - please try again, ensuring that:");
    ImGui::TextWrapped(
        "- You have selected the correct camera intrinsics JSON or camera "
        "calibration video");
    ImGui::TextWrapped("- You have selected the correct ideal field map JSON");
    ImGui::TextWrapped(
        "- You have selected the correct field calibration video directory");
    ImGui::TextWrapped(
        "- Your field calibration video directory contains only field "
        "calibration videos and no other files");
    ImGui::TextWrapped("- Your pinned tag is a valid FRC Apriltag");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("Fmap Conversion Error", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextWrapped(
        "Fmap conversion failed - you can still use the calibration output on "
        "Limelight platforms by converting the .json output to .fmap using the "
        "Limelight map builder tool");
    ImGui::TextWrapped("https://tools.limelightvision.io/map-builder");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // successful field calibration popup window
  if (ImGui::BeginPopupModal("Success", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextWrapped("Success, output JSON generated in selected directory");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // camera calibration popup window
  if (ImGui::BeginPopupModal("Camera Calibration", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    // Camera Calibration Error calibration popup window
    if (ImGui::BeginPopupModal("Camera Calibration Error", NULL,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::TextWrapped(
          "Camera calibration failed. Please make sure you have uploaded the "
          "correct video file");
      ImGui::Separator();
      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if (ImGui::Button("MRcal")) {
      mrcal = true;
    }

    ImGui::SameLine();
    ImGui::Text("or");
    ImGui::SameLine();

    if (ImGui::Button("OpenCV")) {
      mrcal = false;
    }

    if (mrcal) {
      if (ImGui::Button("Select Camera Calibration Video")) {
        camera_intrinsics_selector = std::make_unique<pfd::open_file>(
            "Select Camera Calibration Video", "",
            std::vector<std::string>{"Video Files",
                                     "*.mp4 *.mov *.m4v *.mkv *.avi"},
            pfd::opt::none);
      }

      if (camera_intrinsics_selector) {
        auto selectedFiles = camera_intrinsics_selector->result();
        if (!selectedFiles.empty()) {
          selected_camera_intrinsics = selectedFiles[0];
        }
        camera_intrinsics_selector.reset();
      }

      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Square Width (in)", &squareWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Marker Width (in)", &markerWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputInt("Board Width (squares)", &boardWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputInt("Board Height (squares)", &boardHeight);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Image Width (pixels)", &imagerWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Image Height (pixels)", &imagerHeight);

      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !selected_camera_intrinsics.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        int ret = cameracalibration::calibrate(
            selected_camera_intrinsics.c_str(), cameraModel, markerWidth,
            boardWidth, boardHeight, imagerWidth, imagerHeight, showDebug);
        if (ret == 0) {
          size_t lastSeparatorPos =
              selected_camera_intrinsics.find_last_of("/\\");
          std::string output_file_path;

          if (lastSeparatorPos != std::string::npos) {
            output_file_path =
                selected_camera_intrinsics.substr(0, lastSeparatorPos)
                    .append("/cameracalibration.json");
          }

          selected_camera_intrinsics = output_file_path;

          cameracalibration::dumpJson(cameraModel, output_file_path);
          ImGui::CloseCurrentPopup();
        } else if (ret == 1) {
          std::cout << "calibration failed and popup ready" << std::endl;
          ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
          ImGui::OpenPopup("Camera Calibration Error");
        } else {
          ImGui::CloseCurrentPopup();
        }
      }
    } else {
      if (ImGui::Button("Select Camera Calibration Video")) {
        camera_intrinsics_selector = std::make_unique<pfd::open_file>(
            "Select Camera Calibration Video", "",
            std::vector<std::string>{"Video Files",
                                     "*.mp4 *.mov *.m4v *.mkv *.avi"},
            pfd::opt::none);
      }

      if (camera_intrinsics_selector) {
        auto selectedFiles = camera_intrinsics_selector->result();
        if (!selectedFiles.empty()) {
          selected_camera_intrinsics = selectedFiles[0];
        }
        camera_intrinsics_selector.reset();
      }

      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Square Width (in)", &squareWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Marker Width (in)", &markerWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputInt("Board Width (squares)", &boardWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputInt("Board Height (squares)", &boardHeight);

      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !selected_camera_intrinsics.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        int ret = cameracalibration::calibrate(
            selected_camera_intrinsics.c_str(), cameraModel, squareWidth,
            markerWidth, boardWidth, boardHeight, showDebug);
        if (ret == 0) {
          size_t lastSeparatorPos =
              selected_camera_intrinsics.find_last_of("/\\");
          std::string output_file_path;

          if (lastSeparatorPos != std::string::npos) {
            output_file_path =
                selected_camera_intrinsics.substr(0, lastSeparatorPos)
                    .append("/cameracalibration.json");
          }

          selected_camera_intrinsics = output_file_path;

          cameracalibration::dumpJson(cameraModel, output_file_path);
          ImGui::CloseCurrentPopup();
        } else if (ret == 1) {
          std::cout << "calibration failed and popup ready" << std::endl;
          ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
          ImGui::OpenPopup("Camera Calibration Error");
        } else {
          ImGui::CloseCurrentPopup();
        }
      }
    }

    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  // visualize calibration popup
  if (ImGui::BeginPopupModal("Visualize Calibration", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::Button("Load Calibrated Field")) {
      calibration_json_path =
          std::make_unique<pfd::open_file>(
              "Select Json File", "",
              std::vector<std::string>{"JSON Files", "*.json"}, pfd::opt::none)
              ->result()[0];
    }

    if (!calibration_json_path.empty()) {
      ImGui::SameLine();
      drawCheck();
    }

    if (ImGui::Button("Load Reference Field")) {
      selected_field_map =
          std::make_unique<pfd::open_file>(
              "Select Json File", "",
              std::vector<std::string>{"JSON Files", "*.json"}, pfd::opt::none)
              ->result()[0];
    }

    if (!selected_field_map.empty()) {
      ImGui::SameLine();
      drawCheck();
    }

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Focused Tag", &focusedTag);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Reference Tag", &referenceTag);

    if (!calibration_json_path.empty() && !selected_field_map.empty()) {
      std::ifstream calJson(calibration_json_path);
      std::ifstream refJson(selected_field_map);

      currentCalibrationMap = Fieldmap(wpi::json::parse(calJson));
      currentReferenceMap = Fieldmap(wpi::json::parse(refJson));

      double xDiff = currentReferenceMap.getTag(focusedTag).xPos -
                     currentCalibrationMap.getTag(focusedTag).xPos;
      double yDiff = currentReferenceMap.getTag(focusedTag).yPos -
                     currentCalibrationMap.getTag(focusedTag).yPos;
      double zDiff = currentReferenceMap.getTag(focusedTag).zPos -
                     currentCalibrationMap.getTag(focusedTag).zPos;
      double yawDiff = currentReferenceMap.getTag(focusedTag).yawRot -
                       currentCalibrationMap.getTag(focusedTag).yawRot;
      double pitchDiff = currentReferenceMap.getTag(focusedTag).pitchRot -
                         currentCalibrationMap.getTag(focusedTag).pitchRot;
      double rollDiff = currentReferenceMap.getTag(focusedTag).rollRot -
                        currentCalibrationMap.getTag(focusedTag).rollRot;

      double xRef = currentCalibrationMap.getTag(referenceTag).xPos -
                    currentCalibrationMap.getTag(focusedTag).xPos;
      double yRef = currentCalibrationMap.getTag(referenceTag).yPos -
                    currentCalibrationMap.getTag(focusedTag).yPos;
      double zRef = currentCalibrationMap.getTag(referenceTag).zPos -
                    currentCalibrationMap.getTag(focusedTag).zPos;

      ImGui::TextWrapped("X Difference: %s (m)", std::to_string(xDiff).c_str());
      ImGui::TextWrapped("Y Difference: %s (m)", std::to_string(yDiff).c_str());
      ImGui::TextWrapped("Z Difference: %s (m)", std::to_string(zDiff).c_str());

      ImGui::TextWrapped(
          "Yaw Difference %s°",
          std::to_string(
              Fieldmap::minimizeAngle(yawDiff * (180.0 / std::numbers::pi)))
              .c_str());
      ImGui::TextWrapped(
          "Pitch Difference %s°",
          std::to_string(
              Fieldmap::minimizeAngle(pitchDiff * (180.0 / std::numbers::pi)))
              .c_str());
      ImGui::TextWrapped(
          "Roll Difference %s°",
          std::to_string(
              Fieldmap::minimizeAngle(rollDiff * (180.0 / std::numbers::pi)))
              .c_str());

      ImGui::NewLine();

      ImGui::TextWrapped("X Reference: %s (m)", std::to_string(xRef).c_str());
      ImGui::TextWrapped("Y Reference: %s (m)", std::to_string(yRef).c_str());
      ImGui::TextWrapped("Z Reference: %s (m)", std::to_string(zRef).c_str());
    }

    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  ImGui::End();
}

#ifndef RUNNING_WPICAL_TESTS
#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
  int argc = __argc;
  char** argv = __argv;
#else
int main(int argc, char** argv) {
#endif
  std::string_view saveDir;
  if (argc == 2) {
    saveDir = argv[1];
  }

  gui::CreateContext();

  gui::AddIcon(wpical::GetResource_wpical_16_png());
  gui::AddIcon(wpical::GetResource_wpical_32_png());
  gui::AddIcon(wpical::GetResource_wpical_48_png());
  gui::AddIcon(wpical::GetResource_wpical_64_png());
  gui::AddIcon(wpical::GetResource_wpical_128_png());
  gui::AddIcon(wpical::GetResource_wpical_256_png());
  gui::AddIcon(wpical::GetResource_wpical_512_png());

  gui::AddLateExecute(DisplayGui);

  gui::Initialize("wpical", 600, 400);
  gui::Main();

  gui::DestroyContext();

  return 0;
}
#endif
