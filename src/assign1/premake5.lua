project_root = "../.."
include(project_root.."/tools/build")

group("src")
project("CG-assign1")
  uuid("675CCAC8-16AE-4308-B9BC-6DF5ED2C4A67")
  kind("StaticLib")
  language("C++")
  links({
  })
  defines({
  })
  includedirs({
    project_root.."/third_party/gflags/src",
  })
  files({"*.h", "*.cc"})
