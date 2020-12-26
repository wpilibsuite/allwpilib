// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

/** HAL data types. */
enum HAL_Type {
  HAL_UNASSIGNED = 0,
  HAL_BOOLEAN = 0x01,
  HAL_DOUBLE = 0x02,
  HAL_ENUM = 0x04,
  HAL_INT = 0x08,
  HAL_LONG = 0x10,
};

/** HAL Entry Value.  Note this is a typed union. */
struct HAL_Value {
  union {
    HAL_Bool v_boolean;
    int32_t v_enum;
    int32_t v_int;
    int64_t v_long;
    double v_double;
  } data;
  enum HAL_Type type;
};

#ifdef __cplusplus
extern "C" {
#endif

inline struct HAL_Value HAL_MakeBoolean(HAL_Bool v) {
  struct HAL_Value value;
  value.type = HAL_BOOLEAN;
  value.data.v_boolean = v;
  return value;
}

inline struct HAL_Value HAL_MakeEnum(int v) {
  struct HAL_Value value;
  value.type = HAL_ENUM;
  value.data.v_enum = v;
  return value;
}

inline struct HAL_Value HAL_MakeInt(int v) {
  struct HAL_Value value;
  value.type = HAL_INT;
  value.data.v_int = v;
  return value;
}

inline struct HAL_Value HAL_MakeLong(int64_t v) {
  struct HAL_Value value;
  value.type = HAL_LONG;
  value.data.v_long = v;
  return value;
}

inline struct HAL_Value HAL_MakeDouble(double v) {
  struct HAL_Value value;
  value.type = HAL_DOUBLE;
  value.data.v_double = v;
  return value;
}

#ifdef __cplusplus
}  // extern "C"
#endif
