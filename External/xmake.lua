local function ACL()
    add_headerfiles("External/acl/includes/acl/**.h")
end 

function External()
    ACL()
    add_deps("baselib")
    add_packages("tlsf")
    add_packages("spdlog")
    -- add_packages("tinyobjloader")
    add_packages("joltphysics")
    add_packages("stb","spine-runtimes","reflect-cpp")

end 

add_requires(
    "tlsf", 
    "spdlog", 
    -- "tinyobjloader", 
    "joltphysics", 
    "stb",
    "spine-runtimes",
    "reflect-cpp",
    "LuisaRender"
)

target("baselib")
    set_kind("static")
    add_headerfiles("baselib/**.h")
    add_files("baselib/**.cpp")
    add_defines("BASELIB_INLINE_NAMESPACE=SourceClassic", {public = true})
    add_includedirs("baselib", {public = true})
    add_includedirs("baselib/Include", {public = true})
    add_syslinks("wsock32", "ws2_32")

package("LuisaRender")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "LuisaRender"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "LUISA_COMPUTE_DOWNLOAD_LLVM=ON")
        import("package.tools.cmake").install(package, configs)
    end)
package_end()