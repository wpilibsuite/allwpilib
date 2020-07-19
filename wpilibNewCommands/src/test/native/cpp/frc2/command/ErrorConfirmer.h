/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/simulation/MockHooks.h>

#include "gmock/gmock.h"

class ErrorConfirmer {
 public:
  explicit ErrorConfirmer(const char* msg) : m_msg(msg) {
    if (instance != nullptr) return;
    HALSIM_SetSendError(HandleError);
    EXPECT_CALL(*this, ConfirmError());
    instance = this;
  }

  ~ErrorConfirmer() {
    HALSIM_SetSendError(nullptr);
    instance = nullptr;
  }

  MOCK_METHOD0(ConfirmError, void());

  const char* m_msg;

  static int32_t HandleError(HAL_Bool isError, int32_t errorCode,
                             HAL_Bool isLVCode, const char* details,
                             const char* location, const char* callStack,
                             HAL_Bool printMsg);

 private:
  static ErrorConfirmer* instance;
};
