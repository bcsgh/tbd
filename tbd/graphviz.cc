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

#include "tbd/graphviz.h"

#include <iostream>
#include <set>

#include "absl/strings/str_cat.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "glog/logging.h"
#include "tbd/ast.h"
#include "tbd/semantic.h"

namespace tbd {

void RenderAsGraphViz::Dump(std::ostream& out) {
  out_ << "digraph {\n";
  for (const auto& e : all_edges_) {
    out << "  " << e.first.first << "->" << e.first.second << " [dir=none";
    if (e.second.equ) out << R"( color="black:black")";
    out << "];\n";
  }

  constexpr auto kFmt = R"(

  _date_time_ [label="%Y-%m-%d %H:%M:%S" color=green style=filled];

)";
  out << absl::FormatTime(kFmt, absl::Now(), absl::LocalTimeZone());

  for (auto& s : all_nodes_) {
    std::string label = s.second.label;
    std::string style, color, fillcolor;

    if (label.empty()) label = absl::StrCat("@", s.first);
    if (s.second.dim.has_value())
      label = absl::StrCat(label, " ", s.second.dim->to_str());
    if (s.second.dim) color = "red";
    if (s.second.pinned) {
      style = "filled";
      fillcolor = "cyan";
    } else if (label[0] != '@' && label[0] != '#') {
      style = "filled";
      fillcolor = "yellow";
    }
    if (!s.second.has_value) style = absl::StrCat(style, ",dashed");

    out << "  " << s.first << " [label=\"" << label << "\"";
    if (!color.empty()) out << " color=" << color;
    if (!style.empty()) out << " style=\"" << style << "\"";
    if (!fillcolor.empty()) out << " fillcolor=" << fillcolor;
    out << "];\n";
  }

  out << "}\n";
}

int RenderAsGraphViz::AddDefault(const ExpressionNode* n) {
  int ret = id_.size();
  auto& it = all_nodes_[ret];
  auto sem = doc_->TryGetNode(n);
  if (sem) {
    it.dim = sem->dim;
    it.has_value = sem->resolved;
  }
  CHECK(id_.emplace(n, ret).second);
  return ret;
}

bool RenderAsGraphViz::Process(const BinaryExpression& e) {
  if (!e.left()->VisitNode(this) || !e.right()->VisitNode(this)) return false;

  AddDefault(&e);
  auto id_self = id_.at(&e);
  all_edges_[{id_self, id_.at(e.left())}];
  all_edges_[{id_self, id_.at(e.right())}];
  return true;
}

///////////////////////////////

bool RenderAsGraphViz::operator()(const Equality& e) {
  if (!e.left()->VisitNode(this) || !e.right()->VisitNode(this)) return false;

  all_edges_[{id_.at(e.left()), id_.at(e.right())}].equ = true;
  return true;
}

bool RenderAsGraphViz::operator()(const LiteralValue& l) {
  auto r = AddDefault(&l);
  all_nodes_[r].label = absl::StrCat("#[", l.value(), "]");
  return true;
}

bool RenderAsGraphViz::operator()(const NamedValue& n) {
  auto r = named_.emplace(n.name(), id_.size());
  auto& it = all_nodes_[r.first->second];
  auto sem = doc_->TryGetNamedNode(n.name());

  if (r.second) it.label = n.name();
  if (sem) {
    it.dim = sem->dim;
    it.has_value = sem->resolved;
  }

  id_.emplace(&n, r.first->second);
  return true;
}

bool RenderAsGraphViz::operator()(const PowerExp& e) {
  if (!e.base()->VisitNode(this)) return false;

  AddDefault(&e);
  all_edges_[{id_.at(&e), id_.at(e.base())}];
  return true;
}

bool RenderAsGraphViz::operator()(const ProductExp& p) { return Process(p); }
bool RenderAsGraphViz::operator()(const QuotientExp& q) { return Process(q); }
bool RenderAsGraphViz::operator()(const SumExp& s) { return Process(s); }
bool RenderAsGraphViz::operator()(const DifExp& d) { return Process(d); }

bool RenderAsGraphViz::operator()(const NegativeExp& n) {
  if (!n.value()->VisitNode(this)) return false;

  AddDefault(&n);
  all_edges_[{id_.at(&n), id_.at(n.value())}];

  return true;
}

bool RenderAsGraphViz::operator()(const Define& d) {
  auto it = named_.find(d.name());
  if (it != named_.end()) {
    all_nodes_[it->second].pinned = true;
    return true;
  }

  // Ignore preamble.
  if (d.location().filename == kPreamble) return true;

  // Un-used nodes get negative id.
  auto& n = all_nodes_[-(++unknown_)];
  n.pinned = true;
  n.label = d.name();

  return true;
}

bool RenderAsGraphViz::operator()(const Document& d) {
  for (auto e : d.equality())
    if (!e->VisitNode(this)) return false;
  for (auto d : d.defines())
    if (!d->VisitNode(this)) return false;
  return true;
}

}  // namespace tbd
