/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "gcem/gcem.h"
#include "gtest/gtest.h"

#ifdef _WIN32
constexpr static double kErrorTolerance = 1E-10;
#else
constexpr static double kErrorTolerance = 1E-14;
#endif

#define VAL_IS_INF(val) std::isinf(static_cast<long double>(val))
#define VAL_IS_NAN(val) std::isnan(static_cast<long double>(val))

#define COMPARE_VALS(gcem_f, std_f, ...)                                \
  {                                                                     \
    auto gcem_val = gcem_f(__VA_ARGS__);                                \
    auto std_val = std_f(__VA_ARGS__);                                  \
    if (VAL_IS_NAN(gcem_val) && VAL_IS_NAN(std_val))                    \
      EXPECT_TRUE(true);                                                \
    else if (!VAL_IS_NAN(gcem_val) && VAL_IS_INF(gcem_val) &&           \
             gcem_val == std_val)                                       \
      EXPECT_TRUE(true);                                                \
    else                                                                \
      EXPECT_LT(std::abs(gcem_val - std_val) / (1 + std::abs(std_val)), \
                kErrorTolerance);                                       \
  }

#define TEST_EXPECTED_VAL(gcem_f, expected_val, ...)                         \
  {                                                                          \
    auto f_val = gcem_f(__VA_ARGS__);                                        \
    EXPECT_LT(std::abs(f_val - expected_val) / (1 + std::abs(expected_val)), \
              kErrorTolerance);                                              \
  }

TEST(GCEMathTest, abs) {
  COMPARE_VALS(gcem::abs, std::abs, 0.0);
  COMPARE_VALS(gcem::abs, std::abs, -0.0);
  COMPARE_VALS(gcem::abs, std::abs, 1.0);
  COMPARE_VALS(gcem::abs, std::abs, -1.0);
  COMPARE_VALS(gcem::abs, std::abs, 0);
  COMPARE_VALS(gcem::abs, std::abs, 1);
  COMPARE_VALS(gcem::abs, std::abs, -1);

  COMPARE_VALS(gcem::abs, std::abs, INT64_MIN);
  COMPARE_VALS(gcem::abs, std::abs, INT64_MAX);
}

TEST(GCEMathTest, acos) {
  COMPARE_VALS(gcem::acos, std::acos, 0.0L);
  COMPARE_VALS(gcem::acos, std::acos, 0.001L);
  COMPARE_VALS(gcem::acos, std::acos, 0.5L);
  COMPARE_VALS(gcem::acos, std::acos, -0.5L);
  COMPARE_VALS(gcem::acos, std::acos, 0.7568025L);
  COMPARE_VALS(gcem::acos, std::acos, 1.0L);
  COMPARE_VALS(gcem::acos, std::acos, 5.0L);
}

TEST(GCEMathTest, acosh) {
  COMPARE_VALS(gcem::acosh, std::acosh, 1.001L);
  COMPARE_VALS(gcem::acosh, std::acosh, 1.5L);
  COMPARE_VALS(gcem::acosh, std::acosh, 11.1L);
  COMPARE_VALS(gcem::acosh, std::acosh, 50.0L);
  COMPARE_VALS(gcem::acosh, std::acosh, 0.5L);
}

TEST(GCEMathTest, asin) {
  COMPARE_VALS(gcem::asin, std::asin, 0.0L);
  COMPARE_VALS(gcem::asin, std::asin, 0.001L);
  COMPARE_VALS(gcem::asin, std::asin, 0.5L);
  COMPARE_VALS(gcem::asin, std::asin, -0.5L);
  COMPARE_VALS(gcem::asin, std::asin, 0.7568025L);
  COMPARE_VALS(gcem::asin, std::asin, 1.0L);
  COMPARE_VALS(gcem::asin, std::asin, 5.0L);
}

