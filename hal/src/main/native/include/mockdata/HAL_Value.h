/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include "hal/Types.h"

/** HAL data types. */
enum HAL_Type {
  HAL_UNASSIGNED = 0,
  HAL_BOOLEAN = 0x01,
  HAL_DOUBLE = 0x02,
  HAL_ENUM = 0x16,
  HAL_INT = 0x32,
  HAL_LONG = 0x64,
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

inline HAL_Value MakeBoolean(HAL_Bool v) {
  HAL_Value value;
  value.type = HAL_BOOLEAN;
  value.data.v_boolean = v;
  return value;
}

inline HAL_Value MakeEnum(int v) {
  HAL_Value value;
  value.type = HAL_ENUM;
  value.data.v_enum = v;
  return value;
}

inline HAL_Value MakeInt(int v) {
  HAL_Value value;
  value.type = HAL_INT;
  value.data.v_int = v;
  return value;
}

inline HAL_Value MakeLong(int64_t v) {
  HAL_Value value;
  value.type = HAL_LONG;
  value.data.v_long = v;
  return value;
}

inline HAL_Value MakeDouble(double v) {
  HAL_Value value;
  value.type = HAL_DOUBLE;
  value.data.v_double = v;
  return value;
}

#endif
