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

#ifndef TBD_OPS_H_
#define TBD_OPS_H_

#include "Eigen/Core"
#include "absl/base/attributes.h"
#include "tbd/semantic.h"

// A set of "virtual machine" op-codes for encoding a sequence of math ops.

namespace tbd {
class VisitOps;

// An Op code
class OpI {
 public:
  OpI() {}
  virtual ~OpI() = default;
  virtual Loc location() const = 0;

  ABSL_MUST_USE_RESULT bool VisitOp(VisitOps* v) const { return Visit(v); }

 protected:
  using ExpP = SemanticDocument::Exp*;

 private:
  ABSL_MUST_USE_RESULT virtual bool Visit(VisitOps*) const = 0;
};

////////////////////////////////////////////

class OpAdd final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpAdd(ExpP r_, ExpP a_, ExpP b_) : r(r_), a(a_), b(b_) {}

  const ExpP r, a, b;
};

class OpSub final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpSub(ExpP r_, ExpP a_, ExpP b_) : r(r_), a(a_), b(b_) {}

  const ExpP r, a, b;
};

class OpMul final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpMul(ExpP r_, ExpP a_, ExpP b_) : r(r_), a(a_), b(b_) {}

  const ExpP r, a, b;
};

class OpDiv final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpDiv(ExpP r_, ExpP a_, ExpP b_) : r(r_), a(a_), b(b_) {}

  const ExpP r, a, b;
};

class OpNeg final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpNeg(ExpP r_, ExpP a_) : r(r_), a(a_) {}

  const ExpP r, a;
};

class OpExp final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return r->node->location(); }

 public:
  OpExp(ExpP r_, ExpP b_, double e_) : r(r_), b(b_), e(e_) {}

  const ExpP r, b;
  const double e;
};

class OpAssign final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return d->node->location(); }

 public:
  OpAssign(ExpP d_, ExpP s_) : d(d_), s(s_) {}

  const ExpP d, s;
};

class OpLoad final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return n->node->location(); }

 public:
  OpLoad(ExpP n_, int i_) : n(n_), i(i_) {}

  const ExpP n;
  const int i;
};

class OpCheck final : public OpI {
  ABSL_MUST_USE_RESULT bool Visit(VisitOps*) const override;
  Loc location() const override { return {}; }  // TODO where to get this from?

 public:
  OpCheck(int i_, ExpP a_, ExpP b_) : i(i_), a(a_), b(b_) {}

  const int i;
  const ExpP a, b;
};

////////////////////////////////////////////

class VisitOps {
 public:
  // Get the address of the object as a pointer, even if it's a temporary.
  VisitOps* as_ptr() { return this; }

  ABSL_MUST_USE_RESULT virtual bool operator()(const OpAdd&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpSub&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpMul&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpDiv&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpNeg&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpExp&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpAssign&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpLoad&) = 0;
  ABSL_MUST_USE_RESULT virtual bool operator()(const OpCheck&) = 0;

  // Only exact matches are allowed. Suppress all conversions.
  template <class T>
  bool operator()(const T&) = delete;
};

////////////////////////////////////////////

// Direct in place evaluation.
class DirectEvaluate final : public VisitOps {
 public:
  DirectEvaluate(Eigen::VectorXd* in, Eigen::VectorXd* out)
      : in_(in), out_(out) {}

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
  Eigen::VectorXd* in_;
  Eigen::VectorXd* out_;
};

}  // namespace tbd

#endif  // TBD_OPS_H_
