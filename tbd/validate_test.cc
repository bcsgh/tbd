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

#include "tbd/validate.h"

#include "absl/memory/memory.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/common.h"

namespace tbd {
namespace {

const int _i = logging::InstallSignalhandler();

std::unique_ptr<LiteralValue> Make(float l) {
  return absl::make_unique<LiteralValue>(Loc{}, l);
}

TEST(Validate, TODO) {
  Loc l{};
  double e = 2.7182, p = 3.1415;

  Equality equ{Loc{}, Make(e), Make(p)};
  PowerExp pow{Loc{}, Make(e), 3};
  ProductExp mul{Make(e), Make(p)};
  QuotientExp div{Make(e), Make(p)};
  SumExp add{Make(e), Make(p)};
  DifExp sub{Make(e), Make(p)};
  NegativeExp neg{Loc{}, Make(e)};

  SemanticDocument doc;
  Validate val{&doc};

  const ExpressionNode* all[] = {&equ, &pow, &mul, &div, &add, &sub, &neg};

  // Validate everything
  for (const ExpressionNode* exp : all) {
    (void)exp->VisitNode(&val);
    EXPECT_NE(doc.TryGetNode(exp), nullptr);
  }
}

}  // namespace
}  // namespace tbd