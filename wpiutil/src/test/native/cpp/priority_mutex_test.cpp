/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/priority_mutex.h>  // NOLINT(build/include_order)

#include <atomic>
#include <condition_variable>
#include <thread>

#include "gtest/gtest.h"

namespace wpi {

#ifdef WPI_HAVE_PRIORITY_MUTEX

using std::chrono::system_clock;

// Threading primitive used to notify many threads that a condition is now true.
// The condition can not be cleared.
class Notification {
 public:
  // Efficiently waits until the Notification has been notified once.
  void Wait() {
    std::unique_lock<priority_mutex> lock(m_mutex);
    while (!m_set) {
      m_condition.wait(lock);
    }
  }
  // Sets the condition to true, and wakes all waiting threads.
  void Notify() {
    std::lock_guard<priority_mutex> lock(m_mutex);
    m_set = true;
    m_condition.notify_all();
  }

 private:
  // priority_mutex used for the notification and to protect the bit.
  priority_mutex m_mutex;
  // Condition for threads to sleep on.
  std::condition_variable_any m_condition;
  // Bool which is true when the notification has been notified.
  bool m_set = false;
};

void SetProcessorAffinity(int32_t core_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);

  pthread_t current_thread = pthread_self();
  ASSERT_EQ(pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset),
            0);
}

void SetThreadRealtimePriorityOrDie(int32_t priority) {
  struct sched_param param;
  // Set realtime priority for this thread
  param.sched_priority = priority + sched_get_priority_min(SCHED_RR);
  ASSERT_EQ(pthread_setschedparam(pthread_self(), SCHED_RR, &param), 0)
      << ": Failed to set scheduler priority.";
}

// This thread holds the mutex and spins until signaled to release it and stop.
template <typename MutexType>
class LowPriorityThread {
 public:
  explicit LowPriorityThread(MutexType* mutex)
      : m_mutex(mutex), m_hold_mutex(1), m_success(0) {}

  void operator()() {
    SetProcessorAffinity(0);
    SetThreadRealtimePriorityOrDie(20);
    m_mutex->lock();
    m_started.Notify();
    while (m_hold_mutex.test_and_set()) {
    }
    m_mutex->unlock();
    m_success.store(1);
  }

  void WaitForStartup() { m_started.Wait(); }
  void release_mutex() { m_hold_mutex.clear(); }
  bool success() { return m_success.load(); }

 private:
  // priority_mutex to grab and release.
  MutexType* m_mutex;
  Notification m_started;
  // Atomic type used to signal when the thread should unlock the mutex.
  // Using a mutex to protect something could cause other issues, and a delay
  // between setting and reading isn't a problem as long as the set is atomic.
  std::atomic_flag m_hold_mutex;
  std::atomic<int> m_success;
};

// This thread spins forever until signaled to stop.
class BusyWaitingThread {
 public:
  BusyWaitingThread() : m_run(1), m_success(0) {}

  void operator()() {
    SetProcessorAffinity(0);
    SetThreadRealtimePriorityOrDie(21);
    system_clock::time_point start_time = system_clock::now();
    m_started.Notify();
    while (m_run.test_and_set()) {
      // Have the busy waiting thread time out after a while.  If it times out,
      // the test failed.
      if (system_clock::now() - start_time > std::chrono::milliseconds(50)) {
        return;
      }
    }
    m_success.store(1);
  }

  void quit() { m_run.clear(); }
  void WaitForStartup() { m_started.Wait(); }
  bool success() { return m_success.load(); }

 private:
  // Use an atomic type to signal if the thread should be running or not.  A
  // mutex could affect the scheduler, which isn't worth it.  A delay between
  // setting and reading the new value is fine.
  std::atomic_flag m_run;

  Notification m_started;

  std::atomic<int> m_success;
};

// This thread starts up, grabs the mutex, and then exits.
template <typename MutexType>
class HighPriorityThread {
 public:
  explicit HighPriorityThread(MutexType* mutex) : m_mutex(mutex) {}

