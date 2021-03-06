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

#include "tbd/ast.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tbd {
namespace {

// Some types to check things out.
static const std::string fn = "foo";
struct loc_p {
  const std::string* filename = &fn;
  int line = 123;
  int column = 456;
};
struct loc {
  loc_p begin;
  loc_p end;
};

struct TestNode final : NodeI {
  using NodeI::NodeI;

  ABSL_MUST_USE_RESULT bool Visit(VisitNodes*) const override { return true; }
};

TEST(TestNodeI, Basic) {
  TestNode node(loc{});

  EXPECT_EQ(node.source_file(), "foo");
  EXPECT_EQ(node.source_line(), 123);
}

class TestVisitor : public VisitNodesWithErrors {
 public:
  using VisitNodesWithErrors::VisitNodesWithErrors;

  bool operator()(const UnitExp&) { return false; }
  bool operator()(const UnitDef&) { return false; }
  bool operator()(const Equality&) { return false; }
  bool operator()(const LiteralValue&) { return false; }
  bool operator()(const NamedValue&) { return false; }
  bool operator()(const PowerExp&) { return false; }
  bool operator()(const ProductExp&) { return false; }
  bool operator()(const QuotientExp&) { return false; }
  bool operator()(const SumExp&) { return false; }
  bool operator()(const DifExp&) { return false; }
  bool operator()(const NegativeExp&) { return false; }
  bool operator()(const Define&) { return false; }
  bool operator()(const Specification&) { return false; }
  bool operator()(const Document&) { return false; }
};

TEST(TestErrorMessage, Basic) {
  std::string err;
  TestVisitor sink([&err](const std::string& e) { err += e; });
  ErrorMessage(__FILE__, __LINE__, &sink, TestNode{loc{}}).get() << "Boo";
  EXPECT_EQ(err,
#ifndef NDEBUG
            "(tbd/ast_test.cc:83) "
#endif  // NDEBUG
            "foo:123:[456,456]: Boo\n");
}

}  // namespace
}  // namespace tbd