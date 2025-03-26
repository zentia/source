local function ACL()
    add_headerfiles("External/acl/includes/acl/**.h")
end 

function External()
    ACL()
    add_deps("baselib")
    add_packages("imgui")
    add_packages("tlsf")
    add_packages("vulkan-hpp")
    add_packages("spdlog")
    add_packages("tinyobjloader")
    add_packages("joltphysics")
    add_packages("stb")
    add_packages("spine-runtimes")
    add_packages("glfw")
    add_packages("reflect-cpp")
end 

add_requires("imgui", "tlsf", "spdlog", "tinyobjloader", "joltphysics", "stb", "spine-runtimes", "vulkan-hpp", "glfw"
, "reflect-cpp"
)

target("baselib")
    set_kind("static")
    add_headerfiles("baselib/**.h")
    add_files("baselib/**.cpp")
    add_defines("BASELIB_INLINE_NAMESPACE=SourceClassic", {public = true})
    add_includedirs("baselib", {public = true})
    add_includedirs("baselib/Include", {public = true})
    add_syslinks("wsock32", "ws2_32")
