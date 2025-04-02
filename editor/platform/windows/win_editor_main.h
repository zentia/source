#pragma once

#if SOURCE_EDITOR
#define SOURCE_API __declspec(dllexport)
#else
#define SOURCE_API __declspec(dllimport)
#endif

SOURCE_API int source_main(int argc, char** argv);