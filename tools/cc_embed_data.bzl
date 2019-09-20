# Copyright (c) 2018, Benjamin Shropshire,
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# implementation from:
# https://stackoverflow.com/a/7779766/1343
# http://www.math.utah.edu/docs/info/ld_2.html
# https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html

def cc_embed_data(name=None, srcs=None):
  if not srcs:
    fail("srcs must be provided")
  if not name:
    fail("name must be provided")

  cc = name + "_emebed_data.cc"
  h = name + "_emebed_data.h"
  o = name + "_emebed_data.o"

  #files = " ".join(["%s@$(location %s)" % (x, x) for x in srcs])

  native.genrule(
    name = name + "_make_emebed_src",
    outs = [cc, h],
    srcs = srcs,
    tools = ["@tbd//tools:make_emebed_data"],
    cmd = " ".join([
        "$(location @tbd//tools:make_emebed_data)",
          "--h=$(location %s) --cc=$(location %s)" % (h, cc),
          "--gendir=$(GENDIR) --workspace=$$(basename $$PWD)",
          "$(SRCS)",
    ])
  )
  
  native.genrule(
    name = name + "_make_embed_obj",
    outs = [o],
    srcs = srcs,
    cmd = (
      "$(CC) $(CC_FLAGS) -nostdlib -o $(location %s) -no-pie -Xlinker -r -Wl,-b -Wl,binary $(SRCS)"
    ) % (o),
    toolchains = [
      "@bazel_tools//tools/cpp:current_cc_toolchain",
      "@tbd//tools:cc_flags",
    ],
  )

  native.cc_library(
    name = name,
    srcs = [cc, o],
    hdrs = [h],
    deps = ["@com_google_absl//absl/strings"]
  )