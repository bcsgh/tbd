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

#include "tbd/dimensions.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tbd {

class DimensionTest : public ::testing::Test {
 public:
  static Dimension::D zero() { return Dimension::D::zero(); }
  static Dimension::D one() { return Dimension::D::one(); }
  static Dimension::D value(int n, int d) { return Dimension::D{n, d}; }
};

namespace {

TEST_F(DimensionTest, D) {
  auto z = zero();
  auto o = one();

  EXPECT_EQ(z, z);
  EXPECT_EQ(o, o);
  EXPECT_NE(z, o);

  EXPECT_EQ(z, o * 0);
  EXPECT_EQ(o, o * 1);
  EXPECT_EQ(z, z * 5);

  EXPECT_EQ(o + o, o * 2);
  EXPECT_EQ(o - o, z);
  EXPECT_EQ(o * 2 - o, o);

  EXPECT_EQ(o * 3 / 6, o / 2);
  EXPECT_NE(o * 4 / 6, o / 2);
  EXPECT_EQ(o * 4 / 6, o * 2 / 3);

  EXPECT_EQ(value(5, 3), value(-5, -3));
  EXPECT_EQ(value(-5, 3), value(5, -3));
}

TEST_F(DimensionTest, DimensionEq) {
  EXPECT_EQ(Dimension::L(), Dimension::L());
  EXPECT_NE(Dimension::L(), Dimension::M());
  EXPECT_NE(Dimension::L(), Dimension::T());
  EXPECT_NE(Dimension::L(), Dimension::I());
  EXPECT_NE(Dimension::L(), Dimension::K());
  EXPECT_NE(Dimension::L(), Dimension::N());
  EXPECT_NE(Dimension::L(), Dimension::J());

  EXPECT_EQ(Dimension::M(), Dimension::M());
  EXPECT_NE(Dimension::M(), Dimension::T());
  EXPECT_NE(Dimension::M(), Dimension::I());
  EXPECT_NE(Dimension::M(), Dimension::K());
  EXPECT_NE(Dimension::M(), Dimension::N());
  EXPECT_NE(Dimension::M(), Dimension::J());

  EXPECT_EQ(Dimension::T(), Dimension::T());
  EXPECT_NE(Dimension::T(), Dimension::I());
  EXPECT_NE(Dimension::T(), Dimension::K());
  EXPECT_NE(Dimension::T(), Dimension::N());
  EXPECT_NE(Dimension::T(), Dimension::J());

  EXPECT_EQ(Dimension::I(), Dimension::I());
  EXPECT_NE(Dimension::I(), Dimension::K());
  EXPECT_NE(Dimension::I(), Dimension::N());
  EXPECT_NE(Dimension::I(), Dimension::J());

  EXPECT_EQ(Dimension::K(), Dimension::K());
  EXPECT_NE(Dimension::K(), Dimension::N());
  EXPECT_NE(Dimension::K(), Dimension::J());

  EXPECT_EQ(Dimension::N(), Dimension::N());
  EXPECT_NE(Dimension::N(), Dimension::J());

  EXPECT_EQ(Dimension::J(), Dimension::J());
}

TEST_F(DimensionTest, DimensionMath) {
  EXPECT_EQ(Dimension::L() * Dimension::L(), pow(Dimension::L(), 2));
  EXPECT_NE(Dimension::L() * Dimension::L(), pow(Dimension::T(), 2));
  EXPECT_NE(Dimension::L() * Dimension::L(), pow(Dimension::L(), 3));

  EXPECT_EQ(Dimension::L(), pow(Dimension::L(), 2) / Dimension::L());
  EXPECT_EQ(Dimension::L(), root(Dimension::L() * Dimension::L(), 2));
}

TEST_F(DimensionTest, Output) {
  EXPECT_EQ(Dimension::L().to_str(), "[m]");
  EXPECT_EQ(Dimension::M().to_str(), "[kg]");
  EXPECT_EQ(Dimension::T().to_str(), "[s]");
  EXPECT_EQ(Dimension::I().to_str(), "[A]");
  EXPECT_EQ(Dimension::K().to_str(), "[K]");
  EXPECT_EQ(Dimension::N().to_str(), "[mol]");
  EXPECT_EQ(Dimension::J().to_str(), "[cd]");

  auto vel = Dimension::L() / Dimension::T();
  auto acc = vel / Dimension::T();
  auto frc = Dimension::M() * acc;
  auto eng = frc * Dimension::L();
  auto pow = eng / Dimension::T();
  auto vol = pow / Dimension::I();
  auto res = vol / Dimension::I();

  EXPECT_EQ(vel.to_str(), "[m,s^-1]");
  EXPECT_EQ(acc.to_str(), "[m,s^-2]");
  EXPECT_EQ(frc.to_str(), "[m,kg,s^-2]");
  EXPECT_EQ(eng.to_str(), "[m^2,kg,s^-2]");
  EXPECT_EQ(pow.to_str(), "[m^2,kg,s^-3]");
  EXPECT_EQ(vol.to_str(), "[m^2,kg,s^-3,A^-1]");
  EXPECT_EQ(res.to_str(), "[m^2,kg,s^-3,A^-2]");
}

}  // namespace
}  // namespace tbd
