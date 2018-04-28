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

#ifndef TBD_EVALUATE_H_
#define TBD_EVALUATE_H_

#include "tbd/ast.h"
#include "tbd/common.h"
#include "tbd/semantic.h"

namespace tbd {

class Evaluate final : public VisitNodes {
 public:
  Evaluate(SemanticDocument* doc) : doc_(doc){};

 private:
  bool operator()(const UnitExp&) override {
    LOG(FATAL);
    return true;
  }
  bool operator()(const UnitDef&) override {
    LOG(FATAL);
    return true;
  }

  bool operator()(const Equality&) override;
  bool operator()(const LiteralValue&) override;
  bool operator()(const NamedValue&) override;
  bool operator()(const PowerExp&) override;
  bool operator()(const ProductExp&) override;
  bool operator()(const QuotientExp&) override;
  bool operator()(const SumExp&) override;
  bool operator()(const DifExp&) override;
  bool operator()(const NegativeExp&) override;

  bool operator()(const Define&) override;
  bool operator()(const Specification&) override { return false; }
  bool operator()(const Document&) override;

  void ReportConflict(const ExpressionNode& e) { conflicts_.push_back(&e); }

  SemanticDocument* doc_;
  bool error_ = false;     // Set if an expression evaluation yields an error.
  bool progress_ = false;  // Set when a expressions value it found.
  std::vector<const ExpressionNode*> conflicts_;
};

}  // namespace tbd

#endif  // TBD_EVALUATE_H_