TEST(GCEMathTest, asinh) {
  COMPARE_VALS(gcem::asinh, std::asinh, 0.0L);
  COMPARE_VALS(gcem::asinh, std::asinh, 0.001L);
  COMPARE_VALS(gcem::asinh, std::asinh, 1.001L);
  COMPARE_VALS(gcem::asinh, std::asinh, 1.5L);
  COMPARE_VALS(gcem::asinh, std::asinh, 11.1L);
  COMPARE_VALS(gcem::asinh, std::asinh, 50.0L);
  COMPARE_VALS(gcem::asinh, std::asinh, -1.5L);
}

TEST(GCEMathTest, atan) {
  COMPARE_VALS(gcem::atan, std::atan, 0.0L);
  COMPARE_VALS(gcem::atan, std::atan, 0.001L);
  COMPARE_VALS(gcem::atan, std::atan, 0.49L);
  COMPARE_VALS(gcem::atan, std::atan, -0.5L);
  COMPARE_VALS(gcem::atan, std::atan, -1.5L);
  COMPARE_VALS(gcem::atan, std::atan, 0.7568025L);
  COMPARE_VALS(gcem::atan, std::atan, 0.99L);
  COMPARE_VALS(gcem::atan, std::atan, 1.49L);
  COMPARE_VALS(gcem::atan, std::atan, 1.99L);

  COMPARE_VALS(gcem::atan, std::atan, 2.49L);
  COMPARE_VALS(gcem::atan, std::atan, 2.51L);
  COMPARE_VALS(gcem::atan, std::atan, 3.99L);
  COMPARE_VALS(gcem::atan, std::atan, 7.0L);
  COMPARE_VALS(gcem::atan, std::atan, 11.0L);
  COMPARE_VALS(gcem::atan, std::atan, 25.0L);

  COMPARE_VALS(gcem::atan, std::atan, 101.0L);
  COMPARE_VALS(gcem::atan, std::atan, 900.0L);
  COMPARE_VALS(gcem::atan, std::atan, 1001.0L);
}

TEST(GCEMathTest, atan2) {
  COMPARE_VALS(gcem::atan2, std::atan2, 0.0L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, -0.0L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.0L, -0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, -0.0L, -0.0L);

  COMPARE_VALS(gcem::atan2, std::atan2, 0.2L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, -0.2L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.001L, 0.001L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.49L, 0.49L);

  COMPARE_VALS(gcem::atan2, std::atan2, -0.5L, -0.5L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.5L, -0.5L);
  COMPARE_VALS(gcem::atan2, std::atan2, -0.5L, 0.5L);

  COMPARE_VALS(gcem::atan2, std::atan2, 9.6L, 8.4L);
  COMPARE_VALS(gcem::atan2, std::atan2, 1.0L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.0L, 1.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, -1.0L, 0.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.0L, -1.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, 1.0L, 3.0L);
  COMPARE_VALS(gcem::atan2, std::atan2, -5.0L, 2.5L);
  COMPARE_VALS(gcem::atan2, std::atan2, -1000.0L, -0.001L);
  COMPARE_VALS(gcem::atan2, std::atan2, 0.1337L, -123456.0L);
}

TEST(GCEMathTest, atanh) {
  COMPARE_VALS(gcem::atanh, std::atanh, -0.99L);
  COMPARE_VALS(gcem::atanh, std::atanh, 0.0L);
  COMPARE_VALS(gcem::atanh, std::atanh, 0.001L);
  COMPARE_VALS(gcem::atanh, std::atanh, 1.0L);
  COMPARE_VALS(gcem::atanh, std::atanh, -1.0L);
  COMPARE_VALS(gcem::atanh, std::atanh, 1.1L);
}

TEST(GCEMathTest, binomial_coef) {
  TEST_EXPECTED_VAL(gcem::binomial_coef, 1, 0, 0);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 0, 0, 1);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 1, 1, 0);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 1, 1, 1);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 10, 5, 2);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 45, 10, 8);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 10, 10, 9);
  TEST_EXPECTED_VAL(gcem::binomial_coef, 1, 10, 10);
}

