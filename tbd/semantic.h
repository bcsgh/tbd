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

#ifndef TBD_SEMANTIC_H_
#define TBD_SEMANTIC_H_

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "absl/types/optional.h"
#include "tbd/ast.h"
#include "tbd/dimensions.h"

namespace tbd {

class SemanticDocument final {
 public:
  SemanticDocument();

  bool AddUnit(const std::string& name, Unit u);
  const Unit* LookupUnit(const std::string& name) const;
  void LogUnits(std::ostream&) const;

  struct Exp {
    std::string name;

    absl::optional<Dimension> dim = absl::nullopt;
    absl::optional<Unit> unit = absl::nullopt;
    std::string unit_name = "?";

    bool resolved = false;       // A way to solve for this has been found.
    bool equ_processed = false;  // This equation has been used.
    bool is_literal = false;     // This 'equation' is just a literal value.
    double value = NAN;

    bool referenced = false;

    const Define* def = nullptr;
    const Specification* spec = nullptr;
    const ExpressionNode* node = nullptr;
  };

  Exp* GetNodeForName(std::string name);

  // Reference a named node, creating it if needed.
  Exp* RefernceNamedNode(const NamedValue* node);

  // Get or create a new node (named, unnamed or anon).
  Exp* GetNamedNode(const Define* node);
  Exp* GetUnnamedNode(const ExpressionNode* node);
  Exp* GetNode();

  // Get a named/unnamed node or null.
  Exp* TryGetNamedNode(std::string name);
  Exp* TryGetNode(const ExpressionNode* name);
  std::vector<const Exp*> nodes() const;

 private:
  std::map<std::string, Unit> units_;

  std::vector<std::unique_ptr<Exp>> nodes_;
  std::map<const ExpressionNode*, Exp*> id_nodes_;
  std::map<std::string, Exp*> named_nodes_;
};

std::ostream& operator<<(std::ostream&, const SemanticDocument::Exp&);

}  // namespace tbd

#endif  // TBD_SEMANTIC_H_
