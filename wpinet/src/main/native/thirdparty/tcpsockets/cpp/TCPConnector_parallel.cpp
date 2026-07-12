// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/TCPConnector.h"  // NOLINT(build/include_order)

#include <atomic>
#include <chrono>
#include <thread>
#include <tuple>

#include "wpi/util/SmallSet.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::net;

std::unique_ptr<NetworkStream> TCPConnector::connect_parallel(
    std::span<const std::pair<const char*, int>> servers, wpi::util::Logger& logger,
    int timeout) {
  if (servers.empty()) {
    return nullptr;
  }

  // structure to make sure we don't start duplicate workers
  struct GlobalState {
    wpi::util::mutex mtx;
    wpi::util::SmallSet<std::tuple<std::thread::id, std::string, int>, 16> active;
  };
  static auto global = std::make_shared<GlobalState>();
  auto this_id = std::this_thread::get_id();
  auto local = global;  // copy to an automatic variable for lambda capture

  // structure shared between threads and this function
  struct Result {
    wpi::util::mutex mtx;
    wpi::util::condition_variable cv;
    std::unique_ptr<NetworkStream> stream;
    std::atomic<unsigned int> count{0};
    std::atomic<bool> done{false};
  };
  auto result = std::make_shared<Result>();

  // start worker threads; this is I/O bound so we don't limit to # of procs
  wpi::util::Logger* plogger = &logger;
  unsigned int num_workers = 0;
  for (const auto& server : servers) {
    std::pair<std::string, int> server_copy{std::string{server.first},
                                            server.second};
    std::tuple<std::thread::id, std::string, int> active_tracker{
        this_id, server_copy.first, server_copy.second};

    // don't start a new worker if we had a previously still-active connection
    // attempt to the same server
    {
      std::scoped_lock lock(local->mtx);
      if (local->active.count(active_tracker) > 0) {
        continue;  // already in set
      }
    }

    ++num_workers;

    // start the worker
    std::thread([=] {
      if (!result->done) {
        // add to global state
        {
          std::scoped_lock lock(local->mtx);
          local->active.insert(active_tracker);
        }

        // try to connect
        auto stream = connect(server_copy.first.c_str(), server_copy.second,
                              *plogger, timeout);

        // remove from global state
        {
          std::scoped_lock lock(local->mtx);
          local->active.erase(active_tracker);
        }

        // successful connection
        if (stream) {
          std::scoped_lock lock(result->mtx);
          if (!result->done.exchange(true)) {
            result->stream = std::move(stream);
          }
        }
      }
      ++result->count;
      result->cv.notify_all();
    }).detach();
  }

  // wait for a result, timeout, or all finished
  std::unique_lock lock(result->mtx);
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
