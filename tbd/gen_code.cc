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

#include "tbd/gen_code.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "glog/logging.h"
#include "tbd/semantic.h"

namespace tbd {

bool CodeEvaluate::BinaryOp(ExpP r, ExpP a, ExpP b, absl::string_view op) {
  auto it_a = expressions_.find(a);
  if (it_a == expressions_.end()) {
    if (!a->name.empty()) {
      it_a = expressions_.emplace(a, a->name).first;
    } else if (a->is_literal) {
      it_a = expressions_.emplace(a, absl::StrCat(a->value)).first;
    } else {
      LOG(WARNING) << "Unknown source has no name or value: "
                   << a->node->location();
      return false;
    }
  }
  auto it_b = expressions_.find(b);
  if (it_b == expressions_.end()) {
    if (!b->name.empty()) {
      it_b = expressions_.emplace(b, b->name).first;
    } else if (b->is_literal) {
      it_b = expressions_.emplace(b, absl::StrCat(b->value)).first;
    } else {
      LOG(WARNING) << "Unknown source has no name or value: "
                   << b->node->location();
      return false;
    }
  }

  return Add(r, absl::StrCat("(", it_a->second, op, it_b->second, ")"));
}

bool CodeEvaluate::Add(const ExpP e, std::string v) {
  if (e->name.empty()) {
    return expressions_.emplace(e, std::move(v)).second;
  } else {
    out_ << e->name << " = " << v << ";\n";
    expressions_.emplace(e, e->name);
    return true;
  }
}

bool CodeEvaluate::operator()(const OpAdd& o) {
  return BinaryOp(o.r, o.a, o.b, " + ");
}

bool CodeEvaluate::operator()(const OpSub& o) {
  return BinaryOp(o.r, o.a, o.b, " - ");
}

bool CodeEvaluate::operator()(const OpMul& o) {
  return BinaryOp(o.r, o.a, o.b, " * ");
}

bool CodeEvaluate::operator()(const OpDiv& o) {
  return BinaryOp(o.r, o.a, o.b, " / ");
}

bool CodeEvaluate::operator()(const OpNeg& o) {
  auto it = expressions_.find(o.a);
  if (it == expressions_.end()) {
    if (o.a->name.empty()) {
      LOG(WARNING) << "Unknown source has no name";
      return false;
    }
    it = expressions_.emplace(o.a, o.a->name).first;
  }

  return Add(o.r, absl::StrCat("(-", it->second, ")"));
}

bool CodeEvaluate::operator()(const OpExp& o) {
  auto it_b = expressions_.find(o.b);
  if (it_b == expressions_.end()) {
    if (o.b->name.empty()) return false;
    it_b = expressions_.emplace(o.b, o.b->name).first;
  }

  return Add(o.r, absl::StrCat("std::pow(", it_b->second, ", ", o.e, ")"));
}

bool CodeEvaluate::operator()(const OpAssign& o) {
  auto it = expressions_.find(o.s);
  if (it == expressions_.end()) {
    if (!o.s->name.empty()) {
      it = expressions_.emplace(o.s, o.s->name).first;
    } else if (o.s->is_literal) {
      it = expressions_.emplace(o.s, absl::StrCat(o.s->value)).first;
    } else {
      LOG(WARNING) << "Unknown source has no name or value: "
                   << o.s->node->location();
      return false;
    }
  }

  return Add(o.d, it->second);
}

bool CodeEvaluate::operator()(const OpLoad& o) {
  auto it = expressions_.find(o.n);
  CHECK(it == expressions_.end()) << it->second << " is already loaded";
  CHECK(!o.n->name.empty());
  CHECK(expressions_.emplace(o.n, o.n->name).second);

  out_ << o.n->name << " = @src[" << o.i << "];\n";
  return true;
}

bool CodeEvaluate::operator()(const OpCheck& o) {
  std::string a = "?", b = "?";
  if (o.a) {
    auto it = expressions_.find(o.a);
    if (it != expressions_.end()) a = it->second;
  }
  if (o.b) {
    auto it = expressions_.find(o.b);
    if (it != expressions_.end()) b = it->second;
  }

  out_ << "@des[" << o.i << "] = (" << a << " - " << b << ");\n";
  return true;
}

}  // namespace tbd
