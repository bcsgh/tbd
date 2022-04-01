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

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "tbd/dimensions.h"

namespace tbd {

bool operator==(const Loc& l, const Loc& r) {
  auto lt =
      std::tie(l.filename, l.line_begin, l.line_end, l.col_begin, l.col_end);
  auto rt =
      std::tie(r.filename, r.line_begin, r.line_end, r.col_begin, r.col_end);
  return lt == rt;
}

bool operator<(const Loc& l, const Loc& r) {
  auto lt =
      std::tie(l.filename, l.line_begin, l.line_end, l.col_begin, l.col_end);
  auto rt =
      std::tie(r.filename, r.line_begin, r.line_end, r.col_begin, r.col_end);
  return lt < rt;
}

Loc Join(std::vector<Loc> loc) {
  CHECK(!loc.empty());
  Loc ret = *loc.begin();
  for (const auto& l : loc) {
    CHECK(ret.filename == l.filename) << ret.filename << "!=" << l.filename;

    if (l.line_begin < ret.line_begin) {
      ret.line_begin = l.line_begin;
      ret.col_begin = l.col_begin;
    } else if (l.line_begin == ret.line_begin) {
      ret.col_begin = std::min(ret.col_begin, l.col_begin);
    }

    if (l.line_end > ret.line_end) {
      ret.line_end = l.line_end;
      ret.col_end = l.col_end;
    } else if (l.line_end == ret.line_end) {
      ret.col_end = std::max(ret.col_end, l.col_end);
    }
  }
  return ret;
}

bool StableNodeCompare::operator()(NodeI const* l, NodeI const* r) const {
  auto& loc_l = l->location();
  auto& loc_r = r->location();
  return loc_l == loc_r ? (l < r) : (loc_l < loc_r);
}

void UnitExp::Mul(std::unique_ptr<UnitExp::UnitT> o) {
  bits_.push_back(std::move(*o));
}

void UnitExp::Div(std::unique_ptr<UnitExp::UnitT> o) {
  o->exp = -o->exp;
  Mul(std::move(o));
}

UnitDef::UnitDef(Loc loc, std::unique_ptr<std::string> name,
                 std::unique_ptr<LiteralValue> lit,
                 std::unique_ptr<UnitExp> unit)
    : NodeI(Join(loc, lit->location(), unit->location())),
      name_(std::move(*name)),
      value_(lit->value()),
      unit_(std::move(unit)) {}

Equality::Equality(Loc loc, std::unique_ptr<ExpressionNode> l,
                   std::unique_ptr<ExpressionNode> r)
    : ExpressionNode(Join(loc, l->location(), r->location())),
      l_(std::move(l)),
      r_(std::move(r)) {}

NamedValue::NamedValue(Loc loc, std::unique_ptr<std::string> s)
    : ExpressionNode(loc), name_(std::move(*s)) {}

NamedValue::NamedValue(Loc loc, std::string s)
    : ExpressionNode(loc), name_(std::move(s)) {}

PowerExp::PowerExp(Loc loc, std::unique_ptr<ExpressionNode> b, int e, bool positive)
    : ExpressionNode(Join(loc, b->location())), b_(std::move(b)), e_(e), positive_(positive) {}

BinaryExpression::BinaryExpression(std::unique_ptr<ExpressionNode> l,
                                   std::unique_ptr<ExpressionNode> r)
    : ExpressionNode(Join(l->location(), r->location())),
      l_(std::move(l)),
      r_(std::move(r)) {}

NegativeExp::NegativeExp(Loc loc, std::unique_ptr<ExpressionNode> e)
    : ExpressionNode(Join(loc, e->location())), e_(std::move(e)) {}

Define::Define(Loc loc, std::unique_ptr<std::string> name,
               std::unique_ptr<LiteralValue> val, std::unique_ptr<UnitExp> unit)
    : ExpressionNode(loc),
      name_(std::move(*name)),
      val_(std::move(val)),
      unit_(std::move(unit)) {}

Define::Define(Loc loc, std::string name, double val)
    : Define(loc, absl::make_unique<std::string>(std::move(name)),
             absl::make_unique<LiteralValue>(loc, val),
             absl::make_unique<UnitExp>(loc)) {}

Specification::Specification(Loc loc, std::unique_ptr<std::string> name,
                             std::unique_ptr<UnitExp> unit)
    : NodeI(Join(loc, unit->location())),
      name_(std::move(*name)),
      unit_(std::move(unit)) {}

////////

void Document::AddEquality(std::unique_ptr<Equality> e) {
  equality_.emplace_back(std::move(e));
}
void Document::AddDefinition(std::unique_ptr<Define> d) {
  defines_.emplace_back(std::move(d));
}
void Document::AddSpecification(std::unique_ptr<Specification> s) {
  specs_.emplace_back(std::move(s));
}
void Document::AddUnitDefinition(std::unique_ptr<UnitDef> u) {
  unit_def_.emplace_back(std::move(u));
}

std::vector<Equality*> Document::equality() const {
  std::vector<Equality*> ret;
  ret.reserve(equality_.size());
  for (const auto& e : equality_) ret.push_back(e.get());
  return ret;
}

std::vector<Define*> Document::defines() const {
  std::vector<Define*> ret;
  ret.reserve(defines_.size());
  for (const auto& d : defines_) ret.push_back(d.get());
  return ret;
}

std::vector<Specification*> Document::specs() const {
  std::vector<Specification*> ret;
  ret.reserve(specs_.size());
  for (const auto& s : specs_) ret.push_back(s.get());
  return ret;
}

std::vector<UnitDef*> Document::unit_definition() const {
  std::vector<UnitDef*> ret;
  ret.reserve(unit_def_.size());
  for (const auto& d : unit_def_) ret.push_back(d.get());
  return ret;
}

/////////////////////////////////////////////////////////////////////////

bool UnitExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool UnitDef::Visit(VisitNodes* v) const { return (*v)(*this); }
bool Equality::Visit(VisitNodes* v) const { return (*v)(*this); }
bool LiteralValue::Visit(VisitNodes* v) const { return (*v)(*this); }
bool NamedValue::Visit(VisitNodes* v) const { return (*v)(*this); }
bool PowerExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool ProductExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool QuotientExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool SumExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool DifExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool NegativeExp::Visit(VisitNodes* v) const { return (*v)(*this); }
bool Define::Visit(VisitNodes* v) const { return (*v)(*this); }
bool Specification::Visit(VisitNodes* v) const { return (*v)(*this); }
bool Document::Visit(VisitNodes* v) const { return (*v)(*this); }

}  // namespace tbd