TEST(GCEMathTest, copysign) {
  COMPARE_VALS(gcem::copysign, std::copysign, 1.0, 1.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -1.0, 1.0);
  COMPARE_VALS(gcem::copysign, std::copysign, 1.0, -1.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -1.0, -1.0);

  COMPARE_VALS(gcem::copysign, std::copysign, 1.0, 0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -1.0, 0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, 1.0, -0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -1.0, -0.0);

  COMPARE_VALS(gcem::copysign, std::copysign, 0.0, 0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -0.0, 0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, 0.0, -0.0);
  COMPARE_VALS(gcem::copysign, std::copysign, -0.0, -0.0);
}

TEST(GCEMathTest, cos) {
  COMPARE_VALS(gcem::cos, std::cos, -1.5L);
  COMPARE_VALS(gcem::cos, std::cos, 0.0L);
  COMPARE_VALS(gcem::cos, std::cos, 0.001L);
  COMPARE_VALS(gcem::cos, std::cos, 1.001L);
  COMPARE_VALS(gcem::cos, std::cos, 1.5L);
  COMPARE_VALS(gcem::cos, std::cos, 11.1L);
  COMPARE_VALS(gcem::cos, std::cos, 50.0L);
  COMPARE_VALS(gcem::cos, std::cos, 150.0L);
}

TEST(GCEMathTest, cosh) {
  COMPARE_VALS(gcem::cosh, std::cosh, 0.0L);
  COMPARE_VALS(gcem::cosh, std::cosh, 0.001L);
  COMPARE_VALS(gcem::cosh, std::cosh, 0.5L);
  COMPARE_VALS(gcem::cosh, std::cosh, -0.5L);
  COMPARE_VALS(gcem::cosh, std::cosh, 0.7568025L);
  COMPARE_VALS(gcem::cosh, std::cosh, 1.0L);
  COMPARE_VALS(gcem::cosh, std::cosh, 5.0L);
}

TEST(GCEMathTest, erf) {
  COMPARE_VALS(gcem::erf, std::erf, -3.0L);
  COMPARE_VALS(gcem::erf, std::erf, -2.5L);
  COMPARE_VALS(gcem::erf, std::erf, -2.11L);
  COMPARE_VALS(gcem::erf, std::erf, -2.05L);
  COMPARE_VALS(gcem::erf, std::erf, -1.3L);
  COMPARE_VALS(gcem::erf, std::erf, 0.0L);
  COMPARE_VALS(gcem::erf, std::erf, 1.3L);
  COMPARE_VALS(gcem::erf, std::erf, 2.05L);
  COMPARE_VALS(gcem::erf, std::erf, 2.11L);
  COMPARE_VALS(gcem::erf, std::erf, 2.5L);
  COMPARE_VALS(gcem::erf, std::erf, 3.0L);
}

TEST(GCEMathTest, erf_inv) {
  TEST_EXPECTED_VAL(gcem::erf_inv, -3.0L, -0.999977909503001415L);
  TEST_EXPECTED_VAL(gcem::erf_inv, -2.5L, -0.999593047982555041L);
  TEST_EXPECTED_VAL(gcem::erf_inv, -2.11L, -0.997154845031177802L);
  TEST_EXPECTED_VAL(gcem::erf_inv, -2.05L, -0.996258096044456873L);
  TEST_EXPECTED_VAL(gcem::erf_inv, -1.3L, -0.934007944940652437L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 0.0L, 0.0L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 1.3L, 0.934007944940652437L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 2.05L, 0.996258096044456873L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 2.11L, 0.997154845031177802L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 2.5L, 0.999593047982555041L);
  TEST_EXPECTED_VAL(gcem::erf_inv, 3.0L, 0.999977909503001415L);
}

