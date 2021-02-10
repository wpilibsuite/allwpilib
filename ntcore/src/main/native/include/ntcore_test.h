// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NTCORE_TEST_H_
#define NTCORE_NTCORE_TEST_H_

#include <stdint.h>

#include <string>

#include "ntcore.h"

// Functions in this header are to be used only for testing

extern "C" {
struct NT_String* NT_GetStringForTesting(const char* string, int* struct_size);
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
                                                  const struct NT_String* arr,
                                                  size_t array_len,
                                                  int* struct_size);
// No need for free as one already exists in the main library

struct NT_RpcParamDef* NT_GetRpcParamDefForTesting(const char* name,
                                                   const struct NT_Value* val,
                                                   int* struct_size);

void NT_FreeRpcParamDefForTesting(struct NT_RpcParamDef* def);

struct NT_RpcResultDef* NT_GetRpcResultsDefForTesting(const char* name,
                                                      enum NT_Type type,
                                                      int* struct_size);

void NT_FreeRpcResultsDefForTesting(struct NT_RpcResultDef* def);

struct NT_RpcDefinition* NT_GetRpcDefinitionForTesting(
    unsigned int version, const char* name, size_t num_params,
    const struct NT_RpcParamDef* params, size_t num_results,
    const struct NT_RpcResultDef* results, int* struct_size);
// No need for free as one already exists in the main library

struct NT_RpcCallInfo* NT_GetRpcCallInfoForTesting(
    unsigned int rpc_id, unsigned int call_uid, const char* name,
    const char* params, size_t params_len, int* struct_size);
// No need for free as one already exists in the main library
}  // extern "C"

#endif  // NTCORE_NTCORE_TEST_H_
