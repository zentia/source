target("asset")
    set_kind("$(kind)")
    add_includedirs("./", { public = true })
    add_headerfiles("asset_module.h")
    add_files("asset_module.cpp")
    add_deps(
        "reflect-cpp",
        "spdlog"
    )