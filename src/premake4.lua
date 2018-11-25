solution "cppservice"
location ("../build")

configurations {'Debug','Release'}

project "cppservice"
kind "ConsoleApp"
language "C++"
files {"cpp/*.H",{"cpp/**.cpp"}, {"cpp/**.C"}}
buildoptions { "-std=c++14", "-Wall" }
links("pthread")

configuration "Debug"
defines { "DEBUG" }
flags { "Symbols" }
targetdir "../install"

configuration "Release"
defines { "NDEBUG" }
flags { "Optimize" }
targetdir "../install"


project "service-gtest"
kind "ConsoleApp"
language "C++"
files {"cpp/*.H",{"cpp/**.cpp"}, {"cpp/**.C"}, {"test/**.C"},{"test/**.H"}}
includedirs {"cpp"}
excludes {"cpp/main.cpp"}
buildoptions { "-std=c++14", "-Wall" }
links("pthread")
links("gtest")


configuration "Debug"
defines { "DEBUG" }
flags { "Symbols" }
targetdir "../install"
