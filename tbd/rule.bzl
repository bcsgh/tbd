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

"""
# Bazel/skylark rules to process .tbd files.

## `MODULE.bazel`

```
bazel_dep(
    name = "com_github_bcsgh_tbd",
    version = ...,
)
```
"""

def gen_tbd(
        name = None,
        srcs = None,
        cpp = None,
        dot = None,
        out = None,
        warnings_as_errors = False):
    """Process a .tbd file.

    Args:
      name: The target name.
      srcs: The input file.
      cpp: If set, generate a C++ implementation at the give location.
      dot: If set, generate a graphviz depiction at the give location.
      out: Output the resolved values at the give location.
      warnings_as_errors: Fail on warnings.
    """
    if not srcs:
        fail("srcs must be provided")
    if not name:
        fail("name must be provided")

    cmd = "$(location @com_github_bcsgh_tbd//tbd:tbd)"
    if warnings_as_errors:
        cmd += " --warnings_as_errors"

    cmd += " --src=$<"
    cmd += " > $(location " + out + ")"

    outs = [out]

    if cpp:
        cmd += " --cpp_output=$(location " + cpp + ")"
        outs.append(cpp)

    if dot:
        cmd += " --graphviz_output=$(location " + dot + ")"
        outs.append(dot)

    native.genrule(
        name = name,
        srcs = srcs,
        outs = outs,
        tools = ["@com_github_bcsgh_tbd//tbd:tbd"],
        cmd = cmd,
    )
