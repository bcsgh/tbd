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

#include "tbd/dimensions_simplfy.h"

#include "absl/strings/str_join.h"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/dimensions.h"

namespace tbd {
namespace test_impl {
const std::vector<Dimension>& Classes() {
  static const auto *ret = new std::vector<Dimension>{
        //m   kg  s   A   K   mol cd
        {+1,  0,  0,  0,  0,  0,  0},  // m
        { 0, +1,  0,  0,  0,  0,  0},  // kg
        { 0,  0, +1,  0,  0,  0,  0},  // s
        { 0,  0,  0, +1,  0,  0,  0},  // A
        { 0,  0,  0,  0, +1,  0,  0},  // K
        { 0,  0,  0,  0,  0, +1,  0},  // mol
        { 0,  0,  0,  0,  0,  0, +1},  // cd

        {+3,  0,  0,  0,  0,  0,  0},  // L
        { 0,  0, -1,  0,  0,  0,  0},  // Hz = s^-1

        { 0,  0, +1, +1,  0,  0,  0},  // C

        {+2, +1, -3,  0,  0,  0,  0},  // W
        {+1, +1, -2,  0,  0,  0,  0},  // N
        {-1, +1, -2,  0,  0,  0,  0},  // Pa
        {+2, +1, -2,  0,  0,  0,  0},  // J

        { 0, +1, -2, -1,  0,  0,  0},  // T
        {-2, -1, +4, +2,  0,  0,  0},  // F
        {+2, +1, -2, -1,  0,  0,  0},  // Wb
        {+2, +1, -2, -2,  0,  0,  0},  // H
        {+2, +1, -3, -1,  0,  0,  0},  // V
        {+2, +1, -3, -2,  0,  0,  0},  // Ohm
        {-2, -1, +3, +2,  0,  0,  0},  // S = Ohm^-1
  };
  return *ret;
}
}  // namespace test_impl

namespace {

using unit = Eigen::Matrix<double, 1, 7>;

unit Make(const Dimension& d) {
  unit ret;
  ret << static_cast<double>(d.l()),
         static_cast<double>(d.m()),
         static_cast<double>(d.t()),
         static_cast<double>(d.i()),
         static_cast<double>(d.k()),
         static_cast<double>(d.n()),
         static_cast<double>(d.j());
  return ret;
}


TEST(DimensionSimplifyTest, TODO) {
  const auto &all = test_impl::Classes();

  Dimension W = all[10];
  Dimension F = all[15];

  auto testcase = W/F;
  auto v_testcase = Make(testcase);

  std::vector<int> active;
  for (int i = all.size() - 1; i >= 0; i--) {
    if (v_testcase.dot(Make(all[i])) != 0) active.emplace_back(i);
  }
  EXPECT_TRUE(false) << absl::StrJoin(active, ", ");
}

}  // namespace
}  // namespace tbd
