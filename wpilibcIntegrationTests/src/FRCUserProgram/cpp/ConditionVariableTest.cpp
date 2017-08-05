/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "HAL/cpp/priority_condition_variable.h"
#include "HAL/cpp/priority_mutex.h"
#include "TestBench.h"
#include "gtest/gtest.h"

namespace wpilib {
namespace testing {

// Tests that the condition variable class which we wrote ourselves actually
// does work.
class ConditionVariableTest : public ::testing::Test {
 protected:
  typedef std::unique_lock<hal::priority_mutex> priority_lock;

  // Condition variable to test.
  hal::priority_condition_variable m_cond;

  // Mutex to pass to condition variable when waiting.
  hal::priority_mutex m_mutex;

  // flags for testing when threads are completed.
  std::atomic<bool> m_done1{false}, m_done2{false};
  // Threads to use for testing. We want multiple threads to ensure that it
  // behaves correctly when multiple processes are waiting on a signal.
  std::thread m_watcher1, m_watcher2;

  // Information for when running with predicates.
  std::atomic<bool> m_pred_var{false};

  void ShortSleep(uint32_t time = 10) {
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
  }

  // Start up the given threads with a wait function. The wait function should
  // call some version of m_cond.wait and should take as an argument a reference
  // to an std::atomic<bool> which it will set to true when it is ready to have
  // join called on it.
  template <class Function>
  void StartThreads(Function wait) {
    m_watcher1 = std::thread(wait, std::ref(m_done1));
    m_watcher2 = std::thread(wait, std::ref(m_done2));

    // Wait briefly to let the lock be unlocked.
    ShortSleep();
    bool locked = m_mutex.try_lock();
    if (locked) m_mutex.unlock();
    EXPECT_TRUE(locked) << "The condition variable failed to unlock the lock.";
  }

  void NotifyAll() { m_cond.notify_all(); }
  void NotifyOne() { m_cond.notify_one(); }

  // Test that all the threads are notified by a notify_all() call.
  void NotifyAllTest() {
    NotifyAll();
    // Wait briefly to let the lock be re-locked.
    ShortSleep();
    EXPECT_TRUE(m_done1) << "watcher1 failed to be notified.";
    EXPECT_TRUE(m_done2) << "watcher2 failed to be notified.";
  }

  // For use when testing predicates. First tries signalling the threads with
  // the predicate set to false (and ensures that they do not activate) and then
  // tests with the predicate set to true.
  void PredicateTest() {
    m_pred_var = false;
    NotifyAll();
    ShortSleep();
    EXPECT_FALSE(m_done1) << "watcher1 didn't pay attention to its predicate.";
    EXPECT_FALSE(m_done2) << "watcher2 didn't pay attention to its predicate.";
    m_pred_var = true;
    NotifyAllTest();
  }

  // Used by the WaitFor and WaitUntil tests to test that, without a predicate,
  // the timeout works properly.
  void WaitTimeTest(bool wait_for) {
    std::atomic<bool> timed_out{true};
    auto wait_until = [this, &timed_out, wait_for](std::atomic<bool>& done) {
      priority_lock lock(m_mutex);
      done = false;
      if (wait_for) {
        auto wait_time = std::chrono::milliseconds(100);
        timed_out = m_cond.wait_for(lock, wait_time) == std::cv_status::timeout;
      } else {
        auto wait_time =
            std::chrono::system_clock::now() + std::chrono::milliseconds(100);
        timed_out =
            m_cond.wait_until(lock, wait_time) == std::cv_status::timeout;
      }
      EXPECT_TRUE(lock.owns_lock())
          << "The condition variable should have reacquired the lock.";
      done = true;
    };

    // First, test without timing out.
    timed_out = true;
    StartThreads(wait_until);

    NotifyAllTest();
    EXPECT_FALSE(timed_out) << "The watcher should not have timed out.";

    TearDown();

    // Next, test and time out.
    timed_out = false;
    StartThreads(wait_until);

    ShortSleep(110);

    EXPECT_TRUE(m_done1) << "watcher1 should have timed out.";
    EXPECT_TRUE(m_done2) << "watcher2 should have timed out.";
    EXPECT_TRUE(timed_out) << "The watcher should have timed out.";
  }

