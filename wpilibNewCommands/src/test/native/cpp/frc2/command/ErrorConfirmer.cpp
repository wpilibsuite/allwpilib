// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ErrorConfirmer.h"

#include <regex>

ErrorConfirmer* ErrorConfirmer::instance;

int32_t ErrorConfirmer::HandleError(HAL_Bool isError, int32_t errorCode,
                                    HAL_Bool isLVCode, const char* details,
                                    const char* location, const char* callStack,
                                    HAL_Bool printMsg) {
  if (std::regex_search(details, std::regex(instance->m_msg))) {
    instance->ConfirmError();
  }
  return 1;
}
