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

#include "tbd/tbd.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "absl/memory/memory.h"
#include "glog/logging.h"
#include "tbd/ast.h"
#include "tbd/evaluate.h"
#include "tbd/gen_code.h"
#include "tbd/graphviz.h"
#include "tbd/parser.h"
#include "tbd/preamble_emebed_data.h"
#include "tbd/resolve_units.h"
#include "tbd/validate.h"


namespace tbd {

std::unique_ptr<FullDocument> ProcessInput(const std::string& src,
                                           const std::string& file_string) {
  auto ret = absl::make_unique<FullDocument>();

  CHECK(Parse(kPreamble, ::tbd_preamble_tbd(), &ret->doc) == 0);

  if (int rc = Parse(src, file_string, &ret->doc)) {
    LOG(ERROR) << "Parse failed with rc=" << rc;
    return nullptr;
  }

  if (!ret->doc.VisitNode(Validate(&ret->sem).as_ptr())) {
    LOG(ERROR) << "Failed to validate '" << src << "'";
    return nullptr;
  }

  if (!ret->doc.VisitNode(ResolveUnits(&ret->sem).as_ptr())) {
    LOG(ERROR) << "Failed to resolve units for '" << src << "'";
    return nullptr;
  }

  if (!ret->doc.VisitNode(&ret->eva)) {
    LOG(ERROR) << "Failed to Evaluate values for '" << src << "'";
    return nullptr;
  }
  return ret;
}

bool RenderGraphViz(const std::string& sink, FullDocument &full) {
  std::ofstream out;
  out.open(sink, std::ios::out);
  CHECK(!out.fail()) << sink << ": " << std::strerror(errno);

  return full.doc.VisitNode(RenderAsGraphViz(&full.sem, out).as_ptr());
}

std::vector<std::string> GetValues(const std::string &sink,
                                   FullDocument &full) {
  std::vector<std::string> lines;
  for (const auto* node : full.sem.nodes()) {
    if (node->node && node->node->location().filename == kPreamble) continue;
    std::stringstream out(std::ios_base::out);
    out << *node;
    lines.emplace_back(out.str());
  }
  std::sort(lines.begin(), lines.end());
  return lines;
}

bool RenderCpp(const std::string &sink, FullDocument &full) {
  std::ofstream out;
  out.open(sink, std::ios::out);
  CHECK(!out.fail()) << sink << ": " << std::strerror(errno);

  bool success = true;
  CodeEvaluate code(out);
  for (const auto s : full.eva.GetStages()) {
    out << "// Simple\n";
    for (const auto& op : s->direct_ops) {
      if (!op->VisitOp(&code)) {
        LOG(ERROR) << "Error generateding C++ for opertion at "
                   << op->location();
        success = false;
      }
    }
    out << "// system\n";
    for (const auto& op : s->solve_ops) {
      if (!op->VisitOp(&code)) {
        LOG(ERROR) << "Error generateding C++ for opertion at "
                   << op->location();
        success = false;
      }
    }
  }

  return success;
}

}  // namespace tbd