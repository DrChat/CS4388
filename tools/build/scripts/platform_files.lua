include("build_paths.lua")
include("util.lua")

local function match_platform_files(base_path, base_match)
  files({
    base_path.."/"..base_match..".h",
    base_path.."/"..base_match..".c",
    base_path.."/"..base_match..".cc",
    base_path.."/"..base_match..".cpp",
    base_path.."/"..base_match..".inc",
  })
  removefiles({base_path.."/".."**_main.cc"})
  removefiles({base_path.."/".."**_test.cc"})
  removefiles({base_path.."/".."**_posix.h", base_path.."/".."**_posix.cc"})
  removefiles({base_path.."/".."**_linux.h", base_path.."/".."**_linux.cc"})
  removefiles({base_path.."/".."**_mac.h", base_path.."/".."**_mac.cc"})
  removefiles({base_path.."/".."**_win.h", base_path.."/".."**_win.cc"})
  filter("platforms:Windows")
    files({
      base_path.."/"..base_match.."_win.h",
      base_path.."/"..base_match.."_win.cc",
    })
  filter("platforms:Linux")
    files({
      base_path.."/"..base_match.."_posix.h",
      base_path.."/"..base_match.."_posix.cc",
      base_path.."/"..base_match.."_linux.h",
      base_path.."/"..base_match.."_linux.cc",
    })
  filter({})
end

-- Adds all .h and .cc files in the current path that match the current platform
-- suffix (_win, etc).
function local_platform_files(base_path)
  match_platform_files(base_path or ".", "*")
end

-- Adds all .h and .cc files in the current path and all subpaths that match
-- the current platform suffix (_win, etc).
function recursive_platform_files(base_path)
  match_platform_files(base_path or ".", "**")
end
