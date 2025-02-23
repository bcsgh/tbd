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

#ifndef TBD_FIND_H_
#define TBD_FIND_H_

#include <functional>
#include <type_traits>
#include <vector>

#include "tbd/ast.h"

namespace tbd {
// An generic visitor that acumulates a list of nodes of a given type.
// Also includes a filter function that returns a list of found nodes
// that match a given predicate.

template <class N>
class Find final : public VisitNodes {
 public:
  std::vector<const N*>& nodes() { return nodes_; }

  template <class FN>
  std::vector<const N*> NodesWhere(FN fn) {
    std::vector<const N*> ret;
    ret.reserve(nodes_.size());
    for (const auto& n : nodes_) {
      if (fn(n)) ret.push_back(n);
    }
    return ret;
  }

 private:
  // Anything that is a N gets captured, anything else ignored.
  void Sink(const void*) {}
  void Sink(const N* n) { nodes_.push_back(n); }

  bool operator()(const UnitExp& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const UnitDef& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const Equality& n) override {
    Sink(&n);
    return n.left()->VisitNode(this) && n.right()->VisitNode(this);
  }
  bool operator()(const LiteralValue& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const NamedValue& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const PowerExp& n) override {
    Sink(&n);
    return n.base()->VisitNode(this);
  }
  bool operator()(const ProductExp& n) override {
    Sink(&n);
    return n.left()->VisitNode(this) && n.right()->VisitNode(this);
  }
  bool operator()(const QuotientExp& n) override {
    Sink(&n);
    return n.left()->VisitNode(this) && n.right()->VisitNode(this);
  }
  bool operator()(const SumExp& n) override {
    Sink(&n);
    return n.left()->VisitNode(this) && n.right()->VisitNode(this);
  }
  bool operator()(const DifExp& n) override {
    Sink(&n);
    return n.left()->VisitNode(this) && n.right()->VisitNode(this);
  }
  bool operator()(const NegativeExp& n) override {
    Sink(&n);
    return n.value()->VisitNode(this);
  }
  bool operator()(const Define& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const Specification& n) override {
    Sink(&n);
    return true;
  }
  bool operator()(const Document& n) override {
    Sink(&n);
    for (auto d : n.defines()) (void)d->VisitNode(this);
    for (auto d : n.specs()) (void)d->VisitNode(this);
    for (auto e : n.equality()) (void)e->VisitNode(this);
    return true;
  }

  std::vector<const N*> nodes_;
};

}  // namespace tbd

#endif  // TBD_FIND_H_
