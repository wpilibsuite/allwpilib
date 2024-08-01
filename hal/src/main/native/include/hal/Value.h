// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

/** HAL data types. */
enum HAL_Type {
  /** Unassigned type. */
  HAL_UNASSIGNED = 0,
  /** Boolean. */
  HAL_BOOLEAN = 0x01,
  /** Double. */
  HAL_DOUBLE = 0x02,
  /** Enum. */
  HAL_ENUM = 0x04,
  /** Int. */
  HAL_INT = 0x08,
  /** Long. */
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
/**
 * Build a HAL boolean value.
 *
 * @param v value
 * @return HAL value
 */
inline struct HAL_Value HAL_MakeBoolean(HAL_Bool v) {
  struct HAL_Value value;
  value.type = HAL_BOOLEAN;
  value.data.v_boolean = v;
  return value;
}

/**
 * Build a HAL enum value.
 *
 * @param v value
 * @return HAL value
 */
inline struct HAL_Value HAL_MakeEnum(int v) {
  struct HAL_Value value;
  value.type = HAL_ENUM;
  value.data.v_enum = v;
  return value;
}

/**
 * Build a HAL int value.
 *
 * @param v value
 * @return HAL value
 */
inline struct HAL_Value HAL_MakeInt(int v) {
  struct HAL_Value value;
  value.type = HAL_INT;
  value.data.v_int = v;
  return value;
}

/**
 * Build a HAL long value.
 *
 * @param v value
 * @return HAL value
 */
inline struct HAL_Value HAL_MakeLong(int64_t v) {
  struct HAL_Value value;
  value.type = HAL_LONG;
  value.data.v_long = v;
  return value;
}

/**
 * Build a HAL double value.
 *
 * @param v value
 * @return HAL value
 */
inline struct HAL_Value HAL_MakeDouble(double v) {
  struct HAL_Value value;
  value.type = HAL_DOUBLE;
  value.data.v_double = v;
  return value;
}

#ifdef __cplusplus
}  // extern "C"
#endif
