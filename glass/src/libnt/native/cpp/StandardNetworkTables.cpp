/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTCommandScheduler.h"
#include "glass/networktables/NTCommandSelector.h"
#include "glass/networktables/NTDigitalInput.h"
#include "glass/networktables/NTDigitalOutput.h"
#include "glass/networktables/NTFMS.h"
#include "glass/networktables/NTField2D.h"
#include "glass/networktables/NTGyro.h"
#include "glass/networktables/NTPIDController.h"
#include "glass/networktables/NTSpeedController.h"
#include "glass/networktables/NTStringChooser.h"
#include "glass/networktables/NTSubsystem.h"
#include "glass/networktables/NetworkTablesProvider.h"

using namespace glass;

void glass::AddStandardNetworkTablesViews(NetworkTablesProvider& provider) {
  provider.Register(
      NTCommandSchedulerModel::kType,
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
        return std::make_unique<NTCommandSelectorModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView([=] {
          DisplayCommandSelector(static_cast<NTCommandSelectorModel*>(model));
        });
      });
  provider.Register(
      NTFMSModel::kType,
      [](NT_Inst inst, const char* path) {
        return std::make_unique<NTFMSModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplayFMS(static_cast<FMSModel*>(model)); });
      });
  provider.Register(
      NTDigitalInputModel::kType,
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
        return std::make_unique<NTGyroModel>(inst, path);
      },
      [](Window* win, Model* model, const char* path) {
        win->SetDefaultSize(320, 380);
        return MakeFunctionView(
            [=] { DisplayGyro(static_cast<NTGyroModel*>(model)); });
      });
  provider.Register(
      NTPIDControllerModel::kType,
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
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
      [](NT_Inst inst, const char* path) {
        return std::make_unique<NTSubsystemModel>(inst, path);
      },
      [](Window* win, Model* model, const char*) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        return MakeFunctionView(
            [=] { DisplaySubsystem(static_cast<NTSubsystemModel*>(model)); });
      });
}
