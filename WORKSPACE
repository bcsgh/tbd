workspace(name = "com_github_bcsgh_tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
git_repository(
    name = "com_google_googletest",
    commit = "3026483ae575e2de942db5e760cf95e973308dd5",  # current as of 2022/10/25
    remote = "https://github.com/google/googletest.git",
    shallow_since = "1666712359 -0700",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "3404db100ba321134ef63c6c872ce8dca6db024d",  # current as of 2023/01/29
    remote = "https://github.com/bcsgh/bazel_rules.git",
    shallow_since = "1675037889 -0800",
)

#############################################
register_toolchains("@bazel_rules//parser:linux_flex_bison")

#############################################

load("@bazel_rules//repositories:repositories.bzl", "eigen", "jsoncpp")

eigen()

jsoncpp()

load("//tbd:deps.bzl", tbd_deps = "get_deps")
tbd_deps()
