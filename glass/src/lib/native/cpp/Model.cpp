// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Model.h"

#include "glass/ContextInternal.h"

using namespace glass;

bool Model::IsReadOnly() {
  return false;
}

Model* glass::AddModel(std::unique_ptr<Model> model) {
  return gContext->models.emplace_back(std::move(model)).get();
}

void glass::UpdateModels() {
  for (auto&& model : gContext->models) {
    model->Update();
  }
}
