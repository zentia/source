set_languages("cxx23")

add_rules("mode.debug","mode.releasedbg", "mode.release", "mode.minsizerel")

add_requires("eventpp", "vulkansdk")
set_encodings("source:utf-8", "target:utf-8")

includes("external/xmake.lua")
includes("modules/xmake.lua")

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
        target:add("syslinks", "comdlg32", "setupapi", "shlwapi", "d3d12", "dxgi")
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
    add_headerfiles("runtime/**.h")
    add_files("runtime/**.cpp")
    
    add_headerfiles("modules/camera/**.h")
    add_headerfiles("modules/config/**.h")
    add_headerfiles("modules/render/**.h")
    add_headerfiles("modules/rhi/**.h")
    add_headerfiles("modules/spine/**.h")
    add_headerfiles("modules/time/**.h")
    add_headerfiles("modules/ui/**.h")
    add_headerfiles("modules/window/**.h")
    add_headerfiles("modules/world/**.h")

    add_files("modules/camera/**.cpp")
    add_files("modules/config/**.cpp")
    add_files("modules/render/**.cpp")
    add_files("modules/rhi/**.cpp")
    add_files("modules/spine/**.cpp")
    add_files("modules/time/**.cpp")
    add_files("modules/ui/**.cpp")
    add_files("modules/window/**.cpp")
    add_files("modules/world/**.cpp")

    add_headerfiles("configuration/**.h")
    add_headerfiles("platforms/**.h")
    add_headerfiles("platforms/**.cpp")
    add_cxxflags("gxx::-fexec-charset=GBK", "/Zc:wchar_t")
    if is_mode("debug") then
        add_cxflags("-DDEBUG")
    end
    add_includedirs("./", {public = true})
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
    set_pcxxheader("precompiled_header/source_prefix.h")
    add_deps(
        "D3D12MemoryAllocator",
        "glfw",
        "glm",
        "imgui", 
        "spdlog",
        "spine-runtimes",
        "VulkanMemoryAllocator"
    )
    add_deps("asset")
    add_rules("module")
    add_rules("c++.unity_build", {batchsize = 2})
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

target("source_editor")
    set_kind("binary")
    add_files("Editor/Platform/Windows/entry_point/main.cpp")
    add_deps("source")
    add_defines("SOURCE_EDITOR")