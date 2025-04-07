target("windows")
    set_group("platform")
    set_kind("$(kind)")
    add_includedirs("./", { public = true })
    add_headerfiles("./*.h")
    add_files("./*.cpp")
target_end()

table.insert(global_deps, "windows")