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

#include "tbd/evaluate.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tbd {

TEST(Evaluate, Smoke) {
  SemanticDocument doc;
  Evaluate roots{&doc};
}

TEST(FindUnsolvedRoots, Smoke) {
  SemanticDocument doc;
  FindUnsolvedRoots roots{&doc};
}

TEST(FindUnsolvedRoots, Try) {
  auto ua = absl::make_unique<NamedValue>(Loc{}, "a");
  auto ub = absl::make_unique<NamedValue>(Loc{}, "b");
  auto a = ua.get();
  auto b = ub.get();

  auto us = absl::make_unique<SumExp>(std::move(ua), std::move(ub));
  auto ul = absl::make_unique<LiteralValue>(Loc{}, 2);
  auto s = us.get();
  auto l = ul.get();
  Equality e{Loc{}, std::move(us), std::move(ul)};

  SemanticDocument doc;
  doc.RefernceNamedNode(a);
  doc.RefernceNamedNode(b);
  doc.GetUnnamedNode(s);
  doc.GetUnnamedNode(l)->equ_processed = true;
  doc.GetUnnamedNode(&e)->equ_processed = true;

  FindUnsolvedRoots roots{&doc};
  (void)e.VisitNode(&roots);

  const auto& all = roots.Unsolved();
  ASSERT_EQ(all.size(), 1);

  auto& it = *all.begin();
  EXPECT_EQ(it.first, s);
  EXPECT_EQ(it.second.size(), 2);
}

}  // namespace tbd
