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

#include "tbd/ops.h"

#include <cmath>

#include "tbd/semantic.h"

namespace tbd {

bool DirectEvaluate::operator()(const OpAdd& o) {
  if (std::isnan(o.a->value) || std::isnan(o.b->value)) return false;
  o.r->value = o.a->value + o.b->value;
  return true;
}

bool DirectEvaluate::operator()(const OpSub& o) {
  if (std::isnan(o.a->value) || std::isnan(o.b->value)) return false;
  o.r->value = o.a->value - o.b->value;
  return true;
}

bool DirectEvaluate::operator()(const OpMul& o) {
  if (std::isnan(o.a->value) || std::isnan(o.b->value)) return false;
  o.r->value = o.a->value * o.b->value;
  return true;
}

bool DirectEvaluate::operator()(const OpDiv& o) {
  if (std::isnan(o.a->value) || std::isnan(o.b->value)) return false;
  o.r->value = o.a->value / o.b->value;
  return true;
}

bool DirectEvaluate::operator()(const OpNeg& o) {
  if (std::isnan(o.a->value)) return false;
  o.r->value = -o.a->value;
  return true;
}

bool DirectEvaluate::operator()(const OpExp& o) {
  if (std::isnan(o.b->value)) return false;
  o.r->value = std::pow(o.b->value, o.e);
  return true;
}

bool DirectEvaluate::operator()(const OpAssign& o) {
  if (std::isnan(o.s->value)) return false;
  o.d->value = o.s->value;
  return true;
}

bool DirectEvaluate::operator()(const OpLoad& o) {
  o.n->value = (*in_)[o.i];
  return true;
}

bool DirectEvaluate::operator()(const OpCheck& o) {
  if (std::isnan(o.a->value) || std::isnan(o.b->value)) return false;
  (*out_)[o.i] = o.a->value - o.b->value;
  return true;
}

/////////////////////////////////////////////////////////////////////////

bool OpAdd::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpSub::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpMul::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpDiv::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpNeg::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpExp::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpAssign::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpLoad::Visit(VisitOps* v) const { return (*v)(*this); }
bool OpCheck::Visit(VisitOps* v) const { return (*v)(*this); }

}  // namespace tbd