  void operator()() {
    SetProcessorAffinity(0);
    SetThreadRealtimePriorityOrDie(22);
    m_started.Notify();
    m_mutex->lock();
    m_success.store(1);
  }

  void WaitForStartup() { m_started.Wait(); }
  bool success() { return m_success.load(); }

 private:
  Notification m_started;
  MutexType* m_mutex;
  std::atomic<int> m_success{0};
};

// Class to test a MutexType to see if it solves the priority inheritance
// problem.
//
// To run the test, we need 3 threads, and then 1 thread to kick the test off.
// The threads must all run on the same core, otherwise they wouldn't starve
// eachother. The threads and their roles are as follows:
//
// Low priority thread:
//   Holds a lock that the high priority thread needs, and releases it upon
//   request.
// Medium priority thread:
//   Hogs the processor so that the low priority thread will never run if it's
//   priority doesn't get bumped.
// High priority thread:
//   Starts up and then goes to grab the lock that the low priority thread has.
//
// Control thread:
//   Sets the deadlock up so that it will happen 100% of the time by making sure
//   that each thread in order gets to the point that it needs to be at to cause
//   the deadlock.
template <typename MutexType>
class InversionTestRunner {
 public:
  void operator()() {
    // This thread must run at the highest priority or it can't coordinate the
    // inversion.  This means that it can't busy wait or everything could
    // starve.
    SetThreadRealtimePriorityOrDie(23);

    MutexType m;

    // Start the lowest priority thread and wait until it holds the lock.
    LowPriorityThread<MutexType> low(&m);
    std::thread low_thread(std::ref(low));
    low.WaitForStartup();

    // Start the busy waiting thread and let it get to the loop.
    BusyWaitingThread busy;
    std::thread busy_thread(std::ref(busy));
    busy.WaitForStartup();

    // Start the high priority thread and let it become blocked on the lock.
    HighPriorityThread<MutexType> high(&m);
    std::thread high_thread(std::ref(high));
    high.WaitForStartup();
    // Startup and locking the mutex in the high priority thread aren't atomic,
    // but pretty close.  Wait a bit to let it happen.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Release the mutex in the low priority thread.  If done right, everything
    // should finish now.
    low.release_mutex();

    // Wait for everything to finish and compute success.
    high_thread.join();
    busy.quit();
    busy_thread.join();
    low_thread.join();
    m_success = low.success() && busy.success() && high.success();
  }

  bool success() { return m_success; }

 private:
  bool m_success = false;
};

// TODO: Fix roborio permissions to run as root.

// Priority inversion test.
TEST(MutexTest, DISABLED_PriorityInversionTest) {
  InversionTestRunner<priority_mutex> runner;
  std::thread runner_thread(std::ref(runner));
  runner_thread.join();
  EXPECT_TRUE(runner.success());
}

// Verify that the non-priority inversion mutex doesn't pass the test.
TEST(MutexTest, DISABLED_StdMutexPriorityInversionTest) {
  InversionTestRunner<std::mutex> runner;
  std::thread runner_thread(std::ref(runner));
  runner_thread.join();
  EXPECT_FALSE(runner.success());
}

// Smoke test to make sure that mutexes lock and unlock.
TEST(MutexTest, TryLock) {
  priority_mutex m;
  m.lock();
  EXPECT_FALSE(m.try_lock());
  m.unlock();
  EXPECT_TRUE(m.try_lock());
}

// Priority inversion test.
TEST(MutexTest, DISABLED_ReentrantPriorityInversionTest) {
  InversionTestRunner<priority_recursive_mutex> runner;
  std::thread runner_thread(std::ref(runner));
  runner_thread.join();
  EXPECT_TRUE(runner.success());
}

// Smoke test to make sure that mutexes lock and unlock.
TEST(MutexTest, ReentrantTryLock) {
  priority_recursive_mutex m;
  m.lock();
  EXPECT_TRUE(m.try_lock());
  m.unlock();
  EXPECT_TRUE(m.try_lock());
}

#endif  // WPI_HAVE_PRIORITY_MUTEX

}  // namespace wpi