TEST(GCEMathTest, exp) {
  COMPARE_VALS(gcem::exp, std::exp, -40.0L);
  COMPARE_VALS(gcem::exp, std::exp, -4.0L);
  COMPARE_VALS(gcem::exp, std::exp, 0.0001L);
  COMPARE_VALS(gcem::exp, std::exp, 1.75L);
  COMPARE_VALS(gcem::exp, std::exp, 1.9991L);
  COMPARE_VALS(gcem::exp, std::exp, 2.1L);
  COMPARE_VALS(gcem::exp, std::exp, 4.0L);

  COMPARE_VALS(gcem::exp, std::exp,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::exp, std::exp,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::exp, std::exp,
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, expm) {
  COMPARE_VALS(gcem::expm1, std::expm1, 1.0L);
  COMPARE_VALS(gcem::expm1, std::expm1, 0.0L);
  COMPARE_VALS(gcem::expm1, std::expm1, 1e-04L);
  COMPARE_VALS(gcem::expm1, std::expm1, -1e-04L);
  COMPARE_VALS(gcem::expm1, std::expm1, 1e-05L);
  COMPARE_VALS(gcem::expm1, std::expm1, 1e-06L);
  COMPARE_VALS(gcem::expm1, std::expm1, 1e-22L);

  COMPARE_VALS(gcem::expm1, std::expm1,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::expm1, std::expm1,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::expm1, std::expm1,
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, factorial) {
  std::function<long double(long double)>  // NOLINT(readability/casting)
      std_fn = [](long double x) -> long double { return std::tgamma(x + 1); };

  COMPARE_VALS(gcem::factorial, std_fn, 3.1L);
  COMPARE_VALS(gcem::factorial, std_fn, 7.0L);
  COMPARE_VALS(gcem::factorial, std_fn, 10.0L);
  COMPARE_VALS(gcem::factorial, std_fn, 12.0L);
  COMPARE_VALS(gcem::factorial, std_fn, 14.0L);

  // using long ints
  COMPARE_VALS(gcem::factorial, std_fn, 5L);
  COMPARE_VALS(gcem::factorial, std_fn, 9L);
  COMPARE_VALS(gcem::factorial, std_fn, 11L);
}

TEST(GCEMathTest, fmod) {
  COMPARE_VALS(gcem::fmod, std::fmod, 0.0, 0.0);
  COMPARE_VALS(gcem::fmod, std::fmod, -0.0, 0.0);
  COMPARE_VALS(gcem::fmod, std::fmod, 0.0, -0.0);
  COMPARE_VALS(gcem::fmod, std::fmod, 5.3, 2.0);
  COMPARE_VALS(gcem::fmod, std::fmod, -5.3, 2.0);
  COMPARE_VALS(gcem::fmod, std::fmod, 5.3, -2.0);
  COMPARE_VALS(gcem::fmod, std::fmod, -5.3, -2.0);
  COMPARE_VALS(gcem::fmod, std::fmod, 18.5, 4.2);
  COMPARE_VALS(gcem::fmod, std::fmod, -18.5, 4.2);
  COMPARE_VALS(gcem::fmod, std::fmod, 18.5, -4.2);
  COMPARE_VALS(gcem::fmod, std::fmod, -18.5, -4.2);

  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(), 0.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(), -1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(), 1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(),
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(),
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               -std::numeric_limits<long double>::infinity(),
               std::numeric_limits<long double>::quiet_NaN());

  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(), 0.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(), -1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(), 1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(),
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(),
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::infinity(),
               std::numeric_limits<long double>::quiet_NaN());

  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(), 0.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(), -1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(), 1.0);
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(),
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(),
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::fmod, std::fmod,
               std::numeric_limits<long double>::quiet_NaN(),
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, gcd) {
  TEST_EXPECTED_VAL(gcem::gcd, 6, 12, 18);
  TEST_EXPECTED_VAL(gcem::gcd, 2, -4, 14);
  TEST_EXPECTED_VAL(gcem::gcd, 14, 42, 56);
}

TEST(GCEMathTest, incomplete_beta) {
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.11464699677582491921L, 0.9L, 0.9L,
                    0.1L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.78492840804657726395L, 0.9L, 0.9L,
                    0.8L);

  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.80000000000000004441L, 1.0L, 1.0L,
                    0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.89600000000000001865L, 2.0L, 2.0L,
                    0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.81920000000000003926L, 3.0L, 2.0L,
                    0.8L);

  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.97279999999999999805L, 2.0L, 3.0L,
                    0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 3.9970000000000084279e-09L, 3.0L,
                    2.0L, 0.001L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta, 0.35200000000000003508L, 2.0L, 2.0L,
                    0.4L);
}

TEST(GCEMathTest, incomplete_beta_inv) {
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.085977260425697907276L, 0.9L,
                    0.9L, 0.1L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.81533908558467627081L, 0.9L,
                    0.9L, 0.8L);

  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.80000000000000004441L, 1.0L,
                    1.0L, 0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.71285927458325959449L, 2.0L,
                    2.0L, 0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.78768287172204876079L, 3.0L,
                    2.0L, 0.8L);

  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.58245357452433332845L, 2.0L,
                    3.0L, 0.8L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.064038139102833388505L, 3.0L,
                    2.0L, 0.001L);
  TEST_EXPECTED_VAL(gcem::incomplete_beta_inv, 0.43293107714773171324L, 2.0L,
                    2.0L, 0.4L);
}

TEST(GCEMathTest, incomplete_gamma) {
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.26424111765711527644L, 2.0L,
                    1.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.42759329552912134220L, 1.5L,
                    1.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.80085172652854419439L, 2.0L,
                    3.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.95957231800548714595L, 2.0L,
                    5.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99876590195913317327L, 2.0L,
                    9.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 1.0L, 0.0L, 9.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.0L, 2.0L, 0.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.0L, 0.0L, 0.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.47974821959920432857L, 11.5L,
                    11.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.75410627202774938027L, 15.5L,
                    18.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.43775501395596266851L, 19.0L,
                    18.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.43939261060849132967L, 20.0L,
                    19.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.58504236243630125536L, 38.0L,
                    39.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.46422093592347296598L, 56.0L,
                    55.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.55342727426212001696L, 98.0L,
                    99.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.47356929040257916830L, 102.0L,
                    101.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.48457398488934400049L, 298.0L,
                    297.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.48467673854389853316L, 302.0L,
                    301.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.48807306199561317772L, 498.0L,
                    497.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.48812074555706047585L, 502.0L,
                    501.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.51881664512582725823L, 798.0L,
                    799.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.49059834200005758564L, 801.0L,
                    800.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.52943655952003709775L, 997.0L,
                    999.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.45389544705967349580L, 1005.0L,
                    1001.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99947774194996708008L, 3.0L,
                    12.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99914335878922466705L, 5.0L,
                    15.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99998305525606989708L, 5.0L,
                    20.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99999973309165746116L, 5.0L,
                    25.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99999999995566213329L, 5.0L,
                    35.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 1.0L, 5.0L, 50.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 1.0L, 5.0L, 500.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99791274095086501816L, 9.0L,
                    20.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99999999914307291515L, 9.0L,
                    40.0L);

  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.88153558847098478779L, 11.0L,
                    15.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.98918828117334733907L, 11.0L,
                    20.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99941353837024693441L, 11.0L,
                    25.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99999933855955824846L, 11.0L,
                    35.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 0.99999999999354982627L, 11.0L,
                    50.0L);
  TEST_EXPECTED_VAL(gcem::incomplete_gamma, 1.0L, 11.0L, 500.0L);
}

TEST(GCEMMathTest, is_odd) {
  int run_val = 0;

  run_val += gcem::internal::is_odd(1);
  run_val += gcem::internal::is_odd(3);
  run_val += gcem::internal::is_odd(-5);
  run_val += gcem::internal::is_even(10UL);
  run_val += gcem::internal::is_odd(-400009L);
  run_val += gcem::internal::is_even(100000000L);

  EXPECT_EQ(run_val, 6);
}

TEST(GCEMathTest, lcm) {
  TEST_EXPECTED_VAL(gcem::lcm, 12, 3, 4);
  TEST_EXPECTED_VAL(gcem::lcm, 60, 12, 15);
  TEST_EXPECTED_VAL(gcem::lcm, 1377, 17, 81);
}

TEST(GCEMathTest, lgamma) {
  COMPARE_VALS(gcem::lgamma, std::lgamma, 1.5L);
  COMPARE_VALS(gcem::lgamma, std::lgamma, 0.7L);
  COMPARE_VALS(gcem::lgamma, std::lgamma, 1.0L);
  COMPARE_VALS(gcem::lgamma, std::lgamma, 0.0L);
  COMPARE_VALS(gcem::lgamma, std::lgamma, -1.0L);
}

