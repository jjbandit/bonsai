#ifndef BONSAI_PLAT_CONSTANTS_H
#define BONSAI_PLAT_CONSTANTS_H

#include <stdio.h>

/*
 *  Common
 */

#define True  1
#define False 0

// Screen Resolution

#define SCREEN_RATIO 4

#define SCR_WIDTH (3840/SCREEN_RATIO)
#define SCR_HEIGHT (2160/SCREEN_RATIO)


#define PLATFORM_STORAGE_SIZE Megabytes(256)
#define TargetFrameTime 0.0166666666f

/*
 *  Paths
 */
#define MODELS_PATH "models"
#define SHADER_PATH "shaders"


// Wrapper so assertions give us file/line numbers
#define AssertNoGlErrors {            \
  int glErrorNo = glGetError();       \
  DumpGlErrorEnum(glErrorNo);         \
  Assert(glErrorNo == GL_NO_ERROR); }

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))
#define InvalidDefaultCase default: {Assert(false);} break

#define LOCAL_PERSIST static

#define DEBUG_RECORD_INPUT_SIZE       3600


/*
 *  MSVC
 */
#ifdef _MSC_VER

#define __FUNCTION_NAME__ __FUNCTION__

#define GLOBAL_VARIABLE static
#define DEBUG_GLOBAL static

#define VariadicOutputDebugString(FormatString, ...) {    \
    char Buffer[1024] = {};                               \
    _snprintf_s(Buffer, 1023, FormatString, __VA_ARGS__); \
    PrintConsole(Buffer); }


#define Info(...)  PrintConsole(" - Info - ");             \
                   VariadicOutputDebugString(__VA_ARGS__); \
                   PrintConsole("\n")

#define Debug(...) VariadicOutputDebugString(__VA_ARGS__); \
                   PrintConsole("\n")

#define Error(...) PrintConsole(" ! Error - ");            \
                   VariadicOutputDebugString(__VA_ARGS__); \
                   PrintConsole("\n")

#define RuntimeBreak() __debugbreak()

// Disable warnings about insecure CRT functions
#pragma warning(disable : 4996)


/*
 *  GCC
 */
#else

#define __FUNCTION_NAME__ __func__

// Hush up gcc about unreferenced globals
#define GLOBAL_VARIABLE static __attribute__((unused))
#define DEBUG_GLOBAL static __attribute__((unused))

#define Info(...)  printf(" - Info - ");  printf(__VA_ARGS__); printf("\n")
#define Debug(...) printf(__VA_ARGS__);   printf("\n")
#define Error(...) printf(" ! Error - "); printf(__VA_ARGS__); printf("\n")


#define RuntimeBreak() raise(SIGTRAP)


#endif // _MSC_VER

#endif // BONSAI_PLAT_CONTANTS_H
