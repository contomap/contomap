#!/bin/bash

projectBaseDir=$(dirname -- "$( readlink -f -- "$0"; )") # There are so many (correct/incorrect) ways to do this :/
buildBaseDir="$(pwd)/cmake-build-all"
buildType="Debug"
buildTypeSuffix=""
emscriptenToolchainFile="${HOME}/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

exitCodeFail=1
exitCodeWrongArguments=1
exitCodeBuildFailure=1
exitCodeTestFailure=1
exitCodeLintError=1

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
  echo "help         Prints this information. Exits script afterwards."
  echo "clean        Clean the build directories. Useful to start from scratch."
  echo "generate     Generate all the build files. Required at least once before a 'compile'."
  echo "compile      Builds the binaries."
  echo "test         Executes all tests (of default compilation)."
  echo "lint         Check the source for any errors. Recommended to be done in clean state."
  echo ""
  echo ""
  echo "Options:"
  echo ""
  echo "Options will affect only the arguments following them."
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
  log "Creating '${buildBaseDir}/$1'"
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

function execCMake() {
  local buildDir=$1
  shift
  ensureBuildDir "${buildDir}"
  pushBuildDir "${buildDir}"
  if ! cmake "-DCMAKE_BUILD_TYPE=${buildType}" "$@"; then
    popDir
    log "CMake failed, aborting"
    exit $exitCodeFail
  fi
  popDir
}

function generateBuildFiles() {
  log "Creating build files for build type '${buildType}'"

  execCMake "default-${buildTypeSuffix}" "${projectBaseDir}"
  execCMake "win64-${buildTypeSuffix}" "-DCMAKE_TOOLCHAIN_FILE=${projectBaseDir}/toolchain-mingw.cmake" "${projectBaseDir}"
  execCMake "web-${buildTypeSuffix}" "-DCMAKE_TOOLCHAIN_FILE=${emscriptenToolchainFile}" "-DPLATFORM=Web" "${projectBaseDir}"
}

function compile() {
  log "Compiling..."
  local total=0
  local failed=0
  for dir in "${buildBaseDir}"/*-"${buildTypeSuffix}"/; do
    if [ -d "${dir}" ]; then
      log "Compiling in '${dir}'..."
      total=$(( total + 1 ))
      pushDir "${dir}"
      if ! make -j 6; then
        failed=$(( failed + 1 ))
      fi
      popDir
    fi
  done
  if [ $failed != 0 ]; then
    log "${failed} build(s) failed, aborting"
    exit $exitCodeBuildFailure
  fi
  if [ $total == 0 ]; then
    log "Nothing found to compile, aborting"
    exit $exitCodeBuildFailure
  fi
}

function execTest() {
  log "Running test '$1'"
  if ! $1 --gtest_shuffle;
  then
    return 1
  fi
  return 0
}

function testDefault() {
  local total=0
  local failed=0
  for testFile in "${buildBaseDir}"/default-"${buildTypeSuffix}"/*-test* ; do
    if [ -f "${testFile}" ]; then
      total=$(( total + 1 ))
      if ! execTest "${testFile}"; then
        failed=$(( failed + 1 ))
      fi
    fi
  done
  if [ $failed != 0 ]; then
    log "${failed} test(s) failed, aborting"
    exit $exitCodeTestFailure
  fi
  if [ $total == 0 ]; then
    log "No tests found, aborting"
    exit $exitCodeTestFailure
  fi
}

function lintFormat() {
  local failed=0
  shopt -s globstar
  shopt -s nullglob
  for sourceFile in "${projectBaseDir}"/**/*.cpp "${projectBaseDir}"/**/*.h; do
    if [[ "${sourceFile}" =~ ^"${projectBaseDir}/vendor/" || "${sourceFile}" =~ ^"${projectBaseDir}/cmake-build-" ]]; then
      continue
    fi

    log "Checking '${sourceFile}'"
    if ! clang-format --dry-run --Werror "${sourceFile}"; then
      failed=$(( failed + 1 ))
    fi
  done
  if [ $failed != 0 ]; then
    log "Formatting errors, aborting. Please run clang-format on the affected files."
    exit $exitCodeLintError
  fi
}

function lint() {
  lintFormat
}

function main {
  if [ $# -eq 0 ]; then
    help
    exit
  fi
  while [ $# -gt 0 ]; do
    case "$1" in
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
      "test")
        testDefault
      ;;
      "lint")
        lint
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
