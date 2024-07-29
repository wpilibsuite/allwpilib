// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "ntcore.h"

// Functions in this header are to be used only for testing

#ifdef __cplusplus
extern "C" {
#endif

struct WPI_String* NT_GetStringForTesting(const char* str, int* struct_size);
// No need for free as one already exists in main library

struct NT_EntryInfo* NT_GetEntryInfoForTesting(const char* name,
                                               enum NT_Type type,
                                               unsigned int flags,
                                               uint64_t last_change,
                                               int* struct_size);

void NT_FreeEntryInfoForTesting(struct NT_EntryInfo* info);

struct NT_ConnectionInfo* NT_GetConnectionInfoForTesting(
    const char* remote_id, const char* remote_ip, unsigned int remote_port,
    uint64_t last_update, unsigned int protocol_version, int* struct_size);

void NT_FreeConnectionInfoForTesting(struct NT_ConnectionInfo* info);

struct NT_Value* NT_GetValueBooleanForTesting(uint64_t last_change, int val,
                                              int* struct_size);

struct NT_Value* NT_GetValueDoubleForTesting(uint64_t last_change, double val,
                                             int* struct_size);

struct NT_Value* NT_GetValueStringForTesting(uint64_t last_change,
                                             const char* str, int* struct_size);

struct NT_Value* NT_GetValueRawForTesting(uint64_t last_change, const char* raw,
                                          int raw_len, int* struct_size);

struct NT_Value* NT_GetValueBooleanArrayForTesting(uint64_t last_change,
                                                   const int* arr,
                                                   size_t array_len,
                                                   int* struct_size);

struct NT_Value* NT_GetValueDoubleArrayForTesting(uint64_t last_change,
                                                  const double* arr,
                                                  size_t array_len,
                                                  int* struct_size);

struct NT_Value* NT_GetValueStringArrayForTesting(uint64_t last_change,
                                                  const struct WPI_String* arr,
                                                  size_t array_len,
                                                  int* struct_size);
// No need for free as one already exists in the main library

#ifdef __cplusplus
}  // extern "C"
#endif
