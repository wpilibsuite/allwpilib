// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "wpi/glass/networktables/NTAlerts.hpp"
#include "wpi/glass/networktables/NTCommandScheduler.hpp"
#include "wpi/glass/networktables/NTCommandSelector.hpp"
#include "wpi/glass/networktables/NTDS.hpp"
#include "wpi/glass/networktables/NTDifferentialDrive.hpp"
#include "wpi/glass/networktables/NTDigitalInput.hpp"
#include "wpi/glass/networktables/NTDigitalOutput.hpp"
#include "wpi/glass/networktables/NTField2D.hpp"
#include "wpi/glass/networktables/NTGyro.hpp"
#include "wpi/glass/networktables/NTMecanumDrive.hpp"
#include "wpi/glass/networktables/NTMechanism2D.hpp"
#include "wpi/glass/networktables/NTMotorController.hpp"
#include "wpi/glass/networktables/NTPIDController.hpp"
#include "wpi/glass/networktables/NTProfiledPIDController.hpp"
#include "wpi/glass/networktables/NTSelectable.hpp"
#include "wpi/glass/networktables/NTSubsystem.hpp"
#include "wpi/glass/networktables/NetworkTablesProvider.hpp"

using namespace wpi::glass;

void wpi::glass::AddStandardNetworkTablesViews(
    NetworkTablesProvider& provider) {
  provider.Register(
      NTAlertsModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTAlertsModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(300, 150);
        return MakeFunctionView(
            [=] { DisplayAlerts(static_cast<NTAlertsModel*>(model)); });
      });
  provider.Register(
      NTCommandSchedulerModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTCommandSchedulerModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(400, 200);
        return MakeFunctionView([=] {
          DisplayCommandScheduler(static_cast<NTCommandSchedulerModel*>(model));
        });
      });
  provider.Register(
      NTCommandSelectorModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTCommandSelectorModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayCommandSelector(static_cast<NTCommandSelectorModel*>(model));
        });
      });
  provider.Register(
      NTDifferentialDriveModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDifferentialDriveModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(300, 350);
        return MakeFunctionView([=] {
          DisplayDrive(static_cast<NTDifferentialDriveModel*>(model));
        });
      });
  provider.Register(
      NTDSModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDSModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplayDS(static_cast<DSModel*>(model), true); });
      });
  provider.Register(
      NTDigitalInputModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDigitalInputModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayDIO(static_cast<NTDigitalInputModel*>(model), 0, true);
        });
      });
  provider.Register(
      NTDigitalOutputModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTDigitalOutputModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayDIO(static_cast<NTDigitalOutputModel*>(model), 0, true);
        });
      });
  provider.Register(
      NTField2DModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
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
      NTGyroModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTGyroModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetDefaultSize(320, 380);
        return MakeFunctionView(
            [=] { DisplayGyro(static_cast<NTGyroModel*>(model)); });
      });
  provider.Register(
      NTMecanumDriveModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTMecanumDriveModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetDefaultSize(300, 350);
        return MakeFunctionView(
            [=] { DisplayDrive(static_cast<NTMecanumDriveModel*>(model)); });
      });
  provider.Register(
      NTMechanism2DModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
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
      NTPIDControllerModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTPIDControllerModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayPIDController(static_cast<NTPIDControllerModel*>(model));
        });
      });
  provider.Register(
      NTProfiledPIDControllerModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTProfiledPIDControllerModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayProfiledPIDController(
              static_cast<NTProfiledPIDControllerModel*>(model));
        });
      });
  provider.Register(
      NTMotorControllerModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTMotorControllerModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayMotorController(static_cast<NTMotorControllerModel*>(model));
        });
      });
  provider.Register(
      NTSelectableModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTSelectableModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplaySelectable(static_cast<NTSelectableModel*>(model)); });
      });
  provider.Register(
      NTSubsystemModel::TYPE,
      [](wpi::nt::NetworkTableInstance inst, const char* path) {
        return std::make_unique<NTSubsystemModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplaySubsystem(static_cast<NTSubsystemModel*>(model)); });
      });
}
