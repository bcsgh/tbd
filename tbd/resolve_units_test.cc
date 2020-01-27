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

#include "tbd/resolve_units.h"

#include <memory>
#include <string>
#include <utility>

#include "absl/memory/memory.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/ast.h"
#include "tbd/semantic.h"

namespace tbd {

class ResolveUnitsTest : public ::testing::Test {
 public:
  static std::unique_ptr<std::string> str(std::string s) {
    return absl::make_unique<std::string>(std::move(s));
  }
};

namespace {

TEST_F(ResolveUnitsTest, Smoke) {
  Document doc;
  SemanticDocument sem;
  EXPECT_EQ(sem.LookupUnit("w"), nullptr);
  EXPECT_EQ(sem.LookupUnit("x"), nullptr);
  EXPECT_EQ(sem.LookupUnit("y"), nullptr);
  EXPECT_EQ(sem.LookupUnit("z"), nullptr);

  ResolveUnits ru(&sem);

  Loc l;
  doc.AddUnitDefinition(absl::make_unique<UnitDef>(
      l, str("w"), absl::make_unique<LiteralValue>(l, 3),
      absl::make_unique<UnitExp>(l)));

  auto ue = absl::make_unique<UnitExp>(l);
  ue->Mul(absl::WrapUnique(new UnitExp::UnitT{"w", 1, l}));
  doc.AddUnitDefinition(absl::make_unique<UnitDef>(
      l, str("x"), absl::make_unique<LiteralValue>(l, 5), std::move(ue)));

  ue = absl::make_unique<UnitExp>(l);
  ue->Mul(absl::WrapUnique(new UnitExp::UnitT{"w", 2, l}));
  doc.AddUnitDefinition(absl::make_unique<UnitDef>(
      l, str("y"), absl::make_unique<LiteralValue>(l, 7), std::move(ue)));

  ASSERT_TRUE(doc.VisitNode(&ru));

  auto w = sem.LookupUnit("w");
  auto x = sem.LookupUnit("x");
  auto y = sem.LookupUnit("y");
  EXPECT_EQ(sem.LookupUnit("z"), nullptr);

  ASSERT_NE(w, nullptr);
  EXPECT_EQ(w->scale, 3);

  ASSERT_NE(x, nullptr);
  EXPECT_EQ(x->scale, 3 * 5);

  ASSERT_NE(y, nullptr);
  EXPECT_EQ(y->scale, 3 * 3 * 7);
}

}  // namespace
}  // namespace tbd
