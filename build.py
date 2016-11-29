import subprocess, sys, argparse, glob, os
import distutils.dir_util
from shutil import copyfile

if sys.platform.startswith('win'):
    shell = True
else:
    shell = False

parser = argparse.ArgumentParser()
configType = parser.add_mutually_exclusive_group(required=True)
configType.add_argument('--debug', help='Build in Debug Mode', action='store_true')
configType.add_argument('--release', help='Build in Release Mode', action='store_true')
parser.add_argument('--force', help='Force cmake', action='store_true')
args = parser.parse_args();

buildConfig = ""

if args.debug:
    buildConfig="Debug"

if args.release:
    buildConfig="Release"

startingDir = os.getcwd();

print ("Starting dir = ", startingDir)

needsRebuilt = False

if not os.path.exists("build"):
    needsRebuilt = True;
    os.makedirs("build")

if args.force:
    needsRebuilt = True;

if needsRebuilt:
    cmakeCmd = ["cmake.exe", "-G", "Visual Studio 14 Win64", startingDir]
    cmakeProcess = subprocess.call(cmakeCmd, shell=shell, cwd="build")

msbuildCmd = ["msbuild",  "ALL_BUILD.vcxproj", "/m:6",  "/nologo", "/p:Configuration=%s" % buildConfig]

buildProcess = subprocess.call(msbuildCmd, shell=shell, cwd="build")

# Deploy game to C:/Deploy
buildConfigDir = buildConfig + "/";
binaryDir = "bin/" + buildConfigDir;
libDir = binaryDir;

if not os.path.exists("C:/Deploy/Farlor/" + buildConfigDir):
    os.makedirs("C:/Deploy/Farlor/" + buildConfigDir)

copyfile(binaryDir + "Engine.exe", "C:/Deploy/Farlor/" + buildConfigDir + "Engine.exe")
copyfile(libDir + "Game.dll", "C:/Deploy/Farlor/" + buildConfigDir + "Game.dll")
distutils.dir_util.copy_tree("resources/", "C:/Deploy/Farlor/" + buildConfigDir + "resources/");



exit()
