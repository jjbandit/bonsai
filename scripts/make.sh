#! /bin/bash

# COMMON_OPTIMIZATION_OPTIONS="-O2"

RED="\x1b[31m"
BLUE="\x1b[34m"
GREEN="\x1b[32m"
YELLOW="\x1b[33m"
WHITE="\x1b[37m"

Delimeter="$RED-----------------------------------------------------------$WHITE"
Success="$GREEN  ✔ $WHITE"
Building="$BLUE  Building $WHITE"
Failed="$RED  ✗ $WHITE"

ROOT="."
SRC="$ROOT/src"
EXAMPLES="$ROOT/examples"
TESTS="$SRC/tests"
BIN="$ROOT/bin"
BIN_TEST="$BIN/tests"
META_OUT="$SRC/metaprogramming/output"

# PREPROCESSOR_EXECUTABLE="bin/preprocessor_dev"
# PREPROCESSOR_EXECUTABLE="bin/preprocessor_current"

function SetOutputBinaryPathBasename()
{
  base_file="${1##*/}"
  output_basename="$2/${base_file%%.*}"
}

function ColorizeTitle()
{
  echo -e "$YELLOW$1$WHITE"
  echo -e ""
}

INCLUDE_DIRECTORIES="$SRC"
OUTPUT_DIRECTORY="$BIN"

# NOTE(Jesse): -Wno-global-constructors can be turned off when the defaultPallette
# in colors.h gets axed .. I think.

# TODO(Jesse, tags: build_pipeline): Investigate -Wcast-align situation

  # -fsanitize=address

# Note(Jesse): Using -std=c++17 so I can mark functions with [[nodiscard]]

COMMON_COMPILER_OPTIONS="
  -I/usr/src/linux-headers-4.15.0-88/include/

  -std=c++17
  -ferror-limit=2000
  -ggdb
  -Weverything
  -Wno-c++98-compat-pedantic
  -Wno-gnu-anonymous-struct
  -Wno-missing-prototypes
  -Wno-zero-as-null-pointer-constant
  -Wno-format-nonliteral
  -Wno-cast-qual
  -Wno-unused-function
  -Wno-four-char-constants
  -Wno-old-style-cast
  -Wno-float-equal
  -Wno-double-promotion
  -Wno-padded
  -Wno-global-constructors
  -Wno-cast-align
  -Wno-switch-enum
  -Wno-undef
  -Wno-covered-switch-default
  -Wno-c99-extensions
  -Wno-reserved-id-macro
  -Wno-dollar-in-identifier-extension

  -Wno-unused-value
  -Wno-unused-variable
  -Wno-unused-parameter
"

COMMON_LINKER_OPTIONS="-lpthread -lX11 -ldl -lGL"
SHARED_LIBRARY_FLAGS="-shared -fPIC"

EXAMPLES_TO_BUILD="
  $EXAMPLES/world_gen
"

  # $EXAMPLES/animation_test
  # $EXAMPLES/ssao_test
  # $EXAMPLES/space_invaders

EXECUTABLES_TO_BUILD="
  $SRC/platform.cpp
  $SRC/font/ttf.cpp
  $SRC/net/server.cpp
"

# TODO(Jesse, tags: tests, release): The allocation tests crash in release mode because of some
# ultra-jank-tastic segfault recovery code.  Find another less janky way?
DEBUG_TESTS_TO_BUILD="
  $TESTS/allocation.cpp
"

TESTS_TO_BUILD="
  $TESTS/ui_command_buffer.cpp
  $TESTS/m4.cpp
  $TESTS/colladaloader.cpp
  $TESTS/test_bitmap.cpp
  $TESTS/chunk.cpp
  $TESTS/bonsai_string.cpp
  $TESTS/objloader.cpp
  $TESTS/callgraph.cpp
  $TESTS/heap_allocation.cpp
  $TESTS/preprocessor.cpp
  $TESTS/rng.cpp
  $TESTS/file.cpp
"

