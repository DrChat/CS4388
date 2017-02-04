project_root = "../.."
include(project_root.."/tools/build")
local qt = premake.extensions.qt

group("src")
project("CG-ui")
  uuid("CEC7E24B-004E-4C08-B890-6F4993731EEB")
  kind("StaticLib")
  language("C++")
  qt.enable()
  qtpath "C:\\Qt\\5.7\\msvc2015_64"
  qtmodules {"core", "gui", "widgets", "opengl"}
  qtprefix "Qt5"

  configuration {"Debug"}
    qtsuffix "d"
  configuration {}

  links({
  })
  defines({
  })
  includedirs({
    project_root.."/third_party/gflags/src",
  })
  files({"*.h", "*.cc"})
