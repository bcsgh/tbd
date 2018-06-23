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
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/common.h"

namespace tbd {
const int _i = logging::InstallSignalhandler();

using testing::ElementsAre;
using testing::DoubleNear;

TEST(NewtonRaphson, OneDimLin) {
  VXd exp = VXd::Constant(1, 1, -1.00000);
  auto fn = [](VXd d) { return VXd::Constant(1, 1, d[0] + 1); };
  // Validate that the zero is where it's expected.
  ASSERT_LT(fn(exp).array().abs().maxCoeff(), 1e-10);

  auto res = NewtonRaphson(fn, /*dim=*/1);
  EXPECT_LT((res - exp).array().abs().maxCoeff(), 1e-5)
      << "[" << res.transpose() << "] != [" << exp.transpose() << "], ["
      << (res - exp).transpose() << "]";
}

TEST(NewtonRaphson, OneDim) {
  VXd exp = VXd::Constant(1, 1, +0.793700526);
  auto fn = [](VXd d) { return VXd::Constant(1, 1, std::pow(d[0], 3) - 0.5); };
  // Validate that the zero is where it's expected.
  ASSERT_LT(fn(exp).array().abs().maxCoeff(), 1e-10);

  auto res = NewtonRaphson(fn, /*dim=*/1);
  EXPECT_LT((res - exp).array().abs().maxCoeff(), 3e-4)
      << "[" << res.transpose() << "] != [" << exp.transpose() << "], ["
      << (res - exp).transpose() << "]";
}

}  // namespace tbd