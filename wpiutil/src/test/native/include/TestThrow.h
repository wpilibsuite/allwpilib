/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_TESTTHROW_H_
#define WPIUTIL_TESTTHROW_H_

// clang warns on TEST_THROW_MSG(x == y, ...) saying the result is unused.
// suppress this warning.
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-comparison"
#endif

// variant of GTEST_TEST_THROW_ that also checks the exception's message.
#define TEST_THROW_MSG(statement, expected_exception, expected_msg, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::internal::ConstCharPtr gtest_msg = "") { \
    bool gtest_caught_expected = false; \
    try { \
      GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
    } \
    catch (expected_exception const& gtest_ex) { \
      gtest_caught_expected = true; \
      if (::std::string(gtest_ex.what()) != expected_msg) { \
        ::testing::AssertionResult gtest_ar = ::testing::AssertionFailure(); \
        gtest_ar \
            << "Expected: " #statement " throws an exception with message \"" \
            << expected_msg "\".\n  Actual: it throws message \"" \
            << gtest_ex.what() << "\"."; \
        fail(gtest_ar.failure_message()); \
      } \
    } \
    catch (...) { \
      gtest_msg.value = \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws a different type."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
    if (!gtest_caught_expected) { \
      gtest_msg.value = \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws nothing."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
  } else \
    GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): \
      fail(gtest_msg.value)

#define EXPECT_THROW_MSG(statement, expected_exception, expected_msg) \
  TEST_THROW_MSG(statement, expected_exception, expected_msg, GTEST_NONFATAL_FAILURE_)

#define ASSERT_THROW_MSG(statement, expected_exception, expected_msg) \
  TEST_THROW_MSG(statement, expected_exception, expected_msg, GTEST_FATAL_FAILURE_)

#endif  // WPIUTIL_TESTTHROW_H_
