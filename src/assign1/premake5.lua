project_root = "../.."
include(project_root.."/tools/build")
local qt = premake.extensions.qt

group("src")
project("CG-assign1")
  uuid("675CCAC8-16AE-4308-B9BC-6DF5ED2C4A67")
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