  // For use with tests that have a timeout and a predicate.
  void WaitTimePredicateTest(bool wait_for) {
    // The condition_variable return value from the wait_for or wait_until
    // function should in the case of having a predicate, by a boolean. If the
    // predicate is true, then the return value will always be true. If the
    // condition times out and, at the time of the timeout, the predicate is
    // false, the return value will be false.
    std::atomic<bool> retval{true};
    auto predicate = [this]() -> bool { return m_pred_var; };
    auto wait_until = [this, &retval, predicate,
                       wait_for](std::atomic<bool>& done) {
      priority_lock lock(m_mutex);
      done = false;
      if (wait_for) {
        auto wait_time = std::chrono::milliseconds(100);
        retval = m_cond.wait_for(lock, wait_time, predicate);
      } else {
        auto wait_time =
            std::chrono::system_clock::now() + std::chrono::milliseconds(100);
        retval = m_cond.wait_until(lock, wait_time, predicate);
      }
      EXPECT_TRUE(lock.owns_lock())
          << "The condition variable should have reacquired the lock.";
      done = true;
    };

    // Test without timing out and with the predicate set to true.
    retval = true;
    m_pred_var = true;
    StartThreads(wait_until);

    NotifyAllTest();
    EXPECT_TRUE(retval) << "The watcher should not have timed out.";

    TearDown();

    // Test with timing out and with the predicate set to true.
    retval = false;
    m_pred_var = false;
    StartThreads(wait_until);

    ShortSleep(110);

    EXPECT_TRUE(m_done1) << "watcher1 should have finished.";
    EXPECT_TRUE(m_done2) << "watcher2 should have finished.";
    EXPECT_FALSE(retval) << "The watcher should have timed out.";

    TearDown();

    // Test without timing out and run the PredicateTest().
    retval = false;
    StartThreads(wait_until);

    PredicateTest();
    EXPECT_TRUE(retval) << "The return value should have been true.";

    TearDown();

    // Test with timing out and the predicate set to true while we are waiting
    // for the condition variable to time out.
    retval = true;
    StartThreads(wait_until);
    ShortSleep();
    m_pred_var = true;
    ShortSleep(110);
    EXPECT_TRUE(retval) << "The return value should have been true.";
  }

  virtual void TearDown() {
    // If a thread has not completed, then continuing will cause the tests to
    // hang forever and could cause issues. If we don't call detach, then
    // std::terminate is called and all threads are terminated.
    // Detaching is non-optimal, but should allow the rest of the tests to run
    // before anything drastic occurs.
    if (m_done1)
      m_watcher1.join();
    else
      m_watcher1.detach();
    if (m_done2)
      m_watcher2.join();
    else
      m_watcher2.detach();
  }
};

TEST_F(ConditionVariableTest, NotifyAll) {
  auto wait = [this](std::atomic<bool>& done) {
    priority_lock lock(m_mutex);
    done = false;
    m_cond.wait(lock);
    EXPECT_TRUE(lock.owns_lock())
        << "The condition variable should have reacquired the lock.";
    done = true;
  };

  StartThreads(wait);

  NotifyAllTest();
}

TEST_F(ConditionVariableTest, NotifyOne) {
  auto wait = [this](std::atomic<bool>& done) {
    priority_lock lock(m_mutex);
    done = false;
    m_cond.wait(lock);
    EXPECT_TRUE(lock.owns_lock())
        << "The condition variable should have reacquired the lock.";
    done = true;
  };

  StartThreads(wait);

  NotifyOne();
  // Wait briefly to let things settle.
  ShortSleep();
  EXPECT_TRUE(m_done1 ^ m_done2) << "Only one thread should've been notified.";
  NotifyOne();
  ShortSleep();
  EXPECT_TRUE(m_done2 && m_done2) << "Both threads should've been notified.";
}

TEST_F(ConditionVariableTest, WaitWithPredicate) {
  auto predicate = [this]() -> bool { return m_pred_var; };
  auto wait_predicate = [this, predicate](std::atomic<bool>& done) {
    priority_lock lock(m_mutex);
    done = false;
    m_cond.wait(lock, predicate);
    EXPECT_TRUE(lock.owns_lock())
        << "The condition variable should have reacquired the lock.";
    done = true;
  };

  StartThreads(wait_predicate);

  PredicateTest();
}

TEST_F(ConditionVariableTest, WaitUntil) { WaitTimeTest(false); }

TEST_F(ConditionVariableTest, WaitUntilWithPredicate) {
  WaitTimePredicateTest(false);
}

TEST_F(ConditionVariableTest, WaitFor) { WaitTimeTest(true); }

TEST_F(ConditionVariableTest, WaitForWithPredicate) {
  WaitTimePredicateTest(true);
}

TEST_F(ConditionVariableTest, NativeHandle) {
  auto wait = [this](std::atomic<bool>& done) {
    priority_lock lock(m_mutex);
    done = false;
    m_cond.wait(lock);
    EXPECT_TRUE(lock.owns_lock())
        << "The condition variable should have reacquired the lock.";
    done = true;
  };

  StartThreads(wait);

  pthread_cond_t* native_handle = m_cond.native_handle();
  pthread_cond_broadcast(native_handle);
  ShortSleep();
  EXPECT_TRUE(m_done1) << "watcher1 failed to be notified.";
  EXPECT_TRUE(m_done2) << "watcher2 failed to be notified.";
}

}  // namespace testing
}  // namespace wpilib