TEST(GCEMathTest, log) {
  COMPARE_VALS(gcem::log, std::log, 0.5L);
  COMPARE_VALS(gcem::log, std::log, 0.00199900000000000208L);
  COMPARE_VALS(gcem::log, std::log, 1.5L);
  COMPARE_VALS(gcem::log, std::log, 41.5L);
  COMPARE_VALS(gcem::log, std::log, 0.0L);
  COMPARE_VALS(gcem::log, std::log, -1.0L);

  COMPARE_VALS(gcem::log, std::log,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log, std::log,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log, std::log,
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, log1p) {
  COMPARE_VALS(gcem::log1p, std::log1p, 1.0L);
  COMPARE_VALS(gcem::log1p, std::log1p, 0.0L);
  COMPARE_VALS(gcem::log1p, std::log1p, 1e-04L);
  COMPARE_VALS(gcem::log1p, std::log1p, -1e-04L);
  COMPARE_VALS(gcem::log1p, std::log1p, 1e-05L);
  COMPARE_VALS(gcem::log1p, std::log1p, 1e-06L);
  COMPARE_VALS(gcem::log1p, std::log1p, 1e-22L);

  COMPARE_VALS(gcem::log1p, std::log1p,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log1p, std::log1p,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log1p, std::log1p,
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, log2) {
  COMPARE_VALS(gcem::log2, std::log2, 0.5L);
  COMPARE_VALS(gcem::log2, std::log2, 0.00199900000000000208L);
  COMPARE_VALS(gcem::log2, std::log2, 1.5L);
  COMPARE_VALS(gcem::log2, std::log2, 41.5L);
  COMPARE_VALS(gcem::log2, std::log2, 0.0L);
  COMPARE_VALS(gcem::log2, std::log2, -1.0L);

  COMPARE_VALS(gcem::log2, std::log2,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log2, std::log2,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::log2, std::log2,
               std::numeric_limits<long double>::quiet_NaN());
}

TEST(GCEMathTest, log_binomial_coef) {
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, 0, 0, 0);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, 0, 1, 0);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, 0, 1, 1);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, std::log(10.0L), 5, 2);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, std::log(45.0L), 10, 8);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, std::log(10.0L), 10, 9);
  TEST_EXPECTED_VAL(gcem::log_binomial_coef, 0, 10, 10);
}

TEST(GCEMathTest, other) {
  TEST_EXPECTED_VAL(gcem::sgn, 1, 1.5);
  TEST_EXPECTED_VAL(gcem::sgn, -1, -1.5);

  TEST_EXPECTED_VAL(gcem::internal::find_fraction, -0.5, 1.5);
  TEST_EXPECTED_VAL(gcem::internal::find_fraction, 1.5, -1.5);

  TEST_EXPECTED_VAL(gcem::internal::find_whole, -3, -1.5);

  TEST_EXPECTED_VAL(gcem::internal::neg_zero, 0, 0.0);
  TEST_EXPECTED_VAL(gcem::internal::neg_zero, 1, -0.0);
}

TEST(GCEMathTest, pow) {
  COMPARE_VALS(gcem::pow, std::pow, 0.199900000000000208L, 3.5L);
  COMPARE_VALS(gcem::pow, std::pow, 0.5L, 2.0L);
  COMPARE_VALS(gcem::pow, std::pow, 1.5L, 0.99L);
  COMPARE_VALS(gcem::pow, std::pow, 41.5L, 7.0L);

  // int versions
  COMPARE_VALS(gcem::pow, std::pow, 0.5L, 2L);
  COMPARE_VALS(gcem::pow, std::pow, 41.5L, 7L);

  COMPARE_VALS(gcem::pow, std::pow,
               std::numeric_limits<long double>::quiet_NaN(), 2);
  COMPARE_VALS(gcem::pow, std::pow, 2,
               std::numeric_limits<long double>::quiet_NaN());

  COMPARE_VALS(gcem::pow, std::pow,
               std::numeric_limits<long double>::infinity(), 2);
  COMPARE_VALS(gcem::pow, std::pow,
               std::numeric_limits<long double>::infinity(), -2);
  COMPARE_VALS(gcem::pow, std::pow,
               std::numeric_limits<long double>::infinity(), 0);
}

