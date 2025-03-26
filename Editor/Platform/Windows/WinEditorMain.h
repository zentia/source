#pragma once

#if SOURCE_EDITOR
#define SOURCE_API __declspec(dllexport)
#else
#define SOURCE_API __declspec(dllimport)
#endif

extern "C" SOURCE_API int SourceMain(int argc, char** argv);