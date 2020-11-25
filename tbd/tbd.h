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

#ifndef TBD_TBD_H_
#define TBD_TBD_H_

#include <memory>
#include <string>

#include "tbd/ast.h"
#include "tbd/evaluate.h"
#include "tbd/semantic.h"

namespace tbd {

struct FullDocument {
  FullDocument(Evaluate::ErrorSink outp) : eva(&sem, outp) {}

  Document doc;
  SemanticDocument sem;
  Evaluate eva;
};

class ProcessOutput {
 public:
  template <class... T>
  void Error(const T... t) const {
    Error(absl::StrCat(t...));
  }
  virtual void Error(const std::string &str) const = 0;
};

std::unique_ptr<FullDocument> ProcessInput(const std::string &src,
                                           const std::string &file_string,
                                           const ProcessOutput& out);

bool RenderGraphViz(const std::string& sink, FullDocument &full);
bool RenderCpp(const std::string &src, FullDocument &full);

std::vector<std::string> GetValues(FullDocument &full);

}  // namespace tbd

#endif  // TBD_TBD_H_
