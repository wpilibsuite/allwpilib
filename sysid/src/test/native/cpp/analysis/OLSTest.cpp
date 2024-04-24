// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "sysid/analysis/OLS.h"

TEST(OLSTest, TwoVariablesTwoPoints) {
  // (1, 3) and (2, 5). Should produce y = 2x + 1.
  Eigen::MatrixXd X{{1.0, 1.0}, {1.0, 2.0}};
  Eigen::VectorXd y{{3.0}, {5.0}};

  auto [coeffs, rSquared, rmse] = sysid::OLS(X, y);
  EXPECT_EQ(coeffs.size(), 2u);

  EXPECT_NEAR(coeffs[0], 1.0, 1e-12);
  EXPECT_NEAR(coeffs[1], 2.0, 1e-12);
  EXPECT_DOUBLE_EQ(rSquared, 1.0);
}

TEST(OLSTest, TwoVariablesFivePoints) {
  // (2, 4), (3, 5), (5, 7), (7, 10), (9, 15)
  // Should produce 1.518x + 0.305.
  Eigen::MatrixXd X{{1, 2}, {1, 3}, {1, 5}, {1, 7}, {1, 9}};
  Eigen::VectorXd y{{4}, {5}, {7}, {10}, {15}};

  auto [coeffs, rSquared, rmse] = sysid::OLS(X, y);
  EXPECT_EQ(coeffs.size(), 2u);

  EXPECT_NEAR(coeffs[0], 0.30487804878048774, 1e-12);
  EXPECT_NEAR(coeffs[1], 1.5182926829268293, 1e-12);
  EXPECT_DOUBLE_EQ(rSquared, 0.91906029466386019);
}

#ifndef NDEBUG
TEST(OLSTest, MalformedData) {
  Eigen::MatrixXd X{{1, 2}, {1, 3}, {1, 4}};
  Eigen::VectorXd y{{4}, {5}};
  EXPECT_DEATH(sysid::OLS(X, y), "");
}
#endif
