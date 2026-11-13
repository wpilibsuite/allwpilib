// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "wpi/util/string.h"

/** An independent alert observation cursor, bound to its creating backend. */
typedef struct WPI_AlertReader* WPI_AlertReaderHandle;

/** An alert operation failed. */
#define WPI_ALERT_ERROR -1

/** No synchronized observation is currently available. */
#define WPI_ALERT_NO_VALUE -3

#define WPI_ALERT_OBSERVATION_HAS_TEXT 1u
#define WPI_ALERT_OBSERVATION_HAS_ACTIVE 2u

#define WPI_ALERT_EVENT_BASELINE 0
#define WPI_ALERT_EVENT_CREATED 1
#define WPI_ALERT_EVENT_TEXT_CHANGED 2
#define WPI_ALERT_EVENT_ACTIVE_CHANGED 3
#define WPI_ALERT_EVENT_REMOVED 4

/** A system-visible alert, including inactive alerts. */
struct WPI_AlertObservation {
  /** Opaque identity. Compare the entire key; do not parse it. */
  struct WPI_String key;
  struct WPI_String group;
  struct WPI_String id;
  struct WPI_String text;
  /** Producer's monotonic activation time in nanoseconds; zero is inactive. */
  int64_t activeStartTime;
  /** Producer severity; may include values outside WPI_AlertLevel. */
  int32_t level;
  /** HAS_TEXT / HAS_ACTIVE distinguish missing fields from empty / zero. */
  uint32_t fields;
};

struct WPI_AlertEvent {
  /** One baseline record or live change, applied in array order. */
  int32_t kind;
  /**
   * Event time in nanoseconds in the reader process's monotonic clock domain
   * (undefined epoch, not wall clock). Published timestamps are translated by
   * transport clock synchronization. Without source timing, this is the local
   * observation time. Timestamps can go backwards; array order defines order.
   * BASELINE records describe state, not occurrences, and have a zero
   * timestamp.
   */
  int64_t timestamp;
  /** Full resulting record; REMOVED carries the last observed record. */
  struct WPI_AlertObservation alert;
};

/** Owned result of one consuming read. */
struct WPI_AlertEvents {
  /** Clear the consumer's old state before applying all records, even if empty.
   */
  int32_t reset;
  /** Pending history was lost; remains set until a successful read. */
  int32_t historyLost;
  struct WPI_AlertEvent* events;
  size_t count;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates an independent observation reader on the current alert backend.
 * Synchronization freezes a replacement baseline, then queues subsequent
 * changes, even before the first read. Each reader has its own bounded change
 * queue. Replacement records do not count against capacity. Backend switching
 * does not move existing readers. Observing an alert grants no mutation rights.
 *
 * @param capacity Maximum pending changes; must be positive
 * @param[out] reader New reader, or NULL on failure
 * @return 0 on success, WPI_ALERT_ERROR for invalid arguments or unsupported
 * backend
 */
int32_t WPI_CreateAlertReader(size_t capacity, WPI_AlertReaderHandle* reader);

/**
 * Destroys a reader. Previously returned results remain valid.
 * No other call on this reader may run during or after destruction.
 *
 * @param reader Reader to destroy; NULL is allowed
 */
void WPI_DestroyAlertReader(WPI_AlertReaderHandle reader);

/**
 * Nonblocking consuming read of the pending baseline followed by all queued
 * changes, copied together under one lock. When reset is set, clear the old
 * consumer state first, including when count is zero. Apply the entire result
 * in array order. BASELINE records describe the frozen state at synchronization
 * or overflow; following records preserve subsequent changes, even before the
 * first read. Reading never recaptures a baseline or discards later changes.
 * An empty successful result without reset means no pending changes.
 *
 * historyLost reports dropped history from overflow or reconnect, including
 * before the first read. It survives superseding baselines until a successful
 * read. Initial synchronization alone does not imply lost history.
 *
 * WPI_ALERT_NO_VALUE means unavailable or synchronizing: discard any old
 * synchronized view. Failed reads return a cleared result without allocations.
 * Allocation failure does not advance the cursor or clear pending flags. Calls
 * on a live reader are thread safe; concurrent consumers share its single
 * cursor. Free an earlier result before reusing it as output.
 *
 * @param reader Reader to consume
 * @param[out] result Owned result; failed reads return no allocations
 * @return 0 on success, WPI_ALERT_NO_VALUE without synchronized data, or
 * WPI_ALERT_ERROR on failure
 */
int32_t WPI_ReadAlertEvents(WPI_AlertReaderHandle reader,
                            struct WPI_AlertEvents* result);

/**
 * Frees a result and its strings, independently of backend or reader lifetime.
 * Accepts NULL, clears the result, and can be repeated on a cleared result.
 * Do not free individual strings or the event array separately.
 *
 * @param result Result to free
 */
void WPI_FreeAlertEvents(struct WPI_AlertEvents* result);

#ifdef __cplusplus
}  // extern "C"
#endif
