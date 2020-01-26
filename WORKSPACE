workspace(name = "tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
git_repository(
    name = "com_github_gflags_gflags",
    tag = "v2.2.2",  # current as of 2020/1/25
    remote = "git://github.com/gflags/gflags.git",
)

#############################################
git_repository(
    name = "com_google_googletest",
    tag = "release-1.10.0",  # current as of 2020/1/25
    remote = "git://github.com/google/googletest.git",
)

#############################################
git_repository(
    name = "com_google_absl",
    commit = "44427702614d7b86b064ba06a390f5eb2f85dbf6",  # current as of 2020/1/25
    remote = "git://github.com/abseil/abseil-cpp.git",
)

#############################################
new_git_repository(
    name = "eigen",
    tag = "3.3.7",  # current as of 2020/1/25
    remote = "git://github.com/eigenteam/eigen-git-mirror.git",
    build_file = "@//:BUILD.eigen",
)

#############################################
git_repository(
    name = "bazel_rules",
    commit = "ce4c1d49cc89995bebda145f9d3f67f3fd4a06c3",  # current as of 2020/1/26
    remote = "git://github.com/bcsgh/bazel_rules.git",
)
