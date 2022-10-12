// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTCommandScheduler.h"
#include "glass/networktables/NTCommandSelector.h"
#include "glass/networktables/NTDifferentialDrive.h"
#include "glass/networktables/NTDigitalInput.h"
#include "glass/networktables/NTDigitalOutput.h"
#include "glass/networktables/NTFMS.h"
#include "glass/networktables/NTField2D.h"
#include "glass/networktables/NTGyro.h"
#include "glass/networktables/NTMecanumDrive.h"
#include "glass/networktables/NTMechanism2D.h"
#include "glass/networktables/NTPIDController.h"
#include "glass/networktables/NTSpeedController.h"
#include "glass/networktables/NTStringChooser.h"
#include "glass/networktables/NTSubsystem.h"
#include "glass/networktables/NetworkTablesProvider.h"

using namespace glass;

void glass::AddStandardNetworkTablesViews(NetworkTablesProvider& provider) {
  provider.Register(
      NTCommandSchedulerModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTCommandSchedulerModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(400, 200);
        return MakeFunctionView([=] {
          DisplayCommandScheduler(static_cast<NTCommandSchedulerModel*>(model));
        });
      });
  provider.Register(
      NTCommandSelectorModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTCommandSelectorModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayCommandSelector(static_cast<NTCommandSelectorModel*>(model));
        });
      });
  provider.Register(
      NTDifferentialDriveModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDifferentialDriveModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(300, 350);
        return MakeFunctionView([=] {
          DisplayDrive(static_cast<NTDifferentialDriveModel*>(model));
        });
      });
  provider.Register(
      NTFMSModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTFMSModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplayFMS(static_cast<FMSModel*>(model)); });
      });
  provider.Register(
      NTDigitalInputModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDigitalInputModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayDIO(static_cast<NTDigitalInputModel*>(model), 0, true);
        });
      });
  provider.Register(
      NTDigitalOutputModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDigitalOutputModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayDIO(static_cast<NTDigitalOutputModel*>(model), 0, true);
        });
      });
  provider.Register(
      NTField2DModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTField2DModel>(inst, path);
      },
      [=](Window* win, Model* model, const char* path) {
        win->SetDefaultPos(200, 200);
        win->SetDefaultSize(400, 200);
        win->SetPadding(0, 0);
        return std::make_unique<Field2DView>(
            static_cast<NTField2DModel*>(model));
      });
  provider.Register(
      NTGyroModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTGyroModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetDefaultSize(320, 380);
        return MakeFunctionView(
            [=] { DisplayGyro(static_cast<NTGyroModel*>(model)); });
      });
  provider.Register(
      NTMecanumDriveModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTMecanumDriveModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(300, 350);
        return MakeFunctionView(
            [=] { DisplayDrive(static_cast<NTMecanumDriveModel*>(model)); });
      });
  provider.Register(
      NTMechanism2DModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTMechanism2DModel>(inst, path);
      },
      [=](Window* win, Model* model, const char* path) {
        win->SetDefaultPos(400, 400);
        win->SetDefaultSize(200, 200);
        win->SetPadding(0, 0);
        return std::make_unique<Mechanism2DView>(
            static_cast<NTMechanism2DModel*>(model));
      });
  provider.Register(
      NTPIDControllerModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTPIDControllerModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayPIDController(static_cast<NTPIDControllerModel*>(model));
        });
      });
  provider.Register(
      NTSpeedControllerModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTSpeedControllerModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplaySpeedController(static_cast<NTSpeedControllerModel*>(model));
        });
      });
  provider.Register(
      NTStringChooserModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTStringChooserModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayStringChooser(static_cast<NTStringChooserModel*>(model));
        });
      });
  provider.Register(
      NTSubsystemModel::kType,
      [](nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTSubsystemModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplaySubsystem(static_cast<NTSubsystemModel*>(model)); });
      });
}
