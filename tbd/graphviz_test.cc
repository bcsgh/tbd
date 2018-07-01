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

#include "absl/strings/str_split.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "tbd/common.h"
#include "tbd/semantic.h"

namespace tbd {

class GraphVizTest : public ::testing::Test {
 protected:
  void Pop(RenderAsGraphViz* gv) {
    gv->all_nodes_[0].label = "a";
    gv->all_nodes_[1].label = "b";
    gv->all_nodes_[2].pinned = true;
    gv->all_nodes_[2].has_value = true;
    gv->all_nodes_[2].label = "c";
    gv->all_nodes_[2].dim = Dimension::Dimensionless();
  }
};

namespace {

void RemoveTimestamp(std::set<std::string>* lines) {
  for (auto i = lines->begin(); i != lines->end(); i++) {
    if (i->find("_date_time_") != std::string::npos) {
      lines->erase(i);
      break;
    }
  }
}

TEST_F(GraphVizTest, Blank) {
  SemanticDocument sem;
  std::stringstream out(std::ios_base::out);
  {
    // Force destruction
    RenderAsGraphViz gv(&sem, out);
  }
  std::set<std::string> lines = absl::StrSplit(out.str(), "\n");
  RemoveTimestamp(&lines);
  EXPECT_THAT(lines, testing::UnorderedElementsAre("digraph {", "", "}"));
}

TEST_F(GraphVizTest, Pop) {
  SemanticDocument sem;
  std::stringstream out(std::ios_base::out);
  {
    // Force destruction
    RenderAsGraphViz gv(&sem, out);
    Pop(&gv);
  }

  std::set<std::string> lines = absl::StrSplit(out.str(), "\n");
  RemoveTimestamp(&lines);
  EXPECT_THAT(
      lines,
      testing::UnorderedElementsAre(                                          //
          "digraph {",                                                        //
          "  0 [label=\"a\" style=\"filled,dashed\" fillcolor=yellow];",      //
          "  1 [label=\"b\" style=\"filled,dashed\" fillcolor=yellow];",      //
          "  2 [label=\"c []\" color=red style=\"filled\" fillcolor=cyan];",  //
          "",                                                                 //
          "}"                                                                 //
          ));
}

}  // namespace
}  // namespace tbd
