local function ACL()
    add_headerfiles("External/acl/includes/acl/**.h")
end 

local function SPINE_RUNTIMES()
    add_includedirs("External/spine-runtimes/spine-cpp/spine-cpp/include")
    add_headerfiles("External/spine-runtimes/spine-cpp/spine-cpp/include/spine/*.h")
    add_files("External/spine-runtimes/spine-cpp/spine-cpp/src/spine/*.cpp")
end

local function TLSF()
    add_headerfiles("External/tlsf/**.h")
    add_files("External/tlsf/**.c")
end

function External()
    ACL()
    SPINE_RUNTIMES()
    TLSF()
    add_deps("imgui")
    add_deps("baselib")
    add_deps("vulkan")
end 

target("imgui")
    set_kind("static")
    add_files("imgui/backends/imgui_impl_dx12.cpp")
    add_files("imgui/backends/imgui_impl_win32.cpp")
    add_files("imgui/*.cpp")
    add_includedirs("imgui", {public = true}) --此处的头文件相关目录也会被继承
    add_headerfiles("imgui/*.h")
    add_headerfiles("imgui/backends/imgui_impl_dx12.h")
    add_headerfiles("imgui/backends/imgui_impl_win32.h")

target("baselib")
    set_kind("static")
    add_headerfiles("baselib/**.h")
    add_files("baselib/**.cpp")
    add_defines("BASELIB_INLINE_NAMESPACE=SourceClassic", {public = true})
    add_includedirs("baselib", {public = true})
    add_includedirs("baselib/Include", {public = true})
    add_syslinks("wsock32", "ws2_32")

target("glfw")
    set_kind("static")
    add_headerfiles("Vulkan-Hpp/glfw/include/**.h")
    add_includedirs("Vulkan-Hpp/glfw/include", {public = true})
    add_headerfiles("Vulkan-Hpp/glfw/deps/**.h")
    add_files("Vulkan-Hpp/glfw/deps/**.c")

target("glm")
    set_kind("static")
    add_headerfiles("Vulkan-Hpp/glm/glm/**.cppm")
    add_headerfiles("Vulkan-Hpp/glm/glm/**.inl")
    add_includedirs("Vulkan-Hpp/glm",{public = true})
    add_files("Vlukan-Hpp/glm/glm/**.hpp")

target("vulkan")
    set_kind("static")
    add_deps("glfw")
    add_deps("glm")