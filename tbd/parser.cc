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

#include "tbd/parser.h"

#include <iostream>
#include <string>

#include "absl/flags/flag.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "parser/parser_support.h"
#include "tbd/gen.lexer.h"

#if defined(YYDEBUG) && YYDEBUG
ABSL_FLAG(bool, parser_debug, false, "Enable debuging of the parser");
#endif

YY_DECL;  // Forward declare
int tbdlex(tbd_parser::parser::semantic_type* stype,
           tbd_parser::parser::location_type* loc, tbdscan_t scanner) {
  return tbdlex_inner(stype, loc, scanner);
}

namespace tbd {

int Parse(std::string filename, absl::string_view file,
          std::function<void(const std::string&)> outp, Document* doc) {
  parser_support::ScannerExtra extra;
  extra.filename = &filename;
  extra.outp = outp;

  tbdscan_t scanner;
  tbdlex_init(&scanner);
  auto buffer_state = tbd_scan_bytes(file.data(), file.size(), scanner);
  tbdset_lineno(1, scanner);
  tbdset_column(1, scanner);
  tbdset_extra(&extra, scanner);
  int ret = 0;
  {
    tbd_parser::parser p{scanner, doc, &extra};
#if defined(YYDEBUG) && YYDEBUG
    p.set_debug_level(absl::GetFlag(FLAGS_parser_debug));
    p.set_debug_stream(std::cout);
#endif
    ret = p.parse();
  }
  tbd_delete_buffer(buffer_state, scanner);
  tbdlex_destroy(scanner);
  return ret;
}

}  // namespace tbd
