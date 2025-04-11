target("spdlog")
    set_languages("c++20")
    set_kind("$(kind)")
    add_includedirs("include", { public = true })
    add_files("src/*.cpp")
    add_defines("SPDLOG_COMPILED_LIB=")
target_end()

table.insert(global_deps, "spdlog")