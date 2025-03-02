add_rules("mode.debug", "mode.release")

local function SourceCommon()
    set_languages("c99", "c++17")
    add_cxxflags("gxx::-fexec-charset=GBK")
    if is_mode("debug") then
        add_cxflags("-DDEBUG")
    end
    
    add_includedirs("./")
    if is_os("windows") then
        add_defines("PLATFORM_WIN")
        add_rules("win.sdk.application")
        
    elseif is_os("linux") then 
    elseif is_os("android") then 
    elseif is_os("macosx") then 
    elseif is_os("ios") then 
    end
end 

target("SourceEditor")
    set_kind("binary")
    add_files("Editor/Platform/Windows/EntryPoint/Main.cpp")
    SourceCommon()

target("Source")
    set_kind("shared")

    add_headerfiles("Editor/**.h|Platform/Windows/EntryPoint/*.h")
    add_files("Editor/**.cpp|platform/Windows/EntryPoint/*.cpp")

    add_headerfiles("Runtime/**.h")
    add_files("Runtime/**.cpp")

    add_headerfiles("PlatformDependent/**.h")
    add_files("PlatformDependent/**.cpp")

    add_headerfiles("Modules/**.h")
    add_files("Modules/**.cpp")

    add_headerfiles("External/**.h")

    add_includedirs("./External")

    add_headerfiles("Configuration/**.h")

    add_includedirs("./PlatformDependent/Win")
    add_includedirs("./Platforms/Windows/Configuration")

    add_headerfiles("Platforms/**.h")

    SourceCommon()
    add_defines("SOURCE_EDITOR")
    add_defines("ENABLE_ASSERTIONS")
    add_includedirs("PrecompiledHeaders")
    set_pcxxheader("PrecompiledHeaders/SourcePrefix.h")
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

