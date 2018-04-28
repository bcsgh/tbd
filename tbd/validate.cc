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

#include "gflags/gflags.h"
#include "tbd/ast.h"
#include "tbd/common.h"
#include "tbd/semantic.h"

DEFINE_bool(warnings_as_errors, false, "");

namespace tbd {

bool Validate::Process(const BinaryExpression& e) {
  doc_->GetUnnamedNode(&e);
  if (!e.left()->VisitNode(this) || !e.right()->VisitNode(this)) return false;
  return true;
}

bool Validate::operator()(const Equality& e) {
  doc_->GetUnnamedNode(&e);
  if (!e.left()->VisitNode(this) || !e.right()->VisitNode(this)) return false;
  return true;
}

bool Validate::operator()(const LiteralValue& v) {
  doc_->GetUnnamedNode(&v);
  return true;
}

bool Validate::operator()(const NamedValue& n) {
  doc_->RefernceNamedNode(&n);
  return true;
}

bool Validate::operator()(const PowerExp& e) {
  doc_->GetUnnamedNode(&e);
  return e.base()->VisitNode(this);
}

bool Validate::operator()(const ProductExp& p) { return Process(p); }
bool Validate::operator()(const QuotientExp& q) { return Process(q); }
bool Validate::operator()(const SumExp& s) { return Process(s); }
bool Validate::operator()(const DifExp& d) { return Process(d); }

bool Validate::operator()(const NegativeExp& n) {
  doc_->GetUnnamedNode(&n);
  return n.value()->VisitNode(this);
}

bool Validate::operator()(const Define& d) {
  bool error = false;
  SemanticDocument::Exp* e = doc_->GetNamedNode(&d);
  CHECK(e != nullptr);
  if (&d != e->node) {
    SYM_ERROR(d) << "duplicate definition for '" << d.name()
                 << "'. Prior definition at " << e->def->location();
    error = true;
  }
  return !error;
}

bool Validate::operator()(const Specification& s) {
  SemanticDocument::Exp* e = doc_->GetNodeForName(s.name());
  CHECK(e != nullptr);
  if (e->def != nullptr) {
    SYM_ERROR(s) << "duplicate specification for '" << s.name()
                 << "'. Prior definition at " << e->def->location();
    return false;
  }
  if (e->spec != nullptr) {
    SYM_ERROR(s) << "duplicate specification for '" << s.name()
                 << "'. Prior specification at " << e->spec->location();
    return false;
  }
  e->spec = &s;
  return true;
}

bool Validate::operator()(const Document& d) {
  bool error = false;
  for (auto d : d.defines())
    if (!d->VisitNode(this)) error = true;
  for (auto d : d.specs())
    if (!d->VisitNode(this)) error = true;
  for (auto e : d.equality())
    if (!e->VisitNode(this)) error = true;

  bool warning = false;
  for (const auto& i : doc_->nodes()) {
    if (i->def == nullptr) continue;
    if (i->def->location().filename == kPreamble) continue;
    if (i->referenced) continue;

    SYM_ERROR(*i->def) << "Unused definition for '" << i->def->name() << "'.";
    warning = true;
  }

  if (FLAGS_warnings_as_errors) error = error || warning;
  return !error;
}

}  // namespace tbd
