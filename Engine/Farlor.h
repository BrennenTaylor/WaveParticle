#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstdint>

#include <assert.h>
#include <math.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>

#define SAFE_RELEASE( x ) {if(x) {(x)->Release(); (x) = nullptr;}}
#define SAFE_DELETE( x ) {if(x) {delete (x); (x) = nullptr; }}
#define SAFE_DELETE_ARRAY( x ) {if(x) {delete[] (x); (x) = nullptr; }}

#define FARLOR_PI 3.14159265f
#define DEGREE_TO_RAD( x ) {(x) * FARLOR_PI / 180.0f}


#define ASSERT(x) { assert(x); }

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uptr;

// Must undefine this due to microsoft bull shit!!
// These are defined from the 80s, fuck you microsoft.
// http://lolengine.net/blog/2011/3/4/fuck-you-microsoft-near-far-macros
#undef near
#undef far
