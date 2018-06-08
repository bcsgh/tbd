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

#include "tbd/gen_code.h"

#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/common.h"

namespace tbd {
namespace {
const int _i = logging::InstallSignalhandler();

TEST(TestCodeEvaluate, VisitAdd) {
  std::stringstream out(std::ios_base::out);
  CodeEvaluate code(out);

  EXPECT_THAT(out.str(), testing::Eq(""));

  // Add some operations.
  SemanticDocument::Exp A, B;
  SemanticDocument::Exp R1, R2, R3;
  SemanticDocument::Exp T1, T2, T3;
  A.name = "a";
  B.name = "b";
  T1.name = "T1";
  T2.name = "T2";
  T3.name = "T3";

  OpAdd a(&R1, &A, &B);
  OpAssign e1(&T1, &R1);
  OpSub s(&R2, &T1, &B);
  OpAssign e2(&T2, &R2);
  OpMul m(&R3, &R1, &R1);
  OpAssign e3(&T3, &R3);

  out << "\n";

  OpI* all_ops[] = {&a, &e1, &s, &e2, &m, &e3};
  for (OpI* op : all_ops) EXPECT_TRUE(op->VisitOp(&code));

  EXPECT_THAT(out.str(), testing::Eq(R"(
T1 = (a + b);
T2 = (T1 - b);
T3 = ((a + b) * (a + b));
)"));
}

}  // namespace
}  // namespace tbd