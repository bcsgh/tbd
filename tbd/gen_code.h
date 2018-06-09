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

#ifndef TBD_GEN_CODE_H_
#define TBD_GEN_CODE_H_

#include <map>
#include <ostream>
#include <string>

#include "absl/strings/string_view.h"
#include "tbd/ops.h"
#include "tbd/semantic.h"

namespace tbd {

////////////////////////////////////////////

// Direct in place evaluation.
class CodeEvaluate final : public VisitOps {
 public:
  CodeEvaluate(std::ostream& out) : out_(out) {}

  ABSL_MUST_USE_RESULT bool operator()(const OpAdd&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpSub&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpMul&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpDiv&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpNeg&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpExp&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpAssign&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpLoad&) override;
  ABSL_MUST_USE_RESULT bool operator()(const OpCheck&) override;

 private:
  using ExpP = const SemanticDocument::Exp*;

  bool BinaryOp(ExpP r, ExpP a, ExpP b, absl::string_view op);
  bool Add(ExpP, std::string);

  std::ostream& out_;
  std::map<const SemanticDocument::Exp*, std::string> expressions_;
};

}  // namespace tbd

#endif  // TBD_GEN_CODE_H_
