/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/TCPConnector.h"  // NOLINT(build/include_order)

#include <atomic>
#include <chrono>
#include <thread>
#include <tuple>

#include "wpi/SmallSet.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"

using namespace wpi;

// MSVC < 1900 doesn't have support for thread_local
#if !defined(_MSC_VER) || _MSC_VER >= 1900
// clang check for availability of thread_local
#if !defined(__has_feature) || __has_feature(cxx_thread_local)
#define HAVE_THREAD_LOCAL
#endif
#endif

std::unique_ptr<NetworkStream> TCPConnector::connect_parallel(
    ArrayRef<std::pair<const char*, int>> servers, Logger& logger,
    int timeout) {
  if (servers.empty()) return nullptr;

  // structure to make sure we don't start duplicate workers
  struct GlobalState {
    wpi::mutex mtx;
#ifdef HAVE_THREAD_LOCAL
    SmallSet<std::pair<std::string, int>, 16> active;
#else
    SmallSet<std::tuple<std::thread::id, std::string, int>, 16> active;
#endif
  };
#ifdef HAVE_THREAD_LOCAL
  thread_local auto global = std::make_shared<GlobalState>();
#else
  static auto global = std::make_shared<GlobalState>();
  auto this_id = std::this_thread::get_id();
#endif
  auto local = global;  // copy to an automatic variable for lambda capture

  // structure shared between threads and this function
  struct Result {
    wpi::mutex mtx;
    wpi::condition_variable cv;
    std::unique_ptr<NetworkStream> stream;
    std::atomic<unsigned int> count{0};
    std::atomic<bool> done{false};
  };
  auto result = std::make_shared<Result>();

  // start worker threads; this is I/O bound so we don't limit to # of procs
  Logger* plogger = &logger;
  unsigned int num_workers = 0;
  for (const auto& server : servers) {
    std::pair<std::string, int> server_copy{std::string{server.first},
                                            server.second};
#ifdef HAVE_THREAD_LOCAL
    const auto& active_tracker = server_copy;
#else
    std::tuple<std::thread::id, std::string, int> active_tracker{
        this_id, server_copy.first, server_copy.second};
#endif

    // don't start a new worker if we had a previously still-active connection
    // attempt to the same server
    {
      std::lock_guard<wpi::mutex> lock(local->mtx);
      if (local->active.count(active_tracker) > 0) continue;  // already in set
    }

    ++num_workers;

    // start the worker
    std::thread([=]() {
      if (!result->done) {
        // add to global state
        {
          std::lock_guard<wpi::mutex> lock(local->mtx);
          local->active.insert(active_tracker);
        }

        // try to connect
        auto stream = connect(server_copy.first.c_str(), server_copy.second,
                              *plogger, timeout);

        // remove from global state
        {
          std::lock_guard<wpi::mutex> lock(local->mtx);
          local->active.erase(active_tracker);
        }

        // successful connection
        if (stream) {
          std::lock_guard<wpi::mutex> lock(result->mtx);
          if (!result->done.exchange(true)) result->stream = std::move(stream);
        }
      }
      ++result->count;
      result->cv.notify_all();
    })
        .detach();
  }

  // wait for a result, timeout, or all finished
  std::unique_lock<wpi::mutex> lock(result->mtx);
  if (timeout == 0) {
    result->cv.wait(
        lock, [&] { return result->stream || result->count >= num_workers; });
  } else {
    auto timeout_time =
        std::chrono::steady_clock::now() + std::chrono::seconds(timeout);
    result->cv.wait_until(lock, timeout_time, [&] {
      return result->stream || result->count >= num_workers;
    });
  }

  // no need to wait for remaining worker threads; shared_ptr will clean up
  return std::move(result->stream);
}
