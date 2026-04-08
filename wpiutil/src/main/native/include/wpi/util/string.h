// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>  // NOLINT

/**
 * A const UTF8 string.
 *
 * Notes on usage:
 *   1. WPILib will not have any APIs that manipulate an externally allocated
 *      string.
 *   2. If a WPILib API takes a `const WPI_String*`, that string is an input.
 *      WPILib will not manipulate or attempt to free that string. It is up to
 *      the caller to manage the memory- WPILib will never hold onto that memory
 *      longer than the call.
 *   3. If a WPILib API takes a `WPI_String*`, that string is an output. WPILib
 *      will allocate that string with WPI_AllocateString(), fill in the string,
 *      and return to the caller. When the caller is done with the string, they
 *      must free it with WPI_FreeString().
 *   4. If an input struct containing a WPI_String, or an input array of
 *      [WPI_String](@ref WPI_String)s is passed to WPILib, the individual
 *      strings will not be manipulated or freed by WPILib, and the caller owns
 *      and should free that memory.
 *   5. If an output struct contains a WPI_String member, that member is
 *      considered read only, and should not be explicitly freed individually.
 *      The caller should call the free function for that struct.
 *   6. If an array of [WPI_String](@ref WPI_String)s is returned, each
 *      individual string is considered read only, and should not be explicitly
 *      freed individually. The caller should call the free function for that
 *      array.
 *   7. Callbacks also follow these rules. The most common situation is a
 *      callback either getting passed a `const WPI_String*` or a struct
 *      containing a WPI_String. In both of these cases, the callback target
 *      should consider these strings read only, and not attempt to free them or
 *      manipulate them.
 */
struct WPI_String {
  /** Contents. */
  const char* str;
  /** Length */
  size_t len;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a WPI_String from a null terminated UTF-8 string.
 * If input string is null, initializes output to 0 length.
 * The output length does not include the null terminator.
 *
 * The lifetime of the output string is the lifetime of the input string.
 * Do not call WPI_FreeString() with the output of this call.
 *
 * @param wpiString output string
 * @param utf8String input string (null terminated)
 */
void WPI_InitString(struct WPI_String* wpiString, const char* utf8String);

/**
 * Initializes a WPI_String from a UTF-8 string and length.
 * If input string is null or 0 length, initializes output to 0 length.
 * The input string does not need to be null terminated.
 *
 * The lifetime of the output string is the lifetime of the input string.
 * Do not call WPI_FreeString() with the output of this call.
 *
 * @param wpiString output string
 * @param utf8String input string
 * @param length input string length in chars
 */
void WPI_InitStringWithLength(struct WPI_String* wpiString,
                              const char* utf8String, size_t length);

/**
 * Allocates a WPI_String for the specified length.
 * The resultant string must be freed with WPI_FreeString().
 *
 * @param wpiString output string
 * @param length string length in chars to allocate
 * @return mutable pointer to allocated buffer
 *
 */
char* WPI_AllocateString(struct WPI_String* wpiString, size_t length);

/**
 * Frees a WPI_String that was allocated with WPI_AllocateString()
 *
 * @param wpiString string to free
 */
void WPI_FreeString(const struct WPI_String* wpiString);

/**
 * Allocates an array of WPI_Strings.
 *
 * @param length array length
 * @return string array
 */
struct WPI_String* WPI_AllocateStringArray(size_t length);

/**
 * Frees a WPI_String array returned by WPI_AllocateStringArray().
 *
 * @param wpiStringArray string array to free
 * @param length length of array
 */
void WPI_FreeStringArray(const struct WPI_String* wpiStringArray,
                         size_t length);

#ifdef __cplusplus
}  // extern "C"
#endif
