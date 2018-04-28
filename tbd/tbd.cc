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

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "absl/memory/memory.h"
#include "gflags/gflags.h"
#include "tbd/ast.h"
#include "tbd/common.h"
#include "tbd/evaluate.h"
#include "tbd/graphviz.h"
#include "tbd/parser.h"
#include "tbd/preamble_emebed_data.h"
#include "tbd/resolve_units.h"
#include "tbd/validate.h"

DEFINE_string(graphviz_output, "",
              "Output the sysyem of equations in GraphVis format. "
              "Mostly for debugging");
DEFINE_bool(dump_units, false, "Dump the set of know units to stdout");

namespace tbd {

bool Process(const std::string& src, const std::string& file_string) {
  Document doc;

  CHECK(Parse(kPreamble, ::tbd_preamble_tbd, &doc) == 0);

  if (int ret = Parse(src, file_string, &doc)) {
    LOG(ERROR) << "Parse failed with rc=" << ret;
    return false;
  }

  SemanticDocument sem;

  if (!doc.VisitNode(Validate(&sem).as_ptr())) {
    LOG(ERROR) << "Failed to validate '" << src << "'";
    return false;
  }

  if (!doc.VisitNode(ResolveUnits(&sem).as_ptr())) {
    LOG(ERROR) << "Failed to resolve units for '" << src << "'";
    return false;
  }
  if (FLAGS_dump_units) sem.LogUnits(std::cout);

  if (!doc.VisitNode(Evaluate(&sem).as_ptr())) {
    LOG(ERROR) << "Failed to Evaluate values for '" << src << "'";
    return false;
  }

  if (!FLAGS_graphviz_output.empty()) {
    std::ofstream out;
    out.open(FLAGS_graphviz_output, std::ios::out);
    CHECK(!out.fail()) << FLAGS_graphviz_output << ": " << std::strerror(errno);

    if (!doc.VisitNode(RenderAsGraphViz(&sem, out).as_ptr())) {
      LOG(ERROR) << "Failed to render '" << src << "' as GraphViz";
    }
  }

  for (const auto* node : sem.nodes()) {
    if (node->node && node->node->location().filename == kPreamble) continue;
    std::cout << *node;
  }
  std::cout << std::endl;

  return true;
}

}  // namespace tbd