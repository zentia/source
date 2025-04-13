
table.insert(global_includedirs, "runtime/spdlog")
table.insert(global_headerfiles, "runtime/spdlog/**.h")
table.insert(global_files, "runtime/spdlog/*.cpp")
table.insert(global_defines, "SPDLOG_COMPILED_LIB=")
table.insert(global_defines, "FMT_USE_CONSTEVAL=0")