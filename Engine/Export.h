#pragma once

#ifdef GAME_SHARED

#define GAME_EXPORT extern "C" __declspec(dllexport)

#else

#define GAME_EXPORT 

#endif