add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate")

add_rules("mode.debug","mode.releasedbg", "mode.release", "mode.minsizerel")

add_requires("glm", "glfw", "glslang", "vulkan-headers")
add_requires("imgui")
add_requires("vulkansdk", "vulkan-memory-allocator")
-- enable unicode
-- add_defines("_UNICODE", "UNICODE")
-- for all source/target encodings
-- set_encodings("utf-8") -- msvc: /utf-8
-- set_encodings("source:utf-8", "target:utf-8")

-- -- gcc/clang: -finput-charset=UTF-8, msvc: -source-charset=utf-8
-- set_encodings("source:utf-8")

-- -- gcc/clang: -fexec-charset=UTF-8, msvc: -target-charset=utf-8
-- set_encodings("target:utf-8")
-- set_arch("x64")
-- set_warnings("all")
-- set_toolchains("clang")

includes("External/xmake.lua")

local function SourceCommon()
    add_cxxflags("gxx::-fexec-charset=GBK")
    if is_mode("debug") then
        add_cxflags("-DDEBUG")
    end
    
    add_includedirs("./")
    if is_os("windows") then
        add_defines("PLATFORM_WIN")
        add_defines("__x86_64__")
        add_rules("win.source.shared")
        add_syslinks("synchronization")
    elseif is_os("linux") then 
    elseif is_os("android") then 
    elseif is_os("macosx") then 
    elseif is_os("ios") then 
    end
end 

-- define rule: shared
rule("win.source.shared")
    -- on_load(function (target)
    --     target:set("kind", "shared")
    -- end)

    after_load(function (target)

        -- -- set subsystem: windows
        -- if target:is_plat("mingw") then
        --     target:add("ldflags", "-mwindows", {force = true})
        -- else
        --     local subsystem = false
        --     for _, ldflag in ipairs(target:get("ldflags")) do
        --         if type(ldflag) == "string" then
        --             ldflag = ldflag:lower()
        --             if ldflag:find("[/%-]subsystem:") then
        --                 subsystem = true
        --                 break
        --             end
        --         end
        --     end
        --     if not subsystem then
        --         target:add("ldflags", "-subsystem:windows", {force = true, tools = {"link"}})
        --     end
        -- end

        -- add links
        target:add("syslinks", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32")
        target:add("syslinks", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32", "comctl32")
        target:add("syslinks", "comdlg32", "setupapi", "shlwapi", "d3d12", "dxgi")
        if not target:is_plat("mingw") then
            target:add("syslinks", "strsafe")
        end
    end)


target("Source")
    set_kind("shared")

    add_packages("glm", "imgui", "glfw", "glad", "fmt", "glslang", "vulkan-headers", "volk", "vulkansdk", "vulkan-memory-allocator")
    
    set_languages("c++20")

    add_headerfiles("Editor/**.h|Platform/Windows/EntryPoint/*.h")
    add_files("Editor/**.cpp|platform/Windows/EntryPoint/*.cpp")

    add_headerfiles("Runtime/**.h")
    add_files("Runtime/**.cpp")

    add_headerfiles("PlatformDependent/**.h")
    add_files("PlatformDependent/**.cpp")

    add_headerfiles("Modules/**.h")
    add_files("Modules/**.cpp")

    add_includedirs("./External")
    External()

    add_headerfiles("Configuration/**.h")

    add_includedirs("./PlatformDependent/Win")
    add_includedirs("./Platforms/Windows/Configuration")

    add_headerfiles("Platforms/**.h")
    add_headerfiles("Platforms/**.cpp")

    SourceCommon()
    add_defines("SOURCE_EDITOR")
    add_defines("ENABLE_ASSERTIONS")
    add_includedirs("PrecompiledHeaders")
    set_pcxxheader("PrecompiledHeaders/SourcePrefix.h")
target_end()
    
target("SourceEditor")
    set_kind("binary")
    add_files("Editor/Platform/Windows/EntryPoint/Main.cpp")
    add_deps("Source")
    SourceCommon()
target_end()
--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

