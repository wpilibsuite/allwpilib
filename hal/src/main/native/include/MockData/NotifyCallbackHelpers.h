/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "MockData/NotifyListenerVector.h"

std::shared_ptr<hal::NotifyListenerVector> RegisterCallback(
    std::shared_ptr<hal::NotifyListenerVector> currentVector, const char* name,
    HAL_NotifyCallback callback, void* param, int32_t* newUid);

std::shared_ptr<hal::NotifyListenerVector> CancelCallback(
    std::shared_ptr<hal::NotifyListenerVector> currentVector, int32_t uid);

void InvokeCallback(std::shared_ptr<hal::NotifyListenerVector> currentVector,
                    const char* name, const HAL_Value* value);

std::shared_ptr<hal::BufferListenerVector> RegisterCallback(
    std::shared_ptr<hal::BufferListenerVector> currentVector, const char* name,
    HAL_BufferCallback callback, void* param, int32_t* newUid);

std::shared_ptr<hal::BufferListenerVector> CancelCallback(
    std::shared_ptr<hal::BufferListenerVector> currentVector, int32_t uid);

void InvokeCallback(std::shared_ptr<hal::BufferListenerVector> currentVector,
                    const char* name, uint8_t* buffer, int32_t count);
