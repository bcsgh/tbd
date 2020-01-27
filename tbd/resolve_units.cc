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

#include <cmath>

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"
#include "tbd/ast.h"
#include "tbd/dimensions.h"
#include "tbd/semantic.h"
#include "tbd/util.h"

ABSL_FLAG(int32_t, iteration_limit, 64, "");

namespace tbd {

bool ResolveUnits::operator()(const UnitExp& exp) {
  for (const auto& bit : exp.bits()) {
    if (const auto u = doc_->LookupUnit(bit.id)) {
      unit_value_ = Unit{unit_value_.scale * std::pow(u->scale, bit.exp),
                         unit_value_.dim * pow(u->dim, bit.exp)};
      if (bit.exp < -1) {
        absl::StrAppend(&unit_name_, "/", bit.id, "^", -bit.exp);
      } else if (bit.exp == -1) {
        absl::StrAppend(&unit_name_, "/", bit.id);
      } else if (bit.exp == 1) {
        absl::StrAppend(&unit_name_, "*", bit.id);
      } else {
        absl::StrAppend(&unit_name_, "*", bit.id, "^", bit.exp);
      }
    } else {
      SYM_ERROR(bit.loc) << "Unit '" << bit.id << "' is not defined";
      return false;
    }
  }

  if (!unit_name_.empty()) {
    if (unit_name_[0] == '/') {
      unit_name_ = absl::StrCat("1", unit_name_);
    } else {
      unit_name_ = unit_name_.substr(1);
    }
  }

  return true;
}

bool ResolveUnits::operator()(const UnitDef& unit) {
  bool error = false;
  if (const auto u = doc_->LookupUnit(unit.name())) {
    SYM_ERROR(unit) << "Unit '" << unit.name() << "' already defined at TODO";
    error = true;
  }

  unit_value_ = {unit.value(), Dimension::Dimensionless()};

  if (!unit.unit().VisitNode(this)) return false;

  if (error) return false;

  CHECK(doc_->AddUnit(unit.name(), unit_value_));

  return true;
}

bool ResolveUnits::operator()(const Equality& e) {
  if (!e.left()->VisitNode(this) || !e.right()->VisitNode(this)) return false;

  auto l = doc_->TryGetNode(e.left());
  auto r = doc_->TryGetNode(e.right());
  CHECK(l != nullptr) << e.left()->location();
  CHECK(r != nullptr) << e.right()->location();

  if (!l->dim.has_value() && !r->dim.has_value()) return true;

  if (l->dim.has_value() && r->dim.has_value()) {
    if (*l->dim != *r->dim) {
      SYM_ERROR(e)
          << "Equality expression's terms have different dimensionality: "
          << *l->dim << " and " << *r->dim;
      return false;
    } else {
      return true;
    }
  }

  if (!down_) return true;

  if (!l->dim.has_value()) {
    CHECK(r->dim.has_value());
    l->dim = *r->dim;
    progress_ = true;
    LOG(INFO) << "=< " << e.location();
  } else if (!r->dim.has_value()) {
    CHECK(l->dim.has_value());
    r->dim = *l->dim;
    progress_ = true;
    LOG(INFO) << "=> " << e.location();
  } else {
    LOG(FATAL);
  }

  return true;
}

bool ResolveUnits::operator()(const LiteralValue& l) {
  auto val = doc_->TryGetNode(&l);
  CHECK(val != nullptr);
  if (!val->dim.has_value()) {
    val->dim = Dimension::Dimensionless();
    progress_ = true;
  } else {
    CHECK(val->dim == Dimension::Dimensionless())
        << l.location() << ": literal value's dimensionality is not unitless";
  }
  return true;
}

bool ResolveUnits::operator()(const PowerExp& e) {
  if (!e.base()->VisitNode(this)) return false;

  auto b = doc_->TryGetNode(e.base());
  auto exp = doc_->TryGetNode(&e);
  CHECK(b != nullptr);
  CHECK(exp != nullptr);

  if (!b->dim.has_value() && !exp->dim.has_value()) return true;

  if (b->dim.has_value()) {
    auto dim = pow(*b->dim, e.exp());
    if (!exp->dim.has_value()) {
      exp->dim = dim;
      progress_ = true;
      return true;
    } else if (*exp->dim != dim) {
      SYM_ERROR(e)
          << "Exponential expression's dimensionality is deduced as both "
          << *exp->dim << " and " << dim;
      return false;
    }
    return true;
  }

  if (!down_) return true;

  CHECK(exp->dim.has_value());
  CHECK(!b->dim.has_value());
  b->dim = root(*exp->dim, e.exp());
  progress_ = true;
  LOG(INFO) << "^. " << e.location();

  return true;
}

bool ResolveUnits::operator()(const ProductExp& p) {
  if (!p.left()->VisitNode(this) || !p.right()->VisitNode(this)) return false;

  auto l = doc_->TryGetNode(p.left());
  auto r = doc_->TryGetNode(p.right());
  auto n = doc_->TryGetNode(&p);
  CHECK(l != nullptr) << p.left()->location();
  CHECK(r != nullptr) << p.right()->location();
  CHECK(n != nullptr) << p.location();

  if (!n->dim.has_value() && !l->dim.has_value() && !r->dim.has_value())
    return true;

  if (l->dim.has_value() && r->dim.has_value()) {
    auto dim = *l->dim * *r->dim;
    if (!n->dim.has_value()) {
      n->dim = dim;
      progress_ = true;
      return true;
    } else if (*n->dim != dim) {
      SYM_ERROR(p)
          << "Multiplication expression's dimensionality is deduced as both "
          << *n->dim << " and " << dim;
      return false;
    }
    return true;
  }

  if (!n->dim.has_value()) return true;

  CHECK(l->dim.has_value() != r->dim.has_value());

  if (!down_) return true;

  if (!r->dim.has_value()) {
    CHECK(l->dim.has_value());
    r->dim = *n->dim / *l->dim;
    progress_ = true;
    LOG(INFO) << "*> " << p.location();
  } else if (!l->dim.has_value()) {
    CHECK(r->dim.has_value());
    l->dim = *n->dim / *r->dim;
    progress_ = true;
    LOG(INFO) << "*< " << p.location();
  } else {
    LOG(FATAL);
  }

  return true;
}

bool ResolveUnits::operator()(const QuotientExp& q) {
  if (!q.left()->VisitNode(this) || !q.right()->VisitNode(this)) return false;

  auto l = doc_->TryGetNode(q.left());
  auto r = doc_->TryGetNode(q.right());
  auto n = doc_->TryGetNode(&q);
  CHECK(l != nullptr) << q.left()->location();
  CHECK(r != nullptr) << q.right()->location();
  CHECK(n != nullptr) << q.location();

  if (!n->dim.has_value() && !l->dim.has_value() && !r->dim.has_value())
    return true;

  if (l->dim.has_value() && r->dim.has_value()) {
    auto dim = *l->dim / *r->dim;
    if (!n->dim.has_value()) {
      n->dim = dim;
      progress_ = true;
      return true;
    } else if (*n->dim != dim) {
      SYM_ERROR(q) << "Division expression's dimensionality is deduced as both "
                   << *n->dim << " and " << dim;
      return false;
    }
    return true;
  }

  if (!n->dim.has_value()) return true;

  CHECK(l->dim.has_value() != r->dim.has_value());

  if (!down_) return true;

  if (!r->dim.has_value()) {
    CHECK(l->dim.has_value());
    r->dim = *l->dim / *n->dim;
    progress_ = true;
    LOG(INFO) << "/> " << q.location();
  } else if (!l->dim.has_value()) {
    CHECK(r->dim.has_value());
    l->dim = *r->dim * *n->dim;
    progress_ = true;
    LOG(INFO) << "/< " << q.location();
  } else {
    LOG(FATAL);
  }

  return true;
}

bool ResolveUnits::operator()(const SumExp& s) {
  if (!s.left()->VisitNode(this) || !s.right()->VisitNode(this)) return false;

  auto l = doc_->TryGetNode(s.left());
  auto r = doc_->TryGetNode(s.right());
  auto n = doc_->TryGetNode(&s);
  CHECK(l != nullptr) << s.left()->location();
  CHECK(r != nullptr) << s.right()->location();
  CHECK(n != nullptr) << s.location();

  if (!n->dim.has_value() && !l->dim.has_value() && !r->dim.has_value())
    return true;

  if (l->dim.has_value() && r->dim.has_value()) {
    if (*l->dim != *r->dim) {
      SYM_ERROR(s)
          << "Addition expression's terms have different dimensionality: "
          << *l->dim << " and " << *r->dim;
      return false;
    }
  }
  if (l->dim.has_value() || r->dim.has_value()) {
    auto dim = l->dim.has_value() ? *l->dim : *r->dim;
    if (!n->dim.has_value()) {
      n->dim = dim;
      progress_ = true;
    } else if (*n->dim != dim) {
      SYM_ERROR(s) << "Addition expression's dimensionality is deduced as both "
                   << *n->dim << " and " << dim;
      return false;
    }
  }

  CHECK(n->dim.has_value());
  if (l->dim.has_value() && r->dim.has_value()) return true;

  if (!down_) return true;

  if (!l->dim.has_value()) {
    l->dim = *n->dim;
    progress_ = true;
    LOG(INFO) << "+< " << s.location();
  } else {
    CHECK(*l->dim == *n->dim);
  }

  if (!r->dim.has_value()) {
    r->dim = *n->dim;
    progress_ = true;
    LOG(INFO) << "+> " << s.location();
  } else {
    CHECK(*r->dim == *n->dim);
  }

  return true;
}

bool ResolveUnits::operator()(const DifExp& d) {
  if (!d.left()->VisitNode(this) || !d.right()->VisitNode(this)) return false;

  auto l = doc_->TryGetNode(d.left());
  auto r = doc_->TryGetNode(d.right());
  auto n = doc_->TryGetNode(&d);
  CHECK(l != nullptr) << d.left()->location();
  CHECK(r != nullptr) << d.right()->location();
  CHECK(n != nullptr) << d.location();

  if (!n->dim.has_value() && !l->dim.has_value() && !r->dim.has_value())
    return true;

  if (l->dim.has_value() && r->dim.has_value()) {
    if (*l->dim != *r->dim) {
      SYM_ERROR(d)
          << "Subtraction expression's terms have different dimensionality: "
          << *l->dim << " and " << *r->dim;
      return false;
    }
  }
  if (l->dim.has_value() || r->dim.has_value()) {
    auto dim = l->dim.has_value() ? *l->dim : *r->dim;
    if (!n->dim.has_value()) {
      n->dim = dim;
      progress_ = true;
    } else if (*n->dim != dim) {
      SYM_ERROR(d)
          << "Subtraction expression's dimensionality is deduced as both "
          << *n->dim << " and " << dim;
      return false;
    }
  }

  CHECK(n->dim.has_value());
  if (l->dim.has_value() && r->dim.has_value()) return true;

  if (!down_) return true;

  if (!l->dim.has_value()) {
    LOG(INFO) << "-< " << d.location();
    l->dim = *n->dim;
    progress_ = true;
  } else {
    CHECK(*l->dim == *n->dim);
  }

  if (!r->dim.has_value()) {
    LOG(INFO) << "-> " << d.location();
    r->dim = *n->dim;
    progress_ = true;
  } else {
    CHECK(*r->dim == *n->dim);
  }

  return true;
}

bool ResolveUnits::operator()(const NegativeExp& n) {
  if (!n.value()->VisitNode(this)) return false;

  auto b = doc_->TryGetNode(n.value());
  auto exp = doc_->TryGetNode(&n);
  CHECK(b != nullptr);
  CHECK(exp != nullptr);

  if (!b->dim.has_value() && !exp->dim.has_value()) return true;

  if (b->dim.has_value()) {
    if (!exp->dim.has_value()) {
      exp->dim = *b->dim;
      progress_ = true;
      return true;
    } else if (*exp->dim != *b->dim) {
      SYM_ERROR(n) << "Negation expression's dimensionality is deduced as both "
                   << *exp->dim << " and " << *b->dim;
      return false;
    }
    return true;
  }

  if (!down_) return true;

  CHECK(exp->dim.has_value());
  CHECK(!b->dim.has_value());
  b->dim = *exp->dim;
  progress_ = true;
  LOG(INFO) << "-. " << n.location();

  return true;
}

bool ResolveUnits::operator()(const Define& d) {
  unit_value_ = {1.0, Dimension::Dimensionless()};
  unit_name_ = "";
  if (!d.unit().VisitNode(this)) return false;

  auto node = doc_->TryGetNamedNode(d.name());
  node->dim = unit_value_.dim;
  node->unit = unit_value_;
  node->unit_name = unit_name_;
  return true;
}

bool ResolveUnits::operator()(const Specification& s) {
  unit_value_ = {1.0, Dimension::Dimensionless()};
  unit_name_ = "";
  if (!s.unit().VisitNode(this)) return false;

  auto node = doc_->TryGetNamedNode(s.name());
  node->dim = unit_value_.dim;
  node->unit = unit_value_;
  node->unit_name = unit_name_;
  return true;
}

bool ResolveUnits::operator()(const Document& d) {
  // Collect the set of units.
  bool error = false;
  for (auto ud : d.unit_definition())
    if (!ud->VisitNode(this)) error = true;

  if (error) return false;

  // Assign units to the named/defined values.
  for (auto d : d.defines())
    if (!d->VisitNode(this)) error = true;

  // Assign units to the named specs.
  for (auto s : d.specs())
    if (!s->VisitNode(this)) error = true;

  if (error) return false;

  // Assign units to everything else.
  progress_ = true;
  for (int pass = 0; (pass < absl::GetFlag(FLAGS_iteration_limit) && progress_); pass++) {
    down_ = (pass > 0);
    LOG(INFO) << (down_ ? "==== UP PASS ====" : "==== DOWN PASS ====");
    progress_ = (pass <= 1);  // At least 2 passes;
    for (auto e : d.equality()) {
      if (!e->VisitNode(this)) error = true;
    }
    if (error) return false;
    // Loop as long as progress is being made.
  }
  LOG(INFO) << "==== DONE ====";

  return true;
}

}  // namespace tbd
