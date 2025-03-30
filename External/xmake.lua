local function ACL()
    add_headerfiles("External/acl/includes/acl/**.h")
end 

function External()
    ACL()
    add_deps("baselib")
    add_deps("imgui")
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
    "reflect-cpp"
)

target("baselib")
    set_kind("static")
    add_headerfiles("baselib/**.h")
    add_files("baselib/**.cpp")
    add_defines("BASELIB_INLINE_NAMESPACE=SourceClassic", {public = true})
    add_includedirs("baselib", {public = true})
    add_includedirs("baselib/Include", {public = true})
    add_syslinks("wsock32", "ws2_32")

target("imgui")
    add_packages("vulkansdk")
    set_kind("static")
    add_headerfiles("imgui/*.h", "imgui/backends/imgui_impl_vulkan.h", "imgui/backends/imgui_impl_glfw.h")
    add_files("imgui/*.cpp", "imgui/backends/imgui_impl_vulkan.cpp", "imgui/backends/imgui_impl_glfw.cpp")
    add_includedirs("imgui", {public = true})