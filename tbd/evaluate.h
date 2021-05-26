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

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "glog/logging.h"
#include "tbd/ast.h"
#include "tbd/ops.h"
#include "tbd/semantic.h"

namespace tbd {

class Evaluate final : public VisitNodesWithErrors {
 public:
  Evaluate(SemanticDocument* doc, ErrorSink e)
      : VisitNodesWithErrors(e), doc_(doc) {}

  struct Stage {
    // The ops that directly solve for the parts where that works for.
    std::vector<std::unique_ptr<OpI>> direct_ops;
    // The ops that go from guesses of variables to errors.
    std::vector<std::unique_ptr<OpI>> solve_ops;
    // The number of variables to solve for.
    int count = 0;
  };

  std::vector<const Stage*> GetStages() const {
    std::vector<const Stage*> ret;
    ret.reserve(stages_.size());
    for (const auto& s : stages_) ret.push_back(&s);
    return ret;
  }

 private:
  bool operator()(const UnitExp&) override { LOG(FATAL); return false; }
  bool operator()(const UnitDef&) override { LOG(FATAL); return false; }

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

  bool DirectEvaluateNodes(
      std::set<const ExpressionNode*, StableNodeCompare>* nodes);

  SemanticDocument* doc_;

  bool error_ = false;     // Set if an expression evaluation yields an error.
  bool progress_ = false;  // Set when a expressions value it found.
  bool allow_conflict_ = false;  // Make OpCheck op rather than error on conflit

  std::vector<Stage> stages_;

  // The count of set vars and checked equarions
  // handled so far. Used to assign indexes to them.
  int in_idx_ = 0, out_idx_ = 0;
  std::vector<std::unique_ptr<OpI>>* ops_;
};

class FindUnsolvedRoots final : public VisitNodes {
 public:
  FindUnsolvedRoots(SemanticDocument* doc) : doc_(doc){};

  const std::map<const ExpressionNode*, std::set<std::string>>& Unsolved() {
    return unsolved_;
  }

 private:
  bool operator()(const UnitExp&) override { return false; }
  bool operator()(const UnitDef&) override { return false; }

  bool Resolved(const ExpressionNode*);

  bool operator()(const Equality&) override;
  bool operator()(const LiteralValue&) override { return false; }
  bool operator()(const NamedValue&) override { return false; }
  bool operator()(const PowerExp&) override;
  bool operator()(const ProductExp&) override;
  bool operator()(const QuotientExp&) override;
  bool operator()(const SumExp&) override;
  bool operator()(const DifExp&) override;
  bool operator()(const NegativeExp&) override;

  bool operator()(const Define&) override { return false; }
  bool operator()(const Specification&) override { return false; }
  bool operator()(const Document&) override { return false; }

  SemanticDocument* doc_;
  std::map<const ExpressionNode*, std::set<std::string>> unsolved_;
};

}  // namespace tbd

#endif  // TBD_EVALUATE_H_
