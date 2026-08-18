// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int WPI_MulticastServiceAnnouncerHandle;  // NOLINT

WPI_MulticastServiceAnnouncerHandle WPI_CreateMulticastServiceAnnouncer(
    const char* serviceName, const char* serviceType, int32_t port,
    int32_t txtCount, const char** keys, const char** values);

void WPI_FreeMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

void WPI_StartMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

void WPI_StopMulticastServiceAnnouncer(
    WPI_MulticastServiceAnnouncerHandle handle);

int32_t WPI_GetMulticastServiceAnnouncerHasImplementation(
    WPI_MulticastServiceAnnouncerHandle handle);

#ifdef __cplusplus
}  // extern "C"
#endif
