function main(target)
    import("detect.sdks.find_cuda")
    import("cuda_sdkdir")
    local cuda = find_cuda(cuda_sdkdir())
    print("find cuda")
    if cuda then
        
        local function set(key, value)
            if type(vlaue) == "string" then
                target:add(key, value, {
                    public = true
                })
            elseif type(value) == "table" then
                for i, v in ipairs(value) do 
                    target:add(key, v, {
                        public = true 
                    })
                end
            end
        end
        set("linkdirs", cuda["linkdirs"])
        set("includedirs", cuda["includedirs"])
    else 
        target:set("enabled", false)
    end
    if is_plat("windows") then
        target:add("defines", "UNICODE", "_CRT_SECURE_NO_WARNINGS", {
            public = true
        })
        target:add("syslinks", "Cfgmgr32", "Advapi32", {
            public = true
        })
    end
    if get_config("lc_backend_lto") then 
        target:set("policy", "build.optimization.lto", true)
        if get_config("lc_toolchain") == "llvm" then 
            target:add("ldflags", "-fuse-ld=lld-link")
            target:add("shflags", "-fuse-ld=lld-link")
        end 
    end 
end
