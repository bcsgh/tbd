workspace(name = "tbd")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

# Note:
# local_repository(name = "...", path = "/home/...")

#############################################
# needed by com_github_glog_glog
git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",  # current as of 2020/11/12
    shallow_since = "1604052972 +0000",
)

#############################################
git_repository(
    name = "com_github_glog_glog",
    remote = "git://github.com/google/glog.git",
    commit = "c8f8135a5720aee7de8328b42e4c43f8aa2e60aa",  # current as of 2020/11/12
    shallow_since = "1601711556 +0200",
)

#############################################
git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "0e202cdbe36b87b4fb68de2b5295a794a10c5cf8",  # current as of 2020/11/12
    shallow_since = "1605205962 -0500",
)

#############################################
git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "0453e1653372fd719557aa70eaad34f691e0d5ae",  # current as of 2020/11/12
    shallow_since = "1605107847 -0500",
)

#############################################
new_git_repository(
    name = "eigen",
    build_file = "@//:BUILD.eigen",
    remote = "git://github.com/eigenteam/eigen-git-mirror.git",
    commit = "36b95962756c1fce8e29b1f8bc45967f30773c00",  # current as of 2020/11/12
    shallow_since = "1575476533 +0100",
)

#############################################
git_repository(
    name = "bazel_rules",
    remote = "git://github.com/bcsgh/bazel_rules.git",
    commit = "abedeb83ecbfca3448205de85dcd7281d70f05b4",  # current as of 2020/11/17
    #shallow_since = "1605160622 -0800",
)