function BuildPreprocessor {

  which clang++ > /dev/null
  [ $? -ne 0 ] && echo -e "Please install clang++" && exit 1

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""

  ColorizeTitle "Building Preprocessor"
  executable="$SRC/metaprogramming/preprocessor.cpp"
  SetOutputBinaryPathBasename "$executable" "$BIN"
  echo -e "$Building $executable"
  clang++                \
    $COMMON_OPTIMIZATION_OPTIONS \
    $COMMON_COMPILER_OPTIONS     \
    $COMMON_LINKER_OPTIONS       \
    -D BONSAI_INTERNAL=1         \
    -I"$SRC"                     \
    -o "$output_basename""_dev"  \
    $executable

  if [ $? -eq 0 ]; then
   echo -e "$Success $executable"
  else
   echo ""
   echo -e "$Failed Error building preprocessor, exiting."
   exit 1
  fi

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
}

function BuildWithClang {
  which clang++ > /dev/null
  [ $? -ne 0 ] && echo -e "Please install clang++" && exit 1

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""

  ColorizeTitle "Executables"
  for executable in $EXECUTABLES_TO_BUILD; do
    SetOutputBinaryPathBasename "$executable" "$BIN"
    echo -e "$Building $executable"
    clang++                        \
      $COMMON_OPTIMIZATION_OPTIONS \
      $COMMON_COMPILER_OPTIONS     \
      $COMMON_LINKER_OPTIONS       \
      -D BONSAI_INTERNAL=1         \
      -I"$SRC"                     \
      -o "$output_basename"        \
      $executable && echo -e "$Success $executable" &
  done

  echo ""
  ColorizeTitle "Debug Tests"
  for executable in $DEBUG_TESTS_TO_BUILD; do
    SetOutputBinaryPathBasename "$executable" "$BIN_TEST"
    echo -e "$Building $executable"
    clang++                      \
      $COMMON_COMPILER_OPTIONS   \
      $COMMON_LINKER_OPTIONS     \
      -D BONSAI_INTERNAL=1       \
      -I"$SRC"                   \
      -o "$output_basename"      \
      $executable && echo -e "$Success $executable" &
  done

  echo ""
  ColorizeTitle "Tests"
  for executable in $TESTS_TO_BUILD; do
    SetOutputBinaryPathBasename "$executable" "$BIN_TEST"
    echo -e "$Building $executable"
    clang++                        \
      $COMMON_OPTIMIZATION_OPTIONS \
      $COMMON_COMPILER_OPTIONS     \
      $COMMON_LINKER_OPTIONS       \
      -D BONSAI_INTERNAL=1         \
      -I"$SRC"                     \
      -I"$SRC/debug_system"        \
      -o "$output_basename"        \
      $executable && echo -e "$Success $executable" &
  done

  echo ""
  ColorizeTitle "DebugSystem"
  DEBUG_SRC_FILE="$SRC/debug_system/debug.cpp"
  echo -e "$Building $DEBUG_SRC_FILE"
  clang++                          \
    $COMMON_OPTIMIZATION_OPTIONS   \
    $COMMON_COMPILER_OPTIONS       \
    $SHARED_LIBRARY_FLAGS          \
    $COMMON_LINKER_OPTIONS         \
    -D BONSAI_INTERNAL=1           \
    -I"$SRC"                       \
    -I"$SRC/debug_system"          \
    -o "$BIN/lib_debug_system.so"  \
    "$DEBUG_SRC_FILE" && echo -e "$Success $DEBUG_SRC_FILE" &

  echo ""
  ColorizeTitle "Examples"
  for executable in $EXAMPLES_TO_BUILD; do
    echo -e "$Building $executable"
    SetOutputBinaryPathBasename "$executable" "$BIN"
    clang++                                                     \
      $SHARED_LIBRARY_FLAGS                                     \
      $COMMON_OPTIMIZATION_OPTIONS                              \
      $COMMON_COMPILER_OPTIONS                                  \
      $COMMON_LINKER_OPTIONS                                    \
      -D BONSAI_INTERNAL=1                                      \
      -I"$SRC"                                                  \
      -I"$executable"                                           \
      -o "$output_basename"                                     \
      "$executable/game.cpp" &&                                 \
      mv "$output_basename" "$output_basename""_loadable.so" && \
      echo -e "$Success $executable" &
  done

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""
  ColorizeTitle "Complete"

  wait

  echo -e ""
  echo -e "$Delimeter"
  echo -e ""

  echo -e ""
}

