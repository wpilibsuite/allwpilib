// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HAL_Quaternion {
  int64_t timestamp;
  double w;
  double x;
  double y;
  double z;
};
typedef struct HAL_Quaternion HAL_Quaternion;

struct HAL_Acceleration3d {
  int64_t timestamp;
  double x;
  double y;
  double z;
};
typedef struct HAL_Acceleration3d HAL_Acceleration3d;

struct HAL_GyroRate3d {
  int64_t timestamp;
  double x;
  double y;
  double z;
};
typedef struct HAL_GyroRate3d HAL_GyroRate3d;

struct HAL_EulerAngles3d {
  int64_t timestamp;
  double x;
  double y;
  double z;
};
typedef struct HAL_EulerAngles3d HAL_EulerAngles3d;

#ifdef __cplusplus
}  // extern "C"
#endif
