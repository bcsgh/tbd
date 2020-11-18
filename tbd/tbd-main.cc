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
#include <iterator>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "gflags/gflags.h"  // TODO dump
#include "glog/logging.h"

ABSL_FLAG(std::string, src, "", "The file to read from");

ABSL_FLAG(std::string, graphviz_output, "",
          "Output the sysyem of equations in GraphVis format. "
          "Mostly for debugging");
ABSL_FLAG(std::string, cpp_output, "",
          "Output the sequnce of operation for solving for the unknowns as "
          "C++ assignment expressions.");
ABSL_FLAG(bool, dump_units, false, "Dump the set of know units to stdout");

// TODO: Dump this once absl get logging.
ABSL_FLAG(bool, alsologtostderr_x, false,
          "log messages go to stderr in addition to logfiles");
ABSL_FLAG(bool, logtostderr_x, false,
          "log messages go to stderr instead of logfiles");
ABSL_FLAG(int32_t, v_x, 0,  //
          "Show all VLOG(m) messages for m <= this.");

DECLARE_bool(alsologtostderr);
DECLARE_bool(logtostderr);
DECLARE_int32(v);

class StreamSink : public tbd::ProcessOutput, public tbd::UnitsOutput {
 public:
  StreamSink(std::ostream& o) : out(o) {}

  void Error(const std::string &str) const override {
    out << str << "\n";
  }
  void Output(const std::string &name, const tbd::Unit& unit) const override {
    out << name << " = " << unit << "\n";
  }

 private:
  std::ostream& out;
};

int main(int argc, char** argv) {
  auto args = absl::ParseCommandLine(argc, argv);
  // Forward flags to glog (it doesn't use absl::Flags).
  FLAGS_alsologtostderr = absl::GetFlag(FLAGS_alsologtostderr_x);
  FLAGS_logtostderr = absl::GetFlag(FLAGS_logtostderr_x);
  FLAGS_v = absl::GetFlag(FLAGS_v_x);
  google::InitGoogleLogging(args[0]);
  google::InstallFailureSignalHandler();

  if (absl::GetFlag(FLAGS_src).empty()) {
    LOG(ERROR) << "No --src given";
    return 1;
  }
  LOG(INFO) << absl::GetFlag(FLAGS_src);

  std::ifstream in;
  in.open(absl::GetFlag(FLAGS_src), std::ios::in);
  if (in.fail()) {
    LOG(ERROR) << "'" << absl::GetFlag(FLAGS_src)
               << "': " << std::strerror(errno);
    return 1;
  }
  std::string file_string(std::istreambuf_iterator<char>(in), {});
  in.close();

  StreamSink out(std::cout);

  auto processed = tbd::ProcessInput(absl::GetFlag(FLAGS_src), file_string, out);
  if (!processed) return 1;

  if (absl::GetFlag(FLAGS_dump_units)) processed->sem.LogUnits(out);

  if (!absl::GetFlag(FLAGS_graphviz_output).empty() &&
      !RenderGraphViz(absl::GetFlag(FLAGS_graphviz_output), *processed)) {
    LOG(ERROR) << "Failed to render '" << absl::GetFlag(FLAGS_src)
               << "' as GraphViz";
  }

  if (!absl::GetFlag(FLAGS_cpp_output).empty() &&
      !RenderCpp(absl::GetFlag(FLAGS_cpp_output), *processed)) {
    LOG(ERROR) << "Failed to render '" << absl::GetFlag(FLAGS_src)
               << "' as C++";
  }

  for (const auto& l : tbd::GetValues(*processed)) {
    std::cout << l;
  }
  std::cout << std::endl;

  return 0;
}
