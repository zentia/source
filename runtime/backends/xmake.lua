includes("cuda/xmake.lua")
includes("dx/xmake.lua")
table.insert(global_headerfiles, "runtime/backends/**.h")
table.insert(global_headerfiles, "runtime/backends/**.hpp")