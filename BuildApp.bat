@echo off

msbuild build/ALL_BUILD.vcxproj /m:6 /nologo /p:Configuration=Debug
