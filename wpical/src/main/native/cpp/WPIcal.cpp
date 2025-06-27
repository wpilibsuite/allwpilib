// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>
#include <fieldmap.h>
#include <fmap.h>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numbers>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
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

void processFileSelector(std::unique_ptr<pfd::open_file>& selector,
                         std::string& selected_file) {
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selected_file = selectedFiles[0];
    }
    selector.reset();
  }
}

void processFilesSelector(std::unique_ptr<pfd::open_file>& selector,
                          std::vector<std::string>& selected_files) {
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selected_files = selectedFiles;
    }
    selector.reset();
  }
}

void processDirectorySelector(std::unique_ptr<pfd::select_folder>& selector,
                              std::string& selected_directory) {
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selected_directory = selectedFiles;
    }
    selector.reset();
  }
}

void openFileButton(const char* text, std::string& selected_file,
                    std::unique_ptr<pfd::open_file>& selector,
                    const std::string& file_type,
                    const std::string& file_extensions) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{file_type, file_extensions},
        pfd::opt::none);
  }
}

void openFilesButton(const char* text, std::vector<std::string>& selected_files,
                     std::unique_ptr<pfd::open_file>& selector,
                     const std::string& file_type,
                     const std::string& file_extensions) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{file_type, file_extensions},
        pfd::opt::multiselect);
  }
}

void openDirectoryButton(const char* text,
                         std::unique_ptr<pfd::select_folder>& selector,
                         std::string& selected_directory) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::select_folder>("Select Directory", "");
  }
}

std::string getFileName(std::string path) {
  size_t lastSlash = path.find_last_of("/\\");
  size_t lastDot = path.find_last_of(".");
  return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

static bool EmitEntryTarget(int tag_id, std::string& file) {
  if (!file.empty()) {
    auto text = fmt::format("{}: {}", tag_id, file);
    ImGui::TextUnformatted(text.c_str());
  } else {
    ImGui::Text("Tag ID %i: <none (DROP HERE)>", tag_id);
  }
  bool rv = false;
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload =
            ImGui::AcceptDragDropPayload("FieldCalibration")) {
      file = *(std::string*)payload->Data;
      rv = true;
    }
    ImGui::EndDragDropTarget();
  }
  return rv;
}

