// Copyright (c) 2018, Benjamin Shrselect_solvablehire,
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

#include "tbd/select_solvable.h"

#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/common.h"

namespace tbd {

namespace {

void StripEnds(absl::string_view* data) {
  while (absl::ConsumePrefix(data, " ")) {
  }
  while (absl::ConsumeSuffix(data, " ")) {
  }
}

void Go(int size_found, absl::string_view data) {
  // Extract the mapping.
  StripEnds(&data);

  std::vector<absl::string_view> rows =
      absl::StrSplit(data, '\n', absl::SkipEmpty());
  for (absl::string_view& row : rows) {
    StripEnds(&row);
    ASSERT_EQ(row.length(), rows.size()) << "'" << row << "'";
  }

  // Generate a shuffle (to enshure that presentation order doesn't matter).
  std::vector<int> r_shuf, c_shuf;
  std::set<int> used;
  for (int i = rows.size(); i; i--) {
    int n;
    while (!used.insert(n = std::rand() & 0x1f).second) {
    }
    r_shuf.push_back(n);
    while (!used.insert(n = std::rand() & 0x1f).second) {
    }
    c_shuf.push_back(n);
  }

  // Populate the from-to mapping using the shuffle
  std::map<int, std::set<int>> from_to;
  for (std::size_t i = 0; i < rows.size(); i++) {
    auto& to = from_to[r_shuf[i]];
    for (std::size_t j = 0; j < rows[i].length(); j++) {
      if (rows[i][j] == '1') to.insert(c_shuf[j]);
    }
  }

  // Compute the expected set of rows and columns
  std::set<int> r_exp, c_exp;
  for (int i = 0; i < size_found; i++) {
    r_exp.insert(r_shuf[i]);
    c_exp.insert(c_shuf[i]);
  }

  std::set<int> r_result, c_result;

  ASSERT_TRUE(FindSolution<>(from_to, &r_result, &c_result));

  EXPECT_THAT(r_result, testing::ElementsAreArray(r_exp));
  EXPECT_THAT(c_result, testing::ElementsAreArray(c_exp));
}

TEST(SelectSolvable, Bad) {
  // This should never be needed, but check that one-unknown gets picked.
  Go(1, R"(
    100
    011
    011
  )");
}

TEST(SelectSolvable, MinimalDeg1Case) {
  // The minimal case.
  Go(2, R"(
    11
    11
  )");
}

TEST(SelectSolvable, NonMinimalDeg1Case) {
  // A non-minimal 1 degree of freedom case.
  Go(3, R"(
    110
    101
    011
  )");
}

TEST(SelectSolvable, Deg1SmallDeg1) {
  // A two 1 degree cases of different sizes, return the smaller one.
  Go(2, R"(
    11000
    11000
    00110
    00101
    00011
  )");
}

TEST(SelectSolvable, Deg2In4) {
  // A 3 degree case that yeilds a sub set.
  Go(3, R"(
    1110
    1110
    1110
    1001
  )");
}

TEST(SelectSolvable, Odd) {
  // A case with some equations-of-two-varables that reduce
  // some of-three's into of-two's and make things solvable
  Go(5, R"(
    11000000
    11100000
    10110000
    00111000
    00011000
    00000111
    00000111
    00000111
  )");
}

TEST(SelectSolvable, Deg1Deg2) {
  // A case with both a 1 and 3 degree set.
  Go(2, R"(
    11000
    11000
    00111
    00111
    00111
  )");
}

TEST(SelectSolvable, Deg1In4Deg2In3) {
  // A case with a 1 degree, 4 part set
  // and a 3 degree, 3 parts set.
  Go(4, R"(
    1100000
    0110000
    0011000
    1001000
    0000111
    0000111
    0000111
  )");
}

}  // namespace
}  // namespace tbd