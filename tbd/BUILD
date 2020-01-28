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

load("@bazel_rules//cc_embed_data:cc_embed_data.bzl", "cc_embed_data")
load("@bazel_rules//parser:parser.bzl", "genlex", "genyacc")

cc_library(
    name = "util",
    hdrs = ["util.h"],
)

cc_library(
    name = "test_main",
    srcs = ["test_main.cc"],
    deps = [
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "ast",
    srcs = ["ast.cc"],
    hdrs = ["ast.h"],
    deps = [
        ":dimensions",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "ast_test",
    timeout = "short",
    srcs = ["ast_test.cc"],
    deps = [
        ":ast",
        ":test_main",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "ops",
    srcs = ["ops.cc"],
    hdrs = ["ops.h"],
    deps = [
        ":semantic",
        "@com_google_absl//absl/base:core_headers",
        "@eigen",
    ],
)

cc_test(
    name = "ops_test",
    timeout = "short",
    srcs = ["ops_test.cc"],
    deps = [
        ":ast",
        ":ops",
        ":semantic",
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
        ":dimensions",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "semantic_test",
    timeout = "short",
    srcs = ["semantic_test.cc"],
    deps = [
        ":ast",
        ":semantic",
        ":test_main",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "select_solvable",
    srcs = ["select_solvable.cc"],
    hdrs = ["select_solvable.h"],
    deps = [
        "@com_github_glog_glog//:glog",
    ],
)

cc_test(
    name = "select_solvable_test",
    timeout = "short",
    srcs = ["select_solvable_test.cc"],
    deps = [
        ":select_solvable",
        ":test_main",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "newton_raphson",
    srcs = ["newton_raphson.cc"],
    hdrs = ["newton_raphson.h"],
    deps = [
        "@com_github_glog_glog//:glog",
        "@eigen",
    ],
)

cc_test(
    name = "newton_raphson_test",
    timeout = "short",
    srcs = ["newton_raphson_test.cc"],
    deps = [
        ":newton_raphson",
        ":test_main",
        "@com_google_googletest//:gtest",
        "@eigen",
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
        "gen.lexer.h",
        "gen.parser.h",
        "parser.h",
        ":lexer",
        ":parser",
    ],
    copts = [
        # because bison
        "-fexceptions",
        "-Wno-sign-compare",
    ],
    deps = [
        ":ast",
        "@bazel_rules//parser:parser_support",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
    ],
)

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
        ":evaluate",
        ":gen_code",
        ":graphviz",
        ":parser_lib",
        ":preamble",
        ":resolve_units",
        ":validate",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
    ],
)

cc_binary(
    name = "tbd",
    srcs = ["tbd-main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":tbd_lib",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
)

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
    timeout = "short",
    srcs = ["dimensions_test.cc"],
    deps = [
        ":dimensions",
        ":test_main",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "find",
    hdrs = ["find.h"],
    deps = [
        ":ast",
    ],
)

cc_test(
    name = "find_test",
    timeout = "short",
    srcs = ["find_test.cc"],
    deps = [
        ":find",
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
        ":dimensions",
        ":semantic",
        ":util",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "resolve_units_test",
    timeout = "short",
    srcs = ["resolve_units_test.cc"],
    deps = [
        ":ast",
        ":resolve_units",
        ":semantic",
        ":test_main",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "validate",
    srcs = ["validate.cc"],
    hdrs = ["validate.h"],
    deps = [
        ":ast",
        ":semantic",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_test(
    name = "validate_test",
    srcs = ["validate_test.cc"],
    deps = [
        ":test_main",
        ":validate",
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
        ":find",
        ":newton_raphson",
        ":ops",
        ":select_solvable",
        ":semantic",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "evaluate_test",
    srcs = ["evaluate_test.cc"],
    deps = [
        ":evaluate",
        ":test_main",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "graphviz",
    srcs = ["graphviz.cc"],
    hdrs = ["graphviz.h"],
    deps = [
        ":ast",
        ":semantic",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "graphviz_test",
    timeout = "short",
    srcs = ["graphviz_test.cc"],
    deps = [
        ":graphviz",
        ":semantic",
        ":test_main",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "gen_code",
    srcs = ["gen_code.cc"],
    hdrs = ["gen_code.h"],
    deps = [
        ":ops",
        ":semantic",
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "gen_code_test",
    timeout = "short",
    srcs = ["gen_code_test.cc"],
    deps = [
        ":gen_code",
        ":test_main",
        "@com_google_googletest//:gtest",
    ],
)
