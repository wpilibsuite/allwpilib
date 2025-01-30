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
#include <optional>
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

void DrawCheck() {
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

/**
 * Processes a folder picker.
 * @param selector The folder picker.
 * @param selectedDirectoryPath The path to the selected folder.
 */
void ProcessDirectorySelector(std::unique_ptr<pfd::select_folder>& selector,
                              std::string& selectedDirectoryPath) {
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selectedDirectoryPath = selectedFiles;
    }
    selector.reset();
  }
}

/**
 * Adds a button that opens a file picker for picking one file.
 * @param text The button label.
 * @param selectedFilePath The path to the selected file.
 * @param selector The file selector.
 * @param fileType The human friendly name for the file filter.
 * @param fileExtensions The list of file extensions patterns that the selected
 * file must match. The list of patterns must be space-separated.
 */
void SelectFileButton(const char* text, std::string& selectedFilePath,
                      std::unique_ptr<pfd::open_file>& selector,
                      const std::string& fileType,
                      const std::string& fileExtensions) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{fileType, fileExtensions},
        pfd::opt::none);
  }
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selectedFilePath = selectedFiles[0];
    }
    selector.reset();
  }
}

/**
 * Adds a button that opens a file picker to select multiple files.
 * @param text The button label.
 * @param selectedFilePaths The list of paths to the selected files.
 * @param selector The file selector.
 * @param fileType The human friendly name for the file filter.
 * @param fileExtensions The list of file extensions patterns that selected
 * files must match. The list of patterns must be space-separated.
 */
void SelectFilesButton(const char* text,
                       std::vector<std::string>& selectedFilePaths,
                       std::unique_ptr<pfd::open_file>& selector,
                       const std::string& fileType,
                       const std::string& fileExtensions) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{fileType, fileExtensions},
        pfd::opt::multiselect);
  }
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      selectedFilePaths = selectedFiles;
    }
    selector.reset();
  }
}

/**
 * Adds a button to open a folder picker to select a folder.
 * @param text The button label.
 * @param selector The folder picker.
 * @param selectedDirectory The selected directory.
 */
void SelectDirectoryButton(const char* text,
                           std::unique_ptr<pfd::select_folder>& selector,
                           std::string& selectedDirectory) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::select_folder>("Select Directory", "");
  }
  ProcessDirectorySelector(selector, selectedDirectory);
}

