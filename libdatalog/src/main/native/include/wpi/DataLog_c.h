// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>  // NOLINT

#include <stdint.h>
#include <wpi/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** C-compatible data log (opaque struct). */
struct WPI_DataLog;

/**
 * Construct a new Data Log.
 *
 * @param filename filename to use
 * @param errorCode error if file failed to open (output)
 * @param extraHeader extra header data
 */
struct WPI_DataLog* WPI_DataLog_CreateWriter(
    const struct WPI_String* filename, int* errorCode,
    const struct WPI_String* extraHeader);

/**
 * Construct a new Data Log background writer.  The log will be initially
 * created with a temporary filename.
 *
 * @param dir directory to store the log
 * @param filename filename to use; if none provided, a random filename is
 *                 generated of the form "wpilog_{}.wpilog"
 * @param period time between automatic flushes to disk, in seconds;
 *               this is a time/storage tradeoff
 * @param extraHeader extra header data
 */
struct WPI_DataLog* WPI_DataLog_CreateBackgroundWriter(
    const struct WPI_String* dir, const struct WPI_String* filename,
    double period, const struct WPI_String* extraHeader);

/**
 * Construct a new Data Log background writer that passes its output to the
 * provided function rather than a file.  The write function will be called on a
 * separate background thread and may block.  The write function is called with
 * an empty data array (data=NULL, len=0) when the thread is terminating.
 *
 * @param write write function
 * @param ptr pointer to pass to write function ptr parameter
 * @param period time between automatic calls to write, in seconds;
 *               this is a time/storage tradeoff
 * @param extraHeader extra header data
 */
struct WPI_DataLog* WPI_DataLog_CreateBackgroundWriter_Func(
    void (*write)(void* ptr, const uint8_t* data, size_t len), void* ptr,
    double period, const struct WPI_String* extraHeader);

/**
 * Change log filename.  Can only be used on background writer data logs.
 *
 * @param datalog data log
 * @param filename filename
 */
void WPI_DataLog_SetBackgroundWriterFilename(struct WPI_DataLog* datalog,
                                             const struct WPI_String* filename);

/**
 * Releases a data log object. Closes the file and returns resources to the
 * system.
 *
 * @param datalog data log
 */
void WPI_DataLog_Release(struct WPI_DataLog* datalog);

/**
 * Explicitly flushes the log data to disk.
 *
 * @param datalog data log
 */
void WPI_DataLog_Flush(struct WPI_DataLog* datalog);

/**
 * Pauses appending of data records to the log.  While paused, no data records
 * are saved (e.g. AppendX is a no-op).  Has no effect on entry starts /
 * finishes / metadata changes.
 *
 * @param datalog data log
 */
void WPI_DataLog_Pause(struct WPI_DataLog* datalog);

/**
 * Resumes appending of data records to the log.  If called after Stop(),
 * opens a new file (with random name if SetFilename was not called after
 * Stop()) and appends Start records and schema data values for all previously
 * started entries and schemas.
 *
 * @param datalog data log
 */
void WPI_DataLog_Resume(struct WPI_DataLog* datalog);

/**
 * Stops appending all records to the log, and closes the log file.
 *
 * @param datalog data log
 */
void WPI_DataLog_Stop(struct WPI_DataLog* datalog);

/**
 * Start an entry.  Duplicate names are allowed (with the same type), and
 * result in the same index being returned (Start/Finish are reference
 * counted).  A duplicate name with a different type will result in an error
 * message being printed to the console and 0 being returned (which will be
 * ignored by the Append functions).
 *
 * @param datalog data log
 * @param name Name
 * @param type Data type
 * @param metadata Initial metadata (e.g. data properties)
 * @param timestamp Time stamp (may be 0 to indicate now)
 *
 * @return Entry index
 */
int WPI_DataLog_Start(struct WPI_DataLog* datalog,
                      const struct WPI_String* name,
                      const struct WPI_String* type,
                      const struct WPI_String* metadata, int64_t timestamp);

/**
 * Finish an entry.
 *
 * @param datalog data log
 * @param entry Entry index
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_Finish(struct WPI_DataLog* datalog, int entry,
                        int64_t timestamp);

/**
 * Updates the metadata for an entry.
 *
 * @param datalog data log
 * @param entry Entry index
 * @param metadata New metadata for the entry
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_SetMetadata(struct WPI_DataLog* datalog, int entry,
                             const struct WPI_String* metadata,
                             int64_t timestamp);

/**
 * Appends a raw record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param data Byte array to record
 * @param len Length of byte array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendRaw(struct WPI_DataLog* datalog, int entry,
                           const uint8_t* data, size_t len, int64_t timestamp);

/**
 * Appends a boolean record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Boolean value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBoolean(struct WPI_DataLog* datalog, int entry,
                               int value, int64_t timestamp);

/**
 * Appends an integer record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Integer value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendInteger(struct WPI_DataLog* datalog, int entry,
                               int64_t value, int64_t timestamp);

/**
 * Appends a float record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Float value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendFloat(struct WPI_DataLog* datalog, int entry,
                             float value, int64_t timestamp);

/**
 * Appends a double record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value Double value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendDouble(struct WPI_DataLog* datalog, int entry,
                              double value, int64_t timestamp);

/**
 * Appends a string record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param value String value to record
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendString(struct WPI_DataLog* datalog, int entry,
                              const struct WPI_String* value,
                              int64_t timestamp);

/**
 * Appends a boolean array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Boolean array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBooleanArray(struct WPI_DataLog* datalog, int entry,
                                    const int* arr, size_t len,
                                    int64_t timestamp);

/**
 * Appends a boolean array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Boolean array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendBooleanArrayByte(struct WPI_DataLog* datalog, int entry,
                                        const uint8_t* arr, size_t len,
                                        int64_t timestamp);

/**
 * Appends an integer array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Integer array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendIntegerArray(struct WPI_DataLog* datalog, int entry,
                                    const int64_t* arr, size_t len,
                                    int64_t timestamp);

/**
 * Appends a float array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Float array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendFloatArray(struct WPI_DataLog* datalog, int entry,
                                  const float* arr, size_t len,
                                  int64_t timestamp);

/**
 * Appends a double array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr Double array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendDoubleArray(struct WPI_DataLog* datalog, int entry,
                                   const double* arr, size_t len,
                                   int64_t timestamp);

/**
 * Appends a string array record to the log.
 *
 * @param datalog data log
 * @param entry Entry index, as returned by WPI_DataLog_Start()
 * @param arr String array to record
 * @param len Number of elements in array
 * @param timestamp Time stamp (may be 0 to indicate now)
 */
void WPI_DataLog_AppendStringArray(struct WPI_DataLog* datalog, int entry,
                                   const struct WPI_String* arr, size_t len,
                                   int64_t timestamp);

void WPI_DataLog_AddSchemaString(struct WPI_DataLog* datalog,
                                 const struct WPI_String* name,
                                 const struct WPI_String* type,
                                 const struct WPI_String* schema,
                                 int64_t timestamp);

void WPI_DataLog_AddSchema(struct WPI_DataLog* datalog,
                           const struct WPI_String* name,
                           const struct WPI_String* type, const uint8_t* schema,
                           size_t schema_len, int64_t timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif
