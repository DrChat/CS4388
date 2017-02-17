project_root = "../.."
include(project_root.."/tools/build")
local qt = premake.extensions.qt

group("src")
project("CG-assign2")
  uuid("E9FF5039-8979-4DAD-8888-CBCBE6C0F74E")
  kind("WindowedApp")
  language("C++")
  qt.enable()

  links({
    "cg-core",
    "cg-ui",
  })
  defines({
  })
  includedirs({
    project_root.."/third_party/gflags/src",
  })
  files({"*.h", "*.cc"})
