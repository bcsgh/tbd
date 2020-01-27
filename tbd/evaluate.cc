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

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"
#include "absl/memory/memory.h"
#include "tbd/ast.h"
#include "tbd/find.h"
#include "tbd/newton_raphson.h"
#include "tbd/ops.h"
#include "tbd/select_solvable.h"
#include "tbd/semantic.h"

namespace tbd {

bool Evaluate::operator()(const Equality& e) {
  auto i = doc_->TryGetNode(&e);
  auto l = doc_->TryGetNode(e.left());
  auto r = doc_->TryGetNode(e.right());
  CHECK(i != nullptr) << e.location();
  CHECK(l != nullptr) << e.left()->location();
  CHECK(r != nullptr) << e.right()->location();

  if (l->resolved && r->resolved) {
    if (!i->equ_processed) {
      if (allow_conflict_) {
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, l, r));
        i->equ_processed = true;
        progress_ = true;
      } else {
        SYM_ERROR(e) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (l->resolved) {
    CHECK(!r->resolved);
    ops_->emplace_back(absl::make_unique<OpAssign>(i, l));
    ops_->emplace_back(absl::make_unique<OpAssign>(r, l));
    i->equ_processed = true;
    i->resolved = true;
    r->resolved = true;
    progress_ = true;
    return true;
  }

  if (r->resolved) {
    CHECK(!l->resolved);
    ops_->emplace_back(absl::make_unique<OpAssign>(i, r));
    ops_->emplace_back(absl::make_unique<OpAssign>(l, r));
    i->equ_processed = true;
    i->resolved = true;
    l->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const LiteralValue& l) {
  auto node = doc_->TryGetNode(&l);
  CHECK(node != nullptr) << l.location();
  CHECK(!node->resolved) << l.location();

  node->value = l.value();
  node->equ_processed = true;
  node->resolved = true;
  progress_ = true;

  return true;
}

bool Evaluate::operator()(const NamedValue& n) {
  auto node = doc_->TryGetNamedNode(n.name());
  CHECK(node != nullptr) << n.location();
  node->equ_processed = node->resolved;
  return node->resolved;
}

bool Evaluate::operator()(const PowerExp& e) {
  auto b = doc_->TryGetNode(e.base());
  auto exp = doc_->TryGetNode(&e);
  CHECK(b != nullptr) << e.location();
  CHECK(exp != nullptr) << e.location();

  if (b->resolved && exp->resolved) {
    if (!exp->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpExp>(anon, b, e.exp()));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, exp, anon));
        exp->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(e) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (b->resolved) {
    CHECK(!exp->resolved);
    ops_->emplace_back(absl::make_unique<OpExp>(exp, b, e.exp()));
    exp->equ_processed = true;
    exp->resolved = true;
    progress_ = true;
    return true;
  }

  if (exp->resolved) {
    CHECK(!b->resolved);
    if (e.exp() % 2 == 1) {
      ops_->emplace_back(absl::make_unique<OpExp>(b, exp, 1.0 / e.exp()));
      exp->equ_processed = true;
      b->resolved = true;
      progress_ = true;
      return true;
    } else {
      LOG(INFO) << "^ " << exp->resolved << b->resolved;
    }
  }

  return false;
}

bool Evaluate::operator()(const ProductExp& p) {
  auto l = doc_->TryGetNode(p.left());
  auto r = doc_->TryGetNode(p.right());
  auto n = doc_->TryGetNode(&p);
  CHECK(l != nullptr) << p.left()->location();
  CHECK(r != nullptr) << p.right()->location();
  CHECK(n != nullptr) << p.location();

  if (n->resolved && l->resolved && r->resolved) {
    if (!n->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpMul>(anon, l, r));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, n, anon));
        n->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(p) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (l->resolved && r->resolved) {
    CHECK(!n->resolved);
    ops_->emplace_back(absl::make_unique<OpMul>(n, l, r));
    n->equ_processed = true;
    n->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && l->resolved) {
    CHECK(!r->resolved);
    ops_->emplace_back(absl::make_unique<OpDiv>(r, n, l));
    n->equ_processed = true;
    r->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && r->resolved) {
    CHECK(!l->resolved);
    ops_->emplace_back(absl::make_unique<OpDiv>(l, n, r));
    n->equ_processed = true;
    l->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const QuotientExp& q) {
  auto l = doc_->TryGetNode(q.left());
  auto r = doc_->TryGetNode(q.right());
  auto n = doc_->TryGetNode(&q);
  CHECK(l != nullptr) << q.left()->location();
  CHECK(r != nullptr) << q.right()->location();
  CHECK(n != nullptr) << q.location();

  if (n->resolved && l->resolved && r->resolved) {
    if (!n->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpDiv>(anon, l, r));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, n, anon));
        n->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(q) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (l->resolved && r->resolved) {
    CHECK(!n->resolved);
    ops_->emplace_back(absl::make_unique<OpDiv>(n, l, r));
    n->equ_processed = true;
    n->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && r->resolved) {
    CHECK(!l->resolved);
    ops_->emplace_back(absl::make_unique<OpMul>(l, n, r));
    n->equ_processed = true;
    l->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && l->resolved) {
    CHECK(!r->resolved);
    ops_->emplace_back(absl::make_unique<OpDiv>(r, l, n));
    n->equ_processed = true;
    r->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const SumExp& s) {
  auto l = doc_->TryGetNode(s.left());
  auto r = doc_->TryGetNode(s.right());
  auto n = doc_->TryGetNode(&s);
  CHECK(l != nullptr) << s.left()->location();
  CHECK(r != nullptr) << s.right()->location();
  CHECK(n != nullptr) << s.location();

  if (n->resolved && l->resolved && r->resolved) {
    if (!n->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpAdd>(anon, l, r));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, n, anon));
        n->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(s) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (l->resolved && r->resolved) {
    CHECK(!n->resolved);
    ops_->emplace_back(absl::make_unique<OpAdd>(n, l, r));
    n->equ_processed = true;
    n->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && l->resolved) {
    CHECK(!r->resolved);
    ops_->emplace_back(absl::make_unique<OpSub>(r, n, l));
    n->equ_processed = true;
    r->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && r->resolved) {
    CHECK(!l->resolved);
    ops_->emplace_back(absl::make_unique<OpSub>(l, n, r));
    n->equ_processed = true;
    l->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const DifExp& d) {
  auto l = doc_->TryGetNode(d.left());
  auto r = doc_->TryGetNode(d.right());
  auto n = doc_->TryGetNode(&d);
  CHECK(l != nullptr) << d.left()->location();
  CHECK(r != nullptr) << d.right()->location();
  CHECK(n != nullptr) << d.location();

  if (n->resolved && l->resolved && r->resolved) {
    if (!n->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpSub>(anon, l, r));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, n, anon));
        n->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(d) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (l->resolved && r->resolved) {
    CHECK(!n->resolved);
    ops_->emplace_back(absl::make_unique<OpSub>(n, l, r));
    n->equ_processed = true;
    n->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && l->resolved) {
    CHECK(!r->resolved);
    ops_->emplace_back(absl::make_unique<OpSub>(r, l, n));
    n->equ_processed = true;
    r->resolved = true;
    progress_ = true;
    return true;
  }

  if (n->resolved && r->resolved) {
    CHECK(!l->resolved);
    ops_->emplace_back(absl::make_unique<OpAdd>(l, r, n));
    n->equ_processed = true;
    l->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const NegativeExp& n) {
  auto b = doc_->TryGetNode(n.value());
  auto exp = doc_->TryGetNode(&n);
  CHECK(b != nullptr) << n.value()->location();
  CHECK(exp != nullptr) << n.location();

  if (b->resolved && exp->resolved) {
    if (!exp->equ_processed) {
      if (allow_conflict_) {
        auto anon = doc_->GetNode();
        ops_->emplace_back(absl::make_unique<OpNeg>(anon, b));
        ops_->emplace_back(absl::make_unique<OpCheck>(out_idx_++, exp, anon));
        exp->equ_processed = true;
        anon->equ_processed = true;
        anon->resolved = true;
        progress_ = true;
      } else {
        SYM_ERROR(n) << "Conflicting result";
        error_ = true;
        return false;
      }
    }
    return true;
  }

  if (b->resolved) {
    CHECK(!exp->resolved);
    ops_->emplace_back(absl::make_unique<OpNeg>(exp, b));
    exp->equ_processed = true;
    exp->resolved = true;
    progress_ = true;
    return true;
  }

  if (exp->resolved) {
    CHECK(!b->resolved);
    ops_->emplace_back(absl::make_unique<OpNeg>(b, exp));
    exp->equ_processed = true;
    b->resolved = true;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const Define& d) {
  auto node = doc_->TryGetNamedNode(d.name());
  CHECK(node != nullptr) << d.location();
  CHECK(!node->resolved) << d.location();
  CHECK(node->unit.has_value()) << d.location();

  node->value = d.value() * node->unit->scale;
  node->equ_processed = true;
  node->resolved = true;
  progress_ = true;

  return true;
}

bool Evaluate::DirectEvaluateNodes(
    std::set<const ExpressionNode*, StableNodeCompare>* nodes) {
  // For every pass where progress is made, at least one node is
  // removed from nodes so watching that bounds the number of passes.
  bool made_progress = false;
  progress_ = true;
  for (int p = 0; (progress_ && !nodes->empty()); p++) {
    LOG(INFO) << "Pass " << p << ", " << nodes->size() << " nodes un-resolved";
    progress_ = false;
    for (auto it = nodes->begin(); it != nodes->end();) {
      if ((*it)->VisitNode(this)) {
        it = nodes->erase(it);
        progress_ = true;
        made_progress = true;
      } else {
        ++it;
      }
    }
  }
  LOG(INFO) << nodes->size() << " nodes not resolved";
  return made_progress;
}

bool Evaluate::operator()(const Document& d) {
  // Collect the set of expressions.
  std::set<const ExpressionNode*, StableNodeCompare> nodes;
  for (auto* exp : doc_->nodes()) {
    if (exp && exp->node) nodes.insert(exp->node);
  }

  // Processes all values on defines first.
  for (auto d : d.defines()) {
    CHECK(d->VisitNode(this)) << d;
    CHECK(nodes.erase(static_cast<const ExpressionNode*>(d)) == 1) << d;
  }

  if (error_) return false;

  // The sequence of ops that solve for all the direct solutions.
  stages_.emplace_back();
  auto& stage = stages_.back();
  ops_ = &stage.direct_ops;

  {
    // Find all the literals.
    Find<LiteralValue> literal;
    for (auto const* e : nodes) {
      (void)e->VisitNode(&literal);
    }
    std::set<const ExpressionNode*, StableNodeCompare> l = {
        literal.nodes().begin(), literal.nodes().end()};

    // Process and remove them first becasue they will always
    // process and it makes the error message better.
    for (const auto* n : l) nodes.erase(n);
    DirectEvaluateNodes(&l);
    CHECK(l.empty());
  }
  allow_conflict_ = false;
  DirectEvaluateNodes(&nodes);

  if (error_) return false;

  if (!nodes.empty()) {
    LOG(INFO) << "Finding solvable systems";

    FindUnsolvedRoots roots{doc_};
    for (const auto* e : d.equality()) (void)e->VisitNode(&roots);
    const auto& all = roots.Unsolved();
    LOG(INFO) << "Found " << all.size() << " unresolved components.";

    // Select a small system to solve.
    std::set<ExpressionNode const*, StableNodeCompare> exp_result;
    std::set<std::string> var_result;
    CHECK(FindSolution(all, &exp_result, &var_result))
        << "Failed to select solvable set";

    // Find the involved expressions
    Find<ExpressionNode> find;
    for (auto const* e : exp_result) (void)e->VisitNode(&find);

    // Collect the unresolved into exp_result.
    std::vector<ExpressionNode const*> exp_bits_v =
        find.NodesWhere([this, exp_result](const ExpressionNode* n) {
          return !doc_->TryGetNode(n)->resolved;
        });
    exp_result.insert(exp_bits_v.begin(), exp_bits_v.end());

    ops_ = &stage.solve_ops;  // Switch the output
    allow_conflict_ = true;   // Emit OpCheck
    in_idx_ = out_idx_ = 0;   // Starting in and out at zero
    while (!var_result.empty()) {
      // Pick a variable.
      auto pick = var_result.begin();
      auto node = doc_->TryGetNamedNode(*pick);
      var_result.erase(pick);
      if (node->equ_processed) continue;

      // "Resolve" the picked var.
      node->resolved = true;
      node->equ_processed = true;
      ops_->emplace_back(absl::make_unique<OpLoad>(node, in_idx_++));

      // Figure out what else that pins.
      DirectEvaluateNodes(&exp_result);
    }
    CHECK(in_idx_ == out_idx_) << in_idx_ << "!=" << out_idx_;
    stage.count = in_idx_;
  }

  // Run the evaluation plan.
  VXd in, out;
  DirectEvaluate eval(&in, &out);
  for (const auto& op : stage.direct_ops) (void)op->VisitOp(&eval);

  if (!stage.solve_ops.empty()) {
    in.resize(stage.count);
    out.resize(stage.count);

    // Convert a vector of values guesses into a vector or errors.
    auto fn = [&stage, &in, &out, &eval](const VXd& in_p) {
      CHECK(stage.count == in_p.size()) << stage.count << "!=" << in_p.size();
      in = in_p;
      for (const auto& op : stage.solve_ops) (void)op->VisitOp(&eval);
      return out;
    };

    NewtonRaphson(fn, /*dim=*/stage.count, /*count=*/10, /*tol=*/1e-4);
  }
  LOG(INFO) << "==== DONE ====";

  return !error_;
}

bool FindUnsolvedRoots::Resolved(tbd::ExpressionNode const* e) {
  // If this node is resolved, keep searching.
  auto n = doc_->TryGetNode(e);
  CHECK(n != nullptr) << e->location();
  if (n->equ_processed) return false;

  // This is a root, so find the free variables.
  Find<NamedValue> find_named;
  (void)e->VisitNode(&find_named);

  // filter to the free ones.
  auto free_named = find_named.NodesWhere([this](const tbd::NamedValue* n) {
    return !doc_->TryGetNode(n)->resolved;
  });

  // Extract names.
  std::set<std::string> vars;
  for (const auto* n : free_named) vars.insert(n->name());

  CHECK(unsolved_.emplace(e, std::move(vars)).second) << e->location();

  return true;
}

bool FindUnsolvedRoots::operator()(const Equality& e) {
  return Resolved(&e) ||  //
         e.left()->VisitNode(this) || e.right()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const PowerExp& e) {
  return Resolved(&e) ||  //
         e.base()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const ProductExp& p) {
  return Resolved(&p) ||  //
         p.left()->VisitNode(this) || p.right()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const QuotientExp& q) {
  return Resolved(&q) ||  //
         q.left()->VisitNode(this) || q.right()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const SumExp& s) {
  return Resolved(&s) ||  //
         s.left()->VisitNode(this) || s.right()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const DifExp& d) {
  return Resolved(&d) ||  //
         d.left()->VisitNode(this) || d.right()->VisitNode(this);
}

bool FindUnsolvedRoots::operator()(const NegativeExp& n) {
  return Resolved(&n) ||  //
         n.value()->VisitNode(this);
}

}  // namespace tbd
