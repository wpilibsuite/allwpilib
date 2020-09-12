/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTDigitalInput.h"
#include "glass/networktables/NTDigitalOutput.h"
#include "glass/networktables/NTFMS.h"
#include "glass/networktables/NTField2D.h"
#include "glass/networktables/NTStringChooser.h"
#include "glass/networktables/NetworkTablesProvider.h"

using namespace glass;

void glass::AddStandardNetworkTablesViews(NetworkTablesProvider& provider) {
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
}
