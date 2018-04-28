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

#ifndef TBD_GRAPHVIZ_H_
#define TBD_GRAPHVIZ_H_

#include <iostream>
#include <map>
#include <set>
#include <string>

#include "absl/types/optional.h"
#include "tbd/ast.h"
#include "tbd/semantic.h"

namespace tbd {

class RenderAsGraphViz : public VisitNodes {
 public:
  RenderAsGraphViz(SemanticDocument* doc, std::ostream& out)
      : doc_(doc), out_(out) {}
  ~RenderAsGraphViz() { Dump(out_); }

 private:
  friend class GraphVizTest;

  int AddDefault(const ExpressionNode*);
  bool Process(const BinaryExpression&);
  void Dump(std::ostream&);

  bool operator()(const UnitExp&) override { return false; }
  bool operator()(const UnitDef&) override { return false; }
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

  SemanticDocument* doc_;
  std::ostream& out_;

  std::map<const ExpressionNode*, int> id_;
  std::map<std::string, int> named_;

  struct Node {
    std::string label;
    bool pinned = false;
    absl::optional<Dimension> dim = absl::nullopt;
    bool has_value = false;
  };
  struct Edge {
    bool equ = false;
  };

  std::map<int, Node> all_nodes_;
  std::map<std::pair<int, int>, Edge> all_edges_;
  int unknown_ = 0;
};

}  // namespace tbd

#endif  // TBD_GRAPHVIZ_H_