function BuildWithEmcc {
  which emcc > /dev/null
  [ $? -ne 0 ] && echo -e "Please install emcc" && exit 1

  emcc src/font/ttf.cpp              \
    -D BONSAI_INTERNAL=1             \
    -I src                           \
    -I /usr/include                  \
    -I /usr/include/x86_64-linux-gnu \
    -o bin/emscripten/ttf.html
}


if [ ! -d "$BIN" ]; then
  mkdir "$BIN"
fi

if [ ! -d "$BIN_TEST" ]; then
  mkdir "$BIN_TEST"
fi

if [ ! -d "$BIN_TEST" ]; then
  mkdir "$BIN_TEST"
fi

# SOURCE_FILES="src/metaprogramming/preprocessor.h"

function SetSourceFiles
{
  rm -Rf $META_OUT
  mkdir $META_OUT
  SOURCE_FILES="                                                   \
    src/bonsai_stdlib/headers/primitives.h                         \
    $(find src -type f -name "*.h"                                 \
    -and -not -wholename "src/net/network.h"                       \
    -and -not -wholename "src/bonsai_stdlib/headers/stream.h"      \
    -and -not -wholename "src/bonsai_stdlib/headers/perlin.h"      \
    -and -not -wholename "src/bonsai_stdlib/headers/primitives.h"  \
    -and -not -wholename "src/metaprogramming/defines.h"           \
    -and -not -wholename "src/win32_platform.h"                    \
    -and -not -path      "src/tests/*" )                           \
                                                                   \
    $(find src -type f -name "*.cpp"                               \
    -and -not -wholename "src/bonsai_stdlib/cpp/stream.cpp"        \
    -and -not -wholename "src/net/server.cpp"                      \
    -and -not -wholename "src/win32_platform.cpp"                  \
    -and -not -path "src/tests/*" )                                \
  "
}

function RunPreprocessor
{
  SetSourceFiles
  ColorizeTitle "Preprocessing"
  $PREPROCESSOR_EXECUTABLE $SOURCE_FILES
  if [ $? -ne 0 ]; then
    echo ""
    echo -e "$Failed Preprocessing failed, exiting." 
    git checkout "src/metaprogramming/output"
    exit 1
  fi
}

function RunEntireBuild {

  if [ $DumpSourceFilesAndQuit == 1 ]; then
    SetSourceFiles
    echo "gdb --args bin/preprocessor_dev" $SOURCE_FILES
    exit 1
  fi

  if [ $CheckoutMetaOutput == 1 ]; then
    git checkout "src/metaprogramming/output"
  fi

  if [ $FirstPreprocessor == 1 ]; then
    PREPROCESSOR_EXECUTABLE="bin/preprocessor_current"
    RunPreprocessor
  fi

  if [ $BuildPreprocessor == 1 ]; then
    BuildPreprocessor
    [ ! -x $PREPROCESSOR_EXECUTABLE ] && echo -e "$Failed Couldn't find preprocessor, exiting." && exit 1
  fi

  if [ $SecondPreprocessor == 1 ]; then
    PREPROCESSOR_EXECUTABLE="bin/preprocessor_dev"
    RunPreprocessor
  fi

  if [ $BuildAllProjects == 1 ]; then
    if [ "$EMCC" == "1" ]; then
      BuildWithEmcc
    else
      BuildWithClang
    fi
  fi

  if [ $RunTests == 1 ]; then
    ./scripts/run_tests.sh
  fi

  if [ $FinalPreprocessor == 1 ]; then
    RunPreprocessor
  fi

}

DumpSourceFilesAndQuit=0

CheckoutMetaOutput=0

FirstPreprocessor=1
BuildPreprocessor=1
SecondPreprocessor=0

BuildAllProjects=0
RunTests=0
FinalPreprocessor=0

time RunEntireBuild