TEST(GCEMathTest, rounding) {
  COMPARE_VALS(gcem::floor, std::floor, 0.0);
  COMPARE_VALS(gcem::floor, std::floor, -0.0);
  COMPARE_VALS(gcem::floor, std::floor, 4.2);
  COMPARE_VALS(gcem::floor, std::floor, 4.5);
  COMPARE_VALS(gcem::floor, std::floor, 4.7);
  COMPARE_VALS(gcem::floor, std::floor, 5.0);
  COMPARE_VALS(gcem::floor, std::floor, -4.2);
  COMPARE_VALS(gcem::floor, std::floor, -4.7);
  COMPARE_VALS(gcem::floor, std::floor, -5.0);

  COMPARE_VALS(gcem::floor, std::floor,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::floor, std::floor,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::floor, std::floor,
               std::numeric_limits<long double>::quiet_NaN());

  //

  COMPARE_VALS(gcem::ceil, std::ceil, 0.0);
  COMPARE_VALS(gcem::ceil, std::ceil, -0.0);
  COMPARE_VALS(gcem::ceil, std::ceil, 4.2);
  COMPARE_VALS(gcem::ceil, std::ceil, 4.5);
  COMPARE_VALS(gcem::ceil, std::ceil, 4.7);
  COMPARE_VALS(gcem::ceil, std::ceil, 5.0);
  COMPARE_VALS(gcem::ceil, std::ceil, -4.2);
  COMPARE_VALS(gcem::ceil, std::ceil, -4.7);
  COMPARE_VALS(gcem::ceil, std::ceil, -5.0);

  COMPARE_VALS(gcem::ceil, std::ceil,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::ceil, std::ceil,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::ceil, std::ceil,
               std::numeric_limits<long double>::quiet_NaN());

  //

  COMPARE_VALS(gcem::trunc, std::trunc, 0.0);
  COMPARE_VALS(gcem::trunc, std::trunc, -0.0);
  COMPARE_VALS(gcem::trunc, std::trunc, 4.2);
  COMPARE_VALS(gcem::trunc, std::trunc, 4.5);
  COMPARE_VALS(gcem::trunc, std::trunc, 4.7);
  COMPARE_VALS(gcem::trunc, std::trunc, 5.0);
  COMPARE_VALS(gcem::trunc, std::trunc, -4.2);
  COMPARE_VALS(gcem::trunc, std::trunc, -4.7);
  COMPARE_VALS(gcem::trunc, std::trunc, -5.0);

  COMPARE_VALS(gcem::trunc, std::trunc,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::trunc, std::trunc,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::trunc, std::trunc,
               std::numeric_limits<long double>::quiet_NaN());

  //

  COMPARE_VALS(gcem::round, std::round, 0.0);
  COMPARE_VALS(gcem::round, std::round, -0.0);
  COMPARE_VALS(gcem::round, std::round, 4.2);
  COMPARE_VALS(gcem::round, std::round, 4.5);
  COMPARE_VALS(gcem::round, std::round, 4.7);
  COMPARE_VALS(gcem::round, std::round, 5.0);
  COMPARE_VALS(gcem::round, std::round, -4.2);
  COMPARE_VALS(gcem::round, std::round, -4.5);
  COMPARE_VALS(gcem::round, std::round, -4.7);
  COMPARE_VALS(gcem::round, std::round, -5.0);

  COMPARE_VALS(gcem::round, std::round,
               -std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::round, std::round,
               std::numeric_limits<long double>::infinity());
  COMPARE_VALS(gcem::round, std::round,
               std::numeric_limits<long double>::quiet_NaN());
}

template <typename T>
constexpr int std_test_fn(const T x) {
  return static_cast<int>(std::signbit(static_cast<double>(x)));
}

template <typename T>
constexpr int gcem_test_fn(const T x) {
  return static_cast<int>(gcem::signbit(x));
}

