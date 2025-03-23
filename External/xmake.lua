local function ACL()
    add_headerfiles("External/acl/includes/acl/**.h")
end 

local function TLSF()
    add_headerfiles("External/tlsf/**.h")
    add_files("External/tlsf/**.c")
end

local function Baselib()
    add_headerfiles("External/baselib/**.h")
    add_files("External/baselib/**.cpp")
    add_defines("BASELIB_INLINE_NAMESPACE=SourceClassic")
    add_includedirs("./External/baselib")
    add_includedirs("./External/baselib/Include")
    add_syslinks("wsock32", "ws2_32")
end

local function IMGUI()
    add_includedirs("./External/imgui")
    add_headerfiles("External/imgui/*.h")
    add_files("External/imgui/*.cpp")

    add_headerfiles("External/imgui/backends/imgui_impl_dx12.h")
    add_headerfiles("External/imgui/backends/imgui_impl_win32.h")
    add_files("External/imgui/backends/imgui_impl_dx12.cpp")
    add_files("External/imgui/backends/imgui_impl_win32.cpp")
end

function External()
    ACL()
    TLSF()
    Baselib()
    IMGUI()
end 