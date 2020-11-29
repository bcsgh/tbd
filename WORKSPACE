workspace(name = "tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
# needed by com_github_glog_glog
git_repository(
    name = "com_github_gflags_gflags",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",  # current as of 2020/11/28
    shallow_since = "1604052972 +0000",
    remote = "git://github.com/gflags/gflags.git",
)

#############################################
git_repository(
    name = "com_github_glog_glog",
    commit = "c8f8135a5720aee7de8328b42e4c43f8aa2e60aa",  # current as of 2020/11/28
    shallow_since = "1601711556 +0200",
    remote = "git://github.com/google/glog.git",
)

#############################################
git_repository(
    name = "com_google_googletest",
    commit = "0e202cdbe36b87b4fb68de2b5295a794a10c5cf8",  # current as of 2020/11/12
    shallow_since = "1605205962 -0500",
    remote = "git://github.com/google/googletest.git",
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "5d8fc9192245f0ea67094af57399d7931d6bd53f",  # current as of 2020/11/28
    shallow_since = "1606203805 -0500",
    remote = "git://github.com/abseil/abseil-cpp.git",
)

#############################################
new_git_repository(
    name = "eigen",
    build_file = "@//:BUILD.eigen",
    commit = "36b95962756c1fce8e29b1f8bc45967f30773c00",  # current as of 2020/11/12 MOVE
    shallow_since = "1575476533 +0100",
    remote = "git://github.com/eigenteam/eigen-git-mirror.git",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "abedeb83ecbfca3448205de85dcd7281d70f05b4",  # current as of 2020/11/28
    shallow_since = "1605677400 -0800",
    remote = "git://github.com/bcsgh/bazel_rules.git",
)
