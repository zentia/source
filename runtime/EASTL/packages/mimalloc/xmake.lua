if not _config_project then
    function _config_project(config)
        local batch_size = config["batch_size"]
        if type(batch_size) == "number" and batch_size > 1 and (not _disable_unity_build) then
            add_rules("c.unity_build", {
                batchsize = batch_size
            })
            add_rules("c++.unity_build", {
                batchsize = batch_size
            })
        end
        if type(_config_rules) == "table" then
            add_rules(_config_rules, config)
        end
    end
end

target("mimalloc")
_config_project({
	project_kind = "object",
	no_rtti = true
})
on_load(function(target)
	local function rela(p)
		return path.relative(path.absolute(p, os.scriptdir()), os.projectdir())
	end
	target:add("includedirs", rela("include"), {
		public = true
	})
	target:add("defines", "MI_SHARED_LIB", {public = true})
	target:add("defines", "MI_XMALLOC=1", "MI_WIN_NOREDIRECT", "MI_SHARED_LIB_EXPORT")
	if is_plat("windows") then
		target:add("syslinks","advapi32", "bcrypt", {public = true})
		target:add("defines", "_CRT_SECURE_NO_WARNINGS")
	elseif is_plat("linux") then
		target:add("syslinks","pthread", "atomic", {public = true})
	else
		target:add("syslinks","pthread", {public = true})
	end
end)
add_headerfiles("include/*.h")
add_files("src/build.mimalloc.cpp")
target_end()
