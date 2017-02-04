project_root = "../.."
include(project_root.."/tools/build")

group("src")
project("CG-core")
  uuid("8BB51FBC-AA3C-4822-9F3B-B24A1F81195A")
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
