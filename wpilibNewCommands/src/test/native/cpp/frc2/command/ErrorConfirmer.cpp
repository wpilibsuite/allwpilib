/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