std::string getFileName(std::string path) {
  size_t lastSlash = path.find_last_of("/\\");
  size_t lastDot = path.find_last_of(".");
  return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

void SaveCalibration(wpi::json& field, std::string outputName) {
  static std::unique_ptr<pfd::select_folder> saveDirSelector;
  static std::string saveDir;
  if (saveDir.empty() && !saveDirSelector) {
    saveDirSelector =
        std::make_unique<pfd::select_folder>("Select Download Folder", "");
  }
  ProcessDirectorySelector(saveDirSelector, saveDir);
  if (!field.empty() && !saveDir.empty()) {
    std::cout << "Saving calibration to " << saveDir << std::endl;
    std::ofstream out(saveDir + "/" + outputName + ".json");
    out << field.dump(4);
    out.close();

    std::ofstream fmap(saveDir + "/" + outputName + ".fmap");
    fmap << fmap::convertfmap(field).dump(4);
    fmap.close();

    field.clear();
    saveDir.clear();
  }
}

void CombineCalibrations(
    Fieldmap& currentReferenceMap, std::string& idealFieldMapPath,
    std::unique_ptr<pfd::open_file>& idealFieldMapSelector) {
  static std::unique_ptr<pfd::open_file> calibratedFieldMapMultiselector;
  static std::vector<std::string> calibratedFieldMapPaths;
  static std::map<int, std::string> combinerMap;
  static int currentCombinerTagId = 0;
  static wpi::json field_combination_json;
  static Fieldmap currentCombinerMap;

  SelectFileButton("Select Ideal Map", idealFieldMapPath, idealFieldMapSelector,
                   "JSON", "*.json");
  if (!idealFieldMapPath.empty()) {
    DrawCheck();
    std::ifstream json(idealFieldMapPath);
    currentReferenceMap = Fieldmap(wpi::json::parse(json));
    currentCombinerMap = currentReferenceMap;
  }
  SelectFilesButton("Select Field Calibrations", calibratedFieldMapPaths,
                    calibratedFieldMapMultiselector, "JSON", "*.json");

  if (!idealFieldMapPath.empty() && !calibratedFieldMapPaths.empty()) {
    for (std::string& file : calibratedFieldMapPaths) {
      ImGui::Selectable(getFileName(file).c_str(), false,
                        ImGuiSelectableFlags_DontClosePopups);
      if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("FieldCalibration", &file, sizeof(file));
        ImGui::TextUnformatted(file.c_str());
        ImGui::EndDragDropSource();
      }
    }

    for (auto& [tagId, filePath] : combinerMap) {
      if (!filePath.empty()) {
        auto text = fmt::format("{}: {}", tagId, filePath);
        ImGui::TextUnformatted(text.c_str());
      } else {
        ImGui::Text("Tag ID %i: <none (DROP HERE)>", tagId);
      }
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("FieldCalibration")) {
          filePath = *(std::string*)payload->Data;
        }
        ImGui::EndDragDropTarget();
      }
    }

    ImGui::InputInt("Tag ID", &currentCombinerTagId);
    ImGui::SameLine();
    if (ImGui::Button("Add", ImVec2(0, 0)) &&
        currentCombinerMap.hasTag(currentCombinerTagId)) {
      combinerMap.emplace(currentCombinerTagId, "");
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove", ImVec2(0, 0))) {
      combinerMap.erase(currentCombinerTagId);
    }
  }
  ImGui::Separator();
  if (ImGui::Button("Close", ImVec2(0, 0))) {
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Download", ImVec2(0, 0))) {
    for (auto& [key, val] : combinerMap) {
      std::ifstream json(val);
      Fieldmap map(wpi::json::parse(json));
      currentCombinerMap.replaceTag(key, map.getTag(key));
    }
    field_combination_json = currentCombinerMap.toJson();
    SaveCalibration(field_combination_json, "combined_calibration");
  }

  ImGui::EndPopup();
}

void VisualizeCalibrations(
    Fieldmap& currentReferenceMap, std::string& calibratedFieldMapPath,
    std::unique_ptr<pfd::open_file>& calibratedFieldMapSelector,
    std::string& idealFieldMapPath,
    std::unique_ptr<pfd::open_file>& idealFieldMapSelector) {
  static int focusedTag = 1;
  static int referenceTag = 1;
  static Fieldmap currentCalibrationMap;
  SelectFileButton("Select Calibration JSON", calibratedFieldMapPath,
                   calibratedFieldMapSelector, "JSON", "*.json");

  if (!calibratedFieldMapPath.empty()) {
    ImGui::SameLine();
    DrawCheck();
  }

  SelectFileButton("Select Ideal Field Map", idealFieldMapPath,
                   idealFieldMapSelector, "JSON", "*.json");

  if (!idealFieldMapPath.empty()) {
    ImGui::SameLine();
    DrawCheck();
  }

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Focused Tag", &focusedTag);
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Reference Tag", &referenceTag);

  if (!calibratedFieldMapPath.empty() && !idealFieldMapPath.empty()) {
    std::ifstream calJson(calibratedFieldMapPath);
    std::ifstream refJson(idealFieldMapPath);

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

  static std::unique_ptr<pfd::open_file> cameraIntrinsicsSelector;
  static std::unique_ptr<pfd::open_file> idealFieldMapSelector;
  static std::unique_ptr<pfd::open_file> calibratedFieldMapSelector;

  static std::unique_ptr<pfd::select_folder> fieldVideoDirSelector;

  static wpi::json field_calibration_json;

  static std::string cameraIntrinsicsPath;
  static std::string idealFieldMapPath;
  static std::string fieldVideoDir;
  static std::string calibratedFieldMapPath;

  cameracalibration::CameraModel cameraModel = {
      Eigen::Matrix<double, 3, 3>::Identity(),
      Eigen::Matrix<double, 8, 1>::Zero(), 0.0};
  static bool mrcal = true;

  static double squareWidth = 0.709;
  static double markerWidth = 0.551;
  static int boardWidth = 12;
  static int boardHeight = 8;
  static double imagerWidth = 1920;
  static double imagerHeight = 1080;

  static int pinnedTag = 1;

  static int maxFRCTag = 22;

  static Fieldmap currentReferenceMap;

  // camera matrix selector button
  SelectFileButton("Select Camera Intrinsics JSON", cameraIntrinsicsPath,
                   cameraIntrinsicsSelector, "JSON Files", "*.json");

  ImGui::SameLine();
  ImGui::Text("Or");
  ImGui::SameLine();

  // camera calibration button
  if (ImGui::Button("Calibrate Camera")) {
    cameraIntrinsicsPath.clear();
    ImGui::OpenPopup("Camera Calibration");
  }

  if (!cameraIntrinsicsPath.empty()) {
    DrawCheck();
  }

  // field json selector button
  SelectFileButton("Select Field Map JSON", idealFieldMapPath,
                   idealFieldMapSelector, "JSON Files", "*.json");

  if (!idealFieldMapPath.empty()) {
    DrawCheck();
  }

  // field calibration directory selector button
  SelectDirectoryButton("Select Field Calibration Directory",
                        fieldVideoDirSelector, fieldVideoDir);

  if (!fieldVideoDir.empty()) {
    DrawCheck();
  }

  // pinned tag text field
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Pinned Tag", &pinnedTag);

  // calibrate button
  if (ImGui::Button("Calibrate!!!")) {
    int calibrationOutput = fieldcalibration::calibrate(
        fieldVideoDir.c_str(), field_calibration_json, cameraIntrinsicsPath,
        idealFieldMapPath.c_str(), pinnedTag, showDebug);

    if (calibrationOutput == 1) {
      ImGui::OpenPopup("Field Calibration Error");
    }

    SaveCalibration(field_calibration_json, "field_calibration");
  }

  if (ImGui::Button("Visualize")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Visualize Calibration");
  }
  if (ImGui::Button("Combine Calibrations")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Combine Calibrations");
  }
  if (fieldVideoDir.empty() || cameraIntrinsicsPath.empty() ||
      idealFieldMapPath.empty()) {
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

    SelectFileButton("Select Camera Calibration Video", cameraIntrinsicsPath,
                     cameraIntrinsicsSelector, "Video Files",
                     "*.mp4 *.mov *.m4v *.mkv *.avi");

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputDouble("Square Width (in)", &squareWidth);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputDouble("Marker Width (in)", &markerWidth);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Board Width (squares)", &boardWidth);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Board Height (squares)", &boardHeight);
    std::optional<cameracalibration::CameraModel> model;
    bool calibrateButtonPressed = false;
    if (mrcal) {
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Image Width (pixels)", &imagerWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Image Height (pixels)", &imagerHeight);

      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !cameraIntrinsicsPath.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        model = cameracalibration::calibrate(
            cameraIntrinsicsPath, markerWidth, boardWidth, boardHeight,
            imagerWidth, imagerHeight, showDebug);
        calibrateButtonPressed = true;
      }
    } else {
      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !cameraIntrinsicsPath.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        model = cameracalibration::calibrate(cameraIntrinsicsPath, squareWidth,
                                             markerWidth, boardWidth,
                                             boardHeight, showDebug);
        calibrateButtonPressed = true;
      }
    }
    if (calibrateButtonPressed && model) {
      size_t lastSeparatorPos = cameraIntrinsicsPath.find_last_of("/\\");
      std::string outputFilePath;

      if (lastSeparatorPos != std::string::npos) {
        outputFilePath = cameraIntrinsicsPath.substr(0, lastSeparatorPos)
                             .append("/cameracalibration.json");
      }
      cameraIntrinsicsPath = outputFilePath;

      std::ofstream output_file(outputFilePath);
      output_file << wpi::json(cameraModel).dump(4) << std::endl;
      output_file.close();
      ImGui::CloseCurrentPopup();
      calibrateButtonPressed = false;
    } else if (calibrateButtonPressed && !model) {
      std::cout << "calibration failed and popup ready" << std::endl;
      ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
      ImGui::OpenPopup("Camera Calibration Error");
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
    VisualizeCalibrations(currentReferenceMap, calibratedFieldMapPath,
                          calibratedFieldMapSelector, idealFieldMapPath,
                          idealFieldMapSelector);
  }

  if (ImGui::BeginPopupModal("Combine Calibrations", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    CombineCalibrations(currentReferenceMap, idealFieldMapPath,
                        idealFieldMapSelector);
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
