#!/bin/bash

projectBaseDir=$(dirname -- "$( readlink -f -- "$0"; )") # There are so many (correct/incorrect) ways to do this :/
buildBaseDir="$(pwd)/cmake-build-all"
buildType="Debug"
buildTypeSuffix=""
emscriptenToolchainFile="${HOME}/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

exitCodeFail=1
exitCodeWrongArguments=1

function log() {
  echo "$1" >> /dev/stderr
}

function setBuildType() {
  buildType="$1"
  buildTypeSuffix=$(printf "%s" "${buildType}" | tr "[:upper:]" "[:lower:]")
}

setBuildType "${buildType}"

function help() {
  echo "build.sh [OPTION | COMMAND]*"
  echo ""
  echo "Commands:"
  echo ""
  echo "clean        Clean the build directories. Useful to start from scratch."
  echo "generate     Generate all the build files. Required at least once befor a 'compile'."
  echo "compile      Builds the binaries."
  echo ""
  echo ""
  echo "Options:"
  echo ""
  echo "--build-type <type>      Which build to create. Default: '${buildType}'"
  echo "--emscripten-tc <file>   Specify the CMake toolchain file for emscripten build. Default: '${emscriptenToolchainFile}'"
  echo ""
}

function clean() {
  log "Cleaning '${buildBaseDir}'"
  rm -rf "${buildBaseDir}"
}

function ensureBuildDir() {
  log "Creating '${buildBaseDir}'"
  mkdir --parents "${buildBaseDir}/$1"
}

function pushDir() {
  pushd "$1" > /dev/null || exit $exitCodeFail
}

function pushBuildDir() {
  pushDir "${buildBaseDir}/$1"
}

function popDir() {
  popd > /dev/null || exit $exitCodeFail
}

function generateBuildFiles() {
  log "Creating build files for build type '${buildType}'"

  ensureBuildDir "default-${buildTypeSuffix}"
  pushBuildDir "default-${buildTypeSuffix}"
  cmake "-DCMAKE_BUILD_TYPE=${buildType}" "${projectBaseDir}"
  popDir

  ensureBuildDir "win64-${buildTypeSuffix}"
  pushBuildDir "win64-${buildTypeSuffix}"
  cmake "-DCMAKE_BUILD_TYPE=${buildType}" "-DCMAKE_TOOLCHAIN_FILE=${projectBaseDir}/toolchain-mingw.cmake" "${projectBaseDir}"
  popDir

  ensureBuildDir "web-${buildTypeSuffix}"
  pushBuildDir "web-${buildTypeSuffix}"
  cmake "-DCMAKE_BUILD_TYPE=${buildType}" "-DCMAKE_TOOLCHAIN_FILE=${emscriptenToolchainFile}" "-DPLATFORM=Web" "${projectBaseDir}"
  popDir
}

function compile() {
  log "Compiling..."
  for dir in "${buildBaseDir}"/*/; do
    log "Compiling in '${dir}'..."
    pushDir "${dir}"
      make -j 6
    popDir
  done
}

function main {
  if [ $# -eq 0 ]; then
    help
    exit
  fi
  for arg in "$@"; do
      case "$arg" in
        "--help" | "-h" | "/?" | "help")
          help
          exit
        ;;
        "--build-type")
          shift
          setBuildType "$1"
        ;;
        "--emscripten-tc")
          shift
          emscriptenToolchainFile="$1"
        ;;
        "clean")
          clean
        ;;
        "generate")
          generateBuildFiles
        ;;
        "compile")
          compile
        ;;
        *)
          log "Unknown argument '$1'. Run script with '--help' for available arguments."
          exit $exitCodeWrongArguments
          ;;
      esac
      shift
  done
}

main "$@"
