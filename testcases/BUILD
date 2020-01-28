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

load("//tbd:rule.bzl", "gen_tbd")
load("@bazel_rules//graphviz:graphviz.bzl", "gen_dot")
load("@bazel_rules//build_test:build.bzl", "build_test")

ERROR_CASES = glob(["*.error.tbd"])

SUCCESS_CASES = glob(
    ["*.tbd"],
    exclude = ["*.error.tbd"],
)

[sh_test(
    name = c.replace(".error.tbd", "_failure_test"),
    timeout = "short",
    srcs = ["input_test.sh"],
    args = [
        "$(location //tbd:tbd)",
        "$(location %s)" % c,
    ],
    data = [
        "//tbd",
        c,
    ],
) for c in ERROR_CASES]

test_suite(
    name = "failure_tests",
    tests = [":" + c.replace(".error.tbd", "_failure_test") for c in ERROR_CASES],
)

[gen_tbd(
    name = "gen_" + c.replace(".", "_"),
    srcs = [c],
    out = c.replace("tbd", "txt"),
    cpp = c.replace("tbd", "cpp"),
    dot = c.replace("tbd", "dot"),
    warnings_as_errors = True,
) for c in SUCCESS_CASES]

[gen_dot(
    name = c.replace(".", "_") + "_png",
    src = ":" + c.replace("tbd", "dot"),
    out = c.replace("tbd", "png"),
) for c in SUCCESS_CASES]

[build_test(
    name = c.replace(".tbd", "_success_test"),
    targets = [":gen_" + c.replace(".", "_")],
) for c in SUCCESS_CASES]

[sh_test(
    name = g.replace(".", "_"),
    timeout = "short",
    srcs = ["gold_test.sh"],
    args = [
        "$(location %s)" % g,
        "$(location %s)" % g.replace(".gold", ".txt"),
    ],
    data = [
        g,
        g.replace(".gold", ".txt"),
    ],
) for g in glob(["*.gold"])]

[sh_test(
    name = g.replace(".", "_") + "_cpp",
    timeout = "short",
    srcs = ["gold_test.sh"],
    args = [
        "$(location %s)" % g.replace(".gold", ".gold.cpp"),
        "$(location %s)" % g.replace(".gold", ".cpp"),
    ],
    data = [
        g.replace(".gold", ".cpp"),
        g.replace(".gold", ".gold.cpp"),
    ],
) for g in glob(["*.gold"])]

test_suite(
    name = "success_tests",
    tests = [":" + c.replace(".tbd", "_success_test") for c in SUCCESS_CASES],
)

filegroup(
    name = "outputs",
    srcs = [c.replace("tbd", "txt") for c in SUCCESS_CASES] +
           [c.replace("tbd", "cpp") for c in SUCCESS_CASES] +
           [c.replace("tbd", "dot") for c in SUCCESS_CASES] +
           [c.replace("tbd", "png") for c in SUCCESS_CASES],
)
