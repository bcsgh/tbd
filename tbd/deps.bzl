load("@bazel_rules//cc_embed_data:cc_embed_data_deps.bzl", cc_embed_data_deps = "get_deps")
load("@bazel_rules//repositories:repositories.bzl", "load_absl", "load_skylib")

def get_deps():
    #############################################
    load_skylib()

    #############################################
    load_absl()

    cc_embed_data_deps()

    # For "reasons", these have to be in the WORKSPACE.
    # Juct check for them and tell people how to fix thigns.
    NEEDED = {
        "jsoncpp": "com_github_open_source_parsers_jsoncpp",
        "eigen": "eigen",
    }
    missing = [f for f,r in NEEDED.items() if not native.existing_rule(r)]
    if missing:
        print("\n".join([
            "Missing packages needed by @stl-to-ps//. Add the following to WORKSPACE:\n",
            'load("@bazel_rules//repositories:repositories.bzl", "' + '", "'.join(missing) + '")',
        ] + [
            "%s()" % p for p in missing
        ]) + "\n")