void saveCalibration(wpi::json& field, std::string& output_directory,
                     std::string output_name, bool& isCalibrating) {
  if (!field.empty() && !output_directory.empty()) {
    std::cout << "Saving calibration to " << output_directory << std::endl;
    std::ofstream out(output_directory + "/" + output_name + ".json");
    out << field.dump(4);
    out.close();

    std::ofstream fmap(output_directory + "/" + output_name + ".fmap");
    fmap << fmap::convertfmap(field).dump(4);
    fmap.close();

    field.clear();
    output_directory.clear();
    isCalibrating = false;
  }
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
  static std::unique_ptr<pfd::open_file> field_map_selector;
  static std::unique_ptr<pfd::open_file> output_calibration_json_selector;
  static std::unique_ptr<pfd::open_file> combination_calibrations_selector;

  static std::unique_ptr<pfd::select_folder>
      field_calibration_directory_selector;
  static std::unique_ptr<pfd::select_folder> download_directory_selector;

  static wpi::json field_calibration_json;
  static wpi::json field_combination_json;

  static std::string selected_camera_intrinsics;
  static std::string selected_field_map;
  static std::string selected_field_calibration_directory;
  static std::string selected_download_directory;
  static std::string output_calibration_json_path;
  static std::vector<std::string> selected_combination_calibrations;

  static std::map<int, std::string> combiner_map;
  static int current_combiner_tag_id = 0;

  static bool isCalibrating = false;

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
  static Fieldmap currentCombinerMap;

  // camera matrix selector button
  openFileButton("Select Camera Intrinsics JSON", selected_camera_intrinsics,
                 camera_intrinsics_selector, "JSON Files", "*.json");
  processFileSelector(camera_intrinsics_selector, selected_camera_intrinsics);

  ImGui::SameLine();
  ImGui::Text("Or");
  ImGui::SameLine();

  // camera calibration button
  if (ImGui::Button("Calibrate Camera")) {
    selected_camera_intrinsics.clear();
    ImGui::OpenPopup("Camera Calibration");
  }

  if (!selected_camera_intrinsics.empty()) {
    drawCheck();
  }

  // field json selector button
  openFileButton("Select Field Map JSON", selected_field_map,
                 field_map_selector, "JSON Files", "*.json");
  processFileSelector(field_map_selector, selected_field_map);

  if (!selected_field_map.empty()) {
    drawCheck();
  }

  // field calibration directory selector button
  openDirectoryButton("Select Field Calibration Directory",
                      field_calibration_directory_selector,
                      selected_field_calibration_directory);
  processDirectorySelector(field_calibration_directory_selector,
                           selected_field_calibration_directory);

  if (!selected_field_calibration_directory.empty()) {
    drawCheck();
  }

  // pinned tag text field
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Pinned Tag", &pinnedTag);

  // calibrate button
  if (ImGui::Button("Calibrate!!!")) {
    int calibrationOutput = fieldcalibration::calibrate(
        selected_field_calibration_directory.c_str(), field_calibration_json,
        selected_camera_intrinsics, selected_field_map.c_str(), pinnedTag,
        showDebug);

    if (calibrationOutput == 1) {
      ImGui::OpenPopup("Field Calibration Error");
    }

    if (selected_download_directory.empty() &&
        !field_calibration_json.empty() && !download_directory_selector) {
      download_directory_selector =
          std::make_unique<pfd::select_folder>("Select Download Folder", "");
    }
  }

  processDirectorySelector(download_directory_selector,
                           selected_download_directory);
  saveCalibration(field_calibration_json, selected_download_directory,
                  "field_calibration", isCalibrating);

  if (ImGui::Button("Visualize")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Visualize Calibration");
  }
  if (ImGui::Button("Combine Calibrations")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Combine Calibrations");
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
      openFileButton("Select Camera Calibration Video",
                     selected_camera_intrinsics, camera_intrinsics_selector,
                     "Video Files", "*.mp4 *.mov *.m4v *.mkv *.avi");
      processFileSelector(camera_intrinsics_selector,
                          selected_camera_intrinsics);

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
      openFileButton("Select Camera Calibration Video",
                     selected_camera_intrinsics, camera_intrinsics_selector,
                     "Video Files", "*.mp4 *.mov *.m4v *.mkv *.avi");
      processFileSelector(camera_intrinsics_selector,
                          selected_camera_intrinsics);

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
    openFileButton("Select Calibration JSON", output_calibration_json_path,
                   output_calibration_json_selector, "JSON", "*.json");
    processFileSelector(output_calibration_json_selector,
                        output_calibration_json_path);

    if (!output_calibration_json_path.empty()) {
      ImGui::SameLine();
      drawCheck();
    }

    openFileButton("Select Ideal Field Map", selected_field_map,
                   field_map_selector, "JSON", "*.json");
    processFileSelector(field_map_selector, selected_field_map);

    if (!selected_field_map.empty()) {
      ImGui::SameLine();
      drawCheck();
    }

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Focused Tag", &focusedTag);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Reference Tag", &referenceTag);

    if (!output_calibration_json_path.empty() && !selected_field_map.empty()) {
      std::ifstream calJson(output_calibration_json_path);
      std::ifstream refJson(selected_field_map);

      currentCalibrationMap = Fieldmap(wpi::json::parse(calJson));
      currentReferenceMap = Fieldmap(wpi::json::parse(refJson));

      if (currentCalibrationMap.getNumTags() !=
          currentReferenceMap.getNumTags()) {
        ImGui::TextWrapped(
            "The number of tags in the calibration output and the ideal field "
            "map "
            "do not match. Please ensure that the calibration output and ideal "
            "field "
            "map have the same number of tags.");
      } else if (currentReferenceMap.hasTag(focusedTag) &&
                 currentReferenceMap.hasTag(referenceTag)) {
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

        ImGui::TextWrapped("X Difference: %s (m)",
                           std::to_string(xDiff).c_str());
        ImGui::TextWrapped("Y Difference: %s (m)",
                           std::to_string(yDiff).c_str());
        ImGui::TextWrapped("Z Difference: %s (m)",
                           std::to_string(zDiff).c_str());

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
      } else {
        ImGui::TextWrapped(
            "Please select tags that are in the ideal field map and "
            "calibration map");
      }
    }

    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("Combine Calibrations", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    openFileButton("Select Ideal Map", selected_field_map, field_map_selector,
                   "JSON", "*.json");
    processFileSelector(field_map_selector, selected_field_map);
    if (!selected_field_map.empty()) {
      drawCheck();
      std::ifstream json(selected_field_map);
      currentReferenceMap = Fieldmap(wpi::json::parse(json));
      currentCombinerMap = currentReferenceMap;
    }
    openFilesButton("Select Field Calibrations",
                    selected_combination_calibrations,
                    combination_calibrations_selector, "JSON", "*.json");
    processFilesSelector(combination_calibrations_selector,
                         selected_combination_calibrations);

    if (!selected_field_map.empty() &&
        !selected_combination_calibrations.empty()) {
      for (std::string& file : selected_combination_calibrations) {
        ImGui::Selectable(getFileName(file).c_str(), false,
                          ImGuiSelectableFlags_DontClosePopups);
        if (ImGui::BeginDragDropSource()) {
          ImGui::SetDragDropPayload("FieldCalibration", &file, sizeof(file));
          ImGui::TextUnformatted(file.c_str());
          ImGui::EndDragDropSource();
        }
      }

      for (auto& [key, val] : combiner_map) {
        EmitEntryTarget(key, val);
      }

      ImGui::InputInt("Tag ID", &current_combiner_tag_id);
      ImGui::SameLine();
      if (ImGui::Button("Add", ImVec2(0, 0)) &&
          currentCombinerMap.hasTag(current_combiner_tag_id)) {
        combiner_map.emplace(current_combiner_tag_id, "");
      }
      ImGui::SameLine();
      if (ImGui::Button("Remove", ImVec2(0, 0))) {
        combiner_map.erase(current_combiner_tag_id);
      }
    }
    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(0, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Download", ImVec2(0, 0))) {
      for (auto& [key, val] : combiner_map) {
        std::ifstream json(val);
        Fieldmap map(wpi::json::parse(json));
        currentCombinerMap.replaceTag(key, map.getTag(key));
      }
      field_combination_json = currentCombinerMap.toJson();
    }

    if (selected_download_directory.empty() &&
        !field_combination_json.empty() && !download_directory_selector) {
      download_directory_selector =
          std::make_unique<pfd::select_folder>("Select Download Folder", "");
    }

    processDirectorySelector(download_directory_selector,
                             selected_download_directory);
    saveCalibration(field_combination_json, selected_download_directory,
                    "combined_calibration", isCalibrating);

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
