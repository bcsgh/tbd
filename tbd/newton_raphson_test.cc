// Copyright (c) 2018, Benjamin Shropshire,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "tbd/newton_raphson.h"

#include <cmath>

#include "Eigen/Core"
#include "absl/log/check.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tbd {

using testing::DoubleNear;
using testing::ElementsAre;

struct NRCase {
  VXd exp;
  SystemFunction fn;
  int dim;
  int count;
  double tol;
};

class NewtonRaphsonP : public ::testing::TestWithParam<NRCase> {};

TEST_P(NewtonRaphsonP, Case) {
  NRCase d = GetParam();
  // Validate that the zero is where it's expected.
  ASSERT_LT(d.fn(d.exp).array().abs().maxCoeff(), 1e-10);

  auto res =
      NewtonRaphson(d.fn, /*dim=*/d.dim, /*count=*/d.count, /*tol=*/d.tol);
  EXPECT_LT((res - d.exp).array().abs().maxCoeff(), 1e-5)
      << "[" << res.transpose() << "] != [" << d.exp.transpose() << "], ["
      << (res - d.exp).transpose() << "]";
}

INSTANTIATE_TEST_SUITE_P(  //
    OneDimLin, NewtonRaphsonP,
    ::testing::Values(  //
        NRCase{/*exp=*/VXd::Constant(1, 1, -1.00000),
               [](VXd d) { return VXd::Constant(1, 1, d[0] + 1); },
               /*dim=*/1, /*count=*/1, /*tol=*/0.001}));

INSTANTIATE_TEST_SUITE_P(  //
    OneDim, NewtonRaphsonP,
    ::testing::Values(  //
        NRCase{/*exp=*/VXd::Constant(1, 1, +2),
               [](VXd d) { return VXd::Constant(1, 1, std::pow(d[0], 3) - 8); },
               /*dim=*/1, /*count=*/17, /*tol=*/0.0001}));

INSTANTIATE_TEST_SUITE_P(  //
    TwoDimLin, NewtonRaphsonP,
    ::testing::Values(  //
        NRCase{/*exp=*/(VXd(2) << 1, 2).finished(),
               [](VXd d) {
                 CHECK(d.size() == 2);
                 double a = (d[0] * 5 + d[1] * 7 - 19);
                 double b = (d[0] * 2 + d[1] * 3 - 8);
                 VXd r(2);
                 r << a, b;
                 return r;
               },
               /*dim=*/2, /*count=*/1, /*tol=*/0.001}));

INSTANTIATE_TEST_SUITE_P(  //
    TwoDim, NewtonRaphsonP,
    ::testing::Values(  //
        NRCase{/*exp=*/(VXd(2) << 1, 2).finished(),
               [](VXd d) {
                 CHECK(d.size() == 2);
                 double a = std::pow(2, d[0]) +
                            (d[1] * d[1] * d[1] + d[1] * 10) / 2 - 16;
                 double b = (d[0] * 2 + d[1] * 3 - 8);
                 VXd r(2);
                 r << a, b;
                 return r;
               },
               /*dim=*/2, /*count=*/10, /*tol=*/1e-4}));

}  // namespace tbd