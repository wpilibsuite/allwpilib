// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>

void ExpectMatrixEqual(const Eigen::MatrixXd& lhs, const Eigen::MatrixXd& rhs,
                       double tolerance);

void ExpectPositiveSemidefinite(const Eigen::Ref<const Eigen::MatrixXd>& X);

void ExpectDARESolution(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B,
                        const Eigen::Ref<const Eigen::MatrixXd>& Q,
                        const Eigen::Ref<const Eigen::MatrixXd>& R,
                        const Eigen::Ref<const Eigen::MatrixXd>& X);

void ExpectDARESolution(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B,
                        const Eigen::Ref<const Eigen::MatrixXd>& Q,
                        const Eigen::Ref<const Eigen::MatrixXd>& R,
                        const Eigen::Ref<const Eigen::MatrixXd>& N,
                        const Eigen::Ref<const Eigen::MatrixXd>& X);
