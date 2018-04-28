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

#ifndef TBD_RESOLVE_UNITS_H_
#define TBD_RESOLVE_UNITS_H_

#include <string>

#include "tbd/ast.h"
#include "tbd/semantic.h"
#include "tbd/util.h"

namespace tbd {

class ResolveUnits final : public VisitNodes {
 public:
  ResolveUnits(SemanticDocument* doc) : doc_(doc){};

 private:
  bool operator()(const UnitExp&) override;
  bool operator()(const UnitDef&) override;
  bool operator()(const Equality&) override;
  bool operator()(const LiteralValue&) override;
  bool operator()(const NamedValue&) override { return true; }
  bool operator()(const PowerExp&) override;
  bool operator()(const ProductExp&) override;
  bool operator()(const QuotientExp&) override;
  bool operator()(const SumExp&) override;
  bool operator()(const DifExp&) override;
  bool operator()(const NegativeExp&) override;
  bool operator()(const Define&) override;
  bool operator()(const Specification&) override;
  bool operator()(const Document&) override;

  SemanticDocument* doc_;
  bool down_ = false;      // propagate units downward;
  bool progress_ = false;  // Set when a expressions unit it deduced.

  // Working data
  Unit unit_value_ = Unit{0.0, Dimension::Dimensionless()};
  std::string unit_name_;
};

}  // namespace tbd

#endif  // TBD_RESOLVE_UNITS_H_