TEST(GCEMathTest, signbit) {
  COMPARE_VALS(gcem_test_fn, std_test_fn, 1.0);
  COMPARE_VALS(gcem_test_fn, std_test_fn, -1.0);

  COMPARE_VALS(gcem_test_fn, std_test_fn, 0.0);
  COMPARE_VALS(gcem_test_fn, std_test_fn, -0.0);
}

TEST(GCEMathTest, sin) {
  COMPARE_VALS(gcem::sin, std::sin, -1.5L);
  COMPARE_VALS(gcem::sin, std::sin, 0.0L);
  COMPARE_VALS(gcem::sin, std::sin, 0.001L);
  COMPARE_VALS(gcem::sin, std::sin, 1.001L);
  COMPARE_VALS(gcem::sin, std::sin, 1.5L);
  COMPARE_VALS(gcem::sin, std::sin, 11.1L);
  COMPARE_VALS(gcem::sin, std::sin, 50.0L);
  COMPARE_VALS(gcem::sin, std::sin, 150.0L);
}

TEST(GCEMathTest, sinh) {
  COMPARE_VALS(gcem::sinh, std::sinh, 0.0L);
  COMPARE_VALS(gcem::sinh, std::sinh, 0.001L);
  COMPARE_VALS(gcem::sinh, std::sinh, 0.5L);
  COMPARE_VALS(gcem::sinh, std::sinh, -0.5L);
  COMPARE_VALS(gcem::sinh, std::sinh, 0.7568025L);
  COMPARE_VALS(gcem::sinh, std::sinh, 1.0L);
  COMPARE_VALS(gcem::sinh, std::sinh, 5.0L);
}

TEST(GCEMathTest, sqrt) {
  COMPARE_VALS(gcem::sqrt, std::sqrt, 0.5L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, 0.00199900000000000208L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, 1.5L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, 2.0L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, 41.5L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, 0.0L);
  COMPARE_VALS(gcem::sqrt, std::sqrt, -1.0L);
}

TEST(GCEMathTest, tan) {
  COMPARE_VALS(gcem::tan, std::tan, 0.0L);
  COMPARE_VALS(gcem::tan, std::tan, 0.001L);
  COMPARE_VALS(gcem::tan, std::tan, 1.001L);
  COMPARE_VALS(gcem::tan, std::tan, 1.5L);
  COMPARE_VALS(gcem::tan, std::tan, 11.1L);
  COMPARE_VALS(gcem::tan, std::tan, 50.0L);
  COMPARE_VALS(gcem::tan, std::tan, -1.5L);
}

TEST(GCEMathTest, tanh) {
  COMPARE_VALS(gcem::tanh, std::tanh, 0.0L);
  COMPARE_VALS(gcem::tanh, std::tanh, 0.001L);
  COMPARE_VALS(gcem::tanh, std::tanh, 0.5L);
  COMPARE_VALS(gcem::tanh, std::tanh, -0.5L);
  COMPARE_VALS(gcem::tanh, std::tanh, 0.7568025L);
  COMPARE_VALS(gcem::tanh, std::tanh, 1.0L);
  COMPARE_VALS(gcem::tanh, std::tanh, 5.0L);
}

TEST(GCEMathTest, tgamma) {
  COMPARE_VALS(gcem::tgamma, std::tgamma, 1.5L);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 2.7L);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 3.0L);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 4.0L);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 5.0L);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 1.0);

  COMPARE_VALS(gcem::tgamma, std::tgamma, 0.9);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 0.1);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 0.001);
  COMPARE_VALS(gcem::tgamma, std::tgamma, 0.0);

  COMPARE_VALS(gcem::tgamma, std::tgamma, -0.1);
  COMPARE_VALS(gcem::tgamma, std::tgamma, -1);
  COMPARE_VALS(gcem::tgamma, std::tgamma, -1.1);
  COMPARE_VALS(gcem::tgamma, std::tgamma, -2);
  COMPARE_VALS(gcem::tgamma, std::tgamma, -3);
  COMPARE_VALS(gcem::tgamma, std::tgamma, -4);
}
