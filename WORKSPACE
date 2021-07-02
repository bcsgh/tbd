workspace(name = "tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
# needed by com_github_glog_glog
git_repository(
    name = "com_github_gflags_gflags",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",  # current as of 2021/02/17
    remote = "git://github.com/gflags/gflags.git",
    shallow_since = "1604052972 +0000",
)

#############################################
git_repository(
    name = "com_github_glog_glog",
    commit = "6e46c125e3718ce64570883e2ca558b3c853a125",  # current as of 2021/07/01
    remote = "git://github.com/google/glog.git",
    shallow_since = "1625119458 +0200",
)

#############################################
git_repository(
    name = "com_google_googletest",
    commit = "4ec4cd23f486bf70efcc5d2caa40f24368f752e3",  # current as of 2021/07/01
    remote = "git://github.com/google/googletest.git",
    shallow_since = "1625074437 -0400",
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "9a7e447c511dae7276ab65fde4d04f6ed52b39c9",  # current as of 2021/07/01
    remote = "git://github.com/abseil/abseil-cpp.git",
    shallow_since = "1624810227 -0400",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "f2344ebcc063a98444ea22b07eb795277543ddff",  # current as of 2021/07/01
    remote = "git://github.com/bcsgh/bazel_rules.git",
    shallow_since = "1625189187 -0700",
)

load("@bazel_rules//repositories:repositories.bzl", "eigen")

#############################################
eigen()
