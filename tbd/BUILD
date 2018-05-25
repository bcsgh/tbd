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

cc_library(
  name = "util",
  hdrs = ["util.h"],
)

cc_library(
  name = "test_main",
  srcs = ["test_main.cc"],
  deps = [
    "@com_github_gflags_gflags//:gflags",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "ast",
  srcs = ["ast.cc"],
  hdrs = ["ast.h"],
  deps = [
    ":common",
    ":dimensions",
    "@com_google_absl//absl/base:core_headers", 
    "@com_google_absl//absl/memory", 
  ],
)

cc_test(
  name = "ast_test",
  srcs = ["ast_test.cc"],
  timeout = "short",
  deps = [
    ":ast",
    ":common",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "semantic",
  srcs = ["semantic.cc"],
  hdrs = ["semantic.h"],
  deps = [
    ":ast",
    ":common",
    ":dimensions",
    "@com_google_absl//absl/types:optional"
  ],
)

cc_test(
  name = "semantic_test",
  srcs = ["semantic_test.cc"],
  timeout = "short",
  deps = [
    ":ast",
    ":common",
    ":semantic",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "common",
  srcs = ["common.cc"],
  hdrs = ["common.h"],
  deps = [
    "@com_google_absl//absl/debugging:stacktrace",
    "@com_google_absl//absl/strings",
    "@com_github_gflags_gflags//:gflags",
  ],
)

cc_test(
  name = "common_test",
  srcs = ["common_test.cc"],
  timeout = "short",
  deps = [
    ":common",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "parser_lib",
  srcs = [
    "parser.cc",
    ":lexer",
    ":parser",
  ],
  hdrs = [
    "parser.h",
    ":lexer",
    ":parser",
    "gen.lexer.h",
    "gen.parser.h",
  ],
  copts = [ # because bison
    "-fexceptions",
    "-Wno-sign-compare",
  ],
  deps = [
    ":ast",
    ":parser_support",
    "@com_google_absl//absl/strings",
    "@com_github_gflags_gflags//:gflags",
  ],
)

cc_library(
  name = "parser_support",
  srcs = ["parser_support.cc"],
  hdrs = ["parser_support.h"],
)

cc_test(
  name = "parser_support_test",
  srcs = ["parser_support_test.cc"],
  timeout = "short",
  deps = [
    ":common",
    ":parser_support",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

load("//tools:cc_embed_data.bzl", "cc_embed_data")
cc_embed_data(
    name = "preamble",
    srcs = ["preamble.tbd"],
)

cc_library(
  name = "tbd_lib",
  srcs = ["tbd.cc"],
  hdrs = ["tbd.h"],
  deps = [
    ":ast",
    ":common",
    ":evaluate",
    ":graphviz",
    ":parser_lib",
    ":preamble",
    ":resolve_units",
    ":validate",
    "@com_github_gflags_gflags//:gflags",
    "@com_google_absl//absl/memory",
  ],
)

cc_binary(
  name = "tbd",
  srcs = ["tbd-main.cc"],
  deps = [
    ":common",
    ":tbd_lib",
    "@com_github_gflags_gflags//:gflags",
  ],
  visibility = ["//visibility:public"],
)

load("//tools:parser.bzl", "genlex", "genyacc")

genyacc(
  name = "parser",
  src = "parser.y",
)

genlex(
  name = "lexer",
  src = "lexer.l",
)

cc_library(
  name = "dimensions",
  srcs = ["dimensions.cc"],
  hdrs = ["dimensions.h"],
)

cc_test(
  name = "dimensions_test",
  srcs = ["dimensions_test.cc"],
  timeout = "short",
  deps = [
    ":common",
    ":dimensions",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "resolve_units",
  srcs = ["resolve_units.cc"],
  hdrs = ["resolve_units.h"],
  deps = [
    ":ast",
    ":common",
    ":dimensions",
    ":semantic",
    ":util",
    "@com_google_absl//absl/strings",
    "@com_github_gflags_gflags//:gflags",
  ],
)

cc_test(
  name = "resolve_units_test",
  srcs = ["resolve_units_test.cc"],
  timeout = "short",
  deps = [
    ":ast",
    ":common",
    ":resolve_units",
    ":semantic",
    ":test_main",
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "validate",
  srcs = ["validate.cc"],
  hdrs = ["validate.h"],
  deps = [
    ":ast",
    ":common",
    ":semantic",
    "@com_github_gflags_gflags//:gflags",
  ],
)

cc_test(
  name = "validate_test",
  srcs = ["validate_test.cc"],
  deps = [
    ":common",
    ":evaluate",
    ":validate",
    ":test_main",
    "@com_google_absl//absl/memory", 
    "@com_google_googletest//:gtest",
  ],
)

cc_library(
  name = "evaluate",
  srcs = ["evaluate.cc"],
  hdrs = ["evaluate.h"],
  deps = [
    ":ast",
    ":common",
    ":semantic",
  ],
)

cc_library(
  name = "graphviz",
  srcs = ["graphviz.cc"],
  hdrs = ["graphviz.h"],
  deps = [
    ":ast",
    ":common",
    ":semantic",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/time",
    "@com_google_absl//absl/types:optional"
  ],
)

cc_test(
  name = "graphviz_test",
  srcs = ["graphviz_test.cc"],
  timeout = "short",
  deps = [
    ":common",
    ":graphviz",
    ":test_main",
    "@com_google_absl//absl/strings",
    "@com_google_googletest//:gtest",
  ],
)
