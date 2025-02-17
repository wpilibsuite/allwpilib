// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cameracalibration.h>
#include <fieldcalibration.h>
#include <fmap.h>

#include <filesystem>
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
#include <fmt/ranges.h>
#include <frc/MathUtil.h>
#include <frc/apriltag/AprilTag.h>
#include <frc/apriltag/AprilTagFieldLayout.h>
#include <imgui.h>
#include <portable-file-dialogs.h>
#include <units/length.h>
#include <wpi/json.h>
#include <wpigui.h>

namespace gui = wpi::gui;

const char* GetWPILibVersion();

#ifdef __linux__
const bool showDebug = false;
#else
static bool showDebug = true;
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
static frc::AprilTagFieldLayout gIdealFieldLayout;
static std::string gInvalidLayoutPath;
static cameracalibration::CameraModel gCameraModel;

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

void FieldLoadingError() {
  if (ImGui::BeginPopupModal("AprilTag Field Layout Loading Error", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Failed to load AprilTag field layout located at");
    ImGui::TextWrapped("%s", gInvalidLayoutPath.c_str());
    ImGui::TextWrapped("Please make sure field layout is valid.");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void MissingTagInField() {
  if (ImGui::BeginPopupModal("Tag ID Not In Field", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("This tag is not available in the field.");
    ImGui::TextWrapped(
        "Drag another field to this tag or select another tag to drag this "
        "field to.");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void CameraCalibrationSelectorButton(
    const char* text, cameracalibration::CameraModel& cameraModel) {
  static std::unique_ptr<pfd::open_file> selector;
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{"JSON Files", "*.json"},
        pfd::opt::none);
  }
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      try {
        cameraModel = wpi::json::parse(std::ifstream(selectedFiles[0]))
                          .get<cameracalibration::CameraModel>();
      } catch (...) {
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        ImGui::OpenPopup("Camera Calibration Loading Error");
      }
    }
    selector.reset();
  }
}
void FieldSelectorButton(const char* text,
                         std::unique_ptr<pfd::open_file>& selector,
                         frc::AprilTagFieldLayout& layout) {
  if (ImGui::Button(text)) {
    selector = std::make_unique<pfd::open_file>(
        "Select File", "", std::vector<std::string>{"JSON", "*.json"},
        pfd::opt::none);
  }
  if (selector && selector->ready(0)) {
    auto selectedFiles = selector->result();
    if (!selectedFiles.empty()) {
      std::string idealLayoutPath = selectedFiles[0];
      try {
        layout = wpi::json::parse(std::ifstream(idealLayoutPath))
                     .get<frc::AprilTagFieldLayout>();
      } catch (...) {
        gInvalidLayoutPath = idealLayoutPath;
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        ImGui::OpenPopup("AprilTag Field Layout Loading Error");
      }
    }
    selector.reset();
  }
  if (layout.GetTags().size() > 0) {
    DrawCheck();
  }
}

void IdealFieldSelectorButton(const char* text) {
  static std::unique_ptr<pfd::open_file> idealFieldLayoutSelector;
  FieldSelectorButton(text, idealFieldLayoutSelector, gIdealFieldLayout);
}

void SaveCalibratedField(const frc::AprilTagFieldLayout& field,
                         std::string outputName,
                         std::unique_ptr<pfd::select_folder>& saveDirSelector) {
  static std::string saveDir;
  ProcessDirectorySelector(saveDirSelector, saveDir);
  if (!saveDir.empty()) {
    std::cout << "Saving calibration to " << saveDir << std::endl;
    std::ofstream out(saveDir + "/" + outputName + ".json");
    out << wpi::json{field}.dump(4);

    std::ofstream fmap(saveDir + "/" + outputName + ".fmap");
    fmap << wpi::json{fmap::Fieldmap(field)}.dump(4);

    saveDir.clear();
  }
}

void CalibrateCamera() {
  static std::unique_ptr<pfd::open_file> cameraVideoSelector;
  static std::string cameraVideoPath;
  static bool mrcal = true;

  static double squareWidth = 0.709;
  static double markerWidth = 0.551;
  static int boardWidth = 12;
  static int boardHeight = 8;
  static double imageWidth = 1920;
  static double imageHeight = 1080;

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

    SelectFileButton("Select Camera Calibration Video", cameraVideoPath,
                     cameraVideoSelector, "Video Files",
                     "*.mp4 *.mov *.m4v *.mkv *.avi");

#ifndef __linux__
    ImGui::Checkbox("Show Debug Window", &showDebug);
#endif
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
      ImGui::InputDouble("Image Width (pixels)", &imageWidth);
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
      ImGui::InputDouble("Image Height (pixels)", &imageHeight);

      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !cameraVideoPath.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        model = cameracalibration::calibrate(
            cameraVideoPath, markerWidth, boardWidth, boardHeight, imageWidth,
            imageHeight, showDebug);
        calibrateButtonPressed = true;
      }
    } else {
      ImGui::Separator();
      if (ImGui::Button("Calibrate") && !cameraVideoPath.empty()) {
        std::cout << "calibration button pressed" << std::endl;
        model = cameracalibration::calibrate(cameraVideoPath, squareWidth,
                                             markerWidth, boardWidth,
                                             boardHeight, showDebug);
        calibrateButtonPressed = true;
      }
    }
    if (calibrateButtonPressed && model) {
      gCameraModel = *model;
      std::filesystem::path myPath(cameraVideoPath);
      auto outputPath = myPath.parent_path() / "cameracalibration.json";

      std::ofstream output_file(outputPath);
      output_file << wpi::json(gCameraModel).dump(4) << std::endl;
      ImGui::CloseCurrentPopup();
      calibrateButtonPressed = false;
    } else if (calibrateButtonPressed && !model) {
      std::cout << "calibration failed and popup ready" << std::endl;
      ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
      ImGui::OpenPopup("Camera Calibration Error");
      calibrateButtonPressed = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void CombineCalibrations() {
  static std::unique_ptr<pfd::open_file> calibratedFieldLayoutMultiselector;
  static std::map<std::string, frc::AprilTagFieldLayout> calibratedFieldLayouts;
  static std::unique_ptr<pfd::select_folder> saveDirSelector;
  static std::vector<frc::AprilTag> tags;
  // Maps tag IDs to paths to JSON files containing field layouts
  static std::map<int, std::string> combinerMap;
  static int currentCombinerTagId = 0;

  if (ImGui::BeginPopupModal("Combine Calibrations", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    IdealFieldSelectorButton("Select Ideal Map");

    if (ImGui::Button("Select Field Calibrations")) {
      calibratedFieldLayoutMultiselector = std::make_unique<pfd::open_file>(
          "Select File", "", std::vector<std::string>{"JSON", "*.json"},
          pfd::opt::multiselect);
    }
    if (calibratedFieldLayoutMultiselector &&
        calibratedFieldLayoutMultiselector->ready(0)) {
      auto selectedFiles = calibratedFieldLayoutMultiselector->result();
      if (!selectedFiles.empty()) {
        std::map<std::string, frc::AprilTagFieldLayout> fieldLayouts;
        for (auto& path : selectedFiles) {
          try {
            fieldLayouts.emplace(path, wpi::json::parse(std::ifstream(path))
                                           .get<frc::AprilTagFieldLayout>());
          } catch (...) {
            gInvalidLayoutPath = path;
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
            ImGui::OpenPopup("AprilTag Field Layout Loading Error");
          }
        }
        calibratedFieldLayouts = fieldLayouts;
      }
      calibratedFieldLayoutMultiselector.reset();
    }

    if (gIdealFieldLayout.GetTags().size() > 0 &&
        !calibratedFieldLayouts.empty()) {
      for (auto& [file, layout] : calibratedFieldLayouts) {
        std::vector<int> tagIds;
        for (auto& tags : layout.GetTags()) {
          tagIds.push_back(tags.ID);
        }
        auto text = fmt::format("{} tags: {}",
                                std::filesystem::path(file).filename().string(),
                                fmt::join(tagIds, ", "));
        ImGui::Selectable(text.c_str(), false,
                          ImGuiSelectableFlags_DontClosePopups);
        if (ImGui::BeginDragDropSource()) {
          ImGui::SetDragDropPayload("FieldCalibration", file.c_str(),
                                    file.size());
          ImGui::TextUnformatted(file.c_str());
          ImGui::EndDragDropSource();
        }
      }

      // If a JSON field layout gets dragged over to a tag, that tag's data will
      // be pulled from the dragged JSON field layout
      for (auto& [tagId, filePath] : combinerMap) {
        if (!filePath.empty()) {
          auto text = fmt::format("Tag ID {}: {}", tagId, filePath);
          ImGui::TextUnformatted(text.c_str());
        } else {
          ImGui::Text("Tag ID %i: <none (DROP HERE)>", tagId);
        }
        if (ImGui::BeginDragDropTarget()) {
          if (const ImGuiPayload* payload =
                  ImGui::AcceptDragDropPayload("FieldCalibration")) {
            std::string path(static_cast<char*>(payload->Data),
                             payload->DataSize);
            if (calibratedFieldLayouts.contains(path) &&
                calibratedFieldLayouts[path].GetTagPose(tagId)) {
              filePath = path;
            } else {
              ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
              ImGui::OpenPopup("Tag ID Not In Field");
            }
          }
          ImGui::EndDragDropTarget();
        }
      }
      MissingTagInField();

      // Allow users to add tags to the combined field layout
      ImGui::InputInt("Tag ID", &currentCombinerTagId);
      ImGui::SameLine();
      // Only allow tag IDs that are present in the ideal field layout
      if (ImGui::Button("Add") &&
          gIdealFieldLayout.GetTagPose(currentCombinerTagId)) {
        // Empty string, indicates no selected layout to get tag data from
        combinerMap.emplace(currentCombinerTagId, "");
      }
      ImGui::SameLine();
      if (ImGui::Button("Remove")) {
        combinerMap.erase(currentCombinerTagId);
      }
    }
    ImGui::Separator();
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Download")) {
      tags.clear();
      for (auto& [tagId, layoutPath] : combinerMap) {
        if (layoutPath != "") {
          auto tagPose = calibratedFieldLayouts[layoutPath].GetTagPose(tagId);
          if (tagPose) {
            tags.emplace_back(tagId, tagPose.value());
          }
        }
      }
      saveDirSelector =
          std::make_unique<pfd::select_folder>("Select Download Directory", "");
    }
    SaveCalibratedField({tags, gIdealFieldLayout.GetFieldLength(),
                         gIdealFieldLayout.GetFieldWidth()},
                        "combined_calibration", saveDirSelector);
    ImGui::EndPopup();
  }
}

void VisualizeCalibration() {
  static int focusedTag = 1;
  static int referenceTag = 1;
  static std::unique_ptr<pfd::open_file> calibratedFieldLayoutSelector;
  static frc::AprilTagFieldLayout currentCalibrationLayout;
  if (ImGui::BeginPopupModal("Visualize Calibration", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    FieldSelectorButton("Select Calibrated Field Layout",
                        calibratedFieldLayoutSelector,
                        currentCalibrationLayout);
    IdealFieldSelectorButton("Select Ideal Field Layout");

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Focused Tag", &focusedTag);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
    ImGui::InputInt("Reference Tag", &referenceTag);

    if (currentCalibrationLayout.GetTags().size() > 0 &&
        gIdealFieldLayout.GetTags().size() > 0) {
      if (currentCalibrationLayout.GetTags().size() !=
          gIdealFieldLayout.GetTags().size()) {
        ImGui::TextWrapped(
            "The number of tags in the calibration output and the ideal field "
            "layout do not match. Please ensure that the calibration output "
            "and ideal field layout have the same number of tags.");
      } else if (gIdealFieldLayout.GetTagPose(focusedTag) &&
                 gIdealFieldLayout.GetTagPose(referenceTag)) {
        auto referenceFocusedTagPose =
            gIdealFieldLayout.GetTagPose(focusedTag).value();
        auto calibrationFocusedTagPose =
            currentCalibrationLayout.GetTagPose(focusedTag).value();
        auto calibrationReferenceTagPose =
            currentCalibrationLayout.GetTagPose(referenceTag).value();

        auto diff = referenceFocusedTagPose - calibrationFocusedTagPose;

        auto ref = calibrationReferenceTagPose.Translation() -
                   calibrationFocusedTagPose.Translation();

        ImGui::TextWrapped("X Difference: %f (m)", diff.X().value());
        ImGui::TextWrapped("Y Difference: %f (m)", diff.Y().value());
        ImGui::TextWrapped("Z Difference: %f (m)", diff.Z().value());

        ImGui::TextWrapped("Yaw Difference %f°", diff.Rotation().Z().value());
        ImGui::TextWrapped("Pitch Difference %f°", diff.Rotation().Y().value());
        ImGui::TextWrapped("Roll Difference %f°", diff.Rotation().X().value());

        ImGui::NewLine();

        ImGui::TextWrapped("X Reference: %f (m)", ref.X().value());
        ImGui::TextWrapped("Y Reference: %f (m)", ref.Y().value());
        ImGui::TextWrapped("Z Reference: %f (m)", ref.Z().value());
      } else {
        ImGui::TextWrapped(
            "Please select tags that are in the ideal and calibrated field "
            "layout");
      }
    }

    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
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

  static frc::AprilTagFieldLayout calibratedFieldLayout;
  static std::unique_ptr<pfd::select_folder> saveDirSelector;
  static std::unique_ptr<pfd::select_folder> fieldVideoDirSelector;
  static std::string fieldVideoDir;
  static int pinnedTag = 1;
  static int maxFRCTag = 22;

  // camera matrix selector button
  CameraCalibrationSelectorButton("Select Camera Intrinsics JSON",
                                  gCameraModel);

  ImGui::SameLine();
  ImGui::Text("Or");
  ImGui::SameLine();

  // camera calibration button
  if (ImGui::Button("Calibrate Camera")) {
    ImGui::OpenPopup("Camera Calibration");
  }

  if (gCameraModel.avgReprojectionError != -1) {
    DrawCheck();
  }

  IdealFieldSelectorButton("Select Field Layout JSON");

  // field calibration directory selector button
  SelectDirectoryButton("Select Field Calibration Directory",
                        fieldVideoDirSelector, fieldVideoDir);

  if (!fieldVideoDir.empty()) {
    DrawCheck();
  }

  auto isMissingInputs = fieldVideoDir.empty() ||
                         gCameraModel.avgReprojectionError == -1 ||
                         gIdealFieldLayout.GetTags().size() == 0;
  // pinned tag text field
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputInt("Pinned Tag", &pinnedTag);

  if (ImGui::Button("Calibrate!!!")) {
    if (!isMissingInputs) {
      auto layout = fieldcalibration::calibrate(
          fieldVideoDir, gCameraModel, gIdealFieldLayout, pinnedTag, showDebug);
      if (layout) {
        calibratedFieldLayout = *layout;
        saveDirSelector = std::make_unique<pfd::select_folder>(
            "Select Download Directory", "");
      } else {
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        ImGui::OpenPopup("Field Calibration Error");
      }
    }
  }
  SaveCalibratedField(calibratedFieldLayout, "field_calibration",
                      saveDirSelector);
  if (ImGui::Button("Visualize")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Visualize Calibration");
  }
  if (ImGui::Button("Combine Calibrations")) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
    ImGui::OpenPopup("Combine Calibrations");
  }
  if (isMissingInputs) {
    ImGui::TextWrapped(
        "Some inputs are empty! Please enter your camera calibration video, "
        "field layout, and field calibration directory");
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
    ImGui::Text("Field Calibration Failed - please try again, ensuring that:");
    ImGui::TextWrapped(
        "- You have selected the correct camera intrinsics JSON or camera "
        "calibration video");
    ImGui::TextWrapped(
        "- You have selected the correct ideal field layout JSON");
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

  if (ImGui::BeginPopupModal("Camera Calibration Loading Error", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Could not load camera calibration JSON. Make sure that:");
    ImGui::TextWrapped("- Your camera calibration is valid JSON");
    ImGui::TextWrapped("- The camera matrix is either 3x3 or a 9 item array");
    ImGui::TextWrapped("- There's an array for distortion");
    ImGui::TextWrapped("- There's a reprojection error");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // camera calibration popup window
  CalibrateCamera();

  VisualizeCalibration();
  CombineCalibrations();

  FieldLoadingError();
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

  gui::Initialize("wpical", 900, 600);
  gui::Main();

  gui::DestroyContext();

  return 0;
}
#endif
