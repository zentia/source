add_rules("mode.debug", "mode.release")

set_languages("clatest", "cxxlatest")

add_requires("eventpp", "vulkansdk")
set_encodings("source:utf-8", "target:utf-8")
set_toolchains("clang")

global_includedirs = {}
global_headerfiles = {}
global_files = {}
global_deps = {}
global_defines = {}
global_on_load = {}

includes(
    "external/xmake.lua",
    "module/xmake.lua",
    "platform/xmake.lua",
    "runtime/xmake.lua"
    )

rule("module")
    on_load(function (target)
        if is_mode("debug", "releasedbg") then
            target:set("kind", "shared")
            if is_plat("windows") then
                import("core.project.rule")
                local rule = rule.rule("utils.symbols.export_all")
                target:rule_add(rule)
                target:extraconf_set("rules", "utils.symbols.export_all", {export_classes = true})
            end
        elseif is_mode("release") then
            target:set("kind", "static")
        else
            assert(false, "Unknown build kind")
        end
    end)
    after_load(function (target)
        target:add("syslinks", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32")
        target:add("syslinks", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32", "comctl32")
        target:add("syslinks", "comdlg32", "setupapi", "shlwapi", "d3d12", "dxgi", "Dbghelp")
        if not target:is_plat("mingw") then
            target:add("syslinks", "strsafe")
        end
    end)
rule_end()

target("source")
    set_kind("$(kind)")
    add_packages("glad", "fmt", "volk", "vulkansdk", "eventpp")
    add_headerfiles("editor/**.h")
    add_files("editor/**.cpp|editor/Platform/Windows/entry_point/Main.cpp")
    
    add_headerfiles(
        "module/camera/**.h",
        "module/config/**.h",
        "module/render/**.h",
        "module/rhi/**.h",
        "module/spine/**.h",
        "module/time/**.h",
        "module/ui/**.h",
        "module/window/**.h",
        "module/world/**.h"
        )
    add_headerfiles(table.unpack(global_headerfiles))
    add_files(
        "module/camera/**.cpp",
        "module/config/**.cpp",
        "module/render/**.cpp",
        "module/rhi/**.cpp",
        "module/spine/**.cpp",
        "module/time/**.cpp",
        "module/ui/**.cpp",
        "module/window/**.cpp",
        "module/world/**.cpp"
        )
    add_files(table.unpack(global_files))

    add_headerfiles("configuration/**.h")
    if is_mode("debug") then
        add_cxflags("-DDEBUG")
    end
    add_includedirs("./", {public = true})
    add_includedirs(table.unpack(global_includedirs))
    if is_os("windows") then
        add_defines("PLATFORM_WIN")
        add_defines("__x86_64__")
        add_syslinks("synchronization")
    elseif is_os("linux") then 
    elseif is_os("android") then 
    elseif is_os("macosx") then 
    elseif is_os("ios") then 
    end
    add_defines("SOURCE_EDITOR")
    add_defines("ENABLE_ASSERTIONS")
    add_defines("EASTL_USER_DEFINED_ALLOCATOR=")
    set_pcxxheader("precompiled_header/source_prefix.h")
    add_deps(
        "reflect-cpp",
        "spine-runtimes"
    )
    add_deps(table.unpack(global_deps))
    add_defines(table.unpack(global_defines))
    add_rules("module")
    -- add_rules("c++.unity_build", {batchsize = 2})
    on_load(function(target)
        import("runtime.backends.cuda.module")(target)
    end)
    after_build(function (target) 
        local source_file = os.projectdir() .. "\\configuration\\development\\source_editor.json"
        local build_dir = os.projectdir() .. "\\" .. target:targetdir()

        if os.isfile(source_file) then 
            os.cp(source_file, build_dir)
        else 
            print("file " .. source_file .. " does not exist.")
        end 

        local source_dir = os.projectdir() .. "\\resource"

        if os.isdir(source_dir) then 
            os.cp(source_dir, build_dir)
        else 
            print("dir " .. source_dir .. " does not exist.")
        end 
    end)
target_end()

target("source_editor")
    set_kind("binary")
    add_files("Editor/Platform/Windows/entry_point/main.cpp")
    add_deps("source")
    add_defines("SOURCE_EDITOR")
target_end()