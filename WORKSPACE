workspace(name = "com_github_bcsgh_tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
git_repository(
    name = "com_google_googletest",
    commit = "b10fad38c4026a29ea6561ab15fc4818170d1c10",  # current as of 2023/11/12
    remote = "https://github.com/google/googletest.git",
    shallow_since = "1698701593 -0700",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "be9e3fa50c41cf9a1e93d2288ce02c67047d71c3",  # current as of 2023/11/16
    remote = "https://github.com/bcsgh/bazel_rules.git",
    shallow_since = "1700184387 -0800",
)

#############################################
register_toolchains("@bazel_rules//parser:linux_flex_bison")

#############################################

load("@bazel_rules//repositories:repositories.bzl", "eigen", "jsoncpp")

eigen()

jsoncpp()

load("//tbd:deps.bzl", tbd_deps = "get_deps")
tbd_deps()
