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

#include "tbd/semantic.h"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/ast.h"
#include "tbd/common.h"

namespace tbd {
namespace {

TEST(SemanticDocument, Basic) {
  SemanticDocument doc;

  // Basic types.
  EXPECT_NE(doc.LookupUnit("m"), nullptr);
  EXPECT_NE(doc.LookupUnit("s"), nullptr);
  EXPECT_NE(doc.LookupUnit("kg"), nullptr);
  EXPECT_NE(doc.LookupUnit("A"), nullptr);
  EXPECT_NE(doc.LookupUnit("K"), nullptr);
  EXPECT_NE(doc.LookupUnit("mol"), nullptr);
  EXPECT_NE(doc.LookupUnit("cd"), nullptr);

  // Check for something not there.
  EXPECT_EQ(doc.LookupUnit("foo"), nullptr);

  // Add it and check again.
  EXPECT_TRUE(doc.AddUnit("foo", Unit::m()));
  EXPECT_NE(doc.LookupUnit("foo"), nullptr);

  // Try to add the same thing again, and check for it again.
  EXPECT_FALSE(doc.AddUnit("foo", Unit::m()));
  EXPECT_NE(doc.LookupUnit("foo"), nullptr);
}

TEST(SemanticDocument, Dump) {
  using testing::HasSubstr;

  SemanticDocument doc;
  std::stringstream out(std::ios_base::out);
  doc.LogUnits(out);

  EXPECT_THAT(out.str(), HasSubstr("A = 1[A]\n"));
  EXPECT_THAT(out.str(), HasSubstr("K = 1[K]\n"));
  EXPECT_THAT(out.str(), HasSubstr("cd = 1[cd]\n"));
  EXPECT_THAT(out.str(), HasSubstr("kg = 1[kg]\n"));
  EXPECT_THAT(out.str(), HasSubstr("m = 1[m]\n"));
  EXPECT_THAT(out.str(), HasSubstr("mol = 1[mol]\n"));
  EXPECT_THAT(out.str(), HasSubstr("s = 1[s]\n"));
}

TEST(SemanticDocument, Unnamed) {
  SemanticDocument doc;

  LiteralValue v1(Loc{}, 0), v2(Loc{}, 0);

  EXPECT_EQ(doc.TryGetNode(&v1), nullptr);  // Not yet added

  auto* e1 = doc.GetUnnamedNode(&v1);
  ASSERT_NE(e1, nullptr);
  EXPECT_EQ(&v1, e1->node);

  auto* e = doc.GetUnnamedNode(&v1);
  ASSERT_NE(e, nullptr);
  EXPECT_EQ(&v1, e->node);  // Adding a second time just return the first.

  EXPECT_EQ(e1, doc.TryGetNode(&v1));

  EXPECT_EQ(doc.TryGetNode(&v2), nullptr);  // Another node, not yet known

  auto* e2 = doc.GetUnnamedNode(&v2);
  ASSERT_NE(e2, nullptr);
  EXPECT_EQ(&v2, e2->node);

  EXPECT_EQ(e2, doc.TryGetNode(&v2));
  EXPECT_EQ(e1, doc.TryGetNode(&v1));
}

TEST(SemanticDocument, Named) {
  SemanticDocument doc;

  Define v1(Loc{}, "n1", 1);
  Define v2(Loc{}, "n2", 2);

  EXPECT_EQ(doc.TryGetNode(&v1), nullptr);  // Not yet added

  auto* e1 = doc.GetNamedNode(&v1);
  ASSERT_NE(e1, nullptr);
  EXPECT_EQ(&v1, e1->node);

  auto* e = doc.GetNamedNode(&v1);
  EXPECT_EQ(e, e1);
  EXPECT_EQ(&v1, e->node);  // Adding a second time fails

  EXPECT_EQ(e1, doc.TryGetNode(&v1));

  EXPECT_EQ(doc.TryGetNode(&v2), nullptr);  // Another node, not yet known

  auto* e2 = doc.GetNamedNode(&v2);
  ASSERT_NE(e2, nullptr);
  EXPECT_EQ(&v2, e2->node);

  EXPECT_EQ(e1, doc.TryGetNode(&v1));
  EXPECT_EQ(e2, doc.TryGetNode(&v2));
  EXPECT_EQ(e1, doc.TryGetNamedNode("n1"));
  EXPECT_EQ(e2, doc.TryGetNamedNode("n2"));

  // Now try with adding references
  NamedValue n1(Loc{}, "n1");
  NamedValue n3(Loc{}, "n3");

  EXPECT_EQ(doc.TryGetNamedNode("n3"), nullptr);

  EXPECT_EQ(doc.RefernceNamedNode(&n1), e1);
  auto e3 = doc.RefernceNamedNode(&n3);
  EXPECT_NE(e3, nullptr);

  EXPECT_EQ(e1, doc.TryGetNode(&n1));
  EXPECT_EQ(e3, doc.TryGetNode(&n3));
  EXPECT_EQ(e3, doc.TryGetNamedNode("n3"));
}

}  // namespace
}  // namespace tbd