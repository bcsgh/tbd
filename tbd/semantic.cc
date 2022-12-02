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

#include "tbd/semantic.h"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "tbd/ast.h"
#include "tbd/dimensions.h"

namespace tbd {
SemanticDocument::SemanticDocument()
    : units_{
          // Pre-populate with the base SI units.
          {"m", Unit::m()},   {"kg", Unit::kg()}, {"s", Unit::s()},
          {"A", Unit::A()},   {"K", Unit::K()},   {"mol", Unit::mol()},
          {"cd", Unit::cd()},
      } {}

bool SemanticDocument::AddUnit(const std::string& name, Unit unit) {
  return units_.insert({name, unit}).second;
}

const Unit* SemanticDocument::LookupUnit(const std::string& name) const {
  auto i = units_.find(name);
  if (i == units_.end()) return nullptr;
  return &i->second;
}

void SemanticDocument::LogUnits(const UnitsOutput& out) const {
  for (const auto& unit : units_) {
    out.Output(unit.first, unit.second);
  }
}

using Exp = SemanticDocument::Exp;

Exp* SemanticDocument::GetNodeForName(std::string name) {
  auto ret = TryGetNamedNode(name);
  if (ret != nullptr) {
    return ret;
  }

  auto add_name = named_nodes_.emplace(name, nullptr);
  CHECK(add_name.second);

  // Unknown node name, create a new object.
  auto n = absl::make_unique<Exp>();
  n->name = name;
  add_name.first->second = n.get();
  ret = n.get();
  nodes_.emplace_back(std::move(n));

  return ret;
}

Exp* SemanticDocument::RefernceNamedNode(const NamedValue* node) {
  auto* ret = GetNodeForName(node->name());
  if (ret->node == nullptr) ret->node = node;
  auto add_node = id_nodes_.emplace(node, ret);
  CHECK(add_node.first->second == ret);
  ret->referenced = true;
  return ret;
}

Exp* SemanticDocument::GetNamedNode(const Define* node) {
  auto add_node = id_nodes_.emplace(node, nullptr);
  if (!add_node.second) {
    return add_node.first->second;
  }

  auto* ret = GetNodeForName(node->name());
  add_node.first->second = ret;

  if (ret->def == nullptr) ret->def = node;
  if (ret->node == nullptr) ret->node = node;

  return ret;
}

Exp* SemanticDocument::GetUnnamedNode(const ExpressionNode* node) {
  auto add_node = id_nodes_.emplace(node, nullptr);
  if (!add_node.second) {
    LOG(ERROR) << "added duplicate node";
    return add_node.first->second;
  }

  auto n = absl::make_unique<Exp>();
  n->node = node;
  add_node.first->second = n.get();
  auto* ret = n.get();
  nodes_.emplace_back(std::move(n));

  return ret;
}

Exp* SemanticDocument::GetNode() {
  auto n = absl::make_unique<Exp>();
  auto* ret = n.get();
  nodes_.emplace_back(std::move(n));
  return ret;
}

Exp* SemanticDocument::TryGetNode(ExpressionNode const* n) {
  auto it = id_nodes_.find(n);
  if (it == id_nodes_.end()) return nullptr;
  return it->second;
}

Exp* SemanticDocument::TryGetNamedNode(std::string name) {
  auto it = named_nodes_.find(name);
  if (it == named_nodes_.end()) return nullptr;
  return it->second;
}

std::vector<const Exp*> SemanticDocument::nodes() const {
  std::vector<const Exp*> ret;
  ret.reserve(nodes_.size());
  for (const auto& n : nodes_) ret.push_back(n.get());
  return ret;
}

std::ostream& operator<<(std::ostream& out, const SemanticDocument::Exp& node) {
  if (node.name.empty()) return out;

  out << node.name;
  if (!std::isnan(node.value)) {
    double v = node.value;
    if (node.unit.has_value()) v /= node.unit->scale;
    out << " = " << v;
  }

  out << ";";
  const char* x = "\t//";

  if (node.unit.has_value()) {
    out << x << " [" << node.unit_name << "]";
    x = "";
  } else if (node.dim.has_value()) {
    out << x << " " << *node.dim;
    x = "";
  }

  if (node.node != nullptr) {
    out << x << " " << node.node->location().line();
    x = "";
  }

  out << "\n";
  return out;
}

const char* kPreamble = "<<preamble>>";

}  // namespace tbd
