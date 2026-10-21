workspace("Sh-eL")
architecture("x64")
configurations({ "Debug", "Release" })
startproject("Tests")

language("C++")
cppdialect("C++20")
targetdir("bin/%{cfg.buildcfg}")
objdir("obj/%{cfg.buildcfg}/%{prj.name}")

filter("configurations:Debug")
defines({ "DEBUG" })
runtime("Debug")
symbols("On")
filter("configurations:Release")
defines({ "NDEBUG" })
runtime("Release")
optimize("On")
filter({})

project("Sh-eL")
kind("StaticLib")
files({
    "src/**.cpp",
    "include/**.hpp",
})
includedirs({
    "include",
    "src",
})

project("Tests")
kind("ConsoleApp")
files({ "tests/**.cpp" })
includedirs({ "include" })
links({ "Sh-eL" })
