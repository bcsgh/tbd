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

#include "tbd/ast.h"
#include "tbd/common.h"
#include "tbd/semantic.h"

namespace tbd {

bool Evaluate::operator()(const Equality& e) {
  auto l = doc_->TryGetNode(e.left());
  auto r = doc_->TryGetNode(e.right());
  CHECK(l != nullptr) << e.left()->location();
  CHECK(r != nullptr) << e.right()->location();

  if (l->value.has_value() && r->value.has_value()) {
    if (*l->value != *r->value) ReportConflict(e);
    return true;
  }

  if (l->value.has_value()) {
    CHECK(!r->value.has_value());
    r->value = *l->value;
    progress_ = true;
    return true;
  }

  if (r->value.has_value()) {
    CHECK(!l->value.has_value());
    l->value = *r->value;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const LiteralValue& l) {
  auto node = doc_->TryGetNode(&l);
  CHECK(node != nullptr) << l.location();
  CHECK(!node->value.has_value()) << l.location();

  node->value = l.value();
  progress_ = true;

  return true;
}

bool Evaluate::operator()(const NamedValue& n) {
  auto node = doc_->TryGetNamedNode(n.name());
  CHECK(node != nullptr) << n.location();
  return node->value.has_value();
}

bool Evaluate::operator()(const PowerExp& e) {
  auto b = doc_->TryGetNode(e.base());
  auto exp = doc_->TryGetNode(&e);
  CHECK(b != nullptr) << e.location();
  CHECK(exp != nullptr) << e.location();

  if (b->value.has_value() && exp->value.has_value()) {
    if (*exp->value != std::pow(*b->value, e.exp())) ReportConflict(e);
    return true;
  }

  if (b->value.has_value()) {
    CHECK(!exp->value.has_value());
    exp->value = std::pow(*b->value, e.exp());
    progress_ = true;
    return true;
  }

  if (exp->value.has_value()) {
    CHECK(!b->value.has_value());
    if (e.exp() % 2 == 1) {
      b->value = std::pow(*exp->value, 1.0 / e.exp());
      progress_ = true;
      return true;
    } else {
      LOG(INFO) << "^ " << exp->value.has_value() << b->value.has_value();
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

  if (n->value.has_value() && l->value.has_value() && r->value.has_value()) {
    if (*n->value != *l->value * *r->value) ReportConflict(p);
    return true;
  }

  if (l->value.has_value() && r->value.has_value()) {
    CHECK(!n->value.has_value());
    n->value = *l->value * *r->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && l->value.has_value()) {
    CHECK(!r->value.has_value());
    r->value = *n->value / *l->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && r->value.has_value()) {
    CHECK(!l->value.has_value());
    l->value = *n->value / *r->value;
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

  if (n->value.has_value() && l->value.has_value() && r->value.has_value()) {
    if (*n->value != *l->value / *r->value) ReportConflict(q);
    return true;
  }

  if (l->value.has_value() && r->value.has_value()) {
    CHECK(!n->value.has_value());
    n->value = *l->value / *r->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && r->value.has_value()) {
    CHECK(!l->value.has_value());
    l->value = *n->value * *r->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && l->value.has_value()) {
    CHECK(!r->value.has_value());
    r->value = *l->value / *n->value;
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

  if (n->value.has_value() && l->value.has_value() && r->value.has_value()) {
    if (*n->value != *l->value + *r->value) ReportConflict(s);
    return true;
  }

  if (l->value.has_value() && r->value.has_value()) {
    CHECK(!n->value.has_value());
    n->value = *l->value + *r->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && l->value.has_value()) {
    CHECK(!r->value.has_value());
    r->value = *n->value - *l->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && r->value.has_value()) {
    CHECK(!l->value.has_value());
    l->value = *n->value - *r->value;
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

  if (n->value.has_value() && l->value.has_value() && r->value.has_value()) {
    if (*n->value != *l->value - *r->value) ReportConflict(d);
    return true;
  }

  if (l->value.has_value() && r->value.has_value()) {
    CHECK(!n->value.has_value());
    n->value = *l->value - *r->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && l->value.has_value()) {
    CHECK(!r->value.has_value());
    r->value = *l->value - *n->value;
    progress_ = true;
    return true;
  }

  if (n->value.has_value() && r->value.has_value()) {
    CHECK(!l->value.has_value());
    l->value = *r->value + *n->value;
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

  if (b->value.has_value() && exp->value.has_value()) {
    if (*exp->value != -*b->value) ReportConflict(n);
    return true;
  }

  if (b->value.has_value()) {
    CHECK(!exp->value.has_value());
    exp->value = -*b->value;
    progress_ = true;
    return true;
  }

  if (exp->value.has_value()) {
    CHECK(!b->value.has_value());
    b->value = -*exp->value;
    progress_ = true;
    return true;
  }

  return false;
}

bool Evaluate::operator()(const Define& d) {
  auto node = doc_->TryGetNamedNode(d.name());
  CHECK(node != nullptr) << d.location();
  CHECK(!node->value.has_value()) << d.location();
  CHECK(node->unit.has_value()) << d.location();

  node->value = d.value() * node->unit->scale;
  progress_ = true;

  return true;
}

bool Evaluate::operator()(const Document& d) {
  // Collect the set of expressions.
  std::set<const ExpressionNode*> nodes;
  for (auto* exp : doc_->nodes()) {
    if (exp && exp->node) nodes.insert(exp->node);
  }

  // Processes all values on defines first.
  for (auto d : d.defines()) {
    CHECK(d->VisitNode(this)) << d;
    CHECK(nodes.erase(static_cast<const ExpressionNode*>(d)) == 1) << d;
  }

  if (error_) return false;

  progress_ = true;
  LOG(INFO) << nodes.size();
  // For every pass where progress is made, at least one node is
  // removed from nodes_ so wathing that bounds the number of passes.
  for (int p = 0; (progress_ && !nodes.empty()); p++) {
    LOG(INFO) << "==== pass " << p << " ====";
    progress_ = false;
    for (auto it = nodes.begin(); it != nodes.end();) {
      if ((*it)->VisitNode(this)) {
        it = nodes.erase(it);
        progress_ = true;
      } else {
        ++it;
      }
    }
    LOG(INFO) << nodes.size() << " nodes un-resolved";
    if (error_) return false;
  }

  for (const auto* c : conflicts_) {
    SYM_ERROR(*c) << "Conflicting result";
  }

  LOG(INFO) << "==== DONE ====";

  return conflicts_.empty();
}

}  // namespace tbd
