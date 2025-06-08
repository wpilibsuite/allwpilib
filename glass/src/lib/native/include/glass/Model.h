// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>

namespace glass {

class Model {
 public:
  Model() = default;
  virtual ~Model() = default;

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  virtual void Update() = 0;
  virtual bool Exists() = 0;
  virtual bool IsReadOnly();
};

Model* AddModel(std::unique_ptr<Model> model);

template <std::derived_from<Model> T, typename... Args>
inline T* CreateModel(Args&&... args) {
  return static_cast<T*>(
      AddModel(std::make_unique<T>(std::forward<Args>(args)...)));
}

void UpdateModels();

}  // namespace glass
