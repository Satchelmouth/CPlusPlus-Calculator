-- premake5.lua
workspace "Calculator"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Calculator"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "WalnutExternal.lua"
include "Calculator"