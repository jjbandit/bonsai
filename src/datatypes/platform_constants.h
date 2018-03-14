#ifndef BONSAI_PLAT_CONSTANTS_H
#define BONSAI_PLAT_CONSTANTS_H

#include <stdio.h>

/*
 *  Common
 */

#define True  1
#define False 0

// Screen Resolution

#define SCREEN_RATIO 2

#define SCR_WIDTH (3840/SCREEN_RATIO)
#define SCR_HEIGHT (2160/SCREEN_RATIO)


#define TargetFrameTime 0.0166666666f

/*
 *  Paths
 */
#define MODELS_PATH "models"
#define SHADER_PATH "shaders"
#define SHADER_HEADER "/header.glsl"


// Wrapper so assertions give us file/line numbers
#define AssertNoGlErrors {            \
  int glErrorNo = glGetError();       \
  DumpGlErrorEnum(glErrorNo);         \
  Assert(glErrorNo == GL_NO_ERROR); }

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))
#define InvalidDefaultCase default: {Assert(!"Invalid Default Case");} break

#define LOCAL_PERSIST static


/*
 *  MSVC
 */
#ifdef _MSC_VER

#define __FUNCTION_NAME__ __FUNCTION__

#define global_variable static
#define debug_global static

#define VariadicOutputDebugString(FormatString, ...) {    \
    char Buffer[1024] = {};                               \
    _snprintf_s(Buffer, 1023, FormatString, __VA_ARGS__); \
    PrintConsole(Buffer); }


#define Debug(...) VariadicOutputDebugString(__VA_ARGS__); \
                   PrintConsole("\n")

#define Info(...)  PrintConsole("   Info - ");             \
                   VariadicOutputDebugString(__VA_ARGS__); \
                   PrintConsole("\n")

#define Error(...) PrintConsole(" ! Error - ");            \
                   VariadicOutputDebugString(__VA_ARGS__);   \
                   PrintConsole("\n")

#define Warn(...) PrintConsole(" * Warn - ");            \
                  VariadicOutputDebugString(__VA_ARGS__); \
                  PrintConsole("\n")

#define RuntimeBreak() __debugbreak()

// Disable warnings about insecure CRT functions
#pragma warning(disable : 4996)


/*
 *  GCC
 */
#elif LINUX

#define __FUNCTION_NAME__ __func__

// Hush up gcc about unreferenced globals
#define global_variable static __attribute__((unused))
#define debug_global static __attribute__((unused))

#define Debug(...) printf(__VA_ARGS__);   printf("\n")

#define Info(...)  printf("   Info - ");  printf(__VA_ARGS__); printf("\n")
#define Error(...) printf(" ! Error - "); printf(__VA_ARGS__); printf("\n")
#define Warn(...)  printf(" * Warn - "); printf(__VA_ARGS__); printf("\n")


#define RuntimeBreak() raise(SIGTRAP)

#elif WASM

#define __FUNCTION_NAME__ __func__

// Hush up gcc about unreferenced globals
#define global_variable static __attribute__((unused))
#define debug_global static __attribute__((unused))


#define RuntimeBreak()

#define Debug(...) printf(__VA_ARGS__);   printf("\n")

#define Info(...)  printf("   Info - ");  printf(__VA_ARGS__); printf("\n")
#define Error(...) printf(" ! Error - "); printf(__VA_ARGS__); printf("\n")
#define Warn(...)  printf(" * Warn - "); printf(__VA_ARGS__); printf("\n")

#endif

#endif
